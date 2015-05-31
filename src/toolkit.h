/* -*- c++ -*- */
#include "gsgltype.h"
#include "guiitems.h"

#define GSGL_EVENT_MOUSE_MOVE     100
#define GSGL_EVENT_MOUSE_PRESS    101
#define GSGL_EVENT_MOUSE_RELEASE  102
#define GSGL_EVENT_STATE_NOBUTTON    201
#define GSGL_EVENT_STATE_LEFTBUTTON  202
#define GSGL_EVENT_STATE_RIGHTBUTTON 203
#define GSGL_EVENT_STATE_MIDBUTTON   204

#ifdef __cplusplus
#define GC_NAME_CONFLICT
#include "config.h"
#ifdef HAVE_GCDIR
#  include "gc/gc_cpp.h"
#else
#  include "gc_cpp.h"
#endif

#define BOXCOLOR 0.85
#define HICOLOR  0.95
#define PUSHED   0.3
#define EDGE_LIGHTED 0.98
#define EDGE_SHADDOW 0.3
#define LW 0.05
#define BOXSIZE 0.4

class point : public gc
{
public:
	point (float x1, float y1) { x = x1; y = y1; };
	~point () {};
	float x;
	float y;
};

class rectangle : public gc
{
public:
	rectangle (float x = 0.0, float y = 0.0, float w = 0.0, float h = 0.0);
	~rectangle () {};
	bool pointIsInside (point *p);
	float x;
	float y;
	float w;
	float h;  
};

class gsglEvent : public gc
{
public:
	enum Type {None = 0, MouseButtonPress = 2, MouseButtonRelease = 3, MouseMove = 5, 
		   KeyPress1 = 6, KeyRelease = 7, FocusIn = 8, FocusOut = 9, Enter = 10,
		   Leave = 11};
	enum ButtonState {NoButton, LeftButton, RightButton, MidButton};
	gsglEvent (Type type);
	~gsglEvent () {};
	Type type ();

private:
	Type type0;
};

class gsglMouseEvent : public gsglEvent
{
public:
	gsglMouseEvent (gsglEvent::Type type, const point &, ButtonState);
	~gsglMouseEvent ();
	point *p;
	ButtonState button;
};

class gsglKeyEvent : public gsglEvent
{
public:
	gsglKeyEvent (gsglEvent::Type type);
	~gsglKeyEvent ();
	int key;
	int ascii;
};


class gsglWidget : public gc 
{
public:
	gsglWidget (guialldata *d);
	virtual ~gsglWidget ();
	virtual void setGeometry (float x, float y, float w, float h);
	virtual void draw ();
	virtual void draw_gllist ();
	virtual bool catchEvent (gsglEvent *);
	virtual bool keyPressed (gsglEvent *);
	virtual bool ButtonPressed (gsglEvent *);
	virtual bool ButtonReleased (gsglEvent *);
	virtual bool MouseMove (gsglEvent *);
	virtual bool MouseInside ();
	virtual bool MouseOutside ();
	virtual void show ();

	guialldata *alldata;
	rectangle *rec;
	gsglWidget *parent;
};

class gsglBox : public gsglWidget
{
public:
	gsglBox (guialldata *, float x, float y, float w, float h);
	~gsglBox ();
	void draw ();
	bool pushed;
	bool hilighted;
	bool empty;
	bool frame;
};

class gsglLabel : public gsglWidget
{
public:
	gsglLabel (guialldata *, const char *, int, float offset=0.0);
	~gsglLabel ();
	void draw ();
	int ijust;
	char *label;
private:
	float offset;
};


class gsglLabels : public gsglWidget
{
public:
	gsglLabels (int, gsglWidget *parent);
	~gsglLabels ();
	void insertLabel (const char *);
	void setCurrentItem (int);
	bool MouseMove (gsglEvent *);
	bool ButtonPressed (gsglEvent *);
	bool ButtonReleased (gsglEvent *);
	void draw ();
	gsgllist *labels;
	gsglBox *box;
	int ijust;
	int current;
};

class gsglButton : public gsglWidget
{
public:
	enum Type {Normal = 0, Up = 1, Down = 2};
	gsglButton (guialldata *, const char *, int ijust = JUST_CENTER);
	gsglButton (guialldata *, Type);
	~gsglButton ();
	virtual void draw ();
	virtual bool MouseInside ();
	virtual bool MouseOutside ();
	virtual bool ButtonPressed (gsglEvent *e);
	virtual bool ButtonReleased (gsglEvent *e);
	virtual void setGeometry (float x, float y, float w, float h);
	virtual void draw_addon ();
	void setLabel (const char *);
	gsglLabel *label;
	gsglBox *box;
	bool clickedhere;
	bool mouseinside;

private:
	void initcommon ();
	int buttontype;
};

class gsglCheckButton : public gsglWidget
{
public:
	gsglCheckButton (guialldata *, const char *label);
	~gsglCheckButton ();
	void draw ();
	bool ButtonPressed (gsglEvent *e);
	bool ButtonReleased (gsglEvent *e);
	void setGeometry (float x, float y, float w, float h);
	void setChecked (bool);

private:
	gsglLabel *label;
	gsglBox *box;
	bool clicked;
	bool clickedhere;
};

class gsglComboBox : public gsglButton
{
public:
	gsglComboBox (guialldata *, char *);
	~gsglComboBox ();
	void insertItem (char *);
	void draw_addon ();
	bool ButtonPressed (gsglEvent *e);
	bool ButtonReleased (gsglEvent *e);
	bool MouseMove (gsglEvent *e);
	void setGeometry (float x, float y, float w, float h);
	void setCurrentItem (int);
	void setCurrentItemInit (int);
	int current ();

private:
	gsgllist *items;
	gsglLabels *labels;
	bool clicked;
	bool clickedhere;
};

class gsglLineEdit : public gsglWidget
{
public:
	gsglLineEdit (guialldata *, int max, const char *text);
	~gsglLineEdit ();
	void setGeometry (float x, float y, float w, float h);
	bool ButtonReleased (gsglEvent *e);
	void draw ();
private:
	gsglBox *box;
};

class gsglSpinButtonf : public gsglWidget
{
public:
	gsglSpinButtonf (guialldata *, float init, float lower, float upper, int digits);
	~gsglSpinButtonf ();
	void setGeometry (float x, float y, float w, float h);
	bool ButtonPressed (gsglEvent *e);
	bool ButtonReleased (gsglEvent *e);
	void draw ();

private:
	gsglBox *box;
	gsglButton *up;
	gsglButton *down;
};

class gsglSpinButtoni : public gsglWidget
{
public:
	gsglSpinButtoni (guialldata *, int init, int lower, int upper);
	~gsglSpinButtoni ();
	void setGeometry (float x, float y, float w, float h);
	bool ButtonPressed (gsglEvent *e);
	bool ButtonReleased (gsglEvent *e);
	void draw ();

private:
	gsglBox *box;
	gsglButton *up;
	gsglButton *down;
};


bool gsgl_tk_event_handler (gsglEvent *e, gsglPage *);

#endif

#ifdef __cplusplus
extern "C"{
#endif

void x11_to_gsgl (int x, int y, float *xf, float *yf, gsglWindow *w);
void gsgl_tk_guiitems_init_every_page (gsglPage *);
void gsgl_tk_guiitems_finish_every_page (gsglPage *);
gsglboolean gsgl_inq_tk_have_guiitems (gsglPage *);
void gsgl_tk_add_label (guialldata *);
void gsgl_tk_inq_label_size (const char *text, float *w, float *h);
void gsgl_tk_add_button (guialldata *);
void gsgl_tk_add_checkbutton (guialldata *);
void gsgl_tk_add_combobox (guialldata *);
void gsgl_tk_add_lineedit (guialldata *);
void gsgl_tk_add_spinbuttonf (guialldata *);
void gsgl_tk_add_spinbuttoni (guialldata *);
void gsgl_tk_update_memlist ();
int gsgl_tk_event_handler_c (int, float, float, int, gsglPage *);

#ifdef __cplusplus
}
#endif
