#ifndef __gtk_menu2d_h__
#define __gtk_menu2d_h__

#ifdef __cplusplus
extern "C"{
#endif

void gui_2d_init_every_page (void);
void gsgl_gtk_2d_restore (gpointer gdata, guint action, GtkWidget *widget);
void gsgl_gtk_2d_trans_mouse (gpointer gdata, guint action, GtkWidget *widget);
void gsgl_gtk_2d_trans_cm (GtkWidget *widget, gpointer gdata);
void gsgl_trans_2d_set_default (GtkWidget *widget, gpointer gdata);
void gsgl_gtk_2d_trans_value (gpointer gdata, guint action, GtkWidget *widget);
void gsgl_gtk_2d_scale (gpointer callback_data, guint action, GtkWidget *widget);
void gsgl_gtk_2d_scale_percent (GtkWidget *widget, gpointer gdata);
void gsgl_scale_2d_set_default (GtkWidget *widget, gpointer gdata);
void gsgl_gtk_2d_scale_mouse (gpointer gdata, guint action, GtkWidget *widget);
void gsgl_gtk_2d_scale_value (gpointer gdata, guint action, GtkWidget *widget);
void gsgl_rotate_2d_set_default (GtkWidget *widget, gpointer gdata);
void gsgl_gtk_2d_rotate_value (gpointer gdata, guint action, GtkWidget *widget);
void gsgl_gtk_2d_rotate_mouse (gpointer gdata, guint action, GtkWidget *widget);
void gsgl_gtk_2d_rotate (GtkWidget *widget, gpointer gdata);
void gsgl_gtk_2d_rotate_degree (GtkWidget *widget, gpointer gdata);
void gui_2d_sensitive (gboolean torf);
void gsgl_gtk_2d_select_obj (gpointer gdata, guint acion, GtkWidget *widget);

#ifdef __cplusplus
}
#endif

#endif
