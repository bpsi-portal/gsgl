#ifndef __ogl_h__
#define __ogl_h__

#ifdef __cplusplus
extern "C"{
#endif

void gsgl_doublebuffer_is_available (gsglLibrary *);
void gsgl_doublebuffer_is_unavailable (gsglLibrary *);
gsglboolean gsgl_doublebuffer_is_enabled (gsglWindow *w);
void gsgl_doublebuffer_enable (gsglWindow *w);
void gsgl_doublebuffer_disable (gsglWindow *w);
void gsgl_polygonoffset_enable (gsglWindow *w);
void gsgl_polygonoffset_disable (gsglWindow *w);
void gsgl_depthtest_enable (gsglWindow *w);
void gsgl_depthtest_disable (gsglWindow *w);
void gsgl_erase (gsglPage *);
void gsgl_init_gl_once_in_program (gsglWindow *);
void gsgl_init_gl_every_page (gsglPage *);
void gsgl_redraw (gsglPage *p);
int gsgl_redraw_with_name (int x, int y, int w, int h, gsglPage *);
GLuint * gsgl_inq_select_buffer (gsglWindow *);
void gsgl_free_select_buffer (gsglWindow *);
void gsgl_allocate_select_buffer (gsglWindow *);
void gsgl_enable_antialiasing (gsglWindow *w);
void gsgl_disable_antialiasing (gsglWindow *w);
/*gsglboolean gsgl_inq_antialiasing (void);*/
gsglboolean gsgl_inq_antialiasing_now (gsglWindow *w);

#ifdef __cplusplus
}
#endif

#endif
