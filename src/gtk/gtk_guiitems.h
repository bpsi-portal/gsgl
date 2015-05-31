#ifndef __gtk_guiitems_h__
#define __gtk_guiitems_h__

#ifdef __cplusplus
extern "C"{
#endif

void gui_add_button_user (int x, int y, int w, int h, guialldata *);
void gui_add_label_user (int x, int y, int w, int h, guialldata *);
void gui_inq_label_size (const char *text, float *w, float *h, gsglPage *);
void gui_add_combobox_user (int x, int y, int w, int h, guialldata *);
void gui_add_spinbuttonf_user (int x, int y, int w, int h, guialldata *);
void gui_add_spinbuttoni_user (int x, int y, int w, int h, guialldata *);
void gui_add_checkbutton_user (int x, int y, int w, int h, guialldata *);
void gui_add_lineedit_user (int x, int y, int w, int h, guialldata *);
void gui_guiitems_init_every_page (void);
void gui_guiitems_finish_every_page (gsglPage *);
void gui_guiitems_configure (gsglPage *);
char * gui_popup_edit (char *, char *);
float gui_popup_spinf (int digits, float init, float lower, float upper, gsglPage *);
int gui_popup_spini (int init, int lower, int upper, gsglPage *);
void gui_exit_loop (void);

#ifdef __cplusplus
}
#endif

#endif
