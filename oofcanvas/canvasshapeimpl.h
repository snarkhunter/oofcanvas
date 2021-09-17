// -*- C++ -*-

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modified
 * versions of this software, you first contact the authors at
 * oof_manager@nist.gov. 
 */

#ifndef OOFCANVAS_SHAPE_IMPL_H
#define OOFCANVAS_SHAPE_IMPL_H

#include "oofcanvas/canvasitemimpl.h"
#include "oofcanvas/canvasshape.h"
#include <cairomm/cairomm.h>

namespace OOFCanvas {

  template <class CANVASITEM>
  class CanvasShapeImplementation
    : public CanvasItemImplementation<CANVASITEM>
  {
  public:
    CanvasShapeImplementation(CANVASITEM *item, const Rectangle &bb)
      : CanvasItemImplementation<CANVASITEM>(item, bb)
    {}
    double lineWidthInUserUnits(Cairo::RefPtr<Cairo::Context>) const;
    double lineWidthInUserUnits(const OSCanvasImpl*) const;

    std::vector<double> dashLengthInUserUnits(Cairo::RefPtr<Cairo::Context>)
      const;

    // This pixelExtents is roughly correct for shapes whose only
    // pixel-sized feature is the line thickness.  If the line
    // thickness is large compared to the size of the object the
    // answer will be incorrect.
    virtual void pixelExtents(double&, double&, double&, double&) const;    

    // stroke sets line color, width, and dash pattern and draws the
    // lines.
    void stroke(Cairo::RefPtr<Cairo::Context>) const; 
  };

  template <class CANVASITEM>
  class CanvasFillableShapeImplementation
    : public CanvasShapeImplementation<CANVASITEM>
  {
  public:
    CanvasFillableShapeImplementation(CANVASITEM *item, const Rectangle &bb)
      : CanvasShapeImplementation<CANVASITEM>(item, bb)
    {}
    void fillAndStroke(Cairo::RefPtr<Cairo::Context> ctxt) const;
  };

};

#include "oofcanvas/canvasshapeimpl.C"

#endif // OOFCANVAS_SHAPE_IMPL_H
