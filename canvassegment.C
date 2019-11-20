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
#include "canvassegment.h"
#include <iostream>

namespace OOFCanvas {

  CanvasSegment::CanvasSegment(double x0, double y0, double x1, double y1)
    : segment(x0, y0, x1, y1)
  {
    bbox = Rectangle(x0, y0, x1, y1);
  }

  const std::string &CanvasSegment::classname() const {
    static const std::string name("CanvasSegment");
    return name;
  }

  void CanvasSegment::setLineWidth(double w) {
    CanvasShape::setLineWidth(w);
    bbox = Rectangle(segment.p0, segment.p1);
    bbox.expand(0.5*w);
  }

  void CanvasSegment::drawItem(Cairo::RefPtr<Cairo::Context> ctxt) const {
    ctxt->set_line_width(lineWidth);
    // Cairo::LINE_CAP_BUTT, Cairo::LINE_CAP_SQUARE are other options
    ctxt->set_line_cap(Cairo::LINE_CAP_ROUND);
    lineColor.set(ctxt);
    ctxt->move_to(segment.p0.x, segment.p0.y);
    ctxt->line_to(segment.p1.x, segment.p1.y);
    ctxt->stroke();
  }

  bool CanvasSegment::containsPoint(const Coord &pt) const {
    double alpha = 0;
    double distance2 = 0; // distance squared from pt to segment along normal
    segment.projection(pt, alpha, distance2);
    return (alpha >= 0.0 && alpha <= 1.0 &&
	    distance2 < 0.25*lineWidth*lineWidth);
  }

  std::string *CanvasSegment::print() const {
    return new std::string(to_string(*this));
  }

  std::ostream &operator<<(std::ostream &os, const CanvasSegment &seg) {
    os << "CanvasSegment(" << seg.segment << ")";
    return os;
  }
};
