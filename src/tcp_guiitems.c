#include "gsaf.h"
#include "tcp_guiitems.h"
#include "guiitems.h"
#include "fileformat.h"
#include "utils.h"
#include "tcpip.h"
#include "drv_file.h"
#include "mem_list.h"
#include "displaylist.h"
#include "drv_wrapper.h"

/* GUI ITEMS */

void gsgl_tcp_guiitems_init_every_page ()
{
}

void gsgl_tcp_guiitems_finish_every_page ()
{
}

void gsgl_tcp_begin_objects_after_pagee (gsglPage *p)
{
	gsgl_tcp_push (GSGL_TCP_GUI_BEGIN_OBJECTS, p);
}

void gsgl_tcp_end_objects_after_pagee (gsglPage *p)
{
	gsgl_tcp_push (GSGL_TCP_GUI_END_OBJECTS, p);
}

/************** BUTTON ******************/

/* From server to client */
void gsgl_tcp_gui_button_clicked (unsigned int id, gsglPage *p)
{
	gsgl_tcp_push_server (GSGL_TCP_GUI_BUTTON_CLICKED, p);
	gsgl_tcp_push_server (id, p);
}

/************** COMBOBOX ******************/

/* From server to client */
void gsgl_tcp_gui_combo_changed (unsigned int id, int item, gsglPage *p)
{
	gsgl_tcp_push_server (GSGL_TCP_GUI_COMBO_CHANGED, p);
	gsgl_tcp_push_server (id, p);
	gsgl_tcp_push_server (GSGL_TCP_GUI_COMBO_ITEM, p);
	gsgl_tcp_push_server (item, p);
}

/***************** SPINBOX *************************/

/* From server to client */
void gsgl_tcp_gui_spinf_valuechanged (unsigned int id, float value, gsglPage *p)
{
	gsgl_tcp_push_server (GSGL_TCP_GUI_SPINF_VALUECHANGED, p);
	gsgl_tcp_push_server (id, p);
	gsgl_tcp_push_server (GSGL_TCP_GUI_SPINF_VALUE, p);
	gsgl_file_push_float (value, gsgl_tcp_push_server, p);
}

/* From server to client */
void gsgl_tcp_gui_spini_valuechanged (unsigned int id, int value, gsglPage *p)
{
	unsigned int i;

	gsgl_tcp_push_server (GSGL_TCP_GUI_SPINI_VALUECHANGED, p);
	gsgl_tcp_push_server (id, p);
	gsgl_tcp_push_server (GSGL_TCP_GUI_SPINI_VALUE, p);
	i = value + 2147483647;
        gsgl_tcp_push_server ((unsigned int) gsgl_convert_4byteint_to_1st2byte (i), p);
        gsgl_tcp_push_server ((unsigned int) gsgl_convert_4byteint_to_2nd2byte (i), p);
}

/******************* CHECK BUTTON ***********************/

/* From server to client */
void gsgl_tcp_gui_check_toggled (unsigned int id, int active, gsglPage *p)
{
	gsgl_tcp_push_server (GSGL_TCP_GUI_CHECK_TOGGLED, p);
	gsgl_tcp_push_server (id, p);
	gsgl_tcp_push_server (GSGL_TCP_GUI_CHECK_ACTIVE, p);
	gsgl_tcp_push_server (active, p);
}

/******************** LINE EDIT ***********************/

/* From server to client */
void gsgl_tcp_gui_edit_changed (unsigned int id, const char *text, gsglPage *p)
{
	unsigned int i, l;
	l = strlen (text);

	gsgl_tcp_push_server (GSGL_TCP_GUI_EDIT_CHANGED, p);
	gsgl_tcp_push_server (id, p);
	gsgl_tcp_push_server (GSGL_TCP_GUI_EDIT_TEXT, p);
	gsgl_tcp_push_server (l, p);
	for (i=0; i<l; i++)
		gsgl_tcp_push_server ((unsigned int) text[i], p);
}

/******************* Add Text *************************/

/* From server to client */
void gsgl_tcp_gui_add_text (char *str, float x, float y, int fnt, float size, float angle, int ijust, gsglPage *p)
{
	unsigned int i, l;
	l = strlen (str);
	
	gsgl_tcp_push_server (GSGL_TCP_GUI_ADD_TEXT, p);
	gsgl_tcp_push_server (l, p);
	for (i=0; i<l; i++)
		gsgl_tcp_push_server ((unsigned int) str[i], p);
	gsgl_file_push_float (x, gsgl_tcp_push_server, p);
	gsgl_file_push_float (y, gsgl_tcp_push_server, p);
	gsgl_tcp_push_server (fnt, p);
	gsgl_file_push_float (size, gsgl_tcp_push_server, p);
	gsgl_file_push_float (angle, gsgl_tcp_push_server, p);
	gsgl_tcp_push_server (ijust, p);
}

/* client-side action */
void gsgl_tcp_guiitems_callback (unsigned int id, void *data, gsglPage *p)
{
	int *itemp;
	int item;
	float *value;
	int *active;
	char *text;
	int l;

	gsgllist *list0;
	itemnode *node;
	guialldata *g;
	buttondata *button;
	checkbuttondata *check;
	comboboxdata *combo;
	lineeditdata *edit;
	spinbuttondataf *spinf;
	spinbuttondatai *spini;

	list0 = find_list_from_id (id, p);
	if (!list0) return;
	if (!list0->data) return;
	node = (itemnode *) list0->data;
	g = node->gdata;
	switch (g->common->type) {
	case ITEM_BUTTON:
		button = g->specific;
		if (!button->callback) return;
		gsgl_tcp_begin_objects_after_pagee (p);

		button->callback ();

		gsgl_tcp_end_objects_after_pagee (p);
		break;
	case ITEM_COMBO:
		combo = g->specific;
		if (!combo->callback) return;
		gsgl_tcp_begin_objects_after_pagee (p);

		itemp = (int *) data;
		item = *itemp;
		combo->current = item;
		if (combo->fortran) {
			item++;
			combo->callback (&item);
		} else {
			combo->callback (&item);			
		}

		gsgl_tcp_end_objects_after_pagee (p);
		break;
	case ITEM_SPINF:
		spinf = g->specific;
		if (!spinf->callback) return;
		gsgl_tcp_begin_objects_after_pagee (p);

		value = (float *) data;
		spinf->init = *value;
		spinf->callback (value);

		gsgl_tcp_end_objects_after_pagee (p);
		break;
	case ITEM_SPINI:
		spini = g->specific;
		if (!spini->callback) return;
		gsgl_tcp_begin_objects_after_pagee (p);

		itemp = (int *) data;
		spini->init = *itemp;
		spini->callback (itemp);

		gsgl_tcp_end_objects_after_pagee (p);
		break;
	case ITEM_CHECK:
		check = g->specific;
		if (!check->callback) return;
		gsgl_tcp_begin_objects_after_pagee (p);

		active= (int *) data;
		check->init = *active;
		check->callback (active);

		gsgl_tcp_end_objects_after_pagee (p);
		break;
	case ITEM_EDIT:
		edit = g->specific;
		if (!edit->callback) return;
		gsgl_tcp_begin_objects_after_pagee (p);

		text = (char *) data;
		edit->text = text;
		l = strlen(text);
		edit->callback (text, &l);

		gsgl_tcp_end_objects_after_pagee (p);
		break;
	default:
		return;
	}
}
