#include "gsaf.h"
#include "gtk_gsglwindow.h"
#include "xwin.h"
#include "base.h"
#include "configfile.h"
#include "clientstate.h"
#include "drv_wrapper.h"
#include "diacanvas.h"
#include "device.h"
#include "bsc1.h"
#include "gui_common.h"
#include "ogl.h"
#include "displaylist.h"
#include "callback.h"
#include "utils.h"
#include "anime.h"
#include <gdk/gdk.h>
#include <gdk/gdkx.h>
#include <gdk/gdkkeysyms.h>
#include "gtk_base.h"
#include "gtk_menu2d.h"
#include "gtk_menu3d.h"
#include "gtk_menupro.h"
#include "fileio_write.h"
#include "fileio_base.h"
#include "tcpip.h"
#include <regex.h>

#if GTK_MAJOR_VERSION == 2
#    define USE_GTK20
#else
#    define USE_GTK12
#endif

#define IS_CLOSE(x) (x == GUI_SAVE_DO_CLOSE ? GSGL_TRUE : GSGL_FALSE)

static gsglMainWindow *mainwin = NULL;

static void gsgl_export_command (char *file, gsglPage *Page);

static gchar *menu_translate (const gchar *path, gpointer data)
{
	return _(path);
}

static gint gsgl_destroyapp (GtkWidget *widget, gpointer gdata)
{
	/*gsglMainWindow *w = (gsglMainWindow *) gdata;*/
	gsglMainWindow *w = gsgl_inq_mainwin ();
	/*printf ("%p %p %p %p\n", w, w->Page, w2, w2->Page);*/

	gsgl_gui_quit (gtk_exit, w->Page);
	return FALSE;
}

static gboolean gsgl_gtk_redraw (GtkWidget *widget, GdkEventExpose *event, gpointer gdata)
{
	gsglMainWindow *w = (gsglMainWindow *) gdata;

	if (w->Page)
		gsgl_redraw (w->Page);	
 	return FALSE;
}

static gboolean gsgl_keypress (GtkWidget *widget, GdkEventKey *event, gpointer gdata)
{
	gsglMainWindow *w = (gsglMainWindow *) gdata;
	GdkModifierType state;

	state = (GdkModifierType) event->state;
	
	gsgl_gui_keypress (event->keyval, state & GDK_SHIFT_MASK, &(w->Key_is_pressed), &(w->Xkey), &(w->Ykey), w);
	return FALSE;
}

static gboolean gsgl_keyrelease (GtkWidget *widget, GdkEventKey *event, gpointer gdata)
{
	gsglMainWindow *w = (gsglMainWindow *) gdata;

	gsgl_gui_keyrelease (event->keyval, &(w->Key_is_pressed), &(w->Xkey), &(w->Ykey), w->Page);
        return FALSE;
}

static gboolean gsgl_window_configure (GtkWidget *widget, GdkEvent *event, gpointer gdata)
{
	gsglMainWindow *w = (gsglMainWindow *) gdata;
	unsigned int width = w->Glarea->allocation.width;
	unsigned int height = w->Glarea->allocation.height;

	if (w)
		gsgl_gui_window_configure (width, height, w->Page);
	else
		gsgl_gui_window_configure (width, height, NULL);
		
	return FALSE;
}

static gboolean gsgl_gtk_mouse_motion (GtkWidget *widget, GdkEventMotion *event, gpointer gdata)
{
	gsglMainWindow *w = (gsglMainWindow *) gdata;
	int   x, y;
	GdkModifierType state;
	
	if (event->is_hint) {
		gdk_window_get_pointer (event->window, &x, &y, &state);
	} else {
		x = (int) event->x;
		y = (int) event->y;
		state = (GdkModifierType) (event->state);
	}

	gsgl_gui_mouse_motion (x, y, &(w->Xstart), &(w->Ystart), 
			       (state & GDK_BUTTON1_MASK)||(state & GDK_BUTTON2_MASK), w->Page);

	return FALSE;
}

static gboolean gsgl_gtk_button_press (GtkWidget *widget, GdkEventMotion *event, gpointer gdata) 
{
	gsglMainWindow *w = (gsglMainWindow *) gdata;
	GdkModifierType state;
	gsglboolean left, right, mid;
	int x, y;
	int button_press;

	if (event->is_hint) {
		gdk_window_get_pointer (event->window, &x, &y, &state);
	} else {
		x = (int) event->x;
		y = (int) event->y;
		state = (GdkModifierType) event->state;
	}

	button_press = (event->type == GDK_BUTTON_PRESS ? 1 : 0); 
	left = state & GDK_BUTTON1_MASK;
	right = state & GDK_BUTTON2_MASK;
	mid = state & GDK_BUTTON3_MASK;

	gsgl_gui_button_press (x, y, button_press, left, right, mid,
			       &(w->Xstart), &(w->Ystart), w->Page, w);

	return FALSE;
}

static gboolean gsgl_gtk_button_release (GtkWidget *widget, GdkEventMotion *event, gpointer gdata)
{
	gsglMainWindow *w = (gsglMainWindow *) gdata;
	GdkModifierType state;
	gsglboolean left, right, mid;
	int x, y;

	if (event->is_hint) {
		gdk_window_get_pointer (event->window, &x, &y, &state);
	} else {
		x = (int) event->x;
		y = (int) event->y;
		state = (GdkModifierType) event->state;
	}
	left = state & GDK_BUTTON1_MASK;
	right = state & GDK_BUTTON2_MASK;
	mid = state & GDK_BUTTON3_MASK;

	gsgl_gui_button_release (x, y, left, right, mid, &(w->Xstart), &(w->Ystart), w->Page);
	return FALSE;
}

static gboolean gsgl_gtk_focus_in (GtkWidget *widget, GdkEventFocus *event, gpointer gdata)
{
	gsglMainWindow *w = (gsglMainWindow *) gdata;

	if (!w->have_accel) {
		gtk_window_add_accel_group (GTK_WINDOW(w->Appwindow), w->Accel1);
		gtk_window_add_accel_group (GTK_WINDOW(w->Appwindow), w->Accel2);
	}
	w->have_accel = GSGL_TRUE;
	return FALSE;
}

static gboolean gsgl_gtk_focus_out (GtkWidget *widget, GdkEventFocus *event, gpointer gdata)
{
	gsglMainWindow *w = (gsglMainWindow *) gdata;

	if (w->have_accel) {
		gtk_window_remove_accel_group (GTK_WINDOW(w->Appwindow), w->Accel1);
		gtk_window_remove_accel_group (GTK_WINDOW(w->Appwindow), w->Accel2);
	}
	w->have_accel = GSGL_FALSE;
	return FALSE;
}

static void main_image_full_screen_regrab (GtkWidget *widget, gpointer data)
{
	gsglMainWindow *w = (gsglMainWindow *) data;
        /* full screen, re-grab keyboard after popup-menu close */
        gdk_keyboard_grab(w->Glarea->window, TRUE, GDK_CURRENT_TIME);
}

static int gsgl_right_button_press (GtkWidget *widget, GdkEvent *event1, gpointer gdata)
{
	/*printf("GC counts: %d\n", GC_gc_no);*/
	gsgllist *l = (gsgllist *) gdata;
	GtkObject *obj = (GtkObject *) (l->data); l = l->next;
	gsglMainWindow *w = (gsglMainWindow *) (l->data);
	GdkEventButton *event = (GdkEventButton *) event1;
	GdkModifierType state = (GdkModifierType) event->state;
	
        if (event->type == GDK_BUTTON_PRESS) {
        if ((event->button == 3) ||
	    (event->button == 1 && state & GDK_SHIFT_MASK)) {
		if (w->Add_text) return GSGL_TRUE;

		GdkEventButton *bevent = (GdkEventButton *) event; 
		gtk_menu_popup (GTK_MENU (obj), NULL, NULL, NULL, NULL,
				bevent->button, bevent->time);
		/* full screen, re-grab keyboard after popup-menu close */
		if (gsgl_inq_fullscreen (w->Library))
			gtk_signal_connect (GTK_OBJECT (widget),"selection_done",
					    GTK_SIGNAL_FUNC (main_image_full_screen_regrab), 
					    w);
		return GSGL_TRUE;
	}
	}
	return GSGL_FALSE;
}

#ifdef USE_GTK20
#    define EVENT_METHOD(i, x) GTK_WIDGET_GET_CLASS(i)->x
#else
#    define EVENT_METHOD(i, x) GTK_WIDGET_CLASS(GTK_OBJECT(i)->klass)->x
#endif

void gsgl_gtk_set_page (gsglPage *p)
{
	mainwin->set_page (p);
}

gsglMainWindow::gsglMainWindow (XVisualInfo *vi, GtkItemFactoryEntry items[], int nitems, GtkItemFactoryEntry pitems[], int npitems, gsglWindow *win)
{
	GtkWidget *table;
	GtkWidget *hrule;
	GtkWidget *vrule;
	GdkColor black = {0,0x0000,0x0000,0x0000};
	GdkColor white = {0,0xffff,0xffff,0xffff}; 
	GdkVisual *visual;
	GtkWidget *vbox;

	Win = win;
	Library = win->library;
	Page = NULL;
	Xkey = 0;
	Ykey = 0;
	Key_is_pressed = GSGL_FALSE;
	Add_text = GSGL_FALSE;
	Add_text_dialog = NULL;

	if (!vi) return;

	if (!gsgl_inq_fullscreen (Library)) {
		Appwindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
		gtk_container_border_width (GTK_CONTAINER (Appwindow), 2);
		gtk_window_set_title (GTK_WINDOW(Appwindow), gsgl_window_title());
		if (gsgl_inq_userdefined_window (Library)) {
			gtk_widget_set_uposition (Appwindow,
						  gsgl_inq_userdefined_corner_x (Library),
						  gsgl_inq_userdefined_corner_y (Library));
		} else {
			gtk_window_position (GTK_WINDOW(Appwindow), GTK_WIN_POS_CENTER);
		}
	} else {
		Appwindow = gtk_window_new (GTK_WINDOW_POPUP);
		gtk_container_border_width (GTK_CONTAINER(Appwindow), 0);
	}
	vbox = gtk_vbox_new (FALSE, 0);

	/* menu */
	Accel1 = gtk_accel_group_new ();
	GMenu = gtk_item_factory_new (GTK_TYPE_MENU_BAR, "<main>", Accel1);
	gtk_item_factory_set_translate_func (GMenu, menu_translate, NULL, NULL);

	gtk_item_factory_create_items (GMenu, nitems, items, this);
	
	/*gtk_accel_group_attach (accel, GTK_OBJECT (Appwindow));*/
	gtk_window_add_accel_group (GTK_WINDOW(Appwindow), Accel1);

	gtk_box_pack_start (GTK_BOX(vbox), gtk_item_factory_get_widget (GMenu, "<main>"), FALSE, FALSE, 0);
	gtk_widget_show (gtk_item_factory_get_widget (GMenu, "<main>"));
	
	/* Can't use Restore at first. */
	gtk_widget_set_sensitive(gtk_item_factory_get_widget (GMenu, N_("/Objects/Restore 3D")), FALSE);
	gtk_widget_set_sensitive(gtk_item_factory_get_widget (GMenu, N_("/Objects/Restore 2D")), FALSE);
	gtk_widget_set_sensitive(gtk_item_factory_get_widget (GMenu, N_("/Projected Image/Restore")), FALSE);
	
	/* ルーラーと描画エリアを配置するための table を作成 */
	table = gtk_table_new (2, 2, FALSE);
	gtk_container_add (GTK_CONTAINER(vbox), table);
 	
	/* OpenGL drawing area */
	visual = gdkx_visual_get (vi->visualid);
#ifdef USE_GTK20
	Glarea = dia_canvas_new ();
	gtk_widget_set_colormap (Glarea, gdk_colormap_new (visual, TRUE));

	/* disable widget's backing store */
	gtk_widget_set_double_buffered (Glarea, GSGL_FALSE);
#else
	/* use colormap and visual suitable for OpenGL rendering */
	gtk_widget_push_colormap (gdk_colormap_new (visual, TRUE));
	gtk_widget_push_visual (visual);

	Glarea = dia_canvas_new ();

	/* pop back defaults */
	gtk_widget_pop_visual ();
	gtk_widget_pop_colormap ();	
#endif
	dia_canvas_set_size (DIA_CANVAS(Glarea), gsgl_inq_window_width (Win), gsgl_inq_window_height (Win));

	if (gsgl_inq_fullscreen (Library))
	      gtk_widget_set_uposition (Glarea, 0, 0);

	gtk_widget_show (Glarea);
	gtk_widget_set_events (Glarea,	GDK_EXPOSURE_MASK
			       | GDK_LEAVE_NOTIFY_MASK
			       | GDK_BUTTON_PRESS_MASK
			       | GDK_BUTTON_RELEASE_MASK
			       | GDK_BUTTON_MOTION_MASK
			       | GDK_POINTER_MOTION_MASK
			       | GDK_POINTER_MOTION_HINT_MASK
			       | GDK_KEY_PRESS_MASK
			       | GDK_KEY_RELEASE_MASK
		               | GDK_FOCUS_CHANGE);

	gtk_table_attach (GTK_TABLE(table), Glarea, 1, 2, 1, 2,
			  (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			  (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);
	gtk_container_add (GTK_CONTAINER(Appwindow), vbox);
	gtk_signal_connect (GTK_OBJECT(Appwindow), "delete_event",
			    GTK_SIGNAL_FUNC(gsgl_destroyapp), this);
	gtk_signal_connect (GTK_OBJECT(Glarea), "expose_event",
			    GTK_SIGNAL_FUNC(gsgl_gtk_redraw), this);
	gtk_signal_connect (GTK_OBJECT(Glarea), "key_press_event",
			    GTK_SIGNAL_FUNC(gsgl_keypress), this);
	gtk_signal_connect (GTK_OBJECT(Glarea), "key_release_event",
			    GTK_SIGNAL_FUNC(gsgl_keyrelease), this);
	gtk_signal_connect (GTK_OBJECT(Glarea), "configure_event",
			    GTK_SIGNAL_FUNC(gsgl_window_configure), this);
	gtk_signal_connect (GTK_OBJECT(Glarea), "motion_notify_event",
			    GTK_SIGNAL_FUNC(gsgl_gtk_mouse_motion), this);
	gtk_signal_connect (GTK_OBJECT(Glarea), "button_press_event",
			    GTK_SIGNAL_FUNC(gsgl_gtk_button_press), this);
	gtk_signal_connect (GTK_OBJECT(Glarea), "button_release_event",
			    GTK_SIGNAL_FUNC(gsgl_gtk_button_release), this);
	gtk_signal_connect (GTK_OBJECT(Glarea), "focus-in-event",
			    GTK_SIGNAL_FUNC(gsgl_gtk_focus_in), this);
	gtk_signal_connect (GTK_OBJECT(Glarea), "focus-out-event",
			    GTK_SIGNAL_FUNC(gsgl_gtk_focus_out), this);

	vrule = gtk_vruler_new ();
	hrule = gtk_hruler_new ();
	gtk_table_attach (GTK_TABLE (table), vrule, 0, 1, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
	gtk_table_attach (GTK_TABLE (table), hrule, 1, 2, 0, 1, GTK_FILL,  GTK_FILL, 0, 0);
	gtk_ruler_set_metric (GTK_RULER(vrule), GTK_PIXELS);
	gtk_ruler_set_metric (GTK_RULER(hrule), GTK_PIXELS);

	gtk_ruler_set_range (GTK_RULER(hrule), 0, 25.6, 0, 150);
	gtk_ruler_set_range (GTK_RULER(vrule), 19.0, 0, 0, 150);

	gtk_signal_connect_object (GTK_OBJECT(Glarea), "motion_notify_event",
				   GTK_SIGNAL_FUNC (EVENT_METHOD(hrule, motion_notify_event)),
				   GTK_OBJECT(hrule));
	gtk_signal_connect_object (GTK_OBJECT(Glarea), "motion_notify_event",
				   GTK_SIGNAL_FUNC (EVENT_METHOD(vrule, motion_notify_event)),
				   GTK_OBJECT(vrule));
	
	gtk_widget_show (vrule);
	gtk_widget_show (hrule);
	gtk_widget_show (table);
	gtk_widget_show (vbox);
	gtk_widget_show (Appwindow);
	gtk_widget_show_all (Appwindow);

	GTK_WIDGET_SET_FLAGS (Glarea, GTK_CAN_FOCUS);
	gtk_widget_grab_focus (Glarea);

	if (gsgl_inq_bg_color (Library) == GSGL_BACK_COLOR_BLACK) {
		gdk_color_alloc (gdk_colormap_get_system(), &black);
		gdk_window_set_background (Glarea->window, &black);
	} else {
		gdk_color_alloc (gdk_colormap_get_system(), &white);
		gdk_window_set_background (Glarea->window, &white);
	}

	/* pop up menu の作成 */
	Accel2 = gtk_accel_group_new ();
	GPopup = gtk_item_factory_new (GTK_TYPE_MENU, "<popup>", Accel2);
	gtk_item_factory_set_translate_func (GPopup, menu_translate, NULL, NULL);

	gtk_item_factory_create_items (GPopup, npitems, pitems, this);

	gtk_window_add_accel_group (GTK_WINDOW(Appwindow), Accel2);
	have_accel = GSGL_TRUE;

	list0 = gsgl_list_append (list0, GTK_OBJECT (gtk_item_factory_get_widget (GPopup, "<popup>")));
	list0 = gsgl_list_append (list0, this);

	gtk_signal_connect (GTK_OBJECT (Glarea), "event", GTK_SIGNAL_FUNC (gsgl_right_button_press), list0);

	gtk_widget_set_sensitive (gtk_item_factory_get_widget (GPopup, N_("/Restore 2D")), GSGL_FALSE);
	gtk_widget_set_sensitive (gtk_item_factory_get_widget (GPopup, N_("/Restore 3D")), GSGL_FALSE);
	gtk_widget_set_sensitive (gtk_item_factory_get_widget (GPopup, N_("/Restore Projected Image")), GSGL_FALSE);
	gsgl_gtk_set_sensitive_have_3d (GSGL_FALSE, GMenu, GPopup);
}

Window gsgl_gtk_make_mainwin (XVisualInfo *vi, GtkItemFactoryEntry items[], int nitems, GtkItemFactoryEntry pitems[], int npitems, gsglWindow *win)
{
	mainwin = new gsglMainWindow (vi, items, nitems, pitems, npitems, win);
	return GDK_WINDOW_XWINDOW(mainwin->Glarea->window);
}

void gsgl_make_mainwindow_tcp_mode (gsglWindow *win)
{
	mainwin = new gsglMainWindow (NULL, NULL, 0, NULL, 0, win);
}

GtkWidget * gsgl_gtk_inq_appwindow ()
{
	if (mainwin)
		return mainwin->Appwindow;
	else
		return NULL;
}

GtkWidget * gsgl_gtk_inq_glarea ()
{
	if (mainwin)
		return mainwin->Glarea;
	else
		return NULL;
}

void gsgl_spin_value_x (GtkAdjustment *adjustment, GtkWidget *spin)
{
	float x;
	
	x = gtk_spin_button_get_value_as_float (GTK_SPIN_BUTTON(spin));
	gsgl_utility_cross (x, -1, -1, mainwin->Page);
}

void gsgl_spin_value_y (GtkAdjustment *adjustment, GtkWidget *spin)
{
	float y;
	
	y = gtk_spin_button_get_value_as_float (GTK_SPIN_BUTTON(spin));
	gsgl_utility_cross (-1, y, -1, mainwin->Page);
}

void gsgl_spin_value_z (GtkAdjustment *adjustment, GtkWidget *spin)
{
	float z;
	
	z = gtk_spin_button_get_value_as_float (GTK_SPIN_BUTTON(spin));
	gsgl_utility_cross (-1, -1, z, mainwin->Page);
}

gsglMainWindow * gsgl_inq_mainwin ()
{
	return mainwin;
}

void gsgl_gtk_set_sensitive_have_3d (gsglboolean b, GtkItemFactory *GMenu, GtkItemFactory *GPopup)
{
	SENSITIVE (GPopup, N_("/Select All 3D Objects"), b);
	SENSITIVE (GMenu, N_("/Edit/Select All 3D Objects"), b);
}

void gsgl_gtk_bool_selection (gsglboolean b)
{
	SENSITIVE(mainwin->GPopup, N_("/Pointer"), b);
	SENSITIVE(mainwin->GPopup, N_("/Select All 2D Objects"), b);
	SENSITIVE(mainwin->GPopup, N_("/Select All 3D Objects"), b);
	SENSITIVE(mainwin->GPopup, N_("/Rotate"), b);
	SENSITIVE(mainwin->GPopup, N_("/Scale"), b);
	SENSITIVE(mainwin->GPopup, N_("/Translate (x-y)"), b);
	SENSITIVE(mainwin->GPopup, N_("/Translate (z)"), b);
	SENSITIVE(mainwin->GPopup, N_("/Toggle Lighting"), b);
	SENSITIVE(mainwin->GPopup, N_("/Restore 2D"), b);
	SENSITIVE(mainwin->GPopup, N_("/Restore 3D"), b);
	SENSITIVE(mainwin->GPopup, N_("/Rotate Projected Image"), b);
	SENSITIVE(mainwin->GPopup, N_("/Scale Projected Image"), b);
	SENSITIVE(mainwin->GPopup, N_("/Translate Projected Image"), b);
	SENSITIVE(mainwin->GPopup, N_("/Restore Projected Image"), b);
	SENSITIVE(mainwin->GMenu, N_("/Edit"), b);
	SENSITIVE(mainwin->GMenu, N_("/Objects"), b);
	SENSITIVE(mainwin->GMenu, N_("/Projected Image"), b);	
}

void gui_sensitive_have_3d (gsglboolean b)
{
	gsgl_gtk_set_sensitive_have_3d (b, mainwin->GMenu, mainwin->GPopup);
}

void gsgl_utility_clean (GtkWidget *w, gpointer gdata)
{
	gsgl_utility_clean ((gsglPage *)gdata);
}

void gsgl_gtk_save (gpointer data, unsigned int saving_flag, GtkWidget *widget)
{
	gsglMainWindow *w = (gsglMainWindow *) data;

	if (gsgl_inq_i_am_a_server (w->Library)) {
		gsgl_tcp_server_send_command (GSGL_TCP_GUI_SAVE, w->Library);
	} else {
		w->Saving_flag = saving_flag;
		if (gsgl_inq_savefile_opened ()) { /* file is opened */
			gsgl_saving_message (w->Page);
		} else {
			gsgl_gtk_save_as (data, saving_flag, widget);
		}
	}
}

void gsgl_saving_message (gsglPage *Page)
{
        char *title = _("Save");
	char *message = _("Saving now ...");
	GtkWidget *window;
	GTimer *timer;
	gulong microseconds;
	double seconds;
	gsglWriteToFile *w;

	window = gsgl_message_window_new (title, message);
	
	while (gtk_events_pending())
		gtk_main_iteration ();
	
	timer = g_timer_new ();
	g_timer_start (timer);

	if (gsgl_inq_open_tmp_file_for_anime (Page) && mainwin->Saving_flag != GUI_SAVE_THIS_FRAME) {
		gsgl_copy_anime_file_to_save_file (Page);
	} else {
		w = gsgl_init_write_to_file ();	
		gsgl_write_to_file (gsgl_get_fp_write(Page->library), w, Page);
		gsgl_free (w);
	}
	
	seconds = g_timer_elapsed (timer, &microseconds);
	g_timer_stop (timer);
	
	if (seconds <= 0.1) usleep (100000 - microseconds);
	g_timer_destroy (timer);
	
	if (IS_CLOSE(mainwin->Saving_flag)) {
		if (gsgl_inq_mode (Page->library) == GSGL_MODE_WINDOW)
			gtk_main_quit ();
		else if (gsgl_inq_mode (Page->library) == GSGL_MODE_TCPIP)
			gsgl_tcp_push (GSGL_TCP_GUI_SAVE_AND_CLOSE, Page);
	} 
	while (gtk_events_pending())
		gtk_main_iteration ();

	gtk_widget_hide (window);
}

void gsgl_gtk_save_as (gpointer gdata, unsigned int saving_flag, GtkWidget *widget)
{
	gsglMainWindow *w = (gsglMainWindow *) gdata;

	if (gsgl_inq_i_am_a_server (w->Library)) {
		gsgl_tcp_server_send_command (GSGL_TCP_GUI_SAVE_AS, w->Library);
	} else {
		w->Saving_flag = saving_flag;

		w->File_sel = gtk_file_selection_new (_("Save As ..."));
		gtk_window_position (GTK_WINDOW(w->File_sel), GTK_WIN_POS_CENTER);
		if (gsgl_inq_mode (w->Library) == GSGL_MODE_WINDOW)
			gtk_window_set_transient_for (GTK_WINDOW(w->File_sel), GTK_WINDOW(gsgl_gtk_inq_appwindow ()));
		
		gtk_file_selection_complete (GTK_FILE_SELECTION(w->File_sel), "*.gsgl");
		/*gtk_file_selection_set_filename (GTK_FILE_SELECTION(w->File_sel), ".gsgl");*/
		
		gtk_entry_set_position (GTK_ENTRY((GTK_FILE_SELECTION(w->File_sel)->selection_entry)), 0);
		gtk_signal_connect (GTK_OBJECT (w->File_sel), "destroy", GTK_SIGNAL_FUNC (gsgl_destroy), w->File_sel);
		gtk_signal_connect (GTK_OBJECT(GTK_FILE_SELECTION (w->File_sel)->ok_button),
				    "clicked", GTK_SIGNAL_FUNC (gsgl_fileselec_ok), w->Page);
		gtk_signal_connect_object 
			(GTK_OBJECT (GTK_FILE_SELECTION (w->File_sel)->ok_button),
			 "clicked", GTK_SIGNAL_FUNC (gtk_widget_destroy), GTK_OBJECT (w->File_sel));
		gtk_signal_connect_object 
			(GTK_OBJECT (GTK_FILE_SELECTION (w->File_sel)->cancel_button),
			 "clicked", GTK_SIGNAL_FUNC (gtk_widget_destroy), GTK_OBJECT (w->File_sel));

		gtk_widget_show (w->File_sel);
	}
}

void gsgl_gtk_save_and_close (gpointer gdata, unsigned int saving_flag, GtkWidget *widget)
{
	gsglMainWindow *w = (gsglMainWindow *) gdata;

	if (gsgl_inq_i_am_a_server (w->Library)) {
		gsgl_tcp_server_send_command (GSGL_TCP_GUI_SAVE_AND_CLOSE, w->Library);
	} else {
		w->Saving_flag = saving_flag;
		if (gsgl_inq_savefile_opened ()) { /* file is opened */
			gsgl_saving_message (w->Page);
		} else {
			gsgl_gtk_save_as (gdata, saving_flag, widget);
		}
	}
}

void gsgl_gtk_save_this_frame (gpointer gdata, guint action, GtkWidget *widget)
{
	gsglMainWindow *w = (gsglMainWindow *) gdata;

	w->Saving_flag = GUI_SAVE_THIS_FRAME;
	if (gsgl_inq_i_am_a_server (w->Library)) {
		gsgl_tcp_server_send_command (GSGL_TCP_GUI_SAVE_THIS_FRAME, w->Library);
	} else {
		gsgl_gtk_save (gdata, w->Saving_flag, widget);
	}
}

void gsgl_message_and_ok_button (char *title, char *message)
{
	GtkWidget *window;

	window = gsgl_dialog_new (title, 5);
	gsgl_label_on_dialog_new (message, window, 20, 10);
	gsgl_button_on_dialog_new (_("OK"), window, gsgl_closedialog, window, TRUE);
	gtk_widget_show (window);
	gtk_grab_add (window);
}

void gsgl_fileselec_ok (GtkWidget *w, gpointer data)
{
        char *file0, *file, *base;
	regex_t preg;
	char regex[] = ".gsgl$";
	regmatch_t pmatch[1];
	struct stat st;
	char *message;
	gsglPage *Page = (gsglPage *) data;

	file0 = g_strdup (gtk_file_selection_get_filename (GTK_FILE_SELECTION (mainwin->File_sel)));
	gtk_widget_hide (mainwin->File_sel);

#if GLIB_MAJOR_VERSION == 1
	base = g_basename (file0);
#elif GLIB_MAJOR_VERSION == 2
	base = g_path_get_basename (file0);
#endif
	if (strlen (base) == 0) {
		message = g_strdup_printf (_("\"%s\" is not a file name."), file0);
		gsgl_message_and_ok_button (_("Can not open a file"), message);
		return;
	}
	
	regcomp (&preg, regex, REG_EXTENDED);
	if (regexec (&preg, file0, 1, pmatch, 0)) {  /*dose not match*/
		file = g_strdup_printf ("%s.gsgl", file0);
	} else {
		file = g_strdup (file0);
	}
	regfree (&preg);

	if (stat(file, &st) != 0) { /* file dosen't exist. */
		gsgllist *l = NULL;
		l = gsgl_list_append (l, file);
		l = gsgl_list_append (l, Page);
		gsgl_file_open_and_save (NULL, l); 
	} else {
		if (S_ISREG (st.st_mode)) {
			gsgl_file_exist_dialog (file, Page);
		}
	}
	g_free (file0);
}

static void gsgl_file_open_and_export (GtkWidget *widget, gpointer gdata)
{
	char *message;
	FILE *fp;
	gsgllist *list = (gsgllist *) gdata;
	char *file = (char *) list->data;
	gsglPage *Page;
	list = list->next;
	Page = (gsglPage *) list->data;

	fp = fopen (file, "wb");
	if (fp == NULL) {
		fclose (fp);
		message = g_strdup_printf (_("Can not open file \"%s\""), file);
		gsgl_message_and_ok_button (_("Can not open a file"), message);
		g_free (message);
		g_free (file);
	} else {
		fclose (fp);
		gsgl_export_command (file, Page);
	}
}

static void gsgl_file_exist_export_dialog (char *file, gsglPage *Page)
{
	GtkWidget *window;
	gsgllist *list = NULL;

	list = gsgl_list_append (list, file);
	list = gsgl_list_append (list, Page);

	window = gsgl_dialog_new (_("Overwrite?"), 5);
	if (gsgl_inq_mode (Page->library) == GSGL_MODE_WINDOW)
		gtk_window_set_transient_for (GTK_WINDOW(window), GTK_WINDOW(gsgl_gtk_inq_appwindow ()));	

	gsgl_label_on_dialog_new ("Old file is going to be overwritten.\n"
				  "Are you sure ?", window, 10, 10);

	gsgl_button_on_dialog_new (_("OK"), window, gsgl_file_open_and_export, list, GSGL_TRUE);
	gsgl_cancelbutton_on_dialog_new (window, GSGL_FALSE);

	gtk_widget_show (window);
	gtk_grab_add (window);
}

static void gsgl_export_ok (GtkWidget *w, gpointer data)
{
        gchar *file0, *base;
	struct stat st;
	char *message;
	gsglPage *Page = (gsglPage *) data;

	file0 = g_strdup (gtk_file_selection_get_filename (GTK_FILE_SELECTION (mainwin->File_sel)));
	gtk_widget_hide (mainwin->File_sel);

#if GLIB_MAJOR_VERSION == 1
	base = g_basename (file0);
#elif GLIB_MAJOR_VERSION == 2
	base = g_path_get_basename (file0);
#endif
	if (strlen (base) == 0) {
		message = g_strdup_printf (_("\"%s\" is not a file name."), file0);
		gsgl_message_and_ok_button (_("Can not open a file"), message);
		return;
	}

	if (stat(file0, &st) != 0) {
		gsgllist *l = NULL;

		l = gsgl_list_append (l, file0);
		l = gsgl_list_append (l, Page);

		gsgl_file_open_and_export (NULL, l); 
	} else {
		if (S_ISREG(st.st_mode)) { 
			gsgl_file_exist_export_dialog (file0, Page);
		}
		/*switch (st.st_mode & S_IFMT) {
		  case S_IFREG:
		  gsgl_file_exist_export_dialog (file0, Page);
		  break;
		  default:
		  break;
		  }*/
	}
}

void gsgl_gtk_export (gpointer gdata, guint type, GtkWidget *widget)
{	
	gsglMainWindow *w = (gsglMainWindow *) gdata;
	w->Type = type;

	if (gsgl_inq_i_am_a_server (w->Library)) {
		if (w->Type == EXPORT_EPS) {
			gsgl_tcp_server_send_command (GSGL_TCP_GUI_EXPORT_VEC_EPS, w->Library);
		} else if (w->Type == EXPORT_RASTER_EPS) {
			gsgl_tcp_server_send_command (GSGL_TCP_GUI_EXPORT_RAS_EPS, w->Library);
		} else if (w->Type == EXPORT_PNG) {
			gsgl_tcp_server_send_command (GSGL_TCP_GUI_EXPORT_PNG, w->Library);
		} else if (w->Type == EXPORT_JPG) {
			gsgl_tcp_server_send_command (GSGL_TCP_GUI_EXPORT_JPEG, w->Library);
		}
	} else {
		
		w->File_sel = gtk_file_selection_new (_("Export as ..."));
		gtk_window_position (GTK_WINDOW(w->File_sel), GTK_WIN_POS_CENTER);
		if (gsgl_inq_mode (w->Library) == GSGL_MODE_WINDOW)
			gtk_window_set_transient_for (GTK_WINDOW(w->File_sel), GTK_WINDOW(gsgl_gtk_inq_appwindow ()));		
		gtk_signal_connect (GTK_OBJECT (w->File_sel), "destroy",
				    GTK_SIGNAL_FUNC (gsgl_destroy), w->File_sel);
		
		if (type == EXPORT_EPS)
			gtk_file_selection_complete (GTK_FILE_SELECTION(w->File_sel), "*.eps");
		else if (type == EXPORT_RASTER_EPS)
			gtk_file_selection_complete (GTK_FILE_SELECTION(w->File_sel), "*.eps");
		else if (type == EXPORT_PNG)
			gtk_file_selection_complete (GTK_FILE_SELECTION(w->File_sel), "*.png");
		else if (type == EXPORT_JPG)
			gtk_file_selection_complete (GTK_FILE_SELECTION(w->File_sel), "*.jpg");
		
#ifdef USE_GTK20
		gtk_editable_set_position (GTK_EDITABLE(GTK_FILE_SELECTION(w->File_sel)->selection_entry), 0);
#else
		gtk_entry_set_position (GTK_ENTRY(GTK_FILE_SELECTION(w->File_sel)->selection_entry), 0);
#endif		
		gtk_signal_connect (GTK_OBJECT (GTK_FILE_SELECTION(w->File_sel)->ok_button),
				    "clicked", GTK_SIGNAL_FUNC (gsgl_export_ok), w->Page);
		
		gtk_signal_connect_object
			(GTK_OBJECT (GTK_FILE_SELECTION (w->File_sel)->ok_button),
			 "clicked", GTK_SIGNAL_FUNC (gtk_widget_destroy), GTK_OBJECT (w->File_sel));
		gtk_signal_connect_object
			(GTK_OBJECT (GTK_FILE_SELECTION (w->File_sel)->cancel_button),
			 "clicked", GTK_SIGNAL_FUNC (gtk_widget_destroy), GTK_OBJECT (w->File_sel));
		gtk_widget_show (w->File_sel);
	}
}

typedef struct _entry_and_file {
	GtkWidget *text_entry;
	char *file_name;
	gsglPage *Page;
} entry_and_file;

static void gsgl_export_eps_message (GtkWidget *widget, gpointer gdata)
{
	entry_and_file *eaf = (entry_and_file *) gdata;
	char      *title = _("Export");
	char      *message = _("Exporting now ...");
	GtkWidget *window;
	GTimer    *timer;
	gulong    microseconds;
	double    seconds;
	const gchar     *entry_text;
	gchar     *command;
	FILE      *fp;
	char      tmp[] = "/tmp/gsglXXXXXX";
	int       fd;
	gsglWriteToFile *w;

	entry_text = gtk_entry_get_text(GTK_ENTRY(eaf->text_entry));
	
	fd = mkstemp (tmp);
	fp = fdopen (fd, "w"); 

	window = gsgl_message_window_new (title, message);

	timer = g_timer_new ();
	g_timer_start (timer);

	gtk_widget_show_now (window);
	
	while (gtk_events_pending())
		gtk_main_iteration();

	w = gsgl_init_write_to_file ();		
	gsgl_write_to_file (fp, w, eaf->Page);

	if (fclose (fp))
		gsgl_warning ("gsgl_export_eps_message", "failed to close tmp file");

	command = g_strdup_printf (entry_text, eaf->file_name, tmp);
	system (command);
	g_free (command);

	if (remove (tmp))
		gsgl_warning ("gsgl_export_eps_message", "failed to remove tmp file");
	
	seconds = g_timer_elapsed (timer, &microseconds);
	g_timer_stop (timer);
	
	if (seconds <= 0.1) usleep (100000 - microseconds);
	g_timer_destroy (timer);

	gtk_widget_hide (window);
	g_free (eaf->file_name);
	gsgl_free (eaf);
}

static void gsgl_export_command (char *file, gsglPage *Page)
{
	GtkWidget *window;
	GtkWidget *text;
	entry_and_file *eaf;

	eaf = (entry_and_file *) gsgl_malloc (sizeof(entry_and_file));
	text = gtk_entry_new ();

	if (mainwin->Type == EXPORT_EPS) {
		window = gsgl_dialog_new (_("Export EPS"), 10);
		gsgl_label_on_dialog_new (_("Enter the command line generates eps."
					    "\n('%s' will be replaced by a file name)"),
					  window, 10, 5);
		gtk_entry_set_text(GTK_ENTRY (text), gsgl_inq_eps_vec_command (Page->library));
	} else if (mainwin->Type == EXPORT_RASTER_EPS) {
		window = gsgl_dialog_new (_("Export Raster EPS"), 10);
		gsgl_label_on_dialog_new (_("Enter the command line generates eps."
					    "\n('%s' will be replaced by a file name)"),
					  window, 10, 5);
		gtk_entry_set_text(GTK_ENTRY (text), gsgl_inq_eps_ras_command (Page->library));
	} else if (mainwin->Type == EXPORT_PNG) {
		window = gsgl_dialog_new (_("Export PNG"), 10);
		gsgl_label_on_dialog_new (_("Enter the command line generates png."
					    "\n('%s' will be replaced by a file name)"),
					  window, 10, 5);
		gtk_entry_set_text(GTK_ENTRY (text), gsgl_inq_png_command (Page->library));
	} else if (mainwin->Type == EXPORT_JPG) {
		window = gsgl_dialog_new (_("Export JPEG"), 10);
		gsgl_label_on_dialog_new (_("Enter the command line generates jpeg."
					    "\n('%s' will be replaced by a file name)"),
					  window, 10, 5);
		gtk_entry_set_text(GTK_ENTRY (text), gsgl_inq_jpg_command (Page->library));
	} else {
		return;
	}
	if (gsgl_inq_mode (Page->library) == GSGL_MODE_WINDOW)
			gtk_window_set_transient_for (GTK_WINDOW(window), GTK_WINDOW(gsgl_gtk_inq_appwindow ()));
	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (window)->vbox), text, FALSE, TRUE, 10);

	eaf->text_entry = text;
	eaf->file_name = file;
	eaf->Page = Page;
	
	gtk_signal_connect (GTK_OBJECT (text), "activate",  GTK_SIGNAL_FUNC(gsgl_export_eps_message), eaf);
	gtk_signal_connect (GTK_OBJECT (text), "activate", GTK_SIGNAL_FUNC (gsgl_closedialog), window);
	gtk_widget_show (text);

	gsgl_button_on_dialog_new (_("OK"), window, gsgl_export_eps_message, eaf, TRUE);
	gsgl_cancelbutton_on_dialog_new (window, FALSE);

	gtk_widget_show (window);
	gtk_grab_add (window);
}

void gsgl_closedialog (GtkWidget *widget, gpointer gdata)
{
	gtk_grab_remove (GTK_WIDGET (gdata));
	gtk_widget_destroy (GTK_WIDGET (gdata));
	/*return GSGL_FALSE;*/
}

void gsgl_gtk_print (gpointer data, guint callback_action, GtkWidget *widget)
{
	GtkWidget *window;
	GtkWidget *text;
	gsgllist *list = NULL;
	gsglMainWindow *w = (gsglMainWindow *) data;

	if (gsgl_inq_i_am_a_server (w->Library)) {
		gsgl_tcp_server_send_command (GSGL_TCP_GUI_PRINT, w->Library);
	} else {
		window = gsgl_dialog_new (_("Print ..."), 10);
		gsgl_label_on_dialog_new (_("Enter the command line print a gsgl file.\n('%s' "
					    "will be replaced by a file name)"), window, 10, 5);
		text = gtk_entry_new ();
		gtk_entry_set_text (GTK_ENTRY (text), gsgl_inq_print_command (w->Library));
		gtk_box_pack_start (GTK_BOX (GTK_DIALOG (window)->vbox), text, FALSE, TRUE, 10);

		list = gsgl_list_append (list, text);
		list = gsgl_list_append (list, w->Page);

		gtk_signal_connect (GTK_OBJECT(text), "activate", GTK_SIGNAL_FUNC (gsgl_print_exec), list);
		gtk_signal_connect (GTK_OBJECT (text), "activate", GTK_SIGNAL_FUNC (gsgl_closedialog), window);
		gtk_widget_show (text);

		gsgl_button_on_dialog_new (_("OK"), window, gsgl_print_exec, list, TRUE);
		gsgl_cancelbutton_on_dialog_new (window, FALSE);
	
		gtk_widget_show (window);
		gtk_grab_add (window);
	}
}

void gsgl_gtk_dumpwin (gpointer data, guint callback_action, GtkWidget *widget)
{
	gsglMainWindow *w = (gsglMainWindow *) data;

	if (gsgl_inq_i_am_a_server (w->Library)) {
		gsgl_tcp_dump_window (w->Win, w->Library);
	} else {
		gsgl_dumpwin (w->Win, w->Page);
	}
}

void gsgl_gtk_select_obj (gpointer data, guint callback_acion, GtkWidget *widget)
{	
	gsglMainWindow *w = (gsglMainWindow *) data;

	gsgl_gui_pointer (w->Page, GSGL_TRUE);
	gsgl_yes_select_objects (w->Page);
	gui_cursor_default (w);
}

void gsgl_manual (gpointer data, guint action, GtkWidget *widget)
{
	gsglMainWindow *w = (gsglMainWindow *) data;

	system (gsgl_inq_man_command (w->Library));
}

void gsgl_gtk_rotate_mouse (gpointer gdata, guint with_question, GtkWidget *widget)
{
	gsglMainWindow *w = (gsglMainWindow *) gdata;

	if (gsgl_2d_is_selected (w->Page)) {
		gsgl_gtk_2d_rotate_mouse (gdata, with_question, widget);
	} else if (gsgl_3d_is_selected (w->Page)) {
		gsgl_gtk_3d_rotate_mouse (gdata, with_question, widget);
	} else if (gsgl_inq_3dobjects_exist (w->Page)) {
		gsgl_3d_select_objects_without_color (w->Page);
		gsgl_gtk_3d_rotate_mouse (gdata, with_question, widget);
	} else {
		gsgl_2d_select_objects_without_color (w->Page);
		gsgl_gtk_2d_rotate_mouse (gdata, with_question, widget);
	}
}

void gsgl_gtk_scale_mouse (gpointer gdata, guint with_question, GtkWidget *widget)
{
	gsglMainWindow *w = (gsglMainWindow *) gdata;

	if (gsgl_2d_is_selected (w->Page)) {
		gsgl_gtk_2d_scale_mouse (gdata, with_question, widget);
	} else if (gsgl_3d_is_selected (w->Page)) {
		gsgl_gtk_3d_scale_mouse (gdata, with_question, widget);
	} else if (gsgl_inq_3dobjects_exist (w->Page)) {
		gsgl_3d_select_objects_without_color (w->Page);
		gsgl_gtk_3d_scale_mouse (gdata, with_question, widget);
	} else {
		gsgl_2d_select_objects_without_color (w->Page);
		gsgl_gtk_2d_scale_mouse (gdata, with_question, widget);
	}
}

void gsgl_gtk_trans_xy_mouse (gpointer gdata, guint action, GtkWidget *widget)
{
	gsglMainWindow *w = (gsglMainWindow *) gdata;

	if (gsgl_2d_is_selected (w->Page)) {
		gsgl_gtk_2d_trans_mouse (gdata, action, widget);
	} else if (gsgl_3d_is_selected (w->Page)) {
		gsgl_gtk_3d_trans_xy_mouse (gdata, action, widget);
	} else if (gsgl_inq_3dobjects_exist (w->Page)) {
		gsgl_3d_select_objects_without_color (w->Page);
		gsgl_gtk_3d_trans_xy_mouse (gdata, action, widget);
	} else {
		gsgl_2d_select_objects_without_color (w->Page);
		gsgl_gtk_2d_trans_mouse (gdata, action, widget);
	}
}

void gsgl_gtk_trans_z_mouse (gpointer gdata, guint action, GtkWidget *widget)
{
	gsglMainWindow *w = (gsglMainWindow *) gdata;

	if (gsgl_2d_is_selected (w->Page)) {
	} else if (gsgl_3d_is_selected (w->Page)) {
		gsgl_gtk_3d_trans_z_mouse (gdata, action, widget);
	} else if (gsgl_inq_3dobjects_exist (w->Page)) {
		gsgl_3d_select_objects_without_color (w->Page);
		gsgl_gtk_3d_trans_z_mouse (gdata, action, widget);
	} else {
	}
}

void gsgl_quit_dialog (gpointer gdata, guint acion, GtkWidget *widget)
{
	gsglMainWindow *w = (gsglMainWindow *) gdata;
	GtkWidget *window;
	
	if (gsgl_inq_fullscreen (w->Library))
		gsgl_quitapp (NULL, w->Page);

	window = gsgl_dialog_new (_("Quit?"), 5);	
	gsgl_label_on_dialog_new (_("Are you sure to quit?"), window, 10, 10);

	gsgl_button_on_dialog_new (_("OK"), window, gsgl_quitapp, w->Page, TRUE);
	gsgl_cancelbutton_on_dialog_new (window, FALSE);

	gtk_widget_show (window);
	gtk_grab_add (window);
}

void gsgl_help (gpointer gdata, guint acion, GtkWidget *widget)
{
	GtkWidget *window;
	GtkWidget *text;
	GtkWidget *button;
	char buffer[] = "    c           : continue.\n"
		"    return      : same as c.\n"
		"    n           : same as c.\n" 
		"    a           : open file and save this page.\n"
		"    s           : save this page, open file if not yet.\n"
		"    y           : save this page and close window.\n" 
		"    d           : dump dot image. (x)\n"
		"    h           : this informations.\n"
		"    p           : print out this page.\n"	
		"    q           : quit run.";
#ifdef USE_GTK20
	GtkTextBuffer *textbuffer;
#endif
		
	window = gsgl_dialog_new (_("Key bindings"), 5);
	gtk_widget_set_usize (window, 400, 250);

#ifdef USE_GTK20
	text = gtk_text_view_new ();
	textbuffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (text));
	gtk_text_buffer_set_text (textbuffer, buffer, -1);
	gtk_text_view_set_editable (GTK_TEXT_VIEW(text), FALSE);
#else	
	text = gtk_text_new (NULL, NULL);
	gtk_text_set_editable (GTK_TEXT (text), FALSE);
	gtk_text_insert (GTK_TEXT (text), NULL, NULL, NULL, buffer, -1);
#endif
	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (window)->vbox), text, TRUE, FALSE, 0);

	gtk_box_set_homogeneous (GTK_BOX(GTK_DIALOG(window)->action_area), FALSE);
	button = gtk_button_new_with_label (_("OK"));
	gtk_signal_connect (GTK_OBJECT (button), "clicked", GTK_SIGNAL_FUNC (gsgl_closedialog), window);
	gtk_box_pack_end (GTK_BOX(GTK_DIALOG(window)->action_area), button, FALSE, FALSE, 0);
	GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);
	gtk_widget_grab_default (button);
	
	gtk_widget_show (text);
	gtk_widget_show (button);
	gtk_widget_show (window);
}

void gsgl_about (gpointer gdata, guint acion, GtkWidget *widget)
{
	GtkWidget *window;
	GtkWidget *text;
	GtkWidget *button;
	GtkWidget *label;
	GtkWidget *separator;
	char *version;
	char buffer[] = 
		"Copyright of GSAF system: A. Fukuyama, Kyoto University\n"
		"Copyright of GSAF 3D package (A*.f): T. Akutsu, Kyoto University\n"
		"Copytight of adb_ill_1.1: Adobe Systems, inc\n"
		"pgfont data was derived from Hershey character set used in PGPLOT.\n"
		"\n"
		"This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License\n"
		" as published by the Free Software Foundation; either version 2, or (at your option) any later version.\n"
		"\n"
		"This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;\n"
		"without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n"
		" See the GNU General Public License for more details.\n"
		"\n"
		"You should have received a copy of the GNU General Public License along with this program;\n"
		" if not, write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.";
#ifdef USE_GTK20
	GtkTextBuffer *textbuffer;
#endif

	window = gsgl_dialog_new (_("About"), 10);
	gtk_window_set_default_size (GTK_WINDOW(window), 500, 300);

	gtk_box_set_homogeneous (GTK_BOX(GTK_DIALOG(window)->vbox), FALSE);
	label = gtk_label_new ("GSAF/OpenGL");
	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (window)->vbox), label, FALSE, FALSE, 0);
	gtk_widget_show (label);
	version = g_strdup_printf ("version %s", GSGL_VERSION);
	label = gtk_label_new (version);
	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (window)->vbox), label, FALSE, FALSE, 0);
	gtk_widget_show (label);
	label = gtk_label_new ("Copyright (C) 2001-2004 AKUTSU Taku, FUKUYAMA Atsushi");
	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (window)->vbox), label, FALSE, FALSE, 0);
	gtk_widget_show (label);
	label = gtk_label_new ("<akutsu@p-grp.nucleng.kyoto-u.ac.jp>");
	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (window)->vbox), label, FALSE, FALSE, 0);
	gtk_widget_show (label);
	label = gtk_label_new ("http://p-grp.nucleng.kyoto-u.ac.jp/~akutsu/gsgl.html");
	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (window)->vbox), label, FALSE, FALSE, 0);
	gtk_widget_show (label);

	separator = gtk_hseparator_new ();
	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (window)->vbox), separator, FALSE, FALSE, 5);
	gtk_widget_show (separator);
	separator = gtk_hseparator_new ();
	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (window)->vbox), separator, FALSE, FALSE, 0);
	gtk_widget_show (separator);
	
#ifdef USE_GTK20
	text = gtk_text_view_new ();
	textbuffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (text));
	gtk_text_buffer_set_text (textbuffer, buffer, -1);
	gtk_text_view_set_editable (GTK_TEXT_VIEW(text), FALSE);
#else
	text = gtk_text_new (NULL, NULL);
	gtk_text_set_editable (GTK_TEXT (text), FALSE);
	gtk_text_insert (GTK_TEXT (text), NULL, NULL, NULL, buffer, -1);
#endif
	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (window)->vbox), text, FALSE, FALSE, 10);

	gtk_box_set_homogeneous (GTK_BOX(GTK_DIALOG(window)->action_area), FALSE);
	button = gtk_button_new_with_label (_("OK"));
	gtk_signal_connect (GTK_OBJECT (button), "clicked", GTK_SIGNAL_FUNC (gsgl_closedialog), 
			    window);
	gtk_box_pack_end (GTK_BOX(GTK_DIALOG(window)->action_area), button, FALSE, FALSE, 0);
	GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);
	gtk_widget_grab_default (button);
	
	gtk_widget_show (text);
	gtk_widget_show (button);
	gtk_widget_show (window);
}
