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

#ifdef USE_IMAGEMAGICK
#include <Magick++.h>
#endif // USE_IMAGEMAGICK

namespace OOFCanvas {

  // TODO? Arbitrary rotation.

  class CanvasImage : public CanvasItem, public PixelSized {
  protected:
    Coord location;	      // lower-left corner in user coordinates
    Coord size;
    ICoord pixels;
    double opacity;
    bool pixelScaling;
    Cairo::RefPtr<Cairo::ImageSurface> imageSurface;
    virtual void drawItem(Cairo::RefPtr<Cairo::Context>) const;
    virtual bool containsPoint(const CanvasBase*, const Coord&) const;
    void setSizes(int, int, double, double);
  public:
    CanvasImage(double x, double y, double width, double height);
    
    virtual const Rectangle &findBoundingBox(double ppu);
    void setPixelSize() { pixelScaling = true; }
    virtual bool pixelSized() const { return pixelScaling; }
    virtual Coord referencePoint() const { return location; }
    virtual void pixelExtents(double&, double&, double&, double&) const;
    void setOpacity(double alpha) { opacity = alpha; }
  };

  class CanvasPNGImage : public CanvasImage {
  protected:
    std::string filename;
  public:
    CanvasPNGImage(const std::string &filename, double, double, double, double);
    virtual const std::string &classname() const;
    friend std::ostream &operator<<(std::ostream&, const CanvasPNGImage&);
    virtual std::string print() const;
  };

 #ifdef USE_IMAGEMAGICK
  class CanvasMagickImage : public CanvasImage {
  protected:
    Magick::Image image;
    unsigned char *buffer;
  public:
    CanvasMagickImage(Magick::Image, double, double, double, double);
    ~CanvasMagickImage();
    virtual const std::string &classname() const;
    friend std::ostream &operator<<(std::ostream&, const CanvasPNGImage&);
    virtual std::string print() const;
  };

  // For testing during development, this opens an image file using
  // ImageMagick and creates a CanvasMagickImage item.
  CanvasMagickImage *newCanvasMagickImage(const std::string &filename,
					  double, double, double, double);
  
#endif // USE_IMAGEMAGICK
  
  std::ostream &operator<<(std::ostream&, const CanvasImage&);

};				// namespace OOFCanvas


#endif // OOFCANVASIMAGE_H
