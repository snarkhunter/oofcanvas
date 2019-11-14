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
  #include "canvassegments.h"

  using namespace OOFCanvas;
  using namespace std;

  typedef std::vector<CanvasItem*> CanvasItemList;
%}

%typemap(out) PyObject* {
  $target = $source;
}
%typemap(in) PyObject* {
  $target = $source;
}

void initializePyGTK();

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
  %addmethods {
    %new const string *__repr__() {
      return new string("CanvasRectangle");
    }
  }
};

class CanvasSegments : public CanvasItem {
public:
  CanvasSegments();
  void addSegment(double, double, double, double);
  void setLineWidth(double);
  void setLineColor(double, double, double, double);
  %addmethods {
    %new const string *__repr__() {
      return new string("CanvasSegments");
    }
  }
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
  Canvas(int iw, int ih);
  ~Canvas();
  void destroy();
  PyObject *widget();
  CanvasLayer *newLayer();
  void draw();
  void setPixelsPerUnit(double);
  void setBackgroundColor(double, double, double);
  void setPyMouseCallback(PyObject*, PyObject*);
  void allowMotionEvents(bool);
  //%name(badversion) CanvasItemList clickedItems(double, double);
  %name(clickedItems) %new CanvasItemList* clickedItems_new(double, double);
  %name(allItems) %new CanvasItemList* allItems_new();
  
};


%typemap(in) PyObject*;
%typemap(out) PyObject*;

#endif // OOFCANVAS_SWG
