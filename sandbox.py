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
from gi.repository import Gdk
from gi.repository import GObject
from gi.repository import Pango

import math

import oofcanvas
import chooser
#import cairo

ZOOM = 1.1
defaultfont = "Times 20"
   

# callback for the draw button, not for a canvas event.
def drawCB(button, canvas, fontname=defaultfont):

    layer = canvas.newLayer("grid")
    layer.setClickable(False)

    # Grid of segments
    xmin = -1.
    xmax = 1.
    ymin = -1.
    ymax = 1.
    layer = canvas.newLayer("grid")
    layer.setClickable(False)
    segs = oofcanvas.CanvasSegments()
    segs.setLineColor(oofcanvas.blue)
    segs.setLineWidth(0.001)
    ndivs = 10
    for x in (xmin+(xmax - xmin)/ndivs*n for n in range(ndivs+1)):
        segs.addSegment(x, ymin, x, ymax)
    for y in (ymin+(ymax - ymin)/ndivs*n for n in range(ndivs+1)):
        segs.addSegment(xmin, y, xmax, y)

    layer.addItem(segs)

    rect = oofcanvas.CanvasRectangle(0.0, 0.0, 1.0, 1.0)
    rect.setLineWidth(0.005)
    rect.setLineColor(oofcanvas.black)
    layer.addItem(rect)

    # # Bunch of arrows
    # layer = canvas.newLayer("arrows")
    # layer.setClickable(True)
    # xc = 1.4
    # yc = 0.4
    # r = 0.3
    # r0 = 0.05
    # for angle in range(0, 360, 20):
    #     x0 = xc + r0*math.cos(math.radians(angle))
    #     y0 = yc + r0*math.sin(math.radians(angle))
    #     x1 = xc + r*math.cos(math.radians(angle))
    #     y1 = yc + r*math.sin(math.radians(angle))
    #     seg = oofcanvas.CanvasSegment(x0, y0, x1, y1);
    #     seg.setLineWidth(0.004)
    #     #seg.setLineCap(oofcanvas.lineCapSquare)
    #     #seg.setLineCap(oofcanvas.lineCapRound)
    #     seg.setLineCap(oofcanvas.lineCapButt)

    #     # put the arrow a little bit past the end of the segment so
    #     # that the segment end doesn't stick out past the arrow.
    #     arrow = oofcanvas.CanvasArrowhead(seg, 1.01, 0.02, 0.02)
    #     layer.addItem(seg)
    #     layer.addItem(arrow)
    #     # arrow.drawBoundingBox(0.001, oofcanvas.red)

    #     arrow = oofcanvas.CanvasArrowhead(seg, 0.5, 10, 10)
    #     arrow.setPixelSize()
    #     arrow.setReversed()
    #     layer.addItem(arrow)
    #     # arrow.drawBoundingBox(0.001, oofcanvas.red)

    # -------
    
    # rect = oofcanvas.CanvasRectangle(0.10, 0.10, 0.20, 0.20)
    # rect.setLineWidth(0.02)
    # rect.setLineColor(oofcanvas.red)
    # layer.addItem(rect)

    # rect = oofcanvas.CanvasRectangle(0.15, 0.15, 0.40, 0.40)
    # rect.setFillColor(oofcanvas.red.opacity(0.5))
    # layer.addItem(rect)

    # -------
    
    # layer = canvas.newLayer("dots")
    # layer.setClickable(True)
    # # Bunch of dots
    # xmin = ymin = -0.2
    # dx = dy = 0.1
    # colors = [oofcanvas.green, oofcanvas.yellow, oofcanvas.red]
    # for ix in range(3):
    #     for iy in range(3):
    #         x = xmin + ix*dx
    #         y = ymin + iy*dy
    #         dot = oofcanvas.CanvasDot(x, y, 10)
    #         dot.setFillColor(colors[iy])
    #         if (ix + iy)%2 == 0:
    #             dot.setLineColor(oofcanvas.black)
    #             dot.setLineWidth(1.5)
    #         layer.addItem(dot)

    ## If this is the only canvas layer, zoom to fill does not work,
    ## because no scale can be established.
    # layer = canvas.newLayer("coincident dots")
    # x = y = 0.5
    # dot = oofcanvas.CanvasDot(x, y, 20)
    # dot.setFillColor(oofcanvas.blue)
    # layer.addItem(dot)
    # dot = oofcanvas.CanvasDot(x, y, 10)
    # dot.setFillColor(oofcanvas.red.opacity(0.5))
    # layer.addItem(dot)

    # circle = oofcanvas.CanvasCircle(0, 0, 0.25)
    # circle.setFillColor(oofcanvas.red.opacity(0.5))
    # layer.addItem(circle)
    # circle.drawBoundingBox(0.001, oofcanvas.black)

    # # Single diagonal segment
    # seg = oofcanvas.CanvasSegment(0.0, 1.0, 1.0, 0.0)
    # seg.setLineColor(oofcanvas.Color(0.1, 0.1, 0.1))
    # seg.setLineWidth(.01)
    # layer.addItem(seg)

    # -------
    
    # layer = canvas.newLayer("circles")
    # layer.setClickable(True)

    # # Circles
    # circle = oofcanvas.CanvasCircle(0.25, 0.75, 0.2)
    # circle.setFillColor(oofcanvas.blue.opacity(0.5))
    # layer.addItem(circle)
    # circle = oofcanvas.CanvasCircle(0.5, 0.75, 0.2)
    # circle.setLineWidth(0.02)
    # circle.setFillColor(oofcanvas.green.opacity(0.5))
    # circle.setLineColor(oofcanvas.black)
    # layer.addItem(circle)
    # circle = oofcanvas.CanvasCircle(0.75, 0.75, 0.2)
    # circle.setLineWidth(0.02)
    # circle.setLineColor(oofcanvas.black)
    # layer.addItem(circle)

    # -------
    
    # layer = canvas.newLayer("ellipses")
    # layer.setClickable(True)

    # ellipse0 = oofcanvas.CanvasEllipse(0.5, 0.5, 0.25, 0.125, 30)
    # ellipse0.setLineWidth(0.03)
    # ellipse0.setLineColor(oofcanvas.black.opacity(0.9))
    # # ellipse0.setFillColor(oofcanvas.magenta.opacity(0.5))
    # layer.addItem(ellipse0)
    
    # ellipse1 = oofcanvas.CanvasEllipse(0.1, 0.1, 0.15, 0.05, 45)
    # ellipse1.setLineWidth(0.002)
    # ellipse1.setLineColor(oofcanvas.black.opacity(0.9))
    # ellipse1.setFillColor(oofcanvas.blue.opacity(0.5))
    # layer.addItem(ellipse1)

    # ellipse2 = oofcanvas.CanvasEllipse(0.7, 0.2, 0.15, 0.05, 0)
    # ellipse2.setLineWidth(0.002)
    # ellipse2.setLineColor(oofcanvas.black)
    # ellipse2.setFillColor(oofcanvas.blue.opacity(0.3))
    # layer.addItem(ellipse2)

    # layer = canvas.newLayer()
    # layer.setClickable(False)
    # for ellipse in (ellipse0,ellipse1,ellipse2):
    #     bb = ellipse.boundingBox();
    #     rect = oofcanvas.CanvasRectangle(bb.xmin(), bb.ymin(),
    #                                      bb.xmax(), bb.ymax())
    #     rect.setLineColor(oofcanvas.black)
    #     rect.setLineWidth(0.002)
    #     layer.addItem(rect)

    # -------

    # layer = canvas.newLayer("ellipses 2")

    # # A bunch of ellipses at regularly spaced angles, to check that
    # # the angles are correct.
    # for angle in range(0, 180, 10):
    #     ell = oofcanvas.CanvasEllipse(0.5, 0.5, 0.03, 0.3, angle)
    #     ell.setLineColor(oofcanvas.red)
    #     ell.setLineWidth(0.002)
    #     #ell.setFillColor(oofcanvas.gray.opacity(0.1))
    #     layer.addItem(ell)
    #     bb = ell.boundingBox()
    # circ = oofcanvas.CanvasCircle(0.5, 0.5, 0.3)
    # circ.setLineColor(oofcanvas.black)
    # circ.setLineWidth(0.003)
    # layer.addItem(circ)

    # -----

    # # Polygons

    # def regularpoly(poly, n, r, cx, cy, s=1):
    #     dangle = 2*math.pi/n
    #     for i in range(n):
    #         theta = ((i*s)%n)*dangle
    #         poly.addPoint(cx+r*math.cos(theta), cy+r*math.sin(theta))
    
    # layer = canvas.newLayer("polygons")
    # layer.setClickable(True)

    # poly = oofcanvas.CanvasPolygon()
    # poly.setLineWidth(0.01)
    # poly.setLineColor(oofcanvas.red)
    # regularpoly(poly, n=5, r=0.1, cx=0.2, cy=0.8)
    # layer.addItem(poly)
        
    # poly = oofcanvas.CanvasPolygon()
    # poly.setFillColor(oofcanvas.blue.opacity(0.2))
    # regularpoly(poly, n=6, r=0.1, cx=0.4, cy=0.8)
    # layer.addItem(poly)

    # poly = oofcanvas.CanvasPolygon()
    # layer.addItem(poly)
    # poly.setFillColor(oofcanvas.green.opacity(0.5))
    # poly.setLineColor(oofcanvas.green)
    # poly.setLineWidth(0.02)
    # regularpoly(poly, n=7, r=0.1, cx=0.6, cy=0.8)

    # poly = oofcanvas.CanvasPolygon()
    # layer.addItem(poly)
    # poly.setFillColor(oofcanvas.cyan.opacity(0.2))
    # poly.setLineColor(oofcanvas.cyan)
    # poly.setLineWidth(0.02)
    # poly.setLineJoin(oofcanvas.lineJoinRound);
    # #poly.setLineJoin(oofcanvas.lineJoinMiter);
    # #poly.setLineJoin(oofcanvas.lineJoinBevel);
    # regularpoly(poly, 7, r=0.1, cx=0.8, cy=0.8, s=2)

    # ------

    # Text
    layer = canvas.newLayer("text")
    layer.setClickable(False)
    
    text = oofcanvas.CanvasText(0.1, 0.1, "OOFCanvas!")
    #text.setFont("National Park Bold 10", True)
    #text.setFont("Phosphate Light 0.2")
    text.setFont(fontname, True) # True ==> size in pixels
    text.rotate(45)
    text.setFillColor(oofcanvas.red.opacity(0.9))
    text.drawBoundingBox(0.001, oofcanvas.black);
    layer.addItem(text)

    # text = oofcanvas.CanvasText(0.1, 0.5, "More text")
    # text.setFont("Times Bold 0.2", False)
    # text.setFillColor(oofcanvas.blue.opacity(0.5))
    # text.drawBoundingBox(0.001, oofcanvas.black)
    # layer.addItem(text)

    text = oofcanvas.CanvasText(0.0, 0.0, "Text")
    text.rotate(0)
    text.setFont("Times 30", True)
    text.drawBoundingBox(0.001, oofcanvas.black)
    layer.addItem(text)

    # -------

    # # Image from file
    
    layer = canvas.newLayer("Image")
    layer.setClickable(True)
    image = oofcanvas.CanvasImage.newFromImageMagickFile(-1.0, 0.0,
                                                         "testpattern.jpg",
                                                         100, -1)
    image.setPixelSize()
    image.setOpacity(1.0)
    image.drawBoundingBox(0.001, oofcanvas.black)
    layer.addItem(image)

    image = oofcanvas.CanvasImage.newFromPNGFile(-1.0, -1.0,
                                                 "testpattern.png",
                                                 1, -1)
    layer.addItem(image)

    # image = oofcanvas.CanvasImage.newBlankImage(0, 0,
    #                                        20, 20,
    #                                        1.2, 1.2,
    #                                        0.2, 0.2, 0.2, 1.0)
    # image.setOpacity(0.5)
    # layer.addItem(image)
    

    # dot = oofcanvas.CanvasDot(0.0, 0.0, 40)
    # dot.setFillColor(oofcanvas.red.opacity(0.5))
    # layer.addItem(dot)

    # -------
    
    # A lot of squares

    # layer = canvas.newLayer("squares")
    # layer.setClickable(True)
    # n = 100
    # dx = 1./(n+1)
    # w = dx/2.5;
    # for i in range(n):
    #     x = (i+1)*dx
    #     for j in range(n):
    #         y = (j+1)*dx
    #         rect = oofcanvas.CanvasRectangle(x-w, y-w, x+w, y+w)
    #         #rect.setLineWidth(dx/20)
    #         if (i+j)%2 == 0:
    #             rect.setFillColor(oofcanvas.black)
    #         else:
    #             rect.setFillColor(oofcanvas.white)
    #         #rect.setLineColor(oofcanvas.red)
    #         layer.addItem(rect)
            
            

    print "There are", len(canvas.allItems()), "canvas items."
    # for canvasitem in canvas.allItems():
    #     print canvasitem, "bbox=", canvasitem.boundingBox()
    
    canvas.draw()

#=-=##=-=##=-=##=-=##=-=##=-=##=-=##=-=##=-=##=-=##=-=##=-=##=-=##=-=##=-=##
##=-=##=-=##=-=##=-=##=-=##=-=##=-=##=-=##=-=##=-=##=-=##=-=##=-=##=-=##=-=#

fdialog = None

def fontButtonCB(button, parent, canvas):
    global defaultfont
    global fdialog
    if not fdialog:
        fdialog = Gtk.FontChooserDialog("Font Chooser", parent)
    dialog.set_font(defaultfont)
    result = fdialog.run()
    newfont = fdialog.get_font()
    fdialog.hide()
    if result in (Gtk.ResponseType.CANCEL,
                  Gtk.ResponseType.DELETE_EVENT,
                  Gtk.ResponseType.NONE):
        print "not changing the font", result
        return
    defaultfont = newfont
    ## TODO: This doesn't work because the font size is in points but
    ## the canvas is interpreting it in user units.
    canvas.clear()
    drawCB(None, canvas, newfont)

def fontButtonCB2(fontbutton, window, canvas):
    font = fontbutton.get_font()
    print "fontButtonCB2: font=", font
    canvas.clear()
    drawCB(None, canvas, font)

def reorderCB(button, canvas):
    #which = canvas.nLayers()-1
    which = 1
    # print "Moving layer", canvas.getLayer(which).name(), "down by 2"
    # canvas.getLayer(which).lowerBy(2)
    print "Moving layer", canvas.getLayer(which).name(), "up by 5"
    canvas.getLayer(which).raiseBy(5)
    # print "Lowering layer", canvas.getLayer(which).name(), "to bottom"
    # canvas.getLayer(which).lowerToBottom()
    print [canvas.getLayer(i).name() for i in range(canvas.nLayers())]

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

class StockButton(Gtk.Button):
    def __init__(self, icon_name, labelstr=None, reverse=False, markup=False,
                 align=None, **kwargs):
        Gtk.Button.__init__(self, **kwargs)
        image = Gtk.Image.new_from_icon_name(icon_name, Gtk.IconSize.BUTTON)
        hbox = Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL,
                       halign=Gtk.Align.CENTER)
        self.markup = markup
        self.reverse = reverse
        if reverse:
            if labelstr:
                if markup:
                    self.label = Gtk.Label()
                    self.label.set_markup(labelstr + ' ')
                else:
                    self.label = Gtk.Label(labelstr + ' ')
                if align is not None:
                    self.label.set_halign(Gtk.Align.CENTER)
                hbox.pack_start(self.label, expand=True, fill=True, padding=0)
            hbox.pack_start(image, expand=False, fill=False, padding=0)
        else:                       # not reverse
            hbox.pack_start(image, expand=False, fill=False, padding=0)
            if labelstr:
                if markup:
                    self.label = Gtk.Label()
                    self.label.set_markup(' ' + labelstr)
                else:
                    self.label = Gtk.Label(' ' + labelstr)
                if align is not None:
                    self.label.set_halign(Gtk.Align.CENTER)
                hbox.pack_start(self.label, expand=True, fill=True, padding=0)
        self.add(hbox)

## To print all icon names
# it = Gtk.IconTheme()
# print it.list_icons()

import fileselector
        
def launchFileChooser(button, (window, canvas)):
    dialog = Gtk.Dialog(title="Load Image",
                        flags=Gtk.DialogFlags.MODAL, parent=window)
    #dialog.set_keep_above(False) # this has no effect?
    #dialog.set_title("Load Image")

    okbutton = StockButton("gtk-ok", "OK")    
    dialog.add_action_widget(okbutton, Gtk.ResponseType.OK)

    cancelbutton = StockButton("gtk-cancel", "Cancel")
    dialog.add_action_widget(cancelbutton, Gtk.ResponseType.CANCEL)

    content = dialog.get_content_area()

    fs = fileselector.FileSelector()
    content.pack_start(fs.gtk, expand=True, fill=True, padding=3)

    grid = Gtk.Grid()
    content.pack_start(grid, expand=False, fill=False, padding=0)

    grid.attach(Gtk.Label("x", halign=Gtk.Align.END, margin_end=2), 0,0, 1,1)
    xentry = Gtk.Entry(halign=Gtk.Align.FILL)
    grid.attach(xentry, 1,0, 1,1)
    
    grid.attach(Gtk.Label("y", halign=Gtk.Align.END, margin_end=2), 0,1, 1,1)
    yentry = Gtk.Entry(halign=Gtk.Align.FILL)
    grid.attach(yentry, 1,1, 1,1)
    
    grid.attach(Gtk.Label("width", halign=Gtk.Align.END, margin_end=2),
                0,2, 1,1)
    wentry = Gtk.Entry(halign=Gtk.Align.FILL)
    grid.attach(wentry, 1,2, 1,1)

    grid.attach(Gtk.Label("pixels", halign=Gtk.Align.END, margin_end=2),
                2,2, 1,1)
    pixbutton = Gtk.CheckButton()
    grid.attach(pixbutton, 3,2, 1,1)
    

    fs.show()
    content.show_all()

    response = dialog.run()

    if response != Gtk.ResponseType.OK:
        dialog.close()
        return
    filename = fs.get_value()
    x = float(xentry.get_text())
    y = float(yentry.get_text())
    w = float(wentry.get_text())
    dialog.close()
    print "file selector returned '%s', x=%s, y=%s, w=%s" % (filename, x, y, w)

    layer = canvas.newLayer("Image")
    #image = oofcanvas.CanvasPNGImage(filename, x, y, w, -1)
    image = oofcanvas.CanvasImage.newFromImageMagickFile(x, y, filename, w, -1)
    if pixbutton.get_active():
        image.setPixelSize()
    layer.addItem(image)
    canvas.draw()
    

        
        
def zoom(button, canvas, factor):
    canvas.zoom(factor)

def clear(button, canvas):
    canvas.clear()

def fill(button, canvas):
    canvas.zoomToFill()

def center(button, canvas):
    canvas.center()

def entryCB(entry):
    print entry.get_text()

def antialiasCB(button, canvas):
    canvas.antialias(button.get_active())
        

def delete_event(window, event, canvas):
    quit(canvas)

def quit(canvas):
    canvas.destroy()
    Gtk.main_quit()

def mousefunc(eventname, x, y, button, shift, ctrl, canvas):
    print "mouse:", eventname, x, y, button, "shift=%d"%shift, "ctrl=%d"%ctrl
    if button == 1:
        if eventname == "down":
            canvas.allowMotionEvents(True)
        if eventname == "up":
            canvas.allowMotionEvents(False)
            if shift:
                canvas.zoomAbout(x, y, ZOOM)
            elif ctrl:
                canvas.zoomAbout(x, y, 1./ZOOM)
            else:
                items = canvas.clickedItems(x, y)
                print "Clicked on", len(items), "items:"
                for item in items:
                    print "  ", item

# def resizefunc(data):
#     print "resize: data=", data

def menuCB(menuitem, data):
    print "menuCB:", data
    if menuitem.get_label() == 'Quit':
        Gtk.main_quit()
    if menuitem.get_label() == "A":
        dialog = Gtk.Dialog(flags=Gtk.DialogFlags.MODAL)
        dialog.set_title("A modal dialog")
        content = dialog.get_content_area()
        content.pack_start(Gtk.Label('OK?'),
                           expand=False, fill=False, padding=0)
        okbutton = StockButton("gtk-ok")
        dialog.add_action_widget(okbutton, Gtk.ResponseType.OK)
        dialog.show_all()
        dialog.run()            # ignore return value
        dialog.close()

def checkMenuCB(menuitem):
    print "checkMenuCB: ", menuitem.get_active()

def radioCB(menuitem, word):
    print "radioCB:", word, menuitem.get_active()

def newChooserCB(name):
    print "newChooserCB: ", name
    
def popupCB(menuitem, data):
    obj = data[0]
    word = data[1]
    obj.set_text(word)

def buttonCB(obj, event, anchorwidget):
    popupMenu = Gtk.Menu()
    for word in ("hello", "good-bye", "farewell"):
        last = menuitem = Gtk.MenuItem(word)
        popupMenu.append(menuitem)
        menuitem.connect('activate', popupCB, (obj, word))
        menuitem.set_tooltip_text("Tooltip on a popup menu item")
    popupMenu.show_all()
    popupMenu.popup_at_widget(anchorwidget, Gdk.Gravity.SOUTH_WEST,
                              Gdk.Gravity.NORTH_WEST, event)
    # popupMenu.popup_at_pointer(event)
    return False

def sliderEntryCB(entry, adjust):
    print "sliderEntryCB"
    try:
        v = eval(entry.get_text())
    except:
        return
    # Need to cooy signal blocking code from labelledslider.py
    if v < adjust.get_lower():
        v = adjust.get_lower()
    if v > adjust.get_upper():
        v = adjust.get_upper()
    adjust.set_value(v)

def sliderAdjCB(slider, (entry, signal)):
    print "sliderAdjCB"
    entry.handler_block(signal)
    val = slider.get_value()
    entry.set_text(`val`)
    entry.handler_unblock(signal)

flashstate = False
def flashCB(flasherbox):
    global flashstate
    flasherbox.remove(flasherbox.get_child())
    if flashstate:
        icon = 'gtk-no'
    else:
        icon = 'gtk-yes'
    flashstate = not flashstate
    flasherbox.add(Gtk.Image.new_from_icon_name(icon, Gtk.IconSize.BUTTON))
    flasherbox.show_all()
    return True

def stopFlashing(button, timeout):
    GObject.source_remove(timeout)

#-----------

textWindow = None
paragraphs = 0

lorem = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum."

def addTextCB(button, textView):
    global paragraphs, endmark
    paragraphs += 1
    bfr = textView.get_buffer()
    bfr.insert(bfr.get_end_iter(), `paragraphs` + ': ' + lorem + '\n')
    textView.scroll_mark_onscreen(endmark)

def clearTextCB(button, textView):
    bfr = textView.get_buffer()
    bfr.set_text("")

def hideTextCB(button, textWindow):
    textWindow.hide()

def changeTextSizeCB(entry, textView):
    try:
        size = int(entry.get_text())
    except:
        return
    provider = Gtk.CssProvider()
    provider.load_from_data(
        "textview { font: %dpx monospace; }" % size)
    sc = textView.get_style_context()
    sc.add_provider(provider, Gtk.STYLE_PROVIDER_PRIORITY_APPLICATION)

def deleteTextWindowCB(window, event):
    global textWindow
    textWindow = None

def showTextCB(button):
    global textWindow
    if textWindow:
        textWindow.present_with_time(Gtk.get_current_event_time())
        return
    textWindow = Gtk.Window(Gtk.WindowType.TOPLEVEL, title="Text")
    vbox = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=5,
                   margin=5)
    textWindow.add(vbox)
    textWindow.connect("delete-event", deleteTextWindowCB)

    scrollWindow = Gtk.ScrolledWindow(border_width=0, margin=0)
    vbox.pack_start(scrollWindow, expand=True, fill=True, padding=0)
    textView = Gtk.TextView(editable=False, cursor_visible=False,
                            wrap_mode=Gtk.WrapMode.WORD)
    # textView.set_wrap_mode(Gtk.WrapMode.WORD)
    scrollWindow.add(textView)


    ## Suggestion for using pango fonts, from
    ## https://stackoverflow.com/questions/41257101/setting-gtkentry-font-from-pango-fontdescription
    ## This isn't doing anything...
    # pc = textView.get_pango_context()
    # fd = pc.get_font_description()
    # fd.set_family("Monospace")
    # fd.set_absolute_size(20*Pango.SCALE)
    # pc.set_font_description(fd)

    ## This overrides settings done globally with
    ## add_provider_for_screen in run().
    # sc = textView.get_style_context()
    # provider = Gtk.CssProvider()
    # provider.load_from_data(
    #     # font names: monospace serif sans
    #     """
    #     textview {
    #       font: 5px "monospace";
    #     }
    #     textview text {
    #       color: red
    #     }
    #     """)
    # sc.add_provider(provider, Gtk.STYLE_PROVIDER_PRIORITY_APPLICATION)

    global endmark, midmark
    bfr = textView.get_buffer()
    enditer = bfr.get_end_iter()
    endmark = bfr.create_mark(None, enditer, False)
    midmark = bfr.create_mark(None, enditer, False)
    
    hbox = Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL, spacing=2)
    vbox.pack_start(hbox, expand=False, fill=False, padding=0)

    button = Gtk.Button.new_with_mnemonic("_Add")
    hbox.pack_start(button, expand=False, fill=False, padding=0)
    button.connect("clicked", addTextCB, textView)

    button = Gtk.Button("Clear")
    hbox.pack_start(button, expand=False, fill=False, padding=0)
    button.connect("clicked", clearTextCB, textView)
    
    button = Gtk.Button("Hide")
    hbox.pack_start(button, expand=False, fill=False, padding=0)
    button.connect("clicked", hideTextCB, textWindow)

    entry = Gtk.Entry()
    hbox.pack_start(entry, expand=True, fill=True, padding=0)
    entry.connect("activate", changeTextSizeCB, textView)

    textWindow.show_all()

#----------

# def drawingAreaCB(drawingarea, context):
#     # Simple drawing in a Gtk.DrawingArea, for a Color chooser widget
#     width = drawingarea.get_allocated_width()
#     height = drawingarea.get_allocated_height()
#     context.move_to(0, 0)
#     context.line_to(width/2., 0)
#     context.line_to(width/2., height)
#     context.line_to(0, height)
#     context.close_path()
#     context.set_source_rgb(0.0, 1.0, 0.0)
#     context.fill()
#     context.move_to(width/2., 0)
#     context.line_to(width, 0)
#     context.line_to(width, height)
#     context.line_to(width/2., height)
#     context.close_path()
#     context.set_source_rgb(1.0, 0.0, 0.0)
#     context.fill()
#     return False

def run():

    settings = Gtk.Settings.get_default()
    #settings.set_property("gtk-theme-name", "Default")
    #settings.set_property("gtk-font-name", ".AppleSystemUIFont 15")
    settings.set_property("gtk-font-name", "Skia 15px")
    oofcanvas.initializePyGTK()
    
    window = Gtk.Window(Gtk.WindowType.TOPLEVEL, title="Sandbox")
#    window.set_title("Sandbox")
    vbox0 = Gtk.Box(orientation=Gtk.Orientation.VERTICAL)
    window.add(vbox0)
    accelgrp = Gtk.AccelGroup()
    window.add_accel_group(accelgrp)

    sc = window.get_style_context()
    provider = Gtk.CssProvider()
    provider.load_from_data(
        """
/* * { font: 12px sans; } */
textview { font: 15px monospace; }
/* textview text { color: blue; } */
  #red { color: red; }
""")

    # Priorities are
    # STYLE_PROVIDER_PRIORITY_FALLBACK      1
    # STYLE_PROVIDER_PRIORITY_THEME       200
    # STYLE_PROVIDER_PRIORITY_SETTINGS    400
    # STYLE_PROVIDER_PRIORITY_APPLICATION 600
    # STYLE_PROVIDER_PRIORITY_USER        800
    # Higher priority values override lower priority values.
    sc.add_provider_for_screen(window.get_screen(),
                               provider,
                               Gtk.STYLE_PROVIDER_PRIORITY_APPLICATION)

    # for p in dir(Gtk):
    #     print p

    bar = Gtk.MenuBar()
    vbox0.pack_start(bar, expand=False, fill=False, padding=2)
    for item in ("File", "Edit", "Help"):
        menuitem = Gtk.MenuItem(item)
        bar.append(menuitem)
        submenu = Gtk.Menu()
        menuitem.set_submenu(submenu)
        if item == "Help":
            menuitem.set_right_justified(True)
        for m, subitem in enumerate(("A", "B", "C", "Quit")):
            if subitem == 'C':
                submenuitem = Gtk.CheckMenuItem(subitem)
                submenuitem.connect('activate', checkMenuCB)
            else:
                submenuitem = Gtk.MenuItem(subitem)
                data = 5
                submenuitem.connect('activate', menuCB, data)
            submenu.insert(submenuitem, m)
            if item == "File" and subitem == "Quit":
                submenuitem.add_accelerator('activate', accelgrp,
                                            ord('q'),
                                            Gdk.ModifierType.CONTROL_MASK,
                                            Gtk.AccelFlags.VISIBLE)

    menuitem = Gtk.MenuItem("Radio")
    bar.append(menuitem)
    submenu = Gtk.Menu()
    menuitem.set_submenu(submenu)
    group = None
    for subitem in ("AM", "FM", "Ham"):
        submenuitem = Gtk.RadioMenuItem(subitem)
        # First submenuitem joining group=None makes that item active
        submenuitem.join_group(group)
        if group is None:
            group = submenuitem
        submenuitem.connect("activate", radioCB, subitem)
        submenu.append(submenuitem)


    paned = Gtk.Paned(orientation=Gtk.Orientation.HORIZONTAL)
    vbox0.pack_start(paned, expand=True, fill=True, padding=0)
    

    canvas = oofcanvas.Canvas(width=300, height=300, ppu=200,
                              vexpand=True, hexpand=True)
    
    canvas.setBackgroundColor(0.9, 0.9, 0.9)
    canvas.setMouseCallback(mousefunc, canvas)
    #canvas.setResizeCallback(resizefunc, None)
    canvas.show()

    window.connect("delete-event", delete_event, canvas)
    
    frame = Gtk.Frame(label="Canvas")
    paned.pack1(frame, resize=True, shrink=False)
    frame.set_shadow_type(Gtk.ShadowType.IN)

    ## Put the canvas and its scrollbars in a Grid.

    canvasTable = Gtk.Grid()
    canvasTable.set_column_spacing(5)
    frame.add(canvasTable)

    frame2 = Gtk.Frame()
    frame2.set_shadow_type(Gtk.ShadowType.IN)
    canvasTable.attach(frame2, 0, 0, 1, 1)
    frame2.add(canvas.layout)

    hScrollbar = Gtk.Scrollbar.new(adjustment=canvas.get_hadjustment(),
                               orientation=Gtk.Orientation.HORIZONTAL)
    canvasTable.attach(hScrollbar, 0, 2, 1, 1)
    vScrollbar = Gtk.Scrollbar.new(adjustment=canvas.get_vadjustment(),
                                   orientation=Gtk.Orientation.VERTICAL)
    #vScrollbar.set_hexpand(True)
    # vScrollbar.set_halign(Gtk.Align.START)
    canvasTable.attach(vScrollbar, 1, 0, 1, 1)
    
    # ## Using a ScrolledWindow instead of a Grid is easier, but the
    # ## scrollbars are drawn inside the window, overlapping the Canvas,
    # ## which might not be optimal.
    # swind = Gtk.ScrolledWindow(canvas.get_hadjustment(),
    #                            canvas.get_vadjustment());
    # swind.set_min_content_height(200)
    # swind.add(canvas.layout)
    # frame.add(swind)

    vbox = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=2,
                   margin_end=2, margin_start=2)
    paned.pack2(vbox, resize=True, shrink=True)

    hbox = Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL)
    vbox.pack_start(hbox, expand=False, fill=False, padding=3)

    button = Gtk.Button("Choose File", name="red")
    hbox.pack_start(button, True, True, 3)
    button.connect("clicked", launchFileChooser, (window, canvas))

    button = Gtk.Button("Draw", name="red")
    hbox.pack_start(button, True, True, 3)
    button.connect("clicked", drawCB, canvas);

    button = Gtk.Button("Reorder")
    hbox.pack_start(button, True, True, 3)
    button.connect("clicked", reorderCB, canvas)

    button = Gtk.Button("Show/Hide")
    hbox.pack_start(button, True, True, 3)
    button.connect("clicked", showhideCB, canvas)

    aabutton = Gtk.CheckButton("AA", halign=Gtk.Align.CENTER, hexpand=False)
    aabutton.set_active(True)
    hbox.pack_start(aabutton, True, True, 3)
    aabutton.connect("clicked", antialiasCB, canvas)
    # aabutton.set_halign(Gtk.Align.CENTER)
    # aabutton.set_hexpand(False)

    button = Gtk.ToggleButton("toggle")
    button.set_mode(False)       # What does this do?
    hbox.pack_start(button, True, True, 3)
    button.set_halign(Gtk.Align.CENTER)

    hsep = Gtk.Separator(orientation=Gtk.Orientation.HORIZONTAL,
                         halign=Gtk.Align.FILL,
                         valign=Gtk.Align.CENTER,
                         hexpand=True,
                         vexpand=False)
    # hsep.set_halign(Gtk.Align.FILL)
    # hsep.set_valign(Gtk.Align.CENTER)
    # hsep.set_hexpand(True)
    # hsep.set_vexpand(False)
    vbox.pack_start(hsep, expand=False, fill=False, padding=3)

    hbox = Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL,
                   homogeneous=True, spacing=10,
                   border_width=10)
    hbox.set_halign(Gtk.Align.CENTER)
    vbox.pack_start(hbox, False, False, 3)

    button = Gtk.Button.new_from_icon_name("zoom-in-symbolic",
                                           Gtk.IconSize.BUTTON)
    hbox.pack_start(button, True, True, 3)
    button.connect("clicked", zoom, canvas, ZOOM)

    # new_with_mnemonic makes option-F/alt-F a keyboard accelerator,
    # but for some reason it's not indicated as such on the button.
    button = Gtk.Button.new_with_mnemonic("_Fill")
    hbox.pack_start(button, True, True, 3)
    button.connect("clicked", fill, canvas)

    button = Gtk.Button("Center", border_width=3)
    hbox.pack_start(button, True, True, 3)
    button.connect("clicked", center, canvas)

    button = StockButton("edit-clear")
    hbox.pack_start(button, True, True, 3)
    button.connect("clicked", clear, canvas)
        
    button = Gtk.Button.new_from_icon_name("zoom-out-symbolic",
                                           Gtk.IconSize.BUTTON)
    hbox.pack_start(button, True, True, 3)
    button.connect("clicked", zoom, canvas, 1./ZOOM)

    hbox = Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL)
    vbox.pack_start(hbox, False, False, 3)

    flasherbox = Gtk.Frame(border_width=5)
    #flasherbox.set_border_width(100)
    flasherbox.set_shadow_type(Gtk.ShadowType.IN)
    hbox.pack_start(flasherbox, expand=False, fill=False, padding=0)
    flasherbox.add(Gtk.Image.new_from_icon_name('gtk-yes', Gtk.IconSize.BUTTON))
    timeout = GObject.timeout_add(1000, flashCB, flasherbox)
    button = StockButton("process-stop-symbolic")
    button.connect("clicked", stopFlashing, timeout)
    hbox.pack_start(button, expand=False, fill=False, padding=0)
    button.set_tooltip_text("Stop flashing")

    # Get a pull-down menu on a label by putting it in an event box

    newchooserwidget = chooser.ChooserWidget(
        ("earth", "wind", "------", "fire"),
        callback=newChooserCB,
        separator_func=lambda x: x=="------",
        helpdict={'earth':'here', 'wind':'there', 'fire':'everywhere'}
    )
    hbox.pack_start(newchooserwidget.gtk, True, True, 4)
    
    # frame = Gtk.Frame()
    # eventbox = Gtk.EventBox()
    # hbox.pack_start(eventbox, False, False, 3)
    # eventbox.add(frame)
    # labelbox = Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL)
    # frame.add(labelbox)
    # label = Gtk.Label("Menu Button", halign=Gtk.Align.START, hexpand=False,
    #                   margin_start=5, margin_end=5)
    # labelbox.pack_start(label, expand=True, fill=True, padding=2)
    # img =  Gtk.Image.new_from_icon_name('pan-down-symbolic',
    #                                     Gtk.IconSize.BUTTON)
    # labelbox.pack_start(img, expand=False, fill=False, padding=2)
    # eventbox.connect("button-press-event", buttonCB, label) 
    # label.set_tooltip_text(
    #     "This is a tooltip on a Label that\nis masquerading as a ComboWidget")
    
    entry = Gtk.Entry(hexpand=False, halign=Gtk.Align.END, name="red",
                      width_chars=10)
    hbox.pack_start(entry, True, True, 3)
    entry.set_width_chars(40)
    entry.connect('changed', entryCB)
    entry.connect("button-press-event", buttonCB, entry)
    entry.set_tooltip_markup("This <b>entry</b> has a pop-up menu.")

    hbox = Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL)
    vbox.pack_start(hbox, False, False, 3)
    label = Gtk.Label("Slider=")
    label.set_halign(Gtk.Align.END)
    hbox.pack_start(label, False, False, 0)
    paned = Gtk.Paned(orientation=Gtk.Orientation.HORIZONTAL)
    hbox.pack_start(paned, True, True, 0)
    adjust = Gtk.Adjustment(value=0, lower=0, upper=100,
                            step_incr=10, # arrow keys move this far
                            page_incr=20, # page up/down move this far
                            page_size=0)  # max is upper-page_size
    slider = Gtk.Scale(orientation=Gtk.Orientation.HORIZONTAL,
                       adjustment=adjust)
    slider.set_size_request(100, -1)
    slider.set_draw_value(False) 
    #slider.set_has_origin(False)
    paned.pack1(slider, resize=True, shrink=False)
    entry = Gtk.Entry()
    entry.set_margin_start(3)
    entry.set_margin_end(3)
    paned.pack2(entry, resize=True, shrink=True)
    entry.set_width_chars(10)
    entrysignal = entry.connect('changed', sliderEntryCB, adjust)
    adjust.connect('value-changed', sliderAdjCB, (entry, entrysignal))

    # Chooser variants
    if 1:
        hbox = Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL, spacing=2,
                       halign=Gtk.Align.CENTER)
        vbox.pack_start(hbox, expand=False, fill=False, padding=3)

        def cwCB(*args, **kwargs):
            # print "cwCB:", args, kwargs
            pass
        hbox.pack_start(Gtk.Label("ChooserWidget"), expand=False, fill=False,
                        padding=2)
        cw = chooser.ChooserWidget(["here", "there", "everywhere",
                                    "x"*100],
                                   callback=cwCB)
        hbox.pack_start(cw.gtk, expand=False, fill=False, padding=5)

        def sepfunc(model, iterator):
            return model[iterator][0] == "-----"

        hbox.pack_start(Gtk.Label("ChooserListWidget"), expand=False, fill=False,
                                  padding=2)
        clw = chooser.ChooserListWidget(['alpha', 'beta', '-----', 'gamma'],
                                        callback=cwCB,
                                        separator_func=sepfunc)
        hbox.pack_start(clw.gtk, expand=True, fill=True, padding=5)


        hbox = Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL)
        vbox.pack_start(hbox, expand=False, fill=False, padding=3)

        hbox.pack_start(Gtk.Label("ChooserComboWidget"), expand=False, fill=False,
                                  padding=2)
        ccw = chooser.ChooserComboWidget(["hickory", "dickory", "dock"],
                                          callback=cwCB)
        hbox.pack_start(ccw.gtk, expand=True, fill=True, padding=5)

        hbox.pack_start(Gtk.Label("MultiListWidget"), expand=False, fill=False,
                        padding=2)
        mlw = chooser.MultiListWidget(["true", "false", "file not found"],
                                      callback=cwCB)
        hbox.pack_start(mlw.gtk, expand=True, fill=True, padding=5)

        hbox = Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL)
        vbox.pack_start(hbox, expand=False, fill=False, padding=5)

        hbox.pack_start(Gtk.Label('FramedCLW'), expand=False,
                        fill=False, padding=2)
        fcw = chooser.FramedChooserListWidget(["this", "that", "the other"],
                                              callback=cwCB)
        hbox.pack_start(fcw.gtk, expand=True, fill=True, padding=5)

        hbox.pack_start(Gtk.Label("ScrolledCLW"), expand=False, fill=False,
                        padding=2)
        sclw = chooser.ScrolledChooserListWidget([`i` for i in range(100)],
                                                 callback=cwCB)
        hbox.pack_start(sclw.gtk, expand=True, fill=True, padding=5)

        hbox.pack_start(Gtk.Label("SMLW"), expand=False, fill=False, padding=2)
        smlw = chooser.ScrolledMultiListWidget([`i*i` for i in range(100)],
                                               callback=cwCB)
        hbox.pack_start(smlw.gtk, expand=True, fill=True, padding=5)

    #
    # hbox = Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL)
    # frame = Gtk.Frame()
    # frame.set_shadow_type(Gtk.ShadowType.IN)
    # hbox.pack_start(frame, expand=False, fill=False, padding=2)
    # drawarea = Gtk.DrawingArea()
    # frame.add(drawarea)
    # drawarea.set_size_request(100, 100)
    # drawarea.connect("draw", drawingAreaCB)
    # vbox.pack_start(hbox, expand=False, fill=False, padding=2)

    hbox = Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL, spacing=2,
                   halign=Gtk.Align.END, margin_end=3)
    vbox.pack_start(hbox, expand=False, fill=False, padding=0)
    label = Gtk.Label("This is an END-aligned row")
    hbox.pack_start(label, expand=False, fill=False, padding=0)
    button = Gtk.FontButton("Change Font")
    button.set_font(defaultfont)
    hbox.pack_start(button, expand=False, fill=False, padding=0)
    button.set_use_font(True)
    button.connect("font-set", fontButtonCB2, window, canvas)

    button = Gtk.Button.new_with_mnemonic("Show _Text")
    button.connect("clicked", showTextCB)
    hbox.pack_start(button, expand=False, fill=False, padding=0)
    # button = Gtk.Button("Change Font")
    # hbox.pack_start(button, expand=False, fill=False, padding=0)
    # button.connect("clicked", fontButtonCB, window, canvas)

    window.show_all()
    window.present()
    canvas.antialias(aabutton.get_active())
    drawCB(None, canvas)

    print "Original layers: ", [canvas.getLayer(i).name()
                                for i in range(canvas.nLayers())]

    Gtk.main()

from gi.repository import Pango

if __name__ == "__main__":
    run()
