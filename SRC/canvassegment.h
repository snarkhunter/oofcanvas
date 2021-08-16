// -*- C++ -*-

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modified
 * versions of this software, you first contact the authors at
 * oof_manager@nist.gov. 
 */

#ifndef OOF_CANVASSEGMENT_H
#define OOF_CANVASSEGMENT_H

#include "canvasshape.h"
#include "utility.h"
#include <vector>

namespace OOFCanvas {

  class CanvasArrowhead;

  class CanvasSegment : public CanvasShape {
  protected:
    Segment segment;
    virtual void drawItem(Cairo::RefPtr<Cairo::Context>) const;
    virtual bool containsPoint(const OffScreenCanvas*, const Coord&) const;
  public:
    CanvasSegment(const Coord &p0, const Coord &p1);
    CanvasSegment(const Coord *p0, const Coord *p1);
    virtual const std::string &classname() const;
    virtual void pixelExtents(double&, double&, double&, double&) const;
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
    virtual void drawItem(Cairo::RefPtr<Cairo::Context>) const;
    virtual bool containsPoint(const OffScreenCanvas*, const Coord&) const;
    Rectangle pixelBBox;
    static Rectangle arrowheadBBox(const CanvasSegment*, double, double,
				   double, bool);
  public:
    CanvasArrowhead(const CanvasSegment*, double position, bool rev);
    virtual const std::string &classname() const;

    void setSize(double, double);
    void setSizeInPixels(double, double);

    // Why is pixelExtents public?
    virtual void pixelExtents(double&, double&, double&, double&) const;

    friend std::ostream &operator<<(std::ostream&, const CanvasArrowhead&);
    virtual std::string print() const;
  };

  std::ostream &operator<<(std::ostream&, const CanvasArrowhead&);
  
};				// namespace OOFCanvas


#endif // OOF_CANVASSEGMENT_H
