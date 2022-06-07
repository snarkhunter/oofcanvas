// -*- C++ -*-

// A simple Cairo/Pango/Gtk3 program that demonstrates problems with
// Pango, I think.

// PPU is the pixels-per-unit used to define the Cairo scaling matrix.
#define PPU 10.

// FUDGE changes the pixels-per-unit, but also changes all lengths to
// compensate.  Changing FUDGE should not change the display, in
// principle.  But with PPU=10 and FUDGE=1, the kerning is incorrect
// on small type.  With FUDGE=10 it's better.
#define FUDGE 1.

// The problem only occurs on proportionally spaced fonts.  Change
// FONTNAME to "Courier" instead of "Times" and the display is correct
// with FUDGE=1.
#define FONTNAME "Times"

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

#include <pango/pango.h>
#include <pango/pangocairo.h>
#include <cairomm/cairomm.h>
#include <gtk/gtk.h>
#include <string>
#include <iostream>

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

// Scroll offsets
int offset_x = 0;
int offset_y = 0;

// Draw text at a given size and position.

void textAt(const char *txt, double size, double x, double y,
	    Cairo::RefPtr<Cairo::Context> context)
{
  PangoLayout *lo = pango_cairo_create_layout(context->cobj());
  pango_layout_set_text(lo, txt, -1);
  std::string fd =  std::string(FONTNAME) + " " + std::to_string(size*FUDGE);
  PangoFontDescription *pfd = pango_font_description_from_string(fd.c_str());
  pango_layout_set_font_description(lo, pfd);
  pango_font_description_free(pfd);

  context->save();
  context->move_to(x*FUDGE, y*FUDGE);
  pango_layout_context_changed(lo);
  pango_cairo_show_layout(context->cobj(), lo);
  g_object_unref(lo);
  context->restore();
}

// Draw event handler

bool drawCB(GtkWidget *layout, Cairo::Context::cobject *ctxt, gpointer) {
  Cairo::RefPtr<Cairo::Context> context(new Cairo::Context(ctxt, false));

  guint w,h;
  gtk_layout_get_size(GTK_LAYOUT(layout), &w, &h);
  
  double x1, y1, x2, y2;
  context->get_clip_extents(x1, y1, x2, y2);
  gtk_layout_set_size(GTK_LAYOUT(layout), x2-x1, y2-y1);

  Cairo::Matrix transf(PPU/FUDGE, 0., 0., PPU/FUDGE, offset_x, offset_y);
  context->set_matrix(transf);

  // Draw a 10x10 square for reference.
  context->save();
  context->move_to(0,0);
  context->line_to(10*FUDGE, 0);
  context->line_to(10*FUDGE, 10*FUDGE);
  context->line_to(0, 10*FUDGE);
  context->close_path();
  context->set_source_rgb(1,0,0);
  context->set_line_width(FUDGE/(PPU));
  context->stroke();
  context->restore();

  textAt("1: hello", 1, 1, 1, context);
  textAt("2: hello", 2, 1, 2, context);
  textAt("5: hello", 5, 1, 5, context);
  textAt("10: hello", 10, 1, 10, context);
  textAt("15: hello", 15, 1, 20, context);
  return false;
}

bool scrollCB(GtkWidget *layout, GdkEventScroll *event, gpointer)
{
  if(event->direction == GDK_SCROLL_SMOOTH) {
    offset_x -= event->delta_x;
    offset_y -= event->delta_y;
    gtk_widget_queue_draw(layout);
  }
  return false;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//


int main(int argc, char *argv[]) {
  std::cout << "Using Pango " << PANGO_VERSION_MAJOR << "."
	    << PANGO_VERSION_MINOR << "."
	    << PANGO_VERSION_MICRO << std::endl;
  gtk_init(&argc, &argv);
  GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  g_signal_connect(window, "delete-event", gtk_main_quit, nullptr);

  GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
  gtk_container_add(GTK_CONTAINER(window), vbox);

  GtkWidget *frame = gtk_frame_new(nullptr) ;
  gtk_box_pack_start(GTK_BOX(vbox), frame, true, true, 2);
  gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_IN);

  GtkWidget *swind = gtk_scrolled_window_new(nullptr, nullptr);
  gtk_container_add(GTK_CONTAINER(frame), swind);
  gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(swind),
				      GTK_SHADOW_ETCHED_IN);
								     
  GtkWidget *layout = gtk_layout_new(nullptr, nullptr);
  gtk_container_add(GTK_CONTAINER(swind), layout);
  gtk_widget_add_events(layout, (GdkEventMask) GDK_SCROLL_MASK);
  g_signal_connect(G_OBJECT(layout), "scroll_event",
		   G_CALLBACK(scrollCB), nullptr);
  g_signal_connect(G_OBJECT(layout), "draw",
		   G_CALLBACK(drawCB), nullptr);

  GtkWidget *button = gtk_button_new_with_label("Quit");
  g_signal_connect(button, "clicked", gtk_main_quit, nullptr);
  gtk_box_pack_start(GTK_BOX(vbox), button, false, false, 2);

  gtk_widget_show_all(window);
  gtk_window_present_with_time(GTK_WINDOW(window), 0);

  gtk_main();
}
