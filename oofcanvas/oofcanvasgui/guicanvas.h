// -*- C++ -*-

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modified
 * versions of this software, you first contact the authors at
 * oof_manager@nist.gov. 
 */

// Publicly accessible wrapper for the GUI version of the Canvas.

#ifndef OOFCANVAS_GUICANVAS_H
#define OOFCANVAS_GUICANVAS_H

#include "oofcanvas/canvas.h"
#include <gtk/gtk.h>

namespace OOFCanvas {

  class CanvasImpl;
  class Coord;
  class Rectangle;
  class RubberBand;

  // MotionAllowed indicates the circumstances in which mouse motion
  // events will be accepted.
  enum class MotionAllowed {
		      NEVER,
		      ALWAYS,
		      MOUSEDOWN
  };

  typedef void (*MouseCallback)(const std::string&, const Coord&,
				int, bool, bool, void*);
  typedef void (*ResizeCallback)(void*);
  

  class Canvas : public OffScreenCanvas {
  private:
    CanvasImpl *guiCanvasImpl;
  public:
    Canvas(double ppu);
    virtual ~Canvas() {}

    //=\\=//
    // Methods from GUICanvasImpl
    
    // widgetWidth and widgetHeight return the size of the widget,
    // in pixels.
    int widgetWidth() const;
    int widgetHeight() const;

    void zoom(double);
    void zoomAbout(const Coord&, double factor);
    void zoomAbout(const Coord*, double factor); // for python
    void zoomToFill();
    void center();

    Rectangle visibleRegion() const;

    void removeMouseCallback();
    MotionAllowed allowMotionEvents(MotionAllowed ma);

    void show();		// make gtk widgets visible
    void draw();		// draws all modified layers
    
    GtkAdjustment *getHAdjustment() const;
    GtkAdjustment *getVAdjustment() const;

    void setRubberBand(RubberBand*);
    void removeRubberBand();

    //=\\=//
    // Methods from CanvasImpl

    void destroy();
    void setMouseCallback(MouseCallback, void*);
    void setResizeCallback(ResizeCallback, void*);

    GtkWidget *gtk() const;
    
  };

}; 				// namespace OOFCanvas

#endif // OOFCANVAS_GUICANVAS_H
