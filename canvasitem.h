// -*- C++ -*-

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modified
 * versions of this software, you first contact the authors at
 * oof_manager@nist.gov. 
 */

#ifndef OOFCANVASITEM_H
#define OOFCANVASITEM_H

#include "pythonexportable.h"
#include "utility.h"
#include <cairomm/cairomm.h>

namespace OOFCanvas {

  class Canvas;

  class CanvasItem : public PythonExportable<CanvasItem> {
  protected:
    // Bounding box in user space.  Canvas items that can compute this
    // when they're constructed should do so.  Canvas items that can't
    // compute it without knowing the ppu should override
    // CanvasItem::boundingBox().
    Rectangle bbox;
  public:
    virtual ~CanvasItem();
    virtual const std::string &modulename() const;

    // findBoundingBox() computes the bounding box if it's not already
    // known.  Subclasses that can't compute their bounding boxes
    // unless they know the ppu should override findBoundingBox().
    virtual const Rectangle &findBoundingBox(double ppu) { return bbox; }
    // boundingBox() assumes that the bbox is already computed, and
    // just returns it.
    const Rectangle &boundingBox() const { return bbox; }
    
    // draw() is called by CanvasLayer::draw().  It calls drawItem(),
    // which must be defined in each CanvasItem subclass.
    
    void draw(Cairo::RefPtr<Cairo::Context>) const;
    virtual void drawItem(Cairo::RefPtr<Cairo::Context>) const = 0;

    // containsPoint computes whether the given point in user
    // coordinates is on the item.  It's used to determine if a mouse
    // click selected the item.  It's called after bounding boxes have
    // been checked, so there's no need for it to check again.
    virtual bool containsPoint(const Canvas*, const Coord&) const = 0;

    virtual std::string *print() const = 0; // for python wrapping
  };

  class CanvasItemListIterator {
  private:
    std::vector<CanvasItem*>::iterator end;
    std::vector<CanvasItem*>::iterator iter;
  public:
    CanvasItemListIterator(std::vector<CanvasItem*> *list)
      : iter(list->begin()),
	end(list->end())
    {}
    bool done() { return iter == end; }
    CanvasItem *next_() { assert(!done()); return *iter++; }
  };
};

#endif // OOFCANVASITEM_H

