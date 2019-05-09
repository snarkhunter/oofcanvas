// -*- C++ -*-

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modified
 * versions of this software, you first contact the authors at
 * oof_manager@nist.gov. 
 */

#include "canvas.h"
#include <cairo.h>
#include <gdk/gdk.h>


namespace OOFCanvas {

  Canvas::Canvas(GtkWidget *hostwidget,
		 int pixelwidth,
		 int pixelheight,
		 double width,
		 double height)
    : pixelwidth(pixelwidth),
      pixelheight(pixelheight),
      width(width),
      height(height)
  {
    cairo_surface_t *surf = gdk_window_create_similar_surface(
				     hostwidget->window, // GdkWindow*
				     CAIRO_CONTENT_COLOR,
				     pixelwidth, pixelheight);
    // TODO: the second arg here is has_reference.  What should it be?
    root = Cairo::RefPtr<Cairo::Surface>(new Cairo::Surface(surf, false));
  } 
};				// namespace OOFCanvas

