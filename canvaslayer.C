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
      visible(true)
  {
    clear();
  }

  CanvasLayer::~CanvasLayer() {
    std::cerr << "CanvasLayer::dtor: " << this << std::endl;
    for(CanvasItem *item : items)
      delete item;
  }
  
  void CanvasLayer::clear() {
    std::cerr << "CanvasLayer::clear" << std::endl;
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

      //#define DIRECT_TO_CANVAS
#ifdef DIRECT_TO_CANVAS
      std::cerr << "CanvasLayer::draw: direct to canvas" << std::endl;
      for(CanvasItem *item : items)
      	item->draw(ctxt);
#else
      std::cerr << "CanvasLayer::draw: copying to canvas" << std::endl;
      ctxt->set_source(surface, 0, 0);
      ctxt->paint();
#endif // DIRECT_TO_CANVAS
    }
  }
  
};				// namespace OOFCanvas
