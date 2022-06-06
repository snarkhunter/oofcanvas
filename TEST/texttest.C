// -*- C++ -*-

// Just a demo program for drawing text on a GtkLayout.  This doesn't
// use OOFCanvas.

#include <pango/pango.h>
#include <pango/pangocairo.h>
#include <cairomm/cairomm.h>
#include <gtk/gtk.h>
#include <string>
#include <iostream>

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

#define PIXELSIZE 200
#define PPU 10.

void realizeCB(GtkWidget *layout, gpointer) {
  gtk_layout_set_size(GTK_LAYOUT(layout), PIXELSIZE, PIXELSIZE);
}

void textAt(const char *txt, double size, double x, double y,
	    Cairo::RefPtr<Cairo::Context> context)
{
  PangoLayout *lo = pango_cairo_create_layout(context->cobj());
  pango_layout_set_text(lo, txt, -1);
  std::string fd = "Times " + std::to_string(size);
  PangoFontDescription *pfd = pango_font_description_from_string(fd.c_str());
  pango_layout_set_font_description(lo, pfd);
  pango_font_description_free(pfd);

  context->save();
  double baseline = pango_layout_get_baseline(lo)/double(PANGO_SCALE);
  context->move_to(x, y+baseline);
  pango_layout_context_changed(lo);
  pango_cairo_show_layout(context->cobj(), lo);
  g_object_unref(lo);
  context->restore();
}


bool drawCB(GtkWidget *layout, Cairo::Context::cobject *ctxt, gpointer) {
  Cairo::RefPtr<Cairo::Context> context(new Cairo::Context(ctxt, false));

  Cairo::Matrix transf(PPU, 0., 0., PPU, 0., 0.);
  context->set_matrix(transf);

  textAt("1: hello", 1., 3, 3, context);
  textAt("2: hello", 2,  10, 2, context);
  textAt("5: hello", 5, 20, 5, context);
  textAt("10: hello", 10, 0, 10, context);
  textAt("15: hello", 15, 15, 20, context);
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

  GtkWidget *layout = gtk_layout_new(nullptr, nullptr);
  gtk_container_add(GTK_CONTAINER(frame), layout);

  g_signal_connect(G_OBJECT(layout), "realize",
		   G_CALLBACK(realizeCB), nullptr);
  g_signal_connect(G_OBJECT(layout), "draw",
		   G_CALLBACK(drawCB), nullptr);

  GtkWidget *button = gtk_button_new_with_label("Quit");
  g_signal_connect(button, "clicked", gtk_main_quit, nullptr);
  gtk_box_pack_start(GTK_BOX(vbox), button, false, false, 2);

  gtk_widget_show_all(window);
  gtk_window_present_with_time(GTK_WINDOW(window), 0);

  gtk_main();
}
