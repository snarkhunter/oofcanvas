// -*- C++ -*-

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modified
 * versions of this software, you first contact the authors at
 * oof_manager@nist.gov. 
 */

#include "canvas.h"
#include "canvasrectangle.h"
#include <iostream>

namespace OOFCanvas {
  
    CanvasRectangle::CanvasRectangle(double xmin, double ymin,
				     double xmax, double ymax)
    : xmin(xmin), ymin(ymin),
      xmax(xmax), ymax(ymax),
      fill(false),
      line(false)
  {
    setBBox(xmin, ymin, xmax, ymax);
  }

  void CanvasRectangle::setLineWidth(double w) {
    lineWidth = w;
    line = true;
    double halfw = w/2.;
    setBBox(xmin-halfw, ymin-halfw, xmax+halfw, ymax+halfw);
  }

  void CanvasRectangle::setLineColor(double r, double g, double b) {
    lineColor = Color(r, g, b);
    line = true;
  }

  void CanvasRectangle::setLineColor(double r, double g, double b, double a) {
    lineColor = Color(r, g, b, a);
    line = true;
  }

  void CanvasRectangle::setFillColor(double r, double g, double b) {
    fillColor = Color(r, g, b);
    fill = true;
  }

  void CanvasRectangle::setFillColor(double r, double g, double b, double a) {
    fillColor = Color(r, g, b, a);
    fill = true;
  }

  void CanvasRectangle::drawItem(Cairo::RefPtr<Cairo::Context> ctxt) const {
    std::cerr << "CanvasRectangle::draw: " << xmin << " " << ymin << " "
	      << xmax << " " << ymax
	      << " fill=" << fill << " line=" << line
	      << std::endl;
    ctxt->set_line_width(lineWidth);
    ctxt->move_to(xmin, ymin);
    ctxt->line_to(xmax, ymin);
    ctxt->line_to(xmax, ymax);
    ctxt->line_to(xmin, ymax);
    ctxt->close_path();

    if(fill && line) {
      fillColor.set(ctxt);
      ctxt->fill_preserve();
      lineColor.set(ctxt);
      ctxt->stroke();
    }
    else if(fill) {
      fillColor.set(ctxt);
      ctxt->fill();
    }
    else if(line) {
      lineColor.set(ctxt);
      ctxt->stroke();
    }
  }

};				// namespace OOFCanvas
