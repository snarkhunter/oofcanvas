// -*- C++ -*-

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modified
 * versions of this software, you first contact the authors at
 * oof_manager@nist.gov. 
 */

#include "oofcanvas/canvasitem.h"
#include "oofcanvas/canvaslayer.h"
#include "oofcanvas/oofcanvasgui/guicanvas.h"
#include "oofcanvas/oofcanvasgui/guicanvasimpl.h"
#include "oofcanvas/oofcanvasgui/rubberband.h"
#include <algorithm>
#include <cassert>
#include <gdk/gdk.h>
#include <iostream>
#include <limits>

#ifdef OOFCANVAS_USE_PYTHON
#include <pygobject.h>
#endif

namespace OOFCanvas {

  GUICanvasImpl::GUICanvasImpl(double ppu)
    : OSCanvasImpl(ppu),
      layout(nullptr),
      allowMotion(MotionAllowed::NEVER),
      lastButton(0),
      buttonDown(false),
      rubberBandLayer(this, "<rubberbandlayer>"),
      rubberBand(nullptr),
      nonRubberBandBufferFilled(false),
      destroyed(false)
  {}

  void GUICanvasImpl::initSignals() {
    // initSignals is called by the derived class constructors after
    // layout is set.

    // We don't (yet) use all of these events, but I don't think it
    // hurts to include them here.  I also haven't been able to figure
    // out which ones need to be explicitly included, and which ones
    // are included by default.  gtk_widget_get_events(layout) returns
    // 0 at this point, but gtk_widget_add_events() works. Go figure.
    require_mainthread(__FILE__, __LINE__);
    gtk_widget_add_events(layout,
			  (GdkEventMask) (GDK_BUTTON_PRESS_MASK |
					  GDK_BUTTON_RELEASE_MASK |
					  GDK_POINTER_MOTION_MASK |
					  GDK_KEY_PRESS_MASK |
					  GDK_KEY_RELEASE_MASK |
					  GDK_ENTER_NOTIFY_MASK |
					  GDK_LEAVE_NOTIFY_MASK |
					  GDK_FOCUS_CHANGE_MASK |
					  GDK_SCROLL_MASK));

    g_signal_connect(G_OBJECT(layout), "realize",
		     G_CALLBACK(GUICanvasImpl::realizeCB), this);
    g_signal_connect(G_OBJECT(layout), "size_allocate",
     		     G_CALLBACK(GUICanvasImpl::allocateCB), this);

    g_signal_connect(G_OBJECT(layout), "button_press_event",
    		     G_CALLBACK(GUICanvasImpl::buttonCB), this);
    g_signal_connect(G_OBJECT(layout), "button_release_event",
    		     G_CALLBACK(GUICanvasImpl::buttonCB), this);
    g_signal_connect(G_OBJECT(layout), "motion_notify_event",
    		     G_CALLBACK(GUICanvasImpl::motionCB), this);
    g_signal_connect(G_OBJECT(layout), "draw",
    		     G_CALLBACK(GUICanvasImpl::drawCB), this);
    g_signal_connect(G_OBJECT(layout), "scroll_event",
		     G_CALLBACK(GUICanvasImpl::scrollCB), this);
    g_signal_connect(G_OBJECT(layout), "destroy",
		     G_CALLBACK(GUICanvasImpl::destroyCB), this);

  }
  
  void GUICanvasImpl::show() {
    require_mainthread(__FILE__, __LINE__);
    gtk_widget_show(layout);
  }

  static gboolean queue_draw(void *data) {
    require_mainthread(__FILE__, __LINE__);
    gtk_widget_queue_draw((GtkWidget*) data);
    return false;
  }
  
  void GUICanvasImpl::draw() {
    // This generates a draw event on the drawing area, which causes
    // GUICanvasImpl::drawCB to be called.
    g_idle_add(queue_draw, (void*) layout);
  }

  void GUICanvasImpl::setWidgetSize(int w, int h) {
    require_mainthread(__FILE__, __LINE__);
    gtk_layout_set_size(GTK_LAYOUT(layout), w, h);
  }

  GtkAdjustment *GUICanvasImpl::getHAdjustment() const {
    require_mainthread(__FILE__, __LINE__);
    return gtk_scrollable_get_hadjustment(GTK_SCROLLABLE(layout));
  }

  GtkAdjustment *GUICanvasImpl::getVAdjustment() const {
    require_mainthread(__FILE__, __LINE__);
    return gtk_scrollable_get_vadjustment(GTK_SCROLLABLE(layout));
  }

  //=\\=//

  // Zooming


  void GUICanvasImpl::zoomToFill() {
    if(empty())
      return;

    int n = nVisibleItems();
    if(n == 0)
      return;

    double newppu = getFilledPPU(n, widgetWidth()/(1+margin),
				 widgetHeight()/(1+margin));
    
    if(newppu < std::numeric_limits<double>::max())
      setTransform(newppu);
    else {
      // No ppu was established.  This can only happen if all of the
      // CanvasItems have sizes set in device units and all of their
      // reference points coincide.  In that case, the user units are
      // irrelevant, so any ppu will do.
      setTransform(1.0);
    }
    center();
  } // GUICanvasImpl::zoomToFill

  //=\\=//

  static void centerAdj(GtkAdjustment *adj) {
    // Set a Gtk Adjustment to its center value.
    require_mainthread(__FILE__, __LINE__);
    double l = gtk_adjustment_get_lower(adj);
    double u = gtk_adjustment_get_upper(adj);
    double p = gtk_adjustment_get_page_size(adj);
    double v = l + 0.5*(u - p  - l);
    gtk_adjustment_set_value(adj, v);
  }
  
  void GUICanvasImpl::center() {
    // Move the center of the image to the center of the window,
    // without changing scale.
    centerAdj(getHAdjustment());
    centerAdj(getVAdjustment());
    draw();
  }

  Rectangle GUICanvasImpl::visibleRegion() const {
    require_mainthread(__FILE__, __LINE__);
    ICoord pix0(gtk_adjustment_get_value(getHAdjustment()),
		gtk_adjustment_get_value(getVAdjustment()));
    Coord pt0 = pixel2user(pix0);
    Coord pt1 = pixel2user(pix0 + ICoord(widgetWidth(), widgetHeight()));
    return Rectangle(pt0, pt1);
  }

  void GUICanvasImpl::zoomAbout(const Coord &fixedPt, double factor) {
    // Zoom by factor while keeping the device-space coordinates of
    // the user-space fixedPt fixed.
    // The visible window size is fixed, but the virtual window isn't.
    require_mainthread(__FILE__, __LINE__);

    GtkAdjustment *hadj = getHAdjustment();
    GtkAdjustment *vadj = getVAdjustment();

    // Find the device coordinates of the fixedPt
    ICoord devPt = user2pixel(fixedPt);
    double xadj = gtk_adjustment_get_value(hadj);
    double yadj = gtk_adjustment_get_value(vadj);
    int xdev = devPt.x - xadj;
    int ydev = devPt.y - yadj;

    // Zoom
    setTransform(factor*ppu);

    // Adjust scrollbars so that fixedPt is back where it was in
    // device space.
    devPt = user2pixel(fixedPt);
    xadj = devPt.x - xdev;
    yadj = devPt.y - ydev;
    gtk_adjustment_set_value(hadj, xadj);
    gtk_adjustment_set_value(vadj, yadj);

    draw();
  }

  void GUICanvasImpl::zoomAbout(const Coord *fixedPt, double factor) {
    zoomAbout(*fixedPt, factor);
  }
  
  void GUICanvasImpl::zoom(double factor) {
    int w2 = 0.5*widgetWidth();
    int h2 = 0.5*widgetHeight();
    double xadj = gtk_adjustment_get_value(getHAdjustment());
    double yadj = gtk_adjustment_get_value(getVAdjustment());
    Coord cntr = pixel2user(ICoord(xadj + w2, yadj + h2));
    zoomAbout(cntr, factor);
  }

  //=\\=//

  // widgetHeight and widgetWidth return the size of the visible part
  // of the canvas, ie, the size of the window containing the canvas.

  int GUICanvasImpl::widgetHeight() const {
    require_mainthread(__FILE__, __LINE__);
    return gtk_widget_get_allocated_height(layout);
  }

  int GUICanvasImpl::widgetWidth() const {
    require_mainthread(__FILE__, __LINE__);
    return gtk_widget_get_allocated_width(layout);
  }

  //=\\=//

  void GUICanvasImpl::setRubberBand(RubberBand *rb) {
    rubberBand = rb;
    rubberBandLayer.dirty = true;
  }

  void GUICanvasImpl::removeRubberBand() {
    rubberBand = nullptr;
  }

  //=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

  // Callback routines for gdk events

  // realizeCB is called once, when the Canvas's gtk object is
  // "realized", whatever that means.  It's not as if the Canvas has
  // any existence other than as a pattern of bits.

  void GUICanvasImpl::realizeCB(GtkWidget*, gpointer data) {
    ((CanvasImpl*) data)->realizeHandler();
  }

  void GUICanvasImpl::realizeHandler() {
    // Set the initial size of the virtual window to be the same as
    // the size the actual window.
    require_mainthread(__FILE__, __LINE__);
    gtk_layout_set_size(GTK_LAYOUT(layout), widgetWidth(), widgetHeight());

    // https://developer.gnome.org/gtk3/stable/GtkLayout.html says:
    // When handling expose events on a GtkLayout, you must draw to
    // the GdkWindow returned by gtk_layout_get_bin_window(), rather
    // than to the one returned by gtk_widget_get_window() as you
    // would for a GtkDrawingArea.
    // TODO: Do we need this?  Do we actually catch expose events?
    // GdkWindow *bin_window = gtk_layout_get_bin_window(GTK_LAYOUT(layout));
    // gdk_window_set_events(bin_window,
    // 			  (GdkEventMask)(gdk_window_get_events(bin_window)
    // 					 | GDK_EXPOSURE_MASK));
  }

  //=\\=//

  // static
  void GUICanvasImpl::allocateCB(GtkWidget*, GdkRectangle *allocation,
				 gpointer data)
  {
    ((GUICanvasImpl*) data)->allocateHandler(allocation);
  }

  void GUICanvasImpl::allocateHandler(GdkRectangle *allocation) {
    // The window size has changed.
    resizeHandler();
  }

  
  //=\\=//

  // The Gtk.Layout has been destroyed.  Make sure that we don't try
  // to use it.

  void GUICanvasImpl::destroyCB(GtkWidget *widget, gpointer data) {
    ((CanvasImpl*) data)->destroyHandler();
  }

  void GUICanvasImpl::destroyHandler() {
    layout = nullptr;
  }

  //=\\=//

  // Get the horizontal and vertical adjustments for converting layer
  // bitmap coordinates to window bitmap coordinates.  They are
  // potentially different than the values obtained from the
  // scrollbars' GtkAdjustments because the bitmaps are centered in
  // the window if they're smaller than the window.
  void GUICanvasImpl::getEffectiveAdjustments(double &hadj, double &vadj) {
    require_mainthread(__FILE__, __LINE__);
    hadj = gtk_adjustment_get_value(getHAdjustment());
    vadj = gtk_adjustment_get_value(getVAdjustment());

    ICoord bsize(backingLayer.bitmapSize());

    int w = widgetWidth();
    if(bsize.x < w) {
      hadj = 0.5*(bsize.x - w);
      centerOffset.x = hadj - gtk_adjustment_get_value(getHAdjustment());
    }
    else
      centerOffset.x = 0.0;
    
    int h = widgetHeight();
    if(bsize.y < h) {
      vadj = 0.5*(bsize.y - h);
      centerOffset.y = vadj - gtk_adjustment_get_value(getVAdjustment());
    }
    else
      centerOffset.y = 0.0;
  }
  
  //=\\=//

  bool GUICanvasImpl::drawCB(GtkWidget*, Cairo::Context::cobject *ctxt,
			  gpointer data)
  {
    return ((GUICanvasImpl*) data)->drawHandler(
		Cairo::RefPtr<Cairo::Context>(new Cairo::Context(ctxt, false)));
  }

  bool GUICanvasImpl::drawHandler(Cairo::RefPtr<Cairo::Context> context) {
    // From the gtk2->gtk3 conversion notes: "The cairo context is
    // being set up so that the origin at (0, 0) coincides with the
    // upper left corner of the widget, and is properly clipped."
    // (https://docs.gtk.org/gtk3/migrating-2to3.html)
    KeyHolder kh(lock, __FILE__, __LINE__);
    require_mainthread(__FILE__, __LINE__);

    double hadj, vadj;
    getEffectiveAdjustments(hadj, vadj);

    setTransform(ppu);

    // If the only thing that's changed is the rubberband, make sure
    // that we don't update more than is necessary.  The rubberband
    // needs to be redrawn quickly and often.

    // If there's no rubberband, just update all layers and copy them
    // to the device's context.

    // If there is a rubberband, and if the rubberband buffer is up to
    // date, copy the rubberband buffer and the rubberband to the
    // device. 

    // If there is a rubberband, but the rubberband buffer is out of
    // date, copy the layers to the rubberband buffer and then copy it
    // and the rubberband to the device.

    if(rubberBand && rubberBand->active()) {

      if(nonRubberBandBufferFilled) {

	// Are any non-rubberband layers dirty?
	bool dirty = false;
	for(unsigned int i=0; i<layers.size(); i++)
	  if(layers[i]->dirty) {
	    dirty = true;
	    break;
	  }
	if(!dirty) {
	  // No layers other than the rubberband have changed.  Copy the
	  // nonRubberBandBuffer, which already contains the other layers,
	  // to the destination, and draw the rubberband on top of that.
	  // Restrict all drawing to the region defined by the union
	  // of the current and former rubberband bounding boxes.

	  // Define RESTRICT_RUBBERBAND to include code that sort of
	  // limits the redrawing to the region containing the old and
	  // new rubberbands.  This is of dubious importance, and is
	  // not working properly anyway.  It's dubious because the
	  // whole visible region has to be redrawn anyway, because
	  // Gtk seems to clear it before calling this function.  It
	  // doesn't work because (I think) the user2pixel call when
	  // defining the clipping region is not appropriate for the
	  // WindowSizeCanvasLayer.
	  //#define RESTRICT_RUBBERBAND
#ifdef RESTRICT_RUBBERBAND
	  // rubberBandBBox holds the previous rubberband's bounding
	  // box.  Expand it to hold the current one as well.
	  Rectangle newbb = rubberBandLayer.findBoundingBox(ppu);
	  rubberBandBBox.swallow(newbb);
#endif // RESTRICT_RUBBERBAND
	  
	  // First draw all non-rubberband layers.  These need to be
	  // drawn outside the clipping area.  (Why?  Those regions
	  // should retain their values from the previous draw, but
	  // they don't.  Gtk must be clearing the drawing area before
	  // calling this function.)

	  drawBackground(context);
	  context->set_source(nonRubberBandBuffer, -hadj, -vadj);
	  context->paint();

#ifdef RESTRICT_RUBBERBAND
	  context->save();
	  ICoord ll = user2pixel(rubberBandBBox.lowerLeft());
	  ICoord ur = user2pixel(rubberBandBBox.upperRight());
	  context->move_to(ll.x, ll.y);
	  context->line_to(ll.x, ur.y);
	  context->line_to(ur.x, ur.y);
	  context->line_to(ur.x, ll.y);
	  context->close_path();

	  // For making the clipping region visible:
	  // context->save();
	  // context->set_line_width(1);
	  // context->set_source_rgb(1.0, 0.0, 0.0);
	  // context->stroke_preserve();
	  // context->restore();
	  
	  context->clip();
#endif // RESTRICT_RUBBERBAND

	  // rubberband
	  rubberBandLayer.render();
	  // rubberBandLayer is a WindowSizeCanvasLayer, and its
	  // copyToCanvas method doesn't use the last two arguments.
	  rubberBandLayer.copyToCanvas(context, 0, 0);

#ifdef RESTRICT_RUBBERBAND
	  // Stop restricting drawing to the rubberband bounding box
	  context->restore();
	  rubberBandBBox = newbb;
#endif // RESTRICT_RUBBERBAND
	  return true;
	}
      }	// end of nonRubberBandBufferFilled

      // We have a rubberband, but nonRubberBandBuffer, which contains
      // all the layers *other* than the rubberBandLayer, needs to be
      // rebuilt.
      ICoord bsize(backingLayer.bitmapSize());
      nonRubberBandBuffer = Cairo::RefPtr<Cairo::ImageSurface>(
			       Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32,
							   bsize.x, bsize.y));
      cairo_t *rbctxt = cairo_create(nonRubberBandBuffer->cobj());
      Cairo::RefPtr<Cairo::Context> nonrbContext =
	Cairo::RefPtr<Cairo::Context>(new Cairo::Context(rbctxt, true));

      // Draw all other layers to the nonRubberBandBuffer.  They're
      // drawn at their unscrolled positions, because the
      // nonRubberBandBuffer will be shifted when copied to the
      // screen.
      for(CanvasLayerImpl *layer : layers) {
	layer->render();
	layer->copyToCanvas(nonrbContext, 0, 0); 
      }
      nonRubberBandBufferFilled = true;

      drawBackground(context);
      context->set_source(nonRubberBandBuffer, -hadj, -vadj);
      context->paint();

      rubberBandLayer.render();
      rubberBandLayer.copyToCanvas(context, 0, 0); // last args not used
#ifdef RESTRICT_RUBBERBAND
      rubberBandBBox = rubberBandLayer.findBoundingBox(ppu);
#endif // RESTRICT_RUBBERBAND
      return true;
    }

    // There's no rubberband, just draw.

    // TODO? Extract the clipping region from the context using
    // Cairo::Context::get_clip_extents, and only redraw CanvasItems
    // whose bounding boxes intersect the clipping region.  If the
    // items are stored in an R-tree this might be fast.

    drawBackground(context);

    for(CanvasLayerImpl *layer : layers) {
      layer->render();		// only redraws dirty layers
      layer->copyToCanvas(context, hadj, vadj); // copies layers to canvas
    }
    return true;
  } // GUICanvasImpl::drawHandler

  //=\\=//

  bool GUICanvasImpl::buttonCB(GtkWidget*, GdkEventButton *event, gpointer data)
  {
    return ((GUICanvasImpl*) data)->mouseButtonHandler(event);
  }

  bool GUICanvasImpl::mouseButtonHandler(GdkEventButton *event) {
    if(empty())
      return false;
    KeyHolder kh(lock, __FILE__, __LINE__);
    ICoord pixel(event->x, event->y);
    Coord userpt(pixel2user(pixel));
    std::string eventtype;
    if(event->type == GDK_BUTTON_PRESS) {
      eventtype = "down";
      buttonDown = true;
      mouseDownPt = userpt;
    }
    else {
      eventtype = "up";
      buttonDown = false;
      if(rubberBand && rubberBand->active()) {
	rubberBand->stop();
      }
    }
    lastButton = event->button;
    doCallback(eventtype, userpt, lastButton,
	       event->state & GDK_SHIFT_MASK,   
	       event->state & GDK_CONTROL_MASK);
    // The callback may have installed a rubberband.
    if(eventtype == "down" && rubberBand) {
      if(!rubberBand->active()) {
	nonRubberBandBufferFilled = false;
	rubberBand->start(&rubberBandLayer, mouseDownPt);
      }
      rubberBand->update(userpt);
    }
    return false;
  }

  //=\\=//
  
  bool GUICanvasImpl::motionCB(GtkWidget*, GdkEventMotion *event, gpointer data)
  {
    return ((CanvasImpl*) data)->mouseMotionHandler(event);
  }

  bool GUICanvasImpl::mouseMotionHandler(GdkEventMotion *event) {
    KeyHolder kh(lock, __FILE__, __LINE__);
    if(allowMotion == MotionAllowed::ALWAYS ||
       (allowMotion == MotionAllowed::MOUSEDOWN && buttonDown))
      {
	ICoord pixel(event->x, event->y);
	Coord userpt(pixel2user(pixel));
	if(rubberBand) {
	  rubberBand->update(userpt);
	}
	doCallback("move", userpt, lastButton,
		   event->state & GDK_SHIFT_MASK,
		   event->state & GDK_CONTROL_MASK);
	return false;
      }
    // Returning "true" means that this handler has processed the
    // event and no further processing should be done.  In particular,
    // it prevents the event from being logged if a gui event logger
    // is being used.  This is easier than turning logging on and off
    // in the logger.  (Motion events shouldn't be logged if they're
    // not wanted, because there are too many of them.)
    return true; 
  }

  MotionAllowed GUICanvasImpl::allowMotionEvents(MotionAllowed ma) {
    MotionAllowed old = allowMotion;
    allowMotion = ma;
    return old;
  }

  //=\\=//  

  bool GUICanvasImpl::scrollCB(GtkWidget*, GdkEventScroll *event, gpointer data)
  {
    return ((CanvasImpl*) data)->scrollHandler(event);
  }

  bool GUICanvasImpl::scrollHandler(GdkEventScroll *event) {
    KeyHolder kh(lock, __FILE__, __LINE__);
    if(event->direction == GDK_SCROLL_SMOOTH) {
      // Scroll amount is stored in deltas.
      Coord delta(event->delta_x, event->delta_y);
      // TODO: Do we want to use a different callback here?  Is there
      // any point in transmitting the event location as well as the
      // delta?
      doCallback("scroll", delta, lastButton,
		 event->state & GDK_SHIFT_MASK,
		 event->state & GDK_CONTROL_MASK);

    }
    return false;
  }

  //=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

  // The classes derived from GUICanvasImpl, Canvas and PythonCanvas,
  // differ in how the GtkLayout is created and in how the callback
  // functions are set up.  PythonCanvas assumes that they all come
  // from Python.  The PythonCanvas class is called "Canvas" in
  // Python.


  // Derived class to be used when the Canvas is created and used in
  // C++ with a Gtk GUI.  The mouse callback must be a C++ function.
  // A gtk layout is created by the constructor and can be retrieved
  // by calling CanvasImpl::gtk().

  CanvasImpl::CanvasImpl(double ppu)
    : GUICanvasImpl(ppu),
      mouseCallback(nullptr),
      mouseCallbackData(nullptr),
      resizeCallback(nullptr),
      resizeCallbackData(nullptr)
  {
    require_mainthread(__FILE__, __LINE__);
    layout = gtk_layout_new(NULL, NULL);
    initSignals();
  }

  CanvasImpl::~CanvasImpl() {
    destroy();
  }

  void CanvasImpl::destroy() {
    require_mainthread(__FILE__, __LINE__);
    // Signal handlers are automatically disconnected when the widget
    // is destroyed.
    if(destroyed)
      return;
    destroyed = true;
    // Actually destroy the gtk widget, since we created it.
    if(layout)
      gtk_widget_destroy(layout);
  }

  void CanvasImpl::setMouseCallback(MouseCallback mcb, void *data) {
    mouseCallback = mcb;
    mouseCallbackData = data;
  }

  void CanvasImpl::removeMouseCallback() {
    mouseCallback = nullptr;
    mouseCallbackData = nullptr;
  }

  void CanvasImpl::setResizeCallback(ResizeCallback rscb, void *data) {
    resizeCallback = rscb;
    resizeCallbackData = data;
  }

  void CanvasImpl::doCallback(const std::string &eventtype, const Coord &userpt,
			  int button, bool shift, bool ctrl)
  {
    require_mainthread(__FILE__, __LINE__);
    if(mouseCallback != nullptr) {
      (*mouseCallback)(eventtype, userpt, button, shift, ctrl,
		       mouseCallbackData);
      draw();
    }
  }

  void CanvasImpl::resizeHandler() {
    require_mainthread(__FILE__, __LINE__);
    if(resizeCallback != nullptr) {
      (*resizeCallback)(resizeCallbackData);
    }
  };
  
#ifdef OOFCANVAS_USE_PYTHON

  // Derived class to be used when the Canvas is created in Python.  A
  // Gtk.Layout must be created and passed in to the constructor.  The
  // mouse callback must be a Python function.  All other public
  // methods are available in C++ and Python.

  PythonCanvas::PythonCanvas(PyObject *pyCanvas, double ppu)
    : GUICanvasImpl(ppu),
      mouseCallback(nullptr),
      mouseCallbackData(Py_None),
      resizeCallback(nullptr),
      resizeCallbackData(Py_None)
  {
    require_mainthread(__FILE__, __LINE__);
    PyGILState_STATE pystate = PyGILState_Ensure();
    try {
      // The initial value of the data to be passed to the python mouse
      // callback is None. Since we're storing it, we need to incref it.
      Py_INCREF(mouseCallbackData);
      Py_INCREF(resizeCallbackData);

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

    initSignals();
  }

  PythonCanvas::~PythonCanvas() {
    destroy();
  }

  void PythonCanvas::destroy() {
    if(destroyed)
      return;
    require_mainthread(__FILE__, __LINE__);
    destroyed = true;
    // Dereference, but don't destroy the widget, since we didn't create it.
    g_object_unref(layout);
    PyGILState_STATE pystate = PyGILState_Ensure();
    try {
      if(mouseCallback != nullptr)
	Py_DECREF(mouseCallback);
      if(resizeCallback != nullptr)
	Py_DECREF(resizeCallback);
      Py_DECREF(mouseCallbackData);
      Py_DECREF(resizeCallbackData);
    }
    catch(...) {
      PyGILState_Release(pystate);
      throw;
    }
    PyGILState_Release(pystate);
  }

  void PythonCanvas::doCallback(const std::string &eventtype,
				const Coord &userpt,
				int button, bool shift, bool ctrl)
  {
    require_mainthread(__FILE__, __LINE__);
    if(mouseCallback != nullptr) {
      PyGILState_STATE pystate = PyGILState_Ensure();
      try {
	PyObject *position = Py_BuildValue("dd", userpt.x, userpt.y);
	PyObject *args = Py_BuildValue("sOiiiO", eventtype.c_str(),
				       position,
				       button, shift, ctrl,
				       mouseCallbackData);
	PyObject *result = PyObject_CallObject(mouseCallback, args);
	if(result == nullptr) {
	  PyErr_Print();
	  PyErr_Clear();
	}
	Py_XDECREF(args);
	Py_XDECREF(result);
	Py_XDECREF(position);
      }
      catch (...) {
	PyGILState_Release(pystate);
	throw;
      }
      PyGILState_Release(pystate);
      draw();
    }

  }
  
  void PythonCanvas::setMouseCallback(PyObject *pymcb, PyObject *pydata) {
    PyGILState_STATE pystate = PyGILState_Ensure();
    try {
      removeMouseCallback();
      mouseCallback = pymcb;
      Py_INCREF(mouseCallback);
      if(pydata != nullptr) {
	mouseCallbackData = pydata;
      }
      else {
	mouseCallbackData = Py_None;
      }
      Py_INCREF(mouseCallbackData);
    }
    catch (...) {
      PyGILState_Release(pystate);
      throw;
    }
    PyGILState_Release(pystate);
  }

  void PythonCanvas::removeMouseCallback() {
    PyGILState_STATE pystate = PyGILState_Ensure();
    try {
      if(mouseCallback) {
	Py_DECREF(mouseCallback);
	mouseCallback = nullptr;
      }
      if(mouseCallbackData) {
	Py_DECREF(mouseCallbackData);
      }
    }
    catch (...) {
      PyGILState_Release(pystate);
      throw;
    }
    PyGILState_Release(pystate);
  }

  void PythonCanvas::setResizeCallback(PyObject *rscb, PyObject *pydata) {
    PyGILState_STATE pystate = PyGILState_Ensure();
    try {
      if(resizeCallback) {
	Py_DECREF(resizeCallback);
	resizeCallback = nullptr;
      }
      if(resizeCallbackData) {
	Py_DECREF(resizeCallbackData);
      }
    
      resizeCallback = rscb;
      Py_INCREF(resizeCallback);
      if(pydata != nullptr) {
	resizeCallbackData = pydata;
      }
      else {
	resizeCallbackData = Py_None;
      }
      Py_INCREF(resizeCallbackData);
    }
    catch (...) {
      PyGILState_Release(pystate);
      throw;
    }
    PyGILState_Release(pystate);
  }

  void PythonCanvas::resizeHandler() {
    if(resizeCallback) {
      PyGILState_STATE pystate = PyGILState_Ensure();
      try {
	PyObject *args = Py_BuildValue("(O)", resizeCallbackData);
	PyObject *result = PyObject_CallObject(resizeCallback, args);
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

  // void initializePyGTK() {
  //   static bool initialized = false;
  //   if(!initialized) {
  //     initialized = true;
  //     gtk_init(0, nullptr);
  //     PyGILState_STATE pystate = PyGILState_Ensure();
  //     try {
  // 	if(!pygobject_init(-1, -1, -1))
  // 	  throw "Cannot initialize pygobject!";
  //     }
  //     catch (...) {
  // 	PyGILState_Release(pystate);
  // 	throw;
  //     }
  //     PyGILState_Release(pystate);
  //   }
  // }

#endif // OOFCANVAS_USE_PYTHON

  //=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

  Canvas::Canvas(double ppu)
    : OffScreenCanvas(new CanvasImpl(ppu))
  {
    guiCanvasImpl = dynamic_cast<CanvasImpl*>(osCanvasImpl);
  }

  int Canvas::widgetWidth() const {
    return guiCanvasImpl->widgetWidth();
  }

  int Canvas::widgetHeight() const {
    return guiCanvasImpl->widgetHeight();
  }

  void Canvas::zoom(double factor) {
    guiCanvasImpl->zoom(factor);
  }

  void Canvas::zoomAbout(const Coord &pt, double factor) {
    guiCanvasImpl->zoomAbout(pt, factor);
  }

  void Canvas::zoomAbout(const Coord *pt, double factor) {
    guiCanvasImpl->zoomAbout(pt, factor);
  }

  void Canvas::zoomToFill() {
    guiCanvasImpl->zoomToFill();
  }

  void Canvas::center() {
    guiCanvasImpl->center();
  }

  Rectangle Canvas::visibleRegion() const {
    return guiCanvasImpl->visibleRegion();
  }

  void Canvas::setMouseCallback(MouseCallback mcb, void *data) {
    guiCanvasImpl->setMouseCallback(mcb, data);
  }
  
  void Canvas::removeMouseCallback() {
    guiCanvasImpl->removeMouseCallback();
  }

  void Canvas::setResizeCallback(ResizeCallback rscb, void *data) {
    guiCanvasImpl->setResizeCallback(rscb, data);
  }

  MotionAllowed Canvas::allowMotionEvents(MotionAllowed ma) {
    return guiCanvasImpl->allowMotionEvents(ma);
  }

  void Canvas::show() {
    guiCanvasImpl->show();
  }

  void Canvas::draw() {
    guiCanvasImpl->draw();
  }

  void Canvas::setRubberBand(RubberBand *rb) {
    guiCanvasImpl->setRubberBand(rb);
  }

  void Canvas::destroy() {
    guiCanvasImpl->destroy();
  }

  GtkWidget *Canvas::gtk() const {
    return guiCanvasImpl->gtk();
  }

};				// namespace OOFCanvas
