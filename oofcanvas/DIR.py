# -*- python -*- 

# This software was produced by NIST, an agency of the U.S. government,
# and by statute is not subject to copyright in the United States.
# Recipients of this software assume all responsibilities associated
# with its operation, modification and maintenance. However, to
# facilitate maintenance we ask that before distributing modifed
# versions of this software, you first contact the authors at
# oof_manager@nist.gov.

dirname = 'OOFCANVAS'
clib = 'oofcanvas'
subdirs = ['oofcanvasgui']
clib_order = 0

# cfiles includes all the files that need to be compiled to form clib.
# Files that are included by other files should not be listed here.

cfiles = ['canvas.C', 'canvascircle.C', 'canvasimage.C', 'canvasitem.C',
          'canvaslayer.C', 'canvaspolygon.C', 'canvasrectangle.C',
          'canvassegment.C', 'canvassegments.C', 'canvasshape.C',
          'canvastext.C', 'utility.C', 'version.C',
          'swiglib.C', 'canvas_public.C'
          ]

# TODO: hfiles should only include headers that need to be installed
# in $prefix/include.  Headers that don't need to be installed don't
# need to be listed at all -- they're in git, and will therefore be
# included in the distribution.

hfiles = ['canvas.h', 'canvascircle.h', 'canvasimage.h', 'canvasitem.h',
          'canvaslayer.h', 'canvaspolygon.h', 'canvasrectangle.h',
          'canvassegment.h', 'canvassegments.h', 'canvasshape.h',
          'canvastext.h', 'oofcanvas.h', 'utility.h',
          'swiglib.h', 'canvasshapeimpl.C',
          'pythonexportable.h', 'version.h', 'canvas_public.h']


uninstalled_hfiles = ['utility_private.h', 'canvasitemimpl.h']

if BUILDPYTHONAPI:
    swigfiles = ['oofcanvas.swg']
    swigpyfiles = ['oofcanvas.spy']


def set_clib_flags(clib):
    import oof2setuputils
    oof2setuputils.pkg_check("cairomm-1.0", CAIROMM_VERSION, clib)
    oof2setuputils.pkg_check("pango", PANGO_VERSION, clib)
    oof2setuputils.pkg_check("pangocairo", PANGOCAIRO_VERSION, clib)

    if USEMAGICK:
        oof2setuputils.pkg_check("Magick++", MAGICK_VERSION, clib)
        clib.extra_compile_args.append("-DOOFCANVAS_USE_IMAGEMAGICK")
    if BUILDPYTHONAPI:
        clib.extra_compile_args.append("-DOOFCANVAS_USE_PYTHON")
