// -*- C++ -*-
// A simple gtk2 app that includes an OOFCanvas.

// Started by copying the Hello World program from
// https://developer.gnome.org/gtk-tutorial/stable/c39.html#SEC-HELLOWORLD.

#include <gtk/gtk.h>

#include "canvas.h"
#include "canvaslayer.h"
#include "canvasrectangle.h"
#include "canvassegments.h"

using namespace OOFCanvas;

static void hello( GtkWidget *widget,
                   gpointer   data )
{
    g_print ("Hello World\n");
}

static void draw(GtkWidget *widget, gpointer data) {
  std::cerr << "draw button was pressed" << std::endl;
  Canvas *canvas = (Canvas *) data;


  CanvasRectangle *rect = new CanvasRectangle(10, 10, 20, 20);
  CanvasLayer *layer = canvas->newLayer();
  rect->setLineWidth(2.);
  rect->setLineColor(1., 0., 0.);
  layer->addItem(rect);

  CanvasSegments *segs = new CanvasSegments(3);
  segs->setLineColor(0., 0., 1.);
  segs->setLineWidth(3.);
  segs->addSegment(30, 30, 50, 50);
  segs->addSegment(50, 50, 40, 50);
  segs->addSegment(50, 50, 50, 30);
  layer->addItem(segs);

  layer = canvas->newLayer();
  rect = new CanvasRectangle(15, 5, 40, 40);
  rect->setFillColor(1.0, 0, 0, 0.5);
  layer->addItem(rect);
  
  canvas->draw();
}


static void destroy( GtkWidget *widget,
                     gpointer   data )
{
    gtk_main_quit ();
}

int main( int   argc,
          char *argv[] )
{
    gtk_init (&argc, &argv);
    
    GtkWidget *window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    
    g_signal_connect (window, "destroy",
		      G_CALLBACK (destroy), NULL);
    
    gtk_container_set_border_width (GTK_CONTAINER (window), 10);


    gboolean homogeneous = false;
    gint spacing = 3;
    guint padding = 2;

    GtkWidget *vbox = gtk_vbox_new(homogeneous, spacing);
    gtk_container_add (GTK_CONTAINER (window), vbox);
    
    // Hello button
    GtkWidget *button = gtk_button_new_with_label ("Hello, World!");
    gtk_box_pack_start(GTK_BOX(vbox), button, FALSE, FALSE, padding);
    g_signal_connect (button, "clicked", G_CALLBACK (hello), NULL);

    // Canvas
    Canvas canvas(100, 100);
    canvas.setPixelsPerUnit(100);
    canvas.setBackgroundColor(0.95, 0.95, 1.0);
    gtk_box_pack_start(GTK_BOX(vbox), canvas.gtk(), TRUE, TRUE, padding);
    
    // Draw button
    GtkWidget *draw_button = gtk_button_new_with_label("Draw");
    gtk_box_pack_start(GTK_BOX(vbox), draw_button, FALSE, FALSE, padding);
    g_signal_connect(draw_button, "clicked", G_CALLBACK(draw), &canvas);
    
    gtk_widget_show_all(vbox);
    gtk_widget_show (window);
    
    gtk_main ();
    
    return 0;
}
