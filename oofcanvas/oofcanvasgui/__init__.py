# -*- python -*-

# This software was produced by NIST, an agency of the U.S. government,
# and by statute is not subject to copyright in the United States.
# Recipients of this software assume all responsibilities associated
# with its operation, modification and maintenance. However, to
# facilitate maintenance we ask that before distributing modified
# versions of this software, you first contact the authors at
# oof_manager@nist.gov. 

import sys
if sys.version_info[0] < 3:
    from oofcanvasgui import *
else:
    from .oofcanvasgui import *

# The actual python Canvas class is defined here.  The only thing that
# it adds to the wrapped C++ PythonCanvas class is the Gtk.Layout and
# functions related to it.  It's convenient to define the Gtk.Layout
# from python so that it's treated the same way as other Gtk objects
# created in python, without requiring us to know any of what that
# actually entails.

# If we ever have functions that return a PythonCanvas object from C++
# to Python, then the PythonCanvas should keep a PyObject* that points
# to its Canvas, so that only one Canvas object is associated with
# each PythonCanvas object.

class Canvas(PythonCanvas):
    def __init__(self, width, height, ppu, **kwargs):
        self.layout = Gtk.Layout(width_request=width, height_request=height,
                            **kwargs)
        PythonCanvas.__init__(self, self.layout, ppu)

    def get_hadjustment(self):
        return self.layout.get_hadjustment()
    def get_vadjustment(self):
        return self.layout.get_vadjustment()
