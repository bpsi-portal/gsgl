#ifndef _gtk_gsglwindow_h
#define _gtk_gsglwindow_h

#define SENSITIVE(A,B,C) gtk_widget_set_sensitive(gtk_item_factory_get_widget(A,B),C)

#ifdef __cplusplus

#include <gtk/gtk.h> /* fix me */
#define GC_NAME_CONFLICT
#include "config.h"
#ifdef HAVE_GCDIR
#  include "gc/gc_cpp.h"
#else
#  include "gc_cpp.h"
#endif

class gsglMainWindow : public gc
{
 public:
	gsglMainWindow (XVisualInfo *vi, GtkItemFactoryEntry item[], int, GtkItemFactoryEntry pitem[], int, gsglWindow *win);
	~gsglMainWindow () {};
	gsglPage *Page;
	gsglWindow *Win;
	gsglLibrary *Library;
	GtkWidget *Appwindow;
	GtkWidget *Glarea;
	GtkAccelGroup  *Accel1;
	GtkAccelGroup  *Accel2;
	GtkItemFactory *GMenu;
	GtkItemFactory *GPopup;
	unsigned int Saving_flag;
	GtkWidget      *File_sel;
	int            Type;
	unsigned int Handler_anime;
	int Xkey;
	int Ykey;
	int Xstart;
	int Ystart;
	gsgllist *list0;
	gsglboolean Key_is_pressed;
	gsglboolean have_accel;
	gsglboolean Add_text;
	gsgllist *Add_text_dialog;
	gsglboolean Add_line;
	gsgllist *Add_line_dialog;
	void set_page (gsglPage *p);
	void set_up_gui (gsglboolean b);
	void gsgl_quit_dialog (gpointer gdata, guint acion, GtkWidget *widget);
	void gui_anime_sensitive (gsglboolean b);
	void gui_anime_sensitive_pause (gsglboolean b);
	void gui_anime_sensitive_replay (gsglboolean b);
};

gsglMainWindow * gsgl_inq_mainwin ();
void gsgl_utility_clean (GtkWidget *w, gpointer gdata);
void gsgl_gtk_set_sensitive_have_3d (gsglboolean b, GtkItemFactory *GMenu, GtkItemFactory *GPopup);

extern "C"{
#endif

void gsgl_make_mainwindow_tcp_mode (gsglWindow *win);
void gsgl_gtk_save (gpointer data, unsigned int saving_flag, GtkWidget *widget);
void gsgl_saving_message (gsglPage *p);
void gsgl_gtk_set_page (gsglPage *p);
Window gsgl_gtk_make_mainwin (XVisualInfo *vi, GtkItemFactoryEntry items[], int, GtkItemFactoryEntry pitems[], int, gsglWindow *win);
GtkWidget * gsgl_gtk_inq_appwindow ();
GtkWidget * gsgl_gtk_inq_glarea ();
void gsgl_spin_value_x (GtkAdjustment *adjustment, GtkWidget *spin);
void gsgl_spin_value_y (GtkAdjustment *adjustment, GtkWidget *spin);
void gsgl_spin_value_z (GtkAdjustment *adjustment, GtkWidget *spin);
void gsgl_gtk_bool_selection (gsglboolean b);
void gui_sensitive_have_3d (gsglboolean b);
void gsgl_fileselec_ok (GtkWidget *w, gpointer data);
void gsgl_closedialog (GtkWidget *widget, gpointer gdata);
void gsgl_gtk_save_and_close (gpointer gdata, unsigned int saving_flag, GtkWidget *widget);
void gsgl_gtk_save_this_frame (gpointer gdata, guint action, GtkWidget *widget);
void gsgl_message_and_ok_button (char *title, char *message);
void gsgl_quit_dialog (gpointer gdata, guint acion, GtkWidget *widget);
void gsgl_help (gpointer gdata, guint acion, GtkWidget *widget);
void gsgl_about (gpointer gdata, guint acion, GtkWidget *widget);
void gsgl_gtk_trans_z_mouse (gpointer gdata, guint action, GtkWidget *widget);
void gsgl_gtk_trans_xy_mouse (gpointer gdata, guint action, GtkWidget *widget);
void gsgl_gtk_scale_mouse (gpointer gdata, guint with_question, GtkWidget *widget);
void gsgl_gtk_rotate_mouse (gpointer gdata, guint with_question, GtkWidget *widget);
void gsgl_manual (gpointer data, guint action, GtkWidget *widget);
void gsgl_gtk_dumpwin (gpointer data, guint callback_action, GtkWidget *widget);
void gsgl_gtk_select_obj (gpointer data, guint callback_acion, GtkWidget *widget);

#ifdef __cplusplus
}
#endif

#endif
