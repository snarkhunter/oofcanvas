# This software was produced by NIST, an agency of the U.S. government,
# and by statute is not subject to copyright in the United States.
# Recipients of this software assume all responsibilities associated
# with its operation, modification and maintenance. However, to
# facilitate maintenance we ask that before distributing modified
# versions of this software, you first contact the authors at
# oof_manager@nist.gov. 

target_sources(
  oofcanvasGUI
  PRIVATE
  guicanvas.C
  guicanvas.h
  guicanvasimpl.h
  guicanvaslayer.C
  guicanvaslayer.h
  oofcanvasgui.h
  rubberband.C
  rubberband.h)

set_public_headers(
  guicanvas.h
  guicanvaslayer.h
  oofcanvasgui.h
  rubberband.h)

swig_sources(
  SWIGFILES oofcanvasgui
  LIBRARIES oofcanvasCore oofcanvasGUI)

