// -*- C++ -*-

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modified
 * versions of this software, you first contact the authors at
 * oof_manager@nist.gov. 
 */

// Header file for the CanvasItem Implementation classes.  This file
// is used for building the OOFCanvas library.  It's not exposed to
// the OOFCanvas user.

#ifndef OOFCANVAS_ITEM_IMPL_H
#define OOFCANVAS_ITEM_IMPL_H

#include "oofcanvas/utility_extra.h"
#include <cairomm/cairomm.h>

namespace OOFCanvas {

  class OSCanvasImpl;
  class CanvasLayer;

  class CanvasItemImplBase {
  private:
    // CanvasItemImplBase can only be constructed by CanvasItemImplementation
    template <class T> friend class CanvasItemImplementation;
    CanvasItemImplBase(const Rectangle&); // arg is the bare bounding box
    CanvasLayer *layer;

  public:
    virtual ~CanvasItemImplBase();

    void setLayer(CanvasLayer *lyr) { layer = lyr; }
    const CanvasLayer *getLayer() const { return layer; }
    
    // draw() is called by CanvasLayerImpl::draw().  It calls
    // drawItem(), which must be defined in each
    // CanvasItemImplementation subclass.
    void draw(Cairo::RefPtr<Cairo::Context>) const;
    virtual void drawItem(Cairo::RefPtr<Cairo::Context>) const = 0;

    // drawBoundingBox is a no-op unless DEBUG is defined.
    void drawBoundingBox(double, const Color&);

    // findBoundingBox() computes the actual bounding box, including
    // pixel-sized components, given a value for the pixels per unit.
    // The default implementation uses findBareBoundingBox and
    // pixelExtents.
    Rectangle findBoundingBox(double ppu) const;

    // findBareBoundingBox() returns the what the item's bounding box
    // in user space units would be if the ppu were infinite.  That
    // is, it's the bounding box when any pixel-sized components of
    // the item have been shrunk to zero.  Most subclasses can set
    // CanvasItem::bbox in their constructors and don't need to
    // redefine findBareBoundingBox().
    virtual const Rectangle& findBareBoundingBox() const { return bbox; }

    // pixelExtents returns the distances, in pixel units, that the
    // object extends beyond its bare bounding box.  The default
    // implementation returns zeroes.
    virtual void pixelExtents(double &left, double &right,
			      double &up, double &down) const;

    // containsPoint computes whether the given point in user
    // coordinates is on the item.  It's used to determine if a mouse
    // click selected the item.  It's called after bounding boxes have
    // been checked, so there's no need for it to check again.
    virtual bool containsPoint(const OSCanvasImpl*, const Coord&) const = 0;

    // bbox is the "bare" bounding box in user space coordinates.
    // This is the bounding box that the object would have if the
    // pixels were infinitesimal.  Canvas items that can compute this
    // when they're constructed should do so.  If they can't, they
    // need to redefine findBareBoundingBox().
    Rectangle bbox;

    void modified();

#ifdef DEBUG
    bool drawBBox;
    double bboxLineWidth;
    Color bboxColor;
#endif // DEBUG
  };				// class CanvasItemImplBase

  template <class CANVASITEM>
  class CanvasItemImplementation : public CanvasItemImplBase {
  protected:
    CANVASITEM *canvasitem;
  public:
    CanvasItemImplementation(CANVASITEM *item, const Rectangle &bb)
      : CanvasItemImplBase(bb),
	canvasitem(item)
    {}

    virtual ~CanvasItemImplementation() {}

    // draw() is called by CanvasLayerImpl::draw().  It calls
    // drawItem(), which must be defined in each CanvasItem subclass.
    virtual void drawItem(Cairo::RefPtr<Cairo::Context>) const = 0;

    virtual bool containsPoint(const OSCanvasImpl*, const Coord&) const = 0;
  };
};				// namespace OOFCanvas

#endif // OOFCANVAS_ITEM_IMPL_H
