#include "config.h"
#include <iostream>
#include "qt_gsglwindow.h"
#ifdef USE_QT3
#    include <qvbox.h>
#elif defined USE_QT4
#    include <q3vbox.h>
#endif
#include <qsizepolicy.h>
#include <qsize.h>
#include <qapplication.h>
#include <qgl.h>
#include <qpaintdevice.h>
#include <qmainwindow.h>
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

Display * gsgl_qt_init_gc_window_mode ()
{
	int argc = 0;
		
	App = new QApplication (argc, NULL);

	return QPaintDevice::x11AppDisplay ();
}

Window gui_make_drawable_window_mode (gsglWindow *w)
{
	mainwin = new gsglMainWindow (w);
	//App->setMainWidget (mainwin);
	App->connect (App, SIGNAL(lastWindowClosed()), mainwin, SLOT(quit_immediately()));
	mainwin->show ();
	mainwin->setActiveWindow();
	mainwin->raise ();

	return mainwin->gl->winId ();
}

void gsgl_make_mainwindow_tcp_mode (gsglWindow *w)
{
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

        if (gsgl_inq_i_am_a_server (p->library)) {
		gsgl_qt_tcp_callback_server (gsgl_inq_server_socket (p), p);
        } else {
		App->exec ();
        }
}	

void gui_exit_loop ()
{
	App->exit_loop ();
}

Window gsgl_qt_mainwindow_id ()
{
	return mainwin->winId ();
}

void gsgl_qt_bell ()
{
	QApplication::beep ();
}

void gui_init_tcp_mode (gsglLibrary *lib)
{
	lib->Mydisplay = gsgl_qt_init_gc_window_mode ();
	if (lib->Mydisplay == NULL) 
		gsgl_error ("gsgl_init_gc_window_mode", "Can't open display");
	lib->Prewindow = (Window) inq_parent_window (lib);
}
