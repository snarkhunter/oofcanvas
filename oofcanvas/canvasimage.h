// -*- C++ -*-

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modified
 * versions of this software, you first contact the authors at
 * oof_manager@nist.gov. 
 */

#ifndef OOFCANVAS_IMAGE_H
#define OOFCANVAS_IMAGE_H

#include "oofcanvas/canvasitem.h"
#include "oofcanvas/utility.h"
#include <string>

#ifdef OOFCANVAS_USE_IMAGEMAGICK
#include <Magick++.h>
#endif // OOFCANVAS_USE_IMAGEMAGICK

#ifdef OOFCANVAS_USE_NUMPY
// This suppresses deprecation warnings when using numpy versions 
// older than 2.0.  Eventually we'll stop supporting the old versons.
#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION

#include <Python.h>
#include <numpy/arrayobject.h>
#endif // OOFCANVAS_USE_NUMPY

namespace OOFCanvas {

  // TODO? Arbitrary rotation.

  class CanvasImage : public CanvasItem {
  protected:
    CanvasCoord location;	      // lower-left corner in user coordinates
    CanvasCoord size;
    ICanvasCoord pixels;
    double opacity;
    bool pixelScaling;
    bool drawPixelByPixel;
#ifdef OOFCANVAS_USE_NUMPY
    PyArrayObject *nparray;
#endif // OOFCANVAS_USE_NUMPY
  public:
#ifdef OOFCANVAS_USE_NUMPY
    CanvasImage(const CanvasCoord &pos, const ICanvasCoord &npixels, PyArrayObject *ndarray);
#endif
    CanvasImage(const CanvasCoord &pos, const ICanvasCoord &npixels);
    CanvasImage(const CanvasImage&) = delete;
    virtual ~CanvasImage();
    virtual const std::string &classname() const;

    void setSize(const CanvasCoord&);
    void setSizeInPixels(const CanvasCoord&);
    void setSize(const CanvasCoord *sz) { setSize(*sz); }
    void setSizeInPixels(const CanvasCoord *sz) { setSizeInPixels(*sz); }

    const CanvasCoord& getSize() const { return size; }
    const ICanvasCoord& getSizeInPixels() const { return pixels; }
    const CanvasCoord& getLocation() const { return location; }
    double getOpacity() const { return opacity; }
    bool getPixelScaling() const { return pixelScaling; }
    bool getDrawPixelByPixel() const { return drawPixelByPixel; }
    
    void setDrawIndividualPixels(bool flag);

    // set the color of a single pixel
    void set(const ICanvasCoord&, const CanvasColor&);
    CanvasColor get(const ICanvasCoord&) const;

    // overall opacity
    void setOpacity(double alpha) { opacity = alpha; }

    static CanvasImage *newBlankImage(const CanvasCoord&, // position
				      const ICanvasCoord&,// no. of pixels
				      const CanvasColor&);

    static CanvasImage *newFromPNGFile(const CanvasCoord&,	   // position
				       const std::string&); // filename

    // versions with pointer args are used from python
    static CanvasImage *newBlankImage(const CanvasCoord*, // position
				      const ICanvasCoord*,	 // no. of pixels
				      const CanvasColor&);

    static CanvasImage *newFromPNGFile(const CanvasCoord*,	   // position
				       const std::string&); // filename
    
#ifdef OOFCANVAS_USE_IMAGEMAGICK
    static CanvasImage *newFromImageMagickFile(const CanvasCoord&, // position
					       const std::string&); // filename

    static CanvasImage *newFromImageMagickFile(const CanvasCoord*, // position
					       const std::string&); // filename
    
    static CanvasImage *newFromImageMagick(const CanvasCoord&,	// position
					   Magick::Image);
#endif // OOFCANVAS_USE_IMAGEMAGICK

#ifdef OOFCANVAS_USE_NUMPY
    static CanvasImage *newFromNumpy(const CanvasCoord*, // position
				     PyArrayObject*, bool);
    static CanvasImage *newFromNumpy(const CanvasCoord&, // position
				     PyArrayObject*, bool);
#endif // OOFCANVAS_USE_NUMPY


    friend std::ostream &operator<<(std::ostream&, const CanvasImage&);
    virtual std::string print() const;
  };
  
  std::ostream &operator<<(std::ostream&, const CanvasImage&);

};				// namespace OOFCanvas


#endif // OOFCANVAS_IMAGE_H
