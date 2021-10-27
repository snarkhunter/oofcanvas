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
  class Lock;

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
    
    virtual void start(CanvasLayer*, const Coord&); // sets startPt
    virtual void update(const Coord&); // sets currentPt
    void update(const Coord *pt) { update(*pt); }
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
    virtual void start(CanvasLayer*, const Coord&);
    virtual void stop();
    virtual void update(const Coord&);
  };

  class RectangleRubberBand : public RubberBand {
  protected:
    CanvasRectangle *rect;
  public:
    RectangleRubberBand() : rect(nullptr) {}
    virtual void start(CanvasLayer*, const Coord&);
    virtual void stop();
    virtual void update(const Coord&);
  };

  class CircleRubberBand : public RubberBand {
  protected:
    CanvasCircle *circle;
    CanvasSegment *seg;
  public:
    CircleRubberBand() : circle(nullptr), seg(nullptr) {}
    virtual void start(CanvasLayer*, const Coord&);
    virtual void stop();
    virtual void update(const Coord&);
  };

  class EllipseRubberBand : public RubberBand {
  protected:
    CanvasRectangle *rect;
    CanvasEllipse *ellipse;
  public:
    EllipseRubberBand() {}
    virtual void start(CanvasLayer*, const Coord&);
    virtual void stop();
    virtual void update(const Coord&);
  };

  class SpiderRubberBand : public RubberBand {
  protected:
    std::vector<Coord> points;
    CanvasSegments *segs;
    void makeSegs();
    Lock *lock;	// ptr so that we don't have to include utility_extra.h here
  public:
    SpiderRubberBand();
    ~SpiderRubberBand();
    void addPoints(const std::vector<Coord>*);
    virtual void start(CanvasLayer*, const Coord&);
    virtual void stop();
    virtual void update(const Coord&);
  };

};

#endif // OOFCANVASRUBBERBAND_H
