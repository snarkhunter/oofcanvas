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
  class CanvasLayerPublic;
  class OffScreenCanvas;
  class Color;
  class ICoord;
  class Coord;
  class CanvasItem;
  

  class CanvasPublic {
  private:
    OffScreenCanvas *theCanvas;
  public:
    CanvasPublic(double);
    ~CanvasPublic();

    CanvasLayerPublic *newLayer(const std::string&);
    void deleteLayer(CanvasLayerPublic*);
    CanvasLayerPublic *getLayer(int) const;
    CanvasLayerPublic *getLayer(const std::string&) const;
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

  //=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

  // TODO: Move this into a separate file?

  // CanvasLayerPublic is an abstract base classes that contains the
  // public interface for CanvasLayer.

  class CanvasLayerPublic {
  public:
    CanvasLayerPublic(const std::string&);
    virtual ~CanvasLayerPublic() {}
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
      const;

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

  std::ostream &operator<<(std::ostream&, const CanvasLayerPublic&);

};				// namespace OOFCanvas

#endif // OOFCANVAS_CANVAS_PUBLIC_H
