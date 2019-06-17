// A simple gtk2 app that includes an OOFCanvas.

// This version differs from gtktester.C in that it calls cairomm
// directly instead of using OOFCanvas.

// Started by copying the Hello World program from
// https://developer.gnome.org/gtk-tutorial/stable/c39.html#SEC-HELLOWORLD.

#include <cairomm/cairomm.h>
#include <gtk/gtk.h>
#include <iostream>
#include <stdlib.h>

/* This is a callback function. The data arguments are ignored
 * in this example. More on callbacks below. */
static void hello( GtkWidget *widget,
                   gpointer   data )
{
  std::cout << "Hello, world!" << std::endl;
}

static void draw(GtkWidget *widget, gpointer data) {
  GtkWidget *drawing_area = (GtkWidget*) data;
  gtk_widget_queue_draw(drawing_area);
}

static void expose(GtkWidget *widget, GdkEventExpose *event, gpointer data) {
  cairo_t *ct = gdk_cairo_create(gtk_widget_get_window(widget));
  assert(ct != nullptr);
  Cairo::RefPtr<Cairo::Context> ctxt(new Cairo::Context(ct, TRUE));
  double r = random()/2147483647.;
  double g = random()/2147483647.;
  double b = random()/2147483647.;
  ctxt->set_source_rgb(r, g, b);
  ctxt->paint();
}

static gboolean delete_event( GtkWidget *widget,
                              GdkEvent  *event,
                              gpointer   data )
{
    /* If you return FALSE in the "delete-event" signal handler,
     * GTK will emit the "destroy" signal. Returning TRUE means
     * you don't want the window to be destroyed.
     * This is useful for popping up 'are you sure you want to quit?'
     * type dialogs. */

    g_print ("delete\n");

    /* Change TRUE to FALSE and the main window will be destroyed with
     * a "delete-event". */

    return FALSE;
}

static void destroy( GtkWidget *widget,
                     gpointer   data )
{
    gtk_main_quit ();
}

int main( int   argc,
          char *argv[] )
{
    /* GtkWidget is the storage type for widgets */
    GtkWidget *window;
    GtkWidget *button;
    
    /* This is called in all GTK applications. Arguments are parsed
     * from the command line and are returned to the application. */
    gtk_init (&argc, &argv);
    
    /* create a new window */
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    
    /* When the window is given the "delete-event" signal (this is given
     * by the window manager, usually by the "close" option, or on the
     * titlebar), we ask it to call the delete_event () function
     * as defined above. The data passed to the callback
     * function is NULL and is ignored in the callback function. */
    g_signal_connect (window, "delete-event",
		      G_CALLBACK (delete_event), NULL);
    
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
    gtk_container_add (GTK_CONTAINER (window), vbox);

    
    /* Creates a new button with the label "Hello World". */
    button = gtk_button_new_with_label ("Hello, World!");
    gtk_box_pack_start(GTK_BOX(vbox), button, FALSE, FALSE, padding);
    
    /* When the button receives the "clicked" signal, it will call the
     * function hello() passing it NULL as its argument.  The hello()
     * function is defined above. */
    g_signal_connect (button, "clicked",
		      G_CALLBACK (hello), NULL);
    
    // /* This will cause the window to be destroyed by calling
    //  * gtk_widget_destroy(window) when "clicked".  Again, the destroy
    //  * signal could come from here, or the window manager. */
    // g_signal_connect_swapped (button, "clicked",
    // 			      G_CALLBACK (gtk_widget_destroy),
    //                           window);

    // Canvas canvas(100, 100, 1.0, 1.0);
    // gtk_box_pack_start(GTK_BOX(vbox), canvas.gtk(), TRUE, TRUE, padding);

    std::cerr << "Creating drawing area" << std::endl;
    GtkWidget *drawing_area = gtk_drawing_area_new();
    gtk_widget_set_size_request(drawing_area, -1, 100);
    gtk_box_pack_start(GTK_BOX(vbox), drawing_area, TRUE, TRUE, padding);
    // All cairo drawing is done in the expose event handler.
    g_signal_connect(drawing_area, "expose_event", G_CALLBACK(expose), NULL);
    
    GtkWidget *draw_button = gtk_button_new_with_label("Draw");
    gtk_box_pack_start(GTK_BOX(vbox), draw_button, FALSE, FALSE, padding);
    g_signal_connect(draw_button, "clicked", G_CALLBACK(draw), drawing_area);
    
    /* The final step is to display this newly created widget. */
    // gtk_widget_show (button);
    std::cerr << "Calling show_all" << std::endl;
    gtk_widget_show_all(vbox);
    
    /* and the window */
    gtk_widget_show (window);
    
    /* All GTK applications must have a gtk_main(). Control ends here
     * and waits for an event to occur (like a key press or
     * mouse event). */
    std::cerr << "Calling gtk_main" << std::endl;
    gtk_main ();
    
    return 0;
}
