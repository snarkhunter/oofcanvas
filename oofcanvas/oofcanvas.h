// -*- C++ -*-

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modified
 * versions of this software, you first contact the authors at
 * oof_manager@nist.gov. 
 */

// This file includes all of the headers that are required for
// programs calling OOFCanvas in C++.  It's not used when building
// OOFCanvas itself.

#ifndef OOFCANVAS_ALL_H
#define OOFCANVAS_ALL_H

#include "oofcanvas/canvas.h"
#include "oofcanvas/canvascircle.h"
#include "oofcanvas/canvasimage.h"
#include "oofcanvas/canvaslayer.h"
#include "oofcanvas/canvaspolygon.h"
#include "oofcanvas/canvasrectangle.h"
#include "oofcanvas/canvassegment.h"
#include "oofcanvas/canvassegments.h"
#include "oofcanvas/canvastext.h"
#include "oofcanvas/utility.h"
#include "oofcanvas/version.h"

// Handy macros to convert different coordinate classes to OOFCanvas
// coordinates, when passing arguments to OOFCanvas methods.

#define OOFCANVAS_COORD(obj) OOFCanvas::Coord((obj)[0], (obj)[1])
#define OOFCANVAS_ICOORD(obj) OOFCanvas::ICoord((obj)[0], (obj)[1])

#endif // OOFCANVAS_ALL_H
