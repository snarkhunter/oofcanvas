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
    double angle;
    bool sizeInPixels; // is font size given in pixel units or user units?
    Color color;
    virtual void drawItem(Cairo::RefPtr<Cairo::Context>);
    virtual bool containsPoint(const Canvas*, const Coord&) const;
  public:
    CanvasText(double, double, const std::string &text, double);
    void setFillColor(const Color&);
    void rotate(double);
    void setSizeInPixels(bool v) { sizeInPixels = v; }
    virtual const std::string &classname() const;
    friend std::ostream &operator<<(std::ostream&, const CanvasText&);
    virtual std::string *print() const;
  };

  std::ostream &operator<<(std::ostream&, const CanvasText&);
};


#endif // OOF_CANVASTEXT_H
