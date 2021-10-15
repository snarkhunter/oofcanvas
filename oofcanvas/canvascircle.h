// -*- C++ -*-

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modified
 * versions of this software, you first contact the authors at
 * oof_manager@nist.gov. 
 */

#ifndef OOFCANVAS_CIRCLE_H
#define OOFCANVAS_CIRCLE_H

#include "oofcanvas/canvasshape.h"
#include "oofcanvas/utility.h"

// When the perimeter is drawn on a Circle, Ellipse, or Dot, the outer
// edge of the line is on the edge of the object.

namespace OOFCanvas {

  class CanvasCircle : public CanvasFillableShape {
  protected:
    Coord center;
    double radius;
  public:
    CanvasCircle(const Coord &c, double r);
    CanvasCircle(const Coord *c, double r);
    virtual const std::string &classname() const;
    const Coord &getCenter() const { return center; }
    const double getRadius() const { return radius; }
    void setRadius(double);
    void setCenter(const Coord&);
    friend std::ostream &operator<<(std::ostream&, const CanvasCircle&);
    virtual std::string print() const;
  };
  std::ostream &operator<<(std::ostream&, const CanvasCircle&);

  class CanvasEllipse : public CanvasFillableShape {
  protected:
    Coord center;
    // The distances from the center to the edge are radiusX and
    // radiusY in the x and y directions, if the ellipse is not
    // rotated.  If angle!=0, the ellipse is rotated in the
    // counterclockwise direction around its center.
    double r0, r1;
    double angle;		// stored in radians, specified in degrees
    void setup();
  public:
    CanvasEllipse(const Coord &c, const Coord &r, double angle);
    CanvasEllipse(const Coord *c, const Coord *r, double angle);
    virtual const std::string &classname() const;
    void update(const Coord &c, const Coord &r, double angle);
    const Coord& getCenter() const { return center; }
    double getR0() const { return r0; }
    double getR1() const { return r1; }
    double getAngleRadians() const { return angle; }
    friend std::ostream &operator<<(std::ostream&, const CanvasEllipse&);
    virtual std::string print() const;
  };
  std::ostream &operator<<(std::ostream&, const CanvasEllipse&);

  // A dot is a circle whose radius and lineWidth are given in pixels.
  // The size doesn't change when the window is zoomed.  The center is
  // given in user coordinates.
  
  class CanvasDot : public CanvasFillableShape {
  protected:
    Coord center;
    double radius;
  public:
    CanvasDot(const Coord &c, double r);
    CanvasDot(const Coord *c, double r);
    virtual const std::string &classname() const;
    const Coord& getCenter() const { return center; }
    double getRadius() const { return radius; }
    friend std::ostream &operator<<(std::ostream&, const CanvasDot&);
    virtual std::string print() const;
  };
  std::ostream &operator<<(std::ostream&, const CanvasDot&);

}; 				// namespace OOFCanvas

#endif // OOFCANVASC_IRCLE_H
