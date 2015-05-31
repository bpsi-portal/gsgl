
#ifndef __gtk_menuedit_h__
#define __gtk_menuedit_h__

#ifdef __cplusplus
extern "C"{
#endif

void gsgl_gtk_add_text (gpointer gdata, guint action, GtkWidget *widget);
void gsgl_gtk_add_line (gpointer gdata, guint action, GtkWidget *widget);
void gsgl_add_text_dialog (float, float, gsglPage *p);
void gsgl_gui_close_add_text_dialog ();
void gsgl_gui_close_add_line_dialog ();

#ifdef __cplusplus
}
#endif

#endif
