// -*- C++ -*-

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modified
 * versions of this software, you first contact the authors at
 * oof_manager@nist.gov. 
 */

#include "oofcanvas/canvasimpl.h"
#include "oofcanvas/canvaslayer.h"
#include "oofcanvas/canvasitem.h"
#include "oofcanvas/canvasitemimpl.h"


namespace OOFCanvas {

  CanvasLayerPublic::CanvasLayerPublic(const std::string &name)
    : name(name)
  {}
  
  CanvasLayerImpl::CanvasLayerImpl(OSCanvasImpl *canvas, const std::string &name) 
    : CanvasLayerPublic(name),
      canvas(canvas),
      alpha(1.0),
      visible(true),
      clickable(false),
      dirty(false)
  {}

  CanvasLayerImpl::~CanvasLayerImpl() {
    for(CanvasItem *item : items)
      delete item;
  }

  void CanvasLayerImpl::destroy() {
    // CanvasLayerImpl::destroy is provided as a slightly easier way to
    // delete a layer when a pointer to the Canvas isn't easily
    // available.  Canvas::deleteLayer calls the CanvasLayerImpl
    // destructor.  Don't do anthing else here.
    canvas->deleteLayer(this);
  }

  void CanvasLayerImpl::rebuild() {
    ICoord size(canvas->desiredBitmapSize());
    makeCairoObjs(size.x, size.y);
    context->set_matrix(canvas->getTransform());
    dirty = !empty();
  }

  void CanvasLayerImpl::makeCairoObjs(int x, int y) {
    if(!surface || surface->get_width() != x || surface->get_height() != y) {
      surface = Cairo::RefPtr<Cairo::ImageSurface>(
		   Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, x, y));
      cairo_t *ct = cairo_create(surface->cobj());
      context = Cairo::RefPtr<Cairo::Context>(new Cairo::Context(ct, true));
      dirty = true;
    }
    if(context->get_antialias() != canvas->antialiasing) {
      context->set_antialias(canvas->antialiasing);
      dirty = true;
    }
  }

  ICoord CanvasLayerImpl::bitmapSize() const {
    if(surface)
      return ICoord(surface->get_width(), surface->get_height());
    return ICoord(0,0);
  }

  void CanvasLayerImpl::clear() {
    if(surface) {
      context->save();
      context->set_operator(Cairo::OPERATOR_CLEAR);
      context->paint();
      context->restore();
      dirty = true;
    }
  }

  void CanvasLayerImpl::clear(const Color &color) {
    context->save();
    context->set_source_rgb(color.red, color.green, color.blue);
    context->set_operator(Cairo::OPERATOR_SOURCE);
    context->paint();
    context->restore();
    dirty = true;
  }

  void CanvasLayerImpl::writeToPNG(const std::string &filename) const {
    surface->write_to_png(filename);
  }

  void CanvasLayerImpl::addItem(CanvasItem *item) {
    assert(item->layer == nullptr);
    item->setLayer(this);
    items.push_back(item);
    dirty = true;
  }

  void CanvasLayerImpl::removeAllItems() {
    for(CanvasItem *item : items)
      delete item;
    items.clear();
    dirty = true;
  }

  Rectangle CanvasLayerImpl::findBoundingBox(double ppu, bool newppu) {
    if(!dirty && !newppu && bbox.initialized())
      return bbox;
    bbox = findBoundingBox(ppu);
    return bbox;
  }

  Rectangle CanvasLayerImpl::findBoundingBox(double ppu) const {
    Rectangle bb;
    for(CanvasItem *item : items)
      bb.swallow(item->findBoundingBox(ppu));
    return bb;
  }

  bool CanvasLayerImpl::empty() const {
    return items.empty();
  }

  void CanvasLayerImpl::show() {
    visible = true;
  }

  void CanvasLayerImpl::hide() {
    visible = false;
  }

  // raiseBy and lowerBy aren't called "raise" and "lower" because
  // "raise" is a Python keyword.
  
  void CanvasLayerImpl::raiseBy(int howfar) const {
    canvas->raiseLayer(canvas->layerNumber(this), howfar);
  };

  void CanvasLayerImpl::lowerBy(int howfar) const {
    canvas->lowerLayer(canvas->layerNumber(this), howfar);
  }

  void CanvasLayerImpl::raiseToTop() const {
    canvas->raiseLayerToTop(canvas->layerNumber(this));
  }

  void CanvasLayerImpl::lowerToBottom() const {
    canvas->lowerLayerToBottom(canvas->layerNumber(this));
  }
  
  void CanvasLayerImpl::render() {
    if(dirty) {
      rebuild();
      clear();		    // paints background color over everything
      renderToContext(context);	// draws all items
      dirty = false;
    }
  }

  
  void CanvasLayerImpl::renderToContext(Cairo::RefPtr<Cairo::Context> ctxt) const {
    for(CanvasItem *item : items) {
      item->getImplementation()->draw(ctxt);
    }
  }

  // CanvasLayerImpl::draw copies the layer's surface to the Canvas's
  // surface, via the Canvas' context, which is passed in as an
  // argument.  The layer's items have already been drawn on its
  // surface.
  
  void CanvasLayerImpl::copyToCanvas(Cairo::RefPtr<Cairo::Context> ctxt,
				 double hadj, double vadj)
    const
  {
    // hadj and vadj are pixel offsets, from the scroll bars.
    if(visible && !items.empty()) {
      ctxt->set_source(surface, -hadj, -vadj);
      ctxt->paint_with_alpha(alpha);
    }
  }

  Coord CanvasLayerImpl::pixel2user(const ICoord &pt) const {
    assert(context);
    Coord pp = pt + canvas->centerOffset;
    context->device_to_user(pp.x, pp.y);
    return pp;
  }

  ICoord CanvasLayerImpl::user2pixel(const Coord &pt) const {
    assert(context);
    Coord pp = pt - canvas->centerOffset/canvas->getPixelsPerUnit();
    context->user_to_device(pp.x, pp.y);
    return ICoord(pp.x, pp.y);
  }

  // TODO: Do we need to use device_to_user_distance here?  Can't we
  // just use ppu?
  double CanvasLayerImpl::pixel2user(double d) const {
    assert(context);
    double dummy = 0;
    context->device_to_user_distance(d, dummy);
    return d;
  }

  // TODO: Do we need to use device_to_user_distance here?  Can't we
  // just use ppu?
  double CanvasLayerImpl::user2pixel(double d) const {
    assert(context);
    double dummy = 0;
    context->user_to_device_distance(d, dummy);
    return d;
  }

  void CanvasLayerImpl::clickedItems(const Coord &pt,
				 std::vector<CanvasItem*> &clickeditems)
    const
  {
    // TODO? Use an R-tree for efficient search.
    for(CanvasItem *item : items) {
      if(item->findBoundingBox(canvas->getPixelsPerUnit()).contains(pt) &&
	 item->getImplementation()->containsPoint(canvas, pt))
	{
	  clickeditems.push_back(item);
	}
    }
  }

  void CanvasLayerImpl::allItems(std::vector<CanvasItem*> &itemlist) const {
    itemlist.insert(itemlist.end(), items.begin(), items.end());
  }

  std::ostream &operator<<(std::ostream &os, const CanvasLayerImpl &layer) {
    os << "CanvasLayerImpl(\"" << layer.name << "\")";
    return os;
  }

};				// namespace OOFCanvas
