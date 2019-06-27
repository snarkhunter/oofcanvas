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

def drawCB(button, canvas):
    layer = canvas.newLayer()
    rect = oofcanvas.CanvasRectangle(10, 10, 20, 20)
    rect.setLineWidth(2.)
    rect.setLineColor(1.0, 0, 0, 1.0)
    layer.addItem(rect)

def run():
    oofcanvas.initializePyGTK();
    window = gtk.Window()
    window.connect("delete-event", gtk.main_quit)

    canvas = oofcanvas.Canvas(100, 100)
    canvas.setPixelsPerUnit(100)
    canvas.setBackgroundColor(0.9, 0.9, 1.0)
    widget = canvas.widget()
    widget.show()

    
    vbox = gtk.VBox()
    window.add(vbox)
    
    vbox.pack_start(widget, expand=1, fill=1)
    
    button = gtk.Button("Quit")
    vbox.pack_start(button, expand=0, fill=0)
    button.connect("clicked", gtk.main_quit)

    button = gtk.Button("Draw")
    vbox.pack_start(button, expand=0, fill=0)
    button.connect("clicked", drawCB, canvas);
    
    vbox.show_all()
    window.present()
    gtk.main()

if __name__ == "__main__":
    run()
