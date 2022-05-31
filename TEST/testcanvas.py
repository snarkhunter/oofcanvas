# -*- python -*-

# This software was produced by NIST, an agency of the U.S. government,
# and by statute is not subject to copyright in the United States.
# Recipients of this software assume all responsibilities associated
# with its operation, modification and maintenance. However, to
# facilitate maintenance we ask that before distributing modified
# versions of this software, you first contact the authors at
# oof_manager@nist.gov.

from __future__ import print_function

import gi
gi.require_version("Gtk", "3.0")
from gi.repository import Gtk


import math

import oofcanvas
from oofcanvas import oofcanvasgui
oofcanvas.set_mainthread()

ZOOM = 1.1

do_grid = True
do_arrows = False
do_rectangles = False
do_dots = False 
do_circles = False
do_ellipses = False
do_ellipses_rotated = False
do_polygons = False
do_text = True                 # FIX!
do_manysquares = False

# callback for the draw button, not for a canvas event.
def drawCB(button, canvas):

    if do_grid:
        layer = canvas.newLayer("grid")
        layer.setClickable(False)

        # Grid of segments
        segs = oofcanvas.CanvasSegments.create()
        segs.setLineColor(oofcanvas.blue)
        segs.setLineWidth(0.001)
        ndivs = 10
        for v in (1.0/ndivs*x for x in range(ndivs+1)):
            segs.addSegment((0, v), (1, v))
            segs.addSegment((v, 0), (v, 1))
        layer.addItem(segs)

        rect = oofcanvas.CanvasRectangle.create((0.0, 0.0), (1.0, 1.0))
        rect.setLineWidth(0.005)
        rect.setLineColor(oofcanvas.black)
        layer.addItem(rect)

    # -------
    
    if do_arrows:
        # Bunch of arrows
        layer = canvas.newLayer("arrows")
        layer.setClickable(True)
        xc = 1.4                # outside the grid!
        yc = 0.4
        r = 0.3
        r0 = 0.05
        for angle in range(0, 360, 20):
            x0 = xc + r0*math.cos(math.radians(angle))
            y0 = yc + r0*math.sin(math.radians(angle))
            x1 = xc + r*math.cos(math.radians(angle))
            y1 = yc + r*math.sin(math.radians(angle))
            seg = oofcanvas.CanvasSegment.create((x0, y0), (x1, y1));
            seg.setLineWidth(0.004)
            #seg.setLineCap(oofcanvas.lineCapSquare)
            #seg.setLineCap(oofcanvas.lineCapRound)
            seg.setLineCap(oofcanvas.lineCapButt)

            # put the arrow a little bit past the end of the segment so
            # that the segment end doesn't stick out past the arrow.
            arrow = oofcanvas.CanvasArrowhead.create(seg, 1.01, False)
            arrow.setSize(0.02, 0.02)
            layer.addItem(seg)
            layer.addItem(arrow)
            # arrow.drawBoundingBox(0.001, oofcanvas.red)

            arrow = oofcanvas.CanvasArrowhead.create(seg, 0.5, True)
            arrow.setSizeInPixels(10, 10)
            layer.addItem(arrow)
            # arrow.drawBoundingBox(0.001, oofcanvas.red)

    # -------
    
    if do_rectangles:
        rect = oofcanvas.CanvasRectangle.create((0.10, 0.10), (0.20, 0.20))
        rect.setLineWidth(0.02)
        rect.setLineColor(oofcanvas.red)
        layer.addItem(rect)

        rect = oofcanvas.CanvasRectangle.create((0.15, 0.15), (0.40, 0.40))
        rect.setFillColor(oofcanvas.red.opacity(0.5))
        layer.addItem(rect)

    # -------

    if do_dots:
        layer = canvas.newLayer("dots")
        print("layer=", layer, "thisown=", layer.thisown, "name=", layer.name())
        layer.setClickable(True)
        # Bunch of dots
        xmin = ymin = 0.4
        dx = dy = 0.1
        colors = [oofcanvas.green, oofcanvas.yellow, oofcanvas.red]
        for ix in range(3):
            for iy in range(3):
                x = xmin + ix*dx
                y = ymin + iy*dy
                dot = oofcanvas.CanvasDot.create((x, y), 10)
                dot.setFillColor(colors[iy])
                if (ix + iy)%2 == 0:
                    dot.setLineColor(oofcanvas.black)
                    dot.setLineWidthInPixels(1.5)
                layer.addItem(dot)

    # -------

    if do_circles:
        layer = canvas.newLayer("circles")
        layer.setClickable(True)
        circle = oofcanvas.CanvasCircle.create((0, 0), 0.25)
        circle.setFillColor(oofcanvas.red.opacity(0.5))
        layer.addItem(circle)
        circle.drawBoundingBox(0.001, oofcanvas.black)

        # Single diagonal segment
        seg = oofcanvas.CanvasSegment.create((0.0, 1.0), (1.0, 0.0))
        seg.setLineColor(oofcanvas.Color(0.1, 0.1, 0.1))
        seg.setLineWidth(.01)
        layer.addItem(seg)

        # Circles
        circle = oofcanvas.CanvasCircle.create((0.25, 0.75), 0.2)
        circle.setFillColor(oofcanvas.blue.opacity(0.5))
        layer.addItem(circle)
        circle = oofcanvas.CanvasCircle.create((0.5, 0.75), 0.2)
        circle.setLineWidth(0.02)
        circle.setFillColor(oofcanvas.green.opacity(0.5))
        circle.setLineColor(oofcanvas.black)
        layer.addItem(circle)
        circle = oofcanvas.CanvasCircle.create((0.75, 0.75), 0.2)
        circle.setLineWidth(0.02)
        circle.setLineColor(oofcanvas.black)
        layer.addItem(circle)

    # -------

    if do_ellipses:
        layer = canvas.newLayer("ellipses")
        layer.setClickable(True)

        ellipse0 = oofcanvas.CanvasEllipse.create((0.5, 0.5), (0.25, 0.125), 30)
        ellipse0.setLineWidth(0.03)
        ellipse0.setLineColor(oofcanvas.black.opacity(0.9))
        # ellipse0.setFillColor(oofcanvas.magenta.opacity(0.5))
        ellipse0.drawBoundingBox(0.01, oofcanvas.red)
        layer.addItem(ellipse0)

        ellipse1 = oofcanvas.CanvasEllipse.create((0.1, 0.1), (0.15, 0.05), 45)
        ellipse1.setLineWidth(0.002)
        ellipse1.setLineColor(oofcanvas.black.opacity(0.9))
        ellipse1.setFillColor(oofcanvas.blue.opacity(0.5))
        layer.addItem(ellipse1)

        ellipse2 = oofcanvas.CanvasEllipse.create((0.7, 0.2), (0.15, 0.05), 0)
        ellipse2.setLineWidth(0.002)
        ellipse2.setLineColor(oofcanvas.black)
        ellipse2.setFillColor(oofcanvas.blue.opacity(0.3))
        layer.addItem(ellipse2)

    # -------

    if do_ellipses_rotated:
        layer = canvas.newLayer("ellipses 2")

        # A bunch of ellipses at regularly spaced angles, to check that
        # the angles are correct.
        for angle in range(0, 180, 10):
            ell = oofcanvas.CanvasEllipse.create((0.5, 0.5), (0.03, 0.3), angle)
            ell.setLineColor(oofcanvas.red)
            ell.setLineWidth(0.002)
            ell.setFillColor(oofcanvas.gray.opacity(0.1))
            layer.addItem(ell)
        circ = oofcanvas.CanvasCircle.create((0.5, 0.5), 0.3)
        #circ.setLineColor(oofcanvas.white)
        circ.setLineWidth(0.003)
        layer.addItem(circ)

    # -----

    if do_polygons:
        def regularpoly(poly, n, r, cx, cy, s=1):
            dangle = 2*math.pi/n
            for i in range(n):
                theta = ((i*s)%n)*dangle
                poly.addPoint((cx+r*math.cos(theta), cy+r*math.sin(theta)))

        layer = canvas.newLayer("polygons")
        layer.setClickable(True)

        poly = oofcanvas.CanvasPolygon.create()
        poly.setLineWidth(0.01)
        poly.setLineColor(oofcanvas.red)
        regularpoly(poly, n=5, r=0.1, cx=0.2, cy=0.8)
        layer.addItem(poly)

        poly = oofcanvas.CanvasPolygon.create()
        poly.setFillColor(oofcanvas.blue.opacity(0.2))
        regularpoly(poly, n=6, r=0.1, cx=0.4, cy=0.8)
        layer.addItem(poly)

        poly = oofcanvas.CanvasPolygon.create()
        layer.addItem(poly)
        poly.setFillColor(oofcanvas.green.opacity(0.5))
        poly.setLineColor(oofcanvas.green)
        poly.setLineWidth(0.02)
        regularpoly(poly, n=7, r=0.1, cx=0.6, cy=0.8)

        poly = oofcanvas.CanvasPolygon.create()
        layer.addItem(poly)
        poly.setFillColor(oofcanvas.cyan.opacity(0.2))
        poly.setLineColor(oofcanvas.cyan)
        poly.setLineWidth(0.02)
        poly.setLineJoin(oofcanvas.lineJoinRound);
        #poly.setLineJoin(oofcanvas.lineJoinMiter);
        #poly.setLineJoin(oofcanvas.lineJoinBevel);
        regularpoly(poly, 7, r=0.1, cx=0.8, cy=0.8, s=2)

    # ------

    if do_text:
        layer = canvas.newLayer("text")
        layer.setClickable(False)

        text = oofcanvas.CanvasText.create((-0.1, -0.1), "OOFCanvas!")
        text.setFont("Times Bold 0.2", False)
        text.rotate(45)
        text.setFillColor(oofcanvas.red.opacity(1))
        text.drawBoundingBox(0.001, oofcanvas.black)
        print("text 1: ", text.getText(), text.getSizeInPixels())
        layer.addItem(text)

        text = oofcanvas.CanvasText.create((0.2, 0.0), "subtext")
        text.setFont("Times 10", True)
        text.rotate(45)
        text.drawBoundingBox(0.001, oofcanvas.black)
        print("text 2:", text.getText(),  text.getSizeInPixels())
        layer.addItem(text)

    # -------

    # A lot of squares
    if do_manysquares:
        layer = canvas.newLayer("squares")
        layer.setClickable(True)
        n = 1000
        dx = 1./(n+1)
        w = dx/2.5;
        for i in range(n):
            x = (i+1)*dx
            for j in range(n):
                y = (j+1)*dx
                rect = oofcanvas.CanvasRectangle.create((x-w, y-w), (x+w, y+w))
                rect.setLineWidth(dx/20)
                if (i+j)%2 == 0:
                    rect.setFillColor(oofcanvas.black)
                else:
                    rect.setFillColor(oofcanvas.white)
                rect.setLineColor(oofcanvas.red)
                layer.addItem(rect)
            
            

    print("There are", len(canvas.allItems()), "canvas items.")
    # for canvasitem in canvas.allItems():
    #     print canvasitem, "bbox=", canvasitem.boundingBox()
    print("Calling canvas.draw")
    canvas.draw()
    print("Back from canvas.draw")


def reorderCB(button, canvas):
    #which = canvas.nLayers()-1
    which = 1
    # print "Moving layer", canvas.getLayer(which).name(), "down by 2"
    # canvas.getLayer(which).lowerBy(2)
    print("Moving layer", canvas.getLayer(which).name(), "up by 5")
    canvas.getLayer(which).raiseBy(5)
    # print "Lowering layer", canvas.getLayer(which).name(), "to bottom"
    # canvas.getLayer(which).lowerToBottom()
    print([canvas.getLayer(i).name() for i in range(canvas.nLayers())])

hidden = False

def showhideCB(button, canvas):
    which = 2
    global hidden
    if hidden:
        canvas.getLayer(which).show()
    else:
        canvas.getLayer(which).hide()
    hidden = not hidden
    canvas.draw()

def quit(button, canvas):
    canvas.destroy()
    Gtk.main_quit()

def zoom(button, canvas, factor):
    canvas.zoom(factor)

def fill(button, canvas):
    canvas.zoomToFill()

def center(button, canvas):
    canvas.center()

aastate = True
def antialiasCB(button, canvas):
    global aastate
    aastate = not aastate
    canvas.antialias(aastate)
        

def delete_event(window, event, canvas):
    quit(None, canvas)

def mousefunc(eventname, position, button, shift, ctrl, canvas):
    print("mouse:", eventname, position, button, "shift=%d"%shift, "ctrl=%d"%ctrl)
    if button == 1:
        if eventname == "down":
            canvas.allowMotionEvents(True)
        if eventname == "up":
            canvas.allowMotionEvents(False)
            if shift:
                canvas.zoomAbout(position, ZOOM)
            elif ctrl:
                canvas.zoomAbout(position, 1./ZOOM)
            else:
                items = canvas.clickedItems(position)
                print("Clicked on", len(items), "items:")
                for item in items:
                    print( "  ", item)

def run():
    window = Gtk.Window(type=Gtk.WindowType.TOPLEVEL)

    canvas = oofcanvasgui.Canvas(width=200, height=200, ppu=200,
                                 vexpand=True, hexpand=True)

    c = oofcanvas.Color(0,0,0)
    
    canvas.setBackgroundColor(oofcanvas.white)
    canvas.setMouseCallback(mousefunc, canvas)
    canvas.show()

    window.connect("delete-event", delete_event, canvas)
    
    vbox = Gtk.VBox()
    window.add(vbox)

    frame = Gtk.Frame()
    vbox.pack_start(frame, True, True, 3)
    frame.set_shadow_type(Gtk.ShadowType.IN)

    ## Put the canvas and its scrollbars in a Grid.

    # canvasTable = Gtk.Grid()
    # frame.add(canvasTable)

    # frame2 = Gtk.Frame()
    # frame2.set_shadow_type(Gtk.ShadowType.IN)
    # canvasTable.attach(frame2, 0, 0, 1, 1)
    # frame2.add(canvas.layout)

    # hScrollbar = Gtk.Scrollbar.new(adjustment=canvas.get_hadjustment(),
    #                            orientation=Gtk.Orientation.HORIZONTAL)
    # canvasTable.attach(hScrollbar, 0, 1, 1, 1)
    # vScrollbar = Gtk.Scrollbar.new(adjustment=canvas.get_vadjustment(),
    #                                orientation=Gtk.Orientation.VERTICAL)
    # canvasTable.attach(vScrollbar, 1, 0, 1, 1)

    ## Using a ScrolledWindow instead of a Grid is easier, but the
    ## scrollbars are drawn inside the window, overlapping the Canvas,
    ## which might not be optimal.
    swind = Gtk.ScrolledWindow(hadjustment=canvas.get_hadjustment(),
                               vadjustment=canvas.get_vadjustment());
    swind.set_min_content_height(200)
    swind.add(canvas.layout)
    frame.add(swind)

    hbox = Gtk.HBox()
    vbox.pack_start(hbox, False, False, 3)
    
    button = Gtk.Button(label="Quit")
    hbox.pack_start(button, True, True, 3)
    button.connect("clicked", quit, canvas)

    button = Gtk.Button(label="Draw")
    hbox.pack_start(button, True, True, 3)
    button.connect("clicked", drawCB, canvas);

    button = Gtk.Button(label="Reorder")
    hbox.pack_start(button, True, True, 3)
    button.connect("clicked", reorderCB, canvas)

    button = Gtk.Button(label="Show/Hide")
    hbox.pack_start(button, True, True, 3)
    button.connect("clicked", showhideCB, canvas)

    button = Gtk.Button(label="AA")
    hbox.pack_start(button, True, True, 3)
    button.connect("clicked", antialiasCB, canvas)

    hbox = Gtk.HBox()
    vbox.pack_start(hbox, False, False, 3)

    button = Gtk.Button(label="+")
    image = Gtk.Image.new_from_icon_name(
        "help-about-symbolic", Gtk.IconSize.BUTTON)
    button.set_image(image)
    hbox.pack_start(button, True, True, 3)
    button.connect("clicked", zoom, canvas, ZOOM)

    button = Gtk.Button(label="Fill")
    hbox.pack_start(button, True, True, 3)
    button.connect("clicked", fill, canvas)

    button = Gtk.Button(label="Center")
    hbox.pack_start(button, True, True, 3)
    button.connect("clicked", center, canvas)
        
    button = Gtk.Button(label="-")
    hbox.pack_start(button, True, True, 3)
    button.connect("clicked", zoom, canvas, 1./ZOOM)

    vbox.show_all()
    window.present()

    print("CALLING drawCB")
    drawCB(None, canvas)
    print("BACK FROM drawCB")

    print("Original layers: ", [canvas.getLayer(i).name()
                                for i in range(canvas.nLayers())])

    Gtk.main()

if __name__ == "__main__":
    run()
