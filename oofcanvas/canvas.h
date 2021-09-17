// -*- C++ -*-

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modified
 * versions of this software, you first contact the authors at
 * oof_manager@nist.gov. 
 */

// Wrapper that hides Cairo, etc, from the user, and can be included
// in user's code without requiring that they link to or #include the
// external libraries that OOFCanvas uses.

// TODO: After this is working, de-stupidify the names.

#ifndef OOFCANVAS_CANVAS_PUBLIC_H
#define OOFCANVAS_CANVAS_PUBLIC_H

#include <string>

namespace OOFCanvas {
  class CanvasLayer;
  class OSCanvasImpl;
  class Color;
  class ICoord;
  class Coord;
  class CanvasItem;
  

  class OffScreenCanvas {
  private:
    OSCanvasImpl *osCanvasImpl;
  public:
    OffScreenCanvas(double);
    ~OffScreenCanvas();

    OSCanvasImpl *getCanvas() { return osCanvasImpl; }
    const OSCanvasImpl *getCanvas() const { return osCanvasImpl; }

    CanvasLayer *newLayer(const std::string&);
    void deleteLayer(CanvasLayer*);
    CanvasLayer *getLayer(int) const;
    CanvasLayer *getLayer(const std::string&) const;
    std::size_t nLayers() const;

    void raiseLayer(int layer, int howfar); 
    void lowerLayer(int layer, int howfar);
    void raiseLayerToTop(int layer);
    void lowerLayerToBottom(int layer);

    void clear();
    void draw();

    double getPixelsPerUnit() const;
    ICoord user2pixel(const Coord&) const;
    Coord pixel2user(const ICoord&) const;
    double user2pixel(double) const;
    double pixel2user(double) const;

    void setAntialias(bool);
    void setMargin(double);

    bool empty() const;
    size_t nVisibleItems() const;

    void setBackgroundColor(const Color&);

    bool saveAsPDF(const std::string &filename, int, bool);
    bool saveRegionAsPDF(const std::string &filename, int, bool,
			 const Coord&, const Coord&);
    bool saveAsPNG(const std::string &filename, int, bool);
    bool saveRegionAsPNG(const std::string &filename, int, bool,
			 const Coord&, const Coord&);

    std::vector<CanvasItem*> clickedItems(const Coord&) const;
    std::vector<CanvasItem*> allItems() const;
  };

};				// namespace OOFCanvas

#endif // OOFCANVAS_CANVAS_PUBLIC_H
