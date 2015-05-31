#include "gsaf.h"
#include "toolkit.h"
#include "matrix.h"
#include "xwin.h"
#include "tcp_guiitems.h"
#include "bsc1.h"
#include "utils.h"
#include "drv_wrapper.h"
#include "displaylist.h"
#include "mem_list.h"
#include "drv_file.h"
#include "gui_common.h"
#include "tcpip.h"
#ifdef USE_GTK
#    include <gtk/gtk.h>
#    include <gdk/gdk.h>
#    include <gdk/gdkx.h>
#    include "gtk_base.h"
#    include "gtk_guiitems.h"
#endif
#ifdef USE_QT
#    include "qt_guiitems.h"
#    include "qt_xwin.h"
#endif

gsglboolean gsgl_inq_guiitems_external (gsglPage *p)
{
	return p->gui->External_tool_kit;
}

void gsaf_guiexitloop ()
{
	gsglPage *p = gsgl_inq_page();
	if (gsgl_inq_mode (p->library) == GSGL_MODE_TCPIP) {
		gsgl_tcp_exit_from_pagee (p);
	} else if (gsgl_inq_mode (p->library) == GSGL_MODE_WINDOW) {
		gui_exit_loop ();
	}
}

void gsgl_set_guiitems_external (gsglboolean b, gsglPage *p)
{
	p->gui->External_tool_kit = b;
}

void gsgl_init_guiitems_enery_page (gsglPage *p)
{
	p->gui->External_tool_kit = GSGL_FALSE;
	p->gui->itemlist = NULL;

	gsgl_tk_guiitems_init_every_page (p);
	gsgl_tcp_guiitems_init_every_page ();
	gui_guiitems_init_every_page ();
}

void gsgl_finish_guiitems_every_page (gsglPage *p)
{
	gsgl_tk_guiitems_finish_every_page (p);
	gsgl_tcp_guiitems_finish_every_page ();
	gui_guiitems_finish_every_page (p);

	p->gui->External_tool_kit = GSGL_FALSE;
	p->gui->itemlist = NULL;
}

void gsgl_guiitems_configure (gsglPage *p)
{
	gui_guiitems_configure (p);
}

void gsgl_guiitems_update_memlist (gsglPage *p)
{
	gsgllist *i;
	guialldata *g;

	if (!gsgl_inq_drv_mem (p)) return;

	i = p->gui->itemlist;

	while (i) {
	    if (i && i->data) {
		    itemnode *node = (itemnode *) i->data;
		    g = node->gdata;

		    if (g->common->type == ITEM_COMBO ||
			g->common->type == ITEM_SPINF ||
			g->common->type == ITEM_SPINI ||
			g->common->type == ITEM_CHECK ||
			g->common->type == ITEM_EDIT) {
			
			    gsgl_mem_list_start (g->common->p);

			    switch (g->common->type) {
			    case ITEM_COMBO:
				    gsgl_file_gui_combobox(g, gsgl_list_push);
				    break;
			    case ITEM_SPINF:
				    gsgl_file_gui_spinbuttonf (g, gsgl_list_push);
				    break;
			    case ITEM_SPINI:
				    gsgl_file_gui_spinbuttoni (g, gsgl_list_push);
				    break;
			    case ITEM_CHECK:
				    gsgl_file_gui_checkbutton (g, gsgl_list_push);
				    break;
			    case ITEM_EDIT:
				    gsgl_file_gui_lineedit (g, gsgl_list_push);
				    break;
			    }
			    gsgl_mem_list_inq_size_and_data (g->common->displaylist, g->common->p);
		    }
	    }
	    i = i->next;
	}
}

void gsgl_guiitem_append_item (int toolkit, guialldata *data, void *widget, void *other)
{
	itemnode *node = (itemnode *) gsgl_malloc (sizeof(itemnode));
	node->toolkit = toolkit;
	node->id = gsgl_list_count (data->common->p->gui->itemlist) + 1;
	node->gdata = data;
	node->widget = widget;
	node->other = other;

	data->common->p->gui->itemlist = gsgl_list_append (data->common->p->gui->itemlist, node);
}

unsigned int find_id_from_widget (void *w, gsglPage *p)
{
	gsgllist *i = p->gui->itemlist;
	while (i) {
		if (i && i->data) {
			itemnode *node = (itemnode *) i->data;
			if (node->widget == w) {
				return node->id;
			}
		}
		i = i->next;
	}
	gsgl_warning ("find_id_from_widget", "can not find id from widget");
	return 0;
}

gsgllist * find_list_from_id (unsigned int id, gsglPage *p)
{
	gsgllist *i = p->gui->itemlist;
	while (i) {
		if (i && i->data) {
			itemnode *node = (itemnode *) i->data;
			if (node->id == id) {
				return i;
			}
		}
		i = i->next;
	}
	gsgl_warning ("find_list_from_id", "can not find list from id");
	return NULL;
}

gsgllist * gsgl_guiitem_inq_list (gsglPage *p)
{
	return p->gui->itemlist;
}

void gsgl_to_x11 (float x, float y, float w, float h, 
		  int *x11, int *y11, int *width, int *height, gsglPage *p)
{
	float xw, yw;
	float x_corner, y_corner;

	gsgl_project_world (x,  y+h, &xw, &yw, p->win);
	gsgl_project_world (x+w,  y, &x_corner, &y_corner, p->win);
	
	*width = abs(x_corner-xw);
	*height = abs(y_corner-yw);

	*x11 = xw;
	*y11 = gsgl_inq_window_height (p->win) - yw;
}

void gsaf_inqlabelsize (const char *text, float *w, float *h)
{
	gsgl_inq_label_size (text, w, h, gsgl_inq_page());
}

void gsgl_inq_label_size (const char *text, float *w, float *h, gsglPage *p)
{
	if (p->gui->External_tool_kit && gsgl_inq_mode (p->library) == GSGL_MODE_WINDOW) {
		gui_inq_label_size (text, w, h, p);
	} else {
		gsgl_tk_inq_label_size (text, w, h);
	}
}

void gsgl_add_label_external (guialldata *data1)
{
	int x_x11, y_x11;
	int width, height;
	itemdata *data = data1->common;
	gsglPage *p = data->p;

	if (gsgl_inq_mode (p->library) == GSGL_MODE_WINDOW) {
		gsgl_to_x11 (data->x, data->y, data->w, data->h, &x_x11, &y_x11, &width, &height, p);
		gui_add_label_user (x_x11, y_x11, width, height, data1);
	}
}

void gsgl_add_button_external (guialldata *data1)
{
	int x_x11, y_x11;
	int width, height;
	itemdata *data = data1->common;
	gsglPage *p = data->p;

	if (gsgl_inq_mode (p->library) == GSGL_MODE_WINDOW) {
		gsgl_to_x11 (data->x, data->y, data->w, data->h, &x_x11, &y_x11, &width, &height, p);
		gui_add_button_user (x_x11, y_x11, width, height, data1);
	}
}

void gsgl_add_combobox_external (guialldata *data1)
{
	int x_x11, y_x11;
	int width, height;
	itemdata *data = data1->common;
	gsglPage *p = data->p;

	if (gsgl_inq_mode (p->library) == GSGL_MODE_WINDOW) {
		gsgl_to_x11 (data->x, data->y, data->w, data->h, &x_x11, &y_x11, &width, &height, p);
		gui_add_combobox_user (x_x11, y_x11, width, height, data1);
	}
}

void gsgl_add_spinbuttonf_external (guialldata *data1)
{
	int x_x11, y_x11;
	int width, height;
	itemdata *data = data1->common;
	gsglPage *p = data->p;

	if (gsgl_inq_mode (p->library) == GSGL_MODE_WINDOW) {
		gsgl_to_x11 (data->x, data->y, data->w, data->h, &x_x11, &y_x11, &width, &height, p);
		gui_add_spinbuttonf_user (x_x11, y_x11, width, height, data1);
	}
}

void gsgl_add_spinbuttoni_external (guialldata *data1)
{
	int x_x11, y_x11;
	int width, height;
	itemdata *data = data1->common;
	gsglPage *p = data->p;

	if (gsgl_inq_mode (p->library) == GSGL_MODE_WINDOW) {
		gsgl_to_x11 (data->x, data->y, data->w, data->h, &x_x11, &y_x11, &width, &height, p);
		gui_add_spinbuttoni_user (x_x11, y_x11, width, height, data1);
	}
}

void gsgl_add_checkbutton_external (guialldata *data1)
{
	int x_x11, y_x11;
	int width, height;
	itemdata *data = data1->common;
	gsglPage *p = data->p;

	if (gsgl_inq_mode (p->library) == GSGL_MODE_WINDOW) {
		gsgl_to_x11 (data->x, data->y, data->w, data->h, &x_x11, &y_x11, &width, &height, p);
		gui_add_checkbutton_user (x_x11, y_x11, width, height, data1);
	}
}

void gsgl_add_lineedit_external (guialldata *data1)
{
	int x_x11, y_x11;
	int width, height;
	itemdata *data = data1->common;
	gsglPage *p = data->p;

	if (gsgl_inq_mode (p->library) == GSGL_MODE_WINDOW) {
		gsgl_to_x11 (data->x, data->y, data->w, data->h, &x_x11, &y_x11, &width, &height, p);
		gui_add_lineedit_user (x_x11, y_x11, width, height, data1);
	}
}

void gsgl_guiitems_button_clicked (guialldata *alldata, void *widget)
{
	buttondata *s;

	if (gsgl_inq_i_am_a_server (alldata->common->p->library)) {
		int id = find_id_from_widget (widget, alldata->common->p);
		if (id > 0) {
			gsgl_tcp_gui_button_clicked (id, alldata->common->p);
		} else
			gsgl_warning ("Button", "can not find item id");
	} else {
		if (alldata->common->type == ITEM_BUTTON) {
			s = (buttondata *) alldata->specific;
			if (s->callback) {
				gsgl_begin_guiitem (alldata->common->p);
				s->callback ();
				gsgl_end_guiitem (alldata->common->p);
			}
		}
	}
}

void gsgl_guiitems_checkbutton_toggled (guialldata *alldata, void *widget)
{
	checkbuttondata *s = (checkbuttondata *) alldata->specific;

	if (gsgl_inq_i_am_a_server (alldata->common->p->library)) {
		int id = find_id_from_widget (widget, alldata->common->p);
		if (id > 0)
			gsgl_tcp_gui_check_toggled (id, s->init, alldata->common->p);
		else
			gsgl_warning ("CheckButton", "can not find item id");
	} else {
		if (alldata->common->type == ITEM_CHECK && s->callback) {
			gsgl_begin_guiitem (alldata->common->p);
			s->callback (&(s->init));
			gsgl_end_guiitem (alldata->common->p);
		}
	}
}

void gsgl_guiitems_combobox_selected (guialldata *alldata, void *widget)
{
	int j;
	int id;
	comboboxdata *s = (comboboxdata *) alldata->specific;

	if (gsgl_inq_i_am_a_server (alldata->common->p->library)) {
		id = find_id_from_widget (widget, alldata->common->p);
		if (id > 0)
			gsgl_tcp_gui_combo_changed (id, s->current, alldata->common->p);
		else
			gsgl_warning ("ComboBox", "can not find item id");
	} else {
		if (alldata->common->type == ITEM_COMBO && s->callback) {
			gsgl_begin_guiitem (alldata->common->p);
			if (s->fortran) 
				j = s->current+1;
			else
				j = s->current;
			s->callback (&j);
			gsgl_end_guiitem (alldata->common->p);
		}
	}
}

void gsgl_guiitems_lineedit_changed (guialldata *alldata, void *widget)
{
	lineeditdata *s = (lineeditdata *) alldata->specific;

	if (gsgl_inq_i_am_a_server (alldata->common->p->library)) {
		int id = find_id_from_widget (widget, alldata->common->p);
		if (id > 0) {
			if (s->text)
				gsgl_tcp_gui_edit_changed (id, s->text, alldata->common->p);
		} else {
			gsgl_warning ("LineEdit", "can not find item id");
		}
	} else {
		if (alldata->common->type == ITEM_EDIT && s->callback && s->text) {
			int l = strlen(s->text);
			gsgl_begin_guiitem (alldata->common->p);
			s->callback (s->text, &l);
			gsgl_end_guiitem (alldata->common->p);
		}
	}
}

void gsgl_guiitems_spinbuttonf_changed (guialldata *alldata, void *widget)
{
	spinbuttondataf *s = (spinbuttondataf *) alldata->specific;

	if (gsgl_inq_i_am_a_server (alldata->common->p->library)) {
		int id = find_id_from_widget (widget, alldata->common->p);
		if (id > 0)
			gsgl_tcp_gui_spinf_valuechanged (id, s->init, alldata->common->p);
		else
			gsgl_warning ("SpinButtonf", "can not find item id");
	} else {
		if (alldata->common->type == ITEM_SPINF && s->callback) {
			gsgl_begin_guiitem (alldata->common->p);
			s->callback (&s->init);	
			gsgl_end_guiitem (alldata->common->p);
		}
	}
}

void gsgl_guiitems_spinbuttoni_changed (guialldata *alldata, void *widget)
{
	spinbuttondatai *s = (spinbuttondatai *) alldata->specific;

	if (gsgl_inq_i_am_a_server (alldata->common->p->library)) {
		int id = find_id_from_widget (widget, alldata->common->p);
		if (id > 0)
			gsgl_tcp_gui_spini_valuechanged (id, s->init, alldata->common->p);
		else
			gsgl_warning ("SpinButtoni", "can not find item id");
	} else {
		if (alldata->common->type == ITEM_SPINI && s->callback) {
			gsgl_begin_guiitem (alldata->common->p);
			s->callback (&s->init);	
			gsgl_end_guiitem (alldata->common->p);
		}
	}
}
