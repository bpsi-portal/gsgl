#include "gsaf.h"
#include <math.h>
#include "utils.h"
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk/gdkx.h>
#include "gtk_base.h"
#include "diacanvas.h"
#include "guiitems.h"
#include "tcpip.h"
#include "tcp_guiitems.h"
#include "gui_common.h"
#include "configfile.h"
#include "xwin.h"
#include "gsgl.h"
#include "gtk_gsglwindow.h"
#include "gtk_guiitems.h"

typedef struct _gsgl_gtk_widgets {
	GtkWidget *destroywidget;
	DiaCanvas *canvas;
	GtkWidget *sizewidget;
	GList *label;
} gsgl_gtk_widgets;

static void setfontsize (GtkWidget *widget, float chh);

void gui_guiitems_init_every_page ()
{
}

void gui_guiitems_finish_every_page (gsglPage *p)
{
	gsgllist *list1;
	itemnode *node;
	gsgl_gtk_widgets *widgets;
	gsglMainWindow *w = gsgl_inq_mainwin ();

	list1 = gsgl_guiitem_inq_list (w->Page);

	while (list1 != NULL) {
		node = (itemnode *) list1->data;
		if (node->toolkit == TK_GTK) {
			widgets = (gsgl_gtk_widgets *) node->other;
			gtk_widget_destroy (widgets->destroywidget);
			gtk_widget_destroy (GTK_WIDGET(widgets->canvas));
		}
		list1 = list1->next;
	}
}

void gui_guiitems_configure (gsglPage *p)
{
	gsgllist *list1;
	GList *label;
	itemnode *node;
	gsgl_gtk_widgets *widgets;
	guialldata *gdata;
	int x, y, w, h;
	gsglMainWindow *mw = gsgl_inq_mainwin ();
	
	list1 = gsgl_guiitem_inq_list (mw->Page);
	while (list1 != NULL) {
		node = (itemnode *) list1->data;
		if (node->toolkit == TK_GTK) {
			gdata = node->gdata;
			widgets = (gsgl_gtk_widgets *) node->other;

			gsgl_to_x11 (gdata->common->x, gdata->common->y, gdata->common->w, gdata->common->h, &x, &y, &w, &h, mw->Page);
		
			gtk_widget_set_usize (GTK_WIDGET(widgets->sizewidget), w, h);
#if GTK_MAJOR_VERSION != 2
			gtk_widget_set_usize (GTK_WIDGET(widgets->canvas), w, h);
#endif
			dia_canvas_move (DIA_CANVAS(gsgl_gtk_inq_glarea ()), GTK_WIDGET(widgets->canvas), x, y);
			label = widgets->label;
			while (label != NULL) {
				if (label->data)
					setfontsize ((GtkWidget *)label->data, gdata->common->chh);
				label = label->next;
			}
		}
		list1 = list1->next;
	}
}

static void setfontsize (GtkWidget *widget, float chh)
{
	gsglMainWindow *w = gsgl_inq_mainwin ();

#if GTK_MAJOR_VERSION == 2
	PangoFontDescription *font_desc = widget->style->font_desc;

	float dpi = gsgl_get_screen_resolution (w->Library);
        float pointSize = gsgl_inq_window_width (w->Win) / GSGL_PAPER_WIDTH / dpi * 72.0;
	pango_font_description_set_size (font_desc, (int) (chh * 1.46667 * pointSize * PANGO_SCALE));

	gtk_widget_modify_font (widget, font_desc);
#endif
}

static void setwidgetcolor (GtkWidget *widget)
{
	GdkColor black = {0,0x0000,0x0000,0x0000};
        GdkColor white = {0,0xffff,0xffff,0xffff}; 
	gsglMainWindow *w = gsgl_inq_mainwin ();

	if (gsgl_inq_bg_color (w->Library) == GSGL_BACK_COLOR_BLACK) {
                gdk_color_alloc (gdk_colormap_get_system(), &black);
                gdk_window_set_background (widget->window, &black);
		
		gdk_color_parse ("white", &white);
#if GTK_MAJOR_VERSION == 2
		gtk_widget_modify_fg (widget, GTK_STATE_NORMAL, &white);
#else
		GtkRcStyle *rc_style;
		/* After filling in your GdkColor, create a GtkRcStyle */
		rc_style = gtk_rc_style_new ();
		/* Set foreground (fg) color in normal state to red */
		rc_style->fg[GTK_STATE_NORMAL] = white;

		/* Indicate which colors the GtkRcStyle will affect; 
		 * unflagged colors will follow the theme
		 */
		rc_style->color_flags[GTK_STATE_NORMAL] = (GtkRcFlags)(rc_style->color_flags[GTK_STATE_NORMAL] | GTK_RC_FG);
		gtk_widget_modify_style (widget, rc_style);
		gtk_rc_style_unref (rc_style);
#endif
        } else {
                gdk_color_alloc (gdk_colormap_get_system(), &white);
                gdk_window_set_background (widget->window, &white);
        }
}

/* Label */

void gui_add_label_user (int x, int y, int w, int h, guialldata *data)
{
	GtkWidget *label;
	GtkWidget *canvas;
	labeldata *s = (labeldata *) data->specific;
	GList *list = NULL;
	gsgl_gtk_widgets *widgets;
	
	canvas = dia_canvas_new ();
	label = gtk_label_new (s->text);
	setfontsize (label, data->common->chh);
	gtk_widget_set_usize (label, w, h);

	if (s->ijust == JUST_LEFT)
		gtk_misc_set_alignment (GTK_MISC(label), 0.0, 0.5);
	else if (s->ijust == JUST_RIGHT)
		gtk_misc_set_alignment (GTK_MISC(label), 1.0, 0.5);
	else if (s->ijust == JUST_CENTER)
		gtk_misc_set_alignment (GTK_MISC(label), 0.5, 0.5);
	
	/* we need a window for the button */
	dia_canvas_put (DIA_CANVAS(canvas), label, 0, 0);
	dia_canvas_put (DIA_CANVAS(gsgl_gtk_inq_glarea ()), canvas, x, y);
	gtk_widget_show (label);
	gtk_widget_show (canvas);

	setwidgetcolor (label);

	list = g_list_append (list, label);
	
	widgets = (gsgl_gtk_widgets *) gsgl_malloc (sizeof(gsgl_gtk_widgets));
	widgets->destroywidget = label;
	widgets->sizewidget = label;
	widgets->canvas = DIA_CANVAS(canvas);
	widgets->label = list;

	gsgl_guiitem_append_item (TK_GTK, data, label, widgets);
}

static float getcurrentchh (void)
{
       float chh, chw1, chsp1, angl1, tilt1;
 
       gsaf_inqchr (&chh, &chw1, &chsp1, &angl1, &tilt1);
       return chh;
}

void gui_inq_label_size (const char *text, float *w, float *h, gsglPage *p)
{
	gsglMainWindow *mw = gsgl_inq_mainwin ();
#if GTK_MAJOR_VERSION == 2
	int w1, h1;
	PangoLayout *layout;
	PangoFontDescription *font_desc;
	float dpi, pointSize;

	font_desc = pango_font_description_copy (gsgl_gtk_inq_glarea()->style->font_desc);
	
	dpi = gsgl_get_screen_resolution (mw->Library);
        pointSize = gsgl_inq_window_width (mw->Win) / GSGL_PAPER_WIDTH / dpi * 72.0;
	pango_font_description_set_size (font_desc, 
			     (int) (getcurrentchh () * 1.46667 * pointSize * PANGO_SCALE));

	layout = gtk_widget_create_pango_layout (gsgl_gtk_inq_glarea(), text);
	pango_layout_set_font_description (layout, font_desc);

	pango_layout_get_size (layout, &w1, &h1);
	*w = (float) w1 / PANGO_SCALE / (float) gsgl_inq_window_width(mw->Win) * GSGL_PAPER_WIDTH;
	*h = (float) h1 / PANGO_SCALE / (float) gsgl_inq_window_height(mw->Win) * GSGL_PAPER_HEIGHT;
#else
	gint lbearing, rbearing, w1, ascent, descent, h1;
	GtkStyle *style;
	GdkFont *font;

	style = gtk_widget_get_style (gsgl_gtk_inq_glarea ());
	font = style->font;

	gdk_string_extents (font, text, &lbearing, &rbearing, &w1, &ascent, &descent);
	h1 = ascent + descent;
	*w = (float) w1 / (float) gsgl_inq_window_width(mw->Win) * GSGL_PAPER_WIDTH;
	*h = (float) h1 / (float) gsgl_inq_window_height(mw->Win) * GSGL_PAPER_HEIGHT;
#endif
}

/* Button */

static void gsgl_clicked (GtkWidget *widget, gpointer data1)
{
	buttondata *c;
	c = (buttondata *) data1;
	gsglMainWindow *w = gsgl_inq_mainwin ();

	if (gsgl_inq_i_am_a_server (w->Library)) {
		unsigned int id = find_id_from_widget (GTK_OBJECT(widget), w->Page);
		if (id > 0)
			gsgl_tcp_gui_button_clicked (id, w->Page);
		else
			gsgl_warning ("GTK+ Button", "can not find item id");
	} else {
		if (c->callback) {
			gsgl_begin_guiitem (w->Page);
			c->callback ();
			gsgl_end_guiitem (w->Page);
		}
	}
}

void gui_add_button_user (int x, int y, int w, int h, guialldata *data)
{
	GtkWidget *button, *label;
	GtkWidget *canvas;
	GList *list = NULL;
	gsgl_gtk_widgets *widgets;
	buttondata *s = (buttondata *)data->specific;

	canvas = dia_canvas_new ();
	button = gtk_button_new ();
	label = gtk_label_new (s->text);
	setfontsize (label, data->common->chh);
	gtk_container_add (GTK_CONTAINER(button), label);
	gtk_widget_set_usize (button, w, h);
	/* we need a window for the button */
	dia_canvas_put (DIA_CANVAS(canvas), button, 0, 0);
	dia_canvas_put (DIA_CANVAS(gsgl_gtk_inq_glarea ()), canvas, x, y);

	gtk_signal_connect (GTK_OBJECT(button), "clicked", GTK_SIGNAL_FUNC(gsgl_clicked), s);
	gtk_widget_show (label);
	gtk_widget_show (button);
	gtk_widget_show (canvas);

	list = g_list_append (list, label);

	widgets = (gsgl_gtk_widgets *) gsgl_malloc (sizeof(gsgl_gtk_widgets));
	widgets->destroywidget = button;
	widgets->sizewidget = button;
	widgets->canvas = DIA_CANVAS(canvas);
	widgets->label = list;

	gsgl_guiitem_append_item (TK_GTK, data, button, widgets);
}

/* ComboBox */

#if GTK_MAJOR_VERSION == 2 && GTK_MINOR_VERSION >= 4
void gsgl_changed (GtkComboBox *widget, comboboxdata *data)
#else
void gsgl_changed (GtkEntry *widget, comboboxdata *data)
#endif
{
	int j;
	gsglMainWindow *w = gsgl_inq_mainwin ();

#if GTK_MAJOR_VERSION == 2 && GTK_MINOR_VERSION >= 4
	data->current = gtk_combo_box_get_active (widget);
#else
	const gchar *string = gtk_entry_get_text(widget);
	for (j = 0; j < data->items; j++) {
		if (!strcmp (string, data->text[j])) {
			data->current = j;
			break;
		}
	}
#endif

	if (gsgl_inq_i_am_a_server (w->Library)) {
		unsigned int id = find_id_from_widget (GTK_OBJECT(widget), w->Page);
		if (id > 0)
			gsgl_tcp_gui_combo_changed (id, data->current, w->Page);
		else
			gsgl_warning ("UserComboBox", "can not find item id");
	} else {
		if (data && data->callback) {
			if (data->fortran)
				j = data->current + 1;
			else
				j = data->current;
			gsgl_begin_guiitem (w->Page);
			data->callback (&j);
			gsgl_end_guiitem (w->Page);
		}
	}
}

static void combo_set_font (GtkWidget *widget, gpointer data)
{
	float *chh = (float *) data;
	setfontsize (widget, *chh);
}

void gui_add_combobox_user (int x, int y, int w, int h, guialldata *data)
{
	GtkWidget *box;
	GtkWidget *canvas;
	GList *list = NULL;
	int i;
	gsgl_gtk_widgets *widgets;
	comboboxdata *s = (comboboxdata *)data->specific;

	canvas = dia_canvas_new ();
#if GTK_MAJOR_VERSION == 2 && GTK_MINOR_VERSION >= 4
	box = gtk_combo_box_new_text ();
#else
	box = gtk_combo_new ();
#endif
	setfontsize (box, data->common->chh);
	gtk_widget_set_usize (box, w, h);
	/* regist text */
#if GTK_MAJOR_VERSION == 2 && GTK_MINOR_VERSION >= 4
	for (i=0; i<s->items; i++) {
		gtk_combo_box_append_text (GTK_COMBO_BOX(box), s->text[i]);
	}
#else
	GList *items = NULL;
	
	for (i=0; i<s->items; i++) {
		items = g_list_append (items, s->text[i]);
	}
	gtk_combo_set_popdown_strings (GTK_COMBO (box), items);
       
	GtkEntry *entry = GTK_ENTRY(GTK_COMBO (box)->entry);
	gtk_entry_set_editable (entry, FALSE);
#endif

	gtk_container_forall (GTK_CONTAINER(box), combo_set_font, &(data->common->chh));

	/* set active item */
#if GTK_MAJOR_VERSION == 2 && GTK_MINOR_VERSION >= 4
	gtk_combo_box_set_active (GTK_COMBO_BOX(box), s->current);
#else
	gtk_list_select_item (GTK_LIST (GTK_COMBO(box)->list), s->current);		
#endif		

	/* we need a window for the combobox */
	dia_canvas_put (DIA_CANVAS(canvas), box, 0, 0);
	dia_canvas_put (DIA_CANVAS(gsgl_gtk_inq_glarea ()), canvas, x, y);

#if GTK_MAJOR_VERSION == 2 && GTK_MINOR_VERSION >= 4
	gtk_signal_connect (GTK_OBJECT(box), "changed", GTK_SIGNAL_FUNC(gsgl_changed), s);
#else
	gtk_signal_connect (GTK_OBJECT(entry), "changed", GTK_SIGNAL_FUNC(gsgl_changed), s);
#endif

	gtk_widget_show (box);
	gtk_widget_show (canvas);

#if GTK_MAJOR_VERSION == 2
	list = gtk_container_get_children (GTK_CONTAINER(box));
#else
	list = NULL;
#endif
	widgets = (gsgl_gtk_widgets *) gsgl_malloc (sizeof(gsgl_gtk_widgets));
	widgets->destroywidget = box;
	widgets->sizewidget = box;
	widgets->canvas = DIA_CANVAS(canvas);
	widgets->label = list;

	gsgl_guiitem_append_item (TK_GTK, data, box, widgets);
}

/* SpinButtonf */

typedef struct _spinf_data {
	spinbuttondataf *s;
	GtkSpinButton *spin;
} spinf_data;

static void gsgl_value_changed_f (GtkAdjustment *adjustment, gpointer data1)
{
	spinf_data *data = (spinf_data *) data1;
	spinbuttondataf *s = (spinbuttondataf *)data->s;
	gsglMainWindow *w = gsgl_inq_mainwin ();

#if GTK_MAJOR_VERSION == 2
	s->init = gtk_adjustment_get_value (adjustment);
#else
	s->init = gtk_spin_button_get_value_as_float (GTK_SPIN_BUTTON(data->spin));
#endif
	if (gsgl_inq_i_am_a_server (w->Library)) {
		unsigned int id = find_id_from_widget (GTK_OBJECT(adjustment), w->Page);
		if (id > 0)
			gsgl_tcp_gui_spinf_valuechanged (id, s->init, w->Page);
		else
			gsgl_warning ("UserSpinButtonf", "can not find item id");
	} else {
		if (s->callback) {
			gsgl_begin_guiitem (w->Page);
			s->callback (&(s->init));
			gsgl_end_guiitem (w->Page);
		}
	}
}

void gui_add_spinbuttonf_user (int x, int y, int w, int h, guialldata *data)
{
	GtkAdjustment *spinner_adj;
	GtkWidget *spinner;
	GtkWidget *canvas;
	GList *list = NULL;
	gsgl_gtk_widgets *widgets;
	spinbuttondataf *s = (spinbuttondataf *)data->specific;

	canvas = dia_canvas_new ();
	spinner_adj = (GtkAdjustment *) gtk_adjustment_new 
		(s->init, s->lower, s->upper, pow(10.0, -(s->digits)), 0.0, 0.0);
   
	/* creates the spinner */
	spinner = gtk_spin_button_new (spinner_adj, pow(10.0, -(s->digits)), s->digits);
	setfontsize (spinner, data->common->chh);

	gtk_widget_set_usize (spinner, w, h);
	dia_canvas_put (DIA_CANVAS(canvas), spinner, 0, 0);
	dia_canvas_put (DIA_CANVAS(gsgl_gtk_inq_glarea ()), canvas, x, y);

	gtk_adjustment_value_changed (GTK_ADJUSTMENT(spinner_adj));

	spinf_data *sdata;
	sdata = (spinf_data *) gsgl_malloc (sizeof(spinf_data));
	sdata->s = s;
	sdata->spin = GTK_SPIN_BUTTON(spinner);
	gtk_signal_connect (GTK_OBJECT (spinner_adj), "value-changed", GTK_SIGNAL_FUNC (gsgl_value_changed_f), sdata);

	gtk_widget_show (spinner);
	gtk_widget_show (canvas);
	setwidgetcolor (canvas);

	list = g_list_append (list, spinner);

	widgets = (gsgl_gtk_widgets *) gsgl_malloc (sizeof(gsgl_gtk_widgets));
	widgets->destroywidget = spinner;
	widgets->sizewidget = spinner;
	widgets->canvas = DIA_CANVAS(canvas);
	widgets->label = list;

	gsgl_guiitem_append_item (TK_GTK, data, spinner_adj, widgets);
}

/* SpinButtoni */

typedef struct _spini_data {
	spinbuttondatai *s;
	GtkSpinButton *spin;
} spini_data;

static void gsgl_value_changed_i (GtkAdjustment *adjustment, gpointer data1)
{
	spini_data *data = (spini_data *) data1;
	spinbuttondatai *s = (spinbuttondatai *)data->s;
	gsglMainWindow *w = gsgl_inq_mainwin ();

#if GTK_MAJOR_VERSION == 2
	s->init = (int) gtk_adjustment_get_value (adjustment);
#else
	s->init = (int) gtk_spin_button_get_value_as_float (GTK_SPIN_BUTTON(data->spin));
#endif
	if (gsgl_inq_i_am_a_server (w->Library)) {
		unsigned int id = find_id_from_widget (GTK_OBJECT(adjustment), w->Page);
		if (id > 0)
			gsgl_tcp_gui_spini_valuechanged (id, s->init, w->Page);
		else
			gsgl_warning ("UserSpinButton", "can not find item id");
	} else {
		if (s->callback) {
			gsgl_begin_guiitem (w->Page);
			s->callback (&(s->init));
			gsgl_end_guiitem (w->Page);
		}
	}
}

void gui_add_spinbuttoni_user (int x, int y, int w, int h, guialldata *data)
{
	GtkAdjustment *spinner_adj;
	GtkWidget *spinner;
	GtkWidget *canvas;
	GList *list = NULL;
	gsgl_gtk_widgets *widgets;
	spinbuttondatai *s = (spinbuttondatai *)data->specific;

	canvas = dia_canvas_new ();
	spinner_adj = (GtkAdjustment *) gtk_adjustment_new 
		(s->init, s->lower, s->upper, 1.0, 0.0, 0.0);
   
	/* creates the spinner */
	spinner = gtk_spin_button_new (spinner_adj, 1.0, 0);
	setfontsize (spinner, data->common->chh);

	gtk_widget_set_usize (spinner, w, h);
	dia_canvas_put (DIA_CANVAS(canvas), spinner, 0, 0);
	dia_canvas_put (DIA_CANVAS(gsgl_gtk_inq_glarea ()), canvas, x, y);

	gtk_adjustment_value_changed (GTK_ADJUSTMENT(spinner_adj));

	spini_data *sdata;
	sdata = (spini_data *) gsgl_malloc (sizeof(spini_data));
	sdata->s = s;
	sdata->spin = GTK_SPIN_BUTTON(spinner);
	gtk_signal_connect (GTK_OBJECT (spinner_adj), "value-changed", GTK_SIGNAL_FUNC (gsgl_value_changed_i), sdata);

	gtk_widget_show (spinner);
	gtk_widget_show (canvas);
	setwidgetcolor (canvas);

	list = g_list_append (list, spinner);

	widgets = (gsgl_gtk_widgets *) gsgl_malloc (sizeof(gsgl_gtk_widgets));
	widgets->destroywidget = spinner;
	widgets->sizewidget = spinner;
	widgets->canvas = DIA_CANVAS(canvas);
	widgets->label = list;

	gsgl_guiitem_append_item (TK_GTK, data, spinner_adj, widgets);
}

/* CheckButton */

static void gsgl_toggled (GtkToggleButton *togglebutton, gpointer data1)
{
	checkbuttondata *s = (checkbuttondata *)data1;
	s->init = gtk_toggle_button_get_active (togglebutton);
	gsglMainWindow *w = gsgl_inq_mainwin ();

	if (gsgl_inq_i_am_a_server (w->Library)) {
		unsigned int id = find_id_from_widget (GTK_OBJECT(togglebutton), w->Page);
		if (id > 0)
			gsgl_tcp_gui_check_toggled (id, s->init, w->Page);
		else
			gsgl_warning ("UserCheckButton", "can not find item id");
	} else {
		if (s->callback) {
			gsgl_begin_guiitem (w->Page);
			s->callback (&(s->init));
			gsgl_end_guiitem (w->Page);
		}
	}
}

void gui_add_checkbutton_user (int x, int y, int w, int h, guialldata *data)
{
	GtkWidget *check, *label;
	GtkWidget *canvas;
	GList *list = NULL;
	checkbuttondata *s = (checkbuttondata *) data->specific;
	gsgl_gtk_widgets *widgets;

	canvas = dia_canvas_new ();
	check = gtk_check_button_new ();
	label = gtk_label_new (s->text);
	setfontsize (label, data->common->chh);
	gtk_container_add (GTK_CONTAINER(check), label);
	gtk_widget_set_usize (check, w, h);

	gtk_toggle_button_set_mode (GTK_TOGGLE_BUTTON(check), TRUE);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(check), s->init);
	gtk_signal_connect (GTK_OBJECT(check), "toggled", GTK_SIGNAL_FUNC(gsgl_toggled), s);

	dia_canvas_put (DIA_CANVAS(canvas), check, 0, 0);
	dia_canvas_put (DIA_CANVAS(gsgl_gtk_inq_glarea ()), canvas, x, y);

	gtk_widget_show (label);
	gtk_widget_show (check);
	gtk_widget_show (canvas);

	setwidgetcolor (label);

	list = g_list_append (list, label);

	widgets = (gsgl_gtk_widgets *) gsgl_malloc (sizeof(gsgl_gtk_widgets));
	widgets->destroywidget = check;
	widgets->sizewidget = check;
	widgets->canvas = DIA_CANVAS(canvas);
	widgets->label = list;

	gsgl_guiitem_append_item (TK_GTK, data, check, widgets);
}


/* LineEditer */

static void text_changed (GtkEntry *entry, gpointer data1)
{
	int l;
	lineeditdata *s = (lineeditdata *) data1;
	gsglMainWindow *w = gsgl_inq_mainwin ();

	s->text = gsgl_strdup(gtk_entry_get_text (entry));
	l = strlen (s->text);

	if (gsgl_inq_i_am_a_server (w->Library)) {
		unsigned int id = find_id_from_widget (GTK_OBJECT(entry), w->Page);
		if (id > 0) {
			if (s->text)
				gsgl_tcp_gui_edit_changed (id, s->text, w->Page);
		} else {
			gsgl_warning ("UserLineEdit", "can not find item id");
		}
	} else {
		if (s->callback && s->text) {
			gsgl_begin_guiitem (w->Page);
			s->callback (s->text, &l);
			gsgl_end_guiitem (w->Page);
		}
	}
}

void gui_add_lineedit_user (int x, int y, int w, int h, guialldata *data)
{
	lineeditdata *s = (lineeditdata *) data->specific;
	GList *list = NULL;
	gsgl_gtk_widgets *widgets;
	GtkWidget *canvas = dia_canvas_new ();
	GtkWidget *entry = gtk_entry_new ();
	setfontsize (entry, data->common->chh);
	gtk_entry_set_max_length(GTK_ENTRY(entry), s->max);
	gtk_entry_set_text (GTK_ENTRY(entry), s->text);
	gtk_widget_set_usize (entry, w, h);
	
	gtk_signal_connect (GTK_OBJECT(entry), "changed", GTK_SIGNAL_FUNC(text_changed), s);

	dia_canvas_put (DIA_CANVAS(canvas), entry, 0, 0);
	dia_canvas_put (DIA_CANVAS(gsgl_gtk_inq_glarea ()), canvas, x, y);

	gtk_widget_show (entry);
	gtk_widget_show (canvas);

	list = g_list_append (list, entry);

	widgets = (gsgl_gtk_widgets *) gsgl_malloc (sizeof(gsgl_gtk_widgets));
	widgets->destroywidget = entry;
	widgets->sizewidget = entry;
	widgets->canvas = DIA_CANVAS(canvas);
	widgets->label = list;

	gsgl_guiitem_append_item (TK_GTK, data, entry, widgets);
}



/***********************************************************************/

static gboolean grab_default (GtkWidget *widget)
{
	gtk_widget_grab_default (widget);
	return FALSE;
}

typedef struct _edit_and_char 
{
	GtkWidget *window;
	GtkWidget *entry;
	char *text;
} editchar;

static void popupedit_ok (GtkWidget *widget, gpointer gdata)
{
	editchar *ec = (editchar *) gdata;
	ec->text = strdup(gtk_entry_get_text (GTK_ENTRY(ec->entry)));

	gtk_grab_remove (GTK_WIDGET (ec->window));
	gtk_widget_destroy (GTK_WIDGET (ec->window));

	gtk_main_quit ();
}

static void popupedit_cancel (GtkWidget *widget, gpointer gdata)
{
	editchar *ec = (editchar *) gdata;
	ec->text = NULL;

	gtk_grab_remove (GTK_WIDGET (ec->window));
	gtk_widget_destroy (GTK_WIDGET (ec->window));

	gtk_main_quit ();	
}

char * gui_popup_edit (char *in, char *title)
{
	GtkWidget *window;
	GtkWidget *entry;
	GtkWidget *button;
	editchar *ec;

	ec = (editchar *) gsgl_malloc (sizeof(editchar));
	ec->text = in;

        /*window = gsgl_dialog_new (_("LineEdit"), 5);*/
        window = gsgl_dialog_new (title, 5);       
	ec->window = window;

	/* line edit */
	entry = gtk_entry_new ();
	ec->entry = entry;
	if (in)
		gtk_entry_set_text (GTK_ENTRY (entry), in);
	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (window)->vbox), entry, FALSE, TRUE, 0);
	gtk_widget_grab_focus (entry);

	/* ok button */
	button = gtk_button_new_with_label (_("OK"));
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(window)->action_area), button, TRUE, TRUE, 0);
	gtk_signal_connect (GTK_OBJECT (button), "clicked", GTK_SIGNAL_FUNC (popupedit_ok), ec);
	gtk_signal_connect_object (GTK_OBJECT(entry), "focus_in_event", GTK_SIGNAL_FUNC (grab_default), GTK_OBJECT(button));
	gtk_signal_connect_object (GTK_OBJECT(entry), "activate", GTK_SIGNAL_FUNC (gtk_button_clicked), GTK_OBJECT(button));

	GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);
	gtk_widget_grab_default (button);
        gtk_widget_show (button);

	/* cancel button */
	button = gtk_button_new_with_label (_("Cancel"));
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(window)->action_area), button, TRUE, TRUE, 0);
	gtk_signal_connect (GTK_OBJECT (button), "clicked", GTK_SIGNAL_FUNC (popupedit_cancel), ec);
        gtk_widget_show (button);

        gtk_widget_show (entry);
        gtk_widget_show (window);
        gtk_grab_add (window);
	gtk_window_set_modal (GTK_WINDOW(window), TRUE);
	gtk_window_set_transient_for (GTK_WINDOW(window), GTK_WINDOW(gsgl_gtk_inq_appwindow ()));
	gtk_main ();
	
	return ec->text;
}

typedef struct _spinf_and_adj 
{
	GtkWidget *window;
	GtkWidget *spin;
	GtkAdjustment *adj;
	float value;
	float value_tmp;
	float upper;
} spinfadj;

static void popupspinf_ok (GtkWidget *widget, gpointer gdata)
{
	spinfadj *sad = (spinfadj *) gdata;

	sad->value = sad->value_tmp;

	gtk_grab_remove (GTK_WIDGET (sad->window));
	gtk_widget_destroy (GTK_WIDGET (sad->window));

	gtk_main_quit ();
}

static void popupspinf_cancel (GtkWidget *widget, gpointer gdata)
{
	spinfadj *sad = (spinfadj *) gdata;
	sad->value = sad->upper + 1.0;
	
	gtk_grab_remove (GTK_WIDGET (sad->window));
	gtk_widget_destroy (GTK_WIDGET (sad->window));

	gtk_main_quit ();
}

static gboolean value_changed_f (GtkWidget *w, gpointer gdata)
{
	spinfadj *sad = (spinfadj *) gdata;

#if GTK_MAJOR_VERSION == 2
	sad->value_tmp = gtk_adjustment_get_value (sad->adj);
#else
	sad->value_tmp = gtk_spin_button_get_value_as_float (GTK_SPIN_BUTTON(sad->spin));
#endif

	return FALSE;
}

float gui_popup_spinf (int digits, float init, float lower, float upper, gsglPage *p)
{
	GtkWidget *window;
	GtkWidget *spin;
	GtkAdjustment *spinner_adj;
	GtkWidget *button;
	GtkEntry *entry;
	spinfadj *sad;

	sad = (spinfadj *) gsgl_malloc (sizeof(spinfadj));
	sad->value = init;
	sad->value_tmp = init;
	sad->upper = upper;

        window = gsgl_dialog_new (_("SpinButton"), 5);       
	sad->window = window;

	/* spin button */
	spinner_adj = (GtkAdjustment *) gtk_adjustment_new (init, lower, upper, pow(10.0, -digits), 0.0, 0.0);
	sad->adj = spinner_adj;

	/* creates the spinner */
	spin = gtk_spin_button_new (spinner_adj, pow(10.0, -digits), digits);
	sad->spin = spin;
	gtk_adjustment_value_changed (GTK_ADJUSTMENT(spinner_adj));
	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (window)->vbox), spin, FALSE, TRUE, 0);

	/* ok button */
	button = gtk_button_new_with_label (_("OK"));
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(window)->action_area), button, TRUE, TRUE, 0);
	gtk_signal_connect (GTK_OBJECT (button), "clicked", GTK_SIGNAL_FUNC (popupspinf_ok), sad);
	entry = &(GTK_SPIN_BUTTON(spin)->entry);
	gtk_signal_connect_object (GTK_OBJECT(entry), "focus_in_event", GTK_SIGNAL_FUNC (grab_default), GTK_OBJECT(button));
	gtk_signal_connect (GTK_OBJECT(spinner_adj), "value_changed", GTK_SIGNAL_FUNC (value_changed_f), sad);
	gtk_signal_connect_object_after (GTK_OBJECT(entry), "activate", GTK_SIGNAL_FUNC (gtk_button_clicked), GTK_OBJECT(button));


	GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);
	gtk_widget_grab_default (button);
        gtk_widget_show (button);

	/* cancel button */
	button = gtk_button_new_with_label (_("Cancel"));
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(window)->action_area), button, TRUE, TRUE, 0);
	gtk_signal_connect (GTK_OBJECT (button), "clicked", GTK_SIGNAL_FUNC (popupspinf_cancel), sad);
        gtk_widget_show (button);

        gtk_widget_show (spin);
        gtk_widget_show (window);
        gtk_grab_add (window);
	gtk_window_set_modal (GTK_WINDOW(window), TRUE);
	gtk_window_set_transient_for (GTK_WINDOW(window), GTK_WINDOW(gsgl_gtk_inq_appwindow ()));

	gtk_main ();
	
	return sad->value;
}

typedef struct _spini_and_adj 
{
	GtkWidget *window;
	GtkWidget *spin;
	GtkAdjustment *adj;
	int value;
	int value_tmp;
	int upper;
} spiniadj;

static void popupspini_ok (GtkWidget *widget, gpointer gdata)
{
	spiniadj *sad = (spiniadj *) gdata;

	sad->value = sad->value_tmp;

	gtk_grab_remove (GTK_WIDGET (sad->window));
	gtk_widget_destroy (GTK_WIDGET (sad->window));

	gtk_main_quit ();
}

static gboolean value_changed_i (GtkWidget *w, gpointer gdata)
{
	spiniadj *sad = (spiniadj *) gdata;

#if GTK_MAJOR_VERSION == 2
	sad->value_tmp = (int) gtk_adjustment_get_value (GTK_ADJUSTMENT(sad->adj));
#else
	sad->value_tmp = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON(sad->spin));
#endif

	return FALSE;
}

static void popupspini_cancel (GtkWidget *widget, gpointer gdata)
{
	spiniadj *sad = (spiniadj *) gdata;
	sad->value = sad->upper + 1;
	
	gtk_grab_remove (GTK_WIDGET (sad->window));
	gtk_widget_destroy (GTK_WIDGET (sad->window));

	gtk_main_quit ();	
}

int gui_popup_spini (int init, int lower, int upper, gsglPage *p)
{
	GtkWidget *window;
	GtkWidget *spin;
	GtkAdjustment *spinner_adj;
	GtkWidget *button;
	spiniadj *sad;
	GtkEntry *entry;

	sad = (spiniadj *) gsgl_malloc (sizeof(spiniadj));
	sad->value = init;
	sad->upper = upper;

        window = gsgl_dialog_new (_("SpinButton"), 5);       
	sad->window = window;

	/* spin button */
	spinner_adj = (GtkAdjustment *) gtk_adjustment_new (init, lower, upper, 1.0, 0.0, 0.0);
	sad->adj = spinner_adj;

	/* creates the spinner */
	spin = gtk_spin_button_new (spinner_adj, 1.0, 0);
	sad->spin = spin;
	gtk_adjustment_value_changed (GTK_ADJUSTMENT(spinner_adj));
	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (window)->vbox), spin, FALSE, TRUE, 0);

	/* ok button */
	button = gtk_button_new_with_label (_("OK"));
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(window)->action_area), button, TRUE, TRUE, 0);
	gtk_signal_connect (GTK_OBJECT (button), "clicked", GTK_SIGNAL_FUNC (popupspini_ok), sad);
	
	entry = &(GTK_SPIN_BUTTON(spin)->entry);
	gtk_signal_connect_object (GTK_OBJECT(entry), "focus_in_event", GTK_SIGNAL_FUNC (grab_default), GTK_OBJECT(button));
	gtk_signal_connect (GTK_OBJECT(spinner_adj), "value_changed", GTK_SIGNAL_FUNC (value_changed_i), sad);
	gtk_signal_connect_object_after (GTK_OBJECT(entry), "activate", GTK_SIGNAL_FUNC (gtk_button_clicked), GTK_OBJECT(button));
	GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);
	gtk_widget_grab_default (button);
        gtk_widget_show (button);

	/* cancel button */
	button = gtk_button_new_with_label (_("Cancel"));
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(window)->action_area), button, TRUE, TRUE, 0);
	gtk_signal_connect (GTK_OBJECT (button), "clicked", GTK_SIGNAL_FUNC (popupspini_cancel), sad);
        gtk_widget_show (button);

        gtk_widget_show (spin);
        gtk_widget_show (window);
        gtk_grab_add (window);
	gtk_window_set_modal (GTK_WINDOW(window), TRUE);
	gtk_window_set_transient_for (GTK_WINDOW(window), GTK_WINDOW(gsgl_gtk_inq_appwindow ()));

	gtk_main ();
	
	return sad->value;
}

void gui_exit_loop (void)
{
	gtk_main_quit ();
}
