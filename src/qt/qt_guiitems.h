/* -*- c++ -*- */
#ifndef __qt_guiitems_h
#define __qt_guiitems_h

#ifdef __cplusplus

#include <QPushButton>
#include <QComboBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QCheckBox>
#include <QLabel>
#include "gsaf.h"
#include "guiitems.h"
#ifdef Bool
#undef Bool
#endif

class UserLabel : public QLabel
{
	Q_OBJECT
public:
	UserLabel (guialldata *, const QString &text, QWidget *parent);
        ~UserLabel () {};
	guialldata *alldata;
};
	

class UserButton : public QPushButton
{
	Q_OBJECT
public:
	UserButton (guialldata *, const QString &text, QWidget *parent);
        ~UserButton () {};
        void connect_callback ();
	guialldata *alldata;

public slots:
        void userclicked ();
};


class UserComboBox : public QComboBox
{
	Q_OBJECT
public:
	UserComboBox (guialldata *, QWidget *parent = 0);
	~UserComboBox () {};
        void connect_callback ();
	guialldata *alldata;

public slots:
        void useractivated (int);
};

class UserSpinButtonf : public QSpinBox
{
	Q_OBJECT
public:
	UserSpinButtonf (guialldata *, QWidget * parent = 0);
	~UserSpinButtonf () {};
        void connect_callback ();
	QString textFromValue (int value) const override;
	int valueFromText (const QString &text) const override;
	guialldata *alldata;

public slots:
        void uservalueChanged (int);
};

class UserSpinButtoni : public QSpinBox
{
	Q_OBJECT
public:
	UserSpinButtoni (guialldata *, QWidget * parent = 0);
	~UserSpinButtoni () {};
        void connect_callback ();
	guialldata *alldata;

public slots:
        void uservalueChanged (int);
};

class UserCheckButton : public QCheckBox
{
	Q_OBJECT
public:
	UserCheckButton (guialldata *, const QString & text, QWidget * parent);
	~UserCheckButton () {};
        void connect_callback ();
	guialldata *alldata;

public slots:
        void usertoggled (bool);
};

class UserLineEdit : public QLineEdit
{
	Q_OBJECT
public:
	UserLineEdit (guialldata *, const QString & contents, QWidget * parent);
	~UserLineEdit () {};
        void connect_callback ();
	guialldata *alldata;

public slots:
        void usertextChanged (const QString &);
};

#endif

#ifdef __cplusplus
extern "C"{
#endif

void gui_add_label_user (int x, int y, int w, int h, guialldata *);
void gui_inq_label_size (const char *text, float *w, float *h, gsglPage *);
void gui_add_button_user (int x, int y, int w, int h, guialldata *);
void gui_add_combobox_user (int x, int y, int w, int h, guialldata *);
void gui_add_spinbuttonf_user (int x, int y, int w, int h, guialldata *);
void gui_add_spinbuttoni_user (int x, int y, int w, int h, guialldata *);
void gui_add_checkbutton_user (int x, int y, int w, int h, guialldata *);
void gui_add_lineedit_user (int x, int y, int w, int h, guialldata *);
void gui_guiitems_init_every_page ();
void gui_guiitems_finish_every_page (gsglPage *);
void gui_guiitems_configure (gsglPage *);
void gui_guiitems_callback (unsigned int id, void *data);

void gsgl_qt_update_memlist ();

#ifdef __cplusplus
}
#endif

#endif
