# OOFCanvas

A replacement for libgnomecanvas, designed for use in OOF2, but
probably useful elsewhere. OOFCanvas will be based on cairo and
compatible with gtk+2 and gtk+3.

OOF2 uses libgnomecanvas (https://developer.gnome.org/libgnomecanvas/)
to display and interact with images and meshes.  But lignomecanvas
requires gtk+2, and gtk+2 works only with python3, not python2, and
python2 is being phased out.  In order to upgrade OOF2 to python3, we
need to first upgrade it from gtk+2 to gtk+3, and to do that we need
to first replace libgnomecanvas.

The most mature replacement for libgnomecanvas seems to be goocanvas,
(https://github.com/GNOME/goocanvas) but according to its website,
goocanvas is no longer being developed and is barely being maintained,
so it's not an attractive option.

The backend used by gtk+3 (and gtk+2) is cairo
(https://developer.gnome.org/cairo/).  OOF2 could use cairo directly,
but it doesn't support some useful features of libgnomecanvas.
OOFCanvas is an attempt to write an OOF2-friendly canvas that uses
cairo under the hood and is compatible with gtk+2 and gtk+3.  It's
being developed in its own git repository to make it easier to test
and easier to distribute independently from OOF2.

--------

* OOFCanvas

   * can be constructed from a GtkWidget (usually a drawing_area),
     copying its size and other properties.
   * has pixel coordinate system and user coordinate system
   * calls a callback function when the mouse is clicked
   * can be resized (size in pixels changes)
   * can be rescaled (pixel size stays the same, user coordinates change)
   * can be shifted (offset of user coordinates changes)
   * can be told to update
   * can be saved as pdf or other image file formats
   * can create new OOFCanvasLayers
   * can reorder layers
   * can delete layers
   * can display axes in user coordinates

* OOFCanvasItem
   - base class for things that can be drawn

* OOFCanvasLayer
   - subclass of OOFCanvasItem
   - groups other items together (could be other OOFCanvasLayers)
   - contains lists of other OOFCanvasItems and transparency of each

* OOFCanvasShape
   - subclass of OOFCanvasItem
   - filled? fillColor, fillAlpha, (fillGradient?)
   - outline? lineColor, lineAlpha
   - coordinates of points given in user coordinates
   -  given in either user or pixel coordinates

* OOFCanvasPolygon
   - subclass of OOFCanvasShape

* OOFCanvasCircle
   - subclass of OOFCanvasShape

* OOFCanvasSegment
   - subclass of OOFCanvasItem
   -