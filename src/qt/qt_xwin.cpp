#include "config.h"
#include <iostream>
#include <cmath>
#include "qt_gsglwindow.h"
#include <QSizePolicy>
#include <QSize>
#include <QApplication>
#include <QGuiApplication>
#include <QScreen>
#include <QMainWindow>
#include <QElapsedTimer>
#include <QEventLoop>
#include <QSurfaceFormat>
#include <QOpenGLContext>
#include "toolkit.h"
#include "gsaf.h"
#include "utils.h"
#include "qt_drawarea.h"
#include "qt_xwin.h"
#include "xwin.h"
#include "ogl.h"
#include "tcpip.h"
#include "qt_tcp.h"

static QApplication *App;
static gsglMainWindow *mainwin;
int gsglMainWindow::save_this_frame = 0;
static int gsgl_qt_gl_ready = 0;
static int gsgl_qt_in_paint = 0;
static int gsgl_qt_in_exec = 0;
static QEventLoop *gsgl_qt_loop = NULL;
static int gsgl_qt_initialized = 0;

Display * gsgl_qt_init_gc_window_mode ()
{
	int argc = 0;
	char **argv = NULL;

	QSurfaceFormat fmt;
	fmt.setRenderableType(QSurfaceFormat::OpenGL);
	fmt.setVersion(2, 1);
	fmt.setProfile(QSurfaceFormat::NoProfile);
	fmt.setOption(QSurfaceFormat::DeprecatedFunctions);
	fmt.setDepthBufferSize(16);
	fmt.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
	QSurfaceFormat::setDefaultFormat(fmt);

	App = new QApplication (argc, argv);
	App->setQuitOnLastWindowClosed(false);
	gsgl_qt_initialized = 1;

	return NULL;
}

Window gui_make_drawable_window_mode (gsglWindow *w)
{
	gsgl_qt_gl_ready = 0;
	gsgl_qt_in_paint = 0;
	mainwin = new gsglMainWindow (w);
	//App->setMainWidget (mainwin);
	mainwin->show ();
	mainwin->activateWindow();
	mainwin->raise ();
	gsgl_qt_wait_gl_ready (2000);

	return mainwin->gl->winId ();
}

void gsgl_make_mainwindow_tcp_mode (gsglWindow *w)
{
	gsgl_qt_gl_ready = 0;
	gsgl_qt_in_paint = 0;
	mainwin = new gsglMainWindow (w);
}

gsglMainWindow * gsgl_inq_mainwin ()
{
	return mainwin;
}

void gsgl_qt_set_page (gsglPage *p)
{
	if (gsgl_inq_mode (p->library) == GSGL_MODE_WINDOW) {
		mainwin->set_page (p);
		if (mainwin->gl)
			mainwin->gl->set_page (p);
	}
}

void gui_mainloop (gsglPage *p)
{
	mainwin->set_up_gui (GSGL_FALSE);
	if (gsgl_inq_tk_have_guiitems (p))
		mainwin->gl->setMouseTracking (true);
	else
		mainwin->gl->setMouseTracking (false);

	mainwin->showNormal();
	mainwin->show();
	mainwin->raise();
	mainwin->activateWindow();
	if (mainwin->gl) {
		mainwin->gl->show();
		mainwin->gl->update();
	}
	App->processEvents(QEventLoop::AllEvents, 20);

        if (gsgl_inq_i_am_a_server (p->library)) {
		gsgl_qt_tcp_callback_server (gsgl_inq_server_socket (p), p);
        } else {
		QEventLoop loop;
		gsgl_qt_loop = &loop;
		gsgl_qt_in_exec = 1;
		loop.exec ();
		gsgl_qt_in_exec = 0;
		gsgl_qt_loop = NULL;
        }
}	

void gui_exit_loop ()
{
	if (gsgl_qt_loop) {
		gsgl_qt_loop->exit (0);
	} else if (App) {
		App->quit ();
	}
}

Window gsgl_qt_mainwindow_id ()
{
	return mainwin->winId ();
}

void gsgl_qt_bell ()
{
	QApplication::beep ();
}

float gsgl_qt_get_screen_resolution (void)
{
	QScreen *screen = QGuiApplication::primaryScreen();
	if (!screen)
		return 96.0f;
	QSizeF mm = screen->physicalSize();
	QRect geom = screen->geometry();
	if (mm.width() > 0.0 && mm.height() > 0.0) {
		float h = geom.width() / mm.width();
		float v = geom.height() / mm.height();
		return std::sqrt(h * v) * 25.4f;
	}
	return screen->logicalDotsPerInch();
}

void gsgl_qt_get_screen_size (int *w, int *h)
{
	QScreen *screen = QGuiApplication::primaryScreen();
	if (!screen) {
		if (w) *w = 0;
		if (h) *h = 0;
		return;
	}
	QRect geom = screen->geometry();
	if (w) *w = geom.width();
	if (h) *h = geom.height();
}

void gsgl_qt_raise_window (void)
{
	if (!mainwin)
		return;
	mainwin->showNormal();
	mainwin->show();
	mainwin->raise();
	mainwin->activateWindow();
}

void gsgl_qt_lower_window (void)
{
	if (!mainwin)
		return;
	mainwin->hide();
}

void gsgl_qt_ensure_current (void)
{
	if (!mainwin || !mainwin->gl)
		return;
	if (!mainwin->gl->context() || !mainwin->gl->context()->isValid()) {
		if (mainwin->gl->isVisible() || gsgl_qt_in_exec)
			gsgl_qt_wait_gl_ready (5000);
		else
			return;
	}
	if (!mainwin->gl->isValid()) {
		if (mainwin->gl->isVisible() || gsgl_qt_in_exec) {
			mainwin->gl->update();
			if (gsgl_qt_in_exec)
				App->processEvents();
		} else {
			return;
		}
	}
	QOpenGLContext *ctx = mainwin->gl->context();
	if (!ctx) {
		mainwin->gl->makeCurrent();
		ctx = mainwin->gl->context();
		if (!ctx)
			return;
	}
	if (QOpenGLContext::currentContext() != ctx)
		mainwin->gl->makeCurrent();
}

void gsgl_qt_notify_gl_ready (void)
{
	gsgl_qt_gl_ready = 1;
}

int gsgl_qt_is_gl_ready (void)
{
	return gsgl_qt_gl_ready;
}

static int gsgl_qt_context_valid (void)
{
	if (!mainwin || !mainwin->gl)
		return 0;
	QOpenGLContext *ctx = mainwin->gl->context();
	return (ctx && ctx->isValid()) ? 1 : 0;
}

void gsgl_qt_wait_gl_ready (int timeout_ms)
{
	if (!App || !mainwin || !mainwin->gl)
		return;
	if (!mainwin->gl->isVisible() && !gsgl_qt_in_exec)
		return;

	if (gsgl_qt_context_valid()) {
		gsgl_qt_gl_ready = 1;
		return;
	}
	gsgl_qt_gl_ready = 0;
	mainwin->gl->update();

	QElapsedTimer timer;
	timer.start();
	while (timer.elapsed() < timeout_ms) {
		App->processEvents(QEventLoop::AllEvents, 20);
		if (!gsgl_qt_context_valid())
			mainwin->gl->makeCurrent();
		if (gsgl_qt_gl_ready || gsgl_qt_context_valid()) {
			gsgl_qt_gl_ready = 1;
			return;
		}
	}
	if (!gsgl_qt_gl_ready) {
		gsgl_warning ("gsgl_qt_wait_gl_ready", "OpenGL context not ready");
	}
}

void gsgl_qt_set_in_paint (int flag)
{
	gsgl_qt_in_paint = flag;
}

int gsgl_qt_is_in_paint (void)
{
	return gsgl_qt_in_paint;
}

int gsgl_qt_is_in_exec (void)
{
	return gsgl_qt_in_exec;
}

void gsgl_qt_request_present (void)
{
	if (!App || !mainwin || !mainwin->gl)
		return;
	if (!mainwin->gl->isVisible() && !gsgl_qt_in_exec)
		return;
	mainwin->gl->update();
}

void gsgl_qt_process_events (int max_ms)
{
	if (!App) return;
	if (max_ms <= 0) {
		App->processEvents();
	} else {
		App->processEvents(QEventLoop::AllEvents, max_ms);
	}
}

void gsgl_qt_shutdown (void)
{
	if (!gsgl_qt_initialized) return;
	if (gsgl_qt_loop) {
		gsgl_qt_loop->exit (0);
	}
	if (mainwin) {
		mainwin->hide();
		delete mainwin;
		mainwin = NULL;
	}
	if (App) {
		App->processEvents();
		delete App;
		App = NULL;
	}
	gsgl_qt_initialized = 0;
}

void gui_init_tcp_mode (gsglLibrary *lib)
{
	lib->Mydisplay = gsgl_qt_init_gc_window_mode ();
	lib->Prewindow = 0;
}
