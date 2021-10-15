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
#include "oofcanvas/utility.h"
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

  void RubberBand::start(CanvasLayer *lyr, double x, double y) {
    layer = lyr;
    startPt = Coord(x, y);
    currentPt = startPt;
    active_ = true;
  }

  void RubberBand::draw(double x, double y) {
    currentPt = Coord(x, y);
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

  void LineRubberBand::start(CanvasLayer *lyr, double x, double y) {
    RubberBand::start(lyr, x, y);
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

  void LineRubberBand::draw(double x, double y) {
    if(layer != nullptr) {
      RubberBand::draw(x, y);
      seg->setPoint1(Coord(x, y));
    }
  }

  //=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

  void RectangleRubberBand::start(CanvasLayer *lyr, double x, double y) {
    RubberBand::start(lyr, x, y);
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

  void RectangleRubberBand::draw(double x, double y) {
    if(layer != nullptr) {
      RubberBand::draw(x, y);
      rect->update(startPt, currentPt);
    }
  }

  //=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

  void CircleRubberBand::start(CanvasLayer *lyr, double x, double y) {
    RubberBand::start(lyr, x, y);

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

  void CircleRubberBand::draw(double x, double y) {
    if(layer != nullptr) {
      RubberBand::draw(x, y);
      circle->setRadius(sqrt((currentPt - startPt).norm2()));
      seg->setPoint1(currentPt);
    }
  }

  //=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

  void EllipseRubberBand::start(CanvasLayer *lyr, double x, double y) {
    RubberBand::start(lyr, x, y);
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

  void EllipseRubberBand::draw(double x, double y) {
    if(layer != nullptr) {
      RubberBand::draw(x, y);
      rect->update(startPt, currentPt);
      ellipse->update(0.5*(currentPt + startPt),
		      0.5*(currentPt - startPt),
		      0.0);
    }
  }

  //=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

  SpiderRubberBand::SpiderRubberBand() {
  }

  void SpiderRubberBand::addPoints(const std::vector<Coord> *pts) {
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

  void SpiderRubberBand::start(CanvasLayer *lyr, double x, double y) {
    RubberBand::start(lyr, x, y);
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

  void SpiderRubberBand::draw(double x, double y) {
    if(layer != nullptr) {
      RubberBand::draw(x, y);
      segs->setPoint0(Coord(x, y));
    }
  }
  
};				// namespace OOFCanvas
