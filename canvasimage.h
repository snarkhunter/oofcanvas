// -*- C++ -*-

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modified
 * versions of this software, you first contact the authors at
 * oof_manager@nist.gov. 
 */

#ifndef OOFCANVASIMAGE_H
#define OOFCANVASIMAGE_H

#include "canvasitem.h"
#include "utility.h"
#include <string>

namespace OOFCanvas {

  // TODO: Need to load images from files as well as ImageMagick
  // internal data or other in-memory data.
  // TODO? Arbitrary rotation.

  class CanvasImage : public CanvasItem, public PixelSized {
  protected:
    std::string filename;
    Coord location;	      // lower-left corner in user coordinates
    Coord size;
    ICoord pixels;
    double opacity;
    bool pixelScaling;
    Cairo::RefPtr<Cairo::ImageSurface> imageSurface;
    virtual void drawItem(Cairo::RefPtr<Cairo::Context>) const;
    virtual bool containsPoint(const CanvasBase*, const Coord&) const;
  public:
    CanvasImage(const std::string &filename, double x, double y,
		double width, double height);
    virtual const std::string &classname() const;

    virtual const Rectangle &findBoundingBox(double ppu);
    void setPixelSize() { pixelScaling = true; }
    virtual bool pixelSized() const { return pixelScaling; }
    virtual Coord referencePoint() const { return location; }
    virtual void pixelExtents(double&, double&, double&, double&) const;
    void setOpacity(double alpha) { opacity = alpha; }
    friend std::ostream &operator<<(std::ostream&, const CanvasImage&);
    virtual std::string print() const;
  };

  std::ostream &operator<<(std::ostream&, const CanvasImage&);

};				// namespace OOFCanvas


#endif // OOFCANVASIMAGE_H
