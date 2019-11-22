// -*- C++ -*-

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modified
 * versions of this software, you first contact the authors at
 * oof_manager@nist.gov. 
 */

#ifndef OOFCANVASLAYER_H
#define OOFCANVASLAYER_H

#include <cairomm/cairomm.h>
//#include "OOFCanvas/canvasitem.h"

namespace OOFCanvas {
  
  class Canvas;
  class CanvasItem;

  class CanvasLayer {
  private:
    Cairo::RefPtr<Cairo::ImageSurface> surface;
    Cairo::RefPtr<Cairo::Context> context;
  protected:
    Canvas *canvas;
    std::vector<CanvasItem*> items;
    bool visible;
    bool clickable;
  public:
    CanvasLayer(Canvas*);
    ~CanvasLayer();
    // clear() recreates the surface using the current size of the Canvas.
    void clear();
    // addItem adds an item to the list and draws to the local
    // surface.  The CanvasLayer takes ownership of the item.
    void addItem(CanvasItem*);
    // redraw redraws all items to the local surface
    void redraw();
    // draw() draws the surface to the given context (probably the Canvas)
    void draw(Cairo::RefPtr<Cairo::Context>) const;
    
    Rectangle boundingBox() const;

    ICoord user2pixel(const Coord&) const;
    Coord pixel2user(const ICoord&) const;
    double user2pixel(double) const;
    double pixel2user(double) const;

    void setClickable(bool f) { clickable = f; }
    void clickedItems(const Coord&, std::vector<CanvasItem*>&) const;
    void allItems(std::vector<CanvasItem*>&) const;
    

    // CanvasLayer(Canvas*);
    // void raise(int);
    // void lower(int);
    // void raiseToTop();
    // void lowerToBottom();
    
    friend class Canvas;
  };

};

#endif // OOFCANVASLAYER_H

