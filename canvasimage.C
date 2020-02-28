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
#include <stdlib.h>

namespace OOFCanvas {

  CanvasImage::CanvasImage(double x, double y)
    : location(Coord(x, y)),	// position of lower left corner in user units
      opacity(1.0),
      pixelScaling(false)
  {}

  void CanvasImage::setSizes(int xpix, int ypix, double width, double height) {
    // "size" is the displayed size of the image, given by the
    // width and height. They're assumed to be
    // in user units unless setPixelSize() is called, in which case
    // they're in device units. If one of height or width is negative,
    // it's computed from the other dimension and the aspect ratio,
    // assuming that pixels are square.  If both are negative, the
    // pixels are assumed to be 1x1.
    pixels = ICoord(xpix, ypix);
    if(height <= 0 && width <= 0) {
      width = xpix;
      height = ypix;
    }
    else if(height <= 0) {
      height = width*ypix/xpix;
    }
    else if(width <= 0) {
      width = height*xpix/ypix;
    }
    size = Coord(width, height);
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

  //=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

  // CanvasPNGImage creates a canvas item from a png file, using
  // Cairo's built-in png machinery.

  CanvasPNGImage::CanvasPNGImage(const std::string &filename,
			   double x, double y, // position in user units
			   double width, double height) // size
    : filename(filename),
      CanvasImage(x, y)
  {
    imageSurface = Cairo::ImageSurface::create_from_png(filename);

    // "pixels" is dimensions of the loaded image, in pixels.  It's not
    // necessarily the same as the size of the displayed image.
    pixels.x = imageSurface->get_width();
    pixels.y = imageSurface->get_height();

    setSizes(imageSurface->get_width(), imageSurface->get_height(),
	    width, height);
  }

  const std::string &CanvasPNGImage::classname() const {
    static const std::string name("CanvasPNGImage");
    return name;
  }

  std::string CanvasPNGImage::print() const {
    return to_string(*this);
  }

  std::ostream &operator<<(std::ostream &os, const CanvasPNGImage &image) {
    os << "CanvasPNGImage(\"" << image.filename << "\")";
    return os;
  }

  //=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//
  
 #ifdef USE_IMAGEMAGICK

  // CanvasMagickImage creates a canvas item from an ImageMagick
  // image.  The first constructor reads the image from a file.  The
  // second uses an existing ImageMagick::Image object.
  
  CanvasMagickImage::CanvasMagickImage(
			       const std::string &filename,
			       double x, double y, // position in user units
			       double width, double height)
    : buffer(nullptr),
      CanvasImage(x, y)
  {
    Magick::Image image;
    image.read(filename);
    loadImage(image, width, height);
  }

  CanvasMagickImage::CanvasMagickImage(Magick::Image image,
				       double x, double y,
				       double width, double height)
    : buffer(nullptr),
      CanvasImage(x, y)
  {
    loadImage(image, width, height);
  }
				       
  
  void CanvasMagickImage::loadImage(Magick::Image image,
				    double width, double height)
  {
    // width and height are the size of the displayed image.  w and h
    // are the size in pixels of the source image.
    Magick::Geometry sz = image.size();
    int w = sz.width();
    int h = sz.height();

    // Create a data buffer for the ImageSurface to use.  The buffer
    // must not be deleted until the ImageSurface is destroyed.
    int stride = Cairo::ImageSurface::format_stride_for_width(
				      Cairo::FORMAT_ARGB32, w);
    unsigned char *buffer = (unsigned char*) malloc(stride * h);
    
    Magick::PixelPacket *pixpax = image.getPixels(0, 0, w, h);
    using namespace Magick;
    double scale = 1./QuantumRange * 255;

    // Copy pixel data from ImageMagick to the Cairo buffer.

    // Cairo uses libpixman for image storage.  There is no
    // documentation for libpixman.

    // See _cairo_format_from_pixman_format() in cairo-image-surface.c
    // in the Cairo source for the correspondence between pixman and
    // Cairo formats.  Cairo::FORMAT_ARGB32 corresponds to
    // PIXMAN_a8r8g8b8.

    // From https://afrantzis.com/pixel-format-guide/pixman.html:
    // The pixel is represented by a 32-bit value, with A in bits
    // 24-31, R in bits 16-23, G in bits 8-15 and B in bits 0-7.
    // On little-endian systems the pixel is stored in memory as the
    // bytes B, G, R, A (B at the lowest address, A at the highest).
    // On big-endian systems the pixel is stored in memory as the
    // bytes A, R, G, B (A at the lowest address, B at the highest).
    assert(sizeof(int) == 4);

    // Are we big or little endian?
    int k = 1;
    unsigned char *c = (unsigned char*) &k;
    bool littleEndian = *c;

    if(littleEndian) {
      for(int j=0; j<h; j++) {
	for(int i=0; i<w; i++) {
	  const Magick::PixelPacket *pp = pixpax + (i + j*w);
	  unsigned char *addr = buffer + j*stride + 4*i;
	  *addr++ = pp->blue*scale;
	  *addr++ = pp->green*scale;
	  *addr++ = pp->red*scale;
	  *addr   = 255;	// alpha
	}
      }
    }
    else {			// big endian
      for(int j=0; j<h; j++) {
	for(int i=0; i<w; i++) {
	  const Magick::PixelPacket *pp = pixpax + i + j*w;
	  unsigned char *addr = buffer + j*stride + 4*i;
	  *addr++ = 255;	// alpha
	  *addr++ = pp->red*scale;
	  *addr++ = pp->green*scale;
	  *addr   = pp->blue*scale;
	}
      }
    }
	
    imageSurface = Cairo::ImageSurface::create(buffer,
					       Cairo::FORMAT_ARGB32,
					       w, h, stride);
    setSizes(w, h, width, height);
  }

  CanvasMagickImage::~CanvasMagickImage() {
    free(buffer);
  }

  const std::string &CanvasMagickImage::classname() const {
    static const std::string name("CanvasMagickImage");
    return name;
  }
							  
  std::string CanvasMagickImage::print() const {
    return to_string(*this);
  }

  std::ostream &operator<<(std::ostream &os, const CanvasMagickImage &image) {
    os << "CanvasMagickImage()";
    return os;
  }

#endif // USE_IMAGEMAGICK

  
};				// namespace OOFCanvas
