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

#include <string>
#include <iostream>

namespace OOFCanvas {

  class CanvasItem;
  class Color;
  class Coord;
  class ICoord;

  // CanvasLayer is an abstract base class that contains the
  // public interface for CanvasLayerImpl.

  class CanvasLayer {
  public:
    CanvasLayer(const std::string&);
    virtual ~CanvasLayer() {}
    const std::string name;
    
    virtual void rebuild() = 0;
    virtual void clear() = 0;
    virtual void clear(const Color&) = 0;
    virtual void addItem(CanvasItem*) = 0;
    virtual void removeAllItems() = 0;
    virtual void markDirty() = 0;

    virtual void destroy() = 0;
    virtual void show() = 0;
    virtual void hide() = 0;
    virtual void render() = 0;

    virtual ICoord user2pixel(const Coord&) const = 0;
    virtual Coord pixel2user(const ICoord&) const = 0;
    virtual double user2pixel(double) const = 0;
    virtual double pixel2user(double) const = 0;

    virtual void setClickable(bool) = 0;
    virtual void clickedItems(const Coord&, std::vector<CanvasItem*>&)
      const = 0;

    virtual void setOpacity(double) = 0;
    
    virtual void allItems(std::vector<CanvasItem*>&) const = 0;
    virtual bool empty() const = 0;
    virtual std::size_t size() const = 0;

    virtual void raiseBy(int) const = 0;
    virtual void lowerBy(int) const = 0;
    virtual void raiseToTop() const = 0;
    virtual void lowerToBottom() const = 0;

    virtual void writeToPNG(const std::string&) const = 0;
  };

  std::ostream &operator<<(std::ostream&, const CanvasLayer&);

};				// namespace OOFCanvas

#endif	// OOFCANVAS_LAYER_H
