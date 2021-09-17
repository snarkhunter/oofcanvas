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
#include "oofcanvas/canvasrectangle.h"
#include "oofcanvas/canvasshapeimpl.h"
#include "oofcanvas/utility_private.h"
#include <iostream>

namespace OOFCanvas {

  class CanvasRectangleImplementation
    : public CanvasFillableShapeImplementation<CanvasRectangle>
  {
  public:
    CanvasRectangleImplementation(CanvasRectangle *item, const Rectangle &bb)
      : CanvasFillableShapeImplementation<CanvasRectangle>(item, bb)
    {}
    virtual void drawItem(Cairo::RefPtr<Cairo::Context>) const;
    virtual bool containsPoint(const OSCanvasImpl*, const Coord&) const;
  };
  
  CanvasRectangle::CanvasRectangle(const Coord &p0, const Coord &p1)
    : CanvasFillableShape(new CanvasRectangleImplementation(
						    this, Rectangle(p0, p1))),
      xmin(p0.x), ymin(p0.y),
      xmax(p1.x), ymax(p1.y)
  {}

  CanvasRectangle::CanvasRectangle(const Coord *p0, const Coord *p1)
    : CanvasFillableShape(new CanvasRectangleImplementation(
						    this, Rectangle(*p0, *p1))),
      xmin(p0->x), ymin(p0->y),
      xmax(p1->x), ymax(p1->y)
  {}

  const std::string &CanvasRectangle::classname() const {
    static const std::string name("CanvasRectangle");
    return name;
  }

  void CanvasRectangleImplementation::drawItem(
				       Cairo::RefPtr<Cairo::Context> ctxt)
    const
  {
    double w = lineWidthInUserUnits(ctxt);
    double halfw = 0.5*w;
    Rectangle r = bbox;
    r.expand(-halfw);
    ctxt->move_to(r.xmin(), r.ymin());
    ctxt->line_to(r.xmax(), r.ymin());
    ctxt->line_to(r.xmax(), r.ymax());
    ctxt->line_to(r.xmin(), r.ymax());
    ctxt->close_path();
    fillAndStroke(ctxt);
  }

  bool CanvasRectangleImplementation::containsPoint(
			    const OSCanvasImpl *canvas, const Coord &pt)
    const
  {
    // We already know that the point is within the bounding box, so
    // if the rectangle is filled, the point is on it.
    double lw = lineWidthInUserUnits(canvas);
    return canvasitem->filled() || (canvasitem->lined() &&
				    (pt.x - bbox.xmin() <= lw ||
				     bbox.xmax() - pt.x <= lw ||
				     pt.y - bbox.ymin() <= lw ||
				     bbox.ymax() - pt.y <= lw));
  }

  std::string CanvasRectangle::print() const {
    return to_string(*this);
  }

  std::ostream &operator<<(std::ostream &os, const CanvasRectangle &rect) {
    os << "CanvasRectangle(" << Coord(rect.xmin, rect.ymin)
       << ", " << Coord(rect.xmax, rect.ymax) << ")";
    return os;
  }
};				// namespace OOFCanvas
