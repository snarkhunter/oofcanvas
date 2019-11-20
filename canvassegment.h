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

namespace OOFCanvas {
  class CanvasSegment : public CanvasShape {
  protected:
    Segment segment;
    virtual void drawItem(Cairo::RefPtr<Cairo::Context>) const;
    virtual bool containsPoint(const Coord&) const;
  public:
    CanvasSegment(double x0, double y0, double x1, double y1);
    virtual const std::string &classname() const;
    virtual void setLineWidth(double);
    friend std::ostream &operator<<(std::ostream &, const CanvasSegment&);
    virtual std::string *print() const;
  };

  std::ostream &operator<<(std::ostream &, const CanvasSegment&);
};


#endif // OOF_CANVASSEGMENT_H
