#ifndef __qt_xwin_h
#define __qt_xwin_h

#ifdef __cplusplus
class gsglMainWindow;
gsglMainWindow * gsgl_inq_mainwin ();
#endif

#ifdef __cplusplus
extern "C"{
#endif

void gsgl_make_mainwindow_tcp_mode (gsglWindow *w);
Display * gsgl_qt_init_gc_window_mode (void);
float gsgl_qt_get_screen_resolution (void);
void gsgl_qt_get_screen_size (int *w, int *h);
void gsgl_qt_raise_window (void);
void gsgl_qt_lower_window (void);
void gsgl_qt_ensure_current (void);
void gsgl_qt_notify_gl_ready (void);
int gsgl_qt_is_gl_ready (void);
void gsgl_qt_wait_gl_ready (int timeout_ms);
void gsgl_qt_set_in_paint (int flag);
int gsgl_qt_is_in_paint (void);
int gsgl_qt_is_in_exec (void);
void gsgl_qt_request_present (void);
void gsgl_qt_process_events (int max_ms);
void gsgl_qt_shutdown (void);
Window gui_make_drawable_window_mode (gsglWindow *);
Window gsgl_qt_mainwindow_id (void);
void gui_mainloop (gsglPage *p);
void gui_exit_loop (void);
void gsgl_qt_bell (void);
void gsgl_qt_set_page (gsglPage *p);
void gui_init_tcp_mode (gsglLibrary *lib);

#ifdef __cplusplus
}
#endif

#endif
