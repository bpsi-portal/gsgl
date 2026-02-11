#include "qt_compat.h"
#include "qt_gsglwindow.h"
#include <QApplication>
#include <QGuiApplication>
#include <QInputDialog>
#include <QColorDialog>
#include <QIcon>
#include <QScreen>
#include "qt_dialog.h"
#include "gsaf.h"
#include "gsgl.h"
#include "base.h"
#include "callback.h"
#include "configfile.h"
#include "xwin.h"
#include "gui_common.h"
#include "qt_xwin.h"

#define TR(x) QString::fromUtf8(_(x))

// gsglDialog

gsglDialog::gsglDialog(gsglPage *page, bool with_default1, QWidget* parent, bool modal, WFLAGS fl)
	: QDialog (parent, fl)
{
	p = page;
	with_default = with_default1;
	setModal (modal);
	vbox = new QVBoxLayout (this);
	vbox->setContentsMargins (11, 11, 11, 11);
	vbox->setSpacing (6);
}

void gsglDialog::addbuttons ()
{
	buttonbox = new QHBoxLayout();
	buttonbox->setSpacing(6);
	vbox->addLayout( buttonbox );
	
	okPushButton = new QPushButton( TR("OK"), this );
	buttonbox->addWidget( okPushButton );
	
	cancelPushButton = new QPushButton( TR("Cancel"), this );
	if (with_default) {
		defaultPushButton = new QPushButton( TR("Default"), this );
		buttonbox->addWidget( defaultPushButton );
	}
	buttonbox->addWidget( cancelPushButton );
	
	// signals and slots connections
	connect( okPushButton, SIGNAL(clicked()), this, SLOT(my_accept()));
	connect( cancelPushButton, SIGNAL(clicked()), this, SLOT(my_reject()));
	if (with_default)
		connect( defaultPushButton, SIGNAL(clicked()), this, SLOT(setdefault()));
}

void gsglDialog::addbuttons_close (void)
{
	buttonbox = new QHBoxLayout();
	buttonbox->setSpacing(6);
	vbox->addLayout( buttonbox );
	
	cancelPushButton = new QPushButton( TR("Close"), this );
	buttonbox->addWidget( cancelPushButton );
	
	// signals and slots connections
	connect( cancelPushButton, SIGNAL(clicked()), this, SLOT(my_reject()));
}

void gsglDialog::my_accept ()
{
	accept ();
}

void gsglDialog::my_reject ()
{
	reject ();
}

void gsglDialog::setdefault ()
{
}

// MySpinBox

MySpinBox::MySpinBox (int digits1, int minValue, int maxValue, int step, QWidget *parent)
	: QSpinBox (parent)
{
	setRange (minValue, maxValue);
	setSingleStep (step);
	digits = digits1;
	if (digits > 0) {
		factor = pow (10.0, digits);
		val = new QDoubleValidator (minValue/factor, maxValue/factor, 1, this);
	} else {
		factor = 1.0;
		val = NULL;
	}
}

QString MySpinBox::textFromValue( int value ) const {
	if (digits > 0) {
		return QString::number (value / factor, 'f', digits);
	} else {
		return QString::number (value);
	}
}

int MySpinBox::valueFromText( const QString &text ) const {
	if (digits > 0)
		return (int) ( factor * text.toFloat() + 0.5);
	else
		return text.toInt();
}

// gsglScaleDialog

gsglScaleDialog::gsglScaleDialog(gsglPage *p1, int dim1, QWidget* parent, 
				 bool modal, WFLAGS fl )
	: gsglDialog( p1, true, parent, modal, fl )
{
	dim = dim1;

	caption = new QLabel ( TR("Center of Scaling : "), this);
	vbox->addWidget( caption );

	// x
	xbox = new QHBoxLayout();
	vbox->addLayout( xbox );
	x = new QLabel ( TR("x : "), this);
	//x->setScaledContents (false);
	//x->setSizePolicy (QSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum));
	xbox->addWidget( x );
	spinx = new MySpinBox (1, 0, (int)(GSGL_PAPER_WIDTH*10.0), 1, this);
	xbox->addWidget( spinx );
	mm1 = new QLabel ( TR("cm"), this);
	xbox->addWidget( mm1 );
	// y
	ybox = new QHBoxLayout();
	vbox->addLayout( ybox );
	y = new QLabel ( TR("y : "), this);
	ybox->addWidget( y );
	spiny = new MySpinBox (1, 0, (int)(GSGL_PAPER_HEIGHT*10.0), 1, this);
	ybox->addWidget( spiny );
	mm2 = new QLabel ( TR("cm"), this);
	ybox->addWidget( mm2 );
	// z
	if (dim == 3) {
		zbox = new QHBoxLayout();
		vbox->addLayout( zbox );
		z = new QLabel ( TR("depth : "), this);
		zbox->addWidget( z );
		spinz = new MySpinBox (1, 0, (int)(GSGL_VIEW_DEPTH * 10.0), 1, this);
		zbox->addWidget( spinz );
		mm3= new QLabel ( TR("cm"), this);
		zbox->addWidget( mm3 );
	}

	connect( spinx, SIGNAL(valueChanged(int)), this, SLOT(cross_x(int))); 
	connect( spiny, SIGNAL(valueChanged(int)), this, SLOT(cross_y(int))); 
	if (dim == 3) {
		connect( spinz, SIGNAL(valueChanged(int)), this, SLOT(cross_z(int))); 
	} else {
		gsgl_utility_cross (-1, -1, 0.0, p);
	}
	setdefault ();

	addbuttons ();
}

void gsglScaleDialog::get_result (int *i, int *j, int *k)
{
	*i = spinx->value();
	*j = spiny->value();
	if (dim == 3) {
		*k = spinz->value();
	} else {
		*k = 0;
	}
}

void gsglScaleDialog::setdefault ()
{
	spinx->setValue((int)(GSGL_PAPER_WIDTH*5.0));
	spiny->setValue((int)(GSGL_PAPER_HEIGHT*5.0));
	if (dim == 3) {
		spinz->setValue((int)(gsgl_inq_depth3d(p) * 10.0));
	}
}

void gsglScaleDialog::my_accept ()
{
	gsgl_utility_clean (p);
	accept ();
}

void gsglScaleDialog::my_reject ()
{
	gsgl_utility_clean (p);
	reject ();
}

void gsglScaleDialog::cross_x (int i)
{
	gsgl_utility_cross (i * 0.1, -1, -1, p);
}

void gsglScaleDialog::cross_y (int i)
{
	gsgl_utility_cross (-1, i * 0.1, -1, p);
}

void gsglScaleDialog::cross_z (int i)
{
	gsgl_utility_cross (-1, -1, i * 0.1, p);
}

// ColorButton

ColorButton::ColorButton (QWidget *parent, int r, int g, int b)
	: QPushButton ( parent ) 
{
	c.setRgb (r, g, b);
	p = QPixmap(100, 20);
	setcolor (c);

	connect( this, SIGNAL(clicked()), this, SLOT(select_color()));	
}

void ColorButton::setcolor (QColor c1)
{
	c = c1;
	p.fill (c1);
	setIcon(QIcon(p));
	setIconSize(p.size());
}

void ColorButton::select_color ()
{
	QColor c2;
	c2 = QColorDialog::getColor (c, this);
	if (c2.isValid()) {
		setcolor (c2);
	}
}

// gsglColorDialog

gsglColorDialog::gsglColorDialog (gsglPage *p1, QWidget *parent, bool modal, WFLAGS fl)
	: gsglDialog( p1, true, parent, modal, fl )
{
	// background color
	back = new QLabel ( TR("background color:"), this);
	vbox->addWidget( back );
	combobox = new QComboBox (this);
	combobox->setEditable (false);
	combobox->addItem (TR("black"));
	combobox->addItem (TR("white"));
	combobox->setCurrentIndex (gsgl_inq_bg_color (p->library));
	vbox->addWidget( combobox );

	gsglConfigColor *configcolor = gsgl_inq_config_color (p->library);
	int r1 = (int) (255.0*configcolor->box[0]);
	int g1 = (int) (255.0*configcolor->box[1]);
	int b1 = (int) (255.0*configcolor->box[2]);
	int r2 = (int) (255.0*configcolor->frame[0]);
	int g2 = (int) (255.0*configcolor->frame[1]);
	int b2 = (int) (255.0*configcolor->frame[2]);
	// selecting box color
	boxlabel = new QLabel ( TR("selecting box color:"), this);
	vbox->addWidget( boxlabel );

	layout1 = new QHBoxLayout ();
	vbox->addLayout (layout1);
	box = new ColorButton (this, r1, g1, b1);
	layout1->addStretch (1);
	layout1->addWidget (box);

	// selected objects' color
	objectslabel = new QLabel ( TR("selected object's color:"), this);
	vbox->addWidget( objectslabel );

	layout2 = new QHBoxLayout ();
	vbox->addLayout (layout2);
	objects = new ColorButton (this, r2, g2, b2);

	layout2->addStretch (1);
	layout2->addWidget (objects);

	addbuttons ();
}

void gsglColorDialog::setdefault ()
{
	combobox->setCurrentIndex (GSGL_BACK_COLOR_WHITE);
	int r1 = (int) (DEFAULT_BOX_R * 255.0);
	int g1 = (int) (DEFAULT_BOX_G * 255.0);
	int b1 = (int) (DEFAULT_BOX_B * 255.0);
	int r2 = (int) (DEFAULT_FRAME_R * 255.0);
	int g2 = (int) (DEFAULT_FRAME_G * 255.0);
	int b2 = (int) (DEFAULT_FRAME_B * 255.0);
	QColor c1 (r1, g1, b1);
	QColor c2 (r2, g2, b2);
	box->setcolor (c1);
	objects->setcolor (c2);
}

void gsglColorDialog::get_result (int *bg, double boxcolor[3], double objcolor[3])
{
	*bg = combobox->currentIndex();

	QColor c1;
	c1 = box->inqcolor ();
	boxcolor[0] = c1.red () / 255.0;
	boxcolor[1] = c1.green () / 255.0;
	boxcolor[2] = c1.blue () / 255.0;

	QColor c2 = objects->inqcolor ();
	objcolor[0] = c2.red () / 255.0;
	objcolor[1] = c2.green () / 255.0;
	objcolor[2] = c2.blue () / 255.0;
}

// gsglCommandDialog

gsglCommandDialog::gsglCommandDialog (gsglPage *p1, QWidget *parent, bool modal, WFLAGS fl)
	: gsglDialog (p1, true, parent, modal, fl)
{
	int i;

	label[0] = new QLabel ("Print:", this);
	label[1] = new QLabel ("Browse manuals:", this);
	label[2] = new QLabel ("Export vector eps:", this);
	label[3] = new QLabel ("Export raster eps:", this);
	label[4] = new QLabel ("Export jpeg:", this);
	label[5] = new QLabel ("Export png:", this);

	//QString pr(gsgl_inq_print_command ());
	textedit[0] = new QLineEdit (gsgl_inq_print_command (p->library), this);
	textedit[1] = new QLineEdit (gsgl_inq_man_command (p->library), this);
	textedit[2] = new QLineEdit (gsgl_inq_eps_vec_command (p->library), this);
	textedit[3] = new QLineEdit (gsgl_inq_eps_ras_command (p->library), this);
	textedit[4] = new QLineEdit (gsgl_inq_jpg_command (p->library), this);
	textedit[5] = new QLineEdit (gsgl_inq_png_command (p->library), this);

	for (i=0; i<6; i++) {
		textedit[i]->resize( textedit[i]->sizeHint() );
		vbox->addWidget (label[i]);
		vbox->addWidget (textedit[i]);
	}
	addbuttons ();
}

void gsglCommandDialog::get_result ()
{
	gsgl_set_print_command (textedit[0]->text().toUtf8().constData(), p->library);
	gsgl_set_man_command (textedit[1]->text().toUtf8().constData(), p->library);
	gsgl_set_eps_vec_command (textedit[2]->text().toUtf8().constData(), p->library);
	gsgl_set_eps_ras_command (textedit[3]->text().toUtf8().constData(), p->library);
	gsgl_set_jpg_command (textedit[4]->text().toUtf8().constData(), p->library);
	gsgl_set_png_command (textedit[5]->text().toUtf8().constData(), p->library);
}

void gsglCommandDialog::setdefault ()
{
	textedit[0]->setText (COMMAND_PRINT);
	textedit[1]->setText (COMMAND_MAN);
	textedit[2]->setText (COMMAND_EPS_VEC);
	textedit[3]->setText (COMMAND_EPS_RAS);
	textedit[4]->setText (COMMAND_JPG);
	textedit[5]->setText (COMMAND_PNG);
}

// gsglWindowSizeDialog

gsglWindowSizeDialog::gsglWindowSizeDialog (gsglPage *p1, QWidget *parent, bool modal, WFLAGS fl)
	: gsglDialog (p1, true, parent, modal, fl)
{
	int i;

	label1[0] = new QLabel ("window width :", this);
	label1[1] = new QLabel ("window height :", this);
	label1[2] = new QLabel ("left-upper corner (x) :", this);
	label1[3] = new QLabel ("left-upper corner (y) :", this);

	QScreen *screen = QGuiApplication::primaryScreen();
	int width = screen ? screen->geometry().width() : GSGLBASIC_WINDOWWIDTH;
	int height = screen ? screen->geometry().height() : GSGLBASIC_WINDOWHEIGHT;
	spin[0] = new MySpinBox (0, 0, width, 1, this);
	spin[1] = new MySpinBox (0, 0, height, 1, this);
	spin[2] = new MySpinBox (0, 0, width, 1, this);
	spin[3] = new MySpinBox (0, 0, height, 1, this);

	spin[0]->setValue (gsgl_inq_userdefined_winwidth (p->library));
	spin[1]->setValue (gsgl_inq_userdefined_winheight (p->library));
	spin[2]->setValue (gsgl_inq_userdefined_corner_x (p->library));
	spin[3]->setValue (gsgl_inq_userdefined_corner_y (p->library));

	for (i=0; i<4; i++) {
		label2[i] = new QLabel ("pixel", this);
		layout[i] = new QHBoxLayout ();
		vbox->addLayout (layout[i]);
		layout[i]->addWidget( label1[i] );
		layout[i]->addWidget( spin[i] );
		layout[i]->addWidget( label2[i] );

	}
	addbuttons ();
}

void gsglWindowSizeDialog::get_result ()
{
	gsgl_set_userdefined_winwidth (spin[0]->value(), p->library);
	gsgl_set_userdefined_winheight (spin[1]->value(), p->library);
	gsgl_set_userdefined_corner_x (spin[2]->value(), p->library);
	gsgl_set_userdefined_corner_y (spin[3]->value(), p->library);
}

void gsglWindowSizeDialog::setdefault ()
{
	spin[0]->setValue(GSGLBASIC_WINDOWWIDTH);
	spin[1]->setValue(GSGLBASIC_WINDOWHEIGHT);
	spin[2]->setValue(0);
	spin[3]->setValue(0);
}

// gsglFontsDialog

gsglFontsDialog::gsglFontsDialog (gsglPage *p1, QWidget *parent, bool modal, WFLAGS f)
	: QDialog (parent, f)
{
	char *fontname[] = {
		"Alphabet default",
		"Math default",
		"Times Roman",
		"Times Italic",
		"Times Bold",
		"Times Bold Italic",
		"Arial Roman",
		"Arial Italic",
		"Arial Bold",
		"Arial Bold Italic",
		"Courier Roman",
		"Courier Italic",
		"Courier Bold",
		"Courier Bold Italic",
		"Greek default",
		"Greek Roman",
		"Greek Italic",
		"Greek Bold",
		"Greek Bold Italic",
		"Japanese default",
		"Japanese mincho",
		"Japanese gothic",
		"cmr10",
		"cmti10",
		"cmtt10",
		"cmmi10",
		"cmsy10",
		"cmex10",
		"Times Roman",
		"Times Italic",
                "Times Bold",
                "Times BoldItalic",
                "Helvetica",
                "Helvetica Oblique",
                "Helvetica Bold",
                "Helvetica BoldOblique",
                "Courier",
                "Courier Oblique",
                "Courier Bold",
                "Courier BoldOblique",
                "Symbol"
	};
	int fontnum[] = {
		1000,
		1001,
		1002,
		1003,
		1004,
		1005,
		1006,
		1007,
		1008,
		1009,
		1010,
		1011,
		1012,
		1013,
		1014,
		1015,
		1016,
		1017,
		1018,
		1019,
		1020,
		1021,
		1022,
		1023,
		1024,
		1025,
		1026,
		1027,
		502,
		503,
		504,
		505,
		506,
		507,
		508,
		509,
		510,
		511,
		512,
		513,
		514
	};
	int i;
	p = p1;

	setModal (modal);

	QVBoxLayout *mainLayout = new QVBoxLayout (this);
	tabs = new QTabWidget (this);
	mainLayout->addWidget (tabs);

	for (i=0; i<5; i++) {
		box[i] = new QWidget (tabs);
		grid[i] = new QGridLayout (box[i]);
		grid[i]->setContentsMargins (10, 10, 10, 10);
		grid[i]->setSpacing (10);
	}
	for (i=0; i<14; i++) {
		label11[i] = new QLabel (fontname[i], box[0]);
		label12[i] = new QLabel (QString( "(%1)" ).arg(fontnum[i]), box[0]);
		edit1[i] = new QLineEdit (gsgl_inq_ttfname (i, p->library), box[0]);
		grid[0]->addWidget (label11[i], i, 0);
		grid[0]->addWidget (label12[i], i, 1);
		grid[0]->addWidget (edit1[i], i, 2);
	}
	//QSpacerItem (1, 1, QSizePolicy::Minimum, QSizePolicy::Maximum);

	for (i=0; i<5; i++) {
		label21[i] = new QLabel (fontname[i+14], box[1]);
		label22[i] = new QLabel (QString( "(%1)" ).arg(fontnum[i+14]), box[1]);
		edit2[i] = new QLineEdit (gsgl_inq_ttfname (i, p->library), box[1]);
		grid[1]->addWidget (label21[i], i, 0);
		grid[1]->addWidget (label22[i], i, 1);
		grid[1]->addWidget (edit2[i], i, 2);
	}

	for (i=0; i<3; i++) {
		label31[i] = new QLabel (fontname[i+19], box[2]);
		label32[i] = new QLabel (QString( "(%1)" ).arg(fontnum[i+19]), box[2]);
		edit3[i] = new QLineEdit (gsgl_inq_ttfname (i, p->library), box[2]);
		grid[2]->addWidget (label31[i], i, 0);
		grid[2]->addWidget (label32[i], i, 1);
		grid[2]->addWidget (edit3[i], i, 2);
	}

	for (i=0; i<6; i++) {
		label41[i] = new QLabel (fontname[i+22], box[3]);
		label42[i] = new QLabel (QString( "(%1)" ).arg(fontnum[i+22]), box[3]);
		edit4[i] = new QLineEdit (gsgl_inq_ttfname (i, p->library), box[3]);
		grid[3]->addWidget (label41[i], i, 0);
		grid[3]->addWidget (label42[i], i, 1);
		grid[3]->addWidget (edit4[i], i, 2);
	}

	for (i=0; i<13; i++) {
		label51[i] = new QLabel (fontname[i+28], box[4]);
		label52[i] = new QLabel (QString( "(%1)" ).arg(fontnum[i+28]), box[4]);
		edit5[i] = new QLineEdit (gsgl_inq_ttfname (i, p->library), box[4]);
		grid[4]->addWidget (label51[i], i, 0);
		grid[4]->addWidget (label52[i], i, 1);
		grid[4]->addWidget (edit5[i], i, 2);
	}

	tabs->addTab (box[0], TR("Alphabet"));
	tabs->addTab (box[1], TR("Greek"));
	tabs->addTab (box[2], TR("Japanese"));
	tabs->addTab (box[3], TR("Bakoma"));
	tabs->addTab (box[4], TR("Type1"));

	buttonBox = new QDialogButtonBox (QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
	connect (buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
	connect (buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
	mainLayout->addWidget (buttonBox);
}

void gsglFontsDialog::get_result ()
{
	int i;
	
	for (i=0; i<14; i++)
		gsgl_set_ttfname (i, edit1[i]->text().toUtf8().constData(), p->library);
	for (i=0; i<5; i++)
		gsgl_set_ttfname (i+14, edit2[i]->text().toUtf8().constData(), p->library);
	for (i=0; i<3; i++)
		gsgl_set_ttfname (i+19, edit3[i]->text().toUtf8().constData(), p->library);
	for (i=0; i<6; i++)
		gsgl_set_ttfname (i+22, edit4[i]->text().toUtf8().constData(), p->library);
	for (i=0; i<13; i++)
		gsgl_set_ttfname (i+28, edit5[i]->text().toUtf8().constData(), p->library);
}

char * gui_popup_edit (char *input, char *title)
{
	bool ok = false;
	QString text = QInputDialog::getText(qApp->activeWindow(), title, QString(), QLineEdit::Normal,
					     input, &ok);

	if (ok && !text.isEmpty())
		return strdup(text.toUtf8().constData());
	else
		return NULL;
}

// gsglScaleDialogf

gsglSpinDialogf::gsglSpinDialogf (gsglPage *p1, int digits, float init, float lower, float upper, 
				  QWidget* parent, bool modal, WFLAGS fl )
	: gsglDialog( p1, false, parent, modal, fl )
{
	double factor;
	if (digits > 0)
		factor = pow (10.0, digits);
	else
		factor = 1.0;

	spin = new MySpinBox (digits, (int)(lower * factor + 0.5), (int) (upper * factor + 0.5), 1, this);
	spin->setValue ((int) (init * factor + 0.5));
	vbox->addWidget( spin );

	addbuttons ();
}

// gsglScaleDialogi

gsglSpinDialogi::gsglSpinDialogi (gsglPage *p1, int init, int lower, int upper, 
				  QWidget* parent, bool modal, WFLAGS fl )
	: gsglDialog( p1, false, parent, modal, fl )
{
	spin = new MySpinBox (0, lower, upper, 1, this);
	spin->setValue (init);
	vbox->addWidget (spin);

	addbuttons ();
}

float gui_popup_spinf (int digits, float init, float lower, float upper, gsglPage *p)
{
	double factor;
	float ret;

	if (digits > 0)
		factor = pow (10.0, digits);
	else
		factor = 1.0;

	gsglSpinDialogf *spin = new gsglSpinDialogf (p, digits, init, lower, upper, qApp->activeWindow());
	if (spin->exec () == QDialog::Accepted) {
		ret = spin->spin->value () / factor;
	} else {
		ret = upper + 1.0;
	}
	delete spin;

	return ret;
}

int gui_popup_spini (int init, int lower, int upper, gsglPage *p)
{
	int ret;

	gsglSpinDialogi *spin = new gsglSpinDialogi (p, init, lower, upper, qApp->activeWindow());
	if (spin->exec () == QDialog::Accepted) {
		ret = spin->spin->value ();
	} else {
		ret = upper + 1;
	}
	delete spin;

	return ret;
}

// gsglAddTexts

gsglAddTexts::gsglAddTexts (gsglPage *p, char *fonts[], int fontsnum, char *just[], int justnum, QWidget* parent, bool modal, WFLAGS fl)
	: gsglDialog (p, false, parent, modal, fl)
{
	int i;

	QGridLayout *grid = new QGridLayout();
	vbox->addLayout(grid);
	
	/* Font number */
	QLabel *label = new QLabel (TR("Font"), this);

	combo_font = new QComboBox (this);
	combo_font->setEditable(false);
	for (i=0; i<fontsnum; i++)
		combo_font->addItem (fonts[i]);
	combo_font->setCurrentIndex (0);
	grid->addWidget (label, 0, 0);
	grid->addWidget (combo_font, 0, 1);

	/* Font size */
	label = new QLabel (TR("Size"), this);

	spin_size = new MySpinBox (2, 1, 1000, 1, this);
	spin_size->setValue(35);
	grid->addWidget (label, 1, 0);
	grid->addWidget (spin_size, 1, 1);
	
	/* Angle */
	label = new QLabel (TR("Angle"), this);

	spin_angle = new MySpinBox (0, -360, 360, 1, this);
	spin_angle->setValue(0);
	grid->addWidget (label, 2, 0);
	grid->addWidget (spin_angle, 2, 1);

	/* Justification */
	label = new QLabel (TR("Justification"), this);

	combo_just = new QComboBox (this);
	combo_just->setEditable(false);
	for (i=0; i<justnum; i++)
		combo_just->addItem (just[i]);
	combo_just->setCurrentIndex (0);
	grid->addWidget (label, 3, 0);
	grid->addWidget (combo_just, 3, 1);

	/* buttons */
	addbuttons_close ();
}

void gsglAddTexts::my_reject ()
{
	gsglMainWindow *mw = gsgl_inq_mainwin ();
	gui_cursor_default (mw);
	
	mw->Add_text = GSGL_FALSE;
	mw->Add_text_dialog = NULL;

	reject ();
}
