// -*- C++ -*-

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modified
 * versions of this software, you first contact the authors at
 * oof_manager@nist.gov. 
 */

#ifndef OOFCANVASLAYER_H
#define OOFCANVASLAYER_H

#include <cairomm/cairomm.h>
#include "OOFCanvas/canvasitem.h"

namespace OOFCanvas {

  class Canvas;

  class CanvasLayer : public CanvasItem {
  protected:
    Canvas *canvas;
    Cairo::RefPtr<Cairo::Surface> surface;
    std::vector<CanvasItem*> items;
    bool visible;

  private:
    CanvasLayer(Canvas*);
    void raise(int);
    void lower(int);
    void raiseToTop();
    void lowerToBottom();
    
    friend class Canvas;
  };

};

#endif // OOFCANVASLAYER_H

