#include "gsaf.h"
#include "displaylist.h"
#include "xwin.h"
#ifdef USE_QT6
#include "qt_xwin.h"
#endif
#include "base.h"
#include "mem_list.h"
#include "ogl.h"
#include "device.h"
#include "configfile.h"
#include "utils.h"
#include "text.h"
#include "anime.h"
#include "clientstate.h"
#include "fileio_base.h"
#include "harddisk.h"
#include "drv_wrapper.h"
#include "matrix.h"
#include "tcpip.h"
#include "text_type1_kerning.h"
#include "bsc1.h"
#include "osmesa.h"
#include "guiitems.h"
#include "gui_common.h"
#include "gsgl2ps.h"
#include "internal.h"
#include <locale.h>
#include "apl1.h"
#include "apl2.h"
#include "apl3d.h"
#include "callback.h"
#include "xwin.h"
#ifdef USE_GTK
#    include <gtk/gtk.h>
#    include "gtk_base.h"
#    include "gtk_xwin.h"
#    include "gtk_gsglwindow.h"
#endif
#ifdef USE_QT
#    include "qt_xwin.h"
#endif

static void gsgl_page_end_raster (int4 *ich);
static void gsgl_page_end_vector (int4 *ich, gsglPage *);

gsglPage * gsgl_page_new (gsglLibrary *lib, gsglWindow *w)
{
	gsglPage *Page;

	Page = gsgl_malloc (sizeof(gsglPage));
	Page->base = gsgl_malloc (sizeof(gsglBase));
	Page->Anime = gsgl_malloc (sizeof(gsglAnime));
	Page->pattern = gsgl_malloc (sizeof(gsglpattern));
	Page->TwoDInfo = gsgl_malloc (sizeof(gsgl2dinfo));
	Page->scale3d = gsgl_malloc (sizeof(gsglScale3D));
	Page->ThreeDInfo = gsgl_malloc (sizeof(gsgl3DInfo));
	Page->callback = gsgl_malloc (sizeof(gsglCallBack));
	Page->matrix = gsgl_malloc (sizeof(gsglMatrices));
	Page->text = gsgl_malloc (sizeof(gsglText));
	Page->buffer = gsgl_malloc (sizeof(gsglBuffers));
	Page->driver = gsgl_malloc (sizeof(gsglDrivers));
	Page->dsp = gsgl_malloc (sizeof(gsglDisplay));
	Page->client = gsgl_malloc (sizeof(gsglClientState));
	Page->gui = gsgl_malloc (sizeof(gsglGuiitems));
	Page->library = lib;
	Page->win = w;

	return Page;
}

void gsgl_set_mode (int mode, gsglLibrary *lib)
{
	lib->Mode = mode;
}

int gsgl_inq_page_number (gsglLibrary *lib)
{
	return lib->Pagenum;
}

int gsgl_inq_devicetype (gsglLibrary *lib)
{
	return lib->DeviceType;
}

void gsgl_device_judge (gsglLibrary *lib)
{
	char *str;
	
	str = getenv("GSGL_DEVICE");
	if (str) {
		if(!strcmp(str, "PS")) { /* gsgl2ps */
			lib->DeviceType = GSGL_DEVICE_PS;
			return;
		}
		if(!strcmp(str, "RASTER")) { /* gsgl2raster */
			lib->DeviceType = GSGL_DEVICE_RASTER;
			return;
		}
	}
	lib->DeviceType = GSGL_DEVICE_X11;

	gsgl_check_i_am_a_server (lib);
}

void gsgl_device_type (int4 *ich, gsglLibrary *lib)
{
	char *str;
  
	if (lib->DeviceType == GSGL_DEVICE_X11) {
		if (gsgl_inq_i_am_a_server (lib)) {
			*ich = 0; 
		} else {
			printf(" # Welcome to GSAF/OpenGL \n");
			str = getenv("GSGDP");
			if(str && strlen (str) > 0) {
				*ich = 0;
			} else {
				*ich = 1;
			}
		}
	} else if (lib->DeviceType == GSGL_DEVICE_PS) {
		*ich = 0;
	} else if (lib->DeviceType == GSGL_DEVICE_RASTER) {
		*ich = 0;
	} else {
		gsgl_error ("dvtype", "device type is unknown");
	}
}

static void gsgl_gettextize (void)
{
#ifdef USE_GTK
	gsgl_gtk_gettextize ();
#endif
#ifdef USE_QT
	setlocale (LC_ALL, "");
        bindtextdomain (PACKAGE, LOCALEDIR);
        bind_textdomain_codeset (PACKAGE, "UTF-8");
        textdomain (PACKAGE);
#endif
}

void gsgl_device_open (int4 *ich, gsglLibrary *lib)
{
	char *str = NULL;
	int  c1, c2;
	int  port = 0;
	char *host = NULL;

	c1 = *ich;
	c2 = 1;
	/*GChkeymode = 1; */ /* what is this ? */
	
	str = getenv ("GSGDP");
	if (str && strlen (str) > 0) {
		c1 = str[0];
		if (c1 == '-'){
			/*GChkeymode = 0;*/
			if (str[1]) {
				c1 = str[1];
				if (str[2])
					c2 = str[2];
			}
		} else {
			if (str[1])
				c2 = str[1];
		}
	}
	*ich = c2;
		
	gsgl_gettextize ();
	gsgl_init_bg_color_once_in_program (lib);
	if (!gsgl_inq_i_am_a_server (lib))
		gsgl_config_read (lib);
	else {
		gsgl_config_read_server_ttf (lib);
		gsgl_config_set_server (lib);
	}

	gsgl_set_window_size (c1, ich, lib);

	if (!gsgl_inq_i_am_a_server (lib))
		host = gsgl_inq_server_host (&port);

	if (host != NULL && port > 0 && gsgl_inq_mode (lib) == GSGL_MODE_WINDOW &&
	    gsgl_init_tcp_once_in_program (host, port, c1, lib)) {
		lib->DeviceType = GSGL_DEVICE_TCPIP;
		gsgl_set_mode (GSGL_MODE_TCPIP, lib);
		gui_init_tcp_mode (lib);
	} else if (gsgl_inq_mode (lib) == GSGL_MODE_PIXMAP) { /* write to pixel buffer */
		gsgl_init_gc_pixmap_mode (lib);
	} else if (gsgl_inq_mode (lib) == GSGL_MODE_INLINE) {
#ifndef HAVE_OSMESA
		gsgl_init_gc_pixmap_mode (lib);
#endif
	} else if (gsgl_inq_mode (lib) == GSGL_MODE_WINDOW) { /* write to window */
		gsgl_init_gc_window_mode (lib);
		if (gsgl_inq_fullscreen (lib)) {
#ifdef USE_QT6
			int sw = 0, sh = 0;
			gsgl_qt_get_screen_size (&sw, &sh);
			if (sw > 0 && sh > 0) {
				lib->winwidth0 = sw;
				lib->winheight0 = sh;
			} else if (gsgl_display (lib)) {
				lib->winwidth0 = DisplayWidth (gsgl_display (lib), 0);
				lib->winheight0 = DisplayHeight (gsgl_display (lib), 0);
			}
#else
			lib->winwidth0 = DisplayWidth (gsgl_display (lib), 0);
			lib->winheight0 = DisplayHeight (gsgl_display (lib), 0);
#endif
		}
	}

	gsft_init_freetype_once_in_program (lib);
	gsgl_init_type1_kerning (lib);
}

gsglWindow * gsgl_device_pages (int4 npage, float sizex, float sizey, int4 lkeep, gsglPage *Page)
{
	if (!Page->win) {

		Page->win = gsgl_malloc (sizeof(gsglWindow));
		gsgl_set_window_width (Page->library->winwidth0, Page->win);
		gsgl_set_window_height (Page->library->winheight0, Page->win);
		Page->win->library = Page->library;

		if (gsgl_inq_mode (Page->library) == GSGL_MODE_PIXMAP) {
			gsgl_make_drawable_pixmap_mode (Page->library);
			gsgl_init_gl_once_in_program (Page->win);
		} else if (gsgl_inq_mode (Page->library) == GSGL_MODE_INLINE) {
#ifdef HAVE_OSMESA
			gsgl_make_drawable_osmesa (gsgl_inq_window_width(),
						   gsgl_inq_window_height());
#else
			gsgl_make_drawable_pixmap_mode (Page->library);
#endif
			gsgl_init_gl_once_in_program (Page->win);
		} else if (gsgl_inq_mode (Page->library) == GSGL_MODE_WINDOW) {
			gsgl_make_drawable_window_mode (Page->win);
#ifdef USE_GTK
			gsgl_init_gl_once_in_program (Page->win);
#endif
		} else if (gsgl_inq_mode (Page->library) == GSGL_MODE_TCPIP) {
			gsgl_make_mainwindow_tcp_mode (Page->win);
		}
	} else {
		if (gsgl_inq_mode (Page->library) == GSGL_MODE_WINDOW) {
			if (gsgl_inq_fullscreen (Page->library)) {
				gsgl_raise_my_window_fullscreen (Page->library);
			} else {
				gsgl_raise_my_window (Page->library);
			}
		}
	}
	
	gsgl_drivers_init (Page);

	if (gsgl_inq_mode (Page->library) == GSGL_MODE_NO_GL) {
		gsgl_disable_drv_gl (Page);
		gsgl_disable_drv_mem (Page);
		gsgl_disable_drv_hd (Page);
		gsgl_disable_drv_tcp (Page);

		if (gsgl_inq_savefile_opened ()) {
			gsgl_enable_drv_hd (Page);
			gsgl_init_hd_every_page (GSGL_TRUE, Page);
		} else {
			gsgl_init_hd_every_page (GSGL_FALSE, Page);
		}
	} else if (gsgl_inq_mode (Page->library) == GSGL_MODE_TCPIP) {
		gsgl_disable_drv_gl (Page);
		gsgl_enable_drv_mem (Page);
		gsgl_disable_drv_hd (Page);
		gsgl_enable_drv_tcp (Page);
		
		gsgl_init_hd_every_page (GSGL_FALSE, Page);
	} else {
		gsgl_enable_drv_gl (Page);
		gsgl_enable_drv_mem (Page);
		gsgl_disable_drv_hd (Page);
		gsgl_disable_drv_tcp (Page);
		
		gsgl_init_gl_every_page (Page);
		gsgl_init_hd_every_page (GSGL_FALSE, Page);
	}
	
	/* initialize global variables */
	gsgl_init_mem_list_every_page (Page);
	gsgl_init_matrix_cache (Page);
	gsgl_init_info_every_page (Page);
	gsgl_displaylist_init_remade (Page);
	gsgl_init_client_state_every_page (Page);
	gsgl_yes_select_objects (Page);
	gsgl_free_select_buffer (Page->win);
	gsgl_init_anime_every_page (Page);
	gsgl_init_text (Page);
	gsgl_init_guiitems_enery_page (Page);
	gsgl_gui_set_page (Page);

	return Page->win;
}

void gsgl_usedtime (float time, gsglLibrary *lib)
{
	if (gsgl_inq_i_am_a_server (lib)) return;

	/*fprintf (stdout, " # GSAF/OpenGL %s based on GSAF %s\n", GSGL_VERSION, GSAF_VERSION);*/
	fprintf (stdout, " # GSAF/OpenGL %s\n", GSGL_VERSION);
	fprintf (stdout, " #              CLOSED.  USED CPU TIME = %10.3f SEC\n", time);
}

void gsgl_device_pagee (int4 *ich, gsglPage *p)
{
	gsgl_displaylist_end (p);

	if (gsgl_inq_mode (p->library) == GSGL_MODE_NO_GL) {
		*ich = 0;
		if (gsgl_inq_savefile_opened ())
			gsgl_finish_hd_every_page (p);
	} else if (gsgl_inq_mode (p->library) == GSGL_MODE_PIXMAP) {
		*ich = 0;
		if (p->library->DeviceType == GSGL_DEVICE_PS) {
			gsgl_page_end_vector (ich, p);
		} else if (p->library->DeviceType == GSGL_DEVICE_RASTER) {
			gsgl_page_end_raster (ich);
		}
	} else if (gsgl_inq_mode (p->library) == GSGL_MODE_INLINE) {
#ifdef USE_INLINE
		*ich = 1;
		gsgl_page_end_inline (ich);
#endif
	} else if (gsgl_inq_mode (p->library) == GSGL_MODE_WINDOW) {
		gsgl_page_end_x11 (ich, p);
	} else if (gsgl_inq_mode (p->library) == GSGL_MODE_TCPIP) {
		gsgl_page_end_tcpip (p);
	}
}

void gsgl_finish_common (gsglPage *p)
{
	gsgl_lists_delete (p);
	gsgl_clientstate_disable (p);
	gsgl_anime_finish_every_page (p);
	gsgl_textures_delete (p);
}

void gsgl_finish_gui (gsglPage *p)
{
	gsgl_finish_common (p);
	if (gsgl_inq_mode (p->library) == GSGL_MODE_WINDOW) {
		gsgl_destroy_gc_window_mode (p->library);
		if (!gsgl_inq_i_am_a_server (p->library))
			gsgl_config_write (p->library);
	}
	gsgl_tcp_send_quit_to_client (p->library);
	gsgl_close_message (p->library);
}

void gsgl_finish_tcp (gsglPage *p)
{
	gsgl_finish_common (p);
	if (gsgl_inq_mode (p->library) == GSGL_MODE_TCPIP) {
		gsgl_finish_tcp_once_in_program (p->library);
		gsgl_config_write (p->library);
	}
	gsgl_close_message (p->library);
}

void gsgl_finish_every_page (gsglPage *p)
{
	if (!gsgl_inq_i_am_a_server (p->library))
		gsgl_finish_common (p);
	if (gsgl_inq_mode (p->library) == GSGL_MODE_TCPIP)
		gsgl_tcp_exit_from_pagee (p);
}

void gsgl_device_close (int4 *ich, gsglLibrary *lib)
{
	if (gsgl_inq_mode (lib) == GSGL_MODE_PIXMAP) {
		*ich = 0;
		gsgl_destroy_gc_pixmap_mode (lib);
	} else if (gsgl_inq_mode (lib) == GSGL_MODE_INLINE) {
		*ich = 1;
#ifndef HAVE_OSMESA
		gsgl_destroy_gc_pixmap_mode (lib);
#endif
	} else if (gsgl_inq_mode (lib) == GSGL_MODE_WINDOW) {
		*ich = 1;
		gsgl_destroy_gc_window_mode (lib);
		if (!gsgl_inq_i_am_a_server (lib))
			gsgl_config_write (lib);
	} else if (gsgl_inq_mode (lib) == GSGL_MODE_TCPIP) {
		*ich = 1;
		gsgl_finish_tcp_once_in_program (lib);
		gsgl_config_write (lib);
	}
}

/* vector (postscript) */

GLfloat * gsgl2ps_getfeedbackbuffer (gsglLibrary *lib)
{
	return lib->feedbackBuffer;
}

void gsgl2ps_freefeedbackbuffer (gsglLibrary *lib)
{
	if (lib->feedbackBuffer)
		gsgl_free (lib->feedbackBuffer);
}

static void gsgl_page_end_vector (int4 *ich, gsglPage *p)
{
	gsglGLFeedbackBuffer *Buffer0;

	gsgl_init_gl_every_page (p);
	Buffer0 = gsgl_internal_format_make (p);
	if (Buffer0->size)
		p->library->feedbackBuffer = Buffer0->fbuffer;
	else
		p->library->feedbackBuffer = NULL;
}

/* raster */

void * gsgl2raster_get_image (gsglLibrary *lib)
{
	GLvoid *buffer;
	GLint swapbytes, lsbfirst, rowlength;
	GLint skiprows, skippixels, alignment;
	GLenum format;
	unsigned int size;
	
	format = GL_RGBA;
	size = lib->winwidth0 * lib->winheight0 * 4;
	
	buffer = (GLvoid *) gsgl_malloc_atomic_ignore_off_page (size);
	if (buffer == NULL) return NULL;
	
	/* Save current modes. */
	glGetIntegerv (GL_PACK_SWAP_BYTES, &swapbytes);
	glGetIntegerv (GL_PACK_LSB_FIRST, &lsbfirst);
	glGetIntegerv (GL_PACK_ROW_LENGTH, &rowlength);
	glGetIntegerv (GL_PACK_SKIP_ROWS, &skiprows);
	glGetIntegerv (GL_PACK_SKIP_PIXELS, &skippixels);
	glGetIntegerv (GL_PACK_ALIGNMENT, &alignment);
	/* Little endian machines (DEC Alpha for example) could
	   benefit from setting GL_PACK_LSB_FIRST to GL_TRUE
	   instead of GL_FALSE, but this would require changing the
	   generated bitmaps too. */
	glPixelStorei (GL_PACK_SWAP_BYTES, GL_FALSE);
	glPixelStorei (GL_PACK_LSB_FIRST, GL_FALSE);
	glPixelStorei (GL_PACK_ROW_LENGTH, 0);
	glPixelStorei (GL_PACK_SKIP_ROWS, 0);
	glPixelStorei (GL_PACK_SKIP_PIXELS, 0);
	glPixelStorei (GL_PACK_ALIGNMENT, 1);
	
	/* Actually read the pixels. */
	glReadPixels (0, 0, lib->winwidth0, lib->winheight0, format, GL_UNSIGNED_BYTE, (GLvoid *) buffer);
	
	/* Restore saved modes. */
	glPixelStorei (GL_PACK_SWAP_BYTES, swapbytes);
	glPixelStorei (GL_PACK_LSB_FIRST, lsbfirst);
	glPixelStorei (GL_PACK_ROW_LENGTH, rowlength);
	glPixelStorei (GL_PACK_SKIP_ROWS, skiprows);
	glPixelStorei (GL_PACK_SKIP_PIXELS, skippixels);
	glPixelStorei (GL_PACK_ALIGNMENT, alignment);

	return buffer;
}

static void gsgl_page_end_raster (int4 *ich)
{
}
