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
#include <string>
#include "utility.h"

namespace OOFCanvas {

  class CanvasLayer;
  class IRectangle;

  class Canvas {
  protected:
    Cairo::RefPtr<Cairo::Surface> root;
    std::vector<CanvasLayer*> layers;
    unsigned int width, height;	// in pixels
    void raiseLayer(const CanvasLayer&, int n); // negative n lowers
    void raiseLayerToTop(const CanvasLayer&);
    void lowerLayerToBottom(const CanvasLayer&);

    Rectangle visibleRect;
    // mouse callback args are event type, x and y (in user coords),
    // shift, ctrl
    void (*mouseCallback)(const std::string, double, double, bool, bool);
  public:
    Canvas(GtkWidget*, int pixelwidth, int pixelheight, double width, height);
    ~Canvas();
    void setMouseCallback();
    void removeMouseCallback();
    void resize(int, int);
    void zoom(double);
    void shift(double, double);
    void update(const Rectangle&);

    ICoord user2pixel(const Coord&) const;
    Coord pixel2user(const ICoord&) const;
    
    CanvasLayer *newLayer();
    friend class CanvasLayer;
  };
};

#endif // OOFCANVAS_H

