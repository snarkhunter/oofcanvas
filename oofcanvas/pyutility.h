// -*- C++ -*-

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modified
 * versions of this software, you first contact the authors at
 * oof_manager@nist.gov. 
 */

#ifndef OOFCANVAS_PYUTILITY_H
#define OOFCANVAS_PYUTILITY_H

#include <Python.h>
#include <string>

#ifdef OOFCANVAS_USE_PYTHON

namespace OOFCanvas {

  std::string repr(PyObject*);

};

#endif // OOFCANVAS_USE_PYTHON
#endif // OOFCANVAS_PYUTILITY_H
