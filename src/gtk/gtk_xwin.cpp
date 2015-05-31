#include "gsaf.h"
#include "base.h"
#include "ogl.h"
#include "callback.h"
#include "utils.h"
#include "anime.h"
#include "xwin.h"
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk/gdkx.h>
#include "gtk_base.h"
#include "gtk_xwin.h"
#include "gtk_menu2d.h"
#include "gtk_menu3d.h"
#include "gtk_menupro.h"
#include "gtk_tcp.h"
#include "gtk_gsglwindow.h"

void gui_init_tcp_mode (gsglLibrary *lib)
{
	gtk_set_locale();
	gtk_init (NULL, NULL);
	lib->Mydisplay = NULL;
	lib->Mydisplay = GDK_DISPLAY ();
	
	if (!lib->Mydisplay)
		gsgl_error ("gsgl_init_gtk_tcp_mode", "Can't open display");
	lib->Prewindow = (Window) inq_parent_window (lib);
}

static int exit_from_loop (void)
{
	gtk_main_quit ();
	return GSGL_TRUE;
}

Window gui_make_drawable_window_mode (gsglWindow *w)
{
	/*unsigned int handler = 0;*/

	w->Mywindow = gsgl_gtkwindow (w->library->Vi, w);
	XFree (w->library->Vi);

	glXMakeCurrent (w->library->Mydisplay, GDK_WINDOW_XWINDOW(gsgl_gtk_inq_glarea ()->window), w->library->Cx);
	/*handler = gtk_timeout_add (30, (GtkFunction) exit_from_loop, NULL);
	  gtk_main ();
	  gtk_timeout_remove (handler);*/

	return w->Mywindow;
}

void gui_mainloop (gsglPage *p)
{
	unsigned int handler;

	gsglMainWindow *w = gsgl_inq_mainwin ();
	w->set_up_gui (GSGL_FALSE);
	handler = gsgl_gtk_tcp_input_add (p);
	gtk_main();
	gsgl_gtk_tcp_input_remove (handler, p);
}
