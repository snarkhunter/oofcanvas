// -*- C++ -*-

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modified
 * versions of this software, you first contact the authors at
 * oof_manager@nist.gov. 
 */

#include "oofcanvas/canvasimpl.h"
#include "oofcanvas/canvassegment.h"
#include "oofcanvas/canvasshapeimpl.h"
#include <cassert>
#include <iostream>
#include <math.h>

namespace OOFCanvas {

  class CanvasSegmentImplementation :
    public CanvasShapeImplementation<CanvasSegment>
  {
  public:
    CanvasSegmentImplementation(CanvasSegment *seg, const Rectangle &bb)
      : CanvasShapeImplementation<CanvasSegment>(seg, bb)
    {}
    virtual void drawItem(Cairo::RefPtr<Cairo::Context>) const;
    virtual bool containsPoint(const OSCanvasImpl*, const Coord&) const;
  };

  CanvasSegment::CanvasSegment(const Coord &p0, const Coord &p1)
    : CanvasShape(new CanvasSegmentImplementation(this, Rectangle(p0, p1))),
      segment(p0, p1)
  {}

  CanvasSegment::CanvasSegment(const Coord *p0, const Coord *p1)
    : CanvasShape(new CanvasSegmentImplementation(this, Rectangle(*p0, *p1))),
      segment(*p0, *p1)
  {}

  const std::string &CanvasSegment::classname() const {
    static const std::string name("CanvasSegment");
    return name;
  }

  void CanvasSegment::setPoint0(const Coord &p) {
    segment.p0 = p;
    implementation->bbox = Rectangle(segment.p0, segment.p1);
    modified();
  }

  void CanvasSegment::setPoint1(const Coord &p) {
    segment.p1 = p;
    implementation->bbox = Rectangle(segment.p0, segment.p1);
    modified();
  }

  void CanvasSegmentImplementation::drawItem(Cairo::RefPtr<Cairo::Context> ctxt)
    const
  {
    const Segment &segment = canvasitem->getSegment();
    ctxt->move_to(segment.p0.x, segment.p0.y);
    ctxt->line_to(segment.p1.x, segment.p1.y);
    stroke(ctxt);
  }

  bool CanvasSegmentImplementation::containsPoint(const OSCanvasImpl *canvas,
						  const Coord &pt) const
  {
    double alpha = 0;
    double distance2 = 0; // distance squared from pt to segment along normal
    double lw = lineWidthInUserUnits(canvas);
    canvasitem->getSegment().projection(pt, alpha, distance2);
    return (alpha >= 0.0 && alpha <= 1.0 && distance2 < 0.25*lw*lw);
  }

  std::string CanvasSegment::print() const {
    return to_string(*this);
  }

  std::ostream &operator<<(std::ostream &os, const CanvasSegment &seg) {
    os << "CanvasSegment(" << seg.segment << ")";
    return os;
  }

  //=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

  // CanvasArrowheads are meant to be used to decorate CanvasSegments.
  // They're not usable on their own.

  // The position argument passed to the constructor determines where
  // along the segment the tip of the arrow will appear.  A value of 0
  // puts the tip of the arrow on the first point of the segment.  A
  // value of 1 puts it on the second point.  Intermediate values put
  // it somewhere in the middle.

  // If setReversed() is called, the arrow will point in the direction
  // of the CanvasSegment's first point.  Otherwise it points toward
  // the second point.

  // Oddities that might not be worth fixing:
  // * If you use position=0
  //   and don't call setReversed(), or position=1 and do call
  //   setReversed(), the tail of the arrow will hang off of the end of
  //   the segment.  This may not be what you want.
  // * The end of the segment isn't trimmed to fit inside the
  //   arrowhead.  If you have a thick line and a sharp arrowhead, it
  //   might look funny.  This might be fixed by creating an arrowhead
  //   shaped mask for the segment, if the mask could be applied only
  //   to the ends of the segment somehow.

  // static
  Rectangle CanvasArrowhead::arrowheadBBox(
		  const CanvasSegment *seg, double position,
		  double width, double length, bool reversed)
  {
    // The easiest way to compute the bounding box is to use a Context
    // to transform the coordinates, as in drawItem.  But we don't
    // have a Context when findBoundingBox is called, so create a
    // dummy one.  See CanvasText for a similar treatment.

    auto surface = Cairo::RefPtr<Cairo::ImageSurface>(
		      Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, 1, 1));
    cairo_t *ct = cairo_create(surface->cobj());
    auto ctxt = Cairo::RefPtr<Cairo::Context>(new Cairo::Context(ct, true));
    ctxt->save();
    ctxt->rotate(seg->segment.angle());
    // Find bounding box in the arrow's coordinates
    double l = reversed ? length : -length;
    double w = width/2.;
    std::vector<Coord> corners({{l, -w}, {0, -w}, {0, w}, {l, w}});
    // convert to device space on the dummy device
    for(Coord &corner : corners)
      ctxt->user_to_device(corner.x, corner.y);
    // convert from device space to the real user coordinates
    ctxt->restore();
    for(Coord &corner : corners)
      ctxt->device_to_user(corner.x, corner.y);
    Rectangle bb = Rectangle(corners[0], corners[1]);
    bb.swallow(corners[2]);
    bb.swallow(corners[3]);
    // Put the bbox at the right place on the segment.
    bb.shift(seg->segment.interpolate(position));
    return bb;
  }

  class CanvasArrowheadImplementation
  : public CanvasItemImplementation<CanvasArrowhead>
  {
  public:
    CanvasArrowheadImplementation(CanvasArrowhead *item, const Rectangle &bb)
      : CanvasItemImplementation<CanvasArrowhead>(item, bb)
    {}
    virtual void drawItem(Cairo::RefPtr<Cairo::Context>) const;
    virtual bool containsPoint(const OSCanvasImpl*, const Coord&) const;
    virtual void pixelExtents(double&, double&, double&, double&) const;
    Rectangle pixelBBox;
  };
  

  CanvasArrowhead::CanvasArrowhead(const CanvasSegment *seg,
				   double pos,
				   bool reversed)
    : CanvasItem(new CanvasArrowheadImplementation(this, Rectangle())),
      segment(seg),
      width(0),	       // width of arrowhead, perpendicular to segment
      length(0),       // length of arrowhead along segment
      position(pos),   // relative position along segment, in the range 0->1.
      pixelScaling(false),
      reversed(false)	    
  {}

  const std::string &CanvasArrowhead::classname() const {
    static const std::string name("CanvasArrowhead");
    return name;
  }

  void CanvasArrowhead::setSize(double w, double l) {
    length = l;
    width = w;
    pixelScaling = false;
    implementation->bbox = arrowheadBBox(segment, position, w, l, reversed);
    modified();
  }

  void CanvasArrowhead::setSizeInPixels(double w, double l) {
    length = l;
    width = w;
    pixelScaling = true;
    Coord loc = segment->segment.interpolate(position);
    // compute unscaled sized of arrowhead, and save for use in pixelExtents
    dynamic_cast<CanvasArrowheadImplementation*>(implementation)->pixelBBox =
      arrowheadBBox(segment, position, w, l, reversed);
    implementation->bbox = Rectangle(loc, loc);
    modified();
  }
  
  void CanvasArrowheadImplementation::drawItem(
				       Cairo::RefPtr<Cairo::Context> ctxt)
    const
  {
    assert(bbox.initialized());	// need to call setSize or setSizeInPixels
    
    Coord loc = canvasitem->getSegment().interpolate(canvasitem->getPosition());
    ctxt->translate(loc.x, loc.y);

    // If converting to pixel coordinates, get the size before rotating
    double l = canvasitem->getLength();
    double w = canvasitem->getWidth();
    if(canvasitem->getPixelScaling())
      ctxt->device_to_user_distance(l, w);

    if(canvasitem->getReversed())
      l *= -1;
    
    ctxt->rotate(canvasitem->getSegment().angle());
    ctxt->move_to(0., 0.);
    ctxt->line_to(-l, w/2);
    ctxt->line_to(-l, -w/2);
    ctxt->close_path();
    setColor(canvasitem->getCanvasSegment().getLineColor(), ctxt);
    ctxt->fill();
  }


  void CanvasArrowheadImplementation::pixelExtents(double &left, double &right,
						   double &up, double &down)
    const
  {
    Coord loc(canvasitem->getSegment().interpolate(canvasitem->getPosition()));
    left = loc.x - pixelBBox.xmin();
    right = pixelBBox.xmax() - loc.x;
    up = pixelBBox.ymax() - loc.y;
    down = loc.y - pixelBBox.ymin();
  }

  bool CanvasArrowheadImplementation::containsPoint(const OSCanvasImpl*,
						    const Coord&)
    const
  {
    // Only recognize mouse clicks on the associated segment.
    return false;
  }

  std::string CanvasArrowhead::print() const {
    return to_string(*this);
  }

  std::ostream &operator<<(std::ostream &os, const CanvasArrowhead &arr) {
    os << "CanvasArrowhead(" << arr.segment->segment
       << ", " << arr.position << ", " << arr.width
       << ", " << arr.length << ")";
    return os;
  }
  
};
