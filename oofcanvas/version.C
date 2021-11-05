// -*- C++ -*-

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modified
 * versions of this software, you first contact the authors at
 * oof_manager@nist.gov. 
 */

#include "oofcanvas/version.h"

namespace OOFCanvas {

  const std::string *version() {
    // The make_dist script finds a line *beginning* with
    // 'version_from_make_dist' and replaces everything after the '='
    // with the version number.  So don't change the odd formatting of
    // the next three lines!
    static std::string
version_from_make_dist = "1.0.0"
      ;
    return &version_from_make_dist;
  }

};				// namespace OOFCanvas
