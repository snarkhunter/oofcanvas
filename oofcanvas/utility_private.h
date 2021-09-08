// -*- C++ -*-

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modified
 * versions of this software, you first contact the authors at
 * oof_manager@nist.gov. 
 */

#ifndef UTILITY_PRIVATE_H
#define UTILITY_PRIVATE_H

// Stuff that can't be in utility.h because it would prevent that file
// from being included in places that don't want to import Cairo.
// This file is used when building OOFCanvas but not when building
// code that *uses* OOFCanvas.

#include <cairomm/cairomm.h>
#include <oofcanvas/oofcanvas.h>
#include <oofcanvas/utility.h>

namespace OOFCanvas {
  
  void setColor(const Color&, Cairo::RefPtr<Cairo::Context>);

  Coord user_to_device(const Coord&, Cairo::RefPtr<Cairo::Context>);
  Coord device_to_user(const Coord&, Cairo::RefPtr<Cairo::Context>);

  Rectangle user_to_device(const Rectangle&, Cairo::RefPtr<Cairo::Context>);
  Rectangle device_to_user(const Rectangle&, Cairo::RefPtr<Cairo::Context>);

  std::ostream &operator<<(std::ostream&, const Cairo::Matrix&);
  bool operator==(const Cairo::Matrix&, const Cairo::Matrix&);

};				// namespace OOFCanvas

#endif // UTILITY_PRIVATE_H
