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
#include "canvas.h"

static void quitCB(GtkButton*,  gpointer app) {
  g_application_quit(G_APPLICATION(app));
}

static void activate(GtkApplication *app, gpointer data) {
  GtkWidget *window = gtk_application_window_new(app);
  gtk_window_set_title(GTK_WINDOW(window), "OOFCanvas Test");
  gtk_window_set_default_size(GTK_WINDOW(window), 200, 200);

  GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 3);
  gtk_container_add(GTK_CONTAINER(window), vbox);

  GtkWidget *quitbutton = gtk_button_new_with_label("Quit");
  gtk_box_pack_start(GTK_BOX(vbox), quitbutton, TRUE, TRUE, 3);
  g_signal_connect(quitbutton, "clicked", G_CALLBACK(quitCB), app);
  
  gtk_widget_show_all(window);
}

int main(int argc, char **argv) {
  GtkApplication *app =
    gtk_application_new("gov.nist.ctcms.canvastest", G_APPLICATION_FLAGS_NONE);
  g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
  int status = g_application_run(G_APPLICATION(app), argc, argv);
  g_object_unref(app);
  return status;
}
