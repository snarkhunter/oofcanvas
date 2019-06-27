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
#include "canvaslayer.h"

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
  
  Canvas::Canvas(int pixelwidth, int pixelheight)
    : ppu(1.0),			// pixels per unit
      pixelwidth(pixelwidth),
      pixelheight(pixelheight),
      width(pixelwidth),	// because ppu==1
      height(pixelheight),
      offset(0., 0.),
      bgColor(1.0, 1.0, 1.0),
      initialized(false)
  {
    // Create a GtkDrawingArea.
    drawing_area = gtk_drawing_area_new();
    gtk_widget_set_size_request(drawing_area, pixelwidth, pixelheight);
    
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
  
  void Canvas::setTransform(double scale, const Coord &off) {
    ppu = scale;
    offset = off;
    transform = Cairo::Matrix(scale, 0, 0, -scale,
			      offset.x, pixelheight-offset.y);
  }
  
  void Canvas::setPixelsPerUnit(double scale) {
    setTransform(scale, offset);
  }
  
  void Canvas::shift(double dx, double dy) {
    // -dy because in physics y goes up
    setTransform(ppu, offset + Coord(dx, -dy));
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
    for(CanvasLayer *layer : layers)
      delete layer;
    
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

  CanvasLayer *Canvas::newLayer() {
    CanvasLayer *layer = new CanvasLayer(this);
    layers.push_back(layer);
    return layer;
  }

  void Canvas::deleteLayer(CanvasLayer *layer) {
    auto iter = std::find(layers.begin(), layers.end(), layer);
    if(iter != layers.end())
      layers.erase(iter);
    delete layer;
  }
  
  void Canvas::draw() {
    // This generates an expose event on the drawing area, which
    // causes Canvas::expose to be called.
    std::cerr << "Canvas::draw" << std::endl;
    gtk_widget_queue_draw(drawing_area);
  }

  void Canvas::setBackgroundColor(double r, double g, double b) {
    bgColor = Color(r, g, b);
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
    // Has the size of the window changed?
    bool sizechanged = !initialized || (pixelwidth != widthInPixels() ||
					pixelheight != heightInPixels());
    std::cerr << "Canvas::expose ********************" << std::endl;
    // std::cerr << "Canvas::expose: pw=" << widthInPixels()
    // 	      << " ph=" << heightInPixels() << " changed=" << sizechanged
    // 	      << std::endl;
    if(sizechanged) {
      pixelwidth = widthInPixels();
      pixelheight = heightInPixels();
      // TODO: Changing ppu when the canvas size changes would scale
      // the canvas contents with the size of the window, but it's not
      // clear what to do when the size scales in x differently than
      // in y.  Is it better not to change ppu at all?
      setTransform(widthInPixels()/width, offset);
      initialized = true;
    }
      
    
    cairo_t *ct = gdk_cairo_create(gtk_widget_get_window(widget));
    Cairo::RefPtr<Cairo::Context> context(new Cairo::Context(ct, true));
    
    // std::cerr << "Canvas::expose: ppu=" << ppu << " offset=" << offset
    // 	      << std::endl;
    
    // Set the clipping region to the exposed area
    double x = event->area.x;
    double y = event->area.y;
    double width = event->area.width;
    double height = event->area.height;
    context->device_to_user(x, y);
    context->device_to_user_distance(width, height);
    context->rectangle(x, y, width, height);
    context->reset_clip();
    context->clip();

    context->set_source_rgb(bgColor.red, bgColor.green, bgColor.blue);
    context->paint();

    if(sizechanged) {
      // Force every layer to redraw at new size
      for(CanvasLayer *layer: layers) {
	std::cerr << "Canvas::expose: redrawing " << layer << std::endl;
	layer->redraw();	// draw to layer's surface
	layer->draw(context);	// copy layer to our surface
      }
    }
    else {
      // If the size hasn't changed, a redraw has been forced because
      // some layer or layers have changed.  They've already rebuilt
      // their own internal Cairo::Surfaces.  All we have to do here
      // is to stack the layers' Surfaces on the Canvas's Surface.
      for(CanvasLayer *layer: layers) {
	std::cerr << "Canvas::expose: drawing " << layer << std::endl;
	layer->draw(context);
      }
    }
    std::cerr << "Canvas::expose: done **************" << std::endl;
    
  } // end Canvas::expose

  void Canvas::buttonCB(GtkWidget*, GdkEventButton *event, gpointer data) {
    ((Canvas*) data)->mouseButton(event);
  }

  void Canvas::mouseButton(GdkEventButton *event) {
    std::cerr << "Canvas::mouseButton: ("
	      << event->x << ", " << event->y << ") "
	      << "state=" << event->state << " button=" << event->button
	      << std::endl;
  }
  
};				// namespace OOFCanvas

