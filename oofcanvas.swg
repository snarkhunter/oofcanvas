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
  #include "canvaslayer.h"
  #include "canvasitem.h"
  #include "canvasrectangle.h"
  #include "canvassegments.h"
  using namespace OOFCanvas;
%}

%typemap(out) PyObject* {
  $target = $source;
}

class CanvasItem {
private:
  CanvasItem();			// private constructor is not swigged
  ~CanvasItem();
};

class CanvasRectangle : public CanvasItem {
public:
  CanvasRectangle(double, double, double, double);
  void setLineWidth(double);
  void setLineColor(double, double, double, double);
  void setFillColor(double, double, double, double);
};

class CanvasSegments : public CanvasItem {
public:
  CanvasSegments();
  void addSegment(double, double, double, double);
  void setLineWidth(double);
  void setLineColor(double, double, double, double);
};

class CanvasLayer {
private:
  CanvasLayer();		// private constructor is not swigged
  ~CanvasLayer();
public:
  void addItem(CanvasItem*);
  
  // void raise(int);
  // void lower(int);
  // void raiseToTop();
  // void lowerToBottom();
};

class Canvas {
public:
  Canvas(int iw, int ih);
  ~Canvas();
  PyObject *widget();
  CanvasLayer *newLayer();
  void draw();
  void setPixelsPerUnit(double);
  void setBackgroundColor(double, double, double);
};

void initializePyGTK();


%typemap(in) PyObject*;

#endif // OOFCANVAS_SWG
