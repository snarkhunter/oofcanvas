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
#include "oofcanvas/cversion.h"
using namespace OOFCanvas;

typedef std::vector<CanvasItem*> CanvasItemList;
typedef std::vector<OOFCanvas::Coord> CanvasCoordVec;
typedef std::vector<double> CanvasDoubleVec;
typedef OOFCanvas::Coord CanvasCoord;
typedef OOFCanvas::ICoord CanvasICoord;
%}

%import "oofcanvas/typemaps.swg"

MAKE_LISTVEC_TYPEMAPS(CanvasItem);
// Convert a list of swigged CanvasLayer*s to a std::vector<CanvasLayer*>
MAKE_LISTVEC_TYPEMAPS(CanvasLayer);


//==||==\\==||==//==||==\\==||==//==||==\\==||==//==||==\\==||==//

// %typemap(out) PyObject* {
//   // typemap(python, out) PyObject*				 
//   $result = $1;
// }

// %typemap(in) PyObject* {
//   // typemap(in) PyObject*
//   $1 = $input;
// }


const std::string *version();

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

class Color {
public:
  Color(double, double, double);
  Color opacity(double);
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


class CanvasItem {
private:
  CanvasItem();			// private constructor is not swigged
  // The destructor is not swigged, so destroying a CanvasItem wrapper
  // in Python does not destry the C++ object.  C++ CanvasItems are
  // owned by their CanvasLayer and are destroyed when removed from
  // the layer.
  ~CanvasItem();
public:
  void drawBoundingBox(double, Color);
};

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

class CanvasFillableShape : public CanvasShape {
public:
  void setFillColor(Color);
};

class CanvasRectangle : public CanvasFillableShape {
public:
  CanvasRectangle(CanvasCoord*, CanvasCoord*);
};

ADD_REPR(CanvasRectangle, repr);

class CanvasSegment : public CanvasShape {
public:
  CanvasSegment(CanvasCoord*, CanvasCoord*);
};

ADD_REPR(CanvasSegment, repr);

class CanvasSegments : public CanvasShape {
public:
  CanvasSegments();
  void addSegment(CanvasCoord*, CanvasCoord*);
};

ADD_REPR(CanvasSegments, repr);

class CanvasCurve : public CanvasShape {
public:
  CanvasCurve();
  void addPoint(CanvasCoord*);
  void addPoints(CanvasCoordVec*);
};

ADD_REPR(CanvasCurve, repr);

class CanvasPolygon : public CanvasFillableShape {
public:
  CanvasPolygon();
  void addPoint(CanvasCoord*);
  void addPoints(CanvasCoordVec*);
};

ADD_REPR(CanvasPolygon, repr);

class CanvasCircle : public CanvasFillableShape {
public:
  CanvasCircle(CanvasCoord*, double);
};

ADD_REPR(CanvasCircle, repr);

class CanvasEllipse : public CanvasFillableShape {
public:
  CanvasEllipse(CanvasCoord*, CanvasCoord*, double);
};

ADD_REPR(CanvasEllipse, repr);

class CanvasDot : public CanvasFillableShape {
public:
  CanvasDot(CanvasCoord*, double);
};

ADD_REPR(CanvasDot, repr);

class CanvasArrowhead : public CanvasItem {
public:
  CanvasArrowhead(CanvasSegment*, double, bool);
  void setSize(double, double);
  void setSizeInPixels(double, double);
};

ADD_REPR(CanvasArrowhead, repr);

class CanvasText : public CanvasItem {
public:
  CanvasText(CanvasCoord*, char*);
  void setFillColor(Color);
  void setFont(char*, bool);
  void rotate(double);
};

ADD_REPR(CanvasText, repr);

class CanvasImage : public CanvasItem {
public:
  void setOpacity(double);
  void setSize(CanvasCoord*);
  void setSizeInPixels(CanvasCoord*);
  void setDrawIndividualPixels(bool);

  %newobject newBlankImage;
  %newobject newFromPNGFile;
  static CanvasImage *newBlankImage(CanvasCoord*, CanvasICoord*, Color);
  static CanvasImage *newFromPNGFile(CanvasCoord*, char*);
#ifdef OOFCANVAS_USE_IMAGEMAGICK
  %newobject newFromImageMagickFile;
  static CanvasImage *newFromImageMagickFile(CanvasCoord*, char*);
#endif // OOFCANVAS_USE_IMAGEMAGICK
};

ADD_REPR(CanvasImage, repr);

// This is remarkably ugly, but it converts a c++ preprocessor macro
// which is either defined or not into a python-callable boolean
// function which is either true or false.
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


class CanvasLayer {
private:
  CanvasLayer();		// private constructor is not swigged
public:
  ~CanvasLayer();
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
  const std::string &name() {
    return self->name;
  }
}

// OffScreenCanvas is a wrapper that hides the implementation details
// of OSCanvasImpl from the user.  Python wrapping does the same
// thing, so the Python OffScreenCanvas can be based on OSCanvasImpl
// directly, instead of on the C++ OffScreenCanvas.

%rename(OffScreenCanvas) OSCanvasImpl;

class OSCanvasImpl {
public:
  OSCanvasImpl(double);
  ~OSCanvasImpl();
  CanvasLayer *newLayer(const std::string&);
  void deleteLayer(CanvasLayer*);
  CanvasLayer *getLayer(int);
  // TODO PYTHON3: Check that overloaded functions work.
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
};

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

%newobject list_fonts;
std::vector<std::string> *list_fonts();

#endif // OOFCANVAS_SWG
