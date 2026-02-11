#include "qt_compat.h"
#include "toolkit.h"
#include "qt_drawarea.h"
#include "qt_gsglwindow.h"
#include <QMouseEvent>
#include <QKeyEvent>
#include <QSurfaceFormat>
#include <QOpenGLContext>
#include "gsaf.h"
#include "qt_xwin.h"
#include "xwin.h"
#include "ogl.h"
#include "device.h"
#include "displaylist.h"
#include "callback.h"
#include "guiitems.h"
#include "gui_common.h"

GLDrawArea::GLDrawArea (gsglWindow *window, QWidget *parent, WFLAGS f)
	: QOpenGLWidget (parent, f)
{
	xkey = 0;
	ykey = 0;
	key_is_pressed = 0;
	win = window;
	lib = window->library;
	page = NULL;

	QSurfaceFormat fmt;
	fmt.setRenderableType(QSurfaceFormat::OpenGL);
	fmt.setVersion(2, 1);
	fmt.setProfile(QSurfaceFormat::NoProfile);
	fmt.setOption(QSurfaceFormat::DeprecatedFunctions);
	fmt.setDepthBufferSize(16);
	fmt.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
	setFormat(fmt);
	setUpdateBehavior(QOpenGLWidget::PartialUpdate);

	setMinimumSize (lib->winwidth0, lib->winheight0);
	setFocusPolicy (Qt::StrongFocus);
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
	static QOpenGLContext *last_ctx = NULL;
	QOpenGLContext *ctx = QOpenGLContext::currentContext();
	if (ctx && ctx != last_ctx) {
		last_ctx = ctx;
		if (page)
			gsgl_displaylist_init_remade (page);
	}
	gsgl_qt_set_in_paint (1);
	if (page) {
		if (gsgl_inq_displaylist_is_enabled (page)) {
			if (!gsgl_displaylist_inq_remade (page))
				gsgl_displaylist_remake (page);
			gsgl_redraw (page);
		}
	}
	gsgl_qt_set_in_paint (0);
}

void GLDrawArea :: initializeGL()
{
	//printf ("initialize\n");
	gsgl_doublebuffer_is_available (lib); /* Fix me */
	gsgl_init_gl_once_in_program (win);
	gsgl_qt_notify_gl_ready ();
	if (page)
		gsgl_displaylist_init_remade (page);
}

void GLDrawArea :: resizeGL (int w, int h)
{
	gsgl_gui_window_configure (w, h, page);
}

void GLDrawArea::set_page (gsglPage *p)
{
	page = p;
	if (page)
		gsgl_displaylist_init_remade (page);
}

void GLDrawArea::keyPressEvent (QKeyEvent *e)
{
	gsgl_gui_keypress (e->key (), e->modifiers() & Qt::ShiftModifier, &key_is_pressed, &xkey, &ykey, gsgl_inq_mainwin());
}

void GLDrawArea::keyReleaseEvent (QKeyEvent *e)
{
	gsgl_gui_keyrelease (e->key(), &key_is_pressed, &xkey, &ykey, page);
}

void GLDrawArea::mousePressEvent (QMouseEvent *e)
{
	gsglboolean left, right, mid;

	Qt::MouseButtons buttons = e->buttons();
	left = (buttons & Qt::LeftButton) ? 1 : 0;
	right = (buttons & Qt::RightButton) ? 1 : 0;
	mid = (buttons & Qt::MiddleButton) ? 1 : 0;

	gsgl_gui_button_press (e->x(), e->y(), GSGL_TRUE, left, right, mid, &Xstart, &Ystart, page, gsgl_inq_mainwin());
}

void GLDrawArea::mouseReleaseEvent (QMouseEvent *e)
{
	gsglboolean left, right, mid;

	Qt::MouseButtons buttons = e->buttons();
	left = (buttons & Qt::LeftButton) ? 1 : 0;
	right = (buttons & Qt::RightButton) ? 1 : 0;
	mid = (buttons & Qt::MiddleButton) ? 1 : 0;

	gsgl_gui_button_release (e->x(), e->y(), left, right, mid, &Xstart, &Ystart, page);
}

void GLDrawArea::mouseMoveEvent (QMouseEvent *e)
{
	gsglboolean left_or_right;
	left_or_right = ((e->buttons() & Qt::LeftButton) || (e->buttons() & Qt::RightButton)) ? 1 : 0;
	
	gsgl_gui_mouse_motion (e->x(), e->y(), &Xstart, &Ystart, left_or_right, page);
}
