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
  // PyObject*, but instead creates the layout internally, for use
  // from C.  Maybe there should be two Canvas classes, so that we
  // don't have to define both the C++ and Python callbacks in one
  // class.

  // TODO: Do we really need to store pyCanvas?

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
	throw "Canvas constructor: capsule is not a PyCapsule!";
      }
      const char *capsuleName = PyCapsule_GetName(capsule);
      if(!PyCapsule_IsValid(capsule, capsuleName)) {
	throw "Canvas constructor: capsule is not a valid pyCapsule!";
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
			  GDK_EXPOSURE_MASK
			  | GDK_BUTTON_PRESS_MASK
			  | GDK_BUTTON_RELEASE_MASK
			  | GDK_POINTER_MOTION_MASK
			  );

    g_signal_connect(G_OBJECT(layout), "realize",
		     G_CALLBACK(Canvas::realizeCB), this);
    g_signal_connect(G_OBJECT(layout), "size-allocate",
		     G_CALLBACK(Canvas::allocateCB), this);

    // TODO: Register callbacks for widget destruction, resize, etc.

    // TODO: Why do we get so many apparent motion events?
    // TODO: Should these be connected to the bin_window instead?
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

    g_signal_connect(G_OBJECT(getHAdjustment()),
		     "value-changed",
		     G_CALLBACK(Canvas::hScrollValueChangedCB),
		     this);
    g_signal_connect(G_OBJECT(getVAdjustment()),
		     "value-changed",
		     G_CALLBACK(Canvas::vScrollValueChangedCB),
		     this);
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

  bool Canvas::empty() const {
    for(const CanvasLayer* layer : layers)
      if(!layer->empty())
	return false;
    return true;
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
    bool newppu = ppu != scale;
    ppu = scale;
    offset = off;  // position of the user space origin in pixel space
    transform = Cairo::Matrix(ppu, 0, 0, -ppu, offset.x, offset.y);
    if(newppu && boundingBox.initialized()) {
      gtk_layout_set_size(GTK_LAYOUT(layout),
			  (guint) (ppu*boundingBox.width()),
			  (guint) (ppu*boundingBox.height()));
      guint w, h;
      gtk_layout_get_size(GTK_LAYOUT(layout), &w, &h);

    }
    redrawNeeded = true;
  }

  void Canvas::setPixelsPerUnit(double scale) {
    setTransform(scale, offset);
    draw();
  }

  void Canvas::translate(double dx, double dy) {
    // -dy because in physics y goes up
    setTransform(ppu, offset + ppu*Coord(dx, -dy));
  }

  void Canvas::fill() {
    double ppu_x = widthInPixels()/boundingBox.width();
    double ppu_y = heightInPixels()/boundingBox.height();
    double new_ppu = ppu_x < ppu_y ? ppu_x : ppu_y; 
    setTransform(new_ppu,
		 Coord(0, heightInPixels()) +
		 new_ppu*Coord(-boundingBox.xmin(), boundingBox.ymin()));
    draw();
  }

  /**
     There are TWO offsets.  One is the offset in the Cairo::Matrix
     that translates user coordinates to pixel coordinates *on the
     bin_window*.  The other is the offset that translates the
     bin_window to the Layout widget.  It's the second one that the
     scroll bars control.  The scroll bars should not affect any Cairo
     parameters at all.

     BUT BUT BUT why is it then necessary to call setTransform in the
     ScrollValueChanged callbacks?  If we don't do that and change the
     Cairo::Matrix, the image doesn't scroll.  That probably has to do
     with the Cairo::Context that's sent to Canvas::draw, which has
     shifted the origin to the upper left corner of the widget, not
     the upper left corner of the bin_window.

     The scroll adjustments give the offset (in pixels, presumably)
     between the upper left corner of the visible region and the upper
     left corner of the underlying bitmap (bin_window).
  **/
    
  void Canvas::zoom(double factor) {
    // Zoom by factor while keeping the center of the image fixed.
    // The visible window size is fixed, but the virtual window isn't.

    // TODO: The scroll bars are incorrectly set after applying this
    // transformation.  They fix themselves after they're used once.
    Coord uCenter = pixel2user(ICoord(widthInPixels()/2,
				      heightInPixels()/2));
    uCenter.y *= -1;
    setTransform(factor*ppu, offset+ppu*(1-factor)*uCenter);
    draw();
  }


  void Canvas::updateBoundingBox(const Rectangle &rect) {
    boundingBox.swallow(rect);
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

  int Canvas::binWindowWidth() const {
    GdkWindow *bin_window = gtk_layout_get_bin_window(GTK_LAYOUT(layout));
    return gdk_window_get_width(bin_window);
  }

    int Canvas::binWindowHeight() const {
    GdkWindow *bin_window = gtk_layout_get_bin_window(GTK_LAYOUT(layout));
    return gdk_window_get_height(bin_window);
  }

  GtkAdjustment *Canvas::getHAdjustment() const {
    return gtk_scrollable_get_hadjustment(GTK_SCROLLABLE(layout));
  }

  GtkAdjustment *Canvas::getVAdjustment() const {
    return gtk_scrollable_get_vadjustment(GTK_SCROLLABLE(layout));
  }

  //=\\=//

  // realizeCB is called once, when the Canvas's gtk object is
  // "realized", whatever that means.  It's not as if the Canvas has
  // any existence other than as a pattern of bits.

  void Canvas::realizeCB(GtkWidget*, gpointer data) {
    ((Canvas*) data)->realizeHandler();
  }

  void Canvas::realizeHandler() {
    // Set the initial size of the virtual window to be the same as
    // the size the actual window.
    std::cerr << "Canvas::realizeHandler" << std::endl;
    gtk_layout_set_size(GTK_LAYOUT(layout), widthInPixels(), heightInPixels());
    
    // Set the initial values of ppu and offset
    setTransform(ppu, Coord(0.0, heightInPixels()));

    GdkWindow *bin_window = gtk_layout_get_bin_window(GTK_LAYOUT(layout));
    GdkEventMask events = gdk_window_get_events(bin_window);

    // TODO: Figure out which events need to be handled on the
    // bin_window and which on the Layout.
    gdk_window_set_events(bin_window,
			  (GdkEventMask) (gdk_window_get_events(bin_window)
					  | GDK_EXPOSURE_MASK
					  | GDK_BUTTON_PRESS_MASK
					  | GDK_BUTTON_RELEASE_MASK
					  // | GDK_POINTER_MOTION_MASK
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
  }

  //=\\=//

  void Canvas::allocateCB(GtkWidget*, GdkRectangle*, gpointer data) {
    ((Canvas*) data)->allocateHandler();
  }

  void Canvas::allocateHandler() {
    // std::cerr << "Canvas::allocateHandler: backingLayer="
    // 	      << backingLayer
    // 	      << " w=" << widthInPixels() << " h=" << heightInPixels()
    // 	      << std::endl;
    if(backingLayer)
      backingLayer->clear();	// forces it to resize itself
  }
  
  //=\\=//

  void Canvas::hScrollValueChangedCB(GtkAdjustment *adj, gpointer data) {
    ((Canvas*) data)->hScrollValueChanged(adj);
  }

  void Canvas::vScrollValueChangedCB(GtkAdjustment *adj, gpointer data) {
    ((Canvas*) data)->vScrollValueChanged(adj);
  }
  
  void Canvas::hScrollValueChanged(GtkAdjustment *adj) {
    // Set the offset according to the value of the gtk adjustments.
    double xoff = -gtk_adjustment_get_value(adj);
    double yoff = offset.y;
    setTransform(ppu, Coord(xoff, yoff));
    // std::cerr << "Canvas::scrollValueChanged: H"
    // 	      << " lower=" << gtk_adjustment_get_lower(adj)
    // 	      << " upper=" << gtk_adjustment_get_upper(adj)
    // 	      << " val=" << gtk_adjustment_get_value(adj)
    // 	      << " offset=" << offset
    // 	      << std::endl;
    draw();
  }

  void Canvas::vScrollValueChanged(GtkAdjustment *adj) {
    double xoff = offset.x;
    double yoff = gtk_adjustment_get_upper(adj)-gtk_adjustment_get_value(adj);
    setTransform(ppu, Coord(xoff, yoff));
    // std::cerr << "Canvas::scrollValueChanged: V"
    // 	      << " lower=" << gtk_adjustment_get_lower(adj)
    // 	      << " upper=" << gtk_adjustment_get_upper(adj)
    // 	      << " val=" << gtk_adjustment_get_value(adj)
    // 	      << " offset=" << offset
    // 	      << std::endl;
    draw();
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

    // TODO? Extract the clipping region from the context using
    // Cairo::Context::get_clip_extents, and only redraw CanvasItems
    // whose bounding boxes intersect the clipping region.  It seems
    // that the whole Layout is included in the clip region, even if
    // only a small part is newly exposed. 
    // Rectangle clip_extents;
    // context->get_clip_extents(clip_extents.xmin(), clip_extents.ymin(),
    // 			      clip_extents.xmax(), clip_extents.ymax());
    // std::cerr << "Canvas::drawHandler: clip_extents=" << clip_extents
    // 	      << std::endl;
    
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


