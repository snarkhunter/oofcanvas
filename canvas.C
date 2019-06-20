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
#include <pygtk/pygtk.h>
#include <iostream>

namespace OOFCanvas {

  void initializePyGTK() {
    static bool initialized = false;
    if(!initialized) {
      initialized = true;
      gtk_init(0, nullptr);
      PyGILState_STATE pystate = PyGILState_Ensure();
      try {
	init_pygobject();
	init_pygtk();
      }
      catch (...) {
	PyGILState_Release(pystate);
	throw;
      }
      PyGILState_Release(pystate);
    }
  }

  //=\\=//
  
  Canvas::Canvas(int pixelwidth, int pixelheight,
		 double width, double height)
    : pixelwidth(pixelwidth),
      pixelheight(pixelheight),
      width(width),
      height(height)
  {
    // Create a GtkDrawingArea.
    drawing_area = gtk_drawing_area_new();
    gtk_widget_set_size_request(drawing_area, pixelwidth, pixelheight);
    
    // Create a cairo context for the drawing area.
    cairo_t *cctxt = gdk_cairo_create(drawing_area->window);
    // Convert it to a cairomm Context
    ctxt = Cairo::RefPtr<Cairo::Context>(new Cairo::Context(cctxt));

    gtk_widget_set_events(drawing_area,
			  GDK_EXPOSURE_MASK |
			  GDK_BUTTON_PRESS_MASK);

    // TODO: Register callbacks for widget destruction, resize, expose, etc.
    expose_handler = g_signal_connect(
    				 G_OBJECT(drawing_area),
    				 "expose_event",
    				 G_CALLBACK(Canvas::exposeCB),
    				 this);
    config_handler = g_signal_connect(
				 G_OBJECT(drawing_area),
				 "configure_event",
				 G_CALLBACK(Canvas::configCB),
				 this);
    button_handler = g_signal_connect(
			         G_OBJECT(drawing_area),
				 "button_press_event",
				 G_CALLBACK(Canvas::buttonCB),
				 this);
  }

  //=\\=//
  
  
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

  //=\\=//
  
  Canvas::~Canvas() {
    // TODO: Do we need to destroy the drawing_area, or has wrapping
    // it as a PyGTK widget taken care of that?
    
    // TODO: Do we need to disconnect the signal handlers?  Doing so
    // raises an error in a simple test program, but that program is
    // only destroying the Canvas when it's shutting down.
    // g_signal_handler_disconnect(G_OBJECT(drawing_area), config_handler);
    // g_signal_handler_disconnect(G_OBJECT(drawing_area), button_handler);
    // g_signal_handler_disconnect(G_OBJECT(drawing_area), expose_handler);
  }

  //=\\=//
  
  void Canvas::draw() {
    gtk_widget_queue_draw(drawing_area);
  }

  void Canvas::show() {
    gtk_widget_show(drawing_area);
  }

  //=\\=//
  
  void Canvas::configCB(GtkWidget*, GdkEvent *event, gpointer data) {
    ((Canvas*) data)->config(event);
  };

  void Canvas::config(GdkEvent *event) {
    ;
  }

  void Canvas::exposeCB(GtkWidget *widget, GdkEventExpose *event, gpointer data)
  {
    ((Canvas*) data)->expose(widget, event);
  }

  void Canvas::expose(GtkWidget *widget, GdkEventExpose *event) {
    cairo_t *ct = gdk_cairo_create(gtk_widget_get_window(widget));
    assert(ct != nullptr);
    Cairo::RefPtr<Cairo::Context> ctxt(new Cairo::Context(ct, true));
    // Set the clipping region to the exposed area
    ctxt->rectangle(event->area.x, event->area.y,
		    event->area.width, event->area.height);;
    ctxt->clip();
    
    // Fill with a random color
    double r = random()/2147483647.;
    double g = random()/2147483647.;
    double b = random()/2147483647.;
    ctxt->set_source_rgb(r, g, b);
    ctxt->paint();
  }

  void Canvas::buttonCB(GtkWidget*, GdkEvent *event, gpointer data) {
    ((Canvas*) data)->button(event);
  }

  void Canvas::button(GdkEvent *event) {
    ;
  }
  
};				// namespace OOFCanvas

