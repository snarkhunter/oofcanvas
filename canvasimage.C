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

    pixels.x = imageSurface->get_width();
    pixels.y = imageSurface->get_height();
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
    std::cerr << "CanvasImage::findBoundingBox: bbox=" << bbox << std::endl;
    return bbox;
  }

  void CanvasImage::drawItem(Cairo::RefPtr<Cairo::Context> ctxt) const {
    double posX, posY;
    if(!pixelScaling) {
      ctxt->scale(size.x/pixels.x, -size.y/pixels.y);
      posX = location.x;
      posY = location.y + size.y;
    }
    else {
      // size is in pixels
      double dx = 1.0;
      double dy = 1.0;
      ctxt->device_to_user_distance(dx, dy);
      ctxt->scale(dx, dy);
      posX = location.x;
      posY = location.y + pixels.y*dy;
    }
    // ctxt->move_to(location.x, location.y);
    //ctxt->move_to(posX, posY);
    ctxt->set_source(imageSurface, posX, posY);
    std::cerr << "CanvasImage::drawItem: matrix="
	      << ctxt->get_matrix() << std::endl;
    std::cerr << "CanvasImage::drawItem: pos=" << Coord(posX, posY)
	      << std::endl;
    if(opacity == 1.0)
      ctxt->paint();
    else
      ctxt->paint_with_alpha(opacity);

    // double x0, x1, y0, y1;
    // ctxt->get_clip_extents(x0, y0, x1, y1);
    // std::cerr << "CanvasImage::drawItem: clip extents="
    // 	      << Rectangle(x0, y0, x1, y1) << std::endl;
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
