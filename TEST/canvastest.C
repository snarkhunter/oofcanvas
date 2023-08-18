// -*- C++ -*-

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modified
 * versions of this software, you first contact the authors at
 * oof_manager@nist.gov. 
 */

#include <oofcanvasgui.h>
#include <gtk/gtk.h>

#define FUDGE 10
#define FONTSIZE 0.3

void draw_items(OOFCanvas::Canvas *canvas) {
  canvas->clear();

  OOFCanvas::CanvasLayer *layer = canvas->newLayer("rect");
  
  OOFCanvas::CanvasRectangle *rect =
    new OOFCanvas::CanvasRectangle(FUDGE*OOFCanvas::Coord(0,0),
				   FUDGE*OOFCanvas::Coord(1,1));
  layer->addItem(rect);
  rect->setLineWidthInPixels(1);
  rect->setFillColor(OOFCanvas::Color(0.9, 0.9, 0.9));
  
  OOFCanvas::CanvasRectangle *rect2 =
    new OOFCanvas::CanvasRectangle(FUDGE*OOFCanvas::Coord(0.1, 0.1),
				   FUDGE*OOFCanvas::Coord(1.1, 1.1));
  rect2->setFillColor(OOFCanvas::red.opacity(0.5));
  layer->addItem(rect2);
  //rect2->drawBoundingBox(0.01, OOFCanvas::blue);

  OOFCanvas::CanvasLayer *tlayer = canvas->newLayer("text");
  OOFCanvas::CanvasText *text =
    new OOFCanvas::CanvasText(OOFCanvas::Coord(0.0, 0.0), "Aj");
  text->setFillColor(OOFCanvas::black);
  text->setFont("Times Bold " + std::to_string(FONTSIZE*FUDGE), false);
  tlayer->addItem(text);
  text->drawBoundingBox(0.02, OOFCanvas::red);

  OOFCanvas::CanvasText *text2 =
    new OOFCanvas::CanvasText(FUDGE*OOFCanvas::Coord(0.0, 0.3), "BAC");
  text2->setFont("Times " + std::to_string(FONTSIZE*FUDGE), false);
  text2->rotate(10);
  text2->drawBoundingBox(0.02, OOFCanvas::green);
  
  
  tlayer->addItem(text2);

  OOFCanvas::CanvasText *text2a =
    new OOFCanvas::CanvasText(FUDGE*OOFCanvas::Coord(0.0, 0.3), "BAC");
  text2a->setFont("Times " + std::to_string(FONTSIZE*FUDGE), false);
  text2a->setFillColor(OOFCanvas::Color(1., 0., 0., 0.9));
  text2a->drawBoundingBox(0.02, OOFCanvas::green);
  tlayer->addItem(text2a);

  OOFCanvas::CanvasText *text3 =
    new OOFCanvas::CanvasText(FUDGE*OOFCanvas::Coord(0.9, 0.6), "j");
  text3->setFont("Times " + std::to_string(FONTSIZE*FUDGE), false);
  text3->setFillColor(OOFCanvas::blue);
  text3->drawBoundingBox(0.02, OOFCanvas::black);
  tlayer->addItem(text3);

  canvas->zoomToFill();
}

static void buttonCB(GtkButton *btn, gpointer data) {
  draw_items((OOFCanvas::Canvas*) data);
}



int main(int argc, char *argv[]) {
  gtk_init(&argc, &argv);
  OOFCanvas::set_mainthread();
  std::cout << "Using OOFCanvas version " << OOFCANVAS_VERSION << std::endl;
  
  GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  g_signal_connect(window, "delete-event", gtk_main_quit, nullptr);

  GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
  gtk_container_add(GTK_CONTAINER(window), vbox);

  GtkWidget *frame = gtk_frame_new(nullptr) ;
  gtk_box_pack_start(GTK_BOX(vbox), frame, true, true, 2);
  gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_IN);

  GtkWidget *swind = gtk_scrolled_window_new(nullptr, nullptr);
  gtk_container_add(GTK_CONTAINER(frame), swind);

  OOFCanvas::Canvas canvas(100);
  gtk_container_add(GTK_CONTAINER(swind), canvas.gtk());
  canvas.setMargin(0.05);

  
  GtkWidget *button = gtk_button_new_with_label("Draw");
  g_signal_connect(button, "clicked", G_CALLBACK(buttonCB), &canvas);
  gtk_box_pack_start(GTK_BOX(vbox), button, false, false, 2);

  gtk_widget_show_all(window);
  gtk_window_present_with_time(GTK_WINDOW(window), 0);

  //canvas.draw();
  gtk_main();
}
