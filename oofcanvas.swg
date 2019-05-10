// -*- C++ -*-

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modified
 * versions of this software, you first contact the authors at
 * oof_manager@nist.gov. 
 */

#ifndef OOFCANVAS_SWG
#define OOFCANVAS_SWG

%module oofcanvas

%{
  #define SWIG_FILE_WITH_INIT
  #include "canvas.h"
  //#include "canvaslayer.h"
  using namespace OOFCanvas;
%}

%typemap(out) PyObject* {
  $result = $1;
}

// class CanvasLayer {
// public:
//   // Constructor is not swigged

//   void raise(int);
//   void lower(int);
//   void raiseToTop();
//   void lowerToBottom();
// };

class Canvas {
public:
  Canvas(int iw, int ih, double w, double h);
  PyObject *widget();
  //  CanvasLayer *newLayer();
  void draw();
};


%typemap(in) PyObject*;

#endif // OOFCANVAS_SWG
