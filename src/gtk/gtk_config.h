#ifndef __gtk_config_h__
#define __gtk_config_h__

#ifdef __cplusplus
extern "C"{
#endif

void gsgl_gtk_config_color (gpointer gdata, guint action, GtkWidget *widget);
void gsgl_gtk_config_command (gpointer gdata, guint action, GtkWidget *widget);
void gsgl_gtk_config_ttf (gpointer gdata, guint action, GtkWidget *widget);
void gsgl_gtk_config_window (gpointer gdata, guint action, GtkWidget *widget);
void gsgl_gtk_draw_area (GtkWidget *area, double *color);

#ifdef __cplusplus
}
#endif

#endif
