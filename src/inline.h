#ifndef __inline_h__
#define __inline_h__

struct _gsglinline {
	Display *d;
	int width;
	int height;
	int x;
	int y;
	Window focus;
	Window parent;
	Window term;
	int offset_x;
	int offset_y;
	Pixmap pixmap;
};

void gsgl_init_inline_mode (void);
gsglinline * gsgl_inq_inline (void);

#endif /* __inline_h__ */
