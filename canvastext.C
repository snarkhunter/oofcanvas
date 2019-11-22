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

namespace OOFCanvas {

  CanvasText::CanvasText(double x, double y,
			 const std::string &txt,
			 double fsize)
    : location(x, y),
      text(txt),
      fontSize(fsize),
      angle(0),
      sizeInPixels(false),
      color(black)
  {}

  const std::string &CanvasText::classname() const {
    static const std::string name("CanvasText");
    return name;
  }

  void CanvasText::rotate(double ang) {
    angle = M_PI/180.*ang;
  }

  void CanvasText::setFillColor(const Color &c) {
    color = c;
  }

  void CanvasText::drawItem(Cairo::RefPtr<Cairo::Context> ctxt) {
    color.set(ctxt);
    Cairo::RefPtr<Cairo::ToyFontFace> font =
      Cairo::ToyFontFace::create("Sans",
				 Cairo::FONT_SLANT_NORMAL,
				 Cairo::FONT_WEIGHT_NORMAL);
    ctxt->set_font_face(font);

    if(sizeInPixels) {
      double fs = fontSize;
      double dummy = 0;
      ctxt->device_to_user_distance(fs, dummy);
      ctxt->set_font_size(fs);
    }
    else {
      ctxt->set_font_size(fontSize);
    }

    ctxt->move_to(location.x, location.y);
    ctxt->rotate(angle);
    ctxt->scale(1, -1);	// flip y, because fonts still think y goes down
    ctxt->show_text(text);

    Cairo::TextExtents extents;
    ctxt->get_text_extents(text, extents);
    // std::cerr << "CanvasText::drawItem: x_bearing=" << extents.x_bearing
    // 	      << " y_bearing=" << extents.y_bearing
    // 	      << " width=" << extents.width
    // 	      << " height=" << extents.height
    // 	      << " x_advance=" << extents.x_advance
    // 	      << " y_advance=" << extents.y_advance
    // 	      << std::endl;
    Coord oppositeCorner = location + Coord(extents.width, extents.height);
    bbox = Rectangle(location, oppositeCorner);
  }

  bool CanvasText::containsPoint(const Canvas*, const Coord&) const {
    return false;
  }

  std::string *CanvasText::print() const {
    return new std::string(to_string(*this));
  }

  std::ostream &operator<<(std::ostream &os, const CanvasText &text) {
    os << "CanvasText(\"" << text.text << "\")";
    return os;
  }

};				// namespace OOFCanvas
