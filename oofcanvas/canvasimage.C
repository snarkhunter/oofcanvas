// -*- C++ -*-

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modified
 * versions of this software, you first contact the authors at
 * oof_manager@nist.gov. 
 */

#include "oofcanvas/canvasimpl.h"
#include "oofcanvas/canvasimage.h"
#include "oofcanvas/canvasitemimpl.h"
#include <cassert>
#include <stdlib.h>

namespace OOFCanvas {

  class CanvasImageImplementation :
    public CanvasItemImplementation<CanvasImage>
  {
  public:
    CanvasImageImplementation(CanvasImage *image, const Rectangle &bb)
      : CanvasItemImplementation<CanvasImage>(image, bb),
	buffer(nullptr),
	stride(0)
    {}

    virtual ~CanvasImageImplementation() {}
    
    Cairo::RefPtr<Cairo::ImageSurface> imageSurface;
    unsigned char *buffer; // points to data owned by Cairo::ImageSurface
    int stride;

    virtual void pixelExtents(double&, double&, double&, double&) const;
    virtual void drawItem(Cairo::RefPtr<Cairo::Context>) const;
    virtual bool containsPoint(const OSCanvasImpl*, const Coord&) const;
    void setUp(Cairo::RefPtr<Cairo::ImageSurface>,
	       double, double);	// displayed size
    void setSurface(Cairo::RefPtr<Cairo::ImageSurface>, const ICoord&);

    // set the color of a single pixel
    void set(const ICoord&, const Color&);
    Color get(const ICoord&) const;
  };

  //=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

  static bool getLittleEndian() {
    int k = 1;
    unsigned char *c = (unsigned char*) &k;
    return *c;
  }

  static bool littleEndian = getLittleEndian();

  //=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

  CanvasImage::CanvasImage(const Coord &loc, const ICoord &pix)
    : CanvasItem(new CanvasImageImplementation(this, Rectangle(loc, loc))),
      location(loc),	// position of lower left corner in user units
      size(-1, -1),	// illegal, will be reset by setSize or setSizeInPixels
      pixels(pix),
      opacity(1.0),
      pixelScaling(true),	// will be reset by setSize or setSizeInPixels
      drawPixelByPixel(false)
  {}

  CanvasImage::~CanvasImage() {}

  const std::string &CanvasImage::classname() const {
    static std::string nm("CanvasImage");
    return nm;
  }

  void CanvasImage::setDrawIndividualPixels(bool flag) {
    drawPixelByPixel = flag;
  }
  
  //=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

  // If an image's physical (displayed) size has one or more negative
  // components, the actual value of that component is inferred from
  // the size in pixels, assuming that pixels are square.

  static Coord imageSize_(const Coord &size, const ICoord &pixsize) {
    if(size[0] <= 0 && size[1] <=0)
      return Coord(pixsize[0], pixsize[1]); // assume pixels are 1x1 
    if(size[1] <= 0)
      return Coord(size[0], size[0]*pixsize[1]/pixsize[0]); // assume square
    if(size[0] <= 0)
      return Coord(size[1]*pixsize[0]/pixsize[1], size[1]); // assume square
    return Coord(size[0], size[1]);
  }

  void CanvasImage::setSize(const Coord &sz) {
    size = imageSize_(sz, pixels);
    pixelScaling = false;
    implementation->bbox = Rectangle(location, location + size);
    modified();
  }

  void CanvasImage::setSizeInPixels(const Coord &sz) {
    size = imageSize_(sz, pixels);
    pixelScaling = true;
    implementation->bbox = Rectangle(location, location);
    modified();
  }

  //=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

  // The CanvasImage::set methods set the color and opacity of a
  // single pixel.  They probably don't work correctly unless the
  // Cairo image format is FORMAT_ARGB32.

  void CanvasImage::set(const ICoord &pt, const Color &color) {
    dynamic_cast<CanvasImageImplementation*>(implementation)->set(pt, color);
  }

  void CanvasImageImplementation::set(const ICoord &pt, const Color &color) {
    assert(buffer != nullptr);
    assert(stride != 0);
    unsigned char *addr = buffer + pt.y*stride + 4*pt.x;
    if(littleEndian) {
      *addr++ = color.blue*255;
      *addr++ = color.green*255;
      *addr++ = color.red*255;
      *addr   = color.alpha*255;
    }
    else {
      *addr++ = color.alpha*255;
      *addr++ = color.red*255;
      *addr++ = color.green*255;
      *addr   = color.blue*255;
    }
    imageSurface->mark_dirty();
    canvasitem->modified();
  }
  
  Color CanvasImage::get(const ICoord &pt) const {
    return dynamic_cast<CanvasImageImplementation*>(implementation)->get(pt);
  }

  Color CanvasImageImplementation::get(const ICoord &pt) const {
    assert(buffer != nullptr);
    assert(stride != 0);
    unsigned char *addr = buffer + pt.y*stride + 4*pt.x;
    unsigned char r, g, b, a;
    if(littleEndian) {
      b = *addr++;
      g = *addr++;
      r = *addr++;
      a = *addr;
    }
    else {
      a = *addr++;
      r = *addr++;
      g = *addr++;
      b = *addr;
    }
    return Color(r/255., g/255., b/255., a/255.);
  }

  //=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

  void CanvasImageImplementation::drawItem(Cairo::RefPtr<Cairo::Context> ctxt)
    const
  {
    // The native Cairo image drawing method antialiases each pixel --
    // maybe that's not the right term for it, but each pixel is
    // blurry if you zoom way in.  This may be what you want if you're
    // showing pictures from your vacation, but if the pixels are
    // individual data points that you are examining, you don't want
    // to antialias them.  If drawPixelByPixel is false, then the
    // native Cairo rendering is used.  If it's true, then the pixels
    // are drawn as individual filled rectangles.

    // Drawing the individual rectangles has three possible drawbacks.
    // First, it might be slow (but I haven't actually timed it).
    // Second, it can lead to aliasing problems in which adjacent
    // image pixels don't actually meet when drawn on the screen,
    // producing a stripe of the background color across the image.
    // This can often be fixed by scaling the whole image by a factor
    // very close to one.  Third, when converted to pdf or some other
    // printable format, the boundaries between the image pixels can
    // sometimes be seen.

    // The default behavior of CanvasImage is to use the Cairo
    // rendering and not draw the individual pixels.  To change it,
    // call CanvasImage::setDrawIndividualPixels(true).

    const Coord &size(canvasitem->getSize());
    const ICoord &pixels(canvasitem->getSizeInPixels());
    const Coord &location(canvasitem->getLocation());
    
    assert(size.x > 0.0 && size.y > 0.0); // setSize or setSizeInPixels needed
    
    if(!canvasitem->getDrawPixelByPixel()) {
      // Scaling the context to change the image size also changes the
      // location, so convert the location to device units before
      // scaling, then convert back afterwards.
      double posX, posY;
      if(!canvasitem->getPixelScaling()) {
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
      if(canvasitem->getOpacity() == 1.0)
	ctxt->paint();
      else
	ctxt->paint_with_alpha(canvasitem->getOpacity());
    }
    else {
      // drawPixelByPixel==true
      ctxt->save();
      ctxt->set_antialias(Cairo::ANTIALIAS_NONE);
      double dx = size.x/pixels.x;
      double dy = size.y/pixels.y;
      if(canvasitem->getPixelScaling()) {
	ctxt->device_to_user_distance(dx, dy); // changes sign of dy
	dy *= -1;
      }

      for(unsigned int j=0; j<pixels.y; j++) {
	for(unsigned int i=0; i<pixels.x; i++) {
	  Color clr = get(ICoord(i, pixels.y-j-1));
	  setColor(clr, ctxt);
	  ctxt->move_to(location.x + i*dx, location.y+j*dy);
	  ctxt->rel_line_to(dx, 0);
	  ctxt->rel_line_to(0, dy);
	  ctxt->rel_line_to(-dx, 0);
	  ctxt->close_path();
	  ctxt->fill();
	}
      }
      ctxt->restore();

    }
  } // CanvasImageImplementation::drawItem()

  void CanvasImageImplementation::pixelExtents(double &left, double &right,
					       double &up, double &down)
    const
  {
    left = 0.0;
    down = 0.0;
    if(canvasitem->getPixelScaling()) {
      const Coord &size = canvasitem->getSize();
      right = size.x;
      up = size.y;
    }
    else {
      right = 0.0;
      up = 0.0;
    }
  }

  bool CanvasImageImplementation::containsPoint(const OSCanvasImpl*,
						const Coord&)
    const
  {
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
  
  std::ostream &operator<<(std::ostream &os, const CanvasImage &canvasImage) {
    os << "CanvasImage(pixels=" << canvasImage.pixels
       << ", size=" << canvasImage.size
       << ", position=" << canvasImage.location
       << ")";
    return os;
  }

  //=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

  // Utility function used when creating CanvasImages

  void CanvasImageImplementation::setSurface(
				     Cairo::RefPtr<Cairo::ImageSurface> surf,
				     const ICoord &pixsize)
  {
    imageSurface = surf;
    buffer = surf->get_data();
    stride = Cairo::ImageSurface::format_stride_for_width(surf->get_format(),
							  pixsize.x);
  }

  //=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

  // Factory methods for creating CanvasImages from different sources

  // static
  CanvasImage *CanvasImage::newBlankImage(
			  const Coord &position, // user coords
			  const ICoord &pixsize, // size in pixels
			  const Color &color)
  {
    CanvasImage *canvasImage = new CanvasImage(position, pixsize);
    CanvasImageImplementation *impl =
      dynamic_cast<CanvasImageImplementation*>(canvasImage->implementation);
    Cairo::RefPtr<Cairo::ImageSurface> surf =
      Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, pixsize[0], pixsize[1]);
    impl->setSurface(surf, pixsize);

    double r = color.red;
    double g = color.green;
    double b = color.blue;
    double a = color.alpha;

    int stride = impl->stride;
    unsigned char *buffer = impl->buffer;
    if(littleEndian) {
      for(int j=0; j<pixsize[1]; j++) {
	unsigned char *rowaddr = buffer + j*stride;
	for(int i=0; i<pixsize[0]; i++) {
	  unsigned char *addr = rowaddr + 4*i;
	  // byte order is BGRA
	  *addr++ = b*255;
	  *addr++ = g*255;
	  *addr++ = r*255;
	  *addr =   a*255;
	}
      }
    }
    else {			// big-endian
      for(int j=0; j<pixsize[1]; j++) {
	unsigned char *rowaddr = buffer + j*stride;
	for(int i=0; i<pixsize[0]; i++) {
	  unsigned char *addr = rowaddr + 4*i;
	  // byte order is ARGB
	  *addr++ = a*255;
	  *addr++ = r*255;
	  *addr++ = g*255;
	  *addr++ = b*255;
	}
      }

    }
    impl->imageSurface->mark_dirty();
    return canvasImage;
  }

  // static.  Same as above, but with pointer args for swig compatibility.
  CanvasImage *CanvasImage::newBlankImage(
			  const Coord *position, // user coords
			  const ICoord *pixsize, // size in pixels
			  const Color &color)
  {
    return newBlankImage(*position, *pixsize, color);
  }

  // static
  CanvasImage *CanvasImage::newFromPNGFile(const Coord &position, // lowerleft
					   const std::string &filename)
  {
    // Read the file first, to get the size in pixels.
    Cairo::RefPtr<Cairo::ImageSurface> surf =
      Cairo::ImageSurface::create_from_png(filename);
    ICoord pixsize(surf->get_width(), surf->get_height());
    CanvasImage *canvasImage = new CanvasImage(position, pixsize);
    CanvasImageImplementation *impl =
      dynamic_cast<CanvasImageImplementation*>(canvasImage->implementation);
    impl->setSurface(surf, pixsize);
    return canvasImage;
  }

  // static.  Same as above, but with pointer args for swig compatibility.
  CanvasImage *CanvasImage::newFromPNGFile(const Coord *position, // lowerleft
					   const std::string &filename)
  {
    return newFromPNGFile(*position, filename);
  }

#ifdef OOFCANVAS_USE_IMAGEMAGICK

  // static
  CanvasImage *CanvasImage::newFromImageMagickFile(const Coord &position,
						   const std::string &filename)
  {
    Magick::Image image;	// reference counted
    image.read(filename);
    return CanvasImage::newFromImageMagick(position, image);
  }

  // static.  Same as above, but with pointer args for swig compatibility.
  CanvasImage *CanvasImage::newFromImageMagickFile(const Coord *position,
						   const std::string &filename)
  {
    return newFromImageMagickFile(*position, filename);
    
  }

  // static
  CanvasImage *CanvasImage::newFromImageMagick(const Coord &position,
					       Magick::Image image)
  {
    Magick::Geometry sz = image.size();
    ICoord pixsize(sz.width(), sz.height());
    CanvasImage *canvasImage = new CanvasImage(position, pixsize);
    CanvasImageImplementation *impl =
      dynamic_cast<CanvasImageImplementation*>(canvasImage->implementation);
    int w = pixsize[0];
    int h = pixsize[1];
    
    Cairo::RefPtr<Cairo::ImageSurface> surf =
      Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, w, h);
    impl->setSurface(surf, pixsize);

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
    
    unsigned char *buffer = impl->buffer;
    using namespace Magick;	// Magick::QuantumRange doesn't work?
    double scale = 255./QuantumRange;
    Magick::PixelPacket *pixpax = image.getPixels(0, 0, w, h);
    int stride = impl->stride;
    if(littleEndian) {
      for(int j=0; j<h; j++) {
	unsigned char *rowaddr = buffer + j*stride;
	for(int i=0; i<w; i++) {
	  const Magick::PixelPacket *pp = pixpax + (i + j*w);
	  unsigned char *addr = rowaddr + 4*i;
	  *addr++ = pp->blue*scale;
	  *addr++ = pp->green*scale;
	  *addr++ = pp->red*scale;
	  *addr   = 255;	// alpha
	}
      }
    }
    else {			// big endian
      for(int j=0; j<h; j++) {
	unsigned char *rowaddr = buffer + j*stride;
	for(int i=0; i<w; i++) {
	  const Magick::PixelPacket *pp = pixpax + i + j*w;
	  unsigned char *addr = rowaddr + 4*i;
	  *addr++ = 255;	// alpha
	  *addr++ = pp->red*scale;
	  *addr++ = pp->green*scale;
	  *addr   = pp->blue*scale;
	}
      }
    }
    impl->imageSurface->mark_dirty();

    return canvasImage;
  }

#endif // OOFCANVAS_USE_IMAGEMAGICK
  
};				// namespace OOFCanvas
