#include <qmenubar.h>
#include <qtimer.h>
#include "qt_gsglwindow.h"
#include "gsaf.h"
#include "anime.h"
#include "clientstate.h"
#include "drv_wrapper.h"
#include "xwin.h"
#include "gui_common.h"

void gsglMainWindow::gui_anime_sensitive_pause (bool b)
{
        animeMenu->setItemEnabled (id_anime[8], b);
}

void gsglMainWindow::gui_anime_sensitive (bool b)
{
	gui_anime_sensitive_replay (b);
	gui_anime_sensitive_pause (b);
	fileMenu->setItemEnabled (id_file[3], b);
	if (b) {
		editMenu->setItemEnabled (id_edit[3], GSGL_FALSE);
		editMenu->setItemEnabled (id_edit[4], GSGL_FALSE);
	} else {
		editMenu->setItemEnabled (id_edit[3], GSGL_TRUE);
		editMenu->setItemEnabled (id_edit[4], GSGL_TRUE);
	}
}

void gsglMainWindow::gui_anime_sensitive_replay (bool b)
{
	int i;
	
	for (i=0; i<8; i++)
		animeMenu->setItemEnabled (id_anime[i], b);
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

	gui_anime_sensitive_pause (TRUE);
	gui_anime_sensitive_replay (FALSE);
	gsgl_no_select_objects (Page);
	qt_selection_sensitive (FALSE);

	i = gsgl_inq_current_frame_num (Page);
	if (i >= (int) gsgl_anime_inq_frame_num (Page)) {
		i = 0;
		gsgl_set_current_frame_num (i, Page);
	}

	timer = new QTimer (this);
        connect( timer, SIGNAL(timeout()), this, SLOT(replay_frame()) );
        timer->start( j, FALSE );
}

void gsglMainWindow::anime_reverse_replay (int j)
{
	int i;
	if (!gsgl_anime_init_replay (Page)) return;

	gui_anime_sensitive_pause (TRUE);
	gui_anime_sensitive_replay (FALSE);
	gsgl_no_select_objects (Page);
	qt_selection_sensitive (FALSE);

	i = gsgl_inq_current_frame_num (Page);
	if (i <= 1) {
		i = gsgl_anime_inq_frame_num (Page)+1;
		gsgl_set_current_frame_num (i, Page);
	}

	timer = new QTimer (this);
        connect( timer, SIGNAL(timeout()), this, SLOT(reverse_replay_frame()) );
        timer->start( j, FALSE );
}

void gsglMainWindow::anime_pause ()
{
	gui_anime_sensitive_pause (FALSE);
	gui_anime_sensitive_replay (TRUE);

	timer->stop();

	gsgl_yes_select_objects (Page);
	qt_selection_sensitive (TRUE);

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
