#include "gsaf.h"
#include "gtk_gsglwindow.h"
#include "gtk_anime.h"
#include "anime.h"
#include "gui_common.h"
#include "xwin.h"
#include <gtk/gtk.h>
#include "gtk_base.h"

void gsglMainWindow::gui_anime_sensitive_pause (gsglboolean b)
{
	SENSITIVE(GMenu, N_("/Animation/Pause"), b);
}

void gsglMainWindow::gui_anime_sensitive (gsglboolean b)
{
	gui_anime_sensitive_replay (b);
	gui_anime_sensitive_pause (b);
	SENSITIVE(GMenu, N_("/File/Save This Frame"), b);
	if (b) {
		/*SENSITIVE(GMenu, N_("/Edit/Add Lines"), GSGL_FALSE);
		  SENSITIVE(GMenu, N_("/Edit/Add Texts"), GSGL_FALSE);*/
	} else {
		/*SENSITIVE(GMenu, N_("/Edit/Add Lines"), GSGL_TRUE);
		  SENSITIVE(GMenu, N_("/Edit/Add Texts"), GSGL_TRUE);*/
	}
}

void gsglMainWindow::gui_anime_sensitive_replay (gsglboolean b)
{
	SENSITIVE(GMenu, N_("/Animation/Replay"), b);
	SENSITIVE(GMenu, N_("/Animation/Replay (Slow)"), b);
	SENSITIVE(GMenu, N_("/Animation/Replay (Very Slow)"), b);
	SENSITIVE(GMenu, N_("/Animation/Single Frame Forward"), b);
	SENSITIVE(GMenu, N_("/Animation/Reverse Play"), b);
	SENSITIVE(GMenu, N_("/Animation/Reverse Play (Slow)"), b);
	SENSITIVE(GMenu, N_("/Animation/Reverse Play (Very Slow)"), b);
	SENSITIVE(GMenu, N_("/Animation/Single Frame Reverse"), b);
}

gsglboolean gsgl_anime_replay_frame (gpointer gdata)
{
	gui_replay_frame ((gsglMainWindow *) gdata);
	return GSGL_TRUE;
}

gsglboolean gsgl_anime_reverse_replay_frame (gpointer gdata)
{
	gui_reverse_replay_frame ((gsglMainWindow *) gdata);
	return GSGL_TRUE;
}

void gsgl_gtk_anime_replay (gpointer gdata, guint action, GtkWidget *widget)
{
	int i;
	gsglMainWindow *w = (gsglMainWindow *) gdata;

	if (!gsgl_anime_init_replay (w->Page)) return;
	w->gui_anime_sensitive_pause (GSGL_TRUE);
	w->gui_anime_sensitive_replay (GSGL_FALSE);
	gsgl_no_select_objects (w->Page);
	gsgl_gtk_forbit_selection ();

	i = gsgl_inq_current_frame_num (w->Page);
	if (i >= (int) gsgl_anime_inq_frame_num (w->Page)) {
		i = 0;
		gsgl_set_current_frame_num (i, w->Page);
	}

	w->Handler_anime = gtk_timeout_add (action, gsgl_anime_replay_frame, w);
	gtk_main ();
	gtk_timeout_remove (w->Handler_anime);
	w->gui_anime_sensitive_pause (GSGL_FALSE);
	w->gui_anime_sensitive_replay (GSGL_TRUE);
	gsgl_yes_select_objects (w->Page);
	gsgl_gtk_allow_selection ();
}

void gsgl_gtk_anime_reverse_replay (gpointer gdata, guint action, GtkWidget *widget)
{
	int i;
	gsglMainWindow *w = (gsglMainWindow *) gdata;

	if (!gsgl_anime_init_replay (w->Page)) return;
	w->gui_anime_sensitive_pause (GSGL_TRUE);
	w->gui_anime_sensitive_replay (GSGL_FALSE);
	gsgl_no_select_objects (w->Page);
	gsgl_gtk_forbit_selection ();

	i = gsgl_inq_current_frame_num (w->Page);
	if (i <= 1) {
		i = gsgl_anime_inq_frame_num (w->Page)+1;
		gsgl_set_current_frame_num (i, w->Page);
	}

	w->Handler_anime = gtk_timeout_add (action, gsgl_anime_reverse_replay_frame, w);
	gtk_main ();
	gtk_timeout_remove (w->Handler_anime);
	w->gui_anime_sensitive_pause (GSGL_FALSE);
	w->gui_anime_sensitive_replay (GSGL_TRUE);
	gsgl_yes_select_objects (w->Page);
	gsgl_gtk_allow_selection ();
}

void gsgl_gtk_anime_pause (gpointer gdata, guint action, GtkWidget *widget)
{
	gsglMainWindow *w = (gsglMainWindow *) gdata;

	w->gui_anime_sensitive_pause (GSGL_FALSE);
	w->gui_anime_sensitive_replay (GSGL_TRUE);
	gtk_timeout_remove (w->Handler_anime);
	gsgl_yes_select_objects (w->Page);
	gsgl_gtk_allow_selection ();
}

void gsgl_gtk_anime_forward (gpointer gdata, guint action, GtkWidget *widget)
{
	gui_anime_forward ((gsglMainWindow *) gdata);
}

void gsgl_gtk_anime_backward (gpointer gdata, guint action, GtkWidget *widget)
{
	gui_anime_backward ((gsglMainWindow *) gdata);
}
