# -*- python -*-

# This software was produced by NIST, an agency of the U.S. government,
# and by statute is not subject to copyright in the United States.
# Recipients of this software assume all responsibilities associated
# with its operation, modification and maintenance. However, to
# facilitate maintenance we ask that before distributing modified
# versions of this software, you first contact the authors at
# oof_manager@nist.gov. 

import pygtk
pygtk.require("2.0")
import gtk

import oofcanvas

def run():
    window = gtk.Window()
    window.connect("delete-event", gtk.main_quit)
    canvas = oofcanvas.Canvas(100, 100, 1.0, 1.0)
    widget = canvas.widget()
    widget.show()
    window.add(widget)
    window.present()
    canvas.draw()
    gtk.main()

if __name__ == "__main__":
    run()
