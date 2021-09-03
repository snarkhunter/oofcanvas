# -*- python -*- 

# This software was produced by NIST, an agency of the U.S. government,
# and by statute is not subject to copyright in the United States.
# Recipients of this software assume all responsibilities associated
# with its operation, modification and maintenance. However, to
# facilitate maintenance we ask that before distributing modifed
# versions of this software, you first contact the authors at
# oof_manager@nist.gov.

dirname = 'oofcanvasgui'
clib = 'oofcanvasGUI'
clib_order = 100

cfiles = ['guicanvas.C', 'guicanvaslayer.C', 'rubberband.C']

hfiles = ['guicanvas.h', 'guicanvaslayer.h', 'rubberband.h']

if BUILDPYTHONAPI:
    swigfiles = ['oofcanvasgui.swg']
    swigpyfiles = ['oofcanvasgui.spy']

def set_clib_flags(clib):
    import oof2setuputils
    oof2setuputils.pkg_check("cairomm-1.0", CAIROMM_VERSION, clib)
    oof2setuputils.pkg_check("gtk+-3.0", GTK_VERSION, clib)
    oof2setuputils.pkg_check("pygobject-3.0", PYGOBJECT_VERSION, clib)
    oof2setuputils.pkg_check("pango", PANGO_VERSION, clib)
    oof2setuputils.pkg_check("pangocairo", PANGOCAIRO_VERSION, clib)

    if USEMAGICK:
        oof2setuputils.pkg_check("Magick++", MAGICK_VERSION, clib)
        clib.extra_compile_args.append("-DOOFCANVAS_USE_IMAGEMAGICK")
    if BUILDPYTHONAPI:
        clib.extra_compile_args.append("-DOOFCANVAS_USE_PYTHON")
    
