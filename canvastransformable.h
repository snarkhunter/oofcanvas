// -*- C++ -*-

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modified
 * versions of this software, you first contact the authors at
 * oof_manager@nist.gov. 
 */

#idndef OOFCANVASTRANSFORMABLE_H
#define OOFCANVASTRANSFORMABLE_H

#include "canvasitem.h"

namespace OOFCanvas {

  class CanvasTransformable : public CanvasItem {
  private:
    TMatrix matrix_;
    Coord offset_;
    bool fixedSize_;
  };
};


#endif // OOFCANVASTRANSFORMABLE_H
