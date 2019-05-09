// -*- C++ -*-

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modified
 * versions of this software, you first contact the authors at
 * oof_manager@nist.gov. 
 */

#ifndef OOFCANVASUTIL_H
#define OOFCANVASUTIL_H

namespace OOFCanvas {

  class Rectangle {
  public:
    double xmin, ymin;
    double width, height;
  };

  class IRectangle {
  public:
    int xmin, ymin;
    int width, height;
  };

  //=\\=//
  
  class Coord {
  public:
    double x, y;
    Coord(double x, double y) : x(x), y(y) {}
    Coord &operator*=(double a) { x *= a; y *= a; return *this; }
    Coord &operator+=(const Coord &b) { x += b.x; y += b.y; return *this; }
    Coord operator*(double) const;
    Coord operator+(const Coord&) const;
  };

  class ICoord {
  public:
    int x, y;
  };

  //=\\=//

  class TMatrix {
  private:
    double a[4];		// stored in order 00, 01, 10, 11
  public:
    TMatrix() { a[0] = 1; a[1] = 0; a[2] = 0; a[3] = 1; }
    TMatrix(double, double, double, double);
    TMatrix(const TMatrix&);
    TMatrix &operator*=(const TMatrix&);
    TMatrix &operator*=(double);
    TMatrix operator*(const TMatrix&) const;
    TMatrix operator*(double) const;
    friend Coord operator*(const TMatrix&, const Coord&);
    friend Coord axpy(const TMatrix&, const Coord&, const Coord&);
  };

  TMatrix operator*(double, const TMatrix&);
  Coord operator*(const TMatrix&, const Coord&);
  Coord axpy(const TMatrix&, const Coord&, const Coord&); // A*x + y

};

#endif // OOFCANVASUTIL_H

