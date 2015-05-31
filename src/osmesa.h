
#ifndef __osmesa_h__
#define __osmesa_h__

#ifdef HAVE_OSMESA
void gsgl_make_drawable_osmesa (int width, int height);
char * gsgl_get_osmesa_buffer (void);
#endif

#endif /* __osmesa_h__ */
