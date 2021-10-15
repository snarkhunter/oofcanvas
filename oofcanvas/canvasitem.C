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

#ifdef DEBUG
#include <set>
#endif

namespace OOFCanvas {

  // TODO: Delete debugging lines
#ifdef DEBUG
  static std::set<const CanvasItem*> allItems;
  static std::set<const CanvasItemImplBase*> allImplementations;
  bool addItem(const CanvasItem *item) {
    return allItems.insert(item).second; // true if insert occurred
  }
  bool addImplementation(const CanvasItemImplBase *impl) {
    return allImplementations.insert(impl).second;
  }
  bool deleteItem(const CanvasItem *item) {
    return allItems.erase(item) == 1;
  }
  bool deleteImplementation(const CanvasItemImplBase *impl) {
    return allImplementations.erase(impl) == 1;
  }
  bool existsItem(const CanvasItem *item) {
    return allItems.count(item) == 1;
  }
  bool existsImplementation(const CanvasItemImplBase *impl) {
    std::cerr << "existsImplementation: impl=" << impl << std::endl;
    return allImplementations.count(impl) == 1 && impl->itemExists();
  }
#endif	// DEBUG
  
  CanvasItemImplBase::CanvasItemImplBase(const Rectangle &rect)
    : bbox(rect)
#ifdef DEBUG
    , drawBBox(false)
#endif	// DEBUG
  {
    assert(addImplementation(this));
    std::cerr << "CanvasItemImplBase::ctor: " << this << std::endl;
  }

  CanvasItem::CanvasItem(CanvasItemImplBase *impl)
    : layer(nullptr),
      implementation(impl)
  {
    assert(addItem(this));
    std::cerr << "CanvasItem::ctor: " << this
	      << " (impl=" << impl << ")" << std::endl;
  }
  
  CanvasItem::~CanvasItem() {
    assert(deleteItem(this));
    std::cerr << "CanvasItem::dtor: " << this
	      << " (DELETING impl=" << implementation << ")" << std::endl;
    delete implementation;
    implementation = nullptr;
  }

  CanvasItemImplBase::~CanvasItemImplBase() {
    assert(deleteImplementation(this));
    std::cerr << "CanvasItemImplBase::dtor: " << this << std::endl;
  }

  const std::string &CanvasItem::modulename() const {
    static const std::string name("oofcanvas.SWIG.oofcanvas");
    return name;
  }

#ifdef DEBUG
  bool CanvasItem::implementationExists() const {
    return existsImplementation(implementation);
  }
#endif	// DEBUG

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
    // std::cerr << "CanvasItemImplBase::findBoundingBox: this=" << this
    // 	      << std::endl;
    assert(existsImplementation(this));
    // std::cerr << "CanvasItemImplBase::findBoundingBox: exists!" << std::endl;
    Rectangle bb = findBareBoundingBox();
    assert(bb.initialized());
    double pLeft, pRight, pUp, pDown;
    pixelExtents(pLeft, pRight, pUp, pDown);
    double upp = 1./ppu;
    bb.xmin() -= pLeft*upp;
    bb.xmax() += pRight*upp;
    bb.ymin() -= pDown*upp;
    bb.ymax() += pUp*upp;
    // std::cerr << "CanvasItemImplBase::findBoundingBox: done, bb=" << bb
    // 	      << std::endl;
    return bb;
  }

  Rectangle CanvasItem::findBoundingBox(double ppu) const {
    // std::cerr << "CanvasItem::findBoundingBox: ppu=" << ppu;
    // std::cerr << " this=" << this << " " << *this;
    // std::cerr << " impl=" << implementation << std::endl;
    assert(existsItem(this));
    // std::cerr << "CanvasItem::findBoundingBox: this exists" << std::endl;
    assert(existsImplementation(implementation));
    // std::cerr << "CanvasItem::findBoundingBox: calling impl find bbox" << std::endl;
    return implementation->findBoundingBox(ppu);
  }

  void CanvasItemImplBase::draw(Cairo::RefPtr<Cairo::Context> ctxt) const
  {
    assert(existsImplementation(this));
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
