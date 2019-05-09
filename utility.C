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

};				// namespace OOFCanvas
		   
