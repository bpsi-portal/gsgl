#include <QApplication>
#include "qt_gsglwindow.h"
#include "qt_gsgltcp.h"
#include "qt_tcp.h"
#include "gsaf.h"
#include "gsgl.h"
#include "qt_xwin.h"
#include "tcpip.h"
#include "tcp_guiitems.h"
#include "fileformat.h"
#include "device.h"
#include "displaylist.h"
#include "bsc1.h"
#include "drv_wrapper.h"
#include "anime.h"
#include "utils.h"
#include "xwin.h"
#include "gui_common.h"
#include "libserver.h"

#define GSGL_DEBUG(x)

gsgltcp_base::gsgltcp_base (int socket, gsglPage *p, QObject * parent)
	: QObject (parent)
{
	Page = p;
        notify = new QSocketNotifier (socket, QSocketNotifier::Read, this);	
        QObject::connect (notify, SIGNAL(activated(int)),
                          this, SLOT(callback()));
}

gsgltcp_client::gsgltcp_client (int socket, gsglPage *p, QObject * parent)
	: gsgltcp_base (socket, p, parent)
{
}

gsgltcp_server::gsgltcp_server (int socket, gsglPage *p, QObject * parent)
	: gsgltcp_base (socket, p, parent)
{
}

static void func_exit_from_pagee (void)
{
	qApp->quit ();
}

static void func_save_and_close (void)
{
	gsgl_inq_mainwin ()->save_close ();
}

static void func_save_as (void)
{
	gsgl_inq_mainwin ()->save_as ();
}

static void func_save (void)
{
	gsgl_inq_mainwin ()->save ();
}

static void func_save_this_frame (void)
{
	gsgl_inq_mainwin ()->save_frame ();
}

static void func_export_vec_eps (void)
{
	gsgl_inq_mainwin ()->export_eps_vec ();
}

static void func_export_ras_eps (void)
{
	gsgl_inq_mainwin ()->export_eps_ras ();
}

static void func_export_png (void)
{
	gsgl_inq_mainwin ()->export_png ();
}

static void func_export_jpg (void)
{
	gsgl_inq_mainwin ()->export_jpeg ();
}

static void func_print (void)
{
	gsgl_inq_mainwin ()->print ();
}

static void func_config_color (void)
{
	gsgl_inq_mainwin ()->color ();
}

static void func_config_command (void)
{
	gsgl_inq_mainwin ()->command ();
}

static void func_config_font (void)
{
	gsgl_inq_mainwin ()->font ();
}

static void func_config_window (void)
{
	gsgl_inq_mainwin ()->windowsize ();
}

void gsgltcp_client::callback ()
{
	gsgl_client_callback_template (notify->socket(), Page,
				       func_exit_from_pagee,
				       func_save_and_close,
				       func_save_as,
				       func_save,
				       func_save_this_frame,
				       func_export_vec_eps,
				       func_export_ras_eps,
				       func_export_png,
				       func_export_jpg,
				       func_print,
				       func_config_color,
				       func_config_command,
				       func_config_font,
				       func_config_window);
}

void gsgltcp_server::callback ()
{
	gsgl_server_callback_template (notify->socket(), Page,
				       func_exit_from_pagee);
}	

void gui_tcp_callback_client (int socket, gsglPage *p)
{
	gsglMainWindow::save_this_frame = 0;

	gsgltcp_client tcp (socket, p);
	qApp->exec ();
}

void gsgl_qt_tcp_callback_server (int socket, gsglPage *p)
{
	gsgltcp_server tcp (socket, p);
	qApp->exec ();
}
