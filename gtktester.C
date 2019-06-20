// -*- C++ -*-
// A simple gtk2 app that includes an OOFCanvas.

// Started by copying the Hello World program from
// https://developer.gnome.org/gtk-tutorial/stable/c39.html#SEC-HELLOWORLD.

#include <gtk/gtk.h>

#include "canvas.h"
using namespace OOFCanvas;

/* This is a callback function. The data arguments are ignored
 * in this example. More on callbacks below. */
static void hello( GtkWidget *widget,
                   gpointer   data )
{
    g_print ("Hello World\n");
}

static void draw(GtkWidget *widget, gpointer data) {
  Canvas *canvas = (Canvas *) data;
  canvas->draw();
}

/* static gboolean delete_event( GtkWidget *widget,
 *                               GdkEvent  *event,
 *                               gpointer   data )
 * {
 *     /\* If you return FALSE in the "delete-event" signal handler,
 *      * GTK will emit the "destroy" signal. Returning TRUE means
 *      * you don't want the window to be destroyed.
 *      * This is useful for popping up 'are you sure you want to quit?'
 *      * type dialogs. *\/
 * 
 *     g_print ("delete event occurred\n");
 * 
 *     /\* Change TRUE to FALSE and the main window will be destroyed with
 *      * a "delete-event". *\/
 * 
 *     return FALSE;
 * } */

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
    
    // /* When the window is given the "delete-event" signal (this is given
    //  * by the window manager, usually by the "close" option, or on the
    //  * titlebar), we ask it to call the delete_event () function
    //  * as defined above. The data passed to the callback
    //  * function is NULL and is ignored in the callback function. */
    // g_signal_connect (window, "delete-event",
    // 		      G_CALLBACK (delete_event), NULL);
    
    /* Here we connect the "destroy" event to a signal handler.  
     * This event occurs when we call gtk_widget_destroy() on the window,
     * or if we return FALSE in the "delete-event" callback. */
    g_signal_connect (window, "destroy",
		      G_CALLBACK (destroy), NULL);
    
    /* Sets the border width of the window. */
    gtk_container_set_border_width (GTK_CONTAINER (window), 10);


    gboolean homogeneous = false;
    gint spacing = 3;
    guint padding = 2;
    GtkWidget *vbox = gtk_vbox_new(homogeneous, spacing);
    
    /* Creates a new button with the label "Hello World". */
    GtkWidget *button = gtk_button_new_with_label ("Hello, World!");
    gtk_box_pack_start(GTK_BOX(vbox), button, FALSE, FALSE, padding);
    g_signal_connect (button, "clicked", G_CALLBACK (hello), NULL);
    
    Canvas canvas(100, 100, 1.0, 1.0);
    gtk_box_pack_start(GTK_BOX(vbox), canvas.gtk(), TRUE, TRUE, padding);
    
    gtk_container_add (GTK_CONTAINER (window), vbox);

    GtkWidget *draw_button = gtk_button_new_with_label("Draw");
    gtk_box_pack_start(GTK_BOX(vbox), draw_button, FALSE, FALSE, padding);
    g_signal_connect(draw_button, "clicked", G_CALLBACK(draw), &canvas);
    
    /* The final step is to display this newly created widget. */
    // gtk_widget_show (button);
    gtk_widget_show_all(vbox);
    
    /* and the window */
    gtk_widget_show (window);
    
    /* All GTK applications must have a gtk_main(). Control ends here
     * and waits for an event to occur (like a key press or
     * mouse event). */
    gtk_main ();
    
    return 0;
}
