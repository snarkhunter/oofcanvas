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

%module(package="oofcanvas") oofcanvasgui
%import "oofcanvas/oofcanvas.swg"

%pythoncode %{
  import gi
  gi.require_version("Gtk", "3.0")
  from gi.repository import Gtk
%}

%{
#define SWIG_FILE_WITH_INIT
#include "oofcanvas/canvas.h"
#include "oofcanvas/canvascircle.h"
#include "oofcanvas/canvasimage.h"
#include "oofcanvas/canvaslayer.h"
#include "oofcanvas/canvaspolygon.h"
#include "oofcanvas/canvasrectangle.h"
#include "oofcanvas/canvassegment.h"
#include "oofcanvas/canvassegments.h"
#include "oofcanvas/canvastext.h"
#include "oofcanvas/oofcanvasgui/guicanvas.h"
#include "oofcanvas/oofcanvasgui/guicanvasimpl.h"
#include "oofcanvas/oofcanvasgui/rubberband.h"
#include "oofcanvas/utility.h"
using namespace OOFCanvas;
typedef std::vector<CanvasItem*> CanvasItemList;
typedef std::vector<OOFCanvas::Coord> CanvasCoordVec;
typedef OOFCanvas::Coord CanvasCoord;
%}

%typemap(in) PyObject* {
  $1 = $input;
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
  void update(CanvasCoord*);
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

enum class MotionAllowed { NEVER, ALWAYS, MOUSEDOWN };

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

// The actual python Canvas class is derived from PythonCanvas in
// oofcanvasgui.__init__.py.  That's an odd place to define it.

class PythonCanvas : public OSCanvasImpl {
public:
  PythonCanvas(PyObject*, double ppu);
  ~PythonCanvas();
  void destroy();
  int widgetWidth();
  int widgetHeight();
  void show();

  void zoomAbout(CanvasCoord*, double);
  void zoomToFill();
  void zoom(double);
  void center();
  Rectangle visibleRegion();
  void setMouseCallback(PyObject*, PyObject*);
  void setResizeCallback(PyObject*, PyObject*);

  MotionAllowed allowMotionEvents(MotionAllowed);
  
  void setRubberBand(RubberBand*);
  void removeRubberBand();

  // %pythoncode %{
  //   def get_hadjustment(self):
  //       return self.layout.get_property("hadjustment")
  //   def get_vadjustment(self):
  //       return self.layout.get_property("vadjustment")
  //   %}
};


//void initializePyGTK();

#endif // OOFGUICANVAS_SWG
