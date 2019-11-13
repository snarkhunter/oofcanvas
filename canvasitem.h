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

#include <cairomm/cairomm.h>

namespace OOFCanvas {

  class CanvasItem {
  protected:
    Rectangle bbox;		// bouding box in user space
  public:
    virtual ~CanvasItem();
    void setBBox(const Rectangle &bbx) { bbox = bbx; }
    void setBBox(double xmin, double ymin, double xmax, double ymax) {
      bbox = Rectangle(xmin, ymin, xmax, ymax);
    }
    const Rectangle &boundingBox() const { return bbox; }
    // draw() is called by CanvasLayer::draw().  It calls drawItem(),
    // which must be defined in each CanvasItem subclass.
    void draw(Cairo::RefPtr<Cairo::Context>) const;
    
    virtual void drawItem(Cairo::RefPtr<Cairo::Context>) const = 0;

    // containsPoint computes whether the given point in user
    // coordinates is on the item.  It's used to determine if a mouse
    // click selected the item.  It's called after bounding boxes have
    // been checked, so there's no need for it to check again.
    virtual bool containsPoint(const Coord&) const = 0;
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

