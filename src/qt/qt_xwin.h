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
