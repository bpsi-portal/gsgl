#include <QMenuBar>
#include "qt_gsglwindow.h"
#include "qt_dialog.h"
#include "gsaf.h"
#include "gsgl.h"
#include "xwin.h"
#include "callback.h"
#include "displaylist.h"
#include "gui_common.h"

void gsglMainWindow::qt_projection_restore_enabled (bool b)
{
	if (id_pro[3]) id_pro[3]->setEnabled (b);
	if (id_pop_pro[3]) id_pop_pro[3]->setEnabled (b);
}

void gsglMainWindow::gui_projection_init_every_page ()
{
	qt_projection_restore_enabled (false);
}

void gsglMainWindow::qt_proj_restore ()
{
	gsgl_proj_restore (Page);
	qt_projection_restore_enabled (false);
	gui_cursor_default (this);
}

void gsglMainWindow::qt_proj_translate ()
{
	gsgl_no_select_objects (Page);
	gsgl_displaylist_remake (Page);
	gsgl_proj_trans (Page);
	qt_projection_restore_enabled (true);
	gui_cursor_translate (this);
}

void gsglMainWindow::qt_proj_scale (bool with_question)
{
	gsgl_no_select_objects (Page);
	gsgl_displaylist_remake (Page);
	
	if (with_question) {
		float x, y;
		int i, j, k;

		gsgl_utility_clean (Page);

		gsglScaleDialog * d = new gsglScaleDialog (Page, 2, this, true);
		if (d->exec () == QDialog::Accepted) {
			d->get_result(&i, &j, &k);
			x = i / 10.0;
			y = j / 10.0;
			gsgl_proj_scale (x, y, Page);	
			qt_projection_restore_enabled (true);
		}
		delete d;
	} else {
		gsgl_proj_scale (GSGL_PAPER_WIDTH*0.5, GSGL_PAPER_HEIGHT*0.5, Page);
		qt_projection_restore_enabled (true);
	}
	gui_cursor_scale (this);
}

void gsglMainWindow::qt_proj_rotate (bool with_question)
{
	gsgl_no_select_objects (Page);
	gsgl_displaylist_remake (Page);

	if (with_question) {
		/*---------------*/
		gsgl_utility_cross (GSGL_PAPER_WIDTH*0.5, -1, -1, Page);
		gsgl_utility_cross (-1, GSGL_PAPER_HEIGHT*0.5, -1, Page);
		gsgl_utility_cross (-1, -1, 0.0, Page);
		/*---------------*/
		qt_projection_restore_enabled (true);
	} else {
		gsgl_proj_rotate (GSGL_PAPER_WIDTH*0.5, GSGL_PAPER_HEIGHT*0.5, Page);
		qt_projection_restore_enabled (true);
	}
	gui_cursor_rotate (this);
}

void gsglMainWindow::qt_proj_sensitive (bool b)
{
	int i;
	
	for (i=0; i<3; i++) {
		if (id_pro[i]) id_pro[i]->setEnabled (b);
		if (id_pop_pro[i]) id_pop_pro[i]->setEnabled (b);
	}
}
