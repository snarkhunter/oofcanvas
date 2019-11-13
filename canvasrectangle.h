// -*- C++ -*-

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modified
 * versions of this software, you first contact the authors at
 * oof_manager@nist.gov. 
 */

#ifndef OOF_CANVASRECTANGLE_H
#define OOF_CANVASRECTANGLE_H

#include "canvasitem.h"
#include "utility.h"

namespace OOFCanvas {
  class CanvasRectangle : public CanvasItem {
  protected:
    double lineWidth;
    Color lineColor;
    Color fillColor;
    bool fill;			// fill interior 
    bool line;			// draw perimeter
    double xmin, ymin, xmax, ymax;
    virtual void drawItem(Cairo::RefPtr<Cairo::Context>) const;
    virtual bool containsPoint(const Coord&) const;
  public:
    CanvasRectangle(double xmin, double ymin, double xmax, double ymax);
    // Calling setLineWidth or setLineColor sets line=True
    void setLineWidth(double);
    void setLineColor(double, double, double); // RGB
    void setLineColor(double, double, double, double); // RGBA
    // Calling setFillColor sets fill=True
    void setFillColor(double, double, double); // RGB
    void setFillColor(double, double, double, double); // RGBA
  };
  
};

#endif // OOF_CANVASRECTANGLE_H
