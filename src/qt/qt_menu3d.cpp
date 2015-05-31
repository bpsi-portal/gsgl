#include <qmenubar.h>
#include "qt_gsglwindow.h"
#include "qt_dialog.h"
#include "gsaf.h"
#include "gsgl.h"
#include "base.h"
#include "xwin.h"
#include "callback.h"
#include "displaylist.h"
#include "gui_common.h"

void gsglMainWindow::qt_3d_restore_enabled (bool b)
{
	objectsMenu->setItemEnabled (id_obj[6], b);
        contextMenu->setItemEnabled (id_pop_obj[6], b);
}

void gsglMainWindow::gui_3d_init_every_page ()
{
	qt_3d_restore_enabled (FALSE);
}

void gsglMainWindow::qt_3d_restore ()
{
	if (!gsgl_displaylist_inq_remade (Page)) return;
	gsgl_3d_restore (Page);
	qt_3d_restore_enabled (FALSE);
	gui_cursor_default (this);
}

void gsglMainWindow::qt_3d_trans_z ()
{
	gsgl_no_select_objects (Page);
        gsgl_3d_trans_z (Page);  
	qt_3d_restore_enabled (TRUE);
	gui_cursor_translate_z (this);
}

void gsglMainWindow::qt_3d_trans_xy ()
{
	gsgl_no_select_objects (Page);
	gsgl_3d_trans_xy (Page);
	qt_3d_restore_enabled (TRUE);
	gui_cursor_translate (this);
}

void gsglMainWindow::qt_3d_scale (bool with_question)
{
	gsgl_no_select_objects (Page);

	if (with_question) {
		float x, y, z;
		int i, j, k;
	
		gsgl_utility_clean (Page);

		gsglScaleDialog * d = new gsglScaleDialog (Page, 3, this, "3d scale", TRUE);
		if (d->exec () == QDialog::Accepted) {
			d->get_result(&i, &j, &k);
			x = i / 10.0;
			y = j / 10.0;
			z = k / 10.0;
			gsgl_3d_scale (x, y, z, Page);
			qt_3d_restore_enabled (TRUE);
		}
		delete d;
	} else {
		gsgl_3d_scale(GSGL_PAPER_WIDTH*0.5, GSGL_PAPER_HEIGHT*0.5, gsgl_inq_depth3d (Page), Page);
		qt_3d_restore_enabled (TRUE);
	}
	gui_cursor_scale (this);
}

void gsglMainWindow::qt_3d_rotate (bool with_question)
{
	gsgl_no_select_objects (Page);

	if (with_question) {
	} else {
		gsgl_3d_rotate (GSGL_PAPER_WIDTH*0.5, GSGL_PAPER_HEIGHT*0.5, gsgl_inq_depth3d (Page), Page);
		qt_3d_restore_enabled (TRUE);
	}
	gui_cursor_rotate (this);
}

void gsglMainWindow::gui_3d_sensitive (bool b)
{
	objectsMenu->setItemEnabled (id_obj[3], b);
	objectsMenu->setItemEnabled (id_obj[4], b);
	contextMenu->setItemEnabled (id_pop_obj[3], b);
	contextMenu->setItemEnabled (id_pop_obj[4], b);
}

void gsglMainWindow::gui_sensitive_have_3d (bool b)
{
	editMenu->setItemEnabled (id_edit[2], b);
	contextMenu->setItemEnabled (id_pop_edit[2], b);
}

void gsglMainWindow::qt_3d_select_obj ()
{
	gui_3d_sensitive (TRUE);
	gui_2d_sensitive (FALSE);

	gsgl_yes_select_objects (Page);
	gsgl_3d_select_objects (Page);
}

void gsglMainWindow::lighting()
{
	gsgl_lighting_toggle (Page);
}
