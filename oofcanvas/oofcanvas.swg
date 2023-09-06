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

%module(package="oofcanvas") oofcanvas

%{
#define SWIG_FILE_WITH_INIT
#include <string>
#include "oofcanvas/canvasimpl.h"
#include "oofcanvas/canvascircle.h"
#include "oofcanvas/canvasimage.h"
#include "oofcanvas/canvaspolygon.h"
#include "oofcanvas/canvasrectangle.h"
#include "oofcanvas/canvassegment.h"
#include "oofcanvas/canvassegments.h"
#include "oofcanvas/canvasshape.h"
#include "oofcanvas/canvastext.h"
#include "oofcanvas/utility.h"
#include "oofcanvas/version.h"
#include "oofcanvas/pyutility.h"

using namespace OOFCanvas;

typedef std::vector<CanvasItem*> CanvasItemList;
typedef std::vector<OOFCanvas::Coord> CanvasCoordVec;
typedef std::vector<double> CanvasDoubleVec;
typedef OOFCanvas::Coord CanvasCoord;
typedef OOFCanvas::ICoord CanvasICoord;
%}

%include "oofcanvas/typemaps.swg"

//==||==\\==||==//==||==\\==||==//==||==\\==||==//==||==\\==||==//

// Version information is defined in C++ with preprocessor macros that
// are set in version.h.  version.h is created from version.h.in by
// cmake.  The ultimate source of the version numbers is the cmake
// "project" line in the top level CMakeLists.txt file.

%immutable;
%rename(version) OOFCANVAS_VERSION;
char* OOFCANVAS_VERSION;
%rename(version_major) OOFCANVAS_VERSION_MAJOR;
int OOFCANVAS_VERSION_MAJOR;
%rename(version_minor) OOFCANVAS_VERSION_MINOR;
int OOFCANVAS_VERSION_MINOR;
%rename(version_patch) OOFCANVAS_VERSION_PATCH;
int OOFCANVAS_VERSION_PATCH;
%mutable;

//==||==\\==||==//==||==\\==||==//==||==\\==||==//==||==\\==||==//

// MAKE_LISTVEC_TYPEMAPS generates the typemaps to convert between a
// list of swigged objects and a std::vector of pointers to the
// objects.  See typemaps.swg.
MAKE_LISTVEC_TYPEMAPS(CanvasItem);
MAKE_LISTVEC_TYPEMAPS(CanvasLayer);

//==||==\\==||==//==||==\\==||==//==||==\\==||==//==||==\\==||==//

class Color {
public:
  Color(double, double, double);
  Color opacity(double);
  double red;
  double green;
  double blue;
};

%extend Color {
  %newobject __repr__;
  const std::string *__repr__() {
    return new std::string(to_string(*self));
  }
};

%immutable;
Color black, white, red, green, blue, magenta, cyan, yellow, gray;
%mutable;

class Rectangle {
public:
  Rectangle(CanvasCoord*, CanvasCoord*);
  double xmin();
  double xmax();
  double ymin();
  double ymax();
};

%extend Rectangle {
  %newobject __repr__;
  const std::string *__repr__() {
    return new std::string("[(" +
			   to_string(self->xmin()) + "," +
			   to_string(self->ymin()) + ", (" +
			   to_string(self->xmax()) + "," +
			   to_string(self->ymax()) + ")]");
  }
};

//==\\==||==//==//==\\==||==//==//==\\==||==//==//==\\==||==//==//

// LineJoin and LineCap are enums defined in canvasshape.h that match
// the Cairo LineJoin and LineCap enums.  It doesn't hurt for swig to
// pretend that they're classes.  The value() method helps with
// debugging.

// We could just swig the enums as they're defined in canvasshape.h:
//   enum class LineCap {BUTT, ROUND, SQUARE};
//   enum class LineJoin {MITER, ROUND, BEVEL};

// but the two ROUNDs creates a conflict.  So we'll stick to this hack
// that worked before swig knew about enums.

class LineJoin {
public:
};

%extend LineJoin {
  int value() { return static_cast<int>(*self); }
}

class LineCap {
public:
};

%extend LineCap {
  int value() { return static_cast<int>(*self); }
}

%{
static const LineJoin lineJoinMiter = LineJoin::MITER;
static const LineJoin lineJoinRound = LineJoin::ROUND;
static const LineJoin lineJoinBevel = LineJoin::BEVEL;
static const LineCap lineCapButt = LineCap::BUTT;
static const LineCap lineCapRound = LineCap::ROUND;
static const LineCap lineCapSquare = LineCap::SQUARE;
%}

%immutable;
LineJoin lineJoinMiter, lineJoinRound, lineJoinBevel;
LineCap lineCapButt, lineCapRound, lineCapSquare;
%mutable;

//==\\==||==//==//==\\==||==//==//==\\==||==//==//==\\==||==//==//

%nodefaultctor CanvasItem;
%nodefaultdtor CanvasItem;

class CanvasItem {
public:
  void drawBoundingBox(double, Color);
  %newobject repr;
  const std::string *repr() const;
};

%nodefaultctor CanvasShape;
%nodefaultdtor CanvasShape;

class CanvasShape : public CanvasItem {
public:
  void setLineWidth(double);
  void setLineWidthInPixels(double);
  void setLineColor(Color);
  void setLineJoin(LineJoin);
  void setLineCap(LineCap);
  void setDash(CanvasDoubleVec*, int); 
  void setDashInPixels(CanvasDoubleVec*, int);
  void setDashColor(Color);
  void unsetDashes();
};

%nodefaultctor CanvasFillableShape;
%nodefaultdtor CanvasFillableShape;

class CanvasFillableShape : public CanvasShape {
public:
  void setFillColor(Color);
};

%typemap(out) CanvasItem* {
  $result = $1->pythonObject();
}

ADD_REPR(CanvasRectangle, repr);
%nodefaultctor CanvasRectangle;
%nodefaultdtor CanvasRectangle;

class CanvasRectangle : public CanvasFillableShape {
public:
  // CanvasRectangle(const CanvasCoord*, const CanvasCoord*);
  static CanvasRectangle *create(const CanvasCoord*, const CanvasCoord*);
};

ADD_REPR(CanvasSegment, repr);
%nodefaultctor CanvasSegment;
%nodefaultdtor CanvasSegment;

class CanvasSegment : public CanvasShape {
public:
  // CanvasSegment(CanvasCoord*, CanvasCoord*);
  static CanvasSegment *create(const CanvasCoord*, const CanvasCoord*);
};

ADD_REPR(CanvasSegments, repr);
%nodefaultctor CanvasSegments;
%nodefaultdtor CanvasSegments;

class CanvasSegments : public CanvasShape {
public:
  // CanvasSegments();
  static CanvasSegments *create();
  void addSegment(CanvasCoord*, CanvasCoord*);
};

ADD_REPR(CanvasCurve, repr);
%nodefaultctor CanvasCurve;
%nodefaultdtor CanvasCurve;

class CanvasCurve : public CanvasShape {
public:
  // CanvasCurve();
  static CanvasCurve *create();
  void addPoint(CanvasCoord*);
  void addPoints(CanvasCoordVec*);
};

ADD_REPR(CanvasPolygon, repr);
%nodefaultctor CanvasPolygon;
%nodefaultdtor CanvasPolygon;

class CanvasPolygon : public CanvasFillableShape {
public:
  // CanvasPolygon();
  static CanvasPolygon *create();
  void addPoint(CanvasCoord*);
  void addPoints(CanvasCoordVec*);
};

ADD_REPR(CanvasCircle, repr);
%nodefaultctor CanvasCircle;
%nodefaultdtor CanvasCircle;

class CanvasCircle : public CanvasFillableShape {
public:
  // CanvasCircle(const CanvasCoord*, double);
  static CanvasCircle *create(const CanvasCoord*, double);
};

ADD_REPR(CanvasEllipse, repr);
%nodefaultctor CanvasEllipse;
%nodefaultdtor CanvasEllipse;

class CanvasEllipse : public CanvasFillableShape {
public:
  // CanvasEllipse(const CanvasCoord*, const CanvasCoord*, double);
  static CanvasEllipse *create(const CanvasCoord*, const CanvasCoord*, double);
};

ADD_REPR(CanvasDot, repr);
%nodefaultctor CanvasDot;
%nodefaultdtor CanvasDot;

class CanvasDot : public CanvasFillableShape {
public:
  // CanvasDot(const CanvasCoord*, double);
  static CanvasDot *create(const CanvasCoord*, double);
  const std::string *repr() const;
};

ADD_REPR(CanvasArrowhead, repr);
%nodefaultctor CanvasArrowhead;
%nodefaultdtor CanvasArrowhead;

class CanvasArrowhead : public CanvasItem {
public:
  // CanvasArrowhead(const CanvasSegment*, double, bool);
  static CanvasArrowhead *create(const CanvasSegment*, double, bool);
  void setSize(double, double);
  void setSizeInPixels(double, double);
};

ADD_REPR(CanvasText, repr);
%nodefaultctor CanvasText;
%nodefaultdtor CanvasText;

class CanvasText : public CanvasItem {
public:
  // CanvasText(CanvasCoord*, char*);
  static CanvasText *create(const CanvasCoord*, char*);
  void setFillColor(Color);
  void setFont(char*, bool);
  void rotate(double);
  bool getSizeInPixels();
  const std::string &getText();
};

ADD_REPR(CanvasImage, repr);
%nodefaultctor CanvasImage;
%nodefaultdtor CanvasImage;

class CanvasImage : public CanvasItem {
public:
  void setOpacity(double);
  void setSize(CanvasCoord*);
  void setSizeInPixels(CanvasCoord*);
  void setDrawIndividualPixels(bool);

  static CanvasImage *newBlankImage(CanvasCoord*, CanvasICoord*, Color);
  static CanvasImage *newFromPNGFile(CanvasCoord*, char*);
#ifdef OOFCANVAS_USE_IMAGEMAGICK
  static CanvasImage *newFromImageMagickFile(CanvasCoord*, char*);
#endif // OOFCANVAS_USE_IMAGEMAGICK
#ifdef OOFCANVAS_USE_NUMPY
  static CanvasImage *newFromNumpy(const CanvasCoord*, PyObject*, bool);
#endif // OOFCANVAS_USE_NUMPY
};

// This is remarkably ugly, but it converts a c++ preprocessor macro
// which is either defined or not into a python-callable function
// which returns either true or false.
 #ifdef OOFCANVAS_USE_IMAGEMAGICK
%{
  bool use_imagemagick() { return true; }
%}
#else
%{
  bool use_imagemagick() { return false; }
%}
#endif	// OOFCANVAS_USE_IMAGEMAGICK

bool use_imagemagick();	

%nodefaultctor CanvasLayer;
%nodefaultdtor CanvasLayer;

class CanvasLayer {
public:
  bool empty();
  void destroy();
  void addItem(CanvasItem*);
  void removeAllItems();
  void rebuild();
  void clear();
  void markDirty();
  void render();
  void setClickable(bool);
  void setOpacity(double);
  void show();
  void hide();
  void raiseBy(int);
  void lowerBy(int);
  void raiseToTop();
  void lowerToBottom();
  void writeToPNG(char*);
};

%extend CanvasLayer {
  const char* name() {
    return self->name.c_str();
  }
}

// The C++ OffScreenCanvas is a wrapper that hides the implementation
// details of OSCanvasImpl from the user.  Python wrapping does the
// same thing, so the Python OffScreenCanvas is based on OSCanvasImpl
// directly, instead of on the C++ OffScreenCanvas.

%rename(OffScreenCanvas) OSCanvasImpl;

class OSCanvasImpl {
public:
  OSCanvasImpl(double);
  ~OSCanvasImpl();
  CanvasLayer *newLayer(const char*);
  void deleteLayer(CanvasLayer*);
  CanvasLayer *getLayer(int);
  // swig 4 is supposed to be able to handle overloaded functions, but
  // it's not working.  So getLayer(string) is renamed getLayerByName.
  %rename(getLayerByName) getLayer(const std::string&);
  CanvasLayer *getLayer(const std::string&);
  int nLayers();
  bool empty();
  void raiseLayer(int, int);
  void lowerLayer(int, int);
  void lowerLayerToBottom(int);
  void raiseLayerToTop(int);
  void reorderLayers(CanvasLayerVec*);
  void clear();
  void draw();
  double getPixelsPerUnit();
  void setAntialias(bool);
  void setMargin(double);
  void setBackgroundColor(Color);
  %rename(clickedItems) clickedItems_new;
  %newobject clickedItems_new;
  CanvasItemExpVec* clickedItems_new(CanvasCoord*);
  %rename(allItems) allItems_new;
  %newobject allItems_new;
  CanvasItemExpVec* allItems_new();
  bool saveAsPDF(const std::string&, int, bool);
  bool saveAsPNG(const std::string&, int, bool);
  bool saveRegionAsPDF(const std::string&, int, bool,
		       CanvasCoord*, CanvasCoord*);
  bool saveRegionAsPNG(const std::string&, int, bool,
		       CanvasCoord*, CanvasCoord*);

  %newobject pixel2user;
  CanvasCoord *pixel2user(int, int);

  void datadump(const std::string&);
};

//==||==\\==||==//==||==\\==||==//==||==\\==||==//==||==\\==||==//

%newobject list_fonts;
std::vector<std::string> *list_fonts();

void set_mainthread();

#ifdef OOFCANVAS_USE_NUMPY
%pythoncode "oofcanvas/npconvert.py"
#endif

void init_PyExceptionConverter(PyObject*);

%inline %{
  void oofcanvas_enable_threads() {
    OOFCanvas::threading_enabled = true;
  }
%}

%pythoncode %{
  class PyCanvasException(Exception):
    def __init__(self, msg):
      self.msg = msg
  def pyExceptionConverter(message):
    raise PyCanvasException(message)

  # init_OOFCanvas should be called before any other oofcanvas code
  # is called from python.  It's ok to call it multiple times.
    
  def init_OOFCanvas(threaded=False):
    if threaded:
       oofcanvas_enable_threads();
    init_PyExceptionConverter(pyExceptionConverter)
%}

#endif // OOFCANVAS_SWG
