#ifndef __xwin_h__
#define __xwin_h__

#ifdef __cplusplus
extern "C"{
#endif

void gsgl_bell (gsglLibrary *);
void gsgl_dumpwin (gsglWindow *, gsglPage *);
void gsgl_dumpwin_id (int, gsglPage *);
Display * gsgl_display (gsglLibrary *);
void gsgl_init_gc_pixmap_mode (gsglLibrary *);
void gsgl_init_gc_window_mode (gsglLibrary *);
void gsgl_init_gtk_tcp_mode (gsglLibrary *);
void gsgl_destroy_gc_pixmap_mode (gsglLibrary *);
void gsgl_destroy_gc_window_mode (gsglLibrary *);
void gsgl_make_drawable_pixmap_mode (gsglLibrary *);
void gsgl_make_drawable_window_mode (gsglWindow *);
void gsgl_raise_my_window (gsglLibrary *);
void gsgl_raise_my_window_fullscreen (gsglLibrary *);
void gsgl_lower_my_window (gsglLibrary *);
void gsgl_flush (gsglWindow *);
unsigned int gsgl_inq_window_width (gsglWindow *);
unsigned int gsgl_inq_window_height (gsglWindow *);
void gsgl_set_window_width (unsigned int width, gsglWindow *w);
void gsgl_set_window_height (unsigned int height, gsglWindow *w);
float gsgl_inq_x_ratio (gsglWindow *);
float gsgl_inq_y_ratio (gsglWindow *);
void gsgl_set_window_size (char c1, int *ich, gsglLibrary *);
void gsgl_x11_page_start (void);
gsglboolean gsgl_inq_fullscreen (gsglLibrary *);
int gsgl_inq_mode (gsglLibrary *);
gsglboolean gsgl_inq_userdefined_window (gsglLibrary *);
gsglboolean gsgl_inq_select_objects (gsglPage *);
void gsgl_yes_select_objects (gsglPage *);
void gsgl_no_select_objects (gsglPage *);
int inq_parent_window (gsglLibrary *);
float gsgl_get_screen_resolution (gsglLibrary *);
void gsgl_page_end_x11 (int *ich, gsglPage *p);
Window gsgl_inq_main_window (gsglWindow *w);

#ifdef __cplusplus
}
#endif

#endif
