#include "gsaf.h"
#include "gsgl.h"
#include "base.h"
#include "matrix.h"
#include "callback.h"
#include "device.h"
#include "xwin.h"
#include "utils.h"
#include "ogl.h"
#include "configfile.h"
#include "anime.h"
#include "tcpip.h"
#include "harddisk.h"
#include "guiitems.h"
#include "gui_common.h"
#ifdef USE_GTK
#    include <gtk/gtk.h>
#    include <gdk/gdk.h>
#    include <gdk/gdkx.h>
#    include "gtk_base.h"
#    include "gtk_menu2d.h"
#    include "gtk_menu3d.h"
#    include "gtk_menupro.h"
#    include "gtk_tcp.h"
#    include "gtk_xwin.h"
#    include "gtk_gsglwindow.h"
#    include "gtk_menuedit.h"
#endif
#if defined USE_QT
#    include "qt_xwin.h"
#    include "qt_menuedit.h"
#endif
#ifdef USE_INLINE
#include "inline.h"
#endif

#define GSGL_DEBUG(x)

gsglboolean gsgl_inq_select_objects (gsglPage *p)
{
	return p->Select_objects;
}

void gsgl_yes_select_objects (gsglPage *p)
{
	p->Select_objects = GSGL_TRUE;
}

void gsgl_no_select_objects (gsglPage *p)
{
	p->Select_objects = GSGL_FALSE;
}

gsglboolean gsgl_inq_userdefined_window (gsglLibrary *lib)
{
	return lib->Userdefined_window;
}

unsigned int gsgl_inq_window_width (gsglWindow *w)
{
	return w->WinWidth;
}

unsigned int gsgl_inq_window_height (gsglWindow *w)
{
	return w->WinHeight;
}

void gsgl_set_window_width (unsigned int width, gsglWindow *w)
{
	w->WinWidth = width;
	w->Deltax = (GLfloat) GSGL_PAPER_WIDTH / (GLfloat) w->WinWidth;
}

void gsgl_set_window_height (unsigned int height, gsglWindow *w)
{
	w->WinHeight = height;
	w->Deltay = (GLfloat) GSGL_PAPER_HEIGHT / (GLfloat) w->WinHeight;
}

float gsgl_get_screen_resolution (gsglLibrary *lib)
{
	float h = DisplayWidth (lib->Mydisplay, DefaultScreen(lib->Mydisplay)) / (float) DisplayWidthMM (lib->Mydisplay, DefaultScreen(lib->Mydisplay));
	float v = DisplayHeight (lib->Mydisplay, DefaultScreen(lib->Mydisplay)) / (float) DisplayHeightMM (lib->Mydisplay, DefaultScreen(lib->Mydisplay));	
	return sqrt(h * v) * 25.4;
}

float gsgl_inq_x_ratio (gsglWindow *w)
{
	return w->Deltax;
}

float gsgl_inq_y_ratio (gsglWindow *w)
{
	return w->Deltay;
}

int gsgl_inq_mode (gsglLibrary *lib)
{
	return lib->Mode;
}

Window gsgl_inq_main_window (gsglWindow *w)
{
	return w->Mywindow;
}

/*void gsgl_set_main_window (Window win)
  {
  Mywindow = win;
  }*/

void gsgl_set_window_size (char c1, int *ich, gsglLibrary *lib)
{
	char *str;

	lib->Fullscreen = GSGL_FALSE;
	lib->Userdefined_window = GSGL_FALSE; /* user defined window size (default = false) */

	switch (c1) {
	case '0': /* don't connect to x server */
		lib->Mode = GSGL_MODE_NO_GL;
		*ich = - *ich; /* interactive, but don't ask in saving */
		lib->winwidth0 = GSGLBASIC_WINDOWWIDTH;
		lib->winheight0 = GSGLBASIC_WINDOWHEIGHT;
		break;
	case '1':
		lib->Mode = GSGL_MODE_WINDOW;
		lib->winwidth0 = 512; 
		lib->winheight0 = 380; 
		break;
	case '2':
		lib->Mode = GSGL_MODE_WINDOW;
		lib->winwidth0 = 640;
		lib->winheight0 = 475; 
		break;
	case '3':
		lib->Mode = GSGL_MODE_WINDOW;
		lib->winwidth0 = 768;
		lib->winheight0 = 570; 
		break;
	case '4':
		lib->Mode = GSGL_MODE_WINDOW;
		lib->winwidth0 = 896;
		lib->winheight0 = 665; 
		break;
	case '5':
		lib->Mode = GSGL_MODE_WINDOW;
		lib->winwidth0 = 1024;
		lib->winheight0 = 760; 
		break;
	case '6':
		lib->Mode = GSGL_MODE_WINDOW;
		lib->winwidth0 = 1280;
		lib->winheight0 = 950; 
		break;
	case 'U':
	case 'u':
		lib->Mode = GSGL_MODE_WINDOW;
		lib->Userdefined_window = GSGL_TRUE;
		lib->winwidth0 = gsgl_inq_userdefined_winwidth (lib);
		lib->winheight0 = gsgl_inq_userdefined_winheight (lib);
		break;
	case 'F': 
	case 'f':
		lib->Mode = GSGL_MODE_WINDOW;
		lib->Fullscreen = GSGL_TRUE;
		break;
#ifdef USE_INLINE
	case 'I':
	case 'i':
		lib->Mode = GSGL_MODE_INLINE;
		gsgl_init_inline_mode ();
		break;
#endif
	case '!': /* postscript, png, jpeg, ... */ 
		lib->Mode = GSGL_MODE_PIXMAP;
		*ich = 0; 
		/**ich = - *ich;*/ /* what is this ? */
		lib->winwidth0 = GSGLBASIC_WINDOWWIDTH;
		lib->winheight0 = GSGLBASIC_WINDOWHEIGHT;
		str = getenv("GSGL_WIDTH");
		if (str) lib->winwidth0 = atoi (str);
		str = getenv("GSGL_HEIGHT");
		if (str) lib->winheight0 = atoi (str);
		break;
	default :
		lib->Mode = GSGL_MODE_WINDOW;
		lib->winwidth0 = GSGLBASIC_WINDOWWIDTH;
		lib->winheight0 = GSGLBASIC_WINDOWHEIGHT;
		break;
	}
}

gsglboolean gsgl_inq_fullscreen (gsglLibrary *lib)
{
	return lib->Fullscreen;
}

void gsgl_init_gc_pixmap_mode (gsglLibrary *lib)
{
	int  snglBuf[] = {GLX_RGBA, GLX_DEPTH_SIZE, 16, None};
	int  dblBuf[] = {GLX_RGBA, GLX_DEPTH_SIZE, 16, GLX_DOUBLEBUFFER, None};

	if((lib->Mydisplay = XOpenDisplay (NULL)) == NULL) {
		gsgl_error ("dvopen", "Can't open display");
	}     
	if(!glXQueryExtension (lib->Mydisplay, NULL, NULL)) {
		gsgl_error ("dvopen", "X server has no OpenGL GLX extension\n");
	}
	gsgl_doublebuffer_is_unavailable (lib);
	lib->Vi = glXChooseVisual (lib->Mydisplay, DefaultScreen(lib->Mydisplay), snglBuf);
	if (lib->Vi == NULL) {
		lib->Vi = glXChooseVisual (lib->Mydisplay, DefaultScreen(lib->Mydisplay), dblBuf);
		gsgl_doublebuffer_is_available (lib);
		if (lib->Vi == NULL)
			gsgl_error ("dvopen", "no RGB visual with depth buffer");
	}
	if(lib->Vi->class != TrueColor && lib->Vi->class != DirectColor) { 
		gsgl_error ("dvopen", "TrueColor visual required for this program");
	}
	lib->Cx = glXCreateContext (lib->Mydisplay, lib->Vi, None, GL_FALSE);
	if (lib->Cx == NULL) {
		gsgl_error ("dvopen", "Could not create rendering context");
	}
}

int inq_parent_window (gsglLibrary *lib)
{
	int  revert_to;
	char *id;
	Window pre;

	if ((id = getenv("GSGL_WINDOW_ID")) != NULL) {
		return atoi(id);
	} else if ((id = getenv("WINDOWID")) != NULL) {
		return atoi(id);
	} else {
		if (lib->Mydisplay == NULL) {
			if((lib->Mydisplay = XOpenDisplay (NULL)) == NULL) {
				gsgl_error ("dvopen", "Can't open display");
			}
		}
		XGetInputFocus (lib->Mydisplay, &pre, &revert_to);
		return (int) pre;
	}
}

void gsgl_init_gc_window_mode (gsglLibrary *lib)
{
#if defined USE_QT
	lib->Mydisplay = gsgl_qt_init_gc_window_mode ();
	if (lib->Mydisplay == NULL) 
		gsgl_error ("gsgl_init_gc_window_mode", "Can't open display\n");

#else
	int  snglBuf[] = {GLX_RGBA, GLX_DEPTH_SIZE, 16, None};
	int  dblBuf[] = {GLX_RGBA, GLX_DEPTH_SIZE, 16, GLX_DOUBLEBUFFER, None};

	gtk_set_locale();
	gtk_init (NULL, NULL);
	lib->Mydisplay = GDK_DISPLAY();
	
	if (lib->Mydisplay == NULL) {
		gsgl_error ("gsgl_init_gc_window_mode", "Can't open display\n");
	}	  
	if (!glXQueryExtension (lib->Mydisplay, NULL, NULL)) {
		gsgl_error ("gsgl_init_gc_window_mode", "X server has no OpenGL GLX extension");
	}
	gsgl_doublebuffer_is_available (lib);
	lib->Vi = glXChooseVisual (lib->Mydisplay, DefaultScreen(lib->Mydisplay), dblBuf);
	if (lib->Vi == NULL) {
		lib->Vi = glXChooseVisual (lib->Mydisplay, DefaultScreen(lib->Mydisplay), snglBuf);
		gsgl_doublebuffer_is_unavailable (lib);
		if (lib->Vi == NULL) {
			gsgl_error ("gsgl_init_gc_window_mode", "no RGB visual with depth buffer");
		}
	}
	
	if(lib->Vi->class != TrueColor && lib->Vi->class != DirectColor) { 
		gsgl_error ("gsgl_init_gc_window_mode", "TrueColor visual required for this program");
	}
	
	lib->Cx = glXCreateContext (lib->Mydisplay, lib->Vi, None, GL_TRUE);
	if (lib->Cx == NULL) {
		gsgl_error ("gsgl_init_gc_window_mode", "Could not create rendering context");
	}
#endif
	lib->Prewindow = (Window) inq_parent_window (lib);
}

Display * gsgl_display (gsglLibrary *lib)
{
	return lib->Mydisplay;
}

void gsgl_destroy_gc_pixmap_mode (gsglLibrary *lib)
{
	glXMakeCurrent (lib->Mydisplay, None, NULL);
	glXDestroyContext (lib->Mydisplay, lib->Cx);
}

void gsgl_destroy_gc_window_mode (gsglLibrary *lib)
{
#ifdef USE_GTK
	glXMakeCurrent (lib->Mydisplay, None, NULL);
	glXDestroyContext (lib->Mydisplay, lib->Cx);
	if (gsgl_gtk_inq_glarea () != NULL)
		gtk_widget_destroy (gsgl_gtk_inq_glarea ());
	if (gsgl_gtk_inq_appwindow () != NULL)
		gtk_widget_destroy (gsgl_gtk_inq_appwindow ());
#endif
}

void gsgl_make_drawable_window_mode (gsglWindow *w)
{
	w->Mywindow = gui_make_drawable_window_mode (w);
}

void gsgl_make_drawable_pixmap_mode (gsglLibrary *lib)
{
	GLXPixmap glxpixmap;
	Pixmap p;
#ifdef USE_INLINE
	gsglinline *ginline;
#endif

	p = XCreatePixmap (lib->Mydisplay, RootWindow(lib->Mydisplay, lib->Vi->screen), 
			   lib->winwidth0, lib->winheight0, lib->Vi->depth);
	glxpixmap = glXCreateGLXPixmap (lib->Mydisplay, lib->Vi, p);
	glXMakeCurrent (lib->Mydisplay, glxpixmap, lib->Cx);

#ifdef USE_INLINE
	ginline = gsgl_inq_inline ();
	if (ginline != NULL) {
		ginline->pixmap = p;
	}
#endif
}

static void dvsetwin (Window win, gsglLibrary *lib)
{
        XWindowAttributes attribs;
	Window win1;
	Window root_return;
	Window parent_return;
	Window *children_return;
	unsigned int nchildren_return;

	fflush(stdout);

	parent_return = win;
	do {
		win1 = parent_return;
		XQueryTree(lib->Mydisplay, win1, &root_return, &parent_return, &children_return, &nchildren_return);
	} while (parent_return != root_return);

	//XMapRaised(lib->Mydisplay, win1);
	XMapRaised(lib->Mydisplay, win);

	do {
		XGetWindowAttributes (lib->Mydisplay, win, &attribs);
		/*
		  if(attribs.map_state == IsViewable)
		  printf("IsViewable\n");
		  if(attribs.map_state == IsUnmapped)
		  printf("IsUnmapped\n");
		  if(attribs.map_state == IsUnviewable)
		  printf("IsUnviewable\n");
		*/
		
	} while(attribs.map_state != IsViewable);

	XSetInputFocus (lib->Mydisplay, win, RevertToParent, CurrentTime);	
	XFlush (lib->Mydisplay);
}

void gsgl_raise_my_window (gsglLibrary *lib)
{
	if (lib->Mode != GSGL_MODE_WINDOW) return;

#ifdef USE_GTK
	dvsetwin (GDK_WINDOW_XWINDOW(gsgl_gtk_inq_appwindow ()->window), lib);
	GTK_WIDGET_SET_FLAGS (gsgl_gtk_inq_glarea (), GTK_CAN_FOCUS);
	gtk_widget_grab_focus (gsgl_gtk_inq_glarea ());
#endif
#if defined USE_QT
	dvsetwin (gsgl_qt_mainwindow_id (), lib);
#endif
}

void gsgl_raise_my_window_fullscreen (gsglLibrary *lib)
{
#ifdef USE_GTK
	gtk_widget_show (gsgl_gtk_inq_appwindow ());
	gdk_keyboard_grab (gsgl_gtk_inq_glarea ()->window, GSGL_TRUE, GDK_CURRENT_TIME);
#endif
}

void gsgl_lower_my_window (gsglLibrary *lib)
{
	if (lib->Mode != GSGL_MODE_WINDOW) return;
	dvsetwin (lib->Prewindow, lib);
}

void gsgl_flush (gsglWindow *w)
{
	GSGL_DEBUG(fprintf (stdout, "gsgl_flush\n"));
	if (gsgl_doublebuffer_is_enabled (w)) glXSwapBuffers (w->library->Mydisplay, w->Mywindow);
	else glFlush();
}

void gsgl_bell (gsglLibrary *lib)
{
	if (lib->Mode != GSGL_MODE_WINDOW) return;

#if defined USE_QT
	gsgl_qt_bell ();
#else
        XBell (lib->Mydisplay, 0);
        XFlush (lib->Mydisplay);
#endif
}

void gsgl_dumpwin (gsglWindow *w, gsglPage *p)
{
	char cmd[80], s[20];

	sprintf (s,"gsdump%d", gsgl_inq_page_number (p->library));
	XStoreName (w->library->Mydisplay, w->Mywindow, s);
	XFlush (w->library->Mydisplay);
	sprintf (cmd, "xwd -name %s -out %s", s, s);
	system (cmd);
	sprintf (cmd, "xwud -noclick -in %s &", s);
	system (cmd);
	XStoreName (w->library->Mydisplay, w->Mywindow, gsgl_window_title());
}

void gsgl_dumpwin_id (int id, gsglPage *p)
{
	char cmd[80], s[20];

	sprintf (s,"gsdump%d", gsgl_inq_page_number (p->library));
	sprintf (cmd, "xwd -id %d -out %s", id, s);
	system (cmd);
	sprintf (cmd, "xwud -noclick -in %s &", s);
	system (cmd);
}

/* x11 */

void gsgl_page_end_x11 (int *ich, gsglPage *p)
{
	if(*ich != 0) {
		/* enable animation */
		gsgl_doublebuffer_enable (p->win);
		gsgl_init_matrix_cache (p);
		gsgl_init_gl_every_page (p);

		gui_mainloop (p);
		gsgl_gui_close_add_text_dialog ();

		gsgl_doublebuffer_disable (p->win);
		
		gsgl_free_select_buffer (p->win);
		gsgl_cursor_init_every_page ();
	}
	gsgl_lower_my_window (p->library);
	gsgl_finish_guiitems_every_page (p);
}
