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
#include "canvasitem.h"
#include <iostream>

namespace OOFCanvas {
  
  CanvasItem::~CanvasItem() {}

  const std::string &CanvasItem::modulename() const {
    static const std::string name("oofcanvas");
    return name;
  }

  void CanvasItem::draw(Cairo::RefPtr<Cairo::Context> cr) {
    cr->save();
    try {
      drawItem(cr);
    }
    catch (...) {
      cr->restore();
      throw;
    }
    cr->restore();
  }
}; 				// namespace OOFCanvas
