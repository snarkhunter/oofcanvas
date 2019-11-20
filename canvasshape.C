// -*- C++ -*-

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modified
 * versions of this software, you first contact the authors at
 * oof_manager@nist.gov. 
 */

#include "canvasshape.h"

namespace OOFCanvas {

  void CanvasShape::setLineWidth(double w) {
    lineWidth = w;
    line = true;
  }

  void CanvasShape::setLineColor(const Color &color) {
    lineColor = color;
    line = true;
  }

  void CanvasFillableShape::setFillColor(const Color &color) {
    fillColor = color;
    fill = true;
  }

}; // namespace OOFCanvas
