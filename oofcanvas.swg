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

%pragma(python) include="oofcanvas.spy"

%{
#define SWIG_FILE_WITH_INIT
#include "canvas.h"
#include "canvascircle.h"
#include "canvasitem.h"
#include "canvaslayer.h"
#include "canvaspolygon.h"
#include "canvasrectangle.h"
#include "canvassegment.h"
#include "canvassegments.h"
#include "canvastext.h"
using namespace OOFCanvas;
using namespace std;

typedef std::vector<CanvasItem*> CanvasItemList;
%}

%typemap(python, out) PyObject* {
  // typemap(python, out) PyObject*				 
  $target = $source;
}

%typemap(python, in) PyObject* {
  $target = $source;
}

%typemap(python, out) CanvasItem* {
  // typemap(python, out) CanvasItem*				   
  $target = $source->pythonObject();
}

%typemap(python, out) string* {
  // typemap(python, out) string*
  $target = PyString_FromString($source->c_str());
}

%typemap(python, newfree) string* {
  // typemap(python, newfree) string*
  delete $source;
}

void initializePyGTK();

class Color {
public:
  Color(double, double, double);
  Color opacity(double);
};

class Rectangle {
public:
  double xmin();
  double xmax();
  double ymin();
  double ymax();
  %addmethods {
    %new const string *__repr__() {
      return new std::string("[(" +
			     to_string(self->xmin()) + "," +
			     to_string(self->ymin()) + ", (" +
			     to_string(self->xmax()) + "," +
			     to_string(self->ymax()) + ")]");
    }
  }
};

%readonly
Color black, white, red, green, blue, magenta, cyan, yellow, gray;
%readwrite

// Some enums that Cairo defines in C++ need to be accessible from
// Python.  I don't know how this is done in pycairo.  It would be
// convenient if we could just use the pycairo enums, but for now we
// just define constants in C++ for each enum value and expose the
// constants in python as opaque objects.

%{
typedef Cairo::FontSlant CairoFontSlant;
typedef Cairo::FontWeight CairoFontWeight;
typedef Cairo::LineCap CairoLineCap;
typedef Cairo::LineJoin CairoLineJoin;
%}

class CairoFontWeight {};
class CairoFontSlant {};
class CairoLineJoin {};
class CairoLineCap {};

%readonly
CairoFontSlant fontSlantNormal;
CairoFontSlant fontSlantItalic;
CairoFontSlant fontSlantOblique;
CairoFontWeight fontWeightNormal;
CairoFontWeight fontWeightBold;
CairoLineJoin lineJoinMiter;
CairoLineJoin lineJoinRound;
CairoLineJoin lineJoinBevel;
CairoLineCap lineCapButt;
CairoLineCap lineCapRound;
CairoLineCap lineCapSquare;
%readwrite



class CanvasItem {
private:
  CanvasItem();			// private constructor is not swigged
  ~CanvasItem();
public:
  Rectangle boundingBox();
  void drawBoundingBox(double, Color&);
};

class CanvasShape : public CanvasItem {
public:
  void setLineWidth(double);
  void setLineColor(Color&);
  void setLineJoin(CairoLineJoin);
  void setLineCap(CairoLineCap);
};

class CanvasFillableShape : public CanvasShape {
public:
  void setFillColor(Color&);
};

class CanvasRectangle : public CanvasFillableShape {
public:
  CanvasRectangle(double, double, double, double);
  %addmethods {
    %new const string *__repr__() {
      return self->repr();
    }
  }
};

class CanvasSegment : public CanvasShape {
public:
  CanvasSegment(double, double, double, double);
  %addmethods {
    %new const string *__repr__() {
      return self->repr();
    }
  }
};

class CanvasSegments : public CanvasShape {
public:
  CanvasSegments();
  void addSegment(double, double, double, double);
  %addmethods {
    %new const string *__repr__() {
      return self->repr();
    }
  }
};

class CanvasPolygon : public CanvasFillableShape {
public:
  CanvasPolygon();
  void addPoint(double, double);
  %addmethods {
    %new const string *__repr__() {
      return self->repr();
    }
  }
};

class CanvasCircle : public CanvasFillableShape {
public:
  CanvasCircle(double, double, double);
  %addmethods {
    %new const string *__repr__() {
      return self->repr();
    }
  }
};

class CanvasEllipse : public CanvasFillableShape {
public:
  CanvasEllipse(double, double, double, double, double);
  %addmethods {
    %new const string *__repr__() {
      return self->repr();
    }
  }
};

class CanvasDot : public CanvasFillableShape {
public:
  CanvasDot(double, double, double);
  %addmethods {
    %new const string *__repr__() {
      return self->repr();
    }
  }
};

class CanvasArrowhead : public CanvasItem {
public:
  CanvasArrowhead(CanvasSegment*, double, double, double);
  void setReversed();
  void setPixelSize();
  %addmethods {
    %new const string *__repr__() {
      return self->repr();
    }
  }
};

class CanvasText : public CanvasItem {
public:
  CanvasText(double, double, char*, double);
  void setSizeInPixels(bool);
  void setFillColor(Color);
  void setFont(char*);
  void setSlant(CairoFontSlant);
  void setWeight(CairoFontWeight);
  void setAntiAlias(bool);
  void rotate(double);
  %addmethods {
    %new const string *__repr__() {
      return self->repr();
    }
  }
};

class CanvasLayer {
private:
  CanvasLayer();		// private constructor is not swigged
  ~CanvasLayer();
public:
  void addItem(CanvasItem*);
  void setClickable(bool);
  void setOpacity(double);
  void show();
  void hide();
  void raiseBy(int);
  void lowerBy(int);
  void raiseToTop();
  void lowerToBottom();
  %addmethods {
    const char *name() {
      return self->name.c_str();
    }
  }
};

class CanvasItemList {
  // CanvasItemList is just a typedef for std::vector<CanvasItem*>,
  // but swig doesn't need to know that.
public:
  ~CanvasItemList();
  %addmethods {
    int __len__() { return self->size(); }
    %new CanvasItemListIterator *getiter() {
      return new CanvasItemListIterator(self);
    }
  }
};

class CanvasItemListIterator {
public:
  CanvasItem *next_();
  bool done();
};

%name(Canvas) class CanvasPython {
public:
  %name(Canvas) CanvasPython(PyObject*, double ppu);
  %name(Canvas) ~CanvasPython();
  void destroy();
  CanvasLayer *newLayer(char*);
  CanvasLayer *getLayer(int);
  %name(getLayerByName) CanvasLayer *getLayer(char*);
  int nLayers();
  void raiseLayer(int, int);
  void lowerLayer(int, int);
  void lowerLayerToBottom(int);
  void raiseLayerToTop(int);
  void show();
  void draw();
  double getPixelsPerUnit();
  void zoom(double);
  void zoomAbout(double, double, double);
  void fill();
  void center();
  void antialias(bool);
  void setBackgroundColor(double, double, double);
  void setMouseCallback(PyObject*, PyObject*);
  void allowMotionEvents(bool);
  %name(clickedItems) %new CanvasItemList* clickedItems_new(double, double);
  %name(allItems) %new CanvasItemList* allItems_new();
};


%typemap(in) PyObject*;
%typemap(out) PyObject*;

#endif // OOFCANVAS_SWG
