#include "gsaf.h"
#include "gsgl.h"
#include <gtk/gtk.h>
#include "gtk_base.h"
#include "gtk_gsglwindow.h"
#include "gtk_menuedit.h"
#include "gtk_config.h"
#include "guiitems.h"
#include "gtk_guiitems.h"
#include "gui_common.h"
#include "tcpip.h"
#include "tcp_guiitems.h"
#include "utils.h"
#include "callback.h"

static gchar *fonts[] = {
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

static char *just[] = {
	_("0 Left"),
	_("1 Right"),
	_("2 Center"),
	_("3 Point Arithmetic")
};

static void reset_cursor_text (GtkWidget *w, gpointer gdata)
{
	gui_cursor_default (gsgl_inq_mainwin ());
	
	gsglMainWindow *mw = gsgl_inq_mainwin ();
	mw->Add_text = GSGL_FALSE;
	mw->Add_text_dialog = NULL;
}

static void reset_cursor_line (GtkWidget *w, gpointer gdata)
{
	gui_cursor_default (gsgl_inq_mainwin ());
	
	gsglMainWindow *mw = gsgl_inq_mainwin ();
	mw->Add_line = GSGL_FALSE;
	mw->Add_line_dialog = NULL;
}

static void text_dialog (void)
{
	int i;
	gint root_x, root_y;
	gint width, height;
	GtkWidget *w, *combo, *spin, *label, *table;
	gsglMainWindow *mw = gsgl_inq_mainwin ();

	if (mw->Add_text && mw->Add_text_dialog) return;

	mw->Add_text = GSGL_TRUE;
	w = gsgl_dialog_new (_("Add Texts"), 10);
	mw->Add_text_dialog = gsgl_list_append (mw->Add_text_dialog, w);
	gtk_signal_connect (GTK_OBJECT (w), "destroy",
                            GTK_SIGNAL_FUNC (reset_cursor_text), w);

#if GTK_MAJOR_VERSION == 2 
	gtk_window_get_position (GTK_WINDOW(gsgl_gtk_inq_appwindow ()), &root_x, &root_y);
	gtk_window_get_size (GTK_WINDOW(gsgl_gtk_inq_appwindow ()), &width, &height);
	gtk_window_move (GTK_WINDOW(w), (int) (root_x+width * 0.85), (int) (root_y + height * 0.15));
#endif
	gtk_box_set_spacing (GTK_BOX(GTK_DIALOG(w)->vbox), 5);

	/* Make a Table */
	table = gtk_table_new (2, 4, FALSE);
	gtk_table_set_col_spacing (GTK_TABLE(table), 0, 15);
	gtk_table_set_row_spacing (GTK_TABLE(table), 0, 5);
	gtk_container_add (GTK_CONTAINER(GTK_DIALOG(w)->vbox), table);

	/* Select Font Number */
	label = gtk_label_new (_("Font"));

#if GTK_MAJOR_VERSION == 2 && GTK_MINOR_VERSION >= 4
	combo = gtk_combo_box_new_text ();
	for (i = 0; i < 33; i++)
		gtk_combo_box_append_text (GTK_COMBO_BOX(combo), fonts[i]);
	gtk_combo_box_set_active (GTK_COMBO_BOX(combo), 0);
#else
	GList *items = NULL;

	for (i = 0; i < 33; i++)
		items = g_list_append (items, fonts[i]);

	combo = gtk_combo_new();
	gtk_combo_set_popdown_strings (GTK_COMBO (combo), items);
	gtk_list_select_item (GTK_LIST (GTK_COMBO(combo)->list), 0);
	gtk_entry_set_editable (GTK_ENTRY (GTK_COMBO (combo)->entry), FALSE);
#endif
	mw->Add_text_dialog = gsgl_list_append (mw->Add_text_dialog, combo);

	gtk_table_attach_defaults (GTK_TABLE(table), label, 0, 1, 0, 1);
	gtk_table_attach_defaults (GTK_TABLE(table), combo, 1, 2, 0, 1);

	gtk_widget_show (label);
	gtk_widget_show (combo);

	/* Select Font Size */
	label = gtk_label_new (_("Size"));

#if GTK_MAJOR_VERSION == 2
	spin = gtk_spin_button_new_with_range  (0.01, 10.0, 0.01);
	gtk_spin_button_set_value (GTK_SPIN_BUTTON(spin), 0.35);
#else
	GtkObject *adjustment = gtk_adjustment_new (0.35, 0.01, 10.0, 0.01, 0.01, 0.01);
	spin = gtk_spin_button_new (GTK_ADJUSTMENT(adjustment), 1.0, 2);
#endif
	mw->Add_text_dialog = gsgl_list_append (mw->Add_text_dialog, spin);

	gtk_table_attach_defaults (GTK_TABLE(table), label, 0, 1, 1, 2);
	gtk_table_attach_defaults (GTK_TABLE(table), spin, 1, 2, 1, 2);

	gtk_widget_show (label);
	gtk_widget_show (spin);
	
	/* Angle */
	label = gtk_label_new (_("Angle"));

#if GTK_MAJOR_VERSION == 2
	spin = gtk_spin_button_new_with_range  (-360.0, 360.0, 1.0);
	gtk_spin_button_set_value (GTK_SPIN_BUTTON(spin), 0.0);
#else
	adjustment = gtk_adjustment_new (0.0, -360.0, 360.0, 1.0, 1.0, 1.0);
	spin = gtk_spin_button_new (GTK_ADJUSTMENT(adjustment), 1.0, 0);
#endif
	mw->Add_text_dialog = gsgl_list_append (mw->Add_text_dialog, spin);

	gtk_table_attach_defaults (GTK_TABLE(table), label, 0, 1, 2, 3);
	gtk_table_attach_defaults (GTK_TABLE(table), spin, 1, 2, 2, 3);

	gtk_widget_show (label);
	gtk_widget_show (spin);

	/* Justification */
	label = gtk_label_new (_("Justification"));
#if GTK_MAJOR_VERSION == 2 && GTK_MINOR_VERSION >= 4
	combo = gtk_combo_box_new_text ();
	for (i = 0; i < 4; i++)
		gtk_combo_box_append_text (GTK_COMBO_BOX(combo), just[i]);
	gtk_combo_box_set_active (GTK_COMBO_BOX(combo), 0);
#else
	items = NULL;

	for (i = 0; i < 4; i++)
		items = g_list_append (items, just[i]);

	combo = gtk_combo_new();
	gtk_combo_set_popdown_strings (GTK_COMBO (combo), items);
	gtk_list_select_item (GTK_LIST (GTK_COMBO(combo)->list), 0);
	gtk_entry_set_editable (GTK_ENTRY (GTK_COMBO (combo)->entry), FALSE);
#endif
	mw->Add_text_dialog = gsgl_list_append (mw->Add_text_dialog, combo);

	gtk_table_attach_defaults (GTK_TABLE(table), label, 0, 1, 3, 4);
	gtk_table_attach_defaults (GTK_TABLE(table), combo, 1, 2, 3, 4);

	gtk_widget_show (label);
	gtk_widget_show (combo);

	/* Close button */
	gsgl_button_on_dialog_new (_("Close"), w, reset_cursor_text, NULL, GSGL_TRUE);
	gtk_window_set_transient_for (GTK_WINDOW(w), GTK_WINDOW(gsgl_gtk_inq_appwindow ()));

	gtk_widget_show (table);
	gtk_widget_show (w);
}

void gsgl_gtk_add_text (gpointer gdata, guint action, GtkWidget *widget)
{
        gsglMainWindow *w = gsgl_inq_mainwin ();

	if (w->Add_line)
		gsgl_gui_close_add_line_dialog ();
		
	gsgl_gui_pointer (w->Page, GSGL_FALSE);
	gui_cursor_pencil (w);
	text_dialog ();
}

static void toggle (GtkToggleButton *togglebutton, gpointer user_data)
{
	GtkWidget *spin = GTK_WIDGET(user_data);
	gtk_widget_set_sensitive (spin, gtk_toggle_button_get_active (togglebutton));
}

static void set_area (GtkWidget *widget, gsgllist *l)
{
        gdouble color[3];
	if (!l) return;
	GtkWidget *colorseldlg = (GtkWidget *) l->data;
	GtkWidget *colorsel = GTK_COLOR_SELECTION_DIALOG(colorseldlg)->colorsel;
	l = l->next;
	GtkWidget *area = (GtkWidget *) l->data;
	
#if GTK_MAJOR_VERSION == 1
        /* Get current color */
        gtk_color_selection_get_color (GTK_COLOR_SELECTION(colorsel), color);

#elif GTK_MAJOR_VERSION == 2
        GdkColor gcolor;

        gtk_color_selection_get_current_color (GTK_COLOR_SELECTION(colorsel), &gcolor);

        color[0] = gcolor.red/65535.0;
        color[1] = gcolor.green/65535.0;
        color[2] = gcolor.blue/65535.0;
#endif
        gsgl_gtk_draw_area (area, color);
}

static void area_event (GtkWidget *widget, GdkEvent *event, GtkWidget *area)
{
        if (event->type != GDK_BUTTON_PRESS) return;
	
        GtkWidget *colorseldlg = gtk_color_selection_dialog_new("RGB Color");
	gsgllist *l = NULL;
	l = gsgl_list_append (l, colorseldlg);
	l = gsgl_list_append (l, area);
        
        gtk_widget_destroy(GTK_COLOR_SELECTION_DIALOG(colorseldlg)->help_button);
        
        gtk_signal_connect (GTK_OBJECT (GTK_COLOR_SELECTION_DIALOG (colorseldlg)->ok_button), "clicked", GTK_SIGNAL_FUNC (set_area), l);
        gtk_signal_connect (GTK_OBJECT (GTK_COLOR_SELECTION_DIALOG (colorseldlg)->ok_button), "clicked", GTK_SIGNAL_FUNC (gsgl_closedialog), colorseldlg);
        
        gtk_signal_connect (GTK_OBJECT (GTK_COLOR_SELECTION_DIALOG (colorseldlg)->cancel_button), "clicked", GTK_SIGNAL_FUNC (gsgl_closedialog), colorseldlg);

        gtk_widget_show(colorseldlg);
        gtk_grab_add (colorseldlg);
}

static void line_dialog (void)
{
#if 0
	int i;
	gint root_x, root_y;
	gint width, height;
	GtkWidget *w, *combo, *spin, *label, *table;
	gsglMainWindow *mw = gsgl_inq_mainwin ();

	if (mw->Add_line && mw->Add_line_dialog) return;

	mw->Add_line = GSGL_TRUE;
	w = gsgl_dialog_new (_("Add Lines"), 10);
	mw->Add_line_dialog = gsgl_list_append (mw->Add_line_dialog, w);
	gtk_signal_connect (GTK_OBJECT (w), "destroy",
                            GTK_SIGNAL_FUNC (reset_cursor_line), w);

#if GTK_MAJOR_VERSION == 2 
	gtk_window_get_position (GTK_WINDOW(gsgl_gtk_inq_appwindow ()), &root_x, &root_y);
	gtk_window_get_size (GTK_WINDOW(gsgl_gtk_inq_appwindow ()), &width, &height);
	gtk_window_move (GTK_WINDOW(w), (int) (root_x+width * 0.85), (int) (root_y + height * 0.15));
#endif
	gtk_box_set_spacing (GTK_BOX(GTK_DIALOG(w)->vbox), 5);

	/* Make a Table */
	table = gtk_table_new (2, 11, FALSE);
	gtk_table_set_col_spacing (GTK_TABLE(table), 0, 15);
	gtk_table_set_row_spacing (GTK_TABLE(table), 0, 5);
	gtk_container_add (GTK_CONTAINER(GTK_DIALOG(w)->vbox), table);

	/* Set Line Width */
	label = gtk_label_new (_("Line Width"));
	gtk_table_attach_defaults (GTK_TABLE(table), label, 0, 1, 0, 1);
	gtk_widget_show (label);
	GtkWidget *radio1 = gtk_radio_button_new_with_label (NULL, N_("point"));
	GtkWidget *radio2 = gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON(radio1), N_("cm"));
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(radio1), TRUE);
	gtk_table_attach_defaults (GTK_TABLE(table), radio1, 0, 1, 1, 2);
	gtk_table_attach_defaults (GTK_TABLE(table), radio2, 0, 1, 2, 3);
	gtk_widget_show (radio1);
	gtk_widget_show (radio2);

#if GTK_MAJOR_VERSION == 2
	spin = gtk_spin_button_new_with_range  (0.25, 10.0, 0.25);
	gtk_spin_button_set_value (GTK_SPIN_BUTTON(spin), 1.0);
	gtk_spin_button_set_digits (GTK_SPIN_BUTTON(spin), 2);
#else
	adjustment = gtk_adjustment_new (1.0, 0.25, 10.0, 0.25, 1.0, 1.0);
	spin = gtk_spin_button_new (GTK_ADJUSTMENT(adjustment), 1.0, 2);
#endif
	mw->Add_text_dialog = gsgl_list_append (mw->Add_text_dialog, spin);
	gtk_table_attach_defaults (GTK_TABLE(table), spin, 1, 2, 1, 2);
	gtk_signal_connect (GTK_OBJECT (radio1), "toggled", GTK_SIGNAL_FUNC (toggle), spin);
	gtk_widget_show (spin);

#if GTK_MAJOR_VERSION == 2
	spin = gtk_spin_button_new_with_range  (0.001, 10.00, 0.001);
	gtk_spin_button_set_value (GTK_SPIN_BUTTON(spin), 0.035);
	gtk_spin_button_set_digits (GTK_SPIN_BUTTON(spin), 3);
#else
	adjustment = gtk_adjustment_new (0.035, 0.001, 10.0, 0.001, 1.0, 1.0);
	spin = gtk_spin_button_new (GTK_ADJUSTMENT(adjustment), 1.0, 3);
#endif
	gtk_widget_set_sensitive (spin, FALSE);
	mw->Add_text_dialog = gsgl_list_append (mw->Add_text_dialog, spin);
	gtk_table_attach_defaults (GTK_TABLE(table), spin, 1, 2, 2, 3);
	gtk_signal_connect (GTK_OBJECT (radio2), "toggled", GTK_SIGNAL_FUNC (toggle), spin);
	gtk_widget_show (spin);
	
	/* Set Line Color */
	label = gtk_label_new (_("Line Color"));
	gtk_table_attach_defaults (GTK_TABLE(table), label, 0, 1, 3, 4);
	gtk_widget_show (label);
	radio1 = gtk_radio_button_new_with_label (NULL, N_("Index"));
	radio2 = gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON(radio1), N_("RGB"));
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(radio1), TRUE);
	gtk_table_attach_defaults (GTK_TABLE(table), radio1, 0, 1, 4, 5);
	gtk_table_attach_defaults (GTK_TABLE(table), radio2, 0, 1, 5, 6);
	gtk_widget_show (radio1);
	gtk_widget_show (radio2);

	/* Index Color */
	static char *index[] = {"0 White",
				"1 Aqua",
				"2 Yellow",
				"3 Purple",
				"4 Green",
				"5 Blue",
				"6 Red",
				"7 Black"};
#if GTK_MAJOR_VERSION == 2 && GTK_MINOR_VERSION >= 4
	combo = gtk_combo_box_new_text ();
	for (i = 0; i < 8; i++)
		gtk_combo_box_append_text (GTK_COMBO_BOX(combo), index[i]);
	gtk_combo_box_set_active (GTK_COMBO_BOX(combo), 7);
#else
	GList *items = NULL;
	for (i = 0; i < 8; i++)
		items = g_list_append (items, index[i]);
	combo = gtk_combo_new();
	gtk_combo_set_popdown_strings (GTK_COMBO (combo), items);
	gtk_list_select_item (GTK_LIST (GTK_COMBO(combo)->list), 7);
	gtk_entry_set_editable (GTK_ENTRY (GTK_COMBO (combo)->entry), FALSE);
#endif
	gtk_table_attach_defaults (GTK_TABLE(table), combo, 1, 2, 4, 5);
	gtk_signal_connect (GTK_OBJECT (radio1), "toggled", GTK_SIGNAL_FUNC (toggle), combo);
	gtk_widget_show (combo);

	/* RGB Color */
	double *color = (double *) gsgl_malloc_atomic(sizeof(double)*3);
	color[0] = 0.0; color[1] = 0.0; color[2] = 0.0; 

	GtkWidget *area = gtk_drawing_area_new ();
        gtk_drawing_area_size (GTK_DRAWING_AREA(area), 60, 20);
        gtk_widget_set_events (area, GDK_BUTTON_PRESS_MASK);

	gtk_signal_connect (GTK_OBJECT(area), "event", 
			    GTK_SIGNAL_FUNC (area_event), (gpointer) area);
	
	gtk_table_attach_defaults (GTK_TABLE(table), area, 1, 2, 5, 6);
	gtk_signal_connect (GTK_OBJECT (radio2), "toggled", GTK_SIGNAL_FUNC (toggle), area);
	gtk_widget_set_sensitive (area, FALSE);
	gtk_widget_show (area);

	/* Line Type */
	label = gtk_label_new (_("Line Type"));
	gtk_table_attach_defaults (GTK_TABLE(table), label, 0, 1, 6, 7);
	gtk_widget_show (label);

#include "image/image0.xpm"
#include "image/image1.xpm"
#include "image/image2.xpm"
#include "image/image3.xpm"
#include "image/image4.xpm"
#include "image/image5.xpm"
#include "image/image6.xpm"
#include "image/image7.xpm"
	GdkPixbuf *p0[8];
	GdkPixbuf *p[8];
	const char *number[] = {"0", "1", "2", "3", "4", "5", "6", "7"};
	p0[0] = gdk_pixbuf_new_from_xpm_data  (image0_xpm);
	p0[1] = gdk_pixbuf_new_from_xpm_data  (image1_xpm);
	p0[2] = gdk_pixbuf_new_from_xpm_data  (image2_xpm);
	p0[3] = gdk_pixbuf_new_from_xpm_data  (image3_xpm);
	p0[4] = gdk_pixbuf_new_from_xpm_data  (image4_xpm);
	p0[5] = gdk_pixbuf_new_from_xpm_data  (image5_xpm);
	p0[6] = gdk_pixbuf_new_from_xpm_data  (image6_xpm);
	p0[7] = gdk_pixbuf_new_from_xpm_data  (image7_xpm);
	for (i=0; i<8; i++)
		p[i] = gdk_pixbuf_scale_simple (p0[i], 100, 10, GDK_INTERP_BILINEAR);

	GtkListStore *store = gtk_list_store_new (2, G_TYPE_STRING, GDK_TYPE_PIXBUF);

	GtkTreeIter   iter;
	for (i=0; i<8; i++) {
		gtk_list_store_append (store, &iter);  /* Acquire a top-level iterator */
		gtk_list_store_set (store, &iter, 0, number[i], 1, p[i], -1);
	}
	combo = gtk_combo_box_new_with_model (GTK_TREE_MODEL (store));
	
	g_object_unref (G_OBJECT (store));
	GtkCellRenderer *renderer = gtk_cell_renderer_text_new ();
	GtkCellRenderer *renderer2 = gtk_cell_renderer_pixbuf_new ();

	gtk_cell_layout_clear (GTK_CELL_LAYOUT (combo));
	gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), renderer, TRUE);
	gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer, "text", 0, NULL);
	gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), renderer2, TRUE);
	gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer2, "pixbuf", 1, NULL);
	
	gtk_combo_box_set_active (GTK_COMBO_BOX(combo), 0);
	gtk_table_attach_defaults (GTK_TABLE(table), combo, 0, 2, 7, 8);
	gtk_widget_show (combo);

	/* Arrows */
	label = gtk_label_new (_("Arrows"));
	gtk_table_attach_defaults (GTK_TABLE(table), label, 0, 1, 8, 9);
	gtk_widget_show (label);

	label = gtk_label_new (_("Origin"));
	gtk_table_attach_defaults (GTK_TABLE(table), label, 0, 1, 9, 10);
	gtk_widget_show (label);

	label = gtk_label_new (_("End Point"));
	gtk_table_attach_defaults (GTK_TABLE(table), label, 1, 2, 9, 10);
	gtk_widget_show (label);


	/* Close button */
	gsgl_button_on_dialog_new (_("Close"), w, reset_cursor_line, NULL, GSGL_TRUE);
	gtk_window_set_transient_for (GTK_WINDOW(w), GTK_WINDOW(gsgl_gtk_inq_appwindow ()));

	gtk_widget_show (table);
	gtk_widget_show (w);

	gsgl_gtk_draw_area (area, color);
#endif
}

void gsgl_gtk_add_line (gpointer gdata, guint action, GtkWidget *widget)
{
        gsglMainWindow *w = gsgl_inq_mainwin ();

	if (w->Add_text)
		gsgl_gui_close_add_text_dialog ();

	gsgl_gui_pointer (w->Page, GSGL_FALSE);
	gui_cursor_pencil (w);
	line_dialog ();
}

void gsgl_add_text_dialog (float x, float y, gsglPage *p)
{
	char *str = gui_popup_edit (NULL, _("Text"));
	if (!str) return;

	gsglMainWindow *w = gsgl_inq_mainwin ();

	if (!w->Add_text_dialog) return;
	
	gsgllist *l = w->Add_text_dialog;
	l = l->next;
	GtkWidget *combo = (GtkWidget *) (l->data);

#if GTK_MAJOR_VERSION == 2 && GTK_MINOR_VERSION >= 4
	int fnt = gtk_combo_box_get_active (GTK_COMBO_BOX(combo));
#else
	const gchar *ent = gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(combo)->entry));
	int fnt;
	for (fnt = 0; fnt < 33; fnt++) {
		if (!strcmp (ent, fonts[fnt]))
			break;
	}
#endif
	if (fnt >= 20)
		fnt += 12;
	
	l = l->next;
	GtkWidget *spin = (GtkWidget *) (l->data);
#if GTK_MAJOR_VERSION == 2
	double size = gtk_spin_button_get_value (GTK_SPIN_BUTTON(spin));
#else
	float size = gtk_spin_button_get_value_as_float (GTK_SPIN_BUTTON(spin));
#endif
	
	l = l->next;
	spin = (GtkWidget *) (l->data);
#if GTK_MAJOR_VERSION == 2
	double angle = gtk_spin_button_get_value (GTK_SPIN_BUTTON(spin));
#else
	float angle = gtk_spin_button_get_value_as_float (GTK_SPIN_BUTTON(spin));
#endif
	l = l->next;
	combo = (GtkWidget *) (l->data);
#if GTK_MAJOR_VERSION == 2 && GTK_MINOR_VERSION >= 4
	int ijust = gtk_combo_box_get_active (GTK_COMBO_BOX(combo));
#else
	ent = gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(combo)->entry));
	int ijust;
	for (ijust = 0; ijust < 4; ijust++) {
		if (!strcmp (ent, just[ijust]))
			break;
	}
#endif

	if (gsgl_inq_i_am_a_server (p->library)) {
		gsgl_tcp_gui_add_text (str, x, y, fnt, (float)size, (float)angle, ijust, p);
	} else {
		gsgl_gui_add_text_client (str, x, y, fnt, (float)size, (float)angle, ijust, p);
	}
}

void gsgl_gui_close_add_text_dialog ()
{
	gsglMainWindow *mw = gsgl_inq_mainwin ();
	if (mw->Add_text && mw->Add_text_dialog) {
		gsgllist *l = mw->Add_text_dialog;
		GtkWidget *w = (GtkWidget *) l->data;
		gtk_widget_destroy (w);
		mw->Add_text = GSGL_FALSE;
		mw->Add_text_dialog = NULL;
	}
}

void gsgl_gui_close_add_line_dialog ()
{
	gsglMainWindow *mw = gsgl_inq_mainwin ();
	if (mw->Add_line && mw->Add_line_dialog) {
		gsgllist *l = mw->Add_line_dialog;
		GtkWidget *w = (GtkWidget *) l->data;
		gtk_widget_destroy (w);
		mw->Add_line = GSGL_FALSE;
		mw->Add_line_dialog = NULL;
	}
}
