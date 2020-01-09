// -*- C++ -*-

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modified
 * versions of this software, you first contact the authors at
 * oof_manager@nist.gov. 
 */

#ifndef OOFCANVASSHAPE_H
#define OOFCANVASSHAPE_H

#include "canvasitem.h"
#include "utility.h"

namespace OOFCanvas {
  class CanvasShape : public CanvasItem {
  protected:
    double lineWidth;
    Color lineColor;
    bool line;
  public:
    CanvasShape() : lineWidth(0), lineColor(black), line(false) {}
    virtual ~CanvasShape() {}
    // Subclasses may need to redefine setLineWidth if it's necessary
    // to recompute the bounding box whenever the line width changes.
    virtual void setLineWidth(double);
    virtual void setLineColor(const Color&);
  };

  class CanvasFillableShape : public CanvasShape {
  protected:
    Color fillColor;
    bool fill;
  public:
    CanvasFillableShape() : fillColor(black), fill(false) {
      line = false;
    }
    virtual ~CanvasFillableShape() {}
    virtual void setFillColor(const Color&); 
  };
};				// namespace OOFCanvas





#endif // OOFCANVASSHAPE_H