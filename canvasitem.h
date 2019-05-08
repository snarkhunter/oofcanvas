// -*- C++ -*-

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modified
 * versions of this software, you first contact the authors at
 * oof_manager@nist.gov. 
 */

#ifndef OOFCANVASITEM_H
#define OOFCANVASITEM_H

namespace OOFCanvas {

  class OOFCanvas;

  class CanvasItem {
  public:
    virtual ~CanvasItem() {}
    virtual Rectangle userBoundingBox() const = 0;
    virtual IRectangle pixelBoundingBox() const = 0;
  };

};

#endif // OOFCANVASITEM_H

