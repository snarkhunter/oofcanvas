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
#include "oofcanvas/utility_extra.h"

namespace OOFCanvas {

  void CanvasShape::setLineWidth(double w) {
    std::cerr << "CanvasShape::setLineWidth: " << this << " " << w << std::endl;
    lineWidth = w;
    lineWidthInPixels = false;
    line = true;
    modified();
    std::cerr << "CanvasShape::setLineWidth: done" << std::endl;
  }

  void CanvasShape::setLineWidthInPixels(double w) {
    std::cerr << "CanvasShape::setLineWidthInPixels: " << this << " " << w << std::endl;
    lineWidth = w;
    lineWidthInPixels = true;
    line = true;
    std::cerr << "CanvasShape::setLineWidthInPixels: calling modified" << std::endl;
    modified();
    std::cerr << "CanvasShape::setLineWidthInPixels: done" << std::endl;
  }

  void CanvasShape::setLineColor(const Color &color) {
    std::cerr << "CanvasShape::setLineColor: " << color << " " << this << std::endl;
    lineColor = color;
    line = true;
    std::cerr << "CanvasShape::setLineColor: done" << std::endl;
  }

  void CanvasShape::setDash(const std::vector<double> &d, int offset) {
    std::cerr << "CanvasShape::setDash: offset=" << offset << " " << this << std::endl;
    dash = d;
    dashOffset = offset;
    dashLengthInPixels = false;
    std::cerr << "CanvasShape::setDash: done" << std::endl;
  }
  
  void CanvasShape::setDash(const std::vector<double> *d, int offset) {
    std::cerr << "CanvasShape::setDash 2: offset=" << offset << " " << this << std::endl;
    setDash(*d, offset);
    std::cerr << "CanvasShape::setDash 2: done" << std::endl;
  }

  void CanvasShape::setDash(double d) {
    std::cerr << "CanvasShape::setDash 3: d=" << d << " " << this << std::endl;
    dash = std::vector<double>({d});
    dashOffset = 0;
    dashLengthInPixels = false;
    std::cerr << "CanvasShape::setDash 3: done" << std::endl;
  }
  
  void CanvasShape::setDashInPixels(const std::vector<double> &d, int offset) {
    std::cerr << "CanvasShape::setDashInPixels: offset=" << offset << " " << this << std::endl;
    dash = d;
    dashOffset = offset;
    dashLengthInPixels = true;
    std::cerr << "CanvasShape::setDashInPixels: done" << std::endl;
  }

  void CanvasShape::unsetDashes() {
    std::cerr << "CanvasShape::unsetDashes: " << this << std::endl;
    dash.clear();
    std::cerr << "CanvasShape::unsetDashes: done" << std::endl;
  }

  void CanvasShape::setDashInPixels(const std::vector<double> *d, int offset) {
    std::cerr << "CanvasShape::setDashInPixels: offset=" << offset << " " << this << std::endl;
    setDashInPixels(*d, offset);
    std::cerr << "CanvasShape::setDashInPixels: done" << std::endl;
  }

  void CanvasShape::setDashInPixels(double d) {
    std::cerr << "CanvasShape::setDashInPixels 2 d=" << d << " " << this << std::endl;
    dash = std::vector<double>({d});
    dashOffset = 0;
    dashLengthInPixels = true;
    std::cerr << "CanvasShape::setDashInPixels: done" << std::endl;
  }

  void CanvasShape::setDashColor(const Color &clr) {
    std::cerr << "CanvasShape::setDashColor: " << clr << " " << this << std::endl;
    dashColor = clr;
    dashColorSet = true;
    std::cerr << "CanvasShape::setDashColor: done" << std::endl;
  }

  //=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

  void CanvasFillableShape::setFillColor(const Color &color) {
    std::cerr << "CanvasFillableShape::setFillColor: " << color << " " << this << std::endl;
    fillColor = color;
    fill = true;
    std::cerr << "CanvasFillableShape::setFillColor: done" << std::endl;
  }
}; // namespace OOFCanvas
