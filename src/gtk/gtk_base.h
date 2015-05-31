#ifndef __gtk_base_h__
#define __gtk_base_h__

#define EXPORT_EPS          1
#define EXPORT_RASTER_EPS   2
#define EXPORT_PNG          3
#define EXPORT_JPG          4

#ifdef __cplusplus
extern "C"{
#endif

void gsgl_print_exec (GtkWidget *widget, gpointer gdata);
void gsgl_file_exist_dialog (char *file, gsglPage *Page);
void gsgl_file_open_and_save (GtkWidget *w, gpointer gdata);
void gsgl_quitapp (GtkWidget *widget, gpointer gdata);
Window gsgl_gtkwindow (XVisualInfo *vi, gsglWindow *w);
gint gsgl_destroy (GtkWidget *widget, gpointer gdata);
/*gint gsgl_closedialog (GtkWidget *widget, gpointer gdata);*/
gint gsgl_closedialog_cancel (GtkWidget *widget, gpointer gdata);
GtkWidget *gsgl_dialog_new (gchar *title, gint border);
void gsgl_button_on_dialog_new (gchar *title, GtkWidget *parent, void (*)(GtkWidget *, gpointer), gpointer gdata, gboolean grab_default);
void gsgl_button_on_dialog_new_without_close (gchar *title, GtkWidget *parent, void (*)(GtkWidget *, gpointer), gpointer gdata, gboolean grab_default);
void gsgl_cancelbutton_on_dialog_new (GtkWidget *parent, gboolean grab_default);
void gsgl_label_on_dialog_new (gchar *title, GtkWidget *parent, gint padding1, gint padding2);
void gsgl_gtk_spin_title (GtkWidget *w, gchar *title);
GtkWidget *gsgl_gtk_spin (GtkWidget *w, GtkObject *adjust, gchar *direc, gchar *unit, gfloat climb_rate, guint digits);
void gsgl_gtk_forbit_selection (void);
void gsgl_gtk_allow_selection (void);
void gsgl_gtk_save_as (gpointer gdata, unsigned int, GtkWidget *widget);
void gsgl_gtk_print (gpointer data, guint callback_action, GtkWidget *widget);
void gsgl_gtk_export (gpointer data, guint type, GtkWidget *widget);
void gsgl_gtk_dumpwin (gpointer data, guint callback_action, GtkWidget *widget);
void gsgl_gtk_set_dump_is_enable_or_not (void);
void gsgl_gtk_gettextize ();
GtkWidget * gsgl_message_window_new (char *title, char *message);

#ifdef __cplusplus
}
#endif

#define WITH_QUESTION 1
#define WITHOUT_QUESTION 0

#endif /* __gtk_base_h__ */
