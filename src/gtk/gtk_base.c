#include "gsaf.h"
#include "base.h"
#include "ogl.h"
#include "configfile.h"
#include "xwin.h"
#include "displaylist.h"
#include "matrix.h"
#include "fileio_base.h"
#include "fileio_write.h"
#include "callback.h"
#include "clientstate.h"
#include "utils.h"
#include "tcpip.h"
#include "device.h"
#include "fileformat.h"
#include "anime.h"
#include "drv_wrapper.h"
#include "bsc1.h"
#include "toolkit.h"
#include <regex.h>
#include <locale.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk/gdkx.h>
#include <gdk/gdkkeysyms.h>
#define GTK_BASE
#include "gtk_base.h"
#include "gtk_anime.h"
#include "gtk_menu2d.h"
#include "gtk_menu3d.h"
#include "gtk_menupro.h"
#include "gtk_menuedit.h"
#include "gtk_config.h"
#include "gtk_gsglwindow.h"
#include "guiitems.h"
#include "diacanvas.h"

#if GTK_MAJOR_VERSION == 2
#    define USE_GTK20
#else
#    define USE_GTK12
#endif

void gsgl_gtk_gettextize ()
{
	setlocale (LC_ALL, "");
        bindtextdomain (PACKAGE, LOCALEDIR);
#ifdef USE_GTK20
	bind_textdomain_codeset (PACKAGE, "UTF-8");
#endif
        textdomain (PACKAGE);
}

void gsgl_quitapp (GtkWidget *widget, gpointer gdata)
{
	void (*callback)();
	gsglPage *p = gdata;

        /*if (gdata != NULL)
	  gtk_widget_destroy (GTK_WIDGET (gdata));*/
	gsgl_finish_gui (p);
	callback = gsaf_inq_quit_callback ();
	if (callback != NULL)
		callback ();
	gtk_exit (0);
	/*return (FALSE);*/
}

void gsgl_gtk_spin_title (GtkWidget *w, gchar *title)
{
	GtkWidget *label;
	
	label = gtk_label_new (title);
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	gtk_misc_set_padding (GTK_MISC (label), 10, 5);
	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (w)->vbox), label, FALSE,TRUE,0);
	gtk_widget_show (label);
}

GtkWidget *gsgl_gtk_spin (GtkWidget *w, GtkObject *adjust, gchar *direc,
			  gchar *unit, gfloat climb_rate, guint digits)
{
	GtkWidget *label, *hbox, *spin;
	
	hbox = gtk_hbox_new (FALSE, 0);
	gtk_container_add (GTK_CONTAINER(GTK_DIALOG(w)->vbox), hbox);

	label = gtk_label_new (direc);
	gtk_misc_set_padding (GTK_MISC (label), 0, 0);
	gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, TRUE, 20);
	gtk_widget_show (label);

	spin = gtk_spin_button_new (GTK_ADJUSTMENT(adjust), climb_rate, digits);
	gtk_box_pack_start (GTK_BOX (hbox), spin, TRUE, TRUE, 0);
	gtk_widget_show (spin);

	label = gtk_label_new (unit);
	gtk_misc_set_padding (GTK_MISC (label), 0, 5);
	gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, TRUE, 20);
	gtk_widget_show (label);
	
	gtk_widget_show (hbox);

	return spin;
}

GtkWidget *gsgl_dialog_new (gchar *title, gint border)
{
	GtkWidget *w;
	
	w = gtk_dialog_new ();
	gtk_window_position (GTK_WINDOW(w), GTK_WIN_POS_CENTER);
	gtk_signal_connect (GTK_OBJECT (w), "destroy",
			    GTK_SIGNAL_FUNC (gsgl_destroy), w);
	gtk_window_set_title (GTK_WINDOW (w), title);
	gtk_container_border_width (GTK_CONTAINER (w), border);

	return w;
}

void gsgl_label_on_dialog_new (gchar *title, GtkWidget *parent,
			       gint padding1, gint padding2)
{
	GtkWidget *label;

	label = gtk_label_new (title);
	gtk_misc_set_padding (GTK_MISC (label), padding1, padding2);
	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (parent)->vbox), label, FALSE, TRUE, 0);
	gtk_widget_show (label);
}

void gsgl_button_on_dialog_new (gchar *title, GtkWidget *parent, void (*callback_func) (GtkWidget *, gpointer), gpointer gdata, gboolean grab_default)
{
	GtkWidget *button;

	button = gtk_button_new_with_label (title);
	if (callback_func != NULL)
		gtk_signal_connect (GTK_OBJECT (button), "clicked", GTK_SIGNAL_FUNC (callback_func), gdata);
	gtk_signal_connect (GTK_OBJECT (button), "clicked", GTK_SIGNAL_FUNC (gsgl_closedialog), parent);
	GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(parent)->action_area), button, TRUE, TRUE, 0);
	if (grab_default) gtk_widget_grab_default (button);
	gtk_widget_show (button);
}

void gsgl_button_on_dialog_new_without_close (gchar *title, GtkWidget *parent,
					      void (*callback_func)(GtkWidget *, gpointer),
					      gpointer gdata, 
					      gboolean grab_default)
{
	GtkWidget *button;

	button = gtk_button_new_with_label (title);
	if (callback_func != NULL)
		gtk_signal_connect (GTK_OBJECT (button), "clicked", GTK_SIGNAL_FUNC (callback_func), gdata);
	GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(parent)->action_area), button, TRUE, TRUE, 0);
	if (grab_default) gtk_widget_grab_default (button);
	gtk_widget_show (button);
}

void gsgl_cancelbutton_on_dialog_new (GtkWidget *parent, gboolean grab_default)
{
	gsgl_button_on_dialog_new_without_close (_("Cancel"), parent,
						 GTK_SIGNAL_FUNC (gsgl_closedialog), 
						 parent, grab_default);
}

gint gsgl_destroy (GtkWidget *widget, gpointer gdata)
{
	gtk_grab_remove (GTK_WIDGET (widget));
	return (FALSE);
}

void gsgl_file_open_and_save (GtkWidget *w, gpointer gdata)
{
	int open = 1;
	char *message;
	gsgllist *list;
	char *file;
	gsglPage *Page;

	list = gdata;
	file = list->data; list = list->next;
	Page = list->data;
	
	if (gsgl_inq_savefile_opened ()) gsgl_file_write_close (Page->library);
	open = gsgl_file_write_open (file, Page->library);
	if (open != 0) {
		message = g_strdup_printf (_("Can not open file \"%s\""), file);
		gsgl_message_and_ok_button (_("Can not open a file"), message);
		g_free (message);
	} else {
		gsgl_saving_message (Page);
	}
	g_free (file);
}

void gsgl_file_exist_dialog (char *file, gsglPage *Page)
{
	GtkWidget *window;
	gsgllist *list = NULL;

	window = gsgl_dialog_new (_("Overwrite?"), 5);
	if (gsgl_inq_mode (Page->library) == GSGL_MODE_WINDOW)
			gtk_window_set_transient_for (GTK_WINDOW(window), GTK_WINDOW(gsgl_gtk_inq_appwindow ()));
	gsgl_label_on_dialog_new (_("Old file is going to be overwritten.\n"
				    "Are you sure ?"), window, 10, 10);

	list = gsgl_list_append (list, file);
	list = gsgl_list_append (list, Page);

	gsgl_button_on_dialog_new (_("OK"), window, gsgl_file_open_and_save, list, GSGL_TRUE);
	gsgl_cancelbutton_on_dialog_new (window, GSGL_FALSE);

	gtk_widget_show (window);
	gtk_grab_add (window);
}

GtkWidget * gsgl_message_window_new (char *title, char *message)
{
	GtkWidget *window;
	GtkWidget *label;
	
	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_position (GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	gtk_window_set_title (GTK_WINDOW (window), title);
	gtk_container_border_width (GTK_CONTAINER (window), 5);	
	gtk_signal_connect (GTK_OBJECT (window), "destroy", GTK_SIGNAL_FUNC (gsgl_destroy),
			    window);
	label = gtk_label_new (message);
	
	gtk_container_add (GTK_CONTAINER (window), label);
	gtk_misc_set_padding (GTK_MISC (label), 20, 10);

	gtk_widget_show (label);
	gtk_widget_show_now (window);

	gtk_window_set_modal (GTK_WINDOW(window), TRUE);
        gtk_window_set_transient_for (GTK_WINDOW(window), GTK_WINDOW(gsgl_gtk_inq_appwindow ()));

	return window;
}

void gsgl_print_exec (GtkWidget *widget, gpointer gdata)
{
	const gchar *entry_text;
	gchar *command;
	gchar tmp[] = "/tmp/gsglXXXXXX";
	FILE  *fp;
	int   fd;
	gsglWriteToFile *w;
	gsgllist *list;
	GtkWidget *entry;
	gsglPage *Page;

	list = gdata;
	entry = list->data; list = list->next;
	Page = list->data;

	entry_text = gtk_entry_get_text(GTK_ENTRY(entry));

	fd = mkstemp (tmp);
	fp = fdopen (fd, "w");
	
	w = gsgl_init_write_to_file ();
	gsgl_write_to_file (fp, w, Page);

	command = g_strdup_printf (entry_text, tmp);
	system (command);
	g_free (command);
	
	if (fclose (fp))
		gsgl_warning ("gsgl_print_exec", "failed to close tmp file.");
	if (remove (tmp))
		gsgl_warning ("gsgl_print_exec", "failed to remove tmp file");
}

Window gsgl_gtkwindow (XVisualInfo *vi, gsglWindow *w)
{
	GtkItemFactoryEntry items[] = {
		{N_("/_File"), NULL, 0, 0, "<Branch>"},
		{N_("/File/tear"), NULL, 0, 0, "<Tearoff>"},
		{N_("/File/Save As ..."), "A", gsgl_gtk_save_as, GUI_SAVE_UN_CLOSE, 0},
		{N_("/File/Save"), "S", gsgl_gtk_save, GUI_SAVE_UN_CLOSE, 0},
		{N_("/File/Save and Close"), "Y", gsgl_gtk_save_and_close, GUI_SAVE_DO_CLOSE,0},
		{N_("/File/Save This Frame"), NULL, gsgl_gtk_save_this_frame, 0, 0},
		{N_("/File/Export"), NULL, 0, 0, "<Branch>"},
		{N_("/File/Export/_EPS (Vector)"), NULL, gsgl_gtk_export, EXPORT_EPS, 0},
		{N_("/File/Export/_EPS (Raster)"),NULL,gsgl_gtk_export,EXPORT_RASTER_EPS,0},
		{N_("/File/Export/_PNG"), NULL, gsgl_gtk_export, EXPORT_PNG, 0},
		{N_("/File/Export/_JPEG"), NULL, gsgl_gtk_export, EXPORT_JPG, 0},
		{"/File/sep1", NULL, 0, 0, "<Separator>"},
		{N_("/File/Print ..."), "P", gsgl_gtk_print, 0, 0},
		{"/File/sep2", NULL, 0, 0, "<Separator>"},
		{N_("/File/Dump Dot Image"), "D", gsgl_gtk_dumpwin, 0, 0},
		{"/File/sep3", NULL, 0, 0, "<Separator>"},
		{N_("/File/Close"), "C", gtk_main_quit, 0, 0},
		{N_("/File/Quit"), "Q", gsgl_quit_dialog, 0, 0},
		/*---------------------------------------*/
		{N_("/_Edit"), NULL, 0, 0, "<Branch>"},
		{N_("/Edit/tear"), NULL, 0, 0, "<Tearoff>"},
		{N_("/Edit/_Pointer"), "Escape", gsgl_gtk_select_obj, 0, 0},
		{N_("/Edit/Select All _2D Objects"), "2", gsgl_gtk_2d_select_obj, 0, 0},
		{N_("/Edit/Select All _3D Objects"), "3", gsgl_gtk_3d_select_obj, 0, 0},
		/*{N_("/Edit/sepe1"), NULL, 0, 0, "<Separator>"},*/
		/*{N_("/Edit/Add Lines"), NULL, gsgl_gtk_add_line, 0, 0},*/
		/*{N_("/Edit/Add Texts"), NULL, gsgl_gtk_add_text, 0, 0},*/
		/*---------------------------------------*/
		{N_("/_Objects"), NULL, 0, 0, "<Branch>"},
		{N_("/Objects/tear"), NULL, 0, 0, "<Tearoff>"},
		{N_("/Objects/_Rotate"), "R", gsgl_gtk_rotate_mouse, WITHOUT_QUESTION, 0},
		{N_("/Objects/_Scale"), NULL, gsgl_gtk_scale_mouse, WITH_QUESTION, 0},
		{N_("/Objects/_Translate (x,y)"), "T", gsgl_gtk_trans_xy_mouse, 0, 0},
		{N_("/Objects/_Translate (z)"), NULL, gsgl_gtk_trans_z_mouse, 0, 0},
		{N_("/Objects/_Toggle Lighting"), "L", gsgl_gtk_lighting_toggle, 0, 0},
		{"/Objects/sep_mod", NULL, 0, 0, "<Separator>"},
		{N_("/Objects/_Restore 2D"), "<CTRL>2", gsgl_gtk_2d_restore, 0, 0},
		{N_("/Objects/_Restore 3D"), "<CTRL>3", gsgl_gtk_3d_restore, 0, 0},
		/*---------------------------------------*/
		{N_("/_Projected Image"), NULL, 0, 0, "<Branch>"},
		{N_("/Projected Image/tear"), NULL, 0, 0, "<Tearoff>"},
		{N_("/Projected Image/_Rotate"), "<CTRL>R", gsgl_gtk_proj_rotate_mouse, WITHOUT_QUESTION, 0},
		{N_("/Projected Image/_Scale"), NULL, gsgl_gtk_proj_scale_mouse, WITH_QUESTION, 0},
		{N_("/Projected Image/_Translate"), "<CTRL>T", gsgl_gtk_proj_trans_mouse, 0, 0},
		{"/Projected Image/sepp-4", NULL, 0, 0, "<Separator>"},
		{N_("/Projected Image/_Restore"), "<CTRL>P", gsgl_gtk_proj_restore, 0, 0},
		/*---------------------------------------*/
		{N_("/_Animation"), NULL, 0, 0, "<Branch>"},
		{N_("/Animation/tear"), NULL, 0, 0, "<Tearoff>"},
		{N_("/Animation/_Replay"), NULL, gsgl_gtk_anime_replay, 2, 0},
		{N_("/Animation/_Replay (Slow)"), NULL, gsgl_gtk_anime_replay, 100, 0},
		{N_("/Animation/_Replay (Very Slow)"), NULL, gsgl_gtk_anime_replay, 500, 0},
		{N_("/Animation/_Single Frame Forward"), NULL, gsgl_gtk_anime_forward, 0, 0},
		{N_("/Animation/_Reverse Play"), NULL, gsgl_gtk_anime_reverse_replay, 2, 0},
		{N_("/Animation/_Reverse Play (Slow)"), NULL, gsgl_gtk_anime_reverse_replay, 100, 0},
		{N_("/Animation/_Reverse Play (Very Slow)"), NULL, gsgl_gtk_anime_reverse_replay, 500, 0},
		{N_("/Animation/_Single Frame Reverse"), NULL, gsgl_gtk_anime_backward, 0, 0},
		{N_("/Animation/_Pause"), NULL, gsgl_gtk_anime_pause, 0, 0},
		/*---------------------------------------*/
		{N_("/_Config"),NULL, 0, 0, "<Branch>"},
		{N_("/Config/tear"), NULL, 0, 0, "<Tearoff>"},
		{N_("/Config/_Color"), NULL, gsgl_gtk_config_color, 0, 0},
		{N_("/Config/_Command Line"), NULL, gsgl_gtk_config_command, 0, 0},
		{N_("/Config/_TrueType Font"), NULL, gsgl_gtk_config_ttf, 0, 0},
		{N_("/Config/_Window Size"), NULL, gsgl_gtk_config_window, 0, 0},
		/*---------------------------------------*/
		{N_("/_Help"), NULL, 0, 0, "<LastBranch>"},
		{N_("/Help/tear"), NULL, 0, 0, "<Tearoff>"},
		{N_("/Help/_Key Bindings"), "H", gsgl_help, 0, 0},
		{N_("/Help/_Manual"), "M", gsgl_manual, 0, 0},
		{"/Help/seph-1", NULL, 0, 0, "<Separator>"},
		{N_("/Help/_About"), NULL, gsgl_about, 0, 0},
	};
	GtkItemFactoryEntry pitems[] = {
		{N_("/Pointer"), "Escape", gsgl_gtk_select_obj, 0, 0},
		{N_("/Select All 2D Objects"), "2",  gsgl_gtk_2d_select_obj,0,0},
		{N_("/Select All 3D Objects"), "3", gsgl_gtk_3d_select_obj,0,0},
		{"/sep1", NULL, 0, 0, "<Separator>"},
		/*---------------------------------------*/
		{N_("/Rotate"), "R", gsgl_gtk_rotate_mouse, WITHOUT_QUESTION, 0},
		{N_("/Scale"), "E", gsgl_gtk_scale_mouse, WITHOUT_QUESTION, 0},
		{N_("/Translate (x-y)"), "T", gsgl_gtk_trans_xy_mouse, 0, 0},
		{N_("/Translate (z)"), NULL, gsgl_gtk_trans_z_mouse, 0, 0},
		{N_("/Toggle Lighting"), "L", gsgl_gtk_lighting_toggle, 0, 0},
		{N_("/Restore 2D"), "<CTRL>2", gsgl_gtk_2d_restore, 0, 0},
		{N_("/Restore 3D"), "<CTRL>3", gsgl_gtk_3d_restore, 0, 0},
		{"/sep2", NULL, 0, 0, "<Separator>"},
		/*---------------------------------------*/
		{N_("/Rotate Projected Image"), "<CTRL>R", gsgl_gtk_proj_rotate_mouse,WITHOUT_QUESTION,0},
		{N_("/Scale Projected Image"), "<CTRL>E", gsgl_gtk_proj_scale_mouse,WITHOUT_QUESTION,0},
		{N_("/Translate Projected Image"), "<CTRL>T", gsgl_gtk_proj_trans_mouse, 0, 0},
		{N_("/Restore Projected Image"), "<CTRL>P", gsgl_gtk_proj_restore, 0, 0},
	};
	return gsgl_gtk_make_mainwin (vi, items, sizeof(items)/sizeof(items[0]), 
				      pitems, sizeof(pitems)/sizeof(pitems[0]), w);
}

void gsgl_gtk_forbit_selection ()
{
	gsgl_gtk_bool_selection (GSGL_FALSE);
}

void gsgl_gtk_allow_selection ()
{
	gsgl_gtk_bool_selection (GSGL_TRUE);	
}

void gsgl_gtk_set_dump_is_enable_or_not ()
{
	/*if (gsgl_inq_i_am_a_server (Page->library))
	  gtk_widget_set_sensitive (gtk_item_factory_get_widget (GMenu, N_("/File/Dump Dot Image")), GSGL_FALSE);*/
}
