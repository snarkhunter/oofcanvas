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
// TODO: Delete these comemnts?  
// Setting NPY_NO_DEPRECATED_API to NPY_1_1_API_VERSION suppresses
// *all* numpy deprecation warnings, which is probably not a good
// idea.  Not defining NPY_NO_DEPRECATED_API produces deprecation
// warnings, and the suggestion to set NPY_NO_DEPRECATED_API to
// NPY_1_7_API_VERSION.  But with that setting PyArray_NDIM and
// PyArray_DIMS aren't defined (maybe the arg types changed?)
//#define NPY_NO_DEPRECATED_API NPY_1_1_API_VERSION
#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION

#include <Python.h>
#include <numpy/arrayobject.h>
#endif // OOFCANVAS_USE_NUMPY

namespace OOFCanvas {

  // TODO? Arbitrary rotation.

  class CanvasImage : public CanvasItem {
  protected:
    Coord location;	      // lower-left corner in user coordinates
    Coord size;
    ICoord pixels;
    double opacity;
    bool pixelScaling;
    bool drawPixelByPixel;
#ifdef OOFCANVAS_USE_NUMPY
    PyArrayObject *nparray;
#endif // OOFCANVAS_USE_NUMPY
  public:
#ifdef OOFCANVAS_USE_NUMPY
    CanvasImage(const Coord &pos, const ICoord &npixels, PyArrayObject *ndarray);
#endif
    CanvasImage(const Coord &pos, const ICoord &npixels);
    CanvasImage(const CanvasImage&) = delete;
    virtual ~CanvasImage();
    virtual const std::string &classname() const;

    void setSize(const Coord&);
    void setSizeInPixels(const Coord&);
    void setSize(const Coord *sz) { setSize(*sz); }
    void setSizeInPixels(const Coord *sz) { setSizeInPixels(*sz); }

    const Coord& getSize() const { return size; }
    const ICoord& getSizeInPixels() const { return pixels; }
    const Coord& getLocation() const { return location; }
    double getOpacity() const { return opacity; }
    bool getPixelScaling() const { return pixelScaling; }
    bool getDrawPixelByPixel() const { return drawPixelByPixel; }
    
    void setDrawIndividualPixels(bool flag);

    // set the color of a single pixel
    void set(const ICoord&, const Color&);
    Color get(const ICoord&) const;

    // overall opacity
    void setOpacity(double alpha) { opacity = alpha; }

    static CanvasImage *newBlankImage(const Coord&, // position
				      const ICoord&,// no. of pixels
				      const Color&);

    static CanvasImage *newFromPNGFile(const Coord&,	   // position
				       const std::string&); // filename

    // versions with pointer args are used from python
    static CanvasImage *newBlankImage(const Coord*, // position
				      const ICoord*,	 // no. of pixels
				      const Color&);

    static CanvasImage *newFromPNGFile(const Coord*,	   // position
				       const std::string&); // filename
    
#ifdef OOFCANVAS_USE_IMAGEMAGICK
    static CanvasImage *newFromImageMagickFile(const Coord&, // position
					       const std::string&); // filename

    static CanvasImage *newFromImageMagickFile(const Coord*, // position
					       const std::string&); // filename
    
    static CanvasImage *newFromImageMagick(const Coord&,	// position
					   Magick::Image);
#endif // OOFCANVAS_USE_IMAGEMAGICK

#ifdef OOFCANVAS_USE_NUMPY
    static CanvasImage *newFromNumpy(const Coord*, // position
				     PyArrayObject*, bool);
    static CanvasImage *newFromNumpy(const Coord&, // position
				     PyArrayObject*, bool);
#endif // OOFCANVAS_USE_NUMPY


    friend std::ostream &operator<<(std::ostream&, const CanvasImage&);
    virtual std::string print() const;
  };
  
  std::ostream &operator<<(std::ostream&, const CanvasImage&);

};				// namespace OOFCanvas


#endif // OOFCANVAS_IMAGE_H
