# This software was produced by NIST, an agency of the U.S. government,
# and by statute is not subject to copyright in the United States.
# Recipients of this software assume all responsibilities associated
# with its operation, modification and maintenance. However, to
# facilitate maintenance we ask that before distributing modified
# versions of this software, you first contact the authors at
# oof_manager@nist.gov. 

target_sources(
  oofcanvasCore
  PRIVATE
  canvas.C
  canvas.h
  canvascircle.C
  canvascircle.h
  canvasimage.C
  canvasimage.h
  canvasimpl.h
  canvasitem.C
  canvasitem.h
  canvasitemimpl.h
  canvaslayer.C
  canvaslayer.h
  canvaslayerimpl.h
  canvaspolygon.C
  canvaspolygon.h
  canvasrectangle.C
  canvasrectangle.h
  canvassegment.C
  canvassegment.h
  canvassegments.C
  canvassegments.h
  canvasshape.C
  canvasshape.h
  canvasshapeimpl.h
  canvastext.C
  canvastext.h
  pythonexportable.h
  pyutility.C
  pyutility.h
  utility.C
  utility.h
  utility_extra.h
  cversion.C
  cversion.h
  swigruntime.h
  )

# Files that aren't compiled:
# canvasshapeimpl.C -- template code that's included elsewhere. Maybe
# we need to give it another name.

swig_sources(
  SWIGFILES version
  LIBRARIES oofcanvasCore
  )

swig_sources(
  SWIGFILES
  oofcanvas 
  LIBRARIES oofcanvasCore)

add_subdirectory(oofcanvasgui)
