# This software was produced by NIST, an agency of the U.S. government,
# and by statute is not subject to copyright in the United States.
# Recipients of this software assume all responsibilities associated
# with its operation, modification and maintenance. However, to
# facilitate maintenance we ask that before distributing modified
# versions of this software, you first contact the authors at
# oof_manager@nist.gov.

---

# OOFCanvas summary #

OOFCanvas is a replacement for libgnomecanvas, designed for use in
OOF2, but hopefully useful elsewhere. OOFCanvas is based on
[Cairo](https://www.cairographics.org/) and is compatible with gtk3.
It might eventually also be compatible with gtk+2.

OOF2 uses [libgnomecanvas](https://developer.gnome.org/libgnomecanvas/)
to display and interact with images and meshes.  But libgnomecanvas
requires gtk+2, and gtk+2 works only with python3, not python2, and
python2 is being phased out.  In order to upgrade OOF2 to python3, we
need to first upgrade it from gtk+2 to gtk+3, and to do that we need
to first replace libgnomecanvas.

The canvas is a drawing area that can display a variety of shapes,
including text. It can be scrolled, zoomed, and printed.  Items drawn
on the canvas can be selected with the mouse.

All of the code is in C++.  Wrappers for Python 2.7 have been
generated by an old version of SWIG.  Hopefully we've remembered to
distribute the SWIG output so that users don't need to run SWIG
themselves.  All the objects described here are defined in the C++
OOFCanvas namespace.

OOFCanvas is *not* a drop-in replacement for libgnomecanvas.  It's
also not a full-fledged gtk widget.  It's a set of classes that does
some of what libgnomecanvas did and uses gtk.

## Coordinate Systems ##

Objects drawn on the canvas are specified in physical units, which may
be anything convenient to the user.  The coordinate system is (x,y)
where x goes from left to right on the screen, and **y goes from
bottom to top**.   This is not the convention in many graphics
libraries, but is standard for physics.

The conversion from physical to pixel coordinates depends on the size
of the canvas and the current zoom factor, and determines the ppu
(pixels per unit).  The user should be able to use physical units and
not worry about ppu at all.

Some types of canvas items are naturally defined in terms of pixels,
however. OOFCanvas handles the scaling for these too.


## The Canvas Classes ##

Three kinds of Canvas objects are defined.

* `OffScreenCanvas is` the base class.  It can be used to make drawings
that will be printed or saved to a file, but not displayed.

* `Canvas `is derived from `OOFScreenCanvas` (via an intermediate abstract
`GUICanvasBase` class).  It creates a `Gtk.Layout` which can be used in
Gtk3 to put the `Canvas` in a GUI.  It calls user-provided callback
functions in response to mouse events.

* A slightly different Canvas class is available in Python.  It's
actually a C++ class called `PythonCanvas`, which is derived from
`GUICanvasBase` and renamed to `Canvas` when exported to Python.  The main
difference between the C++ and Python `Canvas` classes is that the
Python class expects callback functions to be Python methods, and the
`GtkLayout` is created in Python.

The pixel size of a `Canvas` or `PythonCanvas` is determined by the Gtk
window that it's part of.  The pixel size of an `OffScreenCanvas` is
only computed when it's saved as an image and the size of the image is
given.

## The CanvasLayer Class

Drawing is done by creating one or more `CanvasLayer`s and adding
`CanvasItem`s to them.  `CanvasLayer`s can be shown, hidden, and
reordered, making it easy to change what's visible on the canvas.
Opaque items in higher layers obscure the items in lower layers.

`CanvasLayer`s are created by calling `OffScreenCanvas::newLayer()` and
destroyed by calling either `CanvasLayer::destroy()` or
`OffScreenCanvas::deleteLayer()`.

The size of a layer in pixels is determined by the physical sizes of
the items being displayed, and the current `ppu`.

## The CanvasItem Classes

Everything drawn on a `Canvas` is an instance of a `CanvasItem` subclass.
Pointers to `CanvasItem`s are passed to `CanvasLayer::addItem`.  The
`CanvasLayer` will destroy its `CanvasItem`s when appropriate -- the user
should never destroy them explicitly. 

Each `CanvasItem` has a bunch of parameters that determine its position,
shape, color, and transparency.  Position parameters are always given
in physical units.  Some parameters, such as line widths, can be
interpreted in either physical or pixel units.  If the `CanvasItem` has
such a parameter, it will have a `setLineWidthInPixels` method (or the
equivalent).

Details of each `CanvasItem` subclass are given [somewhere
below](#canvasitems).

## Basic Usage

In C++

```C++
double ppu; // pixels per unit -- initialize to something sensible
// Create a Canvas
Canvas canvas(ppu);
// Get a pointer to the GtkLayout widget
GtkWidget *widget = canvas.gtk(); 

// Install the canvas in the gui.  For example, if it's going into
// a GtkFrame,
frame.add(widget)

// Create a canvas layer
CanvasLayer *layer = canvas.newLayer("layername");

// Add items to the layer
double x=1., y=2., radius=1.4;  // In physical units.
CanvasCircle *circle = new CanvasCircle(x, y, radius);
double w=2.;                    // In physical units, unless ...
circle->setLineWidthInPixels(); // ... setLineWidthInPixels is called.
circle->setLineWidth(w);
Color red(1., 0., 0., 0.5); // r, g, b, a, all in [0.0, 1.0]
circle.setFillColor(red);
layer->addItem(circle);

// Add more items if you want
...

// Draw the items to the canvas
canvas.draw();
```

The same thing in Python is virtually identical

```python
from OOFCANVAS import oofcanvas, oofcanvasgui
canvas = oofcanvasgui.Canvas(width=300, height=300, ppu=1.0,
							 vexpand=True, hexpand=True)
frame.add(canvas.layout)

layer = oofcanvas.CanvasLayer()

x = 1.
y = 2.
radius = 1.4
circle = CanvasCircle(x, y, radius)
circle.setLineWidthInPixels()
w = 2.
circle.setLineWidth(w)
red = oofcanvas.Color(1., 0, 0, 0.5)
circle.setFillColor(red)
layer.addItem(circle)

canvas.draw()
```

Calling `Canvas::draw` doesn't actually draw anything.  Instead, it
generates a Gtk event that causes `GUICanvasBase::drawHandler` to be
called from the Gtk main loop.

## Mouse 

Call `GUICanvasBase::setMouseCallback(MouseCallback callback, void *data)` to
install a mouse event handler.  `callback` will be called whenever a
mouse button is pressed or the mouse is moved.  The signature of the
callback function is

	typedef void (*MouseCallback)(const std::string &event, 
	                              double x, double y,
								  int button,
								  bool shift, bool ctrl,
								  void *data)

where `event` is either `up`, `down`, or `move`. `x` and `y` are the
position of the mouse event in physical coordinates. `button`
indicates which mouse button was pushed, and `shift` and `ctrl`
indicate whether the shift and control keys were pressed.  `data` is
the pointer that was passed to `setMouseCallback`.


To control how the callback is invoked when the mouse is moved, call
`Canvas::allowMotionEvents(value)`.  In C++, `value` is a
`MotionAllowed` enum, with the the choices `MOTION_ALWAYS` (call the
callback whenever the mouse moves), `MOTION_MOUSEDOWN` (call the
callback only if a mouse button is pressed), or `MOTION_NEVER` (don't
call the callback for motion events).  In Python, `MOTION_ALWAYS` et
al. are variables defined in the OOFCanvas namespace.  The default is
`MOTION_NEVER`.

To install a rubberband that will be displayed when the mouse is
moving, call `GUICanvasBase::setRubberBand(RubberBand*)` from the
callback for the mouse-down event.  The current position of the mouse
will be passed to the rubberband's `draw()` method whenever the mouse
is moved.  Various kinds of rubberbands are defined in `rubberband.h`.
To stop displaying the rubberband, pass `nullptr` (in C++) or `None`
(in Python) to `setRubberBand()`.  OOFCanvas does *not* take ownership
of the rubberband object.  The calling code must delete it when done
with it in C++ (if necessary) and make sure to retain a reference to
it in Python (when necessary).

Five classes of `RubberBand` are defined:

- `LineRubberBand` is a straight line from the mouse-down position to
  the current position.
  
- `RectangleRubberBand` is rectangle with one corner at the mouse-down
  position and the diagonally opposite corner at the current position.
  
- `CircleRubberBand` is a circle centered on the mouse-down position
  and passing through the current position.
  
- `EllipseRubberBand` is an ellipse that is fit into a rectangle, as
	in `RectangleRubberBand`.
	
- `SpiderRubberBand` is a set of line segments, starting a given
  points and ending at the current mouse position.  The start points
  are specified by calling `SpiderRubberBand::addPoints(list)`, where
  in C++, `list` is a `std::vector<double>*` containing
  x0,y0,x1,y1,etc. In Python, it's a list of objects with `x` and `y`
  attributes.

OOFCanvas does not handle selection of objects with the mouse, but it
does provide the position of a mouse click as part of the data passed
to the callback function.  Additionally, it is possible to get a list
of all `CanvasItem`s at a point with
`OffScreenCanvas::clickedItems(x,y)`, where `x` and `y` are doubles.

## Scrolling

A canvas can be scrolled in one of two ways.  It can be connected to
`GtkScrollBar`s or other widgets elsewhere in the GUI, and it can respond
to `scroll` events generated within the `GtkLayout`.

To connect to scroll bars, call `scrollbar.set_adjustment(adj)` (in
Python) or `gtk_range_set_adjustment(scrollbar, adj)` (in C++),
where `adj` is the `GtkAdjustment` returned by
`GUICanvasBase::getHAdjustment()` or `GUICanvasBase::getVAdjustment()`.

If the `GtkLayout` receives a scroll event, the mousehandler is called
with `event` set to `scroll`.  The `x` and `y` values are the changes
in position, and can be used to modify the adjustments of the scroll
bars:
```python
def mouseCB(eventtype, x, y, button, shift, ctrl, data):
	if eventtype == "scroll":
		sx = horizontalScrollBar.get_adjustment().get_value()
		horizontalScrollBar.get_adjustment().set_value(sx + x)
		...
```

## OOFCanvas Classes

### Utility Types: Color, Coord, ICoord, etc.

* `Color`

* `Coord`

* `ICoord`

* `Segment`

* `Rectangle`

* `IRectangle`

### Canvas Item Subclasses {#canvasitems}

Here are the predefined `CanvasItem` classes.

#### Abstract CanvasItem Subclasses

##### CanvasShape

This is an abstract base class for most other `CanvasItem` classes.
It defines the following methods:

* `void setLineWidth(double)`
* `void setLineWidthInPixels()`

	Indicates that line width should be interpreted as pixels,
	not physical units.

* `setLineColor(const Color&)`

	See `Color`, below.

* `setLineJoin(Cairo::LineJoin)`

	  In C++, the argument is a member of the Cairo::LineJoin
	  class.  In Python, it's `lineJoinMiter`,
	  `1ineJoinRound`, or `lineJoinBevel`, which are defined
	  in the OOFCanvas namespace.

* `setLineCap(Cairo::LineCap)`

	In C++, the argument is a member of the `Cairo::LineCap`
	class.  In Python, it's `lineCapButt`, `lineCapRound`, or
	`lineCapSquare`, which are defined in the OOFCanvas
	namespace. 

* `setDash(const std::vector<double>&, int offset)`

	The vector contains a pattern of  dash lengths, which are
	in physical units unless `setDashLengthInPixels()` is
	called.  The pattern repeats as necessary.  `offset`
	indicates where the pattern starts.

* `setDash(double)`

	Use a single dash length, which is in physical units,
	unless `setDashLengthInPixels()` is called.

* `setDashLengthInPixels()`

	Interpret the lengths in `setDash` in pixel units, not
	physical units.

* `setDashColor(const Color&)

	Fill the spaces between dashes with the given color,
	instead of leaving them blank.
			
##### CanvasFillableShape

This abstract class in derived from `CanvasShape` and is used for
closed shapes that can be filled with a color.  It provides one
method:

	* `setFillColor(const Color&)`

		Fill the shape with the given color.

#### Concrete CanvasItem Subclasses

* `CanvasArrowhead`

	An arrowhead can be placed on a `CanvasSegment`.  It is *not*
    derived from `CanvasShape`.  Its constructor
    is
	
	* `CanvasArrowHead(const CanvasSegment *segment, double position, double width, double length, bool reversed)`
		
		`segment` is the `CanvasSegment` that the arrowhead will be
		drawn on. `width` and `length` are in physical units unless
		`setPixelSize()` is called.
	
		`position` ranges from 0.0 to 1.0, and determines where the
        tip of the arrow will appear on the segment.  A value of 0.0
        puts the tip at the first point of the segment, and a value of
        1.0 puts it at the second point.  The color of the arrowhead
        is the same as the line color of the `CanvasSegment`.

* `CanvasCircle`

	Derived from `CanvasFillableShape`.  Its constructor is
	
	* `CanvasCircle(double x, double y, double r)`
	
	or
	
	* `CanvasCircle(const Coord&, double r)`
	
	The coordinates of the center and the radius are in physical
	units.  To specify the radius in pixels, use `CanvasDot` instead.

* `CanvasCurve`

	A `CanvasCurve` is a set of line segments connected end to end.
	It is derived from `CanvasShape`, but not `CanvasFillableShape`.
	It is specified by listing the sequence of points where the
	segments meet.
	Its constructors are
	
	* `CanvasCurve()`
	
		Create an empty curve, containing no points.  This constructor
        must be used in Python.
		
	* `CanvasCurve(int n)` 
	
		Create a curve with room reserved for `n` points, but don't
        actually create the points.
		
	* `CanvasCurve(const std::vector<Coord> &points)`
	
		Create a curve with the given points.
		
	Points can be added to a `CanvasCurve` in C++ via
	
		* `CanvasCurve::addPoint(double x, double y)`
		* `CanvasCurve::addPoint(const Coord&)`
		* `CanvasCurve::addPoints(const std::vector<Coord>&)`
		
	In Python, `addPoint(x, y)` and `addCurvePoint(Coord)` 
		
	`CanvasCurve::size()` returns the number of points in the curve.

* `CanvasDot`

	Derived from `CanvasFillableShape`, a `CanvasDot` is a circle
    with fixed size in pixels.  Its line width is also always measured
    in pixels.  The constructor is
	
	* `CanvasDot(double cx, double cy, double radius)`
	
	or
	
	* `CanvasDot(const Coord &center, double radius)`

* `CanvasEllipse`

	Derived from `CanvasFillableShape`.  The constructor is
	
	* `CanvasEllipse(double cx, double cy, double r0, double r1, double angle)`

	or
	
	* `CanvasEllipse(const Coord &c, const Coord &r, double angle)`
	
	where `cx` and `cy`, or `c`, give the center in physical units.
    `r0` and `r1`, or the components of `r`, are the radii in physical
    units.  `r0` is the radius in the x direction before rotation.
    The rotation angle in degrees is measured counterclockwise.
	
* `CanvasImage`

	`CanvasImage` can display a PNG file, or if compiled with the
    ImageMagick library, any format that ImageMagick can read. The
    constructor creates an empty image:
	
	* `CanvasImage(const Coord &position, const Coord &size, const ICooord &npixels)`
	  
	where `position` is the position of the lower left corner of the
	image in physical coordinates, and `size` is its displayed size.
	If `CanvasImage::setPixelSize()` is called, the size is
	interpreted as a size in pixels, otherwise it's in physical units.
	  
	For convenience `CanvasImage` includes some static factory methods
    for creating `CanvasImage` objects.  For all of these, `x` and `y`
    are the position of the lower left corner of the image in physical
    units.  `width` and `height` are the displayed size, which is in physical
    units unless `CanvasImage::setPixelSize()` is called on the
    returned `CanvasImage`.
	
	* `CanvasImage* CanvasImage::newBlankImage(double x, double y,
	int nx, int ny,
      double width, double height, double red, double green, double
      blue, double alpha)`
	  
	  Create a "blank" image, filled with a single color.  `nx` and
	  `ny` are size of the image in pixels.  `red`, `green`, `blue`,
	  and `alpha` are all between 0 and 1 and specify the color.
		  
	* `CanvasImage *CanvasImage::newFromPNGfile(double x, double y,
        const std::string &filename, double width, double height)`
		
		Create a CanvasImage from a PNG file.  `x` and `y` are the
		position of the lower left corner of the image in physical
		coordinates, and `width` and `height` are its displayed size.
          
	

* `CanvasPolygon`

* `CanvasRectangle`

	Derived from `CanvasFillableShape`.  The constructor is 

	* `CanvasRectangle(double xmin, double xmax, double ymin, double ymax)`
		  
	or
			  
	* `CanvasRectangle(const Coord&, const Coord&)`
		
	where the `Coord`s are any two opposite corners of the
	rectangle.  The corners are given in physical units.  Only the
	first constructor is available in Python.
			
* `CanvasSegment`

	A single line segment, derived from `CanvasShape`.  The
	constructor is
		
	* `CanvasSegment(double x0, double y0, double x1, double y1)`
		
	or
			
	* `CanvasSegment(const Coord &point0, const Coord &point1)`

	The positions are given in physical units.  Only the first form of
	the constructor is available in Python.
		

* `CanvasSegments`

	'CanvasSegments` is derived from `CanvasShape` and draws a set of
    unconnected line segments all with the same color and width.

* `CanvasText`

## Other Canvas Methods

## Appendix: Internal Order of Operations

It shouldn't be necessary to understand this section in order to use
OOFCanvas.  It's here to help development.

Each `CanvasLayer` contains a `Cairo::ImageSurface` which contains a
bitmap of what's been drawn in the layer, a `Cairo::Context` which
controls drawing to surface, and a `Rectangle` which is the bounding box
(in physical coordinates) of all of the layer's `CanvasItems`. 

When a `CanvasItem` is added to a `CanvasLayer`, the layer is marked
"dirty" and the item is stored in the layer.  No drawing is done at
this point.

When all items have been added to the layers, calling
`GUICanvasBase::draw()` generates a draw event on the `GtkLayout`.  This
causes `GUICanvasBase::drawHandler()` to be called.  The argument to
drawHandler is the `Cairo::Context` for drawing to the `GtkLayout`'s
`Cairo::Surface`. 

`GUICanvasBase::drawHandler()` begins by computing the horizontal and
vertical offsets that will be used to keep the image centered in
the gtk window (if the image is smaller than the window) or at the
position determined by the scroll bars (if the image is larger than
the window).

Next, drawHandler calls `Canvas::setTransform()`, which computes the
matrix that converts from physical coordinates to bitmap coordinates
within the layer, given the ppu.  The `GtkLayout` is resized if
necessary so that it is large enough to accomodate the bounding boxes
of all of the layers, plus an optional margin (set by
`OffScreenCanvas::setMargin()`).  Note that a layer's bounding box, in
physical units, can depend on the ppu if the layer contains items with
sizes given in pixels.

What happens next depends on whether or not a rubberband is being
drawn.  If there is no rubberband, `GUICanvasBase::drawHandler` draws the
background color and then, for each layer from bottom to top, tells
the layer to draw all of its `CanvasItems`s to its own `Cairo::ImageSurface`
(`CanvasLayer::render()`), and copies the layer's surface to the
`GtkLayout`'s surface (`CanvasLayer::copyToCanvas()`) at the position
given by the scroll bars.  (`CanvasLayer::render()` only redraws its
items if any have changed since the last time they were drawn.)

If there is an active rubberband, on the first call to `drawHandler`
after the mouse button was  pressed all of the `CanvasLayer`s other
than the rubberband's layer are rendered to a separate
`Cairo::ImageSurface` called the `nonRubberBandBuffer`.  Then this
buffer is copied to the `GtkLayout` with the rubberband drawn on top
of it each time the rubberband changes.  The `nonRubberBandBuffer` is
updated first if any other layers have changed.





					 





