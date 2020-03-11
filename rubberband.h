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

namespace OOFCanvas {

  class CanvasLayer;
  class Color;

  class RubberBand {
  protected:
    bool active_;
    Coord startPt;
    Coord currentPt;
    CanvasLayer *layer;
    double lineWidth;
    Color color;
  public:
    RubberBand();
    virtual ~RubberBand();
    virtual void start(CanvasLayer*, double x, double y);
    virtual void draw(double x, double y) = 0;
    virtual void stop();
    bool active() const { return active_; }

    void setLineWidth(double w) { lineWidth = w; }
    void setColor(Color c) { color = c; }
  };

  // class NoRubberBand : public RubberBand {
  // public:
  //   virtual void start(CanvasLayer*, double, double) {}
  //   virtual void draw(double, double) {}
  //   virtual void stop() {}
  // };

  class RectangleRubberBand : public RubberBand {
  public:
    RectangleRubberBand() {}
    virtual void draw(double x, double y);
  };

};

#endif // OOFCANVASRUBBERBAND_H
