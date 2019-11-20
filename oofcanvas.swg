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
#include "canvasitem.h"
#include "canvaslayer.h"
#include "canvasrectangle.h"
#include "canvassegment.h"
#include "canvassegments.h"
  #include "canvascircle.h"
  
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

%readonly
Color black, white, red, green, blue;
%readwrite

class CanvasItem {
private:
  CanvasItem();			// private constructor is not swigged
  ~CanvasItem();
};

class CanvasShape : public CanvasItem {
public:
  void setLineWidth(double);
  void setLineColor(Color&);
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
      return self->print();
    }
  }
};

class CanvasSegment : public CanvasShape {
public:
  CanvasSegment(double, double, double, double);
  %addmethods {
    %new const string *__repr__() {
      return self->print();
    }
  }
};

class CanvasSegments : public CanvasShape {
public:
  CanvasSegments();
  void addSegment(double, double, double, double);
  %addmethods {
    %new const string *__repr__() {
      return self->print();
    }
  }
};

class CanvasCircle : public CanvasFillableShape {
public:
  CanvasCircle(double, double, double);
  %addmethods {
    %new const string *__repr__() {
      return self->print();
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
  // void raise(int);
  // void lower(int);
  // void raiseToTop();
  // void lowerToBottom();
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



class Canvas {
public:
  Canvas(int iw, int ih, double ppu);
  ~Canvas();
  void destroy();
  PyObject *widget();
  CanvasLayer *newLayer();
  void draw();
  void setPixelsPerUnit(double);
  double getPixelsPerUnit();
  void zoom(double);
  void setBackgroundColor(double, double, double);
  void setPyMouseCallback(PyObject*, PyObject*);
  void allowMotionEvents(bool);
  %name(clickedItems) %new CanvasItemList* clickedItems_new(double, double);
  %name(allItems) %new CanvasItemList* allItems_new();
};


%typemap(in) PyObject*;
%typemap(out) PyObject*;

#endif // OOFCANVAS_SWG
