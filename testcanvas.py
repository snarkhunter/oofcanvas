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
    layer.setClickable(False)

    # rect = oofcanvas.CanvasRectangle(0.10, 0.10, 0.20, 0.20)
    # rect.setLineWidth(0.02)
    # rect.setLineColor(oofcanvas.red)
    # layer.addItem(rect)

    # Grid of segments
    segs = oofcanvas.CanvasSegments()
    segs.setLineColor(oofcanvas.blue)
    segs.setLineWidth(0.001)
    ndivs = 10
    for v in (1.0/ndivs*x for x in range(ndivs+1)):
        segs.addSegment(0, v, 1, v)
        segs.addSegment(v, 0, v, 1)
    layer.addItem(segs)

    # # Single diagonal segment
    # seg = oofcanvas.CanvasSegment(0.0, 1.0, 1.0, 0.0)
    # seg.setLineColor(oofcanvas.Color(0.1, 0.1, 0.1))
    # seg.setLineWidth(.01)
    # layer.addItem(seg)

    layer = canvas.newLayer()
    layer.setClickable(True)

    # rect = oofcanvas.CanvasRectangle(0.15, 0.15, 0.40, 0.40)
    # rect.setFillColor(oofcanvas.red.opacity(0.5))
    # layer.addItem(rect)

    # # Circle
    # circle = oofcanvas.CanvasCircle(0.5, 0.75, 0.2)
    # circle.setLineWidth(0.02)
    # circle.setFillColor(oofcanvas.green.opacity(0.5))
    # circle.setLineColor(oofcanvas.black)
    # layer.addItem(circle)

    ellipse0 = oofcanvas.CanvasEllipse(0.5, 0.5, 0.25, 0.125, 30)
    ellipse0.setLineWidth(0.03)
    ellipse0.setLineColor(oofcanvas.black.opacity(0.9))
    # ellipse0.setFillColor(oofcanvas.magenta.opacity(0.5))
    layer.addItem(ellipse0)
    
    ellipse1 = oofcanvas.CanvasEllipse(0.1, 0.1, 0.15, 0.05, 45)
    ellipse1.setLineWidth(0.002)
    ellipse1.setLineColor(oofcanvas.black.opacity(0.9))
    ellipse1.setFillColor(oofcanvas.blue.opacity(0.5))
    layer.addItem(ellipse1)

    ellipse2 = oofcanvas.CanvasEllipse(0.7, 0.2, 0.15, 0.05, 0)
    ellipse2.setLineWidth(0.002)
    ellipse2.setLineColor(oofcanvas.black)
    ellipse2.setFillColor(oofcanvas.blue.opacity(0.3))
    layer.addItem(ellipse2)

    layer = canvas.newLayer()
    layer.setClickable(False)
    for ellipse in (ellipse0,ellipse1,ellipse2):
        bb = ellipse.boundingBox();
        rect = oofcanvas.CanvasRectangle(bb.xmin(), bb.ymin(),
                                         bb.xmax(), bb.ymax())
        rect.setLineColor(oofcanvas.black)
        rect.setLineWidth(0.002)
        layer.addItem(rect)

    # for angle in range(0, 91, 10):
    #     ell = oofcanvas.CanvasEllipse(0.5, 0.5, 0.03, 0.3, angle)
    #     ell.setLineColor(oofcanvas.red)
    #     ell.setLineWidth(0.01)
    #     #ell.setFillColor(oofcanvas.gray.opacity(0.1))
    #     layer.addItem(ell)
    #     bb = ell.boundingBox()
    #     rect = oofcanvas.CanvasRectangle(bb.xmin(), bb.ymin(), bb.xmax(),
    #                                      bb.ymax())
    #     rect.setLineWidth(0.001)
    #     rect.setLineColor(oofcanvas.black)
    #     layer.addItem(rect)
    # circ = oofcanvas.CanvasCircle(0.5, 0.5, 0.3)
    # circ.setLineColor(oofcanvas.black)
    # circ.setLineWidth(0.01)
    # layer.addItem(circ)



    print "There are", len(canvas.allItems()), "canvas items."
    for canvasitem in canvas.allItems():
        print canvasitem, "bbox=", canvasitem.boundingBox()
    
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

    canvas = oofcanvas.Canvas(200, 200, 200)
    
    canvas.setBackgroundColor(0.9, 0.9, 0.9)
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

    drawCB(None, canvas)
    
    gtk.main()

if __name__ == "__main__":
    run()
