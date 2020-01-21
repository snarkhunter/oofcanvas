// -*- C++ -*-

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modified
 * versions of this software, you first contact the authors at
 * oof_manager@nist.gov. 
 */

#include <gtk/gtk.h>
#include "oofcanvas.h"
using namespace OOFCanvas;

Canvas *canvas = nullptr;

#define ZOOM 1.1;

void quitCB(GtkButton*,  gpointer app) {
  g_application_quit(G_APPLICATION(app));
  delete canvas;
}

void zoomCB(GtkButton*, gpointer factorptr) {
  double factor = *(double*)(factorptr);
  canvas->zoom(factor);
}

void fillCB(GtkButton*, gpointer) {
  canvas->fill();
}

void mousefunc(const std::string &eventname, double x, double y,
	       int button, bool shift, bool ctrl)
{
  if(eventname == "up") {
    std::cerr << "mousefunc: " << eventname << " (" << x << ", " << y << ") "
	      << button << " " << shift <<" " << ctrl << std::endl;
    std::vector<CanvasItem*> clicked(canvas->clickedItems(x, y));
    std::cerr << "Clicked on " << clicked.size() << " items:" << std::endl;
    for(CanvasItem *item : clicked)
      std::cerr << "     " << *item << std::endl;
  }
}

//=\\=//

void draw() {
  std::cerr << "canvastest:draw" << std::endl;
  CanvasLayer *layer = canvas->newLayer("grid");
  CanvasSegments *segs = new CanvasSegments();
  segs->setLineColor(black);
  segs->setLineWidth(0.002);
  int ndivs = 10;
  for(int i=0; i<=ndivs; i++) {
    double v = 1./ndivs*i;
    segs->addSegment(0, v, 1, v);
    segs->addSegment(v, 0, v, 1);
  }
  layer->addItem(segs);

  CanvasRectangle *rect = new CanvasRectangle(0.0, 0.0, 1.0, 1.0);
  rect->setLineWidth(0.05);
  rect->setLineColor(black);
  layer->addItem(rect);

  layer = canvas->newLayer("circles");
  layer->setClickable(true);
  CanvasCircle *circle = new CanvasCircle(0.25, 0.75, 0.2);
  circle->setFillColor(blue.opacity(0.5));
  layer->addItem(circle);
  circle = new CanvasCircle(0.5, 0.75, 0.2);
  circle->setLineWidth(0.02);
  circle->setLineColor(black);
  layer->addItem(circle);
  
  canvas->draw();
}

//=\\=//

void activate(GtkApplication *app, gpointer data) {
  GtkWidget *window = gtk_application_window_new(app);
  gtk_window_set_title(GTK_WINDOW(window), "OOFCanvas Test");
  gtk_window_set_default_size(GTK_WINDOW(window), 200, 200);

  GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 3);
  gtk_container_add(GTK_CONTAINER(window), vbox);

  GtkWidget *frame = gtk_frame_new(NULL);
  gtk_box_pack_start(GTK_BOX(vbox), frame, TRUE, TRUE, 3);

  canvas = new Canvas(300.);
  canvas->show();
  gtk_widget_set_size_request(canvas->gtk(), 300, 300);
  canvas->setMouseCallback(&mousefunc, NULL);

#ifdef USE_GRID
  GtkWidget *grid = gtk_grid_new();
  gtk_container_add(GTK_CONTAINER(frame), grid);
  gtk_grid_attach(GTK_GRID(grid), canvas->gtk(), 0, 0, 1, 1);
  gtk_widget_set_hexpand(canvas->gtk(), TRUE);
  gtk_widget_set_vexpand(canvas->gtk(), TRUE);
  GtkWidget *hScrollbar = gtk_scrollbar_new(GTK_ORIENTATION_HORIZONTAL,
					    canvas->getHAdjustment());
  GtkWidget *vScrollbar = gtk_scrollbar_new(GTK_ORIENTATION_VERTICAL,
					    canvas->getVAdjustment());
  gtk_grid_attach(GTK_GRID(grid), hScrollbar, 0, 1, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), vScrollbar, 1, 0, 1, 1);
#else // Use a GtkScrolledWindow instead of a GtkGrid
  GtkWidget *swind = gtk_scrolled_window_new(canvas->getHAdjustment(),
					     canvas->getVAdjustment());
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(swind),
				 GTK_POLICY_ALWAYS, GTK_POLICY_ALWAYS);
  gtk_container_add(GTK_CONTAINER(frame), swind);
  gtk_container_add(GTK_CONTAINER(swind), canvas->gtk());
#endif // USE_GRID

  GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 3);
  gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 3);

  static double zoom = ZOOM;
  GtkWidget *zoominbutton = gtk_button_new_with_label("+");
  gtk_box_pack_start(GTK_BOX(hbox), zoominbutton, TRUE, TRUE, 3);
  g_signal_connect(zoominbutton, "clicked", G_CALLBACK(zoomCB), &zoom);

  GtkWidget *fillbutton = gtk_button_new_with_label("Fill");
  gtk_box_pack_start(GTK_BOX(hbox), fillbutton, TRUE, TRUE, 3);
  g_signal_connect(fillbutton, "clicked", G_CALLBACK(fillCB), NULL);

  static double invzoom = 1./ZOOM;
  GtkWidget *zoomoutbutton = gtk_button_new_with_label("-");
  gtk_box_pack_start(GTK_BOX(hbox), zoomoutbutton, TRUE, TRUE, 3);
  g_signal_connect(zoomoutbutton, "clicked", G_CALLBACK(zoomCB), &invzoom);

  GtkWidget *quitbutton = gtk_button_new_with_label("Quit");
  gtk_box_pack_start(GTK_BOX(hbox), quitbutton, TRUE, TRUE, 3);
  g_signal_connect(quitbutton, "clicked", G_CALLBACK(quitCB), app);
  
  gtk_widget_show_all(window);

  draw();
}

//=\\=//

int main(int argc, char **argv) {
  GtkApplication *app =
    gtk_application_new("gov.nist.ctcms.canvastest", G_APPLICATION_FLAGS_NONE);
  g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
  int status = g_application_run(G_APPLICATION(app), argc, argv);
  g_object_unref(app);
  return status;
}
