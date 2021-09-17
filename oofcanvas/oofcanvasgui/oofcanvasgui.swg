// -*- C++ -*-

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modified
 * versions of this software, you first contact the authors at
 * oof_manager@nist.gov. 
 */

#ifndef OOFGUICANVAS_SWG
#define OOFGUICANVAS_SWG

%module oofcanvasgui
%extern "oofcanvas.swg"

%pragma(python) include="oofcanvasgui.spy"
%pragma(python) code="from oofcanvas.oofcanvas import OffScreenCanvasPtr"
%pragma(python) code="from oofcanvas.oofcanvas import RectanglePtr"
  
%{
   // TODO GTK3: Get rid of SWIG_FILE_WITH_INIT here and elsewhere? Is
   // it just a swig 1.3 thing?
   
#define SWIG_FILE_WITH_INIT
#include "oofcanvasgui/guicanvas.h"
using namespace OOFCanvas;
typedef std::vector<CanvasItem*> CanvasItemList;
typedef std::vector<OOFCanvas::Coord> CanvasCoordVec;
typedef OOFCanvas::Coord CanvasCoord;
%}

%typemap(python, in) PyObject* {
  $target = $source;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

class RubberBand {
public:
  ~RubberBand();
  void setLineWidth(double);
  void setColor(Color);
  void setDashed(bool);
  void setDashLength(double);
  void setDashColor(Color);
  void draw(double x, double y);
};

class LineRubberBand : public RubberBand {
public:
  LineRubberBand();
};

class RectangleRubberBand : public RubberBand {
public:
  RectangleRubberBand();
};

class CircleRubberBand : public RubberBand {
public:
  CircleRubberBand();
};

class EllipseRubberBand : public RubberBand {
public:
  EllipseRubberBand();
};

class SpiderRubberBand : public RubberBand {
public:
  SpiderRubberBand();
  void addPoints(CanvasCoordVec *Coords);
};

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

// MOTION_ALWAYS, et al are enum values in C++ but swig doesn't deal
// well with them.  The following hijinks makes swig create Python
// constants with the same names (sort of) and values as the C++
// enums.  We can't swig the enum names directly.

class MotionAllowed { // swig doesn't need to know it's really an enum.
public:
  %addmethods {
    int value() { return static_cast<int>(*self); }
  }
};
%{
static const MotionAllowed motionAlways = MotionAllowed::ALWAYS;
static const MotionAllowed motionMouseDown = MotionAllowed::MOUSEDOWN;
static const MotionAllowed motionNever = MotionAllowed::NEVER;
%}

%readonly
MotionAllowed motionAlways, motionMouseDown, motionNever;
%readwrite

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

%name(Canvas) class PythonCanvas : public OSCanvasImpl {
public:
  %name(Canvas) PythonCanvas(PyObject*, double ppu);
  %name(Canvas) ~PythonCanvas();
  void destroy();
  int widgetWidth();
  int widgetHeight();
  void show();

  void zoomAbout(CanvasCoord*, double);
  void zoomToFill();
  void center();
  Rectangle visibleRegion();
  void setMouseCallback(PyObject*, PyObject*);
  void setResizeCallback(PyObject*, PyObject*);

  MotionAllowed allowMotionEvents(MotionAllowed);
  
  void setRubberBand(RubberBand*);
  void removeRubberBand();
};

//void initializePyGTK();

#endif // OOFGUICANVAS_SWG
