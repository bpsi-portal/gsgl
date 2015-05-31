#include <qmenubar.h>
#include <qinputdialog.h>
#include "qt_gsglwindow.h"
#include "qt_dialog.h"
#include "gsaf.h"
#include "gsgl.h"
#include "xwin.h"
#include "callback.h"
#include "qt_menuedit.h"
#include "qt_xwin.h"
#include "gui_common.h"
#include "qt_dialog.h"
#include "utils.h"
#include "tcpip.h"
#include "tcp_guiitems.h"

#define TR(x) QString::fromUtf8(_(x))

static char *fonts[] = {
	"0  Stroke",
	"1  Hardware",
	"2  Roman",
	"3  Roman Italic",
	"4  Roman Bold",
	"5  Roman Bold Italic",
	"6  Sans Serif",
	"7  Script",
	"8  German1",
	"9  German2",
	"10  German3",
	"11  Greek",
	"12  German1",
	"13  German1",
	"14  Cyrillic",
	"15  Cyrillic",
	"16  Large Symbol",
	"17  Various symbols",
	"18  Various symbols",
	"19  Various symbols",
	"32 Times Roman",
	"33 Times Italic",
	"34 Times Bold",
	"35 Times Bold Italic",
	"36 Helvetica",
	"37 Helvetica Oblique",
	"38 Helvetica Bold",
	"39 Helvetica Bold Oblique",
	"40 Courier",
	"41 Courier Oblique",
	"42 Courier Bold",
	"43 Courier Bold Oblique",
	"44 Symbol"
};

void gsglMainWindow::add_lines (void)
{
}

void gsglMainWindow::add_texts (void)
{
	/*char *just[4];
	  just[0] = gsgl_strdup(TR("0 Left"));
	  just[1] = gsgl_strdup(TR("1 Right"));
	  just[2] = gsgl_strdup(TR("2 Center"));
	  just[3] = gsgl_strdup(TR("3 Point Arithmetic"));*/
	char *just[] = {
		"0 Left",
		"1 Right",
		"2 Center",
		"3 Point Arithmetic"};

	if (Add_text && Add_text_dialog) return;
	
	gsgl_gui_pointer (Page, GSGL_FALSE);
	gui_cursor_pencil (this);
	Add_text = GSGL_TRUE;

	gsglAddTexts *a = new gsglAddTexts(Page, fonts, 33, just, 4, this, NULL, FALSE, Qt::WDestructiveClose);
	this->Add_text_dialog = gsgl_list_append (this->Add_text_dialog, a);

	int x = (int) (pos().x() + size().width() * 0.85);
	int y = (int) (pos().y() + size().height() * 0.15);
	a->move (x, y);
	a->show ();
}

void gsgl_add_text_dialog (float x, float y, gsglPage *p)
{
	bool ok = FALSE;
        QString text = QInputDialog::getText(TR("Text"), NULL, QLineEdit::Normal, 
					     NULL, &ok, gsgl_inq_mainwin());

	if (ok && !text.isEmpty()) {
		gsgllist *l = gsgl_inq_mainwin()->Add_text_dialog;
		gsglAddTexts *a;

		if (l && l->data) {
			a = (gsglAddTexts *) l->data;
		} else
			return;
		
		char *str = gsgl_strdup(text.latin1());
		int fnt = a->combo_font->currentItem ();
		if (fnt >= 20)
			fnt += 12;
		float size = a->spin_size->value()*0.01;
		float angle = a->spin_angle->value();
		int ijust = a->combo_just->currentItem ();

		if (gsgl_inq_i_am_a_server (p->library)) {
			gsgl_tcp_gui_add_text (str, x, y, fnt, size, angle, ijust, p);
		} else {
			gsgl_gui_add_text_client (str, x, y, fnt, size, angle, ijust, p);
		}
	}
}

void gsgl_gui_close_add_text_dialog (void)
{
	gsglMainWindow *w = gsgl_inq_mainwin ();

	if (w->Add_text && w->Add_text_dialog) {
                gsgllist *l = w->Add_text_dialog;
                gsglAddTexts *a = (gsglAddTexts *) l->data;
                delete a;
		w->Add_text = GSGL_FALSE;
		w->Add_text_dialog = NULL;
        }

}
