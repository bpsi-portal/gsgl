#ifndef _gtk_anime_h
#define _gtk_anime_h

#ifdef __cplusplus
extern "C"{
#endif

void gsgl_gtk_anime_backward (gpointer gdata, guint action, GtkWidget *widget);
void gsgl_gtk_anime_forward (gpointer gdata, guint action, GtkWidget *widget);
void gsgl_gtk_anime_pause (gpointer gdata, guint action, GtkWidget *widget);
void gsgl_gtk_anime_reverse_replay (gpointer gdata, guint action, GtkWidget *widget);
void gsgl_gtk_anime_replay (gpointer gdata, guint action, GtkWidget *widget);

#ifdef __cplusplus
}
#endif

#endif
