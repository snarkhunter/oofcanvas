// -*- C++ -*-

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modified
 * versions of this software, you first contact the authors at
 * oof_manager@nist.gov. 
 */

#ifndef PYTHONLOCK_H
#define PYTHONLOCK_H

#include <iostream>

// TODO: Put all of this in pyutility.[Ch] ?

// Classes for handling the python global interpreter lock and thread
// state.

// This scheme was copied from code generated by swig 4.0.2 (by
// running swig with the -threads option and examining the C++
// output).  This version is modified slightly so that the
// OOFCanvas_Python_Thread_Allow and OOFCanvas_Python_Thread_Block
// objects do their work in their start() methods, not in their
// constructors.  This allows threading to be turned on and off at run
// time, instead of compile time.  The original swig code is meant to
// be used like this:
//
//    SWIG_Python_Thread_Allow allow_thread;
//    function_call()
//    allow_thread.end()
//
// but we might need to use it like this:
//
//    OOFCanvas_Python_Thread_Allow allow_thread(false); // false means "don't start".
//    if(something)
//       allow_thread.start();
//    function_call();
//    if(something)
//      allow_thread.end()
//
// OOFCanvas_Python_Thread_Allow.start() allows *other* threads to run.  It
// releases the global interpreter lock, and should be called when a
// python thread is going to do something in C++ that takes a long
// time.  The swig %exception typemap uses it to wrap all calls to
// C++.
//
// OOFCanvas_Python_Thread_Block.start() acquires the lock, and should
// be called when C++ needs to make calls to the Python API.
//
// The constructors for OOFCanvas_Python_Thread_Allow and
// OOFCanvas_Python_Thread_Block take an optional boolean argument. If
// it's true, then their constructors call the start() method.  The
// default is true.

namespace OOFCanvas {
  // threading_enabled is set by init_OOFCanvas().
  extern bool threading_enabled;

  class OOFCanvas_Python_Thread_Block {
  private:
    bool status;
    PyGILState_STATE state;
  public:
    void start() {
      if(!status && threading_enabled) {
	state = PyGILState_Ensure();
	status = true;
      }
    }
    void end() {
      if (status && threading_enabled) {
	PyGILState_Release(state);
	status = false;
      }
    }
    OOFCanvas_Python_Thread_Block(bool on=true)
      : status(false)
    {
      if(on)
	start();
    }
    ~OOFCanvas_Python_Thread_Block() {
      end();
    }
  };

  class OOFCanvas_Python_Thread_Allow {
  private:
    PyThreadState *save;
  public:
    OOFCanvas_Python_Thread_Allow(bool on=true)
      : save(nullptr)
    {
      if(on)
	start();
    }
    ~OOFCanvas_Python_Thread_Allow() {
      end();
    }
    void end() {
      // Don't check threading_enabled here!  It might have been
      // turned on by whatever occurred since start() was called.
      if(save) {
	PyEval_RestoreThread(save);
	save = nullptr;
      }
    }
    void start() {
      if(!save && threading_enabled) {
	save = PyEval_SaveThread();
      }
    }
  };

  // These macros are useful only if the begin and end calls are in the
  // same scope.
#define PYTHON_THREAD_BEGIN_BLOCK   OOFCanvas_Python_Thread_Block _thread_block(true)
#define PYTHON_THREAD_END_BLOCK     _thread_block.end()
#define PYTHON_THREAD_BEGIN_ALLOW   OOFCanvas_Python_Thread_Allow _thread_allow(true)
#define PYTHON_THREAD_END_ALLOW     _thread_allow.end()

}; // namespace OOFCanvas

#endif // PYTHONLOCK_H
