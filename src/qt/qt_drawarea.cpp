#include "qt_compat.h"
#include "toolkit.h"
#include "qt_drawarea.h"
#include "qt_gsglwindow.h"
#ifdef USE_QT4
#    include <QMouseEvent>
#    include <QKeyEvent>
#endif
#include "gsaf.h"
#include "qt_xwin.h"
#include "xwin.h"
#include "ogl.h"
#include "device.h"
#include "displaylist.h"
#include "callback.h"
#include "guiitems.h"
#include "gui_common.h"

GLDrawArea::GLDrawArea (gsglWindow *window, const QGLFormat &format, QWidget *parent, const char *name, const QGLWidget *shareWidget, WFLAGS f)
	: QGLWidget (format, parent, name, shareWidget, f)
{
	xkey = 0;
	ykey = 0;
	key_is_pressed = 0;
	win = window;
	lib = window->library;
	page = NULL;

	setMinimumSize (lib->winwidth0, lib->winheight0);
#ifdef USE_QT3
	setFocusPolicy (QWidget::StrongFocus);
#else
	setFocusPolicy (Qt::StrongFocus);
#endif
}

GLDrawArea::~GLDrawArea ()
{
}

void GLDrawArea::swapBuffers ()
{
	//gsgl_flush ();
}

void GLDrawArea :: paintGL()
{
	if (page)
		gsgl_redraw (page);
}

void GLDrawArea :: initializeGL()
{
	//printf ("initialize\n");
	gsgl_doublebuffer_is_available (lib); /* Fix me */
	gsgl_init_gl_once_in_program (win);
}

void GLDrawArea :: resizeGL (int w, int h)
{
	gsgl_gui_window_configure (w, h, page);
}

void GLDrawArea::set_page (gsglPage *p)
{
	page = p;
}

void GLDrawArea::keyPressEvent (QKeyEvent *e)
{
#ifdef USE_QT3
	gsgl_gui_keypress (e->key (), e->state() & Qt::ShiftButton, &key_is_pressed, &xkey, &ykey, gsgl_inq_mainwin());
#else
	gsgl_gui_keypress (e->key (), e->state() & Qt::ShiftModifier, &key_is_pressed, &xkey, &ykey, gsgl_inq_mainwin());
#endif
}

void GLDrawArea::keyReleaseEvent (QKeyEvent *e)
{
	gsgl_gui_keyrelease (e->key(), &key_is_pressed, &xkey, &ykey, page);
}

void GLDrawArea::mousePressEvent (QMouseEvent *e)
{
	gsglboolean left, right, mid;

	left = (e->state() == Qt::LeftButton ? 1 : 0);
	right = (e->state() == Qt::RightButton ? 1 : 0);
	mid = (e->state() == Qt::MidButton ? 1 : 0);

	gsgl_gui_button_press (e->x(), e->y(), GSGL_TRUE, left, right, mid, &Xstart, &Ystart, page, gsgl_inq_mainwin());
}

void GLDrawArea::mouseReleaseEvent (QMouseEvent *e)
{
	gsglboolean left, right, mid;

	left = (e->state() == Qt::LeftButton ? 1 : 0);
	right = (e->state() == Qt::RightButton ? 1 : 0);
	mid = (e->state() == Qt::MidButton ? 1 : 0);

	gsgl_gui_button_release (e->x(), e->y(), left, right, mid, &Xstart, &Ystart, page);
}

void GLDrawArea::mouseMoveEvent (QMouseEvent *e)
{
	gsglboolean left_or_right;
	left_or_right = (e->state() & Qt::LeftButton || e->state() & Qt::RightButton) ? 1 : 0;
	
	gsgl_gui_mouse_motion (e->x(), e->y(), &Xstart, &Ystart, left_or_right, page);
}
