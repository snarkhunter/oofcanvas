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
    const Coord location;
    const std::string text;
    double angle;
    Color color;
    std::string fontName;
    bool sizeInPixels;

  public:
    CanvasText(const Coord&, const std::string &text);
    CanvasText(const Coord*, const std::string &text);
    ~CanvasText();
    virtual const std::string &classname() const;
    void setFillColor(const Color&);
    void setFont(const std::string&, bool);
    void rotate(double);	// in degrees

    const Coord& getLocation() const { return location; }
    const std::string& getText() const { return text; }
    const Color& getColor() const { return color; }
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
