#ifndef __tcp_guiitems_h
#define __tcp_guiitems_h

#ifdef __cplusplus
extern "C"{
#endif

void gsgl_tcp_guiitems_init_every_page (void);
void gsgl_tcp_guiitems_finish_every_page (void);
void gsgl_tcp_guiitems_callback (unsigned int id, void *data, gsglPage *);
void gsgl_tcp_gui_button_clicked (unsigned int id, gsglPage *);
void gsgl_tcp_gui_combo_changed (unsigned int id, int item, gsglPage *);
void gsgl_tcp_gui_spinf_valuechanged (unsigned int id, float value, gsglPage *);
void gsgl_tcp_gui_spini_valuechanged (unsigned int id, int value, gsglPage *);
void gsgl_tcp_gui_check_toggled (unsigned int id, int active, gsglPage *);
void gsgl_tcp_gui_edit_changed (unsigned int id, const char *text, gsglPage *);
void gsgl_tcp_gui_add_text (char *str, float x, float y, int fnt, float size, float angle, int ijust, gsglPage *p);
void gsgl_tcp_begin_objects_after_pagee (gsglPage *p);
void gsgl_tcp_end_objects_after_pagee (gsglPage *p);

#ifdef __cplusplus
}
#endif

#endif

