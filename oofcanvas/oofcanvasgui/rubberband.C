// -*- C++ -*-

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modified
 * versions of this software, you first contact the authors at
 * oof_manager@nist.gov. 
 */

#include "oofcanvas/canvascircle.h"
#include "oofcanvas/canvaslayerimpl.h"
#include "oofcanvas/canvasrectangle.h"
#include "oofcanvas/canvassegment.h"
#include "oofcanvas/canvassegments.h"
#include "oofcanvas/oofcanvasgui/rubberband.h"
#include "oofcanvas/utility_extra.h"
#include <math.h>
#include <vector>

namespace OOFCanvas {

  RubberBand::RubberBand()
    : active_(false),
      layer(nullptr),
      lineWidth(1),
      color(black),
      dashColor(white),
      dashLength(0),
      dashed(true),
      coloredDashes(false)
  {}

  RubberBand::~RubberBand() {}

  void RubberBand::start(CanvasLayer *lyr, const Coord &pt) {
    layer = lyr;
    startPt = pt;
    currentPt = startPt;
    active_ = true;
  }

  void RubberBand::update(const Coord &pt) {
    currentPt = pt;
  }

  void RubberBand::stop() {
    layer->clear();
    layer->removeAllItems();
    active_ = false;
  }

  void RubberBand::setDashColor(const Color &c) {
    dashColor = c;
    coloredDashes = true;
  }

  void RubberBand::doDashes(CanvasShape *shape) {
    if(dashed && dashLength > 0) {
      shape->setDashInPixels(dashLength);
      if(coloredDashes)
	shape->setDashColor(dashColor);
    }
  }

  //=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

  void LineRubberBand::start(CanvasLayer *lyr, const Coord &pt) {
    RubberBand::start(lyr, pt);
    seg = new CanvasSegment(startPt, currentPt);
    seg->setLineWidthInPixels(lineWidth);
    seg->setLineColor(color);
    doDashes(seg);
    layer->addItem(seg);
  }

  void LineRubberBand::stop() {
    RubberBand::stop();
    seg = nullptr;
  }

  void LineRubberBand::update(const Coord &pt) {
    if(layer != nullptr) {
      RubberBand::update(pt);
      seg->setPoint1(pt);
    }
  }

  //=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

  void RectangleRubberBand::start(CanvasLayer *lyr, const Coord &pt) {
    RubberBand::start(lyr, pt);
    rect = new CanvasRectangle(startPt, startPt);
    rect->setLineWidthInPixels(lineWidth);
    rect->setLineColor(color);
    doDashes(rect);
    layer->addItem(rect);
  }

  void RectangleRubberBand::stop() {
    RubberBand::stop();
    rect = nullptr;
  }

  void RectangleRubberBand::update(const Coord &pt) {
    if(layer != nullptr) {
      RubberBand::update(pt);
      rect->update(startPt, currentPt);
    }
  }

  //=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

  void CircleRubberBand::start(CanvasLayer *lyr, const Coord &pt) {
    RubberBand::start(lyr, pt);

    circle = new CanvasCircle(startPt, 0.0); // radius = 0
    circle->setLineWidthInPixels(lineWidth);
    circle->setLineColor(color);
    doDashes(circle);
    layer->addItem(circle);

    seg = new CanvasSegment(startPt, currentPt);
    seg->setLineWidthInPixels(lineWidth/2.);
    seg->setLineColor(color);
    doDashes(seg);
    layer->addItem(seg);
  }

  void CircleRubberBand::stop() {
    RubberBand::stop();
    circle = nullptr;
    seg = nullptr;
  };

  void CircleRubberBand::update(const Coord &pt) {
    if(layer != nullptr) {
      RubberBand::update(pt);
      circle->setRadius(sqrt((currentPt - startPt).norm2()));
      seg->setPoint1(currentPt);
    }
  }

  //=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

  void EllipseRubberBand::start(CanvasLayer *lyr, const Coord &pt) {
    RubberBand::start(lyr, pt);
    rect = new CanvasRectangle(startPt, currentPt);
    rect->setLineWidthInPixels(0.5*lineWidth);
    rect->setLineColor(color);
    // TODO: Allow the ellipse to be rotated.  This would require a
    // more complicated API.
    ellipse = new CanvasEllipse(0.5*(currentPt + startPt),
				0.5*(currentPt - startPt),
				0.0 /* angle */ );
    ellipse->setLineWidthInPixels(lineWidth);
    ellipse->setLineColor(color);
    doDashes(ellipse);
    doDashes(rect);
    layer->addItem(rect);
    layer->addItem(ellipse);
  }

  void EllipseRubberBand::stop() {
    RubberBand::stop();
    rect = nullptr;
    ellipse = nullptr;
  }

  void EllipseRubberBand::update(const Coord &pt) {
    if(layer != nullptr) {
      RubberBand::update(pt);
      rect->update(startPt, currentPt);
      ellipse->update(0.5*(currentPt + startPt),
		      0.5*(currentPt - startPt),
		      0.0);
    }
  }

  //=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

  SpiderRubberBand::SpiderRubberBand()
    : lock(new Lock())
  {}

  SpiderRubberBand::~SpiderRubberBand() {
    delete lock;
  }

  void SpiderRubberBand::addPoints(const std::vector<Coord> *pts) {
    KeyHolder kh(*lock, __FILE__, __LINE__);
    points.insert(points.end(), pts->begin(), pts->end());
    if(active_) {
      makeSegs();
    }
  }

  void SpiderRubberBand::makeSegs() {
    for(Coord &pt : points) {
      segs->addSegment(currentPt, pt);
    }
    points.clear();
  }

  void SpiderRubberBand::start(CanvasLayer *lyr, const Coord &pt) {
    KeyHolder kh(*lock, __FILE__, __LINE__);
    RubberBand::start(lyr, pt);
    segs = new CanvasSegments();
    segs->setLineWidthInPixels(lineWidth);
    segs->setLineColor(color);
    makeSegs();
    doDashes(segs);
    layer->addItem(segs);
  }

  void SpiderRubberBand::stop() {
    RubberBand::stop();
    segs = nullptr;
  }

  void SpiderRubberBand::update(const Coord &pt) {
    KeyHolder kh(*lock, __FILE__, __LINE__);
    if(layer != nullptr) {
      RubberBand::update(pt);
      segs->setPoint0(pt);
    }
  }
  
};				// namespace OOFCanvas
