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

#include "utility.h"


namespace OOFCanvas {

  class CanvasLayer;
  class CanvasItem;

  typedef void (*MouseCallback)(const std::string&, Coord, int, bool, bool);
 
  class Canvas {
  protected:
    PyObject *pyCanvas;
    GtkWidget *layout;
    CanvasLayer *backingLayer;
    std::vector<CanvasLayer*> layers;
    // boundingBox is the bounding box, in user coordinates, of all of
    // the visible objects.
    Rectangle boundingBox;

    // transform is used by the CanvasLayers when drawing their
    // CanvasItems to their ImageSurfaces.
    Cairo::Matrix transform;
    double ppu;	// pixels per unit. Converts user coords to device coords
    
    Color bgColor;

    void setTransform(double);
    void zoomAbout(double, const Coord&);
    
    void raiseLayer(const CanvasLayer&, int n); // negative n lowers
    void raiseLayerToTop(const CanvasLayer&);
    void lowerLayerToBottom(const CanvasLayer&);

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
    Canvas(PyObject*, double ppu);
    ~Canvas();
    void destroy();

    // gtk() is not exported to Python, since the GtkWidget* is not a
    // properly wrapped PyGTK object.
    GtkWidget *gtk() const { return layout; }

    // widthInPixels and heightInPixels return the size of the widget,
    // in pixels.
    int widthInPixels() const;
    int heightInPixels() const;
    ICoord layoutSize() const;
    ICoord boundingBoxSizeInPixels() const;
    
    double getPixelsPerUnit() const { return ppu; }
    void zoom(double);
    void fill();
    void center();

    // Coordinate system transformations
    const Cairo::Matrix &getTransform() const { return transform; }
    ICoord user2pixel(const Coord&) const;
    Coord pixel2user(const ICoord&) const;
    double user2pixel(double) const;
    double pixel2user(double) const;
    

    // Second argument to setMouseCallback and setPyMouseCallback is
    // extra data to be passed through to the callback function.
    void setMouseCallback(MouseCallback*, void*);
    void setPyMouseCallback(PyObject*, PyObject*);
    void removeMouseCallback();
    void allowMotionEvents(bool allow) { allowMotion = allow; }
    

    bool empty() const;		// Is anything drawn?

    void setBackgroundColor(double, double, double);
    void show();		// make gtk widgets visible

    CanvasLayer *newLayer();
    void deleteLayer(CanvasLayer*);
    void draw();

    // TODO: These callbacks should be private
    static void realizeCB(GtkWidget*, gpointer);
    void realizeHandler();
    static void allocateCB(GtkWidget*, GdkRectangle*, gpointer);
    void allocateHandler();

    static void drawCB(GtkWidget*, Cairo::Context::cobject*, gpointer);
    void drawHandler(Cairo::RefPtr<Cairo::Context>);
    static void buttonCB(GtkWidget*, GdkEventButton*, gpointer);
    void mouseButtonHandler(GdkEventButton*);
    static void motionCB(GtkWidget*, GdkEventMotion*, gpointer);
    void mouseMotionHandler(GdkEventMotion*);

    GtkAdjustment *getHAdjustment() const;
    GtkAdjustment *getVAdjustment() const;

    std::vector<CanvasItem*> clickedItems(double, double) const;
    std::vector<CanvasItem*> allItems() const;

    // Versions for swig.
    std::vector<CanvasItem*> *clickedItems_new(double, double) const;
    std::vector<CanvasItem*> *allItems_new() const;

    friend class CanvasLayer;
    friend class CanvasItem;
  };

  void initializePyGTK();

};				// namespace OOFCanvas


#endif // OOFCANVAS_H

