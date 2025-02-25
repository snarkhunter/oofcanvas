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
#include "oofcanvas/canvaspolygon.h"
#include "oofcanvas/canvasshapeimpl.h"

namespace OOFCanvas {

  class CanvasPolygonImplementation
    : public CanvasFillableShapeImplementation<CanvasPolygon>
  {
  public:
    CanvasPolygonImplementation(CanvasPolygon *item, const Rectangle &bb)
      : CanvasFillableShapeImplementation<CanvasPolygon>(item, bb)
    {}
    Rectangle bbox0;
    virtual void drawItem(Cairo::RefPtr<Cairo::Context>) const;
    virtual bool containsPoint(const OSCanvasImpl*, const Coord&) const;
  };

  CanvasPolygon::CanvasPolygon()
    : CanvasFillableShape(new CanvasPolygonImplementation(this, Rectangle()))
  {}

  CanvasPolygon::CanvasPolygon(int n)
    : CanvasFillableShape(new CanvasPolygonImplementation(this, Rectangle()))
  {
    corners.reserve(n);
  }

  CanvasPolygon::CanvasPolygon(const std::vector<Coord> &pts)
    : CanvasFillableShape(new CanvasPolygonImplementation(this, Rectangle()))
  {
    implementation->bbox = Rectangle();
    corners.reserve(pts.size());
    for(const Coord &pt : pts) {
      corners.push_back(pt);
      implementation->bbox.swallow(pt);
    }
  }

  const std::string &CanvasPolygon::classname() const {
    static const std::string name("CanvasPolygon");
    return name;
  }

  void CanvasPolygon::addPoint(const Coord &pt) {
    corners.push_back(pt);
    implementation->bbox.swallow(pt);
    modified();
  }

  void CanvasPolygon::addPoints(const std::vector<Coord> *pts) {
    corners.insert(corners.end(), pts->begin(), pts->end());
    for(const Coord &pt : *pts)
      implementation->bbox.swallow(pt);
    modified();
  }

  void CanvasPolygonImplementation::drawItem(Cairo::RefPtr<Cairo::Context> ctxt)
    const
  {
    if(canvasitem->size() < 2)
      return;
    auto iter = canvasitem->getCorners().begin();
    ctxt->move_to(iter->x, iter->y);
    while(++iter != canvasitem->getCorners().end()) {
      ctxt->line_to(iter->x, iter->y);
    }
    ctxt->close_path();
    fillAndStroke(ctxt);
  }

  int CanvasPolygon::windingNumber(const Coord &pt) const {
    // Compute the winding number of the polygon around a test point,
    // pt.  It's not necessary to check every segment, just the ones
    // that cross a line that passes through the test point. See
    // http://geomalgorithms.com/a03-_inclusion.html.
    int wn = 0;
    int n = corners.size();
    for(int i=0; i<n; i++) {	// loop over segments
      const Coord &next = corners[(i+1)%n];
      const Coord &prev = corners[i];
      if(prev.y <= pt.y) {
	if(pt.y < next.y) {
	  // An upward crossing of the line y=pt.y
	  if(cross(next-prev, pt-prev) > 0) {
	    // pt is to the left of the segment
	    ++wn;
	  }
	}
      }
      else {			// prev.y > pt.y
	if(next.y <= pt.y ) {
	  // A downward crossing of the line y=pt.y
	  if(cross(next-prev, pt-prev) < 0) {
	    // pt is to the right of the segment
	    --wn;
	  }
	}
      }
    } // end loop over segments
    return wn;
  }

  bool CanvasPolygonImplementation::containsPoint(
			  const OSCanvasImpl *canvas, const Coord &pt)
    const
  {
    if(canvasitem->filled()) {
      if(canvasitem->windingNumber(pt) != 0)
	return true;
      // If a thick perimeter is drawn, the click may be outside the
      // nominal polygon but still on the perimeter line, so we have
      // to do the line check even if the winding number check fails.
    }
    if(canvasitem->lined()) {
      const std::vector<Coord> &corners(canvasitem->getCorners());
      int n = corners.size();
      double lw = lineWidthInUserUnits(canvas);
      double hlw2 = 0.25*lw*lw; // (half line width)^2
      for(int i=0; i<n; i++) {
	const Segment segment(corners[i], corners[(i+1)%n]);
	double alpha = 0;
	double distance2 = 0;
	segment.projection(pt, alpha, distance2);
	if(alpha >= 0.0 && alpha <= 1.0 && distance2 < hlw2)
	  return true;
      }
    }
    return false;
  }

  std::string CanvasPolygon::print() const {
    return to_string(*this);
  }

  std::ostream &operator<<(std::ostream &os, const CanvasPolygon &poly) {
    os << "CanvasPolygon(";
    if(poly.size() > 0) {
      os << poly.corners[0];
      for(int i=1; i<poly.size(); i++)
	os << ", " << poly.corners[i];
    }
    os << ")";
    return os;
  }

}; // namespace OOFCanvas
