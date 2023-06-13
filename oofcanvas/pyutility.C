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
#include "oofcanvas/pythonlock.h"

namespace OOFCanvas {

  bool threading_enabled = false;

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
  
  // Python exceptions must be derived from Exception, so simply
  // swigging a C++ exception classs won't work.  pyExConverter is a
  // python function that raises a Python exception.  It's called when
  // the %exception typemap in typemaps.swg detects that a C++ routine
  // called from python has thrown an exception.
  PyObject *pyExConverter = 0;

  void init_PyExceptionConverter(PyObject *converter) {
    OOFCanvas_Python_Thread_Allow allow_threads(true);
    if(pyExConverter == nullptr) {
      Py_XINCREF(converter);
      pyExConverter = converter;
    }
    allow_threads.end();
  }
#endif	// OOFCANVAS_USE_PYTHON == 3
  
#if OOFCANVAS_USE_PYTHON == 2
  std::string repr(PyObject *obj) {
    return PyString_AsString(PyObject_Repr(obj));
  }
  // TODO: write initPyExceptionConverter for Python2
  
#endif	// OOFCANVAS_USE_PYTHON == 2
};				// namespace OOFCanvas
