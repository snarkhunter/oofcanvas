// -*- C++ -*-

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modified
 * versions of this software, you first contact the authors at
 * oof_manager@nist.gov. 
 */

#include "canvasimage.h"

namespace OOFCanvas {

  CanvasImage::CanvasImage(const std::string &filename, double x, double y,
			   double width, double height)
    : filename(filename),
      location(Coord(x, y)),
      opacity(1.0),
      pixelScaling(false)
  {
    imageSurface = Cairo::RefPtr<Cairo::ImageSurface>(
			      Cairo::ImageSurface::create_from_png(filename));

    // "pixels" is dimensions of the loaded image, in pixels.  It's not
    // necessarily the same as the size of the displayed image.
    pixels.x = imageSurface->get_width();
    pixels.y = imageSurface->get_height();
    // "size" is the displayed size of the image, given by the
    // constructor arguments, width and height. They're assumed to be
    // in user units unless setPixelSize() is called, in which case
    // they're in device units. If one of height or width is negative,
    // it's computed from the other dimension and the aspect ratio,
    // assuming that pixels are square.  If both are negative, the
    // pixels are assumed to be 1x1.
    if(height <= 0 && width <= 0) {
      width = pixels.x;
      height = pixels.y;
    }
    else if(height <= 0) {
      height = width*pixels.y/pixels.x;
    }
    else if(width <= 0) {
      width = height*pixels.x/pixels.y;
    }
    size = Coord(width, height);
  }

  const std::string &CanvasImage::classname() const {
    static const std::string name("CanvasImage");
    return name;
  }

  const Rectangle &CanvasImage::findBoundingBox(double ppu) {
    if(pixelScaling)
      bbox = Rectangle(location, location + size/ppu);
    else
      bbox = Rectangle(location, location+size);
    return bbox;
  }

  void CanvasImage::drawItem(Cairo::RefPtr<Cairo::Context> ctxt) const {
    // Scaling the context to change the image size also changes the
    // location, so convert the location to device units before
    // scaling, then convert back afterwards.
    double posX, posY;
    if(!pixelScaling) {
      posX = location.x;
      posY = location.y + size.y;
      ctxt->user_to_device(posX, posY);
      ctxt->scale(size.x/pixels.x, -size.y/pixels.y);
      ctxt->device_to_user(posX, posY);
    }
    else {
      // Given size is in device pixels
      // Find the size (dx, dy) of a pixel in user coordinates
      double dx = 1.0;		
      double dy = 1.0;
      ctxt->device_to_user_distance(dx, dy);
      // dy is negative now.

      // Get the desired display position in device coordinates
      posX = location.x;
      posY = location.y;
      ctxt->user_to_device(posX, posY);

      // Scaling x by dx would make image pixels correspond to device
      // pixels, so scale by dx*size.x/pixels.x to make the image fit
      // into size.x device pixels.
      ctxt->scale(dx*size.x/pixels.x, dy*size.y/pixels.y);
      posY -= size.y;
      
      // Convert the display position back to user coordinates
      ctxt->device_to_user(posX, posY);
    }
    ctxt->set_source(imageSurface, posX, posY);
    if(opacity == 1.0)
      ctxt->paint();
    else
      ctxt->paint_with_alpha(opacity);
  }

  void CanvasImage::pixelExtents(double &left, double &right,
				 double &up, double &down)
    const
  {
    // Only called if pixelScaling==true
    left = 0.0;
    right = size.x;
    up = size.y;
    down = 0.0;
  }

  bool CanvasImage::containsPoint(const CanvasBase*, const Coord&) const {
    // This isn't called unless the point is within the bounding box,
    // and images fill their bounding boxes, so there's nothing to do
    // here.
    // TODO: That's not necessarily true if we allow images to be
    // rotated by arbitrary angles.
    return true;
  }

  std::string CanvasImage::print() const {
    return to_string(*this);
  }

  std::ostream &operator<<(std::ostream &os, const CanvasImage &image) {
    os << "CanvasImage(\"" << image.filename << "\")";
    return os;
  }

};				// namespace OOFCanvas
