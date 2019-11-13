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

#include <iostream>
#include <cairomm/cairomm.h>

namespace OOFCanvas {
  
  class Color {
  public:
    double red, green, blue, alpha;	// 0-1
    Color() : red(0), green(0), blue(0), alpha(1) {}
    Color(double r, double g, double b) 
      : red(r), green(g), blue(b), alpha(1)
    {}
    Color(double r, double g, double b, double a) 
      : red(r), green(g), blue(b), alpha(a)
    {}
    const Color &operator=(const Color &c) {
      red = c.red; green = c.green; blue = c.blue; alpha = c.alpha;
      return *this;
    }
    void set(Cairo::RefPtr<Cairo::Context>) const;
  };

  class Coord {
  public:
    double x, y;
    Coord() : x(0.0), y(0.0) {}
    Coord(double x, double y) : x(x), y(y) {}
    const Coord &operator=(const Coord &p) { x = p.x; y = p.y; return *this; }
    Coord &operator*=(double a) { x *= a; y *= a; return *this; }
    Coord &operator+=(const Coord &b) { x += b.x; y += b.y; return *this; }
    Coord &operator-=(const Coord &b) { x -= b.x; y -= b.y; return *this; }
    Coord operator*(double) const;
    Coord operator+(const Coord&) const;
  };

  std::ostream &operator<<(std::ostream&, const Coord&);

  class ICoord {
  public:
    int x, y;
    ICoord() : x(0), y(0) {}
    ICoord(int x, int y) : x(x), y(y) {}
  };

  std::ostream &operator<<(std::ostream&, const ICoord&);

  //=\\=//

  class Segment {
  public:
    Coord p0, p1;
    Segment(double x0, double y0, double x1, double y1)
      : p0(x0, y0), p1(x1, y1)
    {}
    Segment(const Coord &p0, const Coord &p1)
      : p0(p0), p1(p1)
    {}
  };

  //=\\=//

  // Rectangle is used for bounding boxes.  It's not a rectangle drawn
  // on the canvas. Use CanvasRectangle for that.

  class Rectangle {
  private:
    void setup(double, double, double, double);
  protected:
    Coord pmin, pmax;
    bool initialized;
  public:
    Rectangle();
    Rectangle(const Coord&, const Coord&);
    Rectangle(double xmin, double ymin, double xmax, double ymax);
    void swallow(const Coord&);
    void swallow(const Rectangle &rect) {
      swallow(rect.pmin); swallow(rect.pmax);
    }
    double width() const { return pmax.x - pmin.x; }
    double height() const { return pmax.y - pmin.y; }
    double xmin() const { return pmin.x; }
    double xmax() const { return pmax.x; }
    double ymin() const { return pmin.y; }
    double ymax() const { return pmax.y; }
    const Rectangle &operator=(const Rectangle&);
    bool contains(const Coord&) const;
  };

  class IRectangle {
  public:
    int xmin, ymin;
    int width, height;
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

