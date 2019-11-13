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
    : backingLayer(nullptr),
      ppu(1.0),			// pixels per unit
      pixelwidth(pixelwidth),
      pixelheight(pixelheight),
      width(pixelwidth),	// because ppu==1
      height(pixelheight),
      offset(0., 0.),
      bgColor(1.0, 1.0, 1.0),
      initialized(false),
      mouseCallback(nullptr),
      mouseCallbackData(nullptr),
      pyMouseCallback(nullptr),
      pyMouseCallbackData(Py_None),
      allowMotion(false)
  {
    // The initial value of the data to be passed to the python mouse
    // callback is None. Since we're storing it, we need to incref it.
    Py_INCREF(pyMouseCallbackData);
    
    // Create a GtkDrawingArea.
    drawing_area = gtk_drawing_area_new();
    gtk_widget_set_size_request(drawing_area, pixelwidth, pixelheight);

    gtk_widget_set_events(drawing_area,
			  GDK_EXPOSURE_MASK |
			  GDK_BUTTON_PRESS_MASK |
			  GDK_BUTTON_RELEASE_MASK |
			  GDK_POINTER_MOTION_MASK);

    // TODO: Register callbacks for widget destruction, resize, expose, etc.
    expose_handler = g_signal_connect(G_OBJECT(drawing_area),
				      "expose_event",
				      G_CALLBACK(Canvas::exposeCB),
				      this);
    config_handler = g_signal_connect(G_OBJECT(drawing_area),
				      "configure_event",
				      G_CALLBACK(Canvas::configCB),
				      this);
    button_down_handler = g_signal_connect(G_OBJECT(drawing_area),
					   "button_press_event",
					   G_CALLBACK(Canvas::buttonCB),
					   this);
    button_up_handler = g_signal_connect(G_OBJECT(drawing_area),
					 "button_release_event",
					 G_CALLBACK(Canvas::buttonCB),
					 this);
    motion_handler = g_signal_connect(G_OBJECT(drawing_area),
				      "motion_notify_event",
				      G_CALLBACK(Canvas::buttonCB),
				      this);
  }
  
  void Canvas::destroy() {
    if(backingLayer)
      delete backingLayer;
    for(CanvasLayer *layer : layers)
      delete layer;
    layers.clear();
    gtk_widget_destroy(drawing_area);
  }
  
  Canvas::~Canvas() {
    destroy();
    
    // TODO: Do we need to disconnect the signal handlers?  Doing so
    // raises an error in a simple test program, but that program is
    // only destroying the Canvas when it's shutting down.
    // g_signal_handler_disconnect(G_OBJECT(drawing_area), config_handler);
    // g_signal_handler_disconnect(G_OBJECT(drawing_area), button_handler);
    // g_signal_handler_disconnect(G_OBJECT(drawing_area), expose_handler);
  }

  //=\\=//

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

  ICoord Canvas::user2pixel(const Coord &pt) const {
    return backingLayer->user2pixel(pt);
  }

  Coord Canvas::pixel2user(const ICoord &pt) const {
     return backingLayer->pixel2user(pt);
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
    // The backingLayer is a CanvasLayer that exists just so that the
    // canvas transforms can defined even when nothing is drawn. This
    // allows pixel2user and user2pixel to work in all
    // circumstances. The backingLayer can't be created until the
    // drawing_area is created, however, because it needs to know the
    // window size.  So it's done here, at the first configure event.
    if(backingLayer == nullptr) {
      backingLayer = new CanvasLayer(this);
      backingLayer->setClickable(false);
    }
  }

  void Canvas::exposeCB(GtkWidget *widget, GdkEventExpose *event, gpointer data)
  {
    ((Canvas*) data)->expose(widget, event);
  }

  void Canvas::expose(GtkWidget *widget, GdkEventExpose *event) {
    // Has the size of the window changed?
    bool sizechanged = !initialized || (pixelwidth != widthInPixels() ||
					pixelheight != heightInPixels());
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
	layer->draw(context);
      }
    }
    
  } // end Canvas::expose

  void Canvas::buttonCB(GtkWidget*, GdkEventButton *event, gpointer data) {
    ((Canvas*) data)->mouseButton(event);
  }

  void Canvas::mouseButton(GdkEventButton *event) {
    std::string eventtype;
    switch(event->type) {
    case GDK_BUTTON_PRESS:
      eventtype = "down";
      break;
    case GDK_BUTTON_RELEASE:
      eventtype = "up";
      break;
    case GDK_MOTION_NOTIFY:
      if(!allowMotion)
	return;
      eventtype = "move";
      break;
    default:
      return;
    }
    
    ICoord pixel(event->x, event->y);
    Coord userpt(pixel2user(pixel));

    if(mouseCallback != nullptr)
      (*mouseCallback)(eventtype, userpt, event->button, event->state);
    else if(pyMouseCallback != nullptr) {
      // TODO: Get Python interpreter lock
      PyObject *args = Py_BuildValue("sddiiO", eventtype.c_str(),
				     userpt.x, userpt.y,
				     event->button, event->state,
				     pyMouseCallbackData);
      PyObject *result = PyObject_CallObject(pyMouseCallback, args);
      if(result == nullptr) {
	PyErr_Print();
	PyErr_Clear();
      }
      Py_XDECREF(args);
      Py_XDECREF(result);
      // TODO: Release Python interpreter lock
    }
  }

  void Canvas::setMouseCallback(MouseCallback *mcb, void *data) {
    if(pyMouseCallback) {
      Py_DECREF(pyMouseCallback);
      pyMouseCallback = nullptr;
    }
    if(pyMouseCallbackData) {
      Py_DECREF(pyMouseCallbackData);
      pyMouseCallbackData = nullptr;
    }
    mouseCallback = mcb;
    mouseCallbackData = data;
  }

  void Canvas::setPyMouseCallback(PyObject *pymcb, PyObject *pydata) {
    if(pyMouseCallback) {
      Py_DECREF(pyMouseCallback);
      mouseCallback = nullptr;
    }
    if(pyMouseCallbackData) {
      Py_DECREF(pyMouseCallbackData);
    }
    
    pyMouseCallback = pymcb;
    Py_INCREF(pyMouseCallback);
    if(pydata != nullptr) {
      pyMouseCallbackData = pydata;
    }
    else {
      pyMouseCallbackData = Py_None;
    }
    Py_INCREF(pyMouseCallbackData);
  }

  //=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//
  
  std::vector<CanvasItem*> Canvas::clickedItems(double x, double y) const {
    Coord where(x,y);
    std::vector<CanvasItem*> items;
    for(const CanvasLayer *layer : layers)
      if(layer->clickable) 
	layer->clickedItems(where, items);
    return items;
  }

  std::vector<CanvasItem*> Canvas::allItems() const {
    std::vector<CanvasItem*> items;
    for(const CanvasLayer *layer : layers)
      layer->allItems(items);
    return items;
  }

  // The _new versions of clickedItems and allItems return their
  // results in a new vector, because swig works better that way.  If
  // we instead swig the above versions, without using new, swig will
  // make an extra copy of the vectors.
  std::vector<CanvasItem*> *Canvas::clickedItems_new(double x, double y) const {
    Coord where(x,y);
    std::vector<CanvasItem*> *items = new std::vector<CanvasItem*>;
    for(const CanvasLayer *layer : layers) 
      if(layer->clickable)
	layer->clickedItems(where, *items);
    return items;
  }

  std::vector<CanvasItem*> *Canvas::allItems_new() const {
    std::vector<CanvasItem*> *items = new std::vector<CanvasItem*>;
    for(const CanvasLayer *layer : layers)
      layer->allItems(*items);
    return items;
  }
};				// namespace OOFCanvas

