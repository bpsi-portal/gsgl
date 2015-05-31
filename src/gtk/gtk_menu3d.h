#ifndef __gtk_menu3d_h__
#define __gtk_menu3d_h__

#ifdef __cplusplus
extern "C"{
#endif

void gui_3d_init_every_page (void);
void gui_3d_sensitive (gboolean torf);
void gsgl_gtk_3d_restore (gpointer g_data, guint action, GtkWidget *widget);
void gsgl_gtk_3d_trans_z_mouse (gpointer gdata, guint action, GtkWidget *widget);
void gsgl_gtk_3d_trans_xy_mouse (gpointer gdata, guint action, GtkWidget *widget);
void gsgl_gtk_3d_scale_mouse (gpointer gdata, guint action, GtkWidget *widget);
void gsgl_gtk_3d_scale_percent (GtkWidget *widget, gpointer gdata);
void gsgl_gtk_3d_rotate_mouse (gpointer gdata, guint action, GtkWidget *widget);
void gsgl_gtk_lighting_toggle (gpointer d, guint type, GtkWidget *w);
void gsgl_gtk_3d_select_obj (gpointer gdata, guint acion, GtkWidget *widget);

#ifdef __cplusplus
}
#endif

#endif
