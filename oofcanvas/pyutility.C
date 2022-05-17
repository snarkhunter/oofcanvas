// -*- C++ -*-

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modified
 * versions of this software, you first contact the authors at
 * oof_manager@nist.gov. 
 */

#include "oofcanvas/pyutility.h"

namespace OOFCanvas {

  // Useful function for debugging by printing Python objects from C++.
#if OOFCANVAS_USE_PYTHON == 3
  std::string repr(PyObject *obj) {
    PyObject *repr = PyObject_ASCII(obj);
    assert(obj != 0);
    PyObject *ustr = PyUnicode_AsEncodedString(repr, "UTF-8", "replace");
    assert(ustr != 0);
    std::string r(PyBytes_AsString(ustr));
    Py_XDECREF(repr);
    Py_XDECREF(ustr);
    return r;
  }
#endif
#if OOFCANVAS_USE_PYTHON == 2
  std::string repr(PyObject *obj) {
    return PyString_AsString(PyObject_Repr(obj));
  }
#endif 
};				// namespace OOFCanvas
