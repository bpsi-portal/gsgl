
#include "config.h"
#ifdef USE_INLINE

#include "gsaf.h"
#include <stdlib.h>
#include <stdio.h>
#include <termios.h>
#include <string.h>
#include <signal.h>
#include "utils.h"
#include "inline.h"
#include "xwin.h"
#include "fileio_base.h"
#include "fileio_write.h"
#include "bsc1.h"
#include "iof.h"
#include "configfile.h"
#include "osmesa.h"

static struct termios stored_settings;
static gsglinline *Term = NULL;

static void store_default_key (void)
{
	tcgetattr(0,&stored_settings);
}

static void set_keypress(void)
{
    struct termios new_settings;

    new_settings = stored_settings;

    new_settings.c_lflag &= (~ICANON);
    new_settings.c_lflag &= (~ECHO);
    new_settings.c_cc[VTIME] = 0;
    new_settings.c_cc[VMIN] = 1;

    tcsetattr(0,TCSANOW,&new_settings);
    return;
}

static void reset_keypress(void)
{
    tcsetattr(0,TCSANOW,&stored_settings);
    return;
}

static void clear_inline (void)
{
	if (Term != NULL) {
		XClearArea(Term->d, Term->term, 0, 0, Term->width, Term->height, GSGL_TRUE);
		XFlush (Term->d);
	}
}

static void exit_inline (int r)
{
	clear_inline ();
	reset_keypress ();
	gsgl_close_message ();
	exit (r);
}

static void clear_and_exit_inline (int sig)
{
	exit_inline (0);
}

static void init_signal_inline (void)
{
	signal (SIGINT, clear_and_exit_inline);
	signal (SIGTERM, clear_and_exit_inline);
}

void gsgl_init_inline_mode (void)
{
	int revert_to;
	XWindowAttributes attr;
	unsigned int nchildren, nchildren2;
	Window root, *children, parent2, *children2;
	int i;
	int width, height;

	store_default_key ();

	Term = (gsglinline *) gsgl_malloc (sizeof(gsglinline));

	if((Term->d = XOpenDisplay (NULL)) == NULL) {
		gsgl_error ("dvopen", "Can't open display");
	}
	XGetInputFocus (Term->d, &(Term->focus), &revert_to);
	Term->term = Term->focus;
	XGetWindowAttributes (Term->d, Term->focus, &attr);
	
	Term->width = attr.width;
	Term->height = attr.height;

	while (1) {
		Window p_window;

		XQueryTree(Term->d, Term->term, &root, &(Term->parent),
			   &children, &nchildren);
		XGetWindowAttributes(Term->d, Term->term, &attr);
		p_window = Term->term;
		for (i = 0; i < nchildren; i++) {
			XGetWindowAttributes(Term->d, children[i], &attr);
			if (attr.width > Term->width * 0.7 &&
			    attr.height > Term->height * 0.7) {
				/* maybe text window */
				Term->width = attr.width;
				Term->height = attr.height;
				Term->term = children[i];
			}
		}
		if (p_window == Term->term)
			break;
	}

	XQueryTree(Term->d, Term->focus, &root, &parent2,
		   &children2, &nchildren2);

	for (i = 0; i < nchildren2; i++) {
		XGetWindowAttributes(Term->d, children2[i], &attr);
		if (attr.x > Term->width * 0.8 && attr.x < Term->width &&
		    attr.height > Term->height * 0.7) {
			/* scrollbar of Eterm (right hand side) ? */
			Term->width -= attr.width + attr.border_width * 2;
			break;
		}
	}
	
	Term->offset_x = 0;
	for (i = 0; i < nchildren; i++) {
		XGetWindowAttributes(Term->d, children[i], &attr);
		if (attr.x <= 0 && attr.width < 30 && attr.height > Term->height * 0.7) {
			/* scrollbar of xterm/kterm ? */
			Term->offset_x += attr.x + attr.width + attr.border_width * 2;
			break;
		}
	}
	Term->offset_y = 0;

	/* menubar of Eterm */
	XGetWindowAttributes(Term->d, Term->term, &attr);
	Term->x = attr.x;
	Term->y = attr.y;
	Term->height -= attr.y;

	width = Term->width-Term->offset_x;
	height = Term->height-Term->offset_x;

	if ((float) width/height > (float) GSGLBASIC_WINDOWWIDTH/GSGLBASIC_WINDOWHEIGHT) {
		width = height * ((float) GSGLBASIC_WINDOWWIDTH/GSGLBASIC_WINDOWHEIGHT);
	} else {
		height = width * ((float) GSGLBASIC_WINDOWHEIGHT/GSGLBASIC_WINDOWWIDTH);
	}
	gsgl_set_window_width (width);
	gsgl_set_window_height (height);

	init_signal_inline ();
}

gsglinline * gsgl_inq_inline (void)
{
	return Term;
}

static unsigned int mycolor (Display *d, char *name)
{
	Colormap cmap;
	XColor c0, c1;

	cmap = DefaultColormap (d, 0);
	XAllocNamedColor (d, cmap, name, &c1, &c0);

	return c1.pixel;
}

static void draw_inline (gsglinline *term, GC gc, unsigned long color);

void gsgl_page_end_inline (int4 *ich)
{
	GC gc;
	gsglinline *term;
	char c;
	int offset_x, offset_y;
	int bgmode;
	unsigned long color;
	XImage *img;
	int width, height;
	Visual *v;
	unsigned char *rgb_24;
	unsigned char *rgb_buffer;
	char *osmesa_buffer;
	int i, j;
	int bpp;

	term = gsgl_inq_inline ();
	gc = XCreateGC(term->d, term->parent, 0, NULL);
	
	offset_x = 0; /*(term->width - gsgl_inq_window_width()) / 2;*/
	offset_y = 0; /*(term->height - gsgl_inq_window_height()) / 2;*/

	bgmode = gsgl_inq_bg_color ();
	if (bgmode == GSGL_BACK_COLOR_BLACK) {
		color = mycolor (term->d, "black");
	} else {
		color = mycolor (term->d, "white");
	}

#ifdef HAVE_OSMESA
	width = gsgl_inq_window_width ();
	height = gsgl_inq_window_height ();
	v = DefaultVisual(term->d, DefaultScreen(term->d));
	/*printf ("%d %d\n", v->class, TrueColor);*/

	/*printf ("%d\n", DefaultDepth(term->d, 0));*/

	osmesa_buffer = gsgl_get_osmesa_buffer ();

	rgb_24 = (char *) gsgl_malloc_atomic (width * height * 3);
	for (i = 0; i < height; i++) {
		/*int max;
		  max = width * height - 1;*/
		for (j = 0; j < width; j++) {
			rgb_24[3*((height-i-1)*width+j)+0] = osmesa_buffer[4*(i*width+j)+0];
			rgb_24[3*((height-i-1)*width+j)+1] = osmesa_buffer[4*(i*width+j)+1];
			rgb_24[3*((height-i-1)*width+j)+2] = osmesa_buffer[4*(i*width+j)+2];
		}
	}
	if (DefaultDepth(term->d, 0) == 24) {
		rgb_buffer = (char *) gsgl_malloc_atomic (width * height * 4);
		for (i=0; i<width*height; i++) {
			rgb_buffer[4*i+0] = 0;
			rgb_buffer[4*i+1] = rgb_24[3*i+0];
			rgb_buffer[4*i+2] = rgb_24[3*i+1];
			rgb_buffer[4*i+3] = rgb_24[3*i+2];
		}
		bpp = 4;
	} else if (DefaultDepth(term->d, 0) == 16) {
		int  r0,r1,r2;

		rgb_buffer = (char *) gsgl_malloc_atomic (width * height * 2);
		for(i=0;i < width * height; i++){
			r0= rgb_24[3*i+0] / 8;
			r1= rgb_24[3*i+1] / 8;
			r2= rgb_24[3*i+2] / 8;
			if (r0 == 31 && r1 == 31 && r2 == 31) {
				rgb_buffer[2*i+0]=255;
				rgb_buffer[2*i+1]=255;
			} else {
				rgb_buffer[2*i+0]=r0<<3 | r1>>2;
				rgb_buffer[2*i+1]=(r1 & 0x03)<<6 | r2;
			}
			/*rgb_buffer[2*i+0] = r0 << 2 | r1 >> 3;
			  rgb_buffer[2*i+1]=(r1 & 0x07)<<5 | r2;*/
			/*rgb_buffer[2*i+0] = r0 << 3 | r1 >> 2;
			  rgb_buffer[2*i+1]=(r1 & 0x03)<<6 | r2 << 1;*/
		}
		bpp = 2;
	} else {
		gsgl_error ("", "depth = %d", DefaultDepth(term->d, 0));
	}

	img = XCreateImage(term->d, CopyFromParent, DefaultDepth(term->d, 0), ZPixmap, 0, 
			   rgb_buffer,  width, height, bpp*8, width*bpp);
        /* LSBFirst, MSBFirst */
	img->bitmap_bit_order = MSBFirst;
	img->byte_order = MSBFirst;

	if (img == None) gsgl_error ("", "img == None");
	term->pixmap = XCreatePixmap (term->d, term->parent, width, height, DefaultDepth(term->d, 0));
	XPutImage (term->d, term->pixmap, gc, img, 0, 0, 0, 0, width, height);
#endif
	
	draw_inline (term, gc, color);

	set_keypress ();

	while (1) {
		c = getchar ();
		c = tolower (c);
		switch (c) {
		case 's':
		case 'c':
		case 'q':
		case 'h':
			*ich = c;
			goto EXIT;
		case '\n':
			*ich = 'c';
			goto EXIT;
		case EOF:
			*ich = 'q';
			goto EXIT;
		case 'r':
			draw_inline (term, gc, color);
			break;
		default:
			break;
		}
	}
 EXIT:
	reset_keypress ();
	clear_inline ();
}

static void draw_inline (gsglinline *term, GC gc, unsigned long color)
{
	XSetForeground (term->d, gc, color);
	XFillRectangle (term->d, term->term, gc, 
			0/*term->x*/, 0/*term->y*/,
			term->width, term->height);

	XCopyArea(term->d, term->pixmap, term->term, gc, 0, 0,
		  gsgl_inq_window_width(),
		  gsgl_inq_window_height(),
		  term->offset_x/*+offset_x*/, term->offset_y/*+offset_y*/);
	XFlush (term->d);
}

#endif
