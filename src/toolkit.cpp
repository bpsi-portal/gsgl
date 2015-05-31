#include "toolkit.h"
#if defined USE_QT
#include "qt_dialog.h"
#endif
#include "gsaf.h"
#include "gsgl.h"
#include "utils.h"
#include "guiitems.h"
#include "gui_common.h"
#include "xwin.h"
#include "drv_gl.h"
#include "ogl.h"
#include "tcpip.h"
#include "tcp_guiitems.h"
#include "drv_wrapper.h"
#include "displaylist.h"
#include "text.h"
#ifdef USE_GTK
#include "gtk_guiitems.h"
#endif

#define OFFSET_SPIN 0.3
#define OFFSET_LABEL 0.0
#define OFFSET_EDIT 0.5
#define OFFSET_BUTTON 0.3

#define GSGL_DEBUG(x)

void gsgl_tk_guiitems_init_every_page(gsglPage *p)
{
	p->gui->itemlist2 = NULL;
}

void gsgl_tk_guiitems_finish_every_page(gsglPage *p)
{
	p->gui->itemlist2 = NULL;
}

gsglboolean gsgl_inq_tk_have_guiitems(gsglPage *p)
{
	gsgllist *list0 = gsgl_guiitem_inq_list(p);
	itemnode *node;

	while (list0) {
		node = (itemnode *) list0->data;
		if (node && node->toolkit == TK_GSGL)
			return GSGL_TRUE;
		list0 = list0->next;
	}
	return GSGL_FALSE;
}

void x11_to_gsgl(int x, int y, float *xf, float *yf, gsglWindow *w)
{
	*xf = (float) x / gsgl_inq_window_width(w) * GSGL_PAPER_WIDTH;
	*yf = (1.0 - (float) y / gsgl_inq_window_height(w)) * GSGL_PAPER_HEIGHT;
}

bool gsgl_tk_event_handler(gsglEvent *e, gsglPage *p)
{
	gsgllist *item;
	itemnode *node;
	gsglWidget *widget;

	if (p->gui->itemlist2) 
		item = p->gui->itemlist2;
	else
		item = gsgl_guiitem_inq_list(p);

	while (item != NULL) {
		node = (itemnode *) item->data;
		if (node->toolkit == TK_GSGL) {
			widget = (gsglWidget *) node->widget;
			if (widget->catchEvent(e)) return true;
		}
		item = item->next;
	}
	return false;
}

/* rectangle */

rectangle::rectangle(float x1, float y1, float w1, float h1)
{
	x = x1;
	y = y1;
	w = w1;
	h = h1;
}

bool rectangle::pointIsInside(point *p)
{
	if (p->x >= x && p->x <= x + w &&
	    p->y >= y && p->y <= y + h)
		return true;
	else
		return false;
}

/* gsglEvent */

gsglEvent::gsglEvent(Type type)
{
	type0 = type;
}

gsglEvent::Type gsglEvent::type()
{
	return type0;
}

/* gsglMouseEvent */

gsglMouseEvent::gsglMouseEvent(gsglEvent::Type type, const point &p1, ButtonState button1)
	: gsglEvent(type)
{
	p = new point(p1);
	button = button1;
}

gsglMouseEvent::~gsglMouseEvent()
{
	delete p;
}

/* gsglWidget */

gsglWidget::gsglWidget(guialldata *d)
{
	rec = NULL;
	parent = NULL;
	alldata = d;
}

gsglWidget::~gsglWidget()
{
	if (rec) delete rec;
}

void gsglWidget::setGeometry(float x, float y, float w, float h)
{
	rec = new rectangle(x, y, w, h);
}

void gsglWidget::draw()
{
}

void gsglWidget::draw_gllist()
{
	gsglboolean gl, mem, hd, list1;
	unsigned int old;
	gsglDisplaylist *displaylist;
	gsglPage *p = alldata->common->p;

	gl = gsgl_inq_drv_gl(p);
		
	displaylist = alldata->common->displaylist;
	if (gl && displaylist) {
		old = displaylist->gllist;
		displaylist->gllist = gsgl_drv_glGenLists(1, p);
		gsgl_drv_glNewList(displaylist->gllist, GL_COMPILE, p);
		
		list1 = gsgl_inq_displaylist_is_enabled(p);
		mem = gsgl_inq_drv_mem(p);
		hd = gsgl_inq_drv_hd(p);
		
		if (mem) gsgl_disable_drv_mem(p);
		if (hd) gsgl_disable_drv_hd(p);
		if (list1) gsgl_disable_displaylist(p);
		
		draw();
		
		if (mem) gsgl_enable_drv_mem(p);
		if (hd) gsgl_enable_drv_hd(p);
		if (list1) gsgl_enable_displaylist(p);
		
		gsgl_drv_glEndList(p);
		
		if (old)
			glDeleteLists(old, 1);
		
		gsgl_redraw(p);
	}
}

void gsglWidget::show()
{
	draw();
	gsgl_guiitem_append_item(TK_GSGL, alldata, this, NULL);
}

bool gsglWidget::catchEvent(gsglEvent *e)
{
	if (!e) return false;

	switch (e->type()) {
	case gsglEvent::KeyPress1:
		return keyPressed(e);
	case gsglEvent::MouseButtonPress:
		return ButtonPressed(e);
	case gsglEvent::MouseButtonRelease:
		return ButtonReleased(e);
	case gsglEvent::MouseMove:
		return MouseMove(e);
	default:
		return false;
	}
}

bool gsglWidget::keyPressed(gsglEvent *e)
{
	return false;
}

bool gsglWidget::ButtonPressed(gsglEvent *e)
{
	return false;
}

bool gsglWidget::ButtonReleased(gsglEvent *e)
{
	return false;
}

bool gsglWidget::MouseMove(gsglEvent *e)
{
	if (!rec) return false;
	if (!e) return false;

	gsglMouseEvent *m = (gsglMouseEvent *) e;

	if (rec->pointIsInside(m->p)) {
		return MouseInside();
	} else {
		return MouseOutside();
	}
}

bool gsglWidget::MouseInside()
{
	return false;
}

bool gsglWidget::MouseOutside()
{
	return false;
}

/* gsglBox */

gsglBox::gsglBox(guialldata *data, float x, float y, float w, float h)
	: gsglWidget(data)
{
	rec = new rectangle(x, y, w, h);
	pushed = false;
	hilighted = false;
	empty = false;
	frame = false;
}

gsglBox::~gsglBox()
{
	if (rec)
		delete rec;
}

void gsglBox::draw()
{
	float r, g, b;
	float lw;

	if (!rec) return;

	float xmin = rec->x;
	float xmax = rec->x + rec->w;
	float ymin = rec->y;
	float ymax = rec->y + rec->h;
	
	gsaf_inqrgb(&r, &g, &b);
	gsaf_inqlnw(&lw);

	if (empty) 
		gsaf_setrgb(1.0, 1.0, 1.0);
	else if (pushed)
		gsaf_setrgb(PUSHED, PUSHED, PUSHED);
	else if (hilighted)
		gsaf_setrgb(HICOLOR, HICOLOR, HICOLOR);			
	else
		gsaf_setrgb(BOXCOLOR, BOXCOLOR, BOXCOLOR);
	float x[4] = {xmin, xmax, xmax, xmin}; 
	float y[4] = {ymin, ymin, ymax, ymax}; 
	gsaf_poly(x, y, 4);

	gsaf_setlnw(LW);
	if (frame)
		gsaf_setrgb(EDGE_SHADDOW, EDGE_SHADDOW, EDGE_SHADDOW);
	else if (pushed) 
		gsaf_setrgb(EDGE_SHADDOW, EDGE_SHADDOW, EDGE_SHADDOW);
	else
		gsaf_setrgb(EDGE_LIGHTED, EDGE_LIGHTED, EDGE_LIGHTED);
		
	gsaf_move(xmin + LW * 0.5, ymin + LW * 0.5);
	gsaf_draw(xmin + LW * 0.5, ymax - LW * 0.5);
	gsaf_draw(xmax - LW * 0.5, ymax - LW * 0.5);

	if (frame)
		gsaf_setrgb(EDGE_SHADDOW, EDGE_SHADDOW, EDGE_SHADDOW);
	else if (pushed)
		gsaf_setrgb(EDGE_LIGHTED, EDGE_LIGHTED, EDGE_LIGHTED);
	else
		gsaf_setrgb(EDGE_SHADDOW, EDGE_SHADDOW, EDGE_SHADDOW);		
	
	gsaf_move(xmin + LW * 0.5, ymin + LW * 0.5);
	gsaf_draw(xmax - LW * 0.5, ymin + LW * 0.5);
	gsaf_draw(xmax - LW * 0.5, ymax - LW * 0.5);

	gsaf_setrgb(r, g, b);
	gsaf_setlnw(lw);
}

/* gsglLabel */

void gsgl_tk_add_label(guialldata *data)
{
	labeldata *s = (labeldata *) data->specific;

	gsglLabel *label = new gsglLabel(data, s->text, s->ijust);
	label->setGeometry(data->common->x, data->common->y, data->common->w, data->common->h);
	label->show();
}

void gsgl_tk_inq_label_size(const char *text, float *w, float *h)
{
	float height, depth;
	char *text1 = gsgl_strdup(text);
	gsaf_inqtextbbox(text1, w, &height, &depth);
	gsgl_free(text1);
	*h = height - depth; /* depth <= 0 */
}

gsglLabel::gsglLabel(guialldata *data, const char *text, int ijust1, float offset1)
	: gsglWidget(data)
{
	if (text)
		label = gsgl_strdup(text);
	else
		label = NULL;
	ijust = ijust1;
	offset = offset1;
}
	
gsglLabel::~gsglLabel()
{
	if (label)
		gsgl_free(label);
}

void gsglLabel::draw()
{
	if (!rec) {
		gsgl_warning("geometry is not set", "gsglLabel");
	} else if (!label) {
		gsgl_warning("label is not set", "gsglLabel");		
	} else {
		float r, g, b;

		gsaf_inqrgb(&r, &g, &b);
		gsaf_setlin(-1,-1,7);
		float chh1;
		float chw1;
		float chsp1;
		float angl1;
		float tilt1;
		int fnt1;
		float size, height, depth;
		float y;

		gsaf_inqfnt(&fnt1);
		gsaf_inqchr(&chh1, &chw1, &chsp1, &angl1, &tilt1);

		gsaf_setfnt(alldata->common->fnt);
		gsaf_setchr(alldata->common->chh, alldata->common->chw, alldata->common->chsp, alldata->common->angl, alldata->common->tilt);

		GSGL_DEBUG(printf("label=%s, offset=%f\n", label, offset));
		
		gsaf_inqtextbbox (label, &size, &height, &depth);

		y = (rec->h - height + depth) * 0.5 + rec->y;
		y += fabs(depth);

		if (ijust == JUST_LEFT) {
			gsaf_gtext(rec->x + alldata->common->chw * offset, y, label, 0);
		} else if (ijust == JUST_RIGHT) {
			gsaf_gtext(rec->x + rec->w - alldata->common->chw * offset, y, label, 1);
		} else if (ijust == JUST_CENTER) {
			gsaf_gtext(rec->x + 0.5 * rec->w, y, label, 2);
		}
		gsaf_setrgb(r, g, b);
		gsaf_setfnt(fnt1);
		gsaf_setchr(chh1, chw1, chsp1, angl1, tilt1);
	}
}

/* gsglLabels */

gsglLabels::gsglLabels(int ijust1, gsglWidget *parent1)
	: gsglWidget(parent1->alldata)
{
	ijust = ijust1;
	labels = NULL;
	box = NULL;
	current = -1;

	parent = parent1;
}
	
gsglLabels::~gsglLabels()
{
	gsgllist *item;
	if (labels) {
		item = labels;
		while (item) {
			if (item->data)
				delete(gsglLabel *) item->data;
			item = item->next;
		}
	}
	if (box)
		delete box;
}

void gsglLabels::insertLabel(const char *text)
{
	if (!text) return;

	gsglLabel *label = new gsglLabel(alldata, text, ijust, OFFSET_BUTTON);

	labels = gsgl_list_append(labels, label);
}

void gsglLabels::setCurrentItem(int c)
{
	current = c;
}

void gsglLabels::draw()
{
	gsglLabel *label;
	gsgllist *item;
	float dy;
	float r, g, b;
	int c;
	float x[4], y[4];

	if (!rec) {
		gsgl_warning("geometry is not set", "gsglLabels");
	} else if (!labels) {
		gsgl_warning("labels is not set", "gsglLabels");		
	} else {
		dy = rec->h - alldata->common->chh;
		if (dy > 0.0)
			dy *= 0.5;
		else
			dy = 0.0;

		/* set labels' geometry */
		item = labels;
		c = 1;
		while (item != NULL) {
			label = (gsglLabel *) item->data;
			if (label && !label->rec) {
				label->setGeometry(rec->x + LW,
						   rec->y - 0.5*dy - (rec->h-dy) * c + LW,
						   rec->w - 2*LW, rec->h - dy - 2*LW);
			}
			item = item->next;
			c++;
		}

		/* set background box geometry */
		c = gsgl_list_count(labels);
		if (!box) {
			rectangle all;
			all.w = rec->w;
			all.h = (rec->h - dy) * c + dy;
			all.x = rec->x;
			all.y = rec->y - all.h + LW;

			box = new gsglBox(alldata, all.x, all.y, all.w, all.h);
			box->empty = true;
			box->frame = true;
		}
		box->draw();

		/* draw currently selected label */
		if (current >= 0 && current < c) {
			gsaf_inqrgb(&r, &g, &b);
			item = gsgl_list_nth(labels, current+1);
			label = (gsglLabel *) item->data;

			x[0] = x[3] = label->rec->x;
			x[1] = x[2] = label->rec->x + label->rec->w;
			y[0] = y[1] = label->rec->y;
			y[2] = y[3] = label->rec->y + label->rec->h;
			
			gsaf_setrgb(30.0/255.0, 100.0/255.0, 200.0/255.0);
			gsaf_poly(x, y, 4);
			gsaf_setrgb(r, g, b);
		}

		/* draw all labels */
		item = labels;
		c = 0;
		gsaf_inqrgb(&r, &g, &b);
		while (item != NULL) {
			label = (gsglLabel *) item->data;
			if (c == current) 
				gsaf_setrgb(1.0, 1.0, 1.0);
			else
				gsaf_setrgb(0.0, 0.0, 0.0);

			label->draw();
			item = item->next;
			c++;
		}
		gsaf_setrgb(r, g, b);

		if (parent)
			parent->draw();
	}
}

bool gsglLabels::MouseMove(gsglEvent *e)
{
	if (!rec) return false;
	if (!e) return false;
	if (!box) return false;

	gsglMouseEvent *m = (gsglMouseEvent *) e;
	
	gsglLabel *label;
	int c = 0;
	int current2 = -1;
	gsgllist *item = labels;
	while (item != NULL) {
		label = (gsglLabel *) item->data;
		if (label && label->rec) {
			if (label->rec->pointIsInside (m->p)) {
				current2 = c;
			}
		}
		item = item->next;
		c++;
	}

	if (current2 != current && current2 != -1) {
		current = current2;
		draw_gllist();
		return true;
	}
	return false;
}


bool gsglLabels::ButtonPressed(gsglEvent *e)
{
	gsglMouseEvent *m = (gsglMouseEvent *) e;
	gsglComboBox *p = (gsglComboBox *) parent;
	if (box && box->rec && box->rec->pointIsInside(m->p)) {
		alldata->common->p->gui->itemlist2 = NULL;
		if (parent) {
			gsglLabel *label;
			int c = 0;
			gsgllist *item = labels;
			while (item != NULL) {
				label = (gsglLabel *) item->data;
				if (label && label->rec) {
					if (label->rec->pointIsInside(m->p)) {
						p->setCurrentItem(c);
					}
				}
				item = item->next;
				c++;
			}
			if (!parent->rec->pointIsInside(m->p)) {
				p->box->hilighted = false;
			}
			current = p->current();
			parent->draw_gllist();
		}
		return true;
	} else {
		alldata->common->p->gui->itemlist2 = NULL;
		if (parent) {
			if (!parent->rec->pointIsInside(m->p)) {
				p->box->hilighted = false;
			}
			parent->draw_gllist();
			current = p->current();
		}
		return false;
	}
}

bool gsglLabels::ButtonReleased(gsglEvent *e)
{
	gsglMouseEvent *m = (gsglMouseEvent *) e;
	gsglComboBox *p = (gsglComboBox *) parent;
	if (box && box->rec && box->rec->pointIsInside(m->p)) {
		alldata->common->p->gui->itemlist2 = NULL;
		if (parent) {
			gsglLabel *label;
			int c = 0;
			gsgllist *item = labels;
			gsglComboBox *p = (gsglComboBox *) parent;
			while (item != NULL) {
				label = (gsglLabel *) item->data;
				if (label && label->rec) {
					if (label->rec->pointIsInside(m->p)) {
						p->setCurrentItem(c);
					}
				}
				item = item->next;
				c++;
			}
			if (parent && !parent->rec->pointIsInside(m->p)) {
				p->box->hilighted = false;
			}
			current = p->current();
			parent->draw_gllist();
		}
		return true;
	} else {
		if (parent && !parent->rec->pointIsInside(m->p)) {
			p->box->hilighted = false;
			alldata->common->p->gui->itemlist2 = NULL;
			parent->draw_gllist();
			current = p->current();
		}
		return true;
	}
}

/* gsglButton */

void gsgl_tk_add_button(guialldata *data)
{
	buttondata *s = (buttondata *) data->specific;

	gsglButton *button = new gsglButton(data, s->text);
	button->setGeometry(data->common->x, data->common->y, data->common->w, data->common->h);
	button->show();
}

void gsglButton::initcommon()
{
	clickedhere = false;
	box = NULL;
}

gsglButton::gsglButton(guialldata *data, const char *text, int ijust)
	: gsglWidget(data)
{
	label = new gsglLabel(data, text, ijust, OFFSET_BUTTON);
	buttontype = gsglButton::Normal;

	initcommon();
}

gsglButton::gsglButton(guialldata *data, gsglButton::Type updown)
	: gsglWidget(data)
{
	label = NULL;
	buttontype = updown;

	initcommon();
}

gsglButton::~gsglButton()
{
	if (label)
		delete label;
	if (box)
		delete box;
}

void gsglButton::setGeometry(float x, float y, float w, float h)
{
	rec = new rectangle(x, y, w, h);
	if (label)
		label->setGeometry(x+LW, y+LW, w-2*LW, h-2*LW);
	if (!box)
		box = new gsglBox(alldata, x, y, w, h);
}

void gsglButton::setLabel(const char *text)
{
	int ijust = JUST_CENTER;
	if (label) {
		ijust = label->ijust;
		delete label;
	}
	label = new gsglLabel(alldata, text, ijust, OFFSET_BUTTON);
}

void gsglButton::draw()
{
	float x[3], y[3];

	if (!rec) {
		gsgl_warning("geometry is not set", "gsglButton");
	} else if (!box) {
		gsgl_warning("box is not set", "gsglButton");
	} else {
		if (box)
			box->draw();
		if (label)
			label->draw();
		switch (buttontype) {
		case gsglButton::Up:
		case gsglButton::Down:
			if (rec) {
				int sign;
				if (buttontype == gsglButton::Up)
					sign = 1;
				else 
					sign = -1;

				float r, g, b;
				gsaf_inqrgb(&r, &g, &b);
				gsaf_setrgb(0.0, 0.0, 0.0);

				float xcenter = rec->x + rec->w * 0.5;
				x[0] = xcenter - 0.1;
				x[1] = xcenter;
				x[2] = xcenter + 0.1;

				float ycenter = rec->y + rec->h * 0.5;
				y[0] = ycenter - sign * 0.1;
				y[1] = ycenter + sign * 0.1;
				y[2] = ycenter - sign * 0.1;

				gsaf_poly(x, y, 3);
				gsaf_setrgb(r, g, b);
			}
			break;
		default:
			break;
		}
		draw_addon();
	}
}

void gsglButton::draw_addon()
{
}

bool gsglButton::MouseInside()
{
	if (!mouseinside) {
		if (box)
			box->hilighted = true;
		if (clickedhere && box)
			box->pushed = true;
		draw_gllist();
		mouseinside = true;
	}
	return false;
}

bool gsglButton::MouseOutside()
{
	if (mouseinside) {
		if (box)
			box->hilighted = false;
		if (box && box->pushed)
			box->pushed = false;
		draw_gllist();
		mouseinside = false;
	}
	return false;
}

bool gsglButton::ButtonPressed(gsglEvent *e)
{
	gsglMouseEvent *m = (gsglMouseEvent *) e;
	if (rec->pointIsInside(m->p)) {
		if (box)
			box->pushed = true;
		clickedhere = true;
		draw_gllist();
		return true;
	} else {
		clickedhere = false;
		return false;
	}
}

bool gsglButton::ButtonReleased(gsglEvent *e)
{
	gsglMouseEvent *m = (gsglMouseEvent *) e;

	if (rec->pointIsInside(m->p)) {
		if (clickedhere && box && box->pushed) {
			gsgl_guiitems_button_clicked (alldata, this);
		}
		if (box)
			box->pushed = false;
		clickedhere = false;
		draw_gllist ();
		return true;
	} else {
		clickedhere = false;
		return false;
	}
}

/* gsglCheckButton */

void gsgl_tk_add_checkbutton (guialldata *data)
{
	checkbuttondata *s = (checkbuttondata *) data->specific;

	gsglCheckButton *button = new gsglCheckButton (data, s->text);
	button->setGeometry(data->common->x, data->common->y, data->common->w, data->common->h);
	button->setChecked((bool) s->init);
	button->show ();
}

gsglCheckButton::gsglCheckButton (guialldata *data, const char *label1)
	: gsglWidget (data)
{
	label = new gsglLabel (data, label1, JUST_LEFT, OFFSET_BUTTON);

	clicked = false;
	clickedhere = false;
	box = NULL;
}

gsglCheckButton::~gsglCheckButton ()
{
	if (label)
		delete label;
}

void gsglCheckButton::setGeometry(float x, float y, float w, float h)
{
	rec = new rectangle(x, y, w, h);
	if (label)
		label->setGeometry(x+BOXSIZE, y, w-BOXSIZE, h);
}

void gsglCheckButton::setChecked (bool b)
{
	checkbuttondata *c = (checkbuttondata *) alldata->specific;
	if (b)
		c->init = 1;
	else
		c->init = 0;
}

void gsglCheckButton::draw ()
{
	checkbuttondata *c = (checkbuttondata *) alldata->specific;

	if (!rec) {
		gsgl_warning ("geometry is not set", "gsglCheckButton");
	} else {
		float r, g, b;
		float lw;
		float xmin, xmax, ymin, ymax;

		xmin = rec->x;
		xmax = xmin + BOXSIZE;
		ymin = rec->y + rec->h * 0.5 - BOXSIZE * 0.5;
		ymax = ymin + BOXSIZE;

		if (!box) {
			box = new gsglBox (alldata, xmin, ymin, xmax-xmin, ymax-ymin);
			box->empty = true;
			box->pushed = true;
		}
		box->draw();

		if (c->init) {
			gsaf_inqrgb (&r, &g, &b);
			gsaf_inqlnw (&lw);

			gsaf_setrgb (0.0, 0.0, 0.0);
			gsaf_setlnw (LW);
			gsaf_move (xmin + 1.5 * LW, ymin + 1.5 * LW);
			gsaf_draw (xmax - 1.5 * LW, ymax - 1.5 * LW);
			gsaf_move (xmin + 1.5 * LW, ymax - 1.5 * LW);
			gsaf_draw (xmax - 1.5 * LW, ymin + 1.5 * LW);

			gsaf_setlnw (lw);
			gsaf_setrgb (r, g, b);
		}

		if (label)
			label->draw ();
	}
}

bool gsglCheckButton::ButtonPressed (gsglEvent *e)
{
	gsglMouseEvent *m = (gsglMouseEvent *) e;
	if (rec->pointIsInside (m->p)) {
		clicked = true;
		clickedhere = true;
		draw_gllist ();
		return true;
	} else {
		clickedhere = false;
		return false;
	}
}

bool gsglCheckButton::ButtonReleased (gsglEvent *e)
{
	gsglMouseEvent *m = (gsglMouseEvent *) e;
	checkbuttondata *s = (checkbuttondata *) alldata->specific;

	if (rec->pointIsInside (m->p)) {
		if (clickedhere && clicked) {
			if (s->init)
				s->init = 0;
			else
				s->init = 1;
			gsgl_guiitems_checkbutton_toggled (alldata, this);
		}
		clicked = false;
		clickedhere = false;
		draw_gllist ();
		return true;
	} else {
		clickedhere = false;
		return false;
	}
}

 /* gsglComboBox */

void gsgl_tk_add_combobox(guialldata *data)
{
	int i;
	comboboxdata *s = (comboboxdata *) data->specific;
	gsglComboBox *button = new gsglComboBox(data, s->text[s->current]);
	
	for (i=0; i<s->items; i++) 
                button->insertItem(s->text[i]);

	button->setGeometry(data->common->x, data->common->y, data->common->w, data->common->h);
	button->setCurrentItemInit(s->current);
	button->show();
}

gsglComboBox::gsglComboBox(guialldata *data, char *text)
	: gsglButton(data, text, JUST_LEFT)
{
	clicked = false;
	clickedhere = false;
	items = NULL;
	labels = new gsglLabels(JUST_LEFT, this);
	box = NULL;
}

gsglComboBox::~gsglComboBox()
{
	gsgllist *item;
	if (items) {
		item = items;
		while (item) {
			if (item->data)
				gsgl_free (item->data);
			item = item->next;
		}
	}
	if (labels) 
		delete labels;
	if (box)
		delete box;
}

void gsglComboBox::setGeometry(float x, float y, float w, float h)
{
	rec = new rectangle(x, y, w, h);
	if (label)
		label->setGeometry(x+LW, y+LW, w-2*LW, h-2*LW);
	if (labels)
		labels->setGeometry(x, y, w, h);
	box = new gsglBox(alldata, x, y, w, h);
}

void gsglComboBox::setCurrentItemInit(int i)
{
	comboboxdata *s = (comboboxdata *) alldata->specific;
	s->current = i;

	gsgllist *item = gsgl_list_nth(items, i+1);
	const char *text = (const char *) item->data;
	setLabel(text);
	if (label && rec)
		label->setGeometry(rec->x+LW, rec->y+LW, rec->w-2*LW, rec->h-2*LW);
	if (labels)
		labels->setCurrentItem(i);
}

void gsglComboBox::setCurrentItem(int i)
{
	setCurrentItemInit(i);
	gsgl_guiitems_combobox_selected(alldata, this);
}

void gsglComboBox::insertItem(char *text)
{
	items = gsgl_list_append(items, text);
	if (labels)
		labels->insertLabel(text);
}

void gsglComboBox::draw_addon()
{
	float ymin = rec->y;
	float ymax = rec->y + rec->h;
	float xmax = rec->x + rec->w;
	float xmin = MAX(rec->x, xmax - 0.4);
	float r, g, b, lw;
	float x[3], y[3];

	gsaf_inqrgb(&r, &g, &b);
	gsaf_inqlnw(&lw);

	gsaf_setlnw(LW * 0.5);

	gsaf_setrgb(EDGE_SHADDOW, EDGE_SHADDOW, EDGE_SHADDOW);
	gsaf_move(xmin, ymin + (ymax - ymin) * 0.2);
	gsaf_draw(xmin, ymax - (ymax - ymin) * 0.2);

	x[0] = xmin + 0.1;
	x[1] = (xmin + xmax) * 0.5;
	x[2] = xmax - 0.1;

	y[0] = y[2] = ymax - (ymax - ymin) * 0.42;
	y[1] = ymin + (ymax - ymin) * 0.42;
	gsaf_poly(x, y, 3);

	gsaf_setrgb(EDGE_LIGHTED, EDGE_LIGHTED, EDGE_LIGHTED);
	gsaf_move(xmin + 0.5 * LW, ymin + (ymax - ymin) * 0.25);
	gsaf_draw(xmin + 0.5 * LW, ymax - (ymax - ymin) * 0.25);

	gsaf_setrgb(r, g, b);
	gsaf_setlnw(lw);
}

int gsglComboBox::current()
{
	comboboxdata *s = (comboboxdata *) alldata->specific;
	return s->current;
}

bool gsglComboBox::ButtonPressed(gsglEvent *e)
{
	gsglMouseEvent *m = (gsglMouseEvent *) e;
	itemnode *node;

	if (rec->pointIsInside(m->p)) {
		clicked = true;
		clickedhere = true;
		labels->draw_gllist();

		node = (itemnode *) gsgl_malloc(sizeof(itemnode));
		node->toolkit = TK_GSGL;
		node->id = 0;
		node->gdata = alldata;
		node->widget = labels;

		alldata->common->p->gui->itemlist2 = gsgl_list_append(alldata->common->p->gui->itemlist2, node);

		return true;
	} else {
		clickedhere = false;
		return false;
	}
}

bool gsglComboBox::ButtonReleased(gsglEvent *e)
{
	gsglMouseEvent *m = (gsglMouseEvent *) e;
	if (rec->pointIsInside(m->p)) {
		clickedhere = false;
		clicked = false;
		return true;
	} else {
		if (labels && labels->box && labels->box->rec && 
		    labels->box->rec->pointIsInside(m->p)) {
			box->pushed = false;
			clickedhere = false;
			draw_gllist();
			return true;
		} else {
			alldata->common->p->gui->itemlist2 = NULL;
			clickedhere = false;
			return false;
		}
	}
}

bool gsglComboBox::MouseMove(gsglEvent *e)
{
	if (!rec) return false;
	if (!e) return false;

	gsglMouseEvent *m = (gsglMouseEvent *) e;

	if (rec->pointIsInside(m->p)) {
		return MouseInside();
	} else {
		return MouseOutside();
	}
}

/* gsglLineEdit */

void gsgl_tk_add_lineedit(guialldata *data)
{
	lineeditdata *s = (lineeditdata *) data->specific;

	gsglLineEdit *edit = new gsglLineEdit(data, s->max, s->text);
	edit->setGeometry(data->common->x, data->common->y, data->common->w, data->common->h);
	edit->show ();
}

gsglLineEdit::gsglLineEdit (guialldata *data, int max1, const char *text1)
	: gsglWidget(data)
{
	box = NULL;
}

gsglLineEdit::~gsglLineEdit()
{
	if (box)
		delete box;
}

void gsglLineEdit::setGeometry(float x, float y, float w, float h)
{
	rec = new rectangle(x, y, w, h);
	
	if (!box) {
		box = new gsglBox(alldata, x, y, w, h);
		box->empty = true;
		box->pushed = true;
	}
}

void gsglLineEdit::draw()
{
	float y = (rec->h - alldata->common->chh) * 0.5 + rec->y;
	float x = rec->x + alldata->common->chw * OFFSET_EDIT;
	/*int i, c=0;*/
	int s;

	lineeditdata *c = (lineeditdata *) alldata->specific;

	box->draw();
	if (c->text) {
		/*for (i=0; i<strlen(text); i++) {
		  if (text[i] == '$' || text[i] == '\\')
		  c++;
		  }
		  if (c > 0) {
		  c = 0;
		  char *newc = (char *) gsgl_malloc (sizeof(char) * (strlen(text)+c+1));
		  for (i=0; i<strlen(text); i++) {
		  newc[i+c] = text[i];
		  if (text[i] == '$' || text[i] == '\\') {
		  c++;
		  newc[i+c] = text[i];
		  }
		  }
		  newc[strlen(text)+c] = '\0';
		  gsgl_free (text);
		  text = newc;
		  }
		gsaf_gtext (rec->x + chw, y, text, 0);*/
		s = strlen (c->text);

		float chh1;
		float chw1;
		float chsp1;
		float angl1;
		float tilt1;
		int fnt1;

		gsaf_inqfnt(&fnt1);
		gsaf_inqchr(&chh1, &chw1, &chsp1, &angl1, &tilt1);

		gsaf_setfnt(alldata->common->fnt);
		gsaf_setchr(alldata->common->chh, alldata->common->chw, alldata->common->chsp, alldata->common->angl, alldata->common->tilt);

		gsgltext_analyse(c->text, MIN(s, c->max), &x, &y, 1, GSGL_FALSE, alldata->common->p);

		gsaf_setfnt(fnt1);
		gsaf_setchr(chh1, chw1, chsp1, angl1, tilt1);
	}
}

bool gsglLineEdit::ButtonReleased(gsglEvent *e)
{
	gsglMouseEvent *m = (gsglMouseEvent *) e;
	lineeditdata *s = (lineeditdata *) alldata->specific;
	char *p;

	if (rec->pointIsInside(m->p)) {
		p = gui_popup_edit (s->text, _("LineEdit"));
		if (p) {
			s->text = gsgl_strdup(p);
			free(p);
			draw_gllist();
			gsgl_guiitems_lineedit_changed(alldata, this);
		}
		return true;
	} else {
		return false;
	}
}

/* gsglSpinButtonf */

void gsgl_tk_add_spinbuttonf(guialldata *data)
{
	spinbuttondataf *s = (spinbuttondataf *) data->specific;

	gsglSpinButtonf *spin = new gsglSpinButtonf (data, s->init, s->lower, s->upper, s->digits);
	spin->setGeometry(data->common->x, data->common->y, data->common->w, data->common->h);
	spin->show();
}

gsglSpinButtonf::gsglSpinButtonf(guialldata *data, float init, float lower1, float upper1, int digits1)
	: gsglWidget(data)
{
	box = NULL;
	up = NULL;
	down = NULL;
}

gsglSpinButtonf::~gsglSpinButtonf()
{
	if (box)
		delete box;
	if (up)
		delete up;
	if (down)
		delete down;
}

void gsglSpinButtonf::setGeometry(float x, float y, float w, float h)
{
	rec = new rectangle(x, y, w, h);
	
	if (!box) {
		box = new gsglBox(alldata, x, y, w - 0.5, h);
		box->empty = true;
		box->pushed = true;
	}
	if (!up) {
		up = new gsglButton(alldata, gsglButton::Up);
		up->setGeometry (x+w-0.5, y+h*0.5, 0.5, h*0.5);
	}
	if (!down) {
		down = new gsglButton(alldata, gsglButton::Down);
		down->setGeometry(x+w-0.5, y, 0.5, h*0.5);
	}
}

void gsglSpinButtonf::draw()
{
	float y = (rec->h - alldata->common->chh) * 0.5 + rec->y;
	float x = rec->x + alldata->common->chw * OFFSET_SPIN;
	/*int i, c=0;*/

	if (box)
		box->draw ();
	if (up)
		up->draw ();
	if (down)
		down->draw ();

	float chh1;
	float chw1;
	float chsp1;
	float angl1;
	float tilt1;
	int fnt1;
	
	gsaf_inqfnt(&fnt1);
	gsaf_inqchr(&chh1, &chw1, &chsp1, &angl1, &tilt1);

	gsaf_setfnt(alldata->common->fnt);
	gsaf_setchr(alldata->common->chh, alldata->common->chw, alldata->common->chsp, alldata->common->angl, alldata->common->tilt);

	spinbuttondataf *s = (spinbuttondataf *) alldata->specific;

	if (s->digits == 0) {		
		gsaf_gnumbi(x + LW, y, (int)(s->init + 0.5), 0);
	} else {
		gsaf_gnumbr(x + LW, y, s->init, s->digits, 0);
	}
	gsaf_setfnt(fnt1);
	gsaf_setchr(chh1, chw1, chsp1, angl1, tilt1);	
}

bool gsglSpinButtonf::ButtonReleased(gsglEvent *e)
{
	gsglMouseEvent *m = (gsglMouseEvent *) e;
	spinbuttondataf *s = (spinbuttondataf *) alldata->specific;
	float ret;

	/*if (up)
	  up->ButtonReleased (e);
	  if (down)
	  down->ButtonReleased (e);*/

	if (rec->pointIsInside (m->p)) {
		ret = gui_popup_spinf (s->digits, s->init, s->lower, s->upper, alldata->common->p);
		if (ret >= s->lower && ret <= s->upper) {
			s->init = ret;
			draw_gllist ();
			gsgl_guiitems_spinbuttonf_changed (alldata, this);
		}
		return true;
	} else {
		return false;
	}
}

bool gsglSpinButtonf::ButtonPressed(gsglEvent *e)
{
	gsglMouseEvent *m = (gsglMouseEvent *) e;

	/*if (up)
	  up->ButtonPressed (e);
	  if (down)
	  down->ButtonPressed (e);*/

	if (box && box->rec->pointIsInside(m->p)) {
		return true;
	} else {
		return false;
	}
}

/* gsglSpinButtoni */

void gsgl_tk_add_spinbuttoni(guialldata *data)
{
	spinbuttondatai *s = (spinbuttondatai *) data->specific;

	gsglSpinButtoni *spin = new gsglSpinButtoni(data, s->init, s->lower, s->upper);
	spin->setGeometry(data->common->x, data->common->y, data->common->w, data->common->h);
	spin->show();
}

gsglSpinButtoni::gsglSpinButtoni(guialldata *data, int init, int lower1, int upper1)
	: gsglWidget(data)
{
	box = NULL;
	up = NULL;
	down = NULL;
}

gsglSpinButtoni::~gsglSpinButtoni()
{
	if (box)
		delete box;
	if (up)
		delete up;
	if (down)
		delete down;
}

void gsglSpinButtoni::setGeometry(float x, float y, float w, float h)
{
	rec = new rectangle(x, y, w, h);
	
	if (!box) {
		box = new gsglBox(alldata, x, y, w - 0.5, h);
		box->empty = true;
		box->pushed = true;
	}
	if (!up) {
		up = new gsglButton(alldata, gsglButton::Up);
		up->setGeometry(x+w-0.5, y+h*0.5, 0.5, h*0.5);
	}
	if (!down) {
		down = new gsglButton(alldata, gsglButton::Down);
		down->setGeometry(x+w-0.5, y, 0.5, h*0.5);
	}
}

void gsglSpinButtoni::draw()
{
	float y = (rec->h - alldata->common->chh) * 0.5 + rec->y;
	float x = rec->x + alldata->common->chw * OFFSET_SPIN;
	/*int i, c=0;*/

	if (box)
		box->draw();
	if (up)
		up->draw();
	if (down)
		down->draw();

	float chh1;
	float chw1;
	float chsp1;
	float angl1;
	float tilt1;
	int fnt1;
	
	gsaf_inqfnt(&fnt1);
	gsaf_inqchr(&chh1, &chw1, &chsp1, &angl1, &tilt1);

	gsaf_setfnt(alldata->common->fnt);
	gsaf_setchr(alldata->common->chh, alldata->common->chw, alldata->common->chsp, alldata->common->angl, alldata->common->tilt);

	spinbuttondatai *s = (spinbuttondatai *) alldata->specific;

	gsaf_gnumbi(x + LW, y , s->init, 0);

	gsaf_setfnt(fnt1);
	gsaf_setchr(chh1, chw1, chsp1, angl1, tilt1);	
}

bool gsglSpinButtoni::ButtonReleased(gsglEvent *e)
{
	gsglMouseEvent *m = (gsglMouseEvent *) e;
	spinbuttondatai *s = (spinbuttondatai *) alldata->specific;
	int ret;

	/*if (up)
	  up->ButtonReleased (e);
	  if (down)
	  down->ButtonReleased (e);*/

	if (rec->pointIsInside(m->p)) {
		ret = gui_popup_spini(s->init, s->lower, s->upper, alldata->common->p);
		if (ret >= s->lower && ret <= s->upper) {
			s->init = ret;
			draw_gllist();
			gsgl_guiitems_spinbuttoni_changed(alldata, this);
		}
		return true;
	} else {
		return false;
	}
}

bool gsglSpinButtoni::ButtonPressed(gsglEvent *e)
{
	gsglMouseEvent *m = (gsglMouseEvent *) e;

	/*if (up)
	  up->ButtonPressed (e);
	  if (down)
	  down->ButtonPressed (e);*/

	if (box && box->rec->pointIsInside(m->p)) {
		return true;
	} else {
		return false;
	}
}


int gsgl_tk_event_handler_c(int eventtype, float x, float y, int state, gsglPage *page)
{
	int ret;
	point p(x, y);
	gsglEvent::Type type;
	gsglEvent::ButtonState bstate;

	switch (eventtype) {
	case GSGL_EVENT_MOUSE_MOVE:
		type = gsglEvent::MouseMove;
		break;
	case GSGL_EVENT_MOUSE_PRESS:
		type = gsglEvent::MouseButtonPress;
		break;
	case GSGL_EVENT_MOUSE_RELEASE:
		type = gsglEvent::MouseButtonRelease;
		break;
	default:
		return 0;
	}
	switch (state) {
	case GSGL_EVENT_STATE_NOBUTTON:
		bstate = gsglEvent::NoButton;
		break;
	case GSGL_EVENT_STATE_LEFTBUTTON:
		bstate = gsglEvent::LeftButton;
		break;
	case GSGL_EVENT_STATE_RIGHTBUTTON:
		bstate = gsglEvent::RightButton;
		break;
	case GSGL_EVENT_STATE_MIDBUTTON:
		bstate = gsglEvent::MidButton;
		break;
	default:
		bstate = gsglEvent::NoButton;
		break;
	}
	
	gsglMouseEvent *m = new gsglMouseEvent(type, p, bstate);
	ret = (int) gsgl_tk_event_handler(m, page);
	delete m;

	return ret;
}
