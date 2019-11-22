// -*- C++ -*-

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modified
 * versions of this software, you first contact the authors at
 * oof_manager@nist.gov. 
 */

#include "canvas.h"
#include "canvaslayer.h"
#include "canvasitem.h"


namespace OOFCanvas {
  
  CanvasLayer::CanvasLayer(Canvas *canvas) 
    : canvas(canvas),
      visible(true),
      clickable(false)
  {
    clear();
  }

  CanvasLayer::~CanvasLayer() {
    for(CanvasItem *item : items)
      delete item;
  }
  
  void CanvasLayer::clear() {
    surface = Cairo::RefPtr<Cairo::ImageSurface>(
		 Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, 
					     canvas->widthInPixels(),
					     canvas->heightInPixels()));
    cairo_t *ct = cairo_create(surface->cobj());
    context = Cairo::RefPtr<Cairo::Context>(new Cairo::Context(ct, true));
    context->set_matrix(canvas->getTransform());
  }

  void CanvasLayer::addItem(CanvasItem *item) {
    items.push_back(item);
    item->draw(context);
  }
  
  void CanvasLayer::redraw() {
    clear();
    for(CanvasItem *item : items) {
      item->draw(context);
    }
  }

  // CanvasLayer::draw copies the layer's surface to the Canvas's
  // surface.  The layer's items have already been drawn on its
  // surface.
  void CanvasLayer::draw(Cairo::RefPtr<Cairo::Context> ctxt) const {
    if(visible && !items.empty()) {
      ctxt->set_source(surface, 0, 0);
      ctxt->paint();
    }
  }

  Coord CanvasLayer::pixel2user(const ICoord &pt) const {
    double x = pt.x;
    double y = pt.y;
    context->device_to_user(x, y);
    return Coord(x, y);
  }

  ICoord CanvasLayer::user2pixel(const Coord &pt) const {
    double x = pt.x;
    double y = pt.y;
    context->user_to_device(x, y);
    return ICoord(x, y);
  }

  // TODO: Do we want to support different scales in the x and y
  // directions?
  double CanvasLayer::pixel2user(double d) const {
    double dummy = 0;
    context->device_to_user_distance(d, dummy);
    return d;
  }

  double CanvasLayer::user2pixel(double d) const {
    double dummy = 0;
    context->user_to_device_distance(d, dummy);
    return d;
  }

  void CanvasLayer::clickedItems(const Coord &pt,
				 std::vector<CanvasItem*> &clickeditems)
    const
  {
    for(CanvasItem *item : items) {
      if(item->boundingBox().contains(pt) && item->containsPoint(canvas, pt)) {
	clickeditems.push_back(item);
      }
    }
  }

  void CanvasLayer::allItems(std::vector<CanvasItem*> &itemlist) const {
    itemlist.insert(itemlist.end(), items.begin(), items.end());
  }
};				// namespace OOFCanvas
