// -*- C++ -*-

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modified
 * versions of this software, you first contact the authors at
 * oof_manager@nist.gov. 
 */

#ifndef OOFCANVAS_SEGMENT_H
#define OOFCANVAS_SEGMENT_H

#include "oofcanvas/canvasshape.h"
#include "oofcanvas/utility.h"

namespace OOFCanvas {

  class CanvasArrowhead;

  class CanvasSegment : public CanvasShape {
  protected:
    Segment segment;
  public:
    CanvasSegment(const Coord &p0, const Coord &p1);
    CanvasSegment(const Coord *p0, const Coord *p1);
    static CanvasSegment *create(const Coord *p0, const Coord *p1) {
      return new CanvasSegment(p0, p1);
    }
    virtual const std::string &classname() const;
    const Segment& getSegment() const { return segment; }
    void setPoint0(const Coord&);
    void setPoint1(const Coord&);
    friend class CanvasArrowhead;
    friend std::ostream &operator<<(std::ostream&, const CanvasSegment&);
    friend std::ostream &operator<<(std::ostream&, const CanvasArrowhead&);
    virtual std::string print() const;
  };

  std::ostream &operator<<(std::ostream &, const CanvasSegment&);


  // A CanvasArrowhead gets most of its data from an associated
  // CanvasSegment, and therefore doesn't need to be derived from
  // CanvasShape.
  
  class CanvasArrowhead : public CanvasItem {
  protected:
    const CanvasSegment *segment;
    double width, length;
    double position;		// relative position along segment
    bool pixelScaling;
    bool reversed;		// pointing backwards?
    static Rectangle arrowheadBBox(const CanvasSegment*, double, double,
				   double, bool);
  public:
    CanvasArrowhead(const CanvasSegment*, double position, bool rev);
    static CanvasArrowhead *create(const CanvasSegment *s, double p, bool r) {
      return new CanvasArrowhead(s, p, r);
    }
    virtual const std::string &classname() const;

    void setSize(double, double);
    void setSizeInPixels(double, double);

    const CanvasSegment &getCanvasSegment() const { return *segment; }
    const Segment& getSegment() const { return segment->getSegment(); }
    double getWidth() const { return width; }
    double getLength() const { return length; }
    double getPosition() const { return position; }
    bool getPixelScaling() const { return pixelScaling; }
    bool getReversed() const { return reversed; }

    friend std::ostream &operator<<(std::ostream&, const CanvasArrowhead&);
    virtual std::string print() const;
  };

  std::ostream &operator<<(std::ostream&, const CanvasArrowhead&);
  
};				// namespace OOFCanvas


#endif // OOFCANVAS_SEGMENT_H
