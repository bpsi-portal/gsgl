#include <QMenuBar>
#include <QTimer>
#include "qt_gsglwindow.h"
#include "gsaf.h"
#include "anime.h"
#include "clientstate.h"
#include "drv_wrapper.h"
#include "xwin.h"
#include "gui_common.h"

void gsglMainWindow::gui_anime_sensitive_pause (bool b)
{
        if (id_anime[8]) id_anime[8]->setEnabled (b);
}

void gsglMainWindow::gui_anime_sensitive (bool b)
{
	gui_anime_sensitive_replay (b);
	gui_anime_sensitive_pause (b);
	if (id_file[3]) id_file[3]->setEnabled (b);
	if (b) {
		if (id_edit[3]) id_edit[3]->setEnabled (GSGL_FALSE);
		if (id_edit[4]) id_edit[4]->setEnabled (GSGL_FALSE);
	} else {
		if (id_edit[3]) id_edit[3]->setEnabled (GSGL_TRUE);
		if (id_edit[4]) id_edit[4]->setEnabled (GSGL_TRUE);
	}
}

void gsglMainWindow::gui_anime_sensitive_replay (bool b)
{
	int i;
	
	for (i=0; i<8; i++)
		if (id_anime[i]) id_anime[i]->setEnabled (b);
}

void gsglMainWindow::replay_frame ()
{
	gui_replay_frame (this);
}

void gsglMainWindow::reverse_replay_frame ()
{
	gui_reverse_replay_frame (this);
}

void gsglMainWindow::anime_replay (int j)
{
	int i;
	if (!gsgl_anime_init_replay (Page)) return;

	gui_anime_sensitive_pause (true);
	gui_anime_sensitive_replay (false);
	gsgl_no_select_objects (Page);
	qt_selection_sensitive (false);

	i = gsgl_inq_current_frame_num (Page);
	if (i >= (int) gsgl_anime_inq_frame_num (Page)) {
		i = 0;
		gsgl_set_current_frame_num (i, Page);
	}

	timer = new QTimer (this);
        connect( timer, SIGNAL(timeout()), this, SLOT(replay_frame()) );
	timer->setSingleShot(false);
        timer->start( j );
}

void gsglMainWindow::anime_reverse_replay (int j)
{
	int i;
	if (!gsgl_anime_init_replay (Page)) return;

	gui_anime_sensitive_pause (true);
	gui_anime_sensitive_replay (false);
	gsgl_no_select_objects (Page);
	qt_selection_sensitive (false);

	i = gsgl_inq_current_frame_num (Page);
	if (i <= 1) {
		i = gsgl_anime_inq_frame_num (Page)+1;
		gsgl_set_current_frame_num (i, Page);
	}

	timer = new QTimer (this);
        connect( timer, SIGNAL(timeout()), this, SLOT(reverse_replay_frame()) );
	timer->setSingleShot(false);
        timer->start( j );
}

void gsglMainWindow::anime_pause ()
{
	gui_anime_sensitive_pause (false);
	gui_anime_sensitive_replay (true);

	timer->stop();

	gsgl_yes_select_objects (Page);
	qt_selection_sensitive (true);

	delete timer;
}

void gsglMainWindow::anime_forward ()
{
	gui_anime_forward (this);
}

void gsglMainWindow::anime_backward ()
{
	gui_anime_backward (this);
}
