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
#include "oofcanvas/canvasimpl.h"
#include "oofcanvas/canvasitem.h"
#include "oofcanvas/canvasitemimpl.h"
#include "oofcanvas/canvaslayer.h"
#include "oofcanvas/utility_extra.h"

#include <iostream>

namespace OOFCanvas {

#ifdef DEBUG
  static int count = 0;
#endif // DEBUG

  CanvasItemImplBase::CanvasItemImplBase(const Rectangle &rect)
    : layer(nullptr),
      bbox(rect)
#ifdef DEBUG
    , drawBBox(false)
#endif	// DEBUG
  {
// #ifdef DEBUG
//     std::cerr << "CanvasItemImplBase::ctor: " << this << " " << ++count
// 	      << std::endl;
// #endif // DEBUG
  }

  CanvasItem::CanvasItem(CanvasItemImplBase *impl)
    : implementation(impl)
  {
  }
  
  CanvasItem::~CanvasItem() {
    delete implementation;
    implementation = nullptr;
  }

  CanvasItemImplBase::~CanvasItemImplBase() {
// #ifdef DEBUG
//     std::cerr << "CanvasItemImplBase::dtor: " << this << " " << --count
// 	      << std::endl;
// #endif // DEBUG
  }

  void CanvasItem::setLayer(CanvasLayer *layer) {
    implementation->setLayer(layer);
  }
  
  const CanvasLayer *CanvasItem::getLayer() const {
    return implementation->getLayer();
  }
  
  CanvasItemImplBase *CanvasItem::getImplementation() const {
    return implementation;
  }


  void CanvasItemImplBase::pixelExtents(double &left, double &right,
					double &up, double &down)
    const
  {
    left = 0.0;
    right = 0.0;
    up = 0.0;
    down = 0.0;
  }  

  Rectangle CanvasItemImplBase::findBoundingBox(double ppu) const {
    Rectangle bb = findBareBoundingBox();
    assert(bb.initialized());
    double pLeft, pRight, pUp, pDown;
    pixelExtents(pLeft, pRight, pUp, pDown);
    double upp = 1./ppu;
    bb.xmin() -= pLeft*upp;
    bb.xmax() += pRight*upp;
    bb.ymin() -= pDown*upp;
    bb.ymax() += pUp*upp;
    return bb;
  }

  Rectangle CanvasItem::findBoundingBox(double ppu) const {
    return implementation->findBoundingBox(ppu);
  }

  void CanvasItemImplBase::draw(Cairo::RefPtr<Cairo::Context> ctxt) const
  {
    ctxt->save();
    try {
      drawItem(ctxt);
#ifdef DEBUG
      if(drawBBox) {
	ctxt->restore();
	ctxt->save();
	ctxt->set_line_width(bboxLineWidth);
	setColor(bboxColor, ctxt);
	ctxt->move_to(bbox.xmin(), bbox.ymin());
	ctxt->line_to(bbox.xmax(), bbox.ymin());
	ctxt->line_to(bbox.xmax(), bbox.ymax());
	ctxt->line_to(bbox.xmin(), bbox.ymax());
	ctxt->close_path();
	ctxt->stroke();
      }
#endif // DEBUG

    }
    catch (...) {
      ctxt->restore();
      throw;
    }
    ctxt->restore();
  }

  void CanvasItem::modified() {
    implementation->modified();
  }

  void CanvasItemImplBase::modified() {
    if(layer != nullptr)
      layer->markDirty();
  }

  void CanvasItem::drawBoundingBox(double width, const Color &color) {
    implementation->drawBoundingBox(width, color);    
  }
  
  void CanvasItemImplBase::drawBoundingBox(double lineWidth, const Color &color)
  {
#ifdef DEBUG
    bboxLineWidth = lineWidth;
    bboxColor = color;
    drawBBox = true;
#endif // DEBUG
  }

  std::ostream &operator<<(std::ostream &os, const CanvasItem &item) {
    // Call print() the in derived class, which calls operator<< for
    // the derived class.
    os << item.print();		
    return os;
  }

  std::string *CanvasItem::repr() const { // for calling from Python
    return new std::string(print());
  }

  bool CanvasItem::containsPoint(const OffScreenCanvas *canvas, const Coord &pt)
    const
  {
    return implementation->containsPoint(canvas->getCanvas(), pt);
  }

}; 				// namespace OOFCanvas
