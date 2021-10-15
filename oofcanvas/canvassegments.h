// -*- C++ -*-

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modified
 * versions of this software, you first contact the authors at
 * oof_manager@nist.gov. 
 */

#ifndef OOFCANVAS_SEGMENTS_H
#define OOFCANVAS_SEGMENTS_H

#include "oofcanvas/canvasshape.h"
#include "oofcanvas/utility.h"
#include <vector>

namespace OOFCanvas {

  // CanvasSegments is a set of unconnected line segments.
  
  class CanvasSegments : public CanvasShape {
  protected:
    std::vector<Segment> segments;
  public:
    CanvasSegments();
    CanvasSegments(int n);
    virtual const std::string &classname() const;
    void addSegment(const Coord&, const Coord&);
    void addSegment(const Coord *a, const Coord *b) { addSegment(*a, *b); }
    const std::vector<Segment> &getSegments() const { return segments; }
    std::vector<Segment> &getSegments() { return segments; }
    void setPoint0(const Coord&); // sets pt0 of all segments
    std::size_t size() const { return segments.size(); }
    friend std::ostream &operator<<(std::ostream &, const CanvasSegments&);
    virtual std::string print() const;
  };

  std::ostream &operator<<(std::ostream &, const CanvasSegments&);

  // CanvasCurve is a set of connected line segments.

  class CanvasCurve : public CanvasShape {
  protected:
    std::vector<Coord> points;
  public:
    CanvasCurve();
    CanvasCurve(int n);
    CanvasCurve(const std::vector<Coord>&);
    virtual const std::string &classname() const;
    void addPoint(const Coord&);
    void addPoint(const Coord *p) { addPoint(*p); }
    void addPoints(const std::vector<Coord>*);
    const std::vector<Coord> &getPoints() const { return points; }
    std::size_t size() const { return points.size(); }
    friend std::ostream &operator<<(std::ostream&, const CanvasCurve&);
    virtual std::string print() const;
  };

  std::ostream &operator<<(std::ostream&, const CanvasCurve&);
  
};				// namespace OOFCanvas


#endif // OOFCANVAS_SEGMENTS_H
