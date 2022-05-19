// -*- C++ -*-

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modified
 * versions of this software, you first contact the authors at
 * oof_manager@nist.gov. 
 */

#ifndef OOFCANVAS_PYTHONEXPORTABLE_H
#define OOFCANVAS_PYTHONEXPORTABLE_H


// The PythonExportable base class is used to convert base class
// pointers exported to Python via SWIG into their derived class
// Python objects.  For example, suppose you have a C++ class
// hierarchy with a base class B and derived classes D1 and D2, all of
// which are swigged.  If you have a function B* f() that returns a
// pointer to an instance of D1 or D2, the SWIG generated Python code
// won't give you access to the derived parts of D1 and D2.  The
// swigged f() will return a Python B.  This is consistent with the
// way C++ does things---using a B* only gives you access to those
// parts of D1 and D2 that are declared in the base class---but it's
// inconsistent with Python's approach, in which there's no such thing
// as a base class pointer---all components of all objects are always
// accessible, if you know they're there. (For example, you might have
// created a SWIGged derived class object in Python, stored in a C++
// list of base class objects, and then retrieved it from that list in
// Python.  You'd expect to have all of the derived class functions
// available.)  The PythonExportable class remedies this
// inconsistency, allowing C++ functions to have polymorphic Python
// return types.

// To use this class, you need to do two things:

// 1) Derive a C++ class heirarchy from the templated base class
//    PythonExportable.  The template parameter must be the name of
//    the class derived from PythonExportable.  Each derived type must
//    supply a classname() virtual function that returns the name of
//    the derived class .  If the C++ derived classes will be swigged
//    and used as base classes for Python classes, then the
//    PythonExportable must be a *virtual* base class (see
//    PythonNative, below).
//
//    For example:
//      class MyExportable : public PythonExportable<MyExportable> {
//      public:
//      }
//      class Oil : public MyExportable {
//      public:
//         virtual const std::string &classname() const {
///           static const std::string name("Oil");
//            return name;
//         }
//      }
//      class Cheese : public MyExportable {
//      public:
//         virtual const std::string &classname() const {
//            static const std::string name("Cheese");
//            return name;
//         }
//      }

// 2) In every swig file containing a function that returns a base
//    class pointer ("MyExportable*" in the example above), include
//    the following typemap:
//    %typemap(python, out) BASECLASS* {
//      $target = $source->pythonObject();
//    }
//    where BASECLASS is the base class of the heirarchy derived
//    from PythonExportable.  

// If the C++ class heirarchy derived from PythonExportable is
// swigged, and the swigged Python classes are extended by Python
// inheritance, then the above mechanism doesn't quite work.  The
// class which is to be used as a base class for the Python
// inheritance must be derived from PythonExportable and *also*
// PythonNative.  PythonNative is a template, and must have the same
// template parameter as PythonExportable. 

// For example, if Cheese is swigged and extended in Python,
// then it needs to have been defined like this in C++:
//    class Cheese
//       : public MyExportable, virtual public PythonNative<MyExportable>
//    { ... }
// with a constructor that passes the Python object pointer to PythonNative:
//    Cheese::Cheese(PyObject *self) : PythonNative(self) { ... }
// and a Python constructor for the derived class that also passes the pointer:
// class Cheddar(Cheese):
//    def __init__(self):
//         [cheddar specific stuff]
//         Cheese.__init__(self, self)
// The second "self" in the line above is the "PyObject *self" in the C++
// Cheese constructor.

// If you use PythonNative, then PythonExportable must be a virtual base class:
//    class MyExportable : virtual public PythonExportable<MyExportable> { ... }
// It is legal to use PythonExportable as a virtual base class even if
// you don't use PythonNative, but there may be a small performance
// penalty.


#include <iostream>

// When this is included in swig-generated code we *don't* want to
// include swigruntime.h, as it will repeat definitions already
// present.  We only want to include it when building our C++
// libraries that define the swigged classes and functions.

#ifndef SWIG_FILE_WITH_INIT
#include <Python.h>
#include "swigruntime.h"
#define SWIG_as_voidptr(a) const_cast< void * >(static_cast< const void * >(a)) 
#endif // SWIG_FILE_WITH_INIT

namespace OOFCanvas {

  class PythonExportableBase {
  public:
    virtual PyObject *pythonObject() const = 0; 
    virtual ~PythonExportableBase() {}
  };

  template <class TYPE>
  class PythonExportable : public PythonExportableBase {
  public:
    virtual ~PythonExportable() {}
    
    // classname() must return the name of the *derived* class. 
    virtual const std::string &classname() const = 0;
    
    virtual PyObject *pythonObject() const {
      // For a class named "TYPE", SWIG creates a Python object by
      // passing the string "_TYPE_p" to SWIG_MakePtr, to create a
      // string representation of the pointer to the object.  Then it
      // passes that string to the Python function TYPEPtr, to create an
      // instance of the Ptr version of the Python shadow class.  Here,
      // we do the same thing, but we use the virtual C++ classname()
      // function to get the name of the derived class, and use that
      // instead.

      PyGILState_STATE pystate = PyGILState_Ensure();
      try {
	// Because C++ classes use PythonExportable as a *virtual* base
	// class, the value of "this" in this function is not necessarily
	// the same as the address of the TYPE object.  Swig needs to know
	// the derived class's address, so here we cast "this" to a
	// pointer to the derived type.
	const TYPE *derived_addr = dynamic_cast<const TYPE*>(this);
	
	if (derived_addr==0) {
	  // The dynamic cast can fail, and when it does, a coherent
	  // exception is hard to throw -- this code is inside a
	  // typemap, and outside the usual SWIG exception-handling
	  // code.  So, since this is expected to be rare, we 
	  // print out a message, and abort with a standard C++ exception.
	  // This will be uncaught, and the program will exit.
	  throw std::runtime_error("Dynamic cast failed in PythonExportable.");
	}

	std::string pname = "_p_" + classname();
	
#ifdef SWIG_USE_BUILTIN
	PyObject *self = 0;	// TODO PYTHON3: Is this correct?
	PyObject *result = SWIG_NewPointerObj(SWIG_as_voidptr(derived_addr),
					      SWIG_TypeQuery(pname.c_str()), 
					      SWIG_BUILTIN_INIT|0);
#else	
	PyObject *result = SWIG_NewPointerObj(SWIG_as_voidptr(derived_addr),
					      SWIG_TypeQuery(pname.c_str()), 0);
#endif
	if(!result) {
	  std::cerr << "pythonexportable: Failed to instantiate python object"
		    << std::endl;
	  PyGILState_Release(pystate);
	  return 0;
	}

	PyGILState_Release(pystate);
	return result;
      }
      catch (...) {
	PyGILState_Release(pystate);
	throw;
      }
    }
  };

  //=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

  // If a C++ class derived from PythonExportable is to be used as a
  // base class for other Python classes (via SWIG), then the C++
  // derived class must also include PythonNative as a virtual base
  // class.

  template <class TYPE>
  class PythonNative : virtual public PythonExportable<TYPE> {
  private:
    PyObject * const self;
  public:
    PythonNative(PyObject *slf)
      : self(slf)
    {
      PyGILState_STATE pystate = PyGILState_Ensure();
      Py_XINCREF(self);
      PyGILState_Release(pystate);
    }
    virtual ~PythonNative() {
      PyGILState_STATE pystate = PyGILState_Ensure();
      Py_XDECREF(self);
      PyGILState_Release(pystate);
    }
    virtual PyObject *pythonObject() const {
      PyGILState_STATE pystate = PyGILState_Ensure();
      Py_XINCREF(self);
      PyGILState_Release(pystate);
      return self;
    }
  };
  
}; 				// namespace OOFCanvas

#endif // OOFCANVAS_PYTHONEXPORTABLE_H
