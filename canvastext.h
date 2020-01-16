// -*- C++ -*-

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modified
 * versions of this software, you first contact the authors at
 * oof_manager@nist.gov. 
 */

#ifndef OOF_CANVASTEXT_H
#define OOF_CANVASTEXT_H

#include "canvasitem.h"
#include "utility.h"

namespace OOFCanvas {
  class CanvasText : public CanvasItem {
  protected:
    const Coord location;
    const std::string text;
    double fontSize;
    std::string fontName;
    double angle;
    Color color;
    Cairo::FontSlant slant;
    Cairo::FontWeight weight;
    bool sizeInPixels; // is font size given in pixel units or user units?
    bool antiAlias;
    virtual void drawItem(Cairo::RefPtr<Cairo::Context>) const;
    virtual bool containsPoint(const CanvasBase*, const Coord&) const;
    void prepareContext(Cairo::RefPtr<Cairo::Context>) const;
  public:
    CanvasText(double, double, const std::string &text, double);
    virtual const std::string &classname() const;
    void setFillColor(const Color&);
    void setFont(const std::string &);
    void setWeight(Cairo::FontWeight wt);
    void setSlant(Cairo::FontSlant sl);
    void rotate(double);	// in degrees

    virtual const Rectangle &findBoundingBox(double ppu);

    // setSizeInPixels controls whether the font size is interpreted
    // in pixel units or user units.
    void setSizeInPixels(bool v) { sizeInPixels = v; }
    // Antialiasing is turned on by default.  Use this to turn it off.
    void setAntiAlias(bool v) { antiAlias = v; }
    friend std::ostream &operator<<(std::ostream&, const CanvasText&);
    virtual std::string *print() const;
  };

  std::ostream &operator<<(std::ostream&, const CanvasText&);

  extern const Cairo::FontSlant fontSlantNormal, fontSlantItalic,
			  fontSlantOblique;
  extern const Cairo::FontWeight fontWeightNormal, fontWeightBold;
};


#endif // OOF_CANVASTEXT_H
