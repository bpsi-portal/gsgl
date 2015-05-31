#ifndef __gtk_menupro_h__
#define __gtk_menupro_h__

#ifdef __cplusplus
extern "C"{
#endif

void gui_projection_init_every_page (void);
void gsgl_gtk_proj_restore (gpointer gdata, guint action, GtkWidget *widget);
void gsgl_gtk_proj_trans_mouse (gpointer gdata, guint action, GtkWidget *widget);
void gsgl_gtk_proj_trans_cm (GtkWidget *widget, gpointer gdata);
void gsgl_trans_proj_set_default (GtkWidget *widget, gpointer gdata);
void gsgl_gtk_proj_trans_value (gpointer gdata, guint action, GtkWidget *widget);
void gsgl_gtk_proj_scale (gpointer gdata, guint action, GtkWidget *widget);
void gsgl_gtk_proj_scale_percent (GtkWidget *widget, gpointer gdata);
void gsgl_scale_proj_set_default (GtkWidget *widget, gpointer gdata);
void gsgl_gtk_proj_scale_mouse (gpointer gdata, guint action, GtkWidget *widget);
void gsgl_gtk_proj_scale_value (gpointer gdata, guint action, GtkWidget *widget);
void gsgl_rotate_proj_set_default (GtkWidget *widget, gpointer gdata);
void gsgl_gtk_proj_rotate_value (gpointer gdata,guint action, GtkWidget *widget);
void gsgl_gtk_proj_rotate_mouse (gpointer gdata,guint action, GtkWidget *widget);
void gsgl_gtk_proj_rotate (GtkWidget *widget, gpointer gdata);
void gsgl_gtk_proj_rotate_degree (GtkWidget *widget, gpointer gdata);
void gsgl_gtk_sensitive_proj (gboolean torf);

#ifdef __cplusplus
}
#endif

#endif
