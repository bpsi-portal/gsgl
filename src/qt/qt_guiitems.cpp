#include <QApplication>
#include <QValidator>
#include <QPalette>
#include "qt_gsglwindow.h"
#include "qt_drawarea.h"
#include "gui_common.h"
#include "qt_guiitems.h"
#include "utils.h"
#include "guiitems.h"
#include "tcpip.h"
#include "tcp_guiitems.h"
#include "configfile.h"
#include "xwin.h"
#include "gsgl.h"
#include "mem_list.h"
#include "drv_file.h"
#include "drv_wrapper.h"
#include "displaylist.h"
#include "qt_xwin.h"

void gui_guiitems_init_every_page ()
{
}

void gui_guiitems_finish_every_page (gsglPage *p)
{
	gsgllist *list1;
	itemnode *node;

	list1 = gsgl_guiitem_inq_list (p);
	while (list1 != NULL) {
		node = (itemnode *) list1->data;
		if (node->toolkit == TK_QT && node->widget)
			delete (QWidget *) node->widget;
		list1 = list1->next;
	}
}

static void setfontsize (QWidget *widget, float chh, gsglPage *p)
{
	float dpi = gsgl_get_screen_resolution (p->library);
	QFont f = widget->font();	
	float pointSize = gsgl_inq_window_width (p->win) / GSGL_PAPER_WIDTH / dpi * 72.0;
	f.setPointSizeF (chh * 1.46667 * pointSize);
	widget->setFont (f);
}

static void apply_widget_colors (QWidget *widget, bool black_bg)
{
	if (!widget)
		return;
	if (black_bg)
		widget->setStyleSheet("background-color: black; color: white;");
	else
		widget->setStyleSheet("background-color: white; color: black;");
}

void gui_guiitems_configure (gsglPage *p)
{
	gsgllist *list1;
	itemnode *node;
	QWidget *widget;
	int x, y, w, h;
	
	list1 = gsgl_guiitem_inq_list (p);
	while (list1 != NULL) {
		node = (itemnode *) list1->data;
		if (node->toolkit == TK_QT) {
			gsgl_to_x11 (node->gdata->common->x, node->gdata->common->y, node->gdata->common->w, node->gdata->common->h, &x, &y, &w, &h, node->gdata->common->p);
			widget = (QWidget *) node->widget;
			widget->setGeometry (QRect(x, y, w, h));
			setfontsize (widget, node->gdata->common->chh, node->gdata->common->p);
		}
		list1 = list1->next;
	}
}

/* Label */

void gui_add_label_user (int x, int y, int w, int h, guialldata *data)
{
	labeldata *s = (labeldata *) data->specific; 

	UserLabel *label = new UserLabel (data, s->text, gsgl_inq_mainwin()->gl);
	label->setGeometry (QRect(x, y, w, h));

	if (gsgl_inq_bg_color (data->common->p->library) == GSGL_BACK_COLOR_BLACK) {
		apply_widget_colors (label, true);
        } else {
		apply_widget_colors (label, false);
        }

	if (s->ijust == JUST_LEFT)
		label->setAlignment (Qt::AlignLeft| Qt::AlignVCenter);
	else if (s->ijust == JUST_RIGHT)
		label->setAlignment (Qt::AlignRight | Qt::AlignVCenter);
	else if (s->ijust == JUST_CENTER)
		label->setAlignment (Qt::AlignHCenter | Qt::AlignVCenter);
	
	label->show ();	
	gsgl_guiitem_append_item (TK_QT, data, label, NULL);
}

UserLabel::UserLabel (guialldata *data, const QString &text, QWidget *parent)
	: QLabel (text, parent)
{
	alldata = data;
	setfontsize (this, data->common->chh, data->common->p);
}

void gui_inq_label_size (const char *text, float *w, float *h, gsglPage *p)
{
	float dpi = gsgl_get_screen_resolution (p->library);
	QFont f = qApp->font();	
	float pointSize = gsgl_inq_window_width (p->win) / GSGL_PAPER_WIDTH / dpi * 72.0;
	float chh, chw, chsp, angl, tilt;
	gsaf_inqchr (&chh, &chw, &chsp, &angl, &tilt);
	f.setPointSizeF (chh * 1.46667 * pointSize);
	
	QFontMetrics fm (f);
	QRect rec = fm.boundingRect (text); /* text size in pixel */
	
	*w = rec.width () / (float) gsgl_inq_window_width (p->win) * GSGL_PAPER_WIDTH;
	*h = rec.height ()/ (float) gsgl_inq_window_height(p->win) * GSGL_PAPER_HEIGHT;
}

/* Push Button */

void gui_add_button_user (int x, int y, int w, int h, guialldata *data)
{
	buttondata *s = (buttondata *) data->specific; 

	UserButton *button = new UserButton (data, s->text, gsgl_inq_mainwin()->gl);
	button->setGeometry (QRect(x, y, w, h));
	button->connect_callback ();
	button->show ();
	
	gsgl_guiitem_append_item (TK_QT, data, button, NULL);
}

UserButton::UserButton (guialldata *data, const QString &text, QWidget *parent)
	: QPushButton (text, parent)
{
	alldata = data;
	setfontsize (this, data->common->chh, data->common->p);
}

void UserButton::connect_callback ()
{
	connect (this, SIGNAL(clicked()), this, SLOT(userclicked()));
}

void UserButton::userclicked ()
{
	gsgl_guiitems_button_clicked (alldata, this);
}

/* ComboBox */

void gui_add_combobox_user (int x, int y, int w, int h, guialldata *data)
{
	int i;
	UserComboBox *box = new UserComboBox (data, gsgl_inq_mainwin()->gl);
	comboboxdata *s = (comboboxdata *) data->specific;

	for (i=0; i<s->items; i++) 
		box->addItem (s->text[i]);

	box->setCurrentIndex (s->current);

	box->setGeometry (QRect(x, y, w, h));
	box->connect_callback ();
	box->show ();

	gsgl_guiitem_append_item (TK_QT, data, box, NULL);
}

UserComboBox::UserComboBox (guialldata *data, QWidget *parent)
	: QComboBox (parent)
{
	alldata = data;
	setEditable (false);
	setfontsize (this, data->common->chh, data->common->p);
}

void UserComboBox::connect_callback ()
{
	connect (this, SIGNAL(activated(int)), this, SLOT(useractivated(int)));
}

void UserComboBox::useractivated (int i)
{
	comboboxdata *s = (comboboxdata *) alldata->specific;
	s->current = i;
	gsgl_guiitems_combobox_selected (alldata, this);
}

/* SpinButtonf */

void gui_add_spinbuttonf_user (int x, int y, int w, int h, guialldata *data)
{
	UserSpinButtonf *spin = new UserSpinButtonf (data, gsgl_inq_mainwin()->gl);
	spin->setGeometry (QRect(x, y, w, h));

	spinbuttondataf *s = (spinbuttondataf *) data->specific;

	if (s->digits == 0) {
		spin->setMinimum ((int) s->lower);
		spin->setMaximum ((int) s->upper);
		spin->setValue ((int) s->init);
	} else {
		double factor = pow(10.0, s->digits);
		spin->setMinimum ((int) (s->lower * factor + 0.5));
		spin->setMaximum ((int) (s->upper * factor + 0.5));
		spin->setValue ((int) (s->init * factor + 0.5));
		
		QDoubleValidator *val = new QDoubleValidator (s->lower, s->upper, s->digits, spin);
		(void)val;
	}
	spin->connect_callback ();
	spin->show ();	

	gsgl_guiitem_append_item (TK_QT, data, spin, NULL);
}

UserSpinButtonf::UserSpinButtonf (guialldata *data, QWidget * parent)
	: QSpinBox (parent)
{
	alldata = data;
	setfontsize (this, data->common->chh, data->common->p);
}

void UserSpinButtonf::connect_callback()
{
	connect (this, SIGNAL(valueChanged(int)), this, SLOT(uservalueChanged(int)));
}

void UserSpinButtonf::uservalueChanged (int i)
{
	spinbuttondataf *s = (spinbuttondataf *) alldata->specific;

	float f;
	if (s->digits == 0) {
		f = (float) i;
	} else {
		double factor = pow(10.0, -(s->digits));
		f = (float) (i * factor);
	}
	s->init = f;

	gsgl_guiitems_spinbuttonf_changed (alldata, this);
}

QString UserSpinButtonf::textFromValue (int value) const
{
	spinbuttondataf *s = (spinbuttondataf *) alldata->specific;
	
	if (s->digits == 0) {
		return QString::number (value);
	} else {
		double factor = (int) pow (10.0, s->digits);
		return QString::number (value / factor, 'f', s->digits);
	}
}

int UserSpinButtonf::valueFromText (const QString &text) const
{
	spinbuttondataf *s = (spinbuttondataf *) alldata->specific;

	if (s->digits == 0)
		return text.toInt ();
	else {
		int factor = (int) pow (10.0, s->digits);
		return (int) ( factor * text.toFloat() + 0.5);
	}
}

/* SpinButtoni */

void gui_add_spinbuttoni_user (int x, int y, int w, int h, guialldata *data)
{
	UserSpinButtoni *spin = new UserSpinButtoni (data, gsgl_inq_mainwin()->gl);
	spin->setGeometry (QRect(x, y, w, h));

	spinbuttondatai *s = (spinbuttondatai *) data->specific;

	spin->setMinimum (s->lower);
	spin->setMaximum (s->upper);
	spin->setValue (s->init);
	spin->connect_callback ();
	spin->show ();	

	gsgl_guiitem_append_item (TK_QT, data, spin, NULL);
}

UserSpinButtoni::UserSpinButtoni (guialldata *data, QWidget * parent)
	: QSpinBox (parent)
{
	alldata = data;
	setfontsize (this, data->common->chh, data->common->p);
}

void UserSpinButtoni::connect_callback()
{
	connect (this, SIGNAL(valueChanged(int)), this, SLOT(uservalueChanged(int)));
}

void UserSpinButtoni::uservalueChanged (int i)
{
	spinbuttondatai *s = (spinbuttondatai *) alldata->specific;
	s->init = i;
	gsgl_guiitems_spinbuttoni_changed (alldata, this);
}

/* CheckBox */

void gui_add_checkbutton_user (int x, int y, int w, int h, guialldata *data)
{
	checkbuttondata *s = (checkbuttondata *) data->specific;

	UserCheckButton *button = new UserCheckButton (data, s->text, gsgl_inq_mainwin()->gl);
	if (s->init)
		button->setChecked (true);
	else
		button->setChecked (false);
	button->setGeometry (QRect(x, y, w, h));

	if (gsgl_inq_bg_color (data->common->p->library) == GSGL_BACK_COLOR_BLACK) {
		apply_widget_colors (button, true);
        } else {
		apply_widget_colors (button, false);
        }

	button->connect_callback ();
	button->show ();

	gsgl_guiitem_append_item (TK_QT, data, button, NULL);
}

UserCheckButton::UserCheckButton (guialldata *data, const QString & text, QWidget * parent)
	: QCheckBox (text, parent)
{
	alldata = data;
	setfontsize (this, data->common->chh, data->common->p);
}

void UserCheckButton::connect_callback ()
{
	connect (this, SIGNAL(toggled(bool)), this, SLOT(usertoggled(bool)));
}

void UserCheckButton::usertoggled (bool b)
{
	int i;
	if (b)
		i = 1;
	else
		i = 0;
	
	checkbuttondata *s = (checkbuttondata *) alldata->specific;
	s->init = i;

	gsgl_guiitems_checkbutton_toggled (alldata, this);
}

/* LineEdit */

void gui_add_lineedit_user (int x, int y, int w, int h, guialldata *data)
{
	lineeditdata *s = (lineeditdata *) data->specific;

	UserLineEdit *edit = new UserLineEdit (data, s->text, gsgl_inq_mainwin()->gl);
	edit->setGeometry (QRect(x, y, w, h));
	edit->setMaxLength (s->max);
	edit->setText (s->text);
	edit->connect_callback ();
	edit->show ();

	gsgl_guiitem_append_item (TK_QT, data, edit, NULL);
}

UserLineEdit::UserLineEdit (guialldata *data, const QString & contents, QWidget * parent)
	: QLineEdit (contents, parent)
{
	alldata = data;
	setfontsize (this, data->common->chh, data->common->p);
}

void UserLineEdit::connect_callback()
{
	connect (this, SIGNAL(textChanged(const QString &)), this, SLOT(usertextChanged(const QString &)));
}

void UserLineEdit::usertextChanged (const QString &str)
{
	const char *s =  str.toUtf8().constData();
	lineeditdata *e = (lineeditdata *) alldata->specific;
	e->text = gsgl_strdup (s);

	gsgl_guiitems_lineedit_changed (alldata, this);
}
