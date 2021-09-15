// -*- C++ -*-

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modified
 * versions of this software, you first contact the authors at
 * oof_manager@nist.gov. 
 */

#ifndef OOFCANVAS_SHAPE_H
#define OOFCANVAS_SHAPE_H

#include "oofcanvas/canvasitem.h"
#include "oofcanvas/utility.h"

namespace OOFCanvas {

  // These enums need to be defined in the same way as the equivalent
  // ones in Cairo.
  enum class LineCap {BUTT, ROUND, SQUARE};
  enum class LineJoin {MITER, ROUND, BEVEL};
  
  class CanvasShape : public CanvasItem {
  protected:
    double lineWidth;
    Color lineColor;
    Color dashColor;
    bool line;
    bool lineWidthInPixels;
    bool dashLengthInPixels;
    bool dashColorSet;
    int dashOffset;
    std::vector<double> dash;

    LineJoin lineJoin;
    LineCap lineCap;
  public:
    CanvasShape(CanvasItemImplBase *impl)
      : CanvasItem(impl),
	lineWidth(0),
	lineColor(black),
	line(false),
	lineWidthInPixels(false),
	dashLengthInPixels(false),
	dashOffset(0),
	lineJoin(LineJoin::MITER),
	lineCap(LineCap::ROUND)
    {}
    virtual ~CanvasShape() {}
    // Subclasses may need to redefine setLineWidth() and
    // setLineWidthInPixels() if it's necessary to recompute the
    // bounding box whenever the line width changes.
    virtual void setLineWidth(double);
    virtual void setLineWidthInPixels(double);  
    virtual void setLineColor(const Color&);
    void setLineJoin(LineJoin lj) { lineJoin = lj; }
    void setLineCap(LineCap lc) { lineCap = lc; }
    LineCap getLineCap() const { return lineCap; }
    LineJoin getLineJoin() const { return lineJoin; }

    bool lined() const { return line; }

    // getLineWidth() returns the value passed into setLineWidth() or
    // setLineWidthInPixels(), so it's not useful unless you know
    // which was used, which you can discover by calling
    // getLineWidthInPixels().  To get the actual line width, call
    // CanvasShapeImplementation::lineWidthInUserUnits() instead.
    double getLineWidth() const { return lineWidth; }
    bool getLineWidthInPixels() const { return lineWidthInPixels; }

    // Calling setDash() makes the lines dashed.  The args are a
    // vector of dash lengths, and an offset into that vector.
    // setDashInPixels() is the same, but the dash lengths are
    // interpreted in pixel units, not physical units.
    void setDash(const std::vector<double>&, int);
    void setDash(const std::vector<double>*, int); // ptr version for swig
    void setDash(double l); // same as setDash(std::vector<double>{l}, 0)
    void setDashInPixels(const std::vector<double>&, int);
    void setDashInPixels(const std::vector<double>*, int);
    void setDashInPixels(double l); // setDashInPixels(std::vector<double>{l},0)
    // If setDashColor() is called, the spaces between dashes will be
    // in the given color.  If it's not called, the spaces will be
    // blank.
    void setDashColor(const Color&);
    void unsetDashes();
    const std::vector<double>& getDash() const { return dash; }
    bool getDashLengthInPixels() const { return dashLengthInPixels; }
    bool getDashColorSet() const { return dashColorSet; }
    const Color &getDashColor() const { return dashColor; }
    int getDashOffset() const { return dashOffset; }

    const Color& getLineColor() const { return lineColor; }
  };

  class CanvasFillableShape : public CanvasShape {
  protected:
    Color fillColor;
    bool fill;
  public:
    CanvasFillableShape(CanvasItemImplBase *impl)
      : CanvasShape(impl),
	fillColor(black),
	fill(false)
    {
      line = false;
    }
    virtual ~CanvasFillableShape() {}
    virtual void setFillColor(const Color&);
    const Color& getFillColor() const { return fillColor; }
    bool filled() const { return fill; }
  };

};				// namespace OOFCanvas





#endif // OOFCANVAS_SHAPE_H
