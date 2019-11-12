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
#include "canvassegments.h"
#include <iostream>

namespace OOFCanvas {

  // Use this constructor if you know how many segments you'll be
  // drawing.
  CanvasSegments::CanvasSegments(int n)
  {
    segments.reserve(n);
  }

  void CanvasSegments::addSegment(double x0, double y0, double x1, double y1) {
    segments.emplace_back(x0, y0, x1, y1);
  }

  void CanvasSegments::setLineWidth(double w) {
    width = w;
  }

  void CanvasSegments::setLineColor(double r, double g, double b) {
    color = Color(r, g, b);
  }

  void CanvasSegments::setLineColor(double r, double g, double b, double a) {
    color = Color(r, g, b, a);
  }

  void CanvasSegments::drawItem(Cairo::RefPtr<Cairo::Context> ctxt) const {
    std::cerr << "CanvasSegments::drawItem: " << segments.size() << std::endl;
    ctxt->set_line_width(width);
    color.set(ctxt);
    for(const Segment &segment : segments) {
      ctxt->move_to(segment.p0.x, segment.p0.y);
      ctxt->line_to(segment.p1.x, segment.p1.y);
    }
    ctxt->stroke();
  }
};
