// -*- C++ -*-

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modified
 * versions of this software, you first contact the authors at
 * oof_manager@nist.gov. 
 */

#ifndef OOFCANVAS_LAYER_H
#define OOFCANVAS_LAYER_H

#include <cairomm/cairomm.h>

namespace OOFCanvas {
  class GUICanvasBase;
  class OffScreenCanvas;
  class CanvasLayer;
};

#include "oofcanvas/canvas_public.h"
#include "oofcanvas/canvasitem.h"
#include "oofcanvas/utility.h"

namespace OOFCanvas {
  
  class CanvasLayer : public CanvasLayerPublic {
  protected:
    Cairo::RefPtr<Cairo::ImageSurface> surface;
    Cairo::RefPtr<Cairo::Context> context;
    OffScreenCanvas *canvas;
    std::vector<CanvasItem*> items;
    double alpha;
    bool visible;
    bool clickable;
    bool dirty;		// Is the surface or bounding box out of date?
    Rectangle bbox;	// Cached bounding box of all contained items
    void makeCairoObjs(int, int);
  public:
    CanvasLayer(OffScreenCanvas*, const std::string&);
    virtual ~CanvasLayer();
    const std::string name;

    // Methods that need to be accessible through the public interface
    // are pure virtual functions in the CanvasLayerPublic base
    // class. None of them use Cairo types in their return values or
    // arguments, ensuring that the calling code doesn't need to know
    // anything about Cairo or other internals.
    
    // rebuild() recreates the surface using the current size of the Canvas.
    virtual void rebuild();
    // clear make the layer blank and completely transparent.
    virtual void clear();
    // clear(Color) is like clear(), but also sets an opaque background color.
    virtual void clear(const Color&);
    // addItem adds an item to the list and draws to the local
    // surface.  The CanvasLayer takes ownership of the item.
    virtual void addItem(CanvasItem*);
    // TODO GTK3? removeItem(CanvasItem*)  Should it delete the item?
    virtual void removeAllItems();
    
    // render redraws all items to the local surface if the surface is
    // out of date.  It rebuilds the surface if necessary.
    virtual void render();
    // renderToContext draws items to the given context,
    // unconditionally.
    virtual void renderToContext(Cairo::RefPtr<Cairo::Context>) const;
    // copyToCanvas() draws the surface to the given context (probably
    // the Canvas)
    virtual void copyToCanvas(Cairo::RefPtr<Cairo::Context>, double hadj,
			      double vadj) const;

    // Layers can be removed from a Canvas by calling
    // Canvas::deleteLayer or CanvasLayer::destroy.  The effect is the
    // same.  Don't call both.
    virtual void destroy();

    virtual void show();
    virtual void hide();
    bool isDirty() const { return dirty; }
    void markDirty() { dirty = true; }

    // Given the ppu, compute and cache the bounding box. It's not
    // recomputed if the cached value is current. The bool says
    // whether or not the ppu has changed since the last time.
    Rectangle findBoundingBox(double, bool);
    // This version returns but doesn't cache the bounding box.  It
    // always recomputes.
    Rectangle findBoundingBox(double) const;

    virtual ICoord user2pixel(const Coord&) const;
    virtual Coord  pixel2user(const ICoord&) const;
    // These versions use Cairo::Context::device_to_user_distance
    virtual double user2pixel(double) const;
    virtual double pixel2user(double) const;

    ICoord bitmapSize() const;

    virtual void setClickable(bool f) { clickable = f; }
    virtual void clickedItems(const Coord&, std::vector<CanvasItemPublic*>&) const;

    virtual void setOpacity(double alph) { alpha = alph; }

    virtual void allItems(std::vector<CanvasItemPublic*>&) const;
    virtual bool empty() const;
    virtual std::size_t size() const { return items.size(); } 

    virtual void raiseBy(int) const;
    virtual void lowerBy(int) const;
    virtual void raiseToTop() const;
    virtual void lowerToBottom() const;

    virtual void writeToPNG(const std::string &) const;

    Cairo::RefPtr<Cairo::Context> getContext() const { return context; }
    
    friend class CanvasItem;
    friend class GUICanvasBase;
    friend class OffScreenCanvas;
  };

  std::ostream &operator<<(std::ostream&, const CanvasLayer&);

};

#endif // OOFCANVAS_LAYER_H

