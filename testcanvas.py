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
    # callback for the draw button, not for a canvas event.
    layer = canvas.newLayer()
    rect = oofcanvas.CanvasRectangle(10, 10, 20, 20)
    rect.setLineWidth(2.)
    rect.setLineColor(1.0, 0, 0, 1.0)
    layer.addItem(rect)
    
    segs = oofcanvas.CanvasSegments()
    segs.setLineColor(0., 0., 1., 1.)
    segs.setLineWidth(3.)
    segs.addSegment(30, 30, 50, 50)
    segs.addSegment(50, 50, 40, 50)
    segs.addSegment(50, 50, 50, 30)
    layer.addItem(segs)
    
    layer = canvas.newLayer()
    rect = oofcanvas.CanvasRectangle(15, 15, 40, 40)
    rect.setFillColor(1., 0., 0., 0.5)
    layer.addItem(rect)

    print "allItems=", canvas.allItems()
    print "There are", len(canvas.allItems()), "canvas items."
    for canvasitem in canvas.allItems():
        print canvasitem
    
    canvas.draw()

def quit(button, canvas):
    canvas.destroy()
    gtk.main_quit()

def delete_event(window, event, canvas):
    quit(None, canvas)

def mousefunc(eventname, x, y, button, state, canvas):
    print "mouse:", eventname, x, y, button, state
    if eventname == "down":
        canvas.allowMotionEvents(True)
    if eventname == "up":
        canvas.allowMotionEvents(False)

def run():
    oofcanvas.initializePyGTK();
    window = gtk.Window()

    canvas = oofcanvas.Canvas(100, 100)
    canvas.setPixelsPerUnit(100)
    canvas.setBackgroundColor(0.9, 0.9, 1.0)
    canvas.setPyMouseCallback(mousefunc, canvas)
    widget = canvas.widget()
    widget.show()


    window.connect("delete-event", delete_event, canvas)
    
    vbox = gtk.VBox()
    window.add(vbox)
    
    vbox.pack_start(widget, expand=1, fill=1)
    
    button = gtk.Button("Quit")
    vbox.pack_start(button, expand=0, fill=0)
    button.connect("clicked", quit, canvas)

    button = gtk.Button("Draw")
    vbox.pack_start(button, expand=0, fill=0)
    button.connect("clicked", drawCB, canvas);
    
    vbox.show_all()
    window.present()
    gtk.main()

if __name__ == "__main__":
    run()
