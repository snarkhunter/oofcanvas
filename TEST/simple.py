# -*- python -*-

# This software was produced by NIST, an agency of the U.S. government,
# and by statute is not subject to copyright in the United States.
# Recipients of this software assume all responsibilities associated
# with its operation, modification and maintenance. However, to
# facilitate maintenance we ask that before distributing modified
# versions of this software, you first contact the authors at
# oof_manager@nist.gov.

import gi
gi.require_version("Gtk", "3.0")
from gi.repository import Gtk

import oofcanvas
from oofcanvas import oofcanvasgui
oofcanvas.set_mainthread()      # only required in debug mode

canvas = oofcanvasgui.Canvas(width=300, height=300, ppu=1.0,
                             vexpand=True, hexpand=True)
layer = canvas.newLayer("layername")

x = 1.
y = 2.
radius = 1.4
circle = oofcanvas.CanvasCircle.create((x, y), radius)
circle.setLineWidthInPixels(1.5)
orange = oofcanvas.Color(1., 0.7, 0.0).opacity(0.5)
circle.setFillColor(orange)
layer.addItem(circle)

# Create a window and add the canvas to it.
window = Gtk.Window(type=Gtk.WindowType.TOPLEVEL)
window.add(canvas.layout)

# Ensure that the canvas is destroyed before the window is.
def delete_event(window, event, canvas):
    canvas.destroy()
    Gtk.main_quit()
window.connect("delete-event", delete_event, canvas)

window.show_all()
canvas.zoomToFill()

Gtk.main()
