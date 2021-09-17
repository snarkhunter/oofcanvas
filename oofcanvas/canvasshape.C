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
#include "oofcanvas/canvasshapeimpl.h"
#include "oofcanvas/utility_private.h"

namespace OOFCanvas {

  void CanvasShape::setLineWidth(double w) {
    lineWidth = w;
    lineWidthInPixels = false;
    line = true;
    modified();
  }

  void CanvasShape::setLineWidthInPixels(double w) {
    lineWidth = w;
    lineWidthInPixels = true;
    line = true;
    modified();
  }

  void CanvasShape::setLineColor(const Color &color) {
    lineColor = color;
    line = true;
  }

  void CanvasShape::setDash(const std::vector<double> &d, int offset) {
    dash = d;
    dashOffset = offset;
    dashLengthInPixels = false;
  }
  
  void CanvasShape::setDash(const std::vector<double> *d, int offset) {
    setDash(*d, offset);
  }

  void CanvasShape::setDash(double d) {
    dash = std::vector<double>({d});
    dashOffset = 0;
    dashLengthInPixels = false;
  }
  
  void CanvasShape::setDashInPixels(const std::vector<double> &d, int offset) {
    dash = d;
    dashOffset = offset;
    dashLengthInPixels = true;
  }

  void CanvasShape::unsetDashes() {
    dash.clear();
  }

  void CanvasShape::setDashInPixels(const std::vector<double> *d, int offset) {
    setDashInPixels(*d, offset);
  }

  void CanvasShape::setDashInPixels(double d) {
    dash = std::vector<double>({d});
    dashOffset = 0;
    dashLengthInPixels = true;
  }

  void CanvasShape::setDashColor(const Color &clr) {
    dashColor = clr;
    dashColorSet = true;
  }

  //=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

  void CanvasFillableShape::setFillColor(const Color &color) {
    fillColor = color;
    fill = true;
  }
}; // namespace OOFCanvas
