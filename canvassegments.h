// -*- C++ -*-

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modified
 * versions of this software, you first contact the authors at
 * oof_manager@nist.gov. 
 */

#ifndef OOF_CANVASSEGMENTS_H
#define OOF_CANVASSEGMENTS_H

#include "canvasitem.h"
#include "utility.h"

namespace OOFCanvas {
  class CanvasSegments : public CanvasItem {
  protected:
    double width;
    Color color;
    std::vector<Segment> segments;
    virtual void drawItem(Cairo::RefPtr<Cairo::Context>) const;
  public:
    CanvasSegments() {}
    CanvasSegments(int n);
    void addSegment(double x0, double y0, double x1, double y1);
    void setLineWidth(double);
    void setLineColor(double, double, double);
    void setLineColor(double, double, double, double);
  };
};

#endif // OOF_CANVASSEGMENTS_H
