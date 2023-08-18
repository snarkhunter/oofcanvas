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

  // When C++ CanvasException objects are caught by the %exception
  // typemap they're converted to python PyCanvasException objects and
  // raised in python.  They are also caught by the gtk main iteration
  // callback, GUICanvasImpl::drawCB, which doesn't return to python.
  // It just prints a message and carries on, without re-throwing the
  // exception.

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
