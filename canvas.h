// -*- C++ -*-

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modified
 * versions of this software, you first contact the authors at
 * oof_manager@nist.gov. 
 */

#ifndef OOFCANVAS_H
#define OOFCANVAS_H

#include <cairomm/cairomm.h>
#include <gtk/gtk.h>
#include <string>
#include <Python.h>
#include <vector>
#include <cairomm/cairomm.h>

#include "utility.h"


namespace OOFCanvas {

  class CanvasLayer;
  class CanvasItem;

  typedef void (*MouseCallback)(const std::string&, Coord, int, bool, bool);
 
  class Canvas {
  protected:
    PyObject *pyCanvas;
    GtkWidget *drawing_area;
    CanvasLayer *backingLayer;
    std::vector<CanvasLayer*> layers;

    double ppu;	// pixels per unit. Converts user coords to device coords
    int pixelwidth, pixelheight; // size of drawing area in pixels
    Coord offset;		// device coord of user origin
    Cairo::Matrix transform;
    Color bgColor;
    bool redrawNeeded;

    void setTransform(double, const Coord&);
    
    void raiseLayer(const CanvasLayer&, int n); // negative n lowers
    void raiseLayerToTop(const CanvasLayer&);
    void lowerLayerToBottom(const CanvasLayer&);

    //Rectangle visibleRect;
    
    // mouse callback args are event type, position (in user coords),
    // button, state (GdkModifierType)
    MouseCallback *mouseCallback;
    void *mouseCallbackData;
    PyObject *pyMouseCallback;
    PyObject *pyMouseCallbackData;
    bool allowMotion;
    int lastButton;		// last mouse button pressed
    void doCallback(const std::string&, int, int, int, bool, bool) const;

    // TODO: Do we need to store these?
    guint config_handler, expose_handler, motion_handler,
      button_up_handler, button_down_handler, draw_handler;
  public:
    Canvas(PyObject*, int pixelwidth, int pixelheight, double ppu);
    ~Canvas();
    void destroy();

    GtkWidget *gtk() const { return drawing_area; }
    int heightInPixels() {
      return gtk_widget_get_allocated_height(drawing_area);
    }
    int widthInPixels() {
      return gtk_widget_get_allocated_width(drawing_area);
    }
    const Cairo::Matrix &getTransform() const { return transform; }

    // Second argument to setMouseCallback and setPyMouseCallback is
    // extra data to be passed through to the callback function.
    void setMouseCallback(MouseCallback*, void*);
    void setPyMouseCallback(PyObject*, PyObject*);
    void removeMouseCallback();
    void allowMotionEvents(bool allow) { allowMotion = allow; }
    
    void resize(int, int);
    void setPixelsPerUnit(double);
    double getPixelsPerUnit() const { return ppu; }
    void zoom(double);
    void translate(double, double);
    void update(const Rectangle&);
    void setBackgroundColor(double, double, double);
    void show();

    void draw();

    static void configCB(GtkWidget*, GdkEventConfigure*, gpointer);
    static void buttonCB(GtkWidget*, GdkEventButton*, gpointer);
    static void motionCB(GtkWidget*, GdkEventMotion*, gpointer);
    static void drawCB(GtkWidget*, Cairo::Context::cobject*, gpointer);
    void configHandler(GdkEventConfigure*);
    void mouseButtonHandler(GdkEventButton*);
    void mouseMotionHandler(GdkEventMotion*);
    void drawHandler(Cairo::RefPtr<Cairo::Context>);

    ICoord user2pixel(const Coord&) const;
    Coord pixel2user(const ICoord&) const;
    double user2pixel(double) const;
    double pixel2user(double) const;
    
    CanvasLayer *newLayer();
    void deleteLayer(CanvasLayer*);

    std::vector<CanvasItem*> clickedItems(double, double) const;
    std::vector<CanvasItem*> allItems() const;

    // Versions for swig.
    std::vector<CanvasItem*> *clickedItems_new(double, double) const;
    std::vector<CanvasItem*> *allItems_new() const;

    friend class CanvasLayer;
  };

  void initializePyGTK();
};


#endif // OOFCANVAS_H

