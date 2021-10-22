// -*- C++ -*-

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modified
 * versions of this software, you first contact the authors at
 * oof_manager@nist.gov. 
 */

#ifndef OOFCANVAS_LAYER_IMPL_H
#define OOFCANVAS_LAYER_IMPL_H

#include <cairomm/cairomm.h>

namespace OOFCanvas {
  class GUICanvasBase;
  class OSCanvasImpl;
};

#include "oofcanvas/canvaslayer.h"
#include "oofcanvas/utility_extra.h"

namespace OOFCanvas {
  
  class CanvasLayerImpl : public CanvasLayer {
  protected:
    Cairo::RefPtr<Cairo::ImageSurface> surface;
    Cairo::RefPtr<Cairo::Context> context;
    OSCanvasImpl *canvas;
    std::vector<CanvasItem*> items;
    double alpha;
    bool visible;
    bool clickable;
    bool dirty;		// Is the surface or bounding box out of date?
    Rectangle bbox;	// Cached bounding box of all contained items
    void makeCairoObjs(int, int);
    mutable Lock layerlock; // Controls access to local context and surface
  public:
    CanvasLayerImpl(OSCanvasImpl*, const std::string&);
    virtual ~CanvasLayerImpl();

    // Methods that need to be accessible through the public interface
    // are pure virtual functions in the CanvasLayer base
    // class. None of them use Cairo types in their return values or
    // arguments, ensuring that the calling code doesn't need to know
    // anything about Cairo or other internals.
    
    // rebuild() recreates the surface using the current size of the Canvas.
    virtual void rebuild();
    void rebuild_nolock();
    // clear make the layer blank and completely transparent.
    virtual void clear();
    void clear_nolock();
    // clear(Color) is like clear(), but also sets an opaque background color.
    virtual void clear(const Color&);
    void clear_nolock(const Color&);
    // addItem adds an item to the list and draws to the local
    // surface.  The CanvasLayerImpl takes ownership of the item.
    virtual void addItem(CanvasItem*);
    virtual void removeItem(CanvasItem*);
    virtual void removeAllItems();
    
    // render redraws all items to the local surface if the surface is
    // out of date.  It rebuilds the surface if necessary.
    virtual void render();
    // renderToContext draws items to the given context,
    // unconditionally.
    virtual void renderToContext(Cairo::RefPtr<Cairo::Context>) const;
    void renderToContext_nolock(Cairo::RefPtr<Cairo::Context>) const;
    // copyToCanvas() draws the surface to the given context (probably
    // the Canvas)
    virtual void copyToCanvas(Cairo::RefPtr<Cairo::Context>, double hadj,
			      double vadj) const;

    // Layers can be removed from a Canvas by calling
    // Canvas::deleteLayer or CanvasLayerImpl::destroy.  The effect is the
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
    Rectangle findBoundingBox_nolock(double) const;

    virtual ICoord user2pixel(const Coord&) const;
    virtual Coord  pixel2user(const ICoord&) const;
    // These versions use Cairo::Context::device_to_user_distance
    virtual double user2pixel(double) const;
    virtual double pixel2user(double) const;

    ICoord bitmapSize() const;

    virtual void setClickable(bool f) { clickable = f; }
    virtual void clickedItems(const Coord&, std::vector<CanvasItem*>&) const;

    virtual void setOpacity(double alph) { alpha = alph; }

    virtual void allItems(std::vector<CanvasItem*>&) const;
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
    friend class OSCanvasImpl;
  };

  std::ostream &operator<<(std::ostream&, const CanvasLayerImpl&);

};

#endif // OOFCANVAS_LAYER_IMPL_H

