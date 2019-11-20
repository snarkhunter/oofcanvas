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

# callback for the draw button, not for a canvas event.
def drawCB(button, canvas): 

    layer = canvas.newLayer()
    layer.setClickable(True)

    rect = oofcanvas.CanvasRectangle(0.10, 0.10, 0.20, 0.20)
    rect.setLineWidth(0.02)
    rect.setLineColor(oofcanvas.red)
    layer.addItem(rect)
    
    segs = oofcanvas.CanvasSegments()
    segs.setLineColor(oofcanvas.blue)
    segs.setLineWidth(0.05)
    segs.addSegment(0.30, 0.30, 0.50, 0.50)
    segs.addSegment(0.6, 0.1, 0.6, 0.9)
    segs.addSegment(0.1, 0.6, 0.4, 0.6)
    # segs.addSegment(0.50, 0.50, 0.40, 0.50)
    # segs.addSegment(0.50, 0.50, 0.50, 0.30)
    layer.addItem(segs)

    seg = oofcanvas.CanvasSegment(0.0, 1.0, 1.0, 0.0)
    seg.setLineColor(oofcanvas.Color(0.1, 0.1, 0.1))
    seg.setLineWidth(.01)
    layer.addItem(seg)

    layer = canvas.newLayer()
    layer.setClickable(True)
    rect = oofcanvas.CanvasRectangle(0.15, 0.15, 0.40, 0.40)
    rect.setFillColor(oofcanvas.red.opacity(0.5))
    layer.addItem(rect)

    circle = oofcanvas.CanvasCircle(0.5, 0.75, 0.2)
    circle.setLineWidth(0.01)
    circle.setFillColor(oofcanvas.green.opacity(0.5))
    circle.setLineColor(oofcanvas.black)
    layer.addItem(circle)

    print "There are", len(canvas.allItems()), "canvas items."
    for canvasitem in canvas.allItems():
        print canvasitem
    
    canvas.draw()

def quit(button, canvas):
    canvas.destroy()
    gtk.main_quit()

def zoom(button, canvas, factor):
    canvas.zoom(factor)

def delete_event(window, event, canvas):
    quit(None, canvas)

def mousefunc(eventname, x, y, button, shift, ctrl, canvas):
    print "mouse:", eventname, x, y, button, "shift=%d"%shift, "ctrl=%d"%ctrl
    if button == 1:
        if eventname == "down":
            canvas.allowMotionEvents(True)
        if eventname == "up":
            canvas.allowMotionEvents(False)
            items = canvas.clickedItems(x, y)
            print "Clicked on", len(items), "items:"
            for item in items:
                print "  ", item

def run():
    oofcanvas.initializePyGTK();
    window = gtk.Window()

    canvas = oofcanvas.Canvas(100, 100, 100)
    
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

    hbox = gtk.HBox()
    vbox.pack_start(hbox, expand=0, fill=0)
    button = gtk.Button("+")
    hbox.pack_start(button, expand=1, fill=1)
    button.connect("clicked", zoom, canvas, 1.1)
    button = gtk.Button("-")
    hbox.pack_start(button, expand=1, fill=1)
    button.connect("clicked", zoom, canvas, 0.9)


    vbox.show_all()
    window.present()
    gtk.main()

if __name__ == "__main__":
    run()
