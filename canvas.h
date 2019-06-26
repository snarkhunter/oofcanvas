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

#include "utility.h"


namespace OOFCanvas {

  class CanvasLayer;
  class CanvasItem;

  class Canvas {
  protected:
    GtkWidget *drawing_area;
    std::vector<CanvasLayer*> layers;

    double ppu;			// pixels per unit
    int pixelwidth, pixelheight; // size of drawing area in pixels
    double width, height;	// in user units
    Coord offset;		// device coord of user origin
    Cairo::Matrix transform;
    Color bgColor;
    bool initialized;
    void setTransform(double, const Coord&);
    
    void raiseLayer(const CanvasLayer&, int n); // negative n lowers
    void raiseLayerToTop(const CanvasLayer&);
    void lowerLayerToBottom(const CanvasLayer&);

    //Rectangle visibleRect;
    
    // mouse callback args are event type, x and y (in user coords),
    // shift, ctrl
    void (*mouseCallback)(const std::string, double, double, bool, bool);

    guint config_handler, expose_handler, button_handler;
  public:
    Canvas(int pixelwidth, int pixelheight);
    ~Canvas();

    PyObject *widget();
    GtkWidget *gtk() const { return drawing_area; }
    int heightInPixels() { return drawing_area->allocation.height; }
    int widthInPixels() { return drawing_area->allocation.width; }
    const Cairo::Matrix &getTransform() const { return transform; }
    
    void setMouseCallback();
    void removeMouseCallback();
    void resize(int, int);
    void setPixelPerUnit(double);
    void zoom(double);
    void shift(double, double);
    void update(const Rectangle&);
    void setBackgroundColor(double, double, double);
    void show();

    void draw();

    static void configCB(GtkWidget*, GdkEvent*, gpointer);
    static void exposeCB(GtkWidget*, GdkEventExpose*, gpointer);
    static void buttonCB(GtkWidget*, GdkEventButton*, gpointer);
    void config(GdkEvent*);
    void expose(GtkWidget*, GdkEventExpose*);
    void mouseButton(GdkEventButton*);

    ICoord user2pixel(const Coord&) const;
    Coord pixel2user(const ICoord&) const;
    
    CanvasLayer *newLayer();
    void deleteLayer(CanvasLayer*);
    
    friend class CanvasLayer;
  };

  void initializePyGTK();
};

#endif // OOFCANVAS_H

