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

#include <gdk/gdk.h>
#include <pygobject.h>
//#include <pygtk/pygtk.h>
#include <iostream>

namespace OOFCanvas {

  void initializePyGTK() {
    static bool initialized = false;
    if(!initialized) {
      initialized = true;
      gtk_init(0, nullptr);
      PyGILState_STATE pystate = PyGILState_Ensure();
      try {
	if(!pygobject_init(-1, -1, -1))
	  throw "Cannot initialize pygobject!";
      }
      catch (...) {
	PyGILState_Release(pystate);
	throw;
      }
      PyGILState_Release(pystate);
    }
  }

  //=\\=//

  // TODO: There should be a constructor that doesn't take a
  // PyObject*, but instead creates the layout or layout
  // internally, for use from C.  Maybe there should be two Canvas
  // classes, so that we don't have to define both the C++ and Python
  // callbacks in one class.

  // TODO: Do we really need to store pyCanvas?

  // TODO: Get pixelwidth, pixelheight from layout.  Don't use args.
  
  Canvas::Canvas(PyObject *pycan, double ppu)
    : pyCanvas(pycan),
      backingLayer(nullptr),
      ppu(ppu),			// pixels per unit
      bgColor(1.0, 1.0, 1.0),
      redrawNeeded(false),
      mouseCallback(nullptr),
      mouseCallbackData(nullptr),
      pyMouseCallback(nullptr),
      pyMouseCallbackData(Py_None),
      allowMotion(false),
      lastButton(0)
  {
    // pixelwidth, pixelheight, and offset are set in realizeHandler.
    // They can't be initialized here because they depend on the
    // window size, which isn't known yet.
    
    PyGILState_STATE pystate = PyGILState_Ensure();
    try {
      // The initial value of the data to be passed to the python mouse
      // callback is None. Since we're storing it, we need to incref it.
      Py_INCREF(pyMouseCallbackData);
      Py_INCREF(pyCanvas);
    

      // Extract the GtkLayout from the passed-in PyObject*, which
      // is a Gtk.Layout.
      PyObject *capsule = PyObject_GetAttrString(pyCanvas, "__gpointer__");
      if(!PyCapsule_CheckExact(capsule)) {
	throw "capsule is not a PyCapsule!";
      }
      const char *capsuleName = PyCapsule_GetName(capsule);
      if(!PyCapsule_IsValid(capsule, capsuleName)) {
	throw "pyCanvas is not a valid pyCapsule!";
      }
      layout = (GtkWidget*) PyCapsule_GetPointer(capsule, capsuleName);
      g_object_ref(layout);
      Py_DECREF(capsule);
    }
    catch(...) {
      PyGILState_Release(pystate);
      throw;
    }
    PyGILState_Release(pystate);

    gtk_widget_set_events(layout,
			  GDK_EXPOSURE_MASK |
			  GDK_BUTTON_PRESS_MASK |
			  GDK_BUTTON_RELEASE_MASK |
			  GDK_POINTER_MOTION_MASK);

    g_signal_connect(G_OBJECT(layout), "realize",
		     G_CALLBACK(Canvas::realizeCB), this);

    // TODO: Register callbacks for widget destruction, resize, etc.
    // config_handler = g_signal_connect(G_OBJECT(layout),
    // 				      "configure_event",
    // 				      G_CALLBACK(Canvas::configCB),
    // 				      this);
    button_down_handler = g_signal_connect(G_OBJECT(layout),
					   "button_press_event",
					   G_CALLBACK(Canvas::buttonCB),
					   this);
    button_up_handler = g_signal_connect(G_OBJECT(layout),
					 "button_release_event",
					 G_CALLBACK(Canvas::buttonCB),
					 this);
    motion_handler = g_signal_connect(G_OBJECT(layout),
				      "motion_notify_event",
				      G_CALLBACK(Canvas::motionCB),
				      this);
    draw_handler = g_signal_connect(G_OBJECT(layout),
    				    "draw",
    				    G_CALLBACK(Canvas::drawCB), this);
  }
  
  void Canvas::destroy() {
    if(backingLayer)
      delete backingLayer;
    for(CanvasLayer *layer : layers)
      delete layer;
    layers.clear();
    gtk_widget_destroy(layout);
    Py_DECREF(pyCanvas);
    g_object_unref(layout);
  }
  
  Canvas::~Canvas() {
    destroy();
    
    // TODO: Do we need to disconnect the signal handlers?  Doing so
    // raises an error in a simple test program, but that program is
    // only destroying the Canvas when it's shutting down.
    // g_signal_handler_disconnect(G_OBJECT(layout), config_handler);
    // g_signal_handler_disconnect(G_OBJECT(layout), button_handler);
  }

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
    // This generates an draw event on the drawing area, which
    // causes Canvas::drawCB to be called.
    gtk_widget_queue_draw(layout);
  }

  void Canvas::setBackgroundColor(double r, double g, double b) {
    bgColor = Color(r, g, b);
  }

  void Canvas::show() {
    gtk_widget_show(layout);
  }

  //=\\=//

  void Canvas::setTransform(double scale, const Coord &off) {
    ppu = scale;
    offset = off;
    transform = Cairo::Matrix(scale, 0, 0, -scale,
    			      offset.x, offset.y);
    redrawNeeded = true;
  }
  
  void Canvas::setPixelsPerUnit(double scale) {
    setTransform(scale, offset);
    draw();
  }

  void Canvas::zoom(double factor) {
    setTransform(ppu * factor, offset);
    draw();
  }
  
  void Canvas::translate(double dx, double dy) {
    // -dy because in physics y goes up
    setTransform(ppu, offset + ppu*Coord(dx, -dy));
  }

  ICoord Canvas::user2pixel(const Coord &pt) const {
    assert(backingLayer != nullptr);
    return backingLayer->user2pixel(pt);
  }

  Coord Canvas::pixel2user(const ICoord &pt) const {
    assert(backingLayer != nullptr);
    return backingLayer->pixel2user(pt);
  }

  double Canvas::user2pixel(double d) const {
    assert(backingLayer != nullptr);
    return backingLayer->user2pixel(d);
  }

  double Canvas::pixel2user(double d) const {
    assert(backingLayer != nullptr);
    return backingLayer->pixel2user(d);
  }

  int Canvas::heightInPixels() const {
    return gtk_widget_get_allocated_height(layout);
  }

  int Canvas::widthInPixels() const {
    return gtk_widget_get_allocated_width(layout);
  }

  //=\\=//

  void Canvas::realizeCB(GtkWidget*, gpointer data) {
    ((Canvas*) data)->realizeHandler();
  }

  void Canvas::realizeHandler() {
    // Set the initial values of ppu and offset
    setTransform(ppu, Coord(0.0, heightInPixels()));

    GdkWindow *bin_window = gtk_layout_get_bin_window(GTK_LAYOUT(layout));
    GdkEventMask events = gdk_window_get_events(bin_window);
    gdk_window_set_events(bin_window,
			  (GdkEventMask) (gdk_window_get_events(bin_window)
					  | GDK_EXPOSURE_MASK
					  | GDK_BUTTON_PRESS_MASK
					  | GDK_BUTTON_RELEASE_MASK
					  | GDK_POINTER_MOTION_MASK
					  | GDK_KEY_PRESS_MASK
					  | GDK_KEY_RELEASE_MASK
					  | GDK_ENTER_NOTIFY_MASK
					  | GDK_LEAVE_NOTIFY_MASK
					  | GDK_FOCUS_CHANGE_MASK
					  ));
    // The backingLayer is a CanvasLayer that exists just so that the
    // canvas transforms can be defined even when nothing is
    // drawn. This allows pixel2user and user2pixel to work in all
    // circumstances. The backingLayer can't be created until the
    // layout is created, however, because it needs to know the window
    // size.  So it's done here instead of in the Canvas constructor.
    backingLayer = new CanvasLayer(this);
    backingLayer->setClickable(false);

    // g_signal_connect(G_OBJECT(bin_window), "configure_event",
    // 		     G_CALLBACK(Canvas::configCB), this);
  }
  
  //=\\=//

  void Canvas::drawCB(GtkWidget*, Cairo::Context::cobject *ctxt, gpointer data)
  {
    ((Canvas*) data)->drawHandler(
	  Cairo::RefPtr<Cairo::Context>(new Cairo::Context(ctxt, false)));
  }

  void Canvas::drawHandler(Cairo::RefPtr<Cairo::Context> context) {
    // From the gtk2->gtk3 conversion notes: "The cairo context is
    // being set up so that the origin at (0, 0) coincides with the
    // upper left corner of the widget, and is properly clipped."
    // (https://developer.gnome.org/gtk3/stable/ch26s02.html)
    
    // TODO: This used to be done in the expose event handler.  Does
    // it need to be done anywhere, or is the clipping region already
    // set when the context is passed to the draw callback?
    // // Set the clipping region to the exposed area
    // double x = event->area.x;
    // double y = event->area.y;
    // double width = event->area.width;
    // double height = event->area.height;
    // context->device_to_user(x, y);
    // context->device_to_user_distance(width, height);
    // context->rectangle(x, y, width, height);
    // context->reset_clip();
    // context->clip();

    context->set_source_rgb(bgColor.red, bgColor.green, bgColor.blue);
    context->paint();

    if(redrawNeeded) {
      // Force every layer to redraw at new size
      for(CanvasLayer *layer: layers) {
	layer->redraw();	// clear and draw to layer surface
	layer->draw(context);	// copy layer surface to canvas surface
      }
      backingLayer->redraw(); 
      redrawNeeded = false;
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
  }

  //=\\=//

  // configCB isn't being called for some reason... Do we even need it?

  // void Canvas::configCB(GtkWidget*, GdkEventConfigure *event, gpointer data) {
  //   ((Canvas*) data)->configHandler(event);
  // };

  // void Canvas::configHandler(GdkEventConfigure *event) {
  //   std::cerr << "Canvas::configHandler" << std::endl;
  //   int xchange = widthInPixels() - pixelwidth;
  //   int ychange = heightInPixels() - pixelheight;

  //   double scalefactor = widthInPixels()/double(pixelwidth);
  //   pixelwidth = widthInPixels();
  //   pixelheight = heightInPixels();
  //   offset += Coord(0, ychange);

  //   std::cerr << "Canvas::configHandler: pixelsize= " << pixelwidth
  // 	      << ", " << pixelheight << std::endl;
  //   setTransform(ppu*scalefactor, offset);
  // }

  //=\\=//

  void Canvas::buttonCB(GtkWidget*, GdkEventButton *event, gpointer data) {
    ((Canvas*) data)->mouseButtonHandler(event);
  }

  void Canvas::mouseButtonHandler(GdkEventButton *event) {
    std::string eventtype;
    if(event->type == GDK_BUTTON_PRESS)
      eventtype = "down";
    else
      eventtype = "up";
    lastButton = event->button;

    doCallback(eventtype, event->x, event->y, lastButton,
	       event->state & GDK_SHIFT_MASK,   
	       event->state & GDK_CONTROL_MASK);
    allowMotion = (eventtype == "down");
  }

  void Canvas::motionCB(GtkWidget*, GdkEventMotion *event, gpointer data) {
    ((Canvas*) data)->mouseMotionHandler(event);
  }

  void Canvas::mouseMotionHandler(GdkEventMotion *event) {
    if(!allowMotion)
      return;
    doCallback("move", event->x, event->y, lastButton,
	       event->state & GDK_SHIFT_MASK,
	       event->state & GDK_CONTROL_MASK);
  }

  // Common code shared by all mouse callbacks
  void Canvas::doCallback(const std::string &eventtype,
			  int x, int y, int button, bool shift, bool ctrl)
    const
  {
    ICoord pixel(x, y);
    Coord userpt(pixel2user(pixel));

    if(mouseCallback != nullptr)
      (*mouseCallback)(eventtype, userpt, button, shift, ctrl);
    else if(pyMouseCallback != nullptr) {
      PyGILState_STATE pystate = PyGILState_Ensure();
      try {
	PyObject *args = Py_BuildValue("sddiiiO", eventtype.c_str(),
				       userpt.x, userpt.y,
				       button, shift, ctrl,
				       pyMouseCallbackData);
	PyObject *result = PyObject_CallObject(pyMouseCallback, args);
	if(result == nullptr) {
	  PyErr_Print();
	  PyErr_Clear();
	}
	Py_XDECREF(args);
	Py_XDECREF(result);
      }
      catch (...) {
	PyGILState_Release(pystate);
	throw;
      }
      PyGILState_Release(pystate);
    }

  }

  void Canvas::setMouseCallback(MouseCallback *mcb, void *data) {
    if(pyMouseCallback || pyMouseCallbackData) {
      PyGILState_STATE pystate = PyGILState_Ensure();
      try {
	if(pyMouseCallback) {
	  Py_DECREF(pyMouseCallback);
	  pyMouseCallback = nullptr;
	}
	if(pyMouseCallbackData) {
	  Py_DECREF(pyMouseCallbackData);
	  pyMouseCallbackData = nullptr;
	}
      }
      catch(...) {
	PyGILState_Release(pystate);
	throw;
      }
      PyGILState_Release(pystate);
    }
    mouseCallback = mcb;
    mouseCallbackData = data;
  }

  void Canvas::setPyMouseCallback(PyObject *pymcb, PyObject *pydata) {
    PyGILState_STATE pystate = PyGILState_Ensure();
    try {
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
    catch (...) {
      PyGILState_Release(pystate);
      throw;
    }
    PyGILState_Release(pystate);
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


