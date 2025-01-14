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
  canvasexception.C  
  canvasexception.h
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
  pythonlock.h
  pyutility.C
  pyutility.h
  utility.C
  utility.h
  utility_extra.h
  oofswigruntime.h)

# oofswigruntime.h depends on swigruntime.h.  Because swigruntime.h is
# the output of a custom command the dependency isn't picked up by the
# usual methods.
set_property(
  SOURCE oofswigruntime.h
  DIRECTORY ${PROJECT_SOURCE_DIR}
  PROPERTY OBJECT_DEPENDS swigruntime.h)

target_include_directories(
  oofcanvasCore
  PUBLIC
  ${NUMPY_INCLUDE})

# Files that aren't compiled:
# canvasshapeimpl.C -- template code that's included elsewhere. Maybe
# we need to give it another name.

set_public_headers(
  canvas.h
  canvascircle.h
  canvasimage.h
  canvasitem.h
  canvaslayer.h
  canvaspolygon.h
  canvasrectangle.h
  canvassegment.h
  canvassegments.h
  canvasshape.h
  canvastext.h
  utility.h
  
  # TODO: pythonexportable.h, swigruntime.h, and pyutility.h are
  # listed here only because they're imported by canvasitem.h.  It
  # would be good if they weren't exposed to the user, but that may
  # require refactoring CanvasItem.
  pythonexportable.h
  oofswigruntime.h
  pyutility.h
  pythonlock.h
  )  

swig_sources(
  SWIGFILES
  oofcanvas 
  LIBRARIES oofcanvasCore)

add_subdirectory(oofcanvasgui)
