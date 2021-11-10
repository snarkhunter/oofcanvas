// -*- C++ -*-

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modified
 * versions of this software, you first contact the authors at
 * oof_manager@nist.gov. 
 */

// This is the public API for the abstract CanvasItem base class.

#ifndef OOFCANVAS_ITEM_H
#define OOFCANVAS_ITEM_H

#include "oofcanvas/utility.h"

#ifdef OOFCANVAS_USE_PYTHON
#include "oofcanvas/pythonexportable.h"
#endif

namespace OOFCanvas {
  class CanvasItemImplBase;
  class CanvasLayer;
  class OffScreenCanvas;

  class CanvasItem 
#ifdef OOFCANVAS_USE_PYTHON
    : public PythonExportable<CanvasItem>
#endif 
  {
  protected:
    CanvasItemImplBase *implementation; 

  public:
    CanvasItem(CanvasItemImplBase*);
    virtual ~CanvasItem();
    virtual const std::string &modulename() const;

    void setLayer(CanvasLayer*);
    const CanvasLayer *getLayer() const;
    CanvasItemImplBase *getImplementation() const;

    // drawBoundingBox turns bounding box drawing on for this item.
    // It's a no-op unless DEBUG is defined.
    void drawBoundingBox(double, const Color&);

    // findBoundingBox() computes the actual bounding box in user
    // space units, given a value for the pixels per unit.
    Rectangle findBoundingBox(double ppu) const;

    // containsPoint computes whether the given point in user
    // coordinates is on the item.  It's used to determine if a mouse
    // click selected the item.  It's called after bounding boxes have
    // been checked, so it can assume that the point is within the
    // item's bbox.
    bool containsPoint(const OffScreenCanvas*, const Coord&) const;

    // Any routine that might change a CanvasItem's size after it's
    // been added to a CanvasLayer needs to call modified().
    void modified();

    virtual std::string print() const = 0;
    std::string *repr() const; // for python wrapping
  };

  std::ostream &operator<<(std::ostream&, const CanvasItem&);
};				// namespace OOFCanvas

#endif // OOFCANVAS_ITEM_H

