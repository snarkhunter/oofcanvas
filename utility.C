// -*- C++ -*-

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modified
 * versions of this software, you first contact the authors at
 * oof_manager@nist.gov. 
 */

#include "utility.h"
#include <string.h>

namespace OOFCanvas {

  Coord Coord::operator*(double x) const {
    Coord result(*this);
    result *= x;
    return result;
  }

  Coord Coord::operator+(const Coord &other) const {
    Coord result(*this);
    result += other;
    return result;
  }

  Coord Coord::operator-(const Coord &other) const {
    Coord result(*this);
    result -= other;
    return result;
  }

  std::ostream &operator<<(std::ostream &os, const Coord &p) {
    return os << "(" << p.x << ", " << p.y << ")";
  }
  
  std::ostream &operator<<(std::ostream &os, const ICoord &p) {
    return os << "(" << p.x << ", " << p.y << ")";
  }

  //=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//
  
  Rectangle::Rectangle()
    : initialized(false)
  {}

  Rectangle::Rectangle(double x0, double y0, double x1, double y1)
    : initialized(true)
  {
    setup(x0, y0, x1, y1);
  }
  
  Rectangle::Rectangle(const Coord &a, const Coord &b)
    : initialized(true)
  {
    setup(a.x, a.y, b.x, b.y);
  }

  void  Rectangle::setup(double x0, double y0, double x1, double y1) {
    if(x0 < x1) {
      pmin.x = x0;
      pmax.x = x1;
    }
    else {
      pmin.x = x1;
      pmax.x = x0;
    }
    if(y0 < y1) {
      pmin.y = y0;
      pmax.y = y1;
    }
    else {
      pmin.y = y1;
      pmax.y = y0;
    }
    initialized = true;
  }

  void Rectangle::swallow(const Coord &p) {
    if(initialized) {
      if(p.x < pmin.x)
	pmin.x = p.x;
      else if(p.x > pmax.x)
	pmax.x = p.x;
      if(p.y < pmin.y)
	pmin.y = p.y;
      else if(p.y > pmax.y)
	pmax.y = p.y;
    }
    else {
      pmin = p;
      pmax = p;
      initialized = true;
    }
  }
  
  const Rectangle &Rectangle::operator=(const Rectangle &other) {
    initialized = other.initialized;
    pmin = other.pmin;
    pmax = other.pmax;
    return *this;
  }

  bool Rectangle::contains(const Coord &pt) const {
    return (pt.x >= pmin.x && pt.x <= pmax.x &&
	    pt.y >= pmin.y && pt.y <= pmax.y);
  }

  std::ostream &operator<<(std::ostream &os, const Rectangle &rect) {
    os << "Rectangle(" << rect.pmin << ", " << rect.pmax << ")";
    return os;
  }

  //=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

  // Given a point, compute the normal distance squared from it to the
  // segment and the position 0<alpha<1 along the segment of the
  // normal to the point.  That is, the normal from the point to the
  // segment intersects the segment at
  // (1-alpha)*seg.p0 + alpha*seg.p1.

  void Segment::projection(const Coord &pt, double &alpha, double &distance2)
    const
  {
    Coord pp = p1 - p0;
    alpha = ((pt - p0) * pp)/pp.norm2();
    distance2 = ((p0 + alpha*pp) - pt).norm2();
  }

  std::ostream &operator<<(std::ostream &os, const Segment &seg) {
    os << "[" << seg.p0 << ", " << seg.p1 << "]";
    return os;
  }

  //=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

  TMatrix::TMatrix(double a0, double a1, double a2, double a3) {
    a[0] = a0;
    a[1] = a1;
    a[2] = a2;
    a[3] = a3;
  }
  
  TMatrix::TMatrix(const TMatrix &other) {
    memcpy(a, other.a, 4*sizeof(double));
  }
  
  TMatrix &TMatrix::operator*=(const TMatrix &b) {
    double tmp[4];
    memcpy(tmp, a, 4*sizeof(double));
    a[0] = tmp[0]*b.a[0] + tmp[1]*b.a[2]; // 00
    a[1] = tmp[0]*b.a[1] + tmp[1]*b.a[3]; // 01
    a[2] = tmp[2]*b.a[0] + tmp[3]*b.a[2]; // 10
    a[3] = tmp[2]*b.a[2] + tmp[3]*b.a[3]; // 11
    return *this;
  }

  TMatrix &TMatrix::operator*=(double x) {
    a[0] *= x;
    a[1] *= x;
    a[2] *= x;
    a[3] *= x;
    return *this;
  }

  TMatrix TMatrix::operator*(const TMatrix &b) const {
    TMatrix result(*this);
    result *= b;
    return result;
  }

  TMatrix TMatrix::operator*(double x) const {
    TMatrix result(*this);
    result *= x;
    return result;
  }

  TMatrix operator*(double x, const TMatrix &a) {
    return a*x;
  }

  Coord operator*(const TMatrix &m, const Coord &x) {
    return Coord(m.a[0]*x.x + m.a[1]*x.y,
		 m.a[2]*x.x + m.a[3]*x.y);
  }

  Coord axpy(const TMatrix &a, const Coord &x, const Coord &y) {
    return Coord(a.a[0]*x.x + a.a[1]*x.y + y.x,
		 a.a[2]*x.x + a.a[3]*x.y + y.y);
  }

  //=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

  void Color::set(Cairo::RefPtr<Cairo::Context> ctxt) const {
    if(alpha == 1.0)
      ctxt->set_source_rgb(red, green, blue);
    else
      ctxt->set_source_rgba(red, green, blue, alpha);
  }
  
};				// namespace OOFCanvas
		   

