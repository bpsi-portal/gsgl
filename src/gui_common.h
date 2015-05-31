
#ifndef __gui_common_h__
#define __gui_common_h__

#include "gsgltype.h"
#include "fileformat.h"
#include "libserver.h"

#ifdef __cplusplus
extern "C"{
#endif

char * gsgl_window_title ();
void gsgl_begin_guiitem (gsglPage *p);
void gsgl_end_guiitem (gsglPage *p);
void gsgl_servercallback_reset (void);
void gsgl_servercallback_set (gsglPage * (*) (server_data *, gsglPage *, void (*)(unsigned int, void *, gsglPage *, READ_TOKEN)));
gsglPage * (*gsgl_inq_servercallback(void))(server_data *data, gsglPage *, void (*)(unsigned int, void *, gsglPage *, READ_TOKEN));
void gsgl_gui_set_page (gsglPage *p);
void gsgl_set_up_gui (gsglPage *p);
void gsgl_gui_quit (void (*func_exit)(int), gsglPage *p);
void gsgl_cursor_init_every_page (void);
void gsgl_gui_add_text_client (char *str, float x, float y, int fnt, float size, float angle, int ijust, gsglPage *p);

#ifdef __cplusplus
}

class gsglMainWindow;
void gsgl_gui_keypress (int value, int shift, gsglboolean *key_is_pressed, int *xkey, int *ykey, gsglMainWindow *w);
void gsgl_gui_keyrelease (int value, int *key_is_pressed, int *xkey, int *ykey, gsglPage *page);
void gsgl_gui_window_configure (unsigned int width, unsigned int height, gsglPage *page);
void gsgl_gui_mouse_motion (int x, int y, int *xstart, int *ystart, gsglboolean left_or_right, gsglPage *page);
void gsgl_gui_button_press (int x, int y, int button_press, gsglboolean left, gsglboolean right, gsglboolean mid, int *xstart, int *ystart, gsglPage *page, gsglMainWindow *mw);
void gsgl_gui_button_release (int x, int y, int left, int right, int mid, 
			      int *xstart, int *ystart, gsglPage *page);
void gui_replay_frame (gsglMainWindow *w);
void gui_reverse_replay_frame (gsglMainWindow *w);
void gui_anime_forward (gsglMainWindow *w);
void gui_anime_backward (gsglMainWindow *w);
void gui_cursor_default (gsglMainWindow *w);
void gui_cursor_rotate (gsglMainWindow *w);
void gui_cursor_translate (gsglMainWindow *w);
void gui_cursor_translate_z (gsglMainWindow *w);
void gui_cursor_scale (gsglMainWindow *w);
void gui_cursor_pencil (gsglMainWindow *w);

#endif

#endif
