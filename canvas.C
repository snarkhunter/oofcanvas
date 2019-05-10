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
#include <cairo.h>
#include <gdk/gdk.h>
#include <pygobject.h>

namespace OOFCanvas {

  Canvas::Canvas(int pixelwidth, int pixelheight,
		 double width, double height)
    : pixelwidth(pixelwidth),
      pixelheight(pixelheight),
      width(width),
      height(height)
  {
    // Create a GtkDrawingArea.
    drawing_area = gtk_drawing_area_new();
    // Create a cairo context for the drawing area.
    cairo_t *cctxt = gdk_cairo_create(drawing_area->window);
    // Convert it to a cairomm Context
    ctxt = Cairo::RefPtr<Cairo::Context>(new Cairo::Context(cctxt));

    // TODO: Register callbacks for widget destruction, resize, expose, etc.
  }

  PyObject *Canvas::widget() {
    PyObject *wdgt;
    PyGILState_STATE pystate = PyGILState_Ensure();
    try {
      wdgt = pygobject_new((GObject*) drawing_area);
    }
    catch (...) {
      PyGILState_Release(pystate);
      throw;
    }
    PyGILState_Release(pystate);
    return wdgt;
  }

  Canvas::~Canvas() {
    // TODO: Do we need to destroy the drawing_area, or has wrapping
    // it as a PyGTK widget taken care of that?
  }

  void Canvas::draw() {
    // Just a dummy for now
    ctxt->set_source_rgb(1.0, 0.4, 0.0);
    ctxt->paint();
  }
  
};				// namespace OOFCanvas

