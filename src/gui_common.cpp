#include "config.h"
#include "gsgltype.h"
#include "utils.h"
#include "toolkit.h"
#if defined USE_QT
#    include "qt_gsglwindow.h"
#endif
#if defined USE_QT6
#    include <QTimer>
#    include <QCursor>
#elif defined USE_QT3
#    include <qtimer.h>
#elif defined USE_QT4
#    include <QtCore/QTimer>
#endif
#include "gsaf.h"
#include "drv_wrapper.h"
#include "displaylist.h"
#include "ogl.h"
#include "gui_common.h"
#include "device.h"
#include "bsc1.h"
#include "xwin.h"
#include "callback.h"
#include "base.h"
#include "anime.h"
#include "clientstate.h"
#if defined USE_QT
#    include "qt_xwin.h"
#    include "qt_menuedit.h"
#endif
#ifdef USE_GTK
#    include <gtk/gtk.h>
#    include <gdk/gdk.h>
#    include <gdk/gdkx.h>
#    include <gdk/gdkkeysyms.h>
#    include "gtk_xwin.h"
#    include "gtk_gsglwindow.h"
#    include "gtk_guiitems.h"
#    include "gtk_menu2d.h"
#    include "gtk_menu3d.h"
#    include "gtk_menupro.h"
#    include "gtk_menuedit.h"
#    include "gtk_base.h"
#    include "gtk_anime.h"
#endif
#include <X11/cursorfont.h>
#include "gsgl.h"

char * gsgl_window_title ()
{
	char *str;

	str = getenv("GSGL_TITLE");
	if (str && strlen(str) > 0) {
		return str;
	} else {
		return gsgl_strdup ("GSAF/OpenGL");
	}
}

void gsgl_begin_guiitem (gsglPage *p)
{
	gsgl_enable_drv_mem (p);
	gsgl_enable_displaylist (p);
	/*gsgl_doublebuffer_disable (p->win);*/
}

void gsgl_end_guiitem (gsglPage *p)
{
	gsgl_displaylist_end (p);
	/*gsgl_doublebuffer_enable (p->win);*/
	gsgl_displaylist_remake_added (p);

	gsglMainWindow *w = gsgl_inq_mainwin ();
	w->set_up_gui (GSGL_TRUE);

	if (gsgl_inq_guiitems_external (p) || !gsgl_inq_anime_is_used (p)) { /* FIX ME */
		gsgl_redraw (p);
	}
}

static gsglPage * (*servercallback) (server_data *data, gsglPage *, void (*func)(unsigned int, void *, gsglPage *, READ_TOKEN));

void gsgl_servercallback_reset ()
{
	servercallback = NULL;
}

void gsgl_servercallback_set (gsglPage * (*callback) (server_data *, gsglPage *, void (*)(unsigned int, void *, gsglPage *, READ_TOKEN)))
{
	servercallback = callback;
}

gsglPage * (*gsgl_inq_servercallback(void))(server_data *data, gsglPage *, void (*)(unsigned int, void *, gsglPage *, READ_TOKEN))
{
	return servercallback;
}

void gsglMainWindow::set_page (gsglPage *p)
{
	Page = p;
#ifdef USE_QT
	gl->set_page (p);
#endif
}

void gsgl_gui_set_page (gsglPage *p)
{
	if (gsgl_inq_mode (p->library) == GSGL_MODE_WINDOW || 
	    gsgl_inq_mode (p->library) == GSGL_MODE_TCPIP) {
		gsglMainWindow *w = gsgl_inq_mainwin ();
		w->set_page (p);
	}
}

void gsglMainWindow::set_up_gui (gsglboolean gui) /* FIX ME */
{
	if (Library->Mode == GSGL_MODE_WINDOW)
		gsgl_cursor_init_every_page ();
	gui_2d_init_every_page ();
	gui_3d_init_every_page ();
	gui_projection_init_every_page ();

	if (!gsgl_inq_3dobjects_exist (Page)) {
		gui_3d_sensitive (GSGL_FALSE);
		gui_sensitive_have_3d (GSGL_FALSE);
	} else {
		gui_3d_sensitive (GSGL_TRUE);
		gui_sensitive_have_3d (GSGL_TRUE);
	}

	gsgl_free_select_buffer (Page->win);
	gsgl_init_select (Page);

	if (gsgl_inq_anime_is_used (Page)) {
		gui_anime_sensitive (GSGL_TRUE);
		gui_anime_sensitive_pause (GSGL_FALSE);

		gsgl_displaylist_init_remade (Page);
		gsgl_anime_backup_displaylist (Page);
		/* make the last frame editable */
		gsgl_anime_nth_frame (gsgl_anime_inq_frame_num (Page), Page);

		gsgl_set_current_frame_num (gsgl_anime_inq_frame_num (Page), Page);
	} else {
		gui_anime_sensitive (GSGL_FALSE);
	}
}

void gsgl_gui_quit (void (*func_exit)(int), gsglPage *p)
{
	void (*callback)();

	gsgl_finish_gui (p);
	callback = gsaf_inq_quit_callback ();
	if (callback != NULL)
		callback ();
	func_exit (0);
}

#ifdef USE_GTK
#define GSGL_RETURN GDK_Return
#define GSGL_ENTER  GDK_Return
#define GSGL_N      GDK_n
#define GSGL_LEFT   GDK_Left
#define GSGL_RIGHT  GDK_Right
#define GSGL_UP     GDK_Up
#define GSGL_DOWN   GDK_Down
#define MAINWIN
#endif
#if defined USE_QT
#define GSGL_RETURN Qt::Key_Return
#define GSGL_ENTER  Qt::Key_Enter
#define GSGL_N      Qt::Key_N
#define GSGL_LEFT   Qt::Key_Left
#define GSGL_RIGHT  Qt::Key_Right
#define GSGL_UP     Qt::Key_Up
#define GSGL_DOWN   Qt::Key_Down
#define MAINWIN gsgl_inq_mainwin()->
#endif

void gsgl_gui_keypress (int value, int shift, gsglboolean *key_is_pressed, int *xkey, int *ykey, gsglMainWindow *w)
{
	int offset;
	
	switch (value) {
	case GSGL_RETURN:
#ifdef USE_QT
	case GSGL_ENTER:
#endif
	case GSGL_N:
		gui_exit_loop ();
#ifdef USE_GTK
		if (gsgl_inq_fullscreen (w->Library))
			gtk_widget_hide (w->Appwindow);
#endif
		break;
	case GSGL_LEFT:
	case GSGL_RIGHT:
	case GSGL_UP:
	case GSGL_DOWN:
		if (!gsgl_displaylist_inq_remade (w->Page)) break;
		if (shift)
			offset = 10;
		else
			offset = 1;

		if (*key_is_pressed) {
			if (value == GSGL_LEFT) {
				*xkey -= offset;
			} else if (value == GSGL_RIGHT) {
				*xkey += offset;
			} else if (value == GSGL_UP) {
				*ykey -= offset;
			} else if (value == GSGL_DOWN) { 
				*ykey += offset;
			}
			gsgl_key_motion (*xkey, *ykey, w->Page);
		} else {
			*xkey = 0; *ykey = 0;
			*key_is_pressed = GSGL_TRUE;
			gsgl_button_press (*xkey, *ykey, w->Page);
			if (value == GSGL_LEFT) {
				*xkey -= offset;
			} else if (value == GSGL_RIGHT) {
				*xkey += offset;
			} else if (value == GSGL_UP) {
				*ykey -= offset;
			} else if (value == GSGL_DOWN) { 
				*ykey += offset;
			}
			gsgl_key_motion (*xkey, *ykey, w->Page);
		}
		break;
	default:
		break;
	}
}

void gsgl_gui_keyrelease (int value, int *key_is_pressed, int *xkey, int *ykey, gsglPage *page)
{
        switch (value) {
        case GSGL_LEFT:
        case GSGL_RIGHT:
        case GSGL_UP:
        case GSGL_DOWN:
                *key_is_pressed = GSGL_FALSE;
                *xkey = 0;
                *ykey = 0;
                gsgl_button_release (page);
                break;
        default:
                break;
        }
}

void gsgl_gui_window_configure (unsigned int width, unsigned int height, gsglPage *page)
{
	if (!page) return;

	if (width != gsgl_inq_window_width (page->win) ||
	    height != gsgl_inq_window_height (page->win)) {

		#if defined(USE_QT6) && defined(__APPLE__)
		glFlush();
		#else
		glXWaitX ();
		#endif
		gsgl_set_window_width (width, page->win);
		gsgl_set_window_height (height, page->win);
		glViewport(0, 0, (GLsizei) gsgl_inq_window_width (page->win), (GLsizei) gsgl_inq_window_height (page->win));
		gsgl_redraw (page);
		gsgl_guiitems_configure (page);
	}
}

void gsgl_gui_mouse_motion (int x, int y, int *xstart, int *ystart, gsglboolean left_or_right, gsglPage *page)
{
	if (!page) return;

	if (gsgl_inq_tk_have_guiitems (page)) {
		float fx, fy;
		x11_to_gsgl (x, y, &fx, &fy, page->win);
		point p (fx, fy);
		gsglMouseEvent *m = new gsglMouseEvent (gsglEvent::MouseMove, p, gsglEvent::NoButton);
		gsgl_tk_event_handler (m, page);
		delete m;
	}	

	if (!gsgl_displaylist_inq_remade (page)) return;	
	if (gsgl_inq_mainwin()->Add_text) return;

	if (x < 0) x = 0;
	if (x > (int) gsgl_inq_window_width (page->win)) x = gsgl_inq_window_width (page->win);
	if (y < 0) y = 0;
	if (y > (int) gsgl_inq_window_height (page->win)) y = gsgl_inq_window_height (page->win);

	if (left_or_right) {
		if (gsgl_inq_select_objects (page)) {
			if (*xstart < 0) *xstart = x;
			if (*ystart < 0) *ystart = y;
			gsgl_utility_box (*xstart, *ystart, x, y, page);
		} else {
			gsgl_mouse_motion (x, y, page);
		}
	}
}

void gsgl_gui_button_press (int x, int y, int button_press, gsglboolean left, gsglboolean right, gsglboolean mid, int *xstart, int *ystart, gsglPage *page, gsglMainWindow *mw) 
{
	float fx, fy;

	x11_to_gsgl (x, y, &fx, &fy, page->win);
	point p (fx, fy);
	gsglEvent::ButtonState state;
	
	if (left) {
		state = gsglEvent::LeftButton;
	} else if (right) {
		state = gsglEvent::RightButton;
	} else if (mid) {
		state = gsglEvent::MidButton;
	} else {
		state = gsglEvent::NoButton;
	}
	gsglMouseEvent *m = new gsglMouseEvent (gsglEvent::MouseButtonPress, p, state);
	if (gsgl_tk_event_handler (m, page)) {
		delete m;
		return;
	} else {
		delete m;
	}

#ifdef USE_GTK
	gtk_widget_grab_focus (mw->Glarea);
	if (!button_press) return; /* double-clicked or triple-cliecked */
#endif
	
	if (x < 0) x = 0;
	if (x > (int) gsgl_inq_window_width (page->win)) x = gsgl_inq_window_width (page->win);
	if (y < 0) y = 0;
	if (y > (int) gsgl_inq_window_height (page->win)) y = gsgl_inq_window_height (page->win);
	if (mw->Add_text) {
		*xstart = x;
		*ystart = y;
	} else if (gsgl_inq_select_objects (page)) {
		gsgl_displaylist_remake (page);

		gsgl_redraw_with_name (x, y, 5, 5, page);

		*xstart = x;
		*ystart = y;
	} else {
		if (!gsgl_displaylist_inq_remade (page)) return;
		gsgl_button_press (x, y, page);
	}
}

void gsgl_gui_button_release (int x, int y, int left, int right, int mid, 
			      int *xstart, int *ystart, gsglPage *page)
{
	int hits;
	GLuint names, *ptr;
	int i, j;
	float fx, fy;

	if (gsgl_inq_tk_have_guiitems (page)) {
		x11_to_gsgl (x, y, &fx, &fy, page->win);
		point p (fx, fy);
		gsglEvent::ButtonState state;

		if (left) {
			state = gsglEvent::LeftButton;
		} else if (right) {
			state = gsglEvent::RightButton;
		} else if (mid) {
			state = gsglEvent::MidButton;
		} else {
			state = gsglEvent::NoButton;
		}
		gsglMouseEvent *m = new gsglMouseEvent (gsglEvent::MouseButtonRelease, p, state);
		gsgl_tk_event_handler (m, page);
		delete m;
	}	
	
	if (x < 0) x = 0;
	if (x > (int) gsgl_inq_window_width (page->win)) x = gsgl_inq_window_width (page->win);
	if (y < 0) y = 0;
	if (y > (int) gsgl_inq_window_height (page->win)) y = gsgl_inq_window_height (page->win);
	if (gsgl_inq_mainwin()->Add_text) {
		if (abs(*xstart - x) < 10 && abs(*ystart - y) < 10) {
			x11_to_gsgl (x, y, &fx, &fy, page->win);
			gsgl_add_text_dialog (fx, fy, page);
			return;
		}
	}
	if (!gsgl_displaylist_inq_remade (page)) return;
	if (gsgl_inq_select_objects (page)) {
		
		gsgl_utility_box (-1, 0, 0, 0, page);
		
		hits = gsgl_redraw_with_name ((*xstart+x)/2, (*ystart+y)/2,
					      *xstart>x ? *xstart-x : x-(*xstart),
					      *ystart>y ? *ystart-y : y-(*ystart), page);
		
		gsgl_unselect_objects (page);
		
		*xstart = -1;
		*ystart = -1;
		
		ptr = gsgl_inq_select_buffer (page->win);
		for (i=0; i<hits; i++) {
			names = *ptr;
			ptr++;
			ptr++;
			ptr++;
			for (j=0; j < (int) names; j++) {
				gsgl_select_by_name (*ptr, page);
				if (gsgl_3d_is_selected (page)) {
					MAINWIN gui_2d_sensitive (false);
						MAINWIN gui_3d_sensitive (true);
				} else if (gsgl_2d_is_selected (page)) {
					MAINWIN gui_2d_sensitive (GSGL_TRUE);
					MAINWIN gui_3d_sensitive (GSGL_FALSE);
				}
				ptr++;
			}
		}
		gsgl_redraw (page);
	} else {
		gsgl_button_release (page);
	}
}

void gui_replay_frame (gsglMainWindow *w)
{
	int i = gsgl_inq_current_frame_num (w->Page);
	i++;

	if (i <= (int) gsgl_anime_inq_frame_num (w->Page)) {
		gsgl_clientstate_disable (w->Page);
		gsgl_disable_drv_mem (w->Page);
		gsgl_drv_beginanime (w->Page);
		gsgl_anime_nth_frame (i, w->Page);
		gsgl_disable_drv_mem (w->Page);
		gsgl_drv_endanime (w->Page);
		gsgl_set_current_frame_num (i, w->Page);
	}

	if (i >= (int) gsgl_anime_inq_frame_num (w->Page)) {
#ifdef USE_QT
		w->timer->stop();
#endif

		gsgl_set_current_frame_num (gsgl_anime_inq_frame_num (w->Page), w->Page);
		//gsgl_set_current_frame_num (i, w->Page);
		w->gui_anime_sensitive_pause (GSGL_FALSE);
		w->gui_anime_sensitive_replay (GSGL_TRUE);
#ifdef USE_GTK
		gtk_main_quit ();
#endif
#ifdef USE_QT
		gsgl_yes_select_objects (w->Page);
		w->qt_selection_sensitive (true);

		delete w->timer;
#endif
	}
}

void gui_reverse_replay_frame (gsglMainWindow *w)
{
	int i = gsgl_inq_current_frame_num (w->Page);
	i--;

	if (i > 0) {
		gsgl_clientstate_disable (w->Page);
		gsgl_disable_drv_mem (w->Page);
		gsgl_drv_beginanime (w->Page);
		gsgl_anime_nth_frame (i, w->Page);
		gsgl_disable_drv_mem (w->Page);
		gsgl_drv_endanime (w->Page);
		gsgl_set_current_frame_num (i, w->Page);
	}

	if (i <= 1) {
#ifdef USE_QT
		w->timer->stop();
#endif

		gsgl_set_current_frame_num (1, w->Page);
		w->gui_anime_sensitive_pause (GSGL_FALSE);
		w->gui_anime_sensitive_replay (GSGL_TRUE);
#ifdef USE_GTK
		gtk_main_quit ();
#endif
#ifdef USE_QT
		gsgl_yes_select_objects (w->Page);
		w->qt_selection_sensitive (true);

		delete w->timer;
#endif
	}
}

void gui_anime_forward (gsglMainWindow *w)
{
	int i;
	if (!gsgl_anime_init_replay (w->Page)) return;

	i = gsgl_inq_current_frame_num (w->Page);
	if (i <= -1 || i >= (int) gsgl_anime_inq_frame_num (w->Page)) return;
	i++;
	gsgl_set_current_frame_num (i, w->Page);

	gsgl_disable_drv_mem (w->Page);
	gsgl_drv_beginanime (w->Page);
	gsgl_anime_nth_frame (i, w->Page);
	gsgl_disable_drv_mem (w->Page);
	gsgl_drv_endanime (w->Page);
	gsgl_clientstate_disable (w->Page);
}

void gui_anime_backward (gsglMainWindow *w)
{
	int i;
	if (!gsgl_anime_init_replay (w->Page)) return;

	i = gsgl_inq_current_frame_num (w->Page);
	if (i <= 1 || i > (int) gsgl_anime_inq_frame_num (w->Page)) return;
	i--;
	gsgl_set_current_frame_num (i, w->Page);

	gsgl_disable_drv_mem (w->Page);
	gsgl_drv_beginanime (w->Page);
	gsgl_anime_nth_frame (i, w->Page);
	gsgl_disable_drv_mem (w->Page);
	gsgl_drv_endanime (w->Page);
	gsgl_clientstate_disable (w->Page);
}

static void set_cursor (int cursor_type, gsglMainWindow *w)
{
#ifdef USE_QT6
	if (!w || !w->gl)
		return;
	if (cursor_type < 0) {
		w->gl->unsetCursor();
	} else {
		#ifdef CursorShape
		#undef CursorShape
		#endif
		Qt::CursorShape shape = Qt::ArrowCursor;
		switch (cursor_type) {
		case XC_exchange:
			shape = Qt::SizeAllCursor;
			break;
		case XC_fleur:
			shape = Qt::SizeAllCursor;
			break;
		case XC_double_arrow:
			shape = Qt::SizeVerCursor;
			break;
		case XC_sizing:
			shape = Qt::SizeAllCursor;
			break;
		case XC_pencil:
			shape = Qt::CrossCursor;
			break;
		default:
			shape = Qt::ArrowCursor;
			break;
		}
		w->gl->setCursor (QCursor(shape));
	}
	return;
#endif
	if (cursor_type < 0) {
		XUndefineCursor (w->Library->Mydisplay,
#ifdef USE_GTK
				 GDK_WINDOW_XWINDOW(w->Appwindow->window)
#else
				 w->gl->winId ()
#endif
			);
	} else {
		Cursor cs = XCreateFontCursor (w->Library->Mydisplay, cursor_type);
		XDefineCursor (w->Library->Mydisplay, 
#ifdef USE_GTK
			       GDK_WINDOW_XWINDOW(w->Appwindow->window),
#else
			       w->gl->winId (),
#endif
			       cs);
	}
}

void gui_cursor_default (gsglMainWindow *w)
{
	set_cursor (-1, w);
}

void gui_cursor_rotate (gsglMainWindow *w)
{
	set_cursor (XC_exchange, w);
}

void gui_cursor_translate (gsglMainWindow *w)
{
	set_cursor (XC_fleur, w);
}

void gui_cursor_translate_z (gsglMainWindow *w)
{
	set_cursor (XC_double_arrow, w);
}

void gui_cursor_scale (gsglMainWindow *w)
{
	set_cursor (XC_sizing, w);
}

void gui_cursor_pencil (gsglMainWindow *w)
{
	set_cursor (XC_pencil, w);
}

void gsgl_cursor_init_every_page (void)
{
	gsglMainWindow *w = gsgl_inq_mainwin ();
	if (w && w->Add_text && w->Add_text_dialog) return;
	gui_cursor_default (w);
}

void gsgl_gui_add_text_client (char *str, float x, float y, int fnt, float size, float angle, int ijust, gsglPage *p)
{
	gsgl_begin_guiitem (p);
	gsaf_setpage (0.0, 0.0, 1.0, 1.0);
	gsaf_setfnt (fnt);
	gsaf_setchs (size, angle);
	gsaf_gtext (x, y, str, ijust);
	gsgl_end_guiitem (p);
}
