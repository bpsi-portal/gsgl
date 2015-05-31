#include "gsaf.h"
#include "fileformat.h"
#include "tcpip.h"
#include "displaylist.h"
#include "drv_wrapper.h"
#include "xwin.h"
#include "utils.h"
#include "anime.h"
#include "device.h"
#include "bsc1.h"
#include "tcp_guiitems.h"
#include "libserver.h"
#include "gui_common.h"
#include "gsgl.h"
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk/gdkx.h>
#include "gtk_base.h"
#include "gtk_gsglwindow.h"
#include "gtk_config.h"
#include "gtk_tcp.h"

/* must sync with gtk_base.c 's definition */
#define EXPORT_EPS          1
#define EXPORT_RASTER_EPS   2
#define EXPORT_PNG          3
#define EXPORT_JPG          4

void gsgl_set_main_window (Window win);

/* client-side callback functions */

static void func_save_and_close (void)
{
	gsgl_gtk_save (gsgl_inq_mainwin(), GUI_SAVE_DO_CLOSE, NULL);
}

static void func_save_as (void)
{
	gsgl_gtk_save_as (gsgl_inq_mainwin(), GUI_SAVE_UN_CLOSE, NULL);
}

static void func_save (void)
{
	gsgl_gtk_save (gsgl_inq_mainwin(), GUI_SAVE_UN_CLOSE, NULL);
}

static void func_save_this_frame (void)
{
	gsgl_gtk_save (gsgl_inq_mainwin(), GUI_SAVE_THIS_FRAME, NULL);
}

static void func_export_vec_eps (void)
{
	gsgl_gtk_export (gsgl_inq_mainwin(), EXPORT_EPS, NULL);
}	

static void func_export_ras_eps (void)
{
	gsgl_gtk_export (gsgl_inq_mainwin(), EXPORT_RASTER_EPS, NULL);
}	

static void func_export_png (void)
{
	gsgl_gtk_export (gsgl_inq_mainwin(), EXPORT_PNG, NULL);
}

static void func_export_jpg (void)
{
	gsgl_gtk_export (gsgl_inq_mainwin(), EXPORT_JPG, NULL);
}

static void func_print (void)
{
	gsgl_gtk_print (gsgl_inq_mainwin(), 0, NULL);
}

static void func_config_color (void)
{
	gsgl_gtk_config_color (gsgl_inq_mainwin(), 0, NULL);
}

static void func_config_command (void)
{
	gsgl_gtk_config_command (gsgl_inq_mainwin(), 0, NULL);
}

static void func_config_font (void)
{
	gsgl_gtk_config_ttf (gsgl_inq_mainwin(), 0, NULL);
}

static void func_config_window (void)
{
	gsgl_gtk_config_window (gsgl_inq_mainwin(), 0, NULL);
}

static void client_callback (gpointer data, gint source, GdkInputCondition condition)
{
	gsgl_client_callback_template (source, (gsglPage *) data,
				       gtk_main_quit,
				       func_save_and_close,
				       func_save_as,
				       func_save,
				       func_save_this_frame,
				       func_export_vec_eps,
				       func_export_ras_eps,
				       func_export_png,
				       func_export_jpg,
				       func_print,
				       func_config_color,
				       func_config_command,
				       func_config_font,
				       func_config_window);
}

void gui_tcp_callback_client (int socket, gsglPage *p)
{
	int handler;

	handler = gdk_input_add (p->library->Client_Socket, GDK_INPUT_READ, client_callback, p);
	gtk_main ();
	gdk_input_remove (handler);
}

/* server-side callback functions */

static void server_callback (gpointer data, gint source, GdkInputCondition condition)
{
	gsgl_server_callback_template (source, (gsglPage *) data,
				       gtk_main_quit);
}

int gsgl_gtk_tcp_input_add (gsglPage *p)
{
	int handler;

	if (gsgl_inq_i_am_a_server (p->library)) {
		handler = gdk_input_add (gsgl_inq_server_socket (p), GDK_INPUT_READ, server_callback, p);
		return handler;
	} else {
		return 0;
	}
}

void gsgl_gtk_tcp_input_remove (int handler, gsglPage *p)
{
	if (gsgl_inq_i_am_a_server (p->library)) {
		gdk_input_remove (handler);
	}
}
