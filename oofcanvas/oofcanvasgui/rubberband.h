// -*- C++ -*-

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modified
 * versions of this software, you first contact the authors at
 * oof_manager@nist.gov. 
 */

#ifndef OOFCANVASRUBBERBAND_H
#define OOFCANVASRUBBERBAND_H

#include "oofcanvas/utility.h"
#include <vector>

namespace OOFCanvas {

  class CanvasCircle;
  class CanvasEllipse;
  class CanvasLayer;
  class CanvasLayerImpl;
  class CanvasRectangle;
  class CanvasSegment;
  class CanvasSegments;
  class CanvasShape;
  class Color;

  class RubberBand {
  protected:
    bool active_;
    Coord startPt;
    Coord currentPt;
    CanvasLayer *layer;
    double lineWidth;
    Color color;
    Color dashColor;
    double dashLength;		// length==0 means no dashes
    bool dashed;
    bool coloredDashes;
    void doDashes(CanvasShape*);
  public:
    RubberBand();
    virtual ~RubberBand();
    
    // TODO GTK: Rename "draw" to "update".  start and update should
    // take Coord args, not doubles.
    virtual void start(CanvasLayer*, double x, double y); // sets startPt
    virtual void draw(double x, double y); // sets currentPt
    virtual void stop();
    bool active() const { return active_; }

    void setLineWidth(double w) { lineWidth = w; }
    void setColor(const Color &c) { color = c; }

    void setDashed(bool d) { dashed = d; }
    void setDashColor(const Color&);
    void setDashLength(double l) { dashLength = l; }
  };

  class LineRubberBand : public RubberBand {
  protected:
    CanvasSegment *seg;
  public:
    LineRubberBand() {}
    virtual void start(CanvasLayer*, double, double);
    virtual void stop();
    virtual void draw(double x, double y);
  };

  class RectangleRubberBand : public RubberBand {
  protected:
    CanvasRectangle *rect;
  public:
    RectangleRubberBand() : rect(nullptr) {}
    virtual void start(CanvasLayer*, double, double);
    virtual void stop();
    virtual void draw(double x, double y);
  };

  class CircleRubberBand : public RubberBand {
  protected:
    CanvasCircle *circle;
    CanvasSegment *seg;
  public:
    CircleRubberBand() : circle(nullptr), seg(nullptr) {}
    virtual void start(CanvasLayer*, double, double);
    virtual void stop();
    virtual void draw(double x, double y);
  };

  class EllipseRubberBand : public RubberBand {
  protected:
    CanvasRectangle *rect;
    CanvasEllipse *ellipse;
  public:
    EllipseRubberBand() {}
    virtual void start(CanvasLayer*, double, double);
    virtual void stop();
    virtual void draw(double x, double y);
  };

  class SpiderRubberBand : public RubberBand {
  protected:
    std::vector<Coord> points;
    CanvasSegments *segs;
    void makeSegs();
  public:
    SpiderRubberBand();
    void addPoints(const std::vector<Coord>*);
    virtual void start(CanvasLayer*, double, double);
    virtual void stop();
    virtual void draw(double x, double y);
  };

};

#endif // OOFCANVASRUBBERBAND_H
