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
    bbox.swallow(Coord(x0, y0));
    bbox.swallow(Coord(x1, y1));
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
    ctxt->set_line_width(width);
    // Cairo::LINE_CAP_BUTT, Cairo::LINE_CAP_SQUARE are other options
    ctxt->set_line_cap(Cairo::LINE_CAP_ROUND);
    color.set(ctxt);
    for(const Segment &segment : segments) {
      ctxt->move_to(segment.p0.x, segment.p0.y);
      ctxt->line_to(segment.p1.x, segment.p1.y);
    }
    ctxt->stroke();
  }

  bool CanvasSegments::containsPoint(const Coord &pt) const {
    for(const Segment &seg : segments) {
      double alpha = 0;
      double distance2 = 0; // distance squared from pt to segment along normal
      seg.projection(pt, alpha, distance2);
      if(alpha >= 0.0 && alpha <= 1.0 && distance2 < 0.25*width*width)
	return true;
    }
    return false;
  }

  std::ostream &operator<<(std::ostream &os, const CanvasSegments &segs) {
    os << "[";
    if(segs.size() > 0) {
      os << segs.segments[0];
      for(int i=1; i<segs.size(); i++)
	os << ", " << segs.segments[i];
    }
    os << "]";
    return os;
  }
};
