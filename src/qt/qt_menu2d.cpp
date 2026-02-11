#include <QMenuBar>
#include "qt_gsglwindow.h"
#include "qt_dialog.h"
#include "gsaf.h"
#include "gsgl.h"
#include "xwin.h"
#include "callback.h"
#include "gui_common.h"

void gsglMainWindow::qt_2d_restore_enabled (bool b)
{
	if (id_obj[5]) id_obj[5]->setEnabled (b);
	if (id_pop_obj[5]) id_pop_obj[5]->setEnabled (b);
}

void gsglMainWindow::gui_2d_init_every_page ()
{
	qt_2d_restore_enabled (false);
}

void gsglMainWindow::qt_2d_restore ()
{
	gsgl_2d_restore (Page);
	qt_2d_restore_enabled (false);
	gui_cursor_default (this);
}

void gsglMainWindow::qt_2d_trans ()
{
	gsgl_no_select_objects (Page);
	gsgl_2d_trans (Page);
	qt_2d_restore_enabled (true);
	gui_cursor_translate (this);
}

void gsglMainWindow::qt_2d_scale (bool with_question)
{
	gsgl_no_select_objects (Page);

	if (with_question) {
		float x, y;
		int i, j, k;

		gsgl_utility_clean (Page);

		gsglScaleDialog * d = new gsglScaleDialog (Page, 2, this, true);
		if (d->exec () == QDialog::Accepted) {
			d->get_result(&i, &j, &k);
			x = i / 10.0;
			y = j / 10.0;
			gsgl_2d_scale (x, y, Page);	
			qt_2d_restore_enabled (true);
		}
		delete d;
	} else {
		gsgl_2d_scale (GSGL_PAPER_WIDTH*0.5, GSGL_PAPER_HEIGHT*0.5, Page);
		qt_2d_restore_enabled (true);
	}
	gui_cursor_scale (this);
}

void gsglMainWindow::qt_2d_rotate (bool with_question)
{
	/*QtWidget *window;
	  QtObject *adjust;*/
	
	gsgl_no_select_objects (Page);
	
	if (with_question) {
		float x = GSGL_PAPER_WIDTH*0.5;
		float y = GSGL_PAPER_HEIGHT*0.5;
		
		gsgl_utility_clean (Page);
		/*x = qt_spin_button_get_value_as_float (QT_SPIN_BUTTON(Spin_x));
		  y = qt_spin_button_get_value_as_float (QT_SPIN_BUTTON(Spin_y));*/
		gsgl_2d_rotate (x, y, Page);
		qt_2d_restore_enabled (true);
	} else {
		gsgl_2d_rotate (GSGL_PAPER_WIDTH*0.5, GSGL_PAPER_HEIGHT*0.5, Page);
		qt_2d_restore_enabled (true);
	}
	gui_cursor_rotate (this);
}

void gsglMainWindow::gui_2d_sensitive (bool torf)
{
}

void gsglMainWindow::qt_2d_select_obj ()
{
	gui_2d_sensitive (true);
	gui_3d_sensitive (false);

	gsgl_yes_select_objects (Page);
	gsgl_2d_select_objects (Page);
}
