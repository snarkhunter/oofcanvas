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

cfiles = ['canvas.C', 'canvascircle.C', 'canvasimage.C', 'canvasitem.C',
          'canvaslayer.C', 'canvaspolygon.C', 'canvasrectangle.C',
          'canvassegment.C', 'canvassegments.C', 'canvasshape.C',
          'canvastext.C', 'utility.C', 'swiglib.C']

hfiles = ['canvas.h', 'canvascircle.h', 'canvasimage.h', 'canvasitem.h',
          'canvaslayer.h', 'canvaspolygon.h', 'canvasrectangle.h',
          'canvassegment.h', 'canvassegments.h', 'canvasshape.h',
          'canvastext.h', 'oofcanvas.h', 'utility.h', 'swiglib.h']


swigfiles = ['oofcanvas.swg']

swigpyfiles = ['oofcanvas.spy']


def set_clib_flags(clib):
    import oof2setuputils
    oof2setuputils.pkg_check("cairomm-1.0", CAIROMM_VERSION, clib)
    oof2setuputils.pkg_check("pango", PANGO_VERSION, clib)
    oof2setuputils.pkg_check("pangocairo", PANGOCAIRO_VERSION, clib)
    oof2setuputils.pkg_check("Magick++", MAGICK_VERSION, clib)
    clib.extra_compile_args.extend(["-DOOFCANVAS_USE_PYTHON",
                                    "-DOOFCANVAS_USE_IMAGEMAGICK"])
