// -*- C++ -*-

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modified
 * versions of this software, you first contact the authors at
 * oof_manager@nist.gov. 
 */

#ifndef OOFCANVAS_TEXT_H
#define OOFCANVAS_TEXT_H

#include "oofcanvas/canvasitem.h"
#include "oofcanvas/utility.h"
#include <vector>
#include <string>

namespace OOFCanvas {
  class CanvasText : public CanvasItem {
  protected:
    const CanvasCoord location;
    const std::string text;
    double angle;
    CanvasColor color;
    std::string fontName;
    bool sizeInPixels;

  public:
    CanvasText(const CanvasCoord&, const std::string &text);
    CanvasText(const CanvasCoord*, const std::string &text);
    static CanvasText *create(const CanvasCoord *p, const std::string &text) {
      return new CanvasText(p, text);
    }
    ~CanvasText();
    virtual const std::string &classname() const;
    void setFillColor(const CanvasColor&);
    void setFont(const std::string&, bool);
    void rotate(double);	// in degrees

    const CanvasCoord& getLocation() const { return location; }
    const std::string& getText() const { return text; }
    const CanvasColor& getColor() const { return color; }
    double getAngleRadians() const { return angle; }
    const std::string& getFontName() const { return fontName; }
    bool getSizeInPixels() const { return sizeInPixels; }

    friend std::ostream &operator<<(std::ostream&, const CanvasText&);
    virtual std::string print() const;
  };

  std::vector<std::string> *list_fonts();

  std::ostream &operator<<(std::ostream&, const CanvasText&);

};


#endif // OOFCANVAS_TEXT_H
