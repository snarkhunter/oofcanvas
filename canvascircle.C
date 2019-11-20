// -*- C++ -*-

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modified
 * versions of this software, you first contact the authors at
 * oof_manager@nist.gov. 
 */

#include "canvascircle.h"
#include "utility.h"
#include <math.h>

namespace OOFCanvas {

  CanvasCircle::CanvasCircle(double cx, double cy, double r)
    : center(cx, cy),
      radius(r)
  {
    bbox0 = Rectangle(center.x-radius, center.y-radius,
		     center.x+radius, center.x+radius);
    bbox = bbox0;
  }

  const std::string &CanvasCircle::classname() const {
    static const std::string name("CanvasCircle");
    return name;
  }

  void CanvasCircle::setLineWidth(double w) {
    CanvasShape::setLineWidth(w);
    bbox = bbox0;
    bbox.expand(0.5*lineWidth);
  }

  std::string *CanvasCircle::print() const {
    return new std::string(to_string(*this));
  }

  std::ostream &operator<<(std::ostream &os, const CanvasCircle &circ) {
    os << "CanvasCircle(" << circ.center << ", " << circ.radius << ")";
    return os;
  }

  bool CanvasCircle::containsPoint(const Coord &pt) const {
    return (pt - center).norm2() <= radius*radius;
  }

  void CanvasCircle::drawItem(Cairo::RefPtr<Cairo::Context> ctxt) const {
    ctxt->begin_new_sub_path();
    ctxt->set_line_width(lineWidth);
    ctxt->arc(center.x, center.y, radius, 0, 2*M_PI);

    if(fill && line) {
      fillColor.set(ctxt);
      ctxt->fill_preserve();
      lineColor.set(ctxt);
      ctxt->stroke();
    }
    else if(fill) {
      fillColor.set(ctxt);
      ctxt->fill();
    }
    else if(line) {
      lineColor.set(ctxt);
      ctxt->stroke();
    }
  }

  //=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

  CanvasEllipse::CanvasEllipse(double cx, double cy, double r0, double r1,
			       double angle)
    : center(cx, cy),
      r0(r0), r1(r1),
      angle(M_PI*angle/180.)
  {
    double c = cos(angle);
    double s = sin(angle);
    // Half-widths of the bounding box
    double dx = sqrt(c*c*r0*r0 + s*s*r1*r1);
    double dy = sqrt(c*c*r1*r1 + s*s*r0*r0);
    bbox0 = Rectangle(cx-dx, cy-dy, cx+dx, cy+dy);
    bbox = bbox0;
  }

  const std::string &CanvasEllipse::classname() const {
    static const std::string name("CanvasEllipse");
    return name;
  }
  
  void CanvasEllipse::setLineWidth(double w) {
    CanvasShape::setLineWidth(w);
    bbox = bbox0;
    bbox.expand(0.5*lineWidth);
  }

  std::string *CanvasEllipse::print() const {
    return new std::string(to_string(*this));
  }

  std::ostream &operator<<(std::ostream &os, const CanvasEllipse &ellipse) {
    os << "CanvasEllipse(center=" << ellipse.center << ", r0=" << ellipse.r0
       << ", r1=" << ellipse.r1 << ", angle=" << ellipse.angle << ")";
    return os;
  }

  bool CanvasEllipse::containsPoint(const Coord &pt) const {
    Coord p = pt - center;
    double c = cos(angle);
    double s = sin(angle);
    double px = ( p.x*c + p.y*s)/r0;
    double py = (-p.x*s + p.y*c)/r1;
    return px*px + py*py <= 1.0;
  }

  void CanvasEllipse::drawItem(Cairo::RefPtr<Cairo::Context> ctxt) const {
    ctxt->set_line_width(lineWidth);

    // Save and restore the context before stroking the line, so that
    // the line thickness isn't distorted.
    ctxt->save();

    // Operations are defined in the reverse of the order in which
    // they're applied.  We'll be drawing a circle with radius 1, then
    // scaling it so that it's an ellipse with radii r0 and r1, then
    // rotating it, then translating it to the desired center point.
    ctxt->translate(center.x, center.y);
    ctxt->rotate(angle);
    ctxt->scale(r0, r1);

    ctxt->begin_new_sub_path();
    ctxt->arc(0.0, 0.0, 1.0, 0, 2*M_PI);
    ctxt->restore();

    if(fill && line) {
      fillColor.set(ctxt);
      ctxt->fill_preserve();
      lineColor.set(ctxt);
      ctxt->stroke();
    }
    else if(fill) {
      fillColor.set(ctxt);
      ctxt->fill();
    }
    else if(line) {
      lineColor.set(ctxt);
      ctxt->stroke();
    }
  }
};				// namespace OOFCanvas
