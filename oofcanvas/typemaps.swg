// -*- C++ -*-

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modified
 * versions of this software, you first contact the authors at
 * oof_manager@nist.gov. 
 */

#ifndef OOFCANVAS_TYPEMAPS_SWG
#define OOFCANVAS_TYPEMAPS_SWG

%{
  #include <iostream>
%}

//==||==\\==||==//==||==\\==||==//==||==\\==||==//==||==\\==||==//

// FETCH_CLASS imports the class named CLASS from the module named
// MODULE.  It creates a static variable by appending "Class" to the
// given CLASS and stores a pointer to the the PyObject in it.  Don't
// use quotation marks around the arguments when calling FETCH_CLASS.

%define FETCH_CLASS(MODULE, CLASS)
  static PyObject *CLASS##Class = 0;
  if(!CLASS##Class) {
    PyObject *module = PyImport_ImportModule("MODULE");
    CLASS##Class = PyObject_GetAttrString(module, "CLASS");
    Py_XDECREF(module);
    if(!CLASS##Class)
      SWIG_fail;
  }
%enddef // FETCH_CLASS

//==||==\\==||==//==||==\\==||==//==||==\\==||==//==||==\\==||==//

// ADD_REPR(class, func) adds a __repr__ to the swigged class.  The
// new __repr__ will call the given func, which must return a new
// std::string*.  The func must be swigged if using -builtin, and
// declared with %newobject.  ADD_REPR must appear *before* the class
// declaration in the swig file.

// If not using -builtin, ADD_REPR can appear after the declaration
// and the function does not need to be swigged. TODO PYTHON3: Check
// that.

class std::string;

#ifdef SWIGPYTHON_BUILTIN
%define ADD_REPR(CLASS, FUNC)
  %feature("python:slot", "tp_repr", functype="reprfunc") CLASS::FUNC
%enddef
%define ADD_STR(CLASS, FUNC)
  %feature("python:slot", "tp_str", functype="reprfunc") CLASS::FUNC
%enddef
  
#else  // not SWIGPYTHON_BUILTIN
%define ADD_REPR(CLASS, FUNC)  
%extend CLASS {
  %newobject __repr__;
    const std::string *__repr__() { // ADD_REPR
      return self->FUNC();
    }
  };
%enddef
%define ADD_STR(CLASS, FUNC)  
%extend CLASS {
  %newobject __str__;
    const std::string *__str__() { // ADD_STR
      return self->FUNC();
    }
 };
%enddef
#endif	// SWIGPYTHON_BUILTIN


%typemap(newfree) const std::string* {
  // typemap(newfree) const std::string*
  delete $1;
 }

//==||==\\==||==//==||==\\==||==//==||==\\==||==//==||==\\==||==//

// std::vector<TYPE*>  <--> [TYPE]

// MAKE_LISTVEC_TYPEMAPS defines unnamed in and out typemaps for
// converting between a C++ std::vector* of TYPE*s and a list of
// swigged TYPEs.  It does not copy the objects -- the swigged
// pointers point to the original C++ objects.

// Three typemaps are defined:
// out: TYPEVec *     (transfers ownership of TYPE objects to python)
// out: TYPERefVec *  (does not transfer ownership to python)
// in:  TYPEVec*

// If you use std::vector<TYPE*>* as the return type of a function ,
// it will be interpreted as TYPEVec, and Python will take ownership
// of the objects.  You need to explicitly use TYPERefVec* (eg,
// CoordRefVec if TYPE is Coord) as a return type if you don't want
// ownership of the objects to be transferred to Python.


%define MAKE_LISTVEC_TYPEMAPS(TYPE)

%{
  // These typedefs are used to invoke different "out" typemaps on
  // returned vectors.
  typedef std::vector<TYPE*> TYPE##Vec;
  typedef std::vector<TYPE*> TYPE##RefVec;
  typedef std::vector<TYPE*> TYPE##ExpVec;

%}



%typemap(out) TYPE##Vec* {
  // typemap(out) TYPE##Vec* via MAKE_LISTVEC_TYPEMAPS for TYPE
  TYPE##Vec::size_type sz = $1->size();
  // assert((int)sz >= PY_SSIZE_T_MIN && (int)sz <=PY_SSIZE_T_MAX);
  $result = PyList_New((Py_ssize_t) sz);
  for(TYPE##Vec::size_type i=0; i<sz; i++) {
    PyObject *tmp = SWIG_NewPointerObj(SWIG_as_voidptr((*$1)[i]),
				       $descriptor(TYPE*),
				       SWIG_POINTER_OWN|0);
    PyList_SET_ITEM($result, (Py_ssize_t) i, tmp);
  }
}

// ...ExpVec uses the PythonExportable mechanism on the items in the list.
%typemap(out) TYPE##ExpVec* {
  // typemap(out) TYPE##ExpVec* via MAKE_LISTVEC_TYPEMAPS for TYPE
  TYPE##Vec::size_type sz = $1->size();
  $result = PyList_New((Py_ssize_t) sz);
  for(TYPE##Vec::size_type i=0; i<sz; i++) {
    PyObject *tmp = (*$1)[i]->pythonObject();
    PyList_SET_ITEM($result, (Py_ssize_t) i, tmp);
  }
 }

%typemap(out) TYPE##RefVec* {
  // typemap(out) TYPE##RefVec* via MAKE_LISTVEC_TYPEMAPS for TYPE
  TYPE##RefVec::size_type sz = $1->size();
  $result = PyList_New((Py_ssize_t) sz);
  for(TYPE##RefVec::size_type i=0; i<sz; i++) {
    PyObject *tmp = SWIG_NewPointerObj(SWIG_as_voidptr((*$1)[i]),
				       $descriptor(TYPE*),
				       0); // don't take ownership
    PyList_SET_ITEM($result, (Py_ssize_t) i, tmp);
  }
 }

%typemap(in) TYPE##Vec* (TYPE##Vec v) {
  // typemap(in) TYPE##Vec* via MAKE_LISTVEC_TYPEMAPS for TYPE
  if(!PyList_Check($input)) {
    SWIG_exception_fail(SWIG_TypeError,
			"in '$symname', expected a list of TYPE.");
  }
  TYPE##Vec::size_type sz = PyList_Size($input);
  v.resize(sz);
  for(TYPE##Vec::size_type i=0; i<sz; i++) {
    void *itemptr = 0;
    int res = SWIG_ConvertPtr(PyList_GET_ITEM($input, i), &itemptr,
			      $descriptor(TYPE*), 0);
    if(!SWIG_IsOK(res)) {
      SWIG_exception_fail(SWIG_TypeError,
			  "in method '$symname', expecting list of TYPEs");
    }
    v[i] = reinterpret_cast<TYPE*>(itemptr);
  }
    $1 = &v;
 }

%enddef // MAKE_LISTVEC_TYPEMAPS

%typemap(out) CanvasItem* {
  // typemap(out) CanvasItem*				   
  $result = $1->pythonObject();
}

%typemap(out) std::string* {
  // typemap(out) std::string*
  $result = PyUnicode_FromString($1->c_str()); // std::string* typemap
 }

%typemap(out) std::string& {
  // typemap(out) std::string&
  // Although the typemap arg is a reference, swig converts it to a pointer.
  $result = PyUnicode_FromString($1->c_str());
}

%typemap(out, warning="900: Don't do this!") std::string {
  // This still makes an extra copy of the string.  Putting
  // 'optimal="1"' after 'out' generates code that doesn't create an
  // extra copy of the string, but it can't be used with the
  // %exception typemap.
  const std::string &str = $1;
  $result = PyUnicode_FromString(str.c_str()); // std::string typemap
}

// %typemap(in) std::string& (std::string str) {
//   // typemap(in) std::string&
//   std::cerr << "input=" << input << std::endl;
//   PyObject *ustr = PyUnicode_AsEncodedString($input, "UTF-8", "replace");
//   std::cerr << "ustr=" << ustr << std::endl;
//   str.assign(PyBytes_AsString(ustr));
//   $1 = &str;
//   Py_XDECREF(ustr);
// }

// %typemap(in) std::string* (std::string str) {
//   // typemap(in) std::string*
//   PyObject *ustr = PyUnicode_AsEncodedString($input, "UTF-8", "replace");
//   str.assign(PyBytes_AsString(ustr));
//   $1 = &str;
//   Py_XDECREF(ustr);
// }

// %typemap(in) std::string {
//   // typemap(in) std::string
//   PyObject *ustr = PyUnicode_AsEncodedString($input, "UTF-8", "replace");
//   $1 = PyBytes_AsString(ustr);
//   Py_DECREF(ustr);
//  }

%typemap(out) std::vector<std::string>* {
  // typemap(out) std::vector<std::string>*
  std::vector<std::string>::size_type sz = $1->size();
  $result = PyList_New((Py_ssize_t) sz);
  for(std::vector<std::string>::size_type i=0; i<sz; i++) {
    PyList_SET_ITEM($result, (Py_ssize_t) i,
		    PyUnicode_FromString((*$1)[i].c_str()));
  }
 }

%typemap(in) CanvasCoord* (CanvasCoord pt) {
  // typemap(in) CanvasCoord*
  static PyObject *zero = nullptr;
  static PyObject *one = nullptr;
  if(!zero) {
    zero = PyInt_FromLong(0);
    one = PyInt_FromLong(1);
    Py_INCREF(zero);
    Py_INCREF(one);
  }
  PyObject *x = PyObject_GetItem($input, zero);
  PyObject *y = PyObject_GetItem($input, one);
  if(x == nullptr || y == nullptr) {
    SWIG_exception_fail(SWIG_TypeError,
			"in '$symname', expected 2-tuple");
  }
  pt.x = PyFloat_AsDouble(x);
  pt.y = PyFloat_AsDouble(y);
  Py_XDECREF(x);
  Py_XDECREF(y);
  $1 = &pt;
}

%typemap(out) CanvasCoord* {
  // typemap(out) CanvasCoord*
  // Convert an OOFCanvas::Coord to a tuple
  $result = Py_BuildValue("dd", $1->x, $1->y);
}

%typemap(in) CanvasICoord* (CanvasICoord pt) {
  // typemap(in) CanvasICoord*
  static PyObject *zero = nullptr;
  static PyObject *one = nullptr;
  if(!zero) {
    zero = PyInt_FromLong(0);
    one = PyInt_FromLong(1);
    Py_INCREF(zero);
    Py_INCREF(one);
  }
  PyObject *x = PyObject_GetItem($input, zero);
  PyObject *y = PyObject_GetItem($input, one);
  if(x == nullptr || y == nullptr) {
    SWIG_exception_fail(SWIG_TypeError,
			"in '$symname', expected a 2-tuple");
  }
  pt.x = PyInt_AsLong(x);
  pt.y = PyInt_AsLong(y);
  Py_XDECREF(x);
  Py_XDECREF(y);
  $1 = &pt;
}

%typemap(out) CanvasICoord* {
  // typemap(out) CanvasICoord*
  $result = Py_BuildValue("dd", $1->x, $1->y);
}

// %typemap(python, newfree) CanvasICoord* {
//   delete $source;
// }

// Create a vector of Coords from a Python sequence whose contents
// have a __getitem__ that returns x (0) and y (1) components.

%typemap(in) CanvasCoordVec* (CanvasCoordVec vec) {
  // typemap(in) CanvasCoordVec*
  if(!PySequence_Check($input)) {
    SWIG_exception_fail(SWIG_TypeError,
			"in '$synname', expected Python sequence");
  }
  static PyObject *zero = nullptr;
  static PyObject *one = nullptr;
  if(!zero) {
    zero = PyInt_FromLong(0);
    one = PyInt_FromLong(1);
    Py_INCREF(zero);
    Py_INCREF(one);
  }
  CanvasCoordVec::size_type sz = PySequence_Size($input);
  vec.resize(sz);
  for(CanvasCoordVec::size_type i=0; i<sz; i++) {
    PyObject *item = PySequence_GetItem($input, (Py_ssize_t) i);
    PyObject *x = PyObject_GetItem(item, zero);
    PyObject *y = PyObject_GetItem(item, one);
    if(x == nullptr || y == nullptr) {
      return nullptr;
    }
    vec[i].x = PyFloat_AsDouble(x);
    vec[i].y = PyFloat_AsDouble(y);
    Py_XDECREF(x);
    Py_XDECREF(y);
  }
  $1 = &vec;
}

// Convert a Python sequence of numbers to a C++ vector of floats.

%typemap(in) CanvasDoubleVec* (CanvasDoubleVec vec) {
  // typemap(in) DoubleList*
  if(!PySequence_Check($input)) {
    SWIG_exception_fail(SWIG_TypeError,
			"in '$symname', expected a sequence of numbers.");
  }
  CanvasDoubleVec::size_type sz = PySequence_Size($input);
  if(sz > 0) {
    vec.resize(sz);
    for(CanvasDoubleVec::size_type i=0; i<sz; i++) {
      PyObject *x = PySequence_GetItem($input, (Py_ssize_t) i);
      if(PyFloat_Check(x))
	vec[i] = PyFloat_AsDouble(x);
      else if(PyInt_Check(x))
	vec[i] = PyLong_AsLong(x);
      else {
	SWIG_exception_fail(SWIG_TypeError,
			    "in '$symname', expected a sequence of numbers.");
      }
      Py_XDECREF(x);
    }
  }
  $1 = &vec;
}

#endif // OOFCANVAS_TYPEMAPS_SWG
