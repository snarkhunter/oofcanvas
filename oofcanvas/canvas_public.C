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
#include "oofcanvas/utility.h" 

namespace OOFCanvas {

  CanvasPublic::CanvasPublic(double ppu)
    : theCanvas(new OffScreenCanvas(ppu))
  {}

  CanvasPublic::~CanvasPublic() {
    delete theCanvas;
  }

  CanvasLayerPublic *CanvasPublic::newLayer(const std::string &name) {
    return theCanvas->newLayer(name);
  }

  void CanvasPublic::deleteLayer(CanvasLayerPublic *layer) {
    theCanvas->deleteLayer(layer);
  }

  CanvasLayerPublic* CanvasPublic::getLayer(int i) const {
    return theCanvas->getLayer(i);
  }

  CanvasLayerPublic* CanvasPublic::getLayer(const std::string& nm) const {
    return theCanvas->getLayer(nm);
  }

  std::size_t CanvasPublic::nLayers() const {
    return theCanvas->nLayers();
  }

  std::size_t CanvasPublic::nVisibleItems() const {
    return theCanvas->nVisibleItems();
  }

  void CanvasPublic::raiseLayer(int layer, int howfar) {
    theCanvas->raiseLayer(layer, howfar);
  }
  
  void CanvasPublic::lowerLayer(int layer, int howfar) {
    theCanvas->lowerLayer(layer, howfar);
  }
  
  void CanvasPublic::raiseLayerToTop(int layer) {
    theCanvas->raiseLayerToTop(layer);
  }
  
  void CanvasPublic::lowerLayerToBottom(int layer) {
    theCanvas->lowerLayerToBottom(layer);
  }

  void CanvasPublic::clear() {
    theCanvas->clear();
  }

  void CanvasPublic::draw() {
    theCanvas->draw();
  }

  double CanvasPublic::getPixelsPerUnit() const {
    return theCanvas->getPixelsPerUnit();
  }

  ICoord CanvasPublic::user2pixel(const Coord &pt) const {
    return theCanvas->user2pixel(pt);
  }

  Coord CanvasPublic::pixel2user(const ICoord &pt) const {
    return theCanvas->pixel2user(pt);
  }

  double CanvasPublic::user2pixel(double d) const {
    return theCanvas->user2pixel(d);
  }

  double CanvasPublic::pixel2user(double d) const {
    return theCanvas->pixel2user(d);
  }
  
  void CanvasPublic::setAntialias(bool f) {
    theCanvas->setAntialias(f);
  }

  void CanvasPublic::setMargin(double m) {
    theCanvas->setMargin(m);
  }

  bool CanvasPublic::empty() const {
    return theCanvas->empty();
  }

  void CanvasPublic::setBackgroundColor(const Color &c) {
    theCanvas->setBackgroundColor(c);
  }

  bool CanvasPublic::saveAsPDF(const std::string &filename, int pix, bool bg) {
    return theCanvas->saveAsPDF(filename, pix, bg);
  }
  
  bool CanvasPublic::saveRegionAsPDF(const std::string &filename,
				     int pix, bool bg,
				     const Coord& p0, const Coord& p1)
  {
    return theCanvas->saveRegionAsPDF(filename, pix, bg, p0, p1);
  }
  
  bool CanvasPublic::saveAsPNG(const std::string &filename, int pix, bool bg) {
    return theCanvas->saveAsPNG(filename, pix, bg);
  }
  
  bool CanvasPublic::saveRegionAsPNG(const std::string &filename,
				     int pix, bool bg,
				     const Coord& p0, const Coord& p1)
  {
    return theCanvas->saveRegionAsPNG(filename, pix, bg, p0, p1);
  }

  std::vector<CanvasItem*> CanvasPublic::clickedItems(const Coord &pt)
    const
  {
    return theCanvas->clickedItems(pt);
  }

  std::vector<CanvasItem*> CanvasPublic::allItems() const {
    return theCanvas->allItems();
  }

};				// namespace OOFCanvas
