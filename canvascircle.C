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
		     center.x+radius, center.y+radius);
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
    if(fill && line) {
      double r = line ? radius + 0.5*lineWidth : radius;
      return (pt - center).norm2() <= r*r;
    }
    if(fill) {
      double r = radius + 0.5*lineWidth;
      return (pt - center).norm2() <= r*r;
    }
    if(line) {
      double d2 = (pt - center).norm2();
      double r0 = radius - 0.5*lineWidth;
      double r1 = radius + 0.5*lineWidth;
      return d2 >= r0*r0 && d2 <= r1*r1;
    }
    return false;
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
			       double angle_degrees)
    : center(cx, cy),
      r0(r0), r1(r1),
      angle(M_PI*angle_degrees/180.)
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
    // If there's a line around the ellipse, the point is on it only
    // if it's inside the ellipse that follows the outer edge of the
    // line.  Compute parameters for that ellipse, or the unperturbed
    // ellipse if there's no line.
    double c = cos(angle);
    double s = sin(angle);
    double rr0 = r0;
    double rr1 = r1;
    if(line) {
      rr0 += 0.5*lineWidth;
      rr1 += 0.5*lineWidth;
    }
    // (x/a)^2 and (y/b)^2 in the rotated coordinate system
    double px = ( p.x*c + p.y*s)/rr0;
    double py = (-p.x*s + p.y*c)/rr1;
    if(fill) {
      // If filled, we just have to know if the point is inside the
      // ellipse we just calculated.
      return px*px + py*py <= 1.0;
    }
    if(line) {
      // A point is on the unfilled ellipse only if it's on the line.
      // We've already computed the ellipse on the outer edge of the
      // line. 
      if(px*px + py*py > 1.0) // Is the point outside the outer edge?
	return false;
      // Compute the ellipse on the inner edge of the line.
      rr0 -= lineWidth;
      rr1 -= lineWidth;
      px = ( p.x*c + p.y*s)/rr0;
      py = (-p.x*s + p.y*c)/rr1;
      return px*px + py*py >= 1.0; // point is outside the inner edge
    }
    return false;
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
