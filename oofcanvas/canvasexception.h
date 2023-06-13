// -*- C++ -*-

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modified
 * versions of this software, you first contact the authors at
 * oof_manager@nist.gov. 
 */

#ifndef CANVASEXCEPTION_H
#define CANVASEXCEPTION_H

#include <string>
#include <iostream>

namespace OOFCanvas {

  class CanvasException {
  private:
    const std::string msg;
  public:
    CanvasException(const std::string &msg) : msg(msg) {}
    const std::string& message() const { return msg; }
  };

  std::ostream &operator<<(std::ostream&, const CanvasException&);

};				// end namespace OOFCanvas

#endif // CANVASEXCEPTION_H
