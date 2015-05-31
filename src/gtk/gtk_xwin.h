#ifndef _gtk_xwin_h
#define _gtk_xwin_h

#ifdef __cplusplus
extern "C"{
#endif

void gsgl_gtk_set_up_gui (gsglPage *p);
void gui_init_tcp_mode (gsglLibrary *lib);
Window gui_make_drawable_window_mode (gsglWindow *w);
void gui_mainloop (gsglPage *p);

#ifdef __cplusplus
}
#endif

#endif
