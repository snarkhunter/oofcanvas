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

This is _not_ meant to be a gnome library or a plug-in replacement for
goocanvas.

## Class overview

Everything is in namespace OOFCanvas.

* Canvas
  * can be constructed from a GtkWidget (usually a drawing_area),
    copying its size and other properties.
  * has pixel coordinate system and user coordinate system
  * calls a callback function when the mouse is clicked
  * can be resized (size in pixels changes)
  * can be zoomed (pixel size stays the same, user coordinates change)
  * can be shifted (offset of user coordinates changes)
  * can be told to update whole canvas or given rectangle in pixel units 
  * can be saved as pdf or other image file formats
  * can create new CanvasLayers
  * can reorder layers
  * can delete layers
  * can display axes (in user coordinates?  pixel coordinates?)
  * stores visible region in pixel coordinates

* CanvasItem
  * base class for things that can be drawn
  * computes bounding box in user and pixel coordinates
  * has a draw(Cairo::RefPtr<Cairo::Context>) method
  * subclasses must provide drawItem(Cairo::RefPtr<Cairo::Context>)

* CanvasLayer
  * subclass of CanvasItem
  * groups other items together (could be other CanvasLayers)
  * contains lists of other CanvasItems and transparency of each
  * raise/lower by N layers
  * raise to top, lower to bottom
  * delete

* CanvasTransformable
  * subclass of CanvasItem
  * stores transformation matrix and offset vector
  * transforms and offsets can be applied, and accumulate
  * transformation matrix can be given as
     * matrix
     * rotation angle
     * scale factor
  * transformation matrix applies to previously defined offset as well
    as previously defined matrix
  * can be flagged as fixedSize, in which case it's always drawn at
    the same pixel size, no matter what transformations have been
    applied or if the canvas has been zoomed.

* CanvasShape
  * subclass of CanvasTransformable
  * filled? fillColor, fillAlpha, (fillGradient?)
  * outline? lineColor, lineAlpha, lineWidth
  * specified by some set of positions and sizes

* CanvasPolygon
  * subclass of CanvasShape
  * sequence of points

* CanvasEllipse
  * subclass of CanvasShape

* CanvasCircle
  * subclass of CanvasEllipse

* CanvasSegment
  * subclass of CanvasTransformable
  * lineColor, lineAlpha, lineWidth
  * pattern

* CanvasSegments
  * subclass of CanvasTransformable
  * sequence of points
  * like CanvasPolygon, but not a CanvasShape because it can't be filled

* CanvasText
  * subclass of CanvasTransformable (CanvasShape?)
  * string, font, size, etc.

* CanvasImage
  * subclass of CanvasTransformable