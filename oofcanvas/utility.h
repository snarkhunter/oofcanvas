// -*- C++ -*-

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modified
 * versions of this software, you first contact the authors at
 * oof_manager@nist.gov. 
 */

#ifndef OOFCANVAS_UTIL_H
#define OOFCANVAS_UTIL_H

#include <iostream>
#include <sstream>
#include <vector>

namespace OOFCanvas {

  class CanvasColor {
  public:
    double red, green, blue, alpha;	// 0-1
    CanvasColor() : red(0), green(0), blue(0), alpha(1) {}
    CanvasColor(double r, double g, double b) 
      : red(r), green(g), blue(b), alpha(1)
    {}
    CanvasColor(double r, double g, double b, double a) 
      : red(r), green(g), blue(b), alpha(a)
    {}
    const CanvasColor &operator=(const CanvasColor &c) {
      red = c.red; green = c.green; blue = c.blue; alpha = c.alpha;
      return *this;
    }
    CanvasColor opacity(double) const;
  };

  std::ostream &operator<<(std::ostream&, const CanvasColor&);

  extern const CanvasColor black, white, red, green, blue, gray,
    yellow, magenta, cyan;

  class CanvasCoord {
  public:
    double x, y;
    CanvasCoord() : x(0.0), y(0.0) {}
    CanvasCoord(double x, double y) : x(x), y(y) {}
    CanvasCoord(const CanvasCoord &p) : x(p.x), y(p.y) {}
    const CanvasCoord &operator=(const CanvasCoord &p) { x = p.x; y = p.y; return *this; }
    double operator[](int i) const { return i == 0 ? x : y; }
    double &operator[](int i) { return i == 0 ? x : y; }
    CanvasCoord &operator*=(double a) { x *= a; y *= a; return *this; }
    CanvasCoord &operator/=(double a) { x /= a; y /= a; return *this; }
    CanvasCoord &operator+=(const CanvasCoord &b) { x += b.x; y += b.y; return *this; }
    CanvasCoord &operator-=(const CanvasCoord &b) { x -= b.x; y -= b.y; return *this; }
    CanvasCoord operator*(double) const;
    CanvasCoord operator/(double) const;
    CanvasCoord operator+(const CanvasCoord&) const;
    CanvasCoord operator-(const CanvasCoord&) const;
    double operator*(const CanvasCoord& b) const { return x*b.x + y*b.y; } // dot prod
    double norm2() const { return x*x + y*y; }
    bool operator==(const CanvasCoord&) const;
    bool operator!=(const CanvasCoord&) const;
  };

  inline CanvasCoord operator*(double a, CanvasCoord pt) { return pt*a; }
  double cross(const CanvasCoord&, const CanvasCoord&);


  std::ostream &operator<<(std::ostream&, const CanvasCoord&);

  class ICanvasCoord {
  public:
    int x, y;
    ICanvasCoord() : x(0), y(0) {}
    ICanvasCoord(int x, int y) : x(x), y(y) {}
    ICanvasCoord &operator+=(const ICanvasCoord &a) { x += a.x; y+= a.y; return *this; }
    ICanvasCoord &operator-=(const ICanvasCoord &a) { x -= a.x; y-= a.y; return *this; }
    int operator[](int i) const { return i == 0 ? x : y; }
    ICanvasCoord operator*(int) const;
    CanvasCoord operator*(double) const;
    CanvasCoord operator/(double) const;
    ICanvasCoord operator+(const ICanvasCoord&) const;
    ICanvasCoord operator-(const ICanvasCoord&) const;
    bool operator==(const ICanvasCoord&) const;
    bool operator!=(const ICanvasCoord&) const;
  };

  inline CanvasCoord operator*(double a, ICanvasCoord pt) { return pt*a; }
  inline ICanvasCoord operator*(int a, ICanvasCoord pt) { return pt*a; }

  CanvasCoord operator+(const CanvasCoord&, const ICanvasCoord&);
  CanvasCoord operator+(const ICanvasCoord&, const CanvasCoord&);
  CanvasCoord operator-(const CanvasCoord&, const ICanvasCoord&);
  CanvasCoord operator-(const ICanvasCoord&, const CanvasCoord&);

  std::ostream &operator<<(std::ostream&, const ICanvasCoord&);

  //=\\=//

  class Segment {
  public:
    CanvasCoord p0, p1;
    Segment(double x0, double y0, double x1, double y1)
      : p0(x0, y0), p1(x1, y1)
    {}
    Segment(const CanvasCoord &p0, const CanvasCoord &p1)
      : p0(p0), p1(p1)
    {}
    // Given a point, compute the normal distance from it to the
    // segment and the relative distance along the segment (0<alpha<1)
    // of its normal projection onto the segment.
    void projection(const CanvasCoord&, double &alpha, double &distance) const;

    // angle measured from the x axis, counterclockwise.
    double angle() const;
    CanvasCoord interpolate(double) const;
  };

  std::ostream &operator<<(std::ostream&, const Segment&);

  //=\\=//

  // Rectangle is used for bounding boxes.  It's not a rectangle drawn
  // on the canvas. Use CanvasRectangle for that.

  class Rectangle {
  private:
    void setup(double, double, double, double);
  protected:
    CanvasCoord pmin, pmax;
    bool initialized_;
  public:
    Rectangle();
    Rectangle(const CanvasCoord&, const CanvasCoord&);
    Rectangle(const CanvasCoord*, const CanvasCoord*);
    Rectangle(double xmin, double ymin, double xmax, double ymax);
    Rectangle(const Rectangle&);
    bool initialized() const { return initialized_; }
    void swallow(const CanvasCoord&);
    void swallow(const Rectangle &rect);
    void expand(double);
    void shift(const CanvasCoord&);
    void scale(double, double);
    double width() const { return pmax.x - pmin.x; }
    double height() const { return pmax.y - pmin.y; }
    double xmin() const { return pmin.x; }
    double xmax() const { return pmax.x; }
    double ymin() const { return pmin.y; }
    double ymax() const { return pmax.y; }
    double &xmin() { return pmin.x; }
    double &xmax() { return pmax.x; }
    double &ymin() { return pmin.y; }
    double &ymax() { return pmax.y; }
    CanvasCoord lowerLeft() const { return pmin; }
    CanvasCoord lowerRight() const { return CanvasCoord(pmax.x, pmin.y); }
    CanvasCoord upperLeft() const { return CanvasCoord(pmin.x, pmax.y); }
    CanvasCoord upperRight() const { return pmax; }
    CanvasCoord center() const;
    const Rectangle &operator=(const Rectangle&);
    bool contains(const CanvasCoord&) const;
    void clear() { initialized_ = false; }
    bool operator==(const Rectangle&) const;
    bool operator!=(const Rectangle&) const;

    friend std::ostream &operator<<(std::ostream&, const Rectangle&);
  };

  std::ostream &operator<<(std::ostream&, const Rectangle&);

  //=\\=//
  
  template <class TYPE>
  std::string to_string(const TYPE &x) {
    std::ostringstream os;
    os << x;
    return os.str();
  }

  std::ostream &operator<<(std::ostream&, const std::vector<double>&);

  //=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

  // Calls to gtk need to be done on a single thread.  If the program
  // is using multiple threads and crashing oddly,

  // (1) Add a call to OOFCanvas::set_mainthread() in the calling
  // program before the first call to any other OOFCanvas method.

  // (2) Rebuild OOFCanvas with the --debug flag:
  //      python setup.py build --debug ...

  // (3) Recompile all parts of the calling program that use OOFCanvas.
  
  // When the program is run, require_mainthread() calls in OOFCanvas
  // will throw an exception if the calling program is calling them
  // from the wrong thread.  Those OOFCanvas calls have to be moved to
  // the main thread.  Call require_mainthread() with __FILE__ and
  // __LINE__ arguments for easy debugging.

  void set_mainthread();
  bool check_mainthread();   // returns true if on the right thread.
  void require_mainthread(const char *file, int line); // aborts on wrong thread

  //=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

  // Macro to raise an informative exception instead of creating a
  // ImageSurface that's too large.  Call this before every call to
  // Cairo::ImageSurface::create, passing in the bitmap size.  Cairo
  // uses 16 bit integers for pixel indices. Since it crashes with
  // sizes greater than 2**15, I assume it's using signed integers.
#define CHECK_SURFACE_SIZE(x, y) \
  if((x) >= 32768 || (y) >= 32768) \
    throw CanvasException(std::string(__FILE__) + ":" + to_string(__LINE__) \
			  + " OOFCanvas bitmap is too large! " + \
			  to_string(x) + "x" + to_string(y))

};				// end namespace OOFCanvas

#endif // OOFCANVAS_UTIL_H

