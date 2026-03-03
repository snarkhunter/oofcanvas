// -*- C++ -*-

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modified
 * versions of this software, you first contact the authors at
 * oof_manager@nist.gov. 
 */

#ifndef OOFCANVAS_GUICANVAS_IMPL_H
#define OOFCANVAS_GUICANVAS_IMPL_H

#include "oofcanvas/canvasimpl.h"
#include "oofcanvas/oofcanvasgui/guicanvas.h"
#include "oofcanvas/oofcanvasgui/guicanvaslayer.h"
#include "oofcanvas/oofcanvasgui/rubberband.h"
#include <gtk/gtk.h>

namespace OOFCanvas {
  class Canvas;
  class GUICanvasImpl;
  class RubberBand;
  class WindowSizeCanvasLayer;
  enum class MotionAllowed;
#ifdef OOFCANVAS_USE_PYTHON
#include <Python.h>
  class CanvasPython;
#endif

  // GUICanvasImpl includes code that actually draws in a GtkWidget
  // and can interact with the user.  Other than this and its derived
  // classes, no OOFCanvas classes use Gtk.
  
  class GUICanvasImpl : public OSCanvasImpl {
  protected:
    
    GtkWidget *layout;
    MotionAllowed allowMotion;
    int lastButton;		// last mousebutton pressed
    bool buttonDown;

    void initSignals();
    // Mouse handling 
    static bool buttonCB(GtkWidget*, GdkEventButton*, gpointer);
    bool mouseButtonHandler(GdkEventButton*);
    static bool motionCB(GtkWidget*, GdkEventMotion*, gpointer);
    bool mouseMotionHandler(GdkEventMotion*);
    virtual void doCallback(const std::string&, const Coord&,
			    int, bool, bool) = 0;
    // Scrollwheel
    static bool scrollCB(GtkWidget*, GdkEventScroll*, gpointer);
    bool scrollHandler(GdkEventScroll*);

    // Window creation
    static void realizeCB(GtkWidget*, gpointer);
    virtual void realizeHandler();

    // Window resizing
    static void allocateCB(GtkWidget*, GdkRectangle*, gpointer);
    void allocateHandler(GdkRectangle*);
    virtual void resizeHandler() = 0;

    static void destroyCB(GtkWidget*, gpointer);
    void destroyHandler();

    static bool drawCB(GtkWidget*, Cairo::Context::cobject*, gpointer);
    bool drawHandler(Cairo::RefPtr<Cairo::Context>);

    virtual void setWidgetSize(int, int);
    
    // Machinery used to draw rubberbands quickly.
    WindowSizeCanvasLayer rubberBandLayer; // rubberband representation
    Cairo::RefPtr<Cairo::ImageSurface> nonRubberBandBuffer;
    RubberBand *rubberBand;	   // the rubberband, or nullptr
    Rectangle rubberBandBBox;	   // bounding box of the previous rubberband
    Coord mouseDownPt;		   // where the rubberband drawing started
    bool nonRubberBandBufferFilled;

    bool destroyed;

  public:
    GUICanvasImpl(double ppu);
    virtual ~GUICanvasImpl() {}

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

    MotionAllowed allowMotionEvents(MotionAllowed ma);

    void show();		// make gtk widgets visible
    void draw();		// draws all modified layers
    
    GtkAdjustment *getHAdjustment() const;
    GtkAdjustment *getVAdjustment() const;
    void getEffectiveAdjustments(double&, double&);

    void setRubberBand(RubberBand*);
    void removeRubberBand();
  };				// GUICanvasImpl

  //=\\=//

  // In C++, the OOFCanvas constructor creates the gtk Layout.

  class CanvasImpl : public GUICanvasImpl {
  protected:
    // mouse callback args are event type, position (in user coords),
    // button, state (GdkModifierType)
    MouseCallback mouseCallback;
    void *mouseCallbackData;
    virtual void doCallback(const std::string&, const Coord&, int, bool, bool);
    ResizeCallback resizeCallback;
    void *resizeCallbackData;
    virtual void resizeHandler();
  public:
    CanvasImpl(double);
    virtual ~CanvasImpl();
    virtual void destroy();
    // Second argument to setMouseCallback is extra data to be passed
    // through to the callback function.    
    void setMouseCallback(MouseCallback, void*);
    void removeMouseCallback();
    void setResizeCallback(ResizeCallback, void*);

    // gtk() is not exported to Python, since the GtkWidget* is not a
    // properly wrapped PyGTK object.
    GtkWidget *gtk() const { return layout; }
  };
  
  //=\\=//
#ifdef OOFCANVAS_USE_PYTHON
  
  // In Python, the Gtk.Layout is created in Python and passed in to
  // the OOFCanvasPython constructor, and the callback functions are
  // Python functions.

  class PythonCanvas : public GUICanvasImpl {
  private:
    bool destroyed;
  protected:
    PyObject *mouseCallback;
    PyObject *mouseCallbackData;
    PyObject *resizeCallback;
    PyObject *resizeCallbackData;
    virtual void doCallback(const std::string&, const Coord&, int, bool, bool);
    virtual void resizeHandler();
  public:
    PythonCanvas(PyObject*, double);
    virtual ~PythonCanvas();
    virtual void destroy();
    // Second argument to setMouseCallback is extra data to be passed
    // through to the callback function.
    void setMouseCallback(PyObject*, PyObject*);
    void removeMouseCallback();
    void setResizeCallback(PyObject*, PyObject*);
  };
#endif	// OOFCANVAS_USE_PYTHON

  // void initializePyGTK();

};				// namespace OOFCanvas

#endif	// OOFCANVAS_GUICANVAS_IMPL_H

