// -*- C++ -*-

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modified
 * versions of this software, you first contact the authors at
 * oof_manager@nist.gov. 
 */

#include "canvastext.h"
#include <math.h>
#include <pango/pangocairo.h>

// TODO: Currently it's not possible to tell if a given point is on a
// CanvasText item, so text items aren't selectable by the mouse.  It
// is possible to tell if a point is inside a text item's bounding
// box, so text could be selected if nearby clicks are acceptable.
// However, it might be possible to render text the text to an
// otherwise empty surface and check the color of the selected point
// on the surface.

namespace OOFCanvas {

  CanvasText::CanvasText(double x, double y,
			 const std::string &txt,
			 double fsize)
    : location(x, y),
      text(txt),
      fontSize(fsize),
      fontName("sans-serif"),
      angle(0),
      color(black),
      slant(Cairo::FONT_SLANT_NORMAL),
      weight(Cairo::FONT_WEIGHT_NORMAL),
      sizeInPixels(false),
      antiAlias(true)
  {}

  const std::string &CanvasText::classname() const {
    static const std::string name("CanvasText");
    return name;
  }

  void CanvasText::rotate(double ang) {
    angle = M_PI/180.*ang;
    bbox.clear();
  }

  void CanvasText::setFillColor(const Color &c) {
    color = c;
  }

  void CanvasText::setFont(const std::string &name) {
    // According to the Cairo manual, "the standard CSS2 generic
    // family names, ('serif', 'sans-serif', 'cursive', 'fantasy',
    // 'monospace'), are likely to work as expected".
    fontName = name;
    bbox.clear();
  }

  void CanvasText::setWeight(Cairo::FontWeight wt) {
    weight = wt;
    bbox.clear();
  }

  void CanvasText::setSlant(Cairo::FontSlant sl) {
    slant = sl;
    bbox.clear();
  }
    

  // These are for use from python. See comments in oofcanvas.swg;
  const Cairo::FontSlant fontSlantNormal(Cairo::FontSlant::FONT_SLANT_NORMAL);
  const Cairo::FontSlant fontSlantItalic(Cairo::FontSlant::FONT_SLANT_ITALIC);
  const Cairo::FontSlant fontSlantOblique(Cairo::FontSlant::FONT_SLANT_OBLIQUE);
  const Cairo::FontWeight fontWeightNormal(Cairo::FontWeight::FONT_WEIGHT_NORMAL);
  const Cairo::FontWeight fontWeightBold(Cairo::FontWeight::FONT_WEIGHT_BOLD);


  PangoLayout *CanvasText::getLayout(Cairo::RefPtr<Cairo::Context> ctxt) const {
    PangoLayout *layout = pango_cairo_create_layout(ctxt->cobj());
    pango_layout_set_text(layout, text.c_str(), -1);
    PangoFontDescription *desc =
      pango_font_description_from_string(fontName.c_str());
    pango_layout_set_font_description(layout, desc);
    // std::cerr << "CanvasText::getLayout: font_desc="
    // 	      << pango_font_description_to_string(desc) << std::endl;
    // std::cerr << "CanvasText::getLayout: size="
    // 	      << pango_font_description_get_size(desc)
    // 	      << " absolute="
    // 	      << pango_font_description_get_size_is_absolute(desc)
    // 	      << " scale=" << PANGO_SCALE
    // 	      << std::endl;
    pango_font_description_free(desc);
    return layout;
    
    // Cairo::RefPtr<Cairo::ToyFontFace> font =
    //   Cairo::ToyFontFace::create(fontName, slant, weight);
    // ctxt->set_font_face(font);

    // if(sizeInPixels) {
    //   double fs = fontSize;
    //   double dummy = 0;
    //   ctxt->device_to_user_distance(fs, dummy);
    //   ctxt->set_font_size(fs);
    // }
    // else {
    //   ctxt->set_font_size(fontSize);
    // }

    // // By default, antialiasing is on.
    // if(!antiAlias) {
    //   Cairo::FontOptions options;
    //   options.set_antialias(Cairo::ANTIALIAS_NONE);
    //   ctxt->set_font_options(options);
    // }
  }
  
  void CanvasText::drawItem(Cairo::RefPtr<Cairo::Context> ctxt) const {
    PangoLayout *layout = getLayout(ctxt);
    color.set(ctxt);
    ctxt->move_to(location.x, location.y);
    ctxt->rotate(angle);
    ctxt->scale(1, -1);	// flip y, because fonts still think y goes down
    pango_cairo_show_layout(ctxt->cobj(), layout);
    g_object_unref(layout);
  }

  const Rectangle &CanvasText::findBoundingBox(double ppu) {
    // To get the bounding box, we need to have a Cairo::Context, but
    // we need the bounding box to figure out the dimensions of the
    // Cairo::Surface, from which we get the Context.  So create a
    // dummy Context here.

    if(bbox.initialized() && !sizeInPixels)
      return bbox;

    // Ths size of the Surface doesn't matter.  We need it to create
    // the Context, but the Context doesn't seem to need the Surface
    // to get the text size.
    auto surface = Cairo::RefPtr<Cairo::ImageSurface>(
		      Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32,
						  10, 10));
    cairo_t *ct = cairo_create(surface->cobj());
    auto ctxt = Cairo::RefPtr<Cairo::Context>(new Cairo::Context(ct, true));

    // Compute bounding box in the text's coordinates
    PangoLayout *layout = getLayout(ctxt);
    PangoRectangle ink_pango_rect, logical_pango_rect;
    pango_layout_get_extents(layout, &ink_pango_rect, &logical_pango_rect);
    bbox = Rectangle(logical_pango_rect);
    g_object_unref(layout);
    bbox.scale(1./PANGO_SCALE, 1./PANGO_SCALE);
    
    if(angle != 0.0) {
      // Find the Rectangle that contains the rotated bounding box,
      // before translating.
      Cairo::Matrix rot(Cairo::rotation_matrix(-angle));
      Rectangle rotatedBBox(bbox.lowerRight().transform(rot),
			    bbox.upperRight().transform(rot));
      rotatedBBox.swallow(bbox.upperLeft().transform(rot));
      rotatedBBox.swallow(bbox.lowerLeft().transform(rot));
      bbox = rotatedBBox;
    }
    // Put the bounding box into the OOFCanvas coordinate system
    if(sizeInPixels) {
      double factor = 1./ppu;
      bbox.scale(factor, -factor);
    }
    else
      bbox.scale(1.0, -1.0);
    bbox.shift(location);
    return bbox;
  }

  bool CanvasText::containsPoint(const CanvasBase*, const Coord&) const {
    return false;
  }

  std::string CanvasText::print() const {
    return to_string(*this);
  }

  std::ostream &operator<<(std::ostream &os, const CanvasText &text) {
    os << "CanvasText(\"" << text.text << "\")";
    return os;
  }

  std::vector<std::string> *list_fonts() {
    std::vector<std::string> *result = new std::vector<std::string>;
    PangoFontFamily **families;
    int n;
    PangoFontMap *fontmap = pango_cairo_font_map_get_default();
    pango_font_map_list_families(fontmap, &families, &n);
    for(int i=0; i<n; i++) {
      const char *family_name = pango_font_family_get_name(families[i]);
      result->emplace_back(family_name);
    }
    g_free(families);
    return result;
  }


};				// namespace OOFCanvas
