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

#include <algorithm>

namespace OOFCanvas {

  CanvasLayer::CanvasLayer(const std::string &name)
    : name(name)
  {  }
  
  CanvasLayerImpl::CanvasLayerImpl(OSCanvasImpl *canvas, const std::string &name) 
    : CanvasLayer(name),
      canvas(canvas),
      alpha(1.0),
      visible(true),
      clickable(false),
      dirty(false)
  {
    // It may be possible to disable (or eliminate) the layerlock
    // safely.  But leaving it enabled doesn't have much of an effect
    // on performance, so there is no point in removing it.
    //layerlock.disable();
  }

  CanvasLayer::~CanvasLayer() {
  }

  CanvasLayerImpl::~CanvasLayerImpl() {
    KeyHolder kh(layerlock, __FILE__, __LINE__);
    for(CanvasItem *item : items)
      delete item;
  }

  void CanvasLayerImpl::destroy() {
    // CanvasLayerImpl::destroy is provided as a slightly easier way to
    // delete a layer when a pointer to the Canvas isn't easily
    // available.  Canvas::deleteLayer calls the CanvasLayerImpl
    // destructor.  Don't do anything else here.
    canvas->deleteLayer(this);
  }

  void CanvasLayerImpl::rebuild() {
    KeyHolder kh(layerlock, __FILE__, __LINE__);
    rebuild_nolock();
  }
  
  void CanvasLayerImpl::rebuild_nolock() {
    ICoord size(canvas->desiredBitmapSize());
    makeCairoObjs(size.x, size.y);
    context->set_matrix(canvas->getTransform());
    dirty = !items.empty();
  }

  void CanvasLayerImpl::makeCairoObjs(int x, int y) {
    // This can't require the main thread, because it must be run to
    // create an off screen canvas, which ought to be possible on any
    // thread.
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
    KeyHolder kh(layerlock, __FILE__, __LINE__);
    clear_nolock();
  }
  
  void CanvasLayerImpl::clear_nolock() {
    if(surface) {
      context->save();
      context->set_operator(Cairo::OPERATOR_CLEAR);
      context->paint();
      context->restore();
      dirty = true;
    }
  }

  void CanvasLayerImpl::clear(const Color &color) {
    KeyHolder kh(layerlock, __FILE__, __LINE__);
    clear_nolock(color);
  }
  
  void CanvasLayerImpl::clear_nolock(const Color &color) {
    context->save();
    context->set_source_rgb(color.red, color.green, color.blue);
    context->set_operator(Cairo::OPERATOR_SOURCE);
    context->paint();
    context->restore();
    dirty = true;
  }

  void CanvasLayerImpl::writeToPNG(const std::string &filename) const {
    KeyHolder kh(layerlock, __FILE__, __LINE__);
    surface->write_to_png(filename);
  }

  void CanvasLayerImpl::addItem(CanvasItem *item) {
    KeyHolder kh(layerlock, __FILE__, __LINE__);
    assert(item->getLayer() == nullptr);
    item->setLayer(this);
    items.push_back(item);
    dirty = true;
  }

  void CanvasLayerImpl::removeAllItems() {
    KeyHolder kh(layerlock, __FILE__, __LINE__);
    for(CanvasItem *item : items)
      delete item;
    items.clear();
    dirty = true;
  }

  void CanvasLayerImpl::removeItem(CanvasItem *item) {
    KeyHolder kh(layerlock, __FILE__, __LINE__);
    auto iter = std::find(items.begin(), items.end(), item);
    assert(iter != items.end());
    items.erase(iter);
    delete item;
    dirty = true;
  };

  Rectangle CanvasLayerImpl::findBoundingBox(double ppu, bool newppu) {
    KeyHolder kh(layerlock, __FILE__, __LINE__);
    if(!dirty && !newppu && bbox.initialized())
      return bbox;
    bbox = findBoundingBox_nolock(ppu);
    return bbox;
  }

  Rectangle CanvasLayerImpl::findBoundingBox(double ppu) const {
    KeyHolder kh(layerlock, __FILE__, __LINE__);
    return findBoundingBox_nolock(ppu);
  }
  
  Rectangle CanvasLayerImpl::findBoundingBox_nolock(double ppu) const {
    Rectangle bb;
    for(CanvasItem *item : items)
      bb.swallow(item->findBoundingBox(ppu));
    return bb;
  }

  bool CanvasLayerImpl::empty() const {
    KeyHolder kh(layerlock, __FILE__, __LINE__);
    return items.empty();
  }

  void CanvasLayerImpl::show() {
    KeyHolder kh(layerlock, __FILE__, __LINE__);
    visible = true;
  }

  void CanvasLayerImpl::hide() {
    KeyHolder kh(layerlock, __FILE__, __LINE__);
    visible = false;
  }

  // raiseBy and lowerBy aren't called "raise" and "lower" because
  // "raise" is a Python keyword.
  
  void CanvasLayerImpl::raiseBy(int howfar) const {
    KeyHolder kh(layerlock, __FILE__, __LINE__);
    canvas->raiseLayer(canvas->layerNumber(this), howfar);
  };

  void CanvasLayerImpl::lowerBy(int howfar) const {
    KeyHolder kh(layerlock, __FILE__, __LINE__);
    canvas->lowerLayer(canvas->layerNumber(this), howfar);
  }

  void CanvasLayerImpl::raiseToTop() const {
    KeyHolder kh(layerlock, __FILE__, __LINE__);
    canvas->raiseLayerToTop(canvas->layerNumber(this));
  }

  void CanvasLayerImpl::lowerToBottom() const {
    KeyHolder kh(layerlock, __FILE__, __LINE__);
    canvas->lowerLayerToBottom(canvas->layerNumber(this));
  }
  
  void CanvasLayerImpl::render() {
    KeyHolder kh(layerlock, __FILE__, __LINE__);
    if(dirty) {
      rebuild_nolock();
      clear_nolock();	    // paints background color over everything
      renderToContext_nolock(context);	// draws all items
      dirty = false;
    }
  }

  
  void CanvasLayerImpl::renderToContext(Cairo::RefPtr<Cairo::Context> ctxt)
    const
  {
    KeyHolder kh(layerlock, __FILE__, __LINE__);
    renderToContext_nolock(ctxt);
  }
  
  void CanvasLayerImpl::renderToContext_nolock(
				       Cairo::RefPtr<Cairo::Context> ctxt)
    const
  {
    // This doesn't need to be called on the main thread if the
    // context is not the context for the graphics window.
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
    require_mainthread(__FILE__, __LINE__);
    KeyHolder kh(layerlock, __FILE__, __LINE__);
    // hadj and vadj are pixel offsets, from the scroll bars.
    if(visible && !items.empty()) {
      ctxt->set_source(surface, -hadj, -vadj);
      ctxt->paint_with_alpha(alpha);
    }
  }

  Coord CanvasLayerImpl::pixel2user(const ICoord &pt) const {
    KeyHolder kh(layerlock, __FILE__, __LINE__);
    assert(context);
    Coord pp = pt + canvas->centerOffset;
    context->device_to_user(pp.x, pp.y);
    return pp;
  }

  ICoord CanvasLayerImpl::user2pixel(const Coord &pt) const {
    KeyHolder kh(layerlock, __FILE__, __LINE__);
    assert(context);
    Coord pp = pt - canvas->centerOffset/canvas->getPixelsPerUnit();
    context->user_to_device(pp.x, pp.y);
    return ICoord(pp.x, pp.y);
  }

  double CanvasLayerImpl::pixel2user(double d) const {
    KeyHolder kh(layerlock, __FILE__, __LINE__);
    assert(canvas != nullptr && canvas->ppu > 0.0);
    return d/canvas->ppu;
  }

  double CanvasLayerImpl::user2pixel(double d) const {
    KeyHolder kh(layerlock, __FILE__, __LINE__);
    assert(canvas != nullptr && canvas->ppu > 0.0);
    return d*canvas->ppu;
  }

  void CanvasLayerImpl::clickedItems(const Coord &pt,
				 std::vector<CanvasItem*> &clickeditems)
    const
  {
    KeyHolder kh(layerlock, __FILE__, __LINE__);
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
    KeyHolder kh(layerlock, __FILE__, __LINE__);
    itemlist.insert(itemlist.end(), items.begin(), items.end());
  }

  std::ostream &operator<<(std::ostream &os, const CanvasLayerImpl &layer) {
    os << "CanvasLayerImpl(\"" << layer.name << "\")";
    return os;
  }

  std::ostream &operator<<(std::ostream &os, const CanvasLayer &layer) {
    os << "CanvasLayer(\"" << layer.name << "\")";
    return os;
  }

};				// namespace OOFCanvas
