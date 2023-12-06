// -*- C++ -*-

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modified
 * versions of this software, you first contact the authors at
 * oof_manager@nist.gov. 
 */

#include "oofcanvas/canvas.h"
#include "oofcanvas/canvasexception.h"
#include "oofcanvas/canvasimpl.h"
#include "oofcanvas/canvasitem.h"
#include "oofcanvas/canvasitemimpl.h"
#include "oofcanvas/canvaslayer.h"

#include <algorithm>
#include <cairomm/context.h>
#include <cassert>
#include <fstream>
#include <iostream>
#include <limits>
#include <math.h>

namespace OOFCanvas {

  // OSCanvasImpl is the implementation of the OffScreenCanvas.

  OSCanvasImpl::OSCanvasImpl(double ppu)
    : backingLayer(this, "<backinglayer>"),
      transform(Cairo::identity_matrix()),
      ppu(ppu),
      bgColor(1.0, 1.0, 1.0),
      margin(0.0),
      antialiasing(Cairo::ANTIALIAS_DEFAULT),
      initialized(false)
  {
    assert(ppu > 0.0);
    backingLayer.setClickable(false);
    setTransform(ppu);
  }

  OSCanvasImpl::~OSCanvasImpl() {
    for(CanvasLayerImpl *layer : layers)
      delete layer;
    layers.clear();
  }

  CanvasLayer *OSCanvasImpl::newLayer(const std::string &name) {
    // The OSCanvasImpl owns the CanvasLayers and is responsible
    // for deleting them.  Even if the layers are returned to Python,
    // Python does not take ownership.
#ifdef DEBUG
    for(CanvasLayerImpl *oldlayer: layers) {
      if(oldlayer->name == name) {
	std::cerr << "OOFCanvas warning: layer name is not unique:"
		  << name << std::endl;
      }
    }
#endif // DEBUG
    CanvasLayerImpl *layer = new CanvasLayerImpl(this, name);
    layers.push_back(layer);
    return layer;
  }

  void OSCanvasImpl::deleteLayer(CanvasLayer *layer) {
    CanvasLayerImpl *lyr = dynamic_cast<CanvasLayerImpl*>(layer);
    auto iter = std::find(layers.begin(), layers.end(), lyr);
    if(iter != layers.end())
      layers.erase(iter);
    delete layer;
  }

  void OSCanvasImpl::clear() {
    for(CanvasLayerImpl *layer : layers)
      delete layer;
    layers.clear();
    draw();
  }

  bool OSCanvasImpl::empty() const {
    for(const CanvasLayerImpl* layer : layers)
      if(!layer->empty())
	return false;
    return true;
  }

  std::size_t OSCanvasImpl::layerNumber(const CanvasLayer *layer)
    const
  {
    const CanvasLayerImpl *lyr = dynamic_cast<const CanvasLayerImpl*>(layer);
    for(std::size_t i=0; i<layers.size(); i++)
      if(layers[i] == lyr)
	return i;
    throw "Layer number out of range."; 
  }

  CanvasLayer *OSCanvasImpl::getLayer(const std::string &nm) const {
    for(CanvasLayerImpl *layer : layers)
      if(layer->name == nm)
	return layer;
    throw "Layer not found.";
  }

  void OSCanvasImpl::raiseLayer(int which, int howfar) {
    assert(howfar >= 0);
    assert(which >= 0 && which < layers.size());
    CanvasLayerImpl *moved = layers[which];
    int maxlayer = which + howfar; // highest layer that will be moved
    if(maxlayer >= layers.size())
      maxlayer = layers.size() - 1;
    for(int i=which; i < maxlayer; i++)
      layers[i] = layers[i+1];
    layers[maxlayer] = moved;
    draw();
  }
  
  void OSCanvasImpl::lowerLayer(int which, int howfar) {
    assert(howfar >= 0);
    assert(which >= 0 && which < layers.size());
    CanvasLayerImpl *moved = layers[which];
    int minlayer = which - howfar; // lowest layer that will be moved
    if(minlayer < 0)
      minlayer = 0;
    for(int i=which; i > minlayer; i--)
      layers[i] = layers[i-1];
    layers[minlayer] = moved;
    draw();
  }

  void OSCanvasImpl::raiseLayerToTop(int which) {
    CanvasLayerImpl *moved = layers[which];
    for(int i=which; i<layers.size()-1; i++)
      layers[i] = layers[i+1];
    layers[layers.size()-1] = moved;
    draw();
  }

  void OSCanvasImpl::lowerLayerToBottom(int which) {
    CanvasLayerImpl *moved = layers[which];
    for(int i=which; i>0; i--) 
      layers[i] = layers[i-1];
    layers[0] = moved;
    draw();
  }

  void OSCanvasImpl::reorderLayers(const std::vector<CanvasLayer*> *neworder)
  {
    // reorderLayers should be called with a list of layers that is
    // the same as the existing list, but in a different order.

    // This copy is sort of silly, because CanvasLayer* and
    // CanvasLayerImpl* probably are bitwise identical, and "layers =
    // *neworder" ought to be sufficient.  But they're different types
    // so it doesn't work.  This operation won't be done often so it's
    // probably ok to be suboptimal.
    layers.clear();
    for(auto layr : *neworder) 
      layers.push_back(dynamic_cast<CanvasLayerImpl*>(layr));
  }
  
  std::size_t OSCanvasImpl::nVisibleItems() const {
    std::size_t n = 0;
    for(CanvasLayerImpl *layer : layers)
      if(layer->visible) {
	n += layer->size();
      }
    return n;
  }
  
  void OSCanvasImpl::setBackgroundColor(const Color &color) {
    bgColor = color;
    bgColor.alpha = 1.0;
  }

  void OSCanvasImpl::drawBackground(Cairo::RefPtr<Cairo::Context> ctxt) const
  {
    ctxt->save();
    // One might think that one should call ctxt->reset_clip() here,
    // to ensure that the background is painted over the entire
    // canvas.  But doing that will paint the background over the
    // entire window (ie, outside the canvas widget!).  Just be sure
    // to call drawBackground before any other drawing operations
    // reset the clip region.
    ctxt->set_source_rgb(bgColor.red, bgColor.green, bgColor.blue);
    ctxt->paint();
    ctxt->restore();
  }

  void OSCanvasImpl::setAntialias(bool aa) {
    if(aa && antialiasing != Cairo::ANTIALIAS_DEFAULT) {
      antialiasing = Cairo::ANTIALIAS_DEFAULT;
    }
    else if(!aa && antialiasing != Cairo::ANTIALIAS_NONE) {
      antialiasing = Cairo::ANTIALIAS_NONE;
    }
    else
      return;
    for(CanvasLayerImpl *layer : layers) {
      layer->rebuild();
    }
    draw();
  }

  void OSCanvasImpl::setMargin(double m) {
    margin = m;
  }

  //=\\=//

  Rectangle OSCanvasImpl::findBoundingBox(double ppu) const {
    Rectangle bb;
    for(const CanvasLayerImpl *layer : layers)
      if(!layer->empty())
	bb.swallow(layer->findBoundingBox(ppu));
    return bb;
  }

  //=\\=//

  // OSCanvasImpl::transform is a Cairo::Matrix that converts from user
  // coordinates to device coordinates in the CanvasLayerImpls'
  // Cairo::Contexts. It is *not* the transform that maps the
  // CanvasLayerImpls to the gtk Layout, nor does it have anything to do
  // with scrolling.        

  // findTransform() computes the transform without setting or using
  // any state data from the Canvas.  setTransform() uses
  // findTransform() and state data to set OSCanvasImpl::transform.

  Cairo::Matrix OSCanvasImpl::findTransform(
					double peepeeyou, const Rectangle &bbox,
					const ICoord pxlsize)
    const
  {
    // The bounding box for the objects that are actually drawn is
    // smaller than the bitmap and centered in it.
    double bbw = peepeeyou*bbox.width();
    double bbh = peepeeyou*bbox.height();
    double deltax = 0.5*(pxlsize.x - bbw);
    double deltay = 0.5*(pxlsize.y - bbh);
    Coord offset = peepeeyou*bbox.lowerLeft() + Coord(-deltax, deltay);
    return Cairo::Matrix(peepeeyou, 0., 0., -peepeeyou,
			 -offset.x, bbh+offset.y);
  }
			     

  void OSCanvasImpl::setTransform(double scale) {
    assert(scale > 0.0);
    // If no layers are dirty and ppu hasn't changed, don't do anything.
    bool newppu = (scale != ppu);
    bool layersChanged = false;
    if(!newppu) {
      for(CanvasLayerImpl *layer : layers) {
	if(!layer->empty() && layer->dirty) {
	  layersChanged = true;
	  break;
	}
      }
    }
    if(initialized && !newppu && !layersChanged &&
       backingLayer.bitmapSize() == desiredBitmapSize())
      {
	return;
      }

    // Find the bounding box of all drawn objects at the new scale
    Rectangle bbox;
    for(CanvasLayerImpl *layer : layers) {
      if(!layer->empty()) {
	bbox.swallow(layer->findBoundingBox(scale, newppu));
      }
    }

    if(!bbox.initialized()) {
      // Nothing is drawn.
      transform = Cairo::identity_matrix();
    }
    else {
      if(newppu  || !boundingBox.initialized() || bbox != boundingBox) {
	boundingBox = bbox;
	ppu = scale;
	ICoord bitmapsz = desiredBitmapSize();
	setWidgetSize(bitmapsz.x, bitmapsz.y); // is a no-op for OSCanvasImpl

	transform = findTransform(ppu, boundingBox, bitmapsz);

	// Force layers to be redrawn
	for(CanvasLayerImpl *layer : layers) {
	  layer->dirty = true; 
	}
      }
    }

    backingLayer.rebuild();
    initialized = true;
  } // OSCanvasImpl::setTransform

  ICoord OSCanvasImpl::user2pixel(const Coord &pt) const {
    return backingLayer.user2pixel(pt);
  }

  Coord OSCanvasImpl::pixel2user(const ICoord &pt) const {
    return backingLayer.pixel2user(pt);
  }

  // This version is called from python.
  
  Coord *OSCanvasImpl::pixel2user(int px, int py) const {
    return new Coord(backingLayer.pixel2user(ICoord(px, py)));
  }

  double OSCanvasImpl::user2pixel(double d) const {
    assert(ppu > 0.0);
    return d * ppu;
  }

  double OSCanvasImpl::pixel2user(double d) const {
    assert(ppu > 0.0);
    return d/ppu;
  }

  ICoord OSCanvasImpl::desiredBitmapSize() const {
    return ICoord(ppu*boundingBox.width()*(1+2*margin),
		  ppu*boundingBox.height()*(1+2*margin));
  }

  //=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

  // getFilledPPU() and helper routines used to calculate the optimal
  // ppu for a given size canvas.  Used by GUICanvas::zoomToFill().
  // At one point this was also used when saving the canvas to a file,
  // and might be used that way again, which is why it's here and not
  // in guicanvas.C.

  // pixSize() computes the size in pixels that the window would have
  // to be at the given ppu in order to contain a bounding box from
  // bbmin to bbmax and a bunch of objects spanning user coords from
  // refLo to refHi with pixel extensions pLo and pHi.  X and Y are
  // handled separately. pixSize() just does one of them at a time.

  static double pixSize(double ppu,
			const std::vector<double> &pLo,
			const std::vector<double> &refLo,
			const std::vector<double> &pHi,
			const std::vector<double> &refHi)
  {
    // CanvasItem i extends from refLo[i] to refHi[i] in physical
    // units, but extends past that by pLo[i] and pHi[i] in pixel
    // units.
    // The total size is
    //    max(ppu*refHi_i + pHi_i) - min(ppu*refLo_i - pLo_i)
    double maxHi = -std::numeric_limits<double>::max();
    double minLo = std::numeric_limits<double>::max();
    int iHi = -1;
    int iLo = -1;
    for(unsigned int i=0; i<pLo.size(); i++) {
      double xHi = ppu*refHi[i] + pHi[i];
      if(xHi > maxHi) {
	maxHi = xHi;
	iHi = i;
      }
      double xLo = ppu*refLo[i] - pLo[i];
      if(xLo < minLo) {
	minLo = xLo;
	iLo = i;
      }
    }
    return maxHi - minLo;
  }

  //=\\=//

  // getFilledPPU() uses optimalPPU() to compute the ppu in one
  // direction.

  static double optimalPPU(double w0, 
			   const std::vector<double> &pLo,
			   const std::vector<double> &refLo, 
			   const std::vector<double> &pHi,
			   const std::vector<double> &refHi)
  {
    // Item i extends from
    //     xLo[i] = ppu*(refLo[i]-C) - pLo[i]
    // to
    //     xHi[i] = ppu*(refHi[i]-C) + pHi[i]
    // where the origin C is arbitrary and ignored.
    // The total width is 
    //     w(ppu) = max_i(xHi[i]) - min_i(xLo[i])
    // We need to solve w(ppu) = w0.
    // w is a piecewise linear function of ppu, so we find the
    // critical ppu values at which it changes slope, and look for a
    // solution in each interval.

    assert(pLo.size() == refLo.size() &&
	   pHi.size() == refHi.size() &&
	   pLo.size() == pHi.size());
    unsigned int n = pLo.size();

    double maxRefHi = -std::numeric_limits<double>::max();
    double minRefLo = std::numeric_limits<double>::max();
    int iMax, iMin;
    
    // ppus at which the slope of max(ppu*xmax+pmax) or
    // min(ppu*xmin-pmin) changes.
    std::vector<double> criticalPPUs;
    criticalPPUs.reserve(2*n*(n-1));
    criticalPPUs.push_back(0.0);

    // TODO: Is there a way to do this that isn't o(N^2)?
    
    for(unsigned int i=0; i<n; i++) {
      if(refHi[i] > maxRefHi) {
	maxRefHi = refHi[i];
	iMax = i;
      }
      if(refLo[i] < minRefLo) {
	minRefLo = refLo[i];
	iMin = i; 
      }
      for(unsigned int j=i+1; j<n; j++) {
	// Find the ppu at which items i and j extend equally far down.
	// ppu*refLo[i] - pLo[i] = ppu*refLo[j] - pLo[j]
	if(refLo[i] != refLo[j]) {
	  double ppu = (pLo[i] - pLo[j])/(refLo[i] - refLo[j]);
	  if(ppu > 0)
	    criticalPPUs.push_back(ppu);
	}
	// Find the ppu at which items i and j extend equally far up.
	// ppu*refHi[i] + pHi[i] = ppu*refHi[j] + pHi[j]
	if(refHi[i] != refHi[j]) {
	  double ppu = (pHi[j] - pHi[i])/(refHi[i] - refHi[j]);
	  if(ppu > 0)
	    criticalPPUs.push_back(ppu);
	}
      }
    }

    std::sort(criticalPPUs.begin(), criticalPPUs.end());

    double ppuMax = 0.0;	// maximum ppu that gives a solution
    for(unsigned int i=0; i<criticalPPUs.size()-1; i++) {
      // Interval in which W is a linear function of ppu
      double ppuA = criticalPPUs[i];
      double ppuB = criticalPPUs[i+1];
      if(ppuA != ppuB) {
	// Find value of ppu that gives width W = totalPixels
	double wA = pixSize(ppuA, pLo, refLo, pHi, refHi);
	double wB = pixSize(ppuB, pLo, refLo, pHi, refHi);
	if((wA - w0) * (wB - w0) <= 0.0) {
	  double ppu = ppuA + (w0 - wA)*(ppuB - ppuA)/(wB - wA);
	  if(ppuA <= ppu && ppu <= ppuB && ppu > ppuMax) {
	    ppuMax = ppu;
	  }
	}
      }
    }

    // The interval from the largest criticalPPU to infinity has to be
    // examined too.  As ppu goes to infinity, only the largest refHi
    // and smallest refLo contribute.
    // w0 = (ppu*refHi[iMax] + pHi[iMax]) - (ppu*refLo[iMin] - pLo[iMin])
    double ppu = (w0 - pHi[iMax] - pLo[iMin])/(refHi[iMax] - refLo[iMin]);
    if(ppu > criticalPPUs.back() && ppu > ppuMax)
      ppuMax = ppu;
    
    return ppuMax;
  } // optimalPPU

  //=\\=//

  // If the target surface is xsize by ysize pixels, compute the ppu
  // that fills the surface.  This is tricky, because some objects
  // have fixed sizes in device units and therefore change their size
  // when the ppu is changed.  n is the number of visible items being
  // drawn.
  
  double OSCanvasImpl::getFilledPPU(int n, double xsize, double ysize)
    const
  {
    if(n == 0)
      return 1.0;
    
    // Pixel extents of each item from its reference point.
    std::vector<double> pxLo, pxHi, pyLo, pyHi;
    // User coordinates of the upper and lower reference points of
    // each object in each direction.
    std::vector<double> xLo, yLo, xHi, yHi;
    pxLo.reserve(n);
    pxHi.reserve(n);
    pyLo.reserve(n);
    pyHi.reserve(n);
    xLo.reserve(n);
    xHi.reserve(n);
    yLo.reserve(n);
    yHi.reserve(n);

    for(CanvasLayerImpl *layer : layers) {
      if(layer->visible) {
	for(CanvasItem *item : layer->items) {
	  const Rectangle &bbox0 =
	    item->getImplementation()->findBareBoundingBox();
	  xHi.push_back(bbox0.xmax());
	  xLo.push_back(bbox0.xmin());
	  yHi.push_back(bbox0.ymax());
	  yLo.push_back(bbox0.ymin());
	  double pxlo, pxhi, pylo, pyhi;
	  item->getImplementation()->pixelExtents(pxlo, pxhi, pyhi, pylo);
	  pxLo.push_back(pxlo);
	  pxHi.push_back(pxhi);
	  pyHi.push_back(pyhi);
	  pyLo.push_back(pylo);
	}
      }
    }
    double ppu_x = optimalPPU(xsize, pxLo, xLo, pxHi, xHi);
    double ppu_y = optimalPPU(ysize, pyLo, yLo, pyHi, yHi);
    // Pick the smaller of ppu_x and ppu_y, so that the entire image
    // is visible in both directions.
    double newppu = ppu_x < ppu_y ? ppu_x : ppu_y;
    return newppu;
  }

  //=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

  // Routines that can be called from a mouse callback to retrieve the
  // CanvasItem(s) at a given user coordinate.
  
  std::vector<CanvasItem*> OSCanvasImpl::clickedItems(const Coord &where)
    const
  {
    std::vector<CanvasItem*> items;
    for(const CanvasLayerImpl *layer : layers) {
      if(layer->clickable) 
	layer->clickedItems(where, items);
    }
    return items;
  }

  std::vector<CanvasItem*> OSCanvasImpl::allItems() const {
    std::vector<CanvasItem*> items;
    for(const CanvasLayerImpl *layer : layers)
      layer->allItems(items);
    return items;
  }

  // The *_new versions of clickedItems and allItems return their
  // results in a new vector, because swig works better that way.  If
  // we instead swig the above versions, without using new, swig will
  // make an extra copy of the vectors.
  std::vector<CanvasItem*> *OSCanvasImpl::clickedItems_new(
						      const Coord *where)
    const
  {
    std::vector<CanvasItem*> *items = new std::vector<CanvasItem*>;
    for(const CanvasLayerImpl *layer : layers) 
      if(layer->clickable)
	layer->clickedItems(*where, *items);
    return items;
  }

  std::vector<CanvasItem*> *OSCanvasImpl::allItems_new() const {
    std::vector<CanvasItem*> *items = new std::vector<CanvasItem*>;
    for(const CanvasLayerImpl *layer : layers)
      layer->allItems(*items);
    return items;
  }

  //=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

  // Save the whole canvas or a region of it in various formats.  The
  // saveAs methods return true if they were successful.
  
  // SurfaceCreator objects are passed in to saveRegion() so that it
  // can make the correct type of Surface for the desired output.  If
  // the size were known ahead of time, a Surface could be passed in
  // instead, but it's not.
  
  bool OSCanvasImpl::saveRegion(SurfaceCreator &createSurface,
				int maxpix, // no. of pixels in max(w, h)
				bool drawBG,
				const Coord &pt0, const Coord &pt1)
  {
    if(nVisibleItems() == 0) {
      return false;
    }

    Rectangle region(pt0, pt1); // ensures that upperRight[i] >= lowerLeft[i]

    // Compute pixel size of region and make a PdfSurface to fit.
    Coord imgsize = region.upperRight() - region.lowerLeft();
    double peepeeyou = maxpix/(imgsize.x > imgsize.y ? imgsize.x : imgsize.y);
    Coord psize = peepeeyou*imgsize;
    ICoord pxlsize(ceil(psize.x), ceil(psize.y));
    
    auto surface = createSurface.create(pxlsize.x, pxlsize.y);
    cairo_t *ct = cairo_create(surface->cobj());
    auto outctxt = Cairo::RefPtr<Cairo::Context>(new Cairo::Context(ct, true));
    outctxt->set_antialias(antialiasing);

    if(drawBG)
      drawBackground(outctxt);

    // Create a surface and context to draw each layer to before it's
    // copied to the final surface.  The surface can be re-used for
    // each layer.
    auto layersurf = Cairo::Surface::create(surface,
					    Cairo::CONTENT_COLOR_ALPHA,
					    pxlsize.x, pxlsize.y);
    cairo_t *lt = cairo_create(layersurf->cobj());
    auto lctxt = Cairo::RefPtr<Cairo::Context>(new Cairo::Context(lt, true));

    Cairo::Matrix transf = findTransform(peepeeyou, region, pxlsize); 
    lctxt->set_matrix(transf);
    Coord deviceOrigin(0,0);
    lctxt->device_to_user(deviceOrigin.x, deviceOrigin.y);
    Coord offset = deviceOrigin - region.upperLeft();
    lctxt->translate(offset.x, offset.y);

    for(CanvasLayerImpl *layer : layers) {
      if(!layer->empty() && layer->visible) {
	// Clear the re-used surface for the layer.
	lctxt->save();
	lctxt->set_operator(Cairo::OPERATOR_CLEAR);
	lctxt->paint();
	lctxt->restore();

	// Draw the layer.
	layer->renderToContext(lctxt);

	// Copy the layer to the final surface.
	outctxt->set_source(layersurf, 0, 0);
	outctxt->paint_with_alpha(layer->alpha);
      }
    }
    outctxt->show_page();
    return true;
  } // OSCanvasImpl::saveRegion


  // saveAsPDF, saveAsPNG, etc, make an appropriate SurfaceCreator and
  // call saveRegion.
  
  bool OSCanvasImpl::saveAsPDF(const std::string &filename,
			       int maxpix, bool drawBG)
  {
    // Saving the whole image requires that we compute the ppu as if
    // we're zooming to fill.
    double newppu = getFilledPPU(nVisibleItems(), maxpix, maxpix); // margin?
    Rectangle bb = findBoundingBox(newppu);
    return saveRegionAsPDF(filename, maxpix, drawBG,
			   bb.lowerLeft(), bb.upperRight());
  }

  bool OSCanvasImpl::saveAsPNG(const std::string &filename,
			       int maxpix, bool drawBG)
  {
    // Saving the whole image requires that we compute the ppu as if
    // we're zooming to fill.
    double newppu = getFilledPPU(nVisibleItems(), maxpix, maxpix);
    Rectangle bb = findBoundingBox(newppu);
    return saveRegionAsPNG(filename, maxpix, drawBG,
			   bb.lowerLeft(), bb.upperRight());
  }

  // TODO?  In principle, we could save the canvas in more image
  // formats.  In practice, it doesn't seem like it's worth the
  // effort.  There are lots of tools that users can use to convert
  // from pdf or png to whatever format they like.  Supporting other
  // formats that aren't supported by Cairo would be a pain.

  bool OSCanvasImpl::saveRegionAsPDF(const std::string &filename,
				     int maxpix, bool drawBG,
				     const Coord &pt0, const Coord &pt1)
  {
    auto pdfsc = PDFSurfaceCreator(filename);
    return saveRegion(pdfsc, maxpix, drawBG, pt0, pt1);
  }

  bool OSCanvasImpl::saveRegionAsPDF(const std::string &filename,
				     int maxpix, bool drawBG,
				     const Coord *pt0, const Coord *pt1)
  {
    return saveRegionAsPDF(filename, maxpix, drawBG, *pt0, *pt1);
  }

  bool OSCanvasImpl::saveRegionAsPNG(const std::string &filename,
				     int maxpix, bool drawBG,
				     const Coord &pt0, const Coord &pt1)
  {
    auto isc = ImageSurfaceCreator();
    bool ok = saveRegion(isc, maxpix, drawBG, pt0, pt1);
    if(ok) {
      isc.saveAsPNG(filename);
    }
    return ok;
  }

  bool OSCanvasImpl::saveRegionAsPNG(const std::string &filename,
				     int maxpix, bool drawBG,
				     const Coord *pt0, const Coord *pt1)
  {
    return saveRegionAsPNG(filename, maxpix, drawBG, *pt0, *pt1);
  }

  // SurfaceCreators, used by OSCanvasImpl::saveRegion
  
  SurfaceCreator::~SurfaceCreator() {
    surface->finish();
  }
  
  Cairo::RefPtr<Cairo::Surface> PDFSurfaceCreator::create(int x, int y) {
    surface = Cairo::PdfSurface::create(filename, x, y);
    // Restrict output to pdf version 1.4.  The default (at least for
    // cairomm 1.12) seems to be 1.5.  When using 1.5, the pdf files
    // aren't reproducible.  They display correctly but contain binary
    // data that differs from run to run, making tests fail.
    Cairo::RefPtr<Cairo::PdfSurface> pdfsurf =
      Cairo::RefPtr<Cairo::PdfSurface>::cast_dynamic(surface);
    pdfsurf->restrict_to_version(Cairo::PDF_VERSION_1_4);
    return surface;
  }
  
  Cairo::RefPtr<Cairo::Surface> ImageSurfaceCreator::create(int x, int y) {
    CHECK_SURFACE_SIZE(x, y);
    surface = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, x, y);
    return surface;
  }

  void ImageSurfaceCreator::saveAsPNG(const std::string &filename) {
    surface->write_to_png(filename);
  }

  void OSCanvasImpl::datadump(const std::string &filename) const {
    std::ofstream os(filename.c_str());
    for(CanvasLayerImpl *layer : layers)
      layer->datadump(os);
    os.close();
  }

  //=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//
  //=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

  // OffScreenCanvas is a wrapper for OSCanvasImpl that can be
  // imported into external code without creating dependencies on
  // Cairo or other implementation details.

  OffScreenCanvas::OffScreenCanvas(double ppu)
    : osCanvasImpl(new OSCanvasImpl(ppu))
  {}

  OffScreenCanvas::OffScreenCanvas(OSCanvasImpl *impl)
    : osCanvasImpl(impl)
  {}

  OffScreenCanvas::~OffScreenCanvas() {
    delete osCanvasImpl;
  }

  CanvasLayer *OffScreenCanvas::newLayer(const std::string &name) {
    KeyHolder k(osCanvasImpl->lock, __FILE__, __LINE__);
    return osCanvasImpl->newLayer(name);
  }

  void OffScreenCanvas::deleteLayer(CanvasLayer *layer) {
    KeyHolder k(osCanvasImpl->lock, __FILE__, __LINE__);
    osCanvasImpl->deleteLayer(layer);
  }

  CanvasLayer* OffScreenCanvas::getLayer(int i) const {
    KeyHolder k(osCanvasImpl->lock, __FILE__, __LINE__);
    return osCanvasImpl->getLayer(i);
  }

  CanvasLayer* OffScreenCanvas::getLayer(const std::string& nm) const {
    KeyHolder k(osCanvasImpl->lock, __FILE__, __LINE__);
    return osCanvasImpl->getLayer(nm);
  }

  std::size_t OffScreenCanvas::nLayers() const {
    return osCanvasImpl->nLayers();
  }

  std::size_t OffScreenCanvas::nVisibleItems() const {
    return osCanvasImpl->nVisibleItems();
  }

  void OffScreenCanvas::raiseLayer(int layer, int howfar) {
    KeyHolder k(osCanvasImpl->lock, __FILE__, __LINE__);
    osCanvasImpl->raiseLayer(layer, howfar);
  }
  
  void OffScreenCanvas::lowerLayer(int layer, int howfar) {
    KeyHolder k(osCanvasImpl->lock, __FILE__, __LINE__);
    osCanvasImpl->lowerLayer(layer, howfar);
  }
  
  void OffScreenCanvas::raiseLayerToTop(int layer) {
    KeyHolder k(osCanvasImpl->lock, __FILE__, __LINE__);
    osCanvasImpl->raiseLayerToTop(layer);
  }
  
  void OffScreenCanvas::lowerLayerToBottom(int layer) {
    KeyHolder k(osCanvasImpl->lock, __FILE__, __LINE__);
    osCanvasImpl->lowerLayerToBottom(layer);
  }

  void OffScreenCanvas::clear() {
    KeyHolder k(osCanvasImpl->lock, __FILE__, __LINE__);
    osCanvasImpl->clear();
  }

  void OffScreenCanvas::draw() {
    KeyHolder k(osCanvasImpl->lock, __FILE__, __LINE__);
    osCanvasImpl->draw();
  }

  double OffScreenCanvas::getPixelsPerUnit() const {
    return osCanvasImpl->getPixelsPerUnit();
  }

  ICoord OffScreenCanvas::user2pixel(const Coord &pt) const {
    KeyHolder k(osCanvasImpl->lock, __FILE__, __LINE__);
    return osCanvasImpl->user2pixel(pt);
  }

  Coord OffScreenCanvas::pixel2user(const ICoord &pt) const {
    KeyHolder k(osCanvasImpl->lock, __FILE__, __LINE__);
    return osCanvasImpl->pixel2user(pt);
  }

  double OffScreenCanvas::user2pixel(double d) const {
    KeyHolder k(osCanvasImpl->lock, __FILE__, __LINE__);
    return osCanvasImpl->user2pixel(d);
  }

  double OffScreenCanvas::pixel2user(double d) const {
    KeyHolder k(osCanvasImpl->lock, __FILE__, __LINE__);
    return osCanvasImpl->pixel2user(d);
  }
  
  void OffScreenCanvas::setAntialias(bool f) {
    KeyHolder k(osCanvasImpl->lock, __FILE__, __LINE__);
    osCanvasImpl->setAntialias(f);
  }

  void OffScreenCanvas::setMargin(double m) {
    KeyHolder k(osCanvasImpl->lock, __FILE__, __LINE__);
    osCanvasImpl->setMargin(m);
  }

  bool OffScreenCanvas::empty() const {
    KeyHolder k(osCanvasImpl->lock, __FILE__, __LINE__);
    return osCanvasImpl->empty();
  }

  void OffScreenCanvas::setBackgroundColor(const Color &c) {
    KeyHolder k(osCanvasImpl->lock, __FILE__, __LINE__);
    osCanvasImpl->setBackgroundColor(c);
  }

  bool OffScreenCanvas::saveAsPDF(const std::string &filename, int pix, bool bg)
  {
    KeyHolder k(osCanvasImpl->lock, __FILE__, __LINE__);
    return osCanvasImpl->saveAsPDF(filename, pix, bg);
  }
  
  bool OffScreenCanvas::saveRegionAsPDF(const std::string &filename,
				     int pix, bool bg,
				     const Coord& p0, const Coord& p1)
  {
    KeyHolder k(osCanvasImpl->lock, __FILE__, __LINE__);
    return osCanvasImpl->saveRegionAsPDF(filename, pix, bg, p0, p1);
  }
  
  bool OffScreenCanvas::saveAsPNG(const std::string &filename, int pix, bool bg)
  {
    KeyHolder k(osCanvasImpl->lock, __FILE__, __LINE__);
    return osCanvasImpl->saveAsPNG(filename, pix, bg);
  }
  
  bool OffScreenCanvas::saveRegionAsPNG(const std::string &filename,
				     int pix, bool bg,
				     const Coord& p0, const Coord& p1)
  {
    KeyHolder k(osCanvasImpl->lock, __FILE__, __LINE__);
    return osCanvasImpl->saveRegionAsPNG(filename, pix, bg, p0, p1);
  }

  std::vector<CanvasItem*> OffScreenCanvas::clickedItems(const Coord &pt)
    const
  {
    KeyHolder k(osCanvasImpl->lock, __FILE__, __LINE__);
    return osCanvasImpl->clickedItems(pt);
  }

  std::vector<CanvasItem*> OffScreenCanvas::allItems() const {
    KeyHolder k(osCanvasImpl->lock, __FILE__, __LINE__);
    return osCanvasImpl->allItems();
  }
  
  void OffScreenCanvas::datadump(const std::string &filename) const {
    osCanvasImpl->datadump(filename);
  }

};				// namespace OOFCanvas


