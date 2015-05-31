#include "gsaf.h"
#include <gtk/gtk.h>
#include "xwin.h"
#include "gtk_base.h"
#include "callback.h"
#include "gtk_config.h"
#include "configfile.h"
#include "tcpip.h"
#include "utils.h"
#include "gtk_gsglwindow.h"
#include "fileformat.h"

typedef struct _area_colorsel {
	GtkWidget *area;
	GtkWidget *colorsel;
} area_colorsel;

typedef struct _three_widgets {
	GtkWidget *one;
	GtkWidget *two;
	GtkWidget *three;
} three_widgets;

typedef struct _four_objects {
	GtkObject *one;
	GtkObject *two;
	GtkObject *three;
	GtkObject *four;
} four_objects;

typedef struct _six_widgets {
	GtkWidget *one;
	GtkWidget *two;
	GtkWidget *three;
	GtkWidget *four;
	GtkWidget *five;
	GtkWidget *six;
} six_widgets;

gint Bg;
gsglrgb Boxcolor;
gsglrgb Framecolor;
gsglrgb Boxcolor_back;
gsglrgb Framecolor_back;

/* Callback function when Color Selection Dialog's OK button is pushed */
static void set_box (GtkWidget *widget, gpointer gdata)
{
	gsglMainWindow *w = gsgl_inq_mainwin ();
	gsglConfigColor *gsglcolor = gsgl_inq_config_color (w->Library);

	gsglcolor->box[0]=Boxcolor.r;
	gsglcolor->box[1]=Boxcolor.g;
	gsglcolor->box[2]=Boxcolor.b;
}

void gsgl_gtk_draw_area (GtkWidget *area, double *color)
{
	GdkColor gdk_color;
	GdkColormap *colormap;
	
	colormap = gdk_window_get_colormap (area->window);
	
	/* Fit to a unsigned 16 bit integer (0..65535) and
	 * insert into the GdkColor structure */
	gdk_color.red =   (guint16) (color[0]*65535.0);
	gdk_color.green = (guint16) (color[1]*65535.0);
	gdk_color.blue =  (guint16) (color[2]*65535.0);
	
	/* Allocate color */
	gdk_color_alloc (colormap, &gdk_color);
	
	/* Set window background color */
	gdk_window_set_background (area->window, &gdk_color);
	
	/* Clear window */
	gdk_window_clear (area->window);	
}

static void revert_box (GtkWidget *widget, area_colorsel *ac)
{
	gsglMainWindow *w = gsgl_inq_mainwin ();
	gsglConfigColor * gsglcolor = gsgl_inq_config_color (w->Library);

	gsgl_gtk_draw_area (ac->area, gsglcolor->box);
}

static void color_changed_box (GtkWidget *widget, area_colorsel *ac)
{
	gdouble color[4];
#if GTK_MAJOR_VERSION == 1
	/* Get current color */
	gtk_color_selection_get_color (GTK_COLOR_SELECTION(ac->colorsel), color);

	Boxcolor.r = color[0];
	Boxcolor.g = color[1];
	Boxcolor.b = color[2];
#elif GTK_MAJOR_VERSION == 2
	GdkColor gcolor;

	gtk_color_selection_get_current_color (GTK_COLOR_SELECTION(ac->colorsel), &gcolor);
	Boxcolor.r = gcolor.red/65535.0;
	Boxcolor.g = gcolor.green/65535.0;
	Boxcolor.b = gcolor.blue/65535.0;

	color[0] = Boxcolor.r;
	color[1] = Boxcolor.g;
	color[2] = Boxcolor.b;
#endif
	gsgl_gtk_draw_area (ac->area, color);
}

static void colorsel_set_color (GtkWidget *colorsel, double *color)
{
	gtk_color_selection_set_color (GTK_COLOR_SELECTION(colorsel), color);
}

static void area_event_box (GtkWidget *widget, GdkEvent *event, GtkWidget *area)
{
	GtkWidget *colorsel;
	area_colorsel *ac;
	GtkWidget *colorseldlg;
	gsglMainWindow *w = gsgl_inq_mainwin ();
	gsglConfigColor * gsglcolor = gsgl_inq_config_color (w->Library);
	
	if (event->type != GDK_BUTTON_PRESS) return;

	colorseldlg = gtk_color_selection_dialog_new("Select box color");
	colorsel = GTK_COLOR_SELECTION_DIALOG(colorseldlg)->colorsel;
	
	ac = (area_colorsel *) gsgl_malloc(sizeof(area_colorsel));
	ac->area = area;
	ac->colorsel = colorsel;
	
	Boxcolor.r = gsglcolor->box[0];
	Boxcolor.g = gsglcolor->box[1];
	Boxcolor.b = gsglcolor->box[2];

	colorsel_set_color (colorsel, gsglcolor->box);

	gtk_widget_destroy(GTK_COLOR_SELECTION_DIALOG(colorseldlg)->help_button);
	
	gtk_signal_connect (GTK_OBJECT (GTK_COLOR_SELECTION_DIALOG (colorseldlg)->ok_button), "clicked", GTK_SIGNAL_FUNC (set_box), NULL);
	gtk_signal_connect (GTK_OBJECT (GTK_COLOR_SELECTION_DIALOG (colorseldlg)->ok_button), "clicked", GTK_SIGNAL_FUNC (gsgl_closedialog), colorseldlg);
	
	gtk_signal_connect (GTK_OBJECT (GTK_COLOR_SELECTION_DIALOG (colorseldlg)->cancel_button), "clicked", GTK_SIGNAL_FUNC (revert_box), ac);
	gtk_signal_connect (GTK_OBJECT (GTK_COLOR_SELECTION_DIALOG (colorseldlg)->cancel_button), "clicked", GTK_SIGNAL_FUNC (gsgl_closedialog), colorseldlg);

	gtk_signal_connect(GTK_OBJECT(colorsel), "color_changed", GTK_SIGNAL_FUNC (color_changed_box), (gpointer) ac);
	
	gtk_widget_show(colorseldlg);
	gtk_grab_add (colorseldlg);
}

static void set_frame (GtkWidget *widget, gpointer gdata)
{
	gsglMainWindow *w = gsgl_inq_mainwin ();
	gsglConfigColor * gsglcolor = gsgl_inq_config_color (w->Library);

	gsglcolor->frame[0]=Framecolor.r;
	gsglcolor->frame[1]=Framecolor.g;
	gsglcolor->frame[2]=Framecolor.b;
}

static void revert_frame (GtkWidget *widget, area_colorsel *ac)
{
	gsglMainWindow *w = gsgl_inq_mainwin ();
	gsglConfigColor * gsglcolor = gsgl_inq_config_color (w->Library);

	gsgl_gtk_draw_area (ac->area, gsglcolor->frame);
}

static void color_changed_frame (GtkWidget *widget, area_colorsel *ac)
{
	 gdouble color[4];
#if GTK_MAJOR_VERSION == 1
	 /* Get current color */
	 gtk_color_selection_get_color (GTK_COLOR_SELECTION(ac->colorsel),color);

	 Framecolor.r = color[0];
	 Framecolor.g = color[1];
	 Framecolor.b = color[2];
#elif GTK_MAJOR_VERSION == 2
	 GdkColor gcolor;

	 gtk_color_selection_get_current_color (GTK_COLOR_SELECTION(ac->colorsel), &gcolor);
	 Framecolor.r = gcolor.red/65535.0;
	 Framecolor.g = gcolor.green/65535.0;
	 Framecolor.b = gcolor.blue/65535.0;
	 
	 color[0] = Framecolor.r;
	 color[1] = Framecolor.g;
	 color[2] = Framecolor.b;
#endif
	 gsgl_gtk_draw_area (ac->area, color);
}

static void area_event_frame (GtkWidget *widget,GdkEvent *event, GtkWidget *area)
{
	gint handled = FALSE;
	GtkWidget *colorsel;
	area_colorsel *ac;
	GtkWidget *colorseldlg;
	gsglMainWindow *w = gsgl_inq_mainwin ();
	gsglConfigColor * gsglcolor = gsgl_inq_config_color (w->Library);
	
	if (event->type != GDK_BUTTON_PRESS) return;

	handled = TRUE;
	colorseldlg = gtk_color_selection_dialog_new("Selected object's color");
	colorsel = GTK_COLOR_SELECTION_DIALOG(colorseldlg)->colorsel;
	
	ac = (area_colorsel *) gsgl_malloc(sizeof(area_colorsel));
	ac->area = area;
	ac->colorsel = colorsel;
	
	Framecolor.r = gsglcolor->frame[0];
	Framecolor.g = gsglcolor->frame[1];
	Framecolor.b = gsglcolor->frame[2];

	colorsel_set_color (colorsel, gsglcolor->frame);
	
	gtk_widget_destroy (GTK_COLOR_SELECTION_DIALOG (colorseldlg)->help_button);
	
	gtk_signal_connect (GTK_OBJECT (GTK_COLOR_SELECTION_DIALOG (colorseldlg)->ok_button), "clicked", GTK_SIGNAL_FUNC (set_frame), NULL);
	gtk_signal_connect (GTK_OBJECT (GTK_COLOR_SELECTION_DIALOG (colorseldlg)->ok_button), "clicked", GTK_SIGNAL_FUNC (gsgl_closedialog), colorseldlg);
	
	gtk_signal_connect (GTK_OBJECT (GTK_COLOR_SELECTION_DIALOG (colorseldlg)->cancel_button), "clicked", GTK_SIGNAL_FUNC (revert_frame), ac);
	gtk_signal_connect (GTK_OBJECT (GTK_COLOR_SELECTION_DIALOG (colorseldlg)->cancel_button), "clicked", GTK_SIGNAL_FUNC (gsgl_closedialog), colorseldlg);
	
	gtk_signal_connect(GTK_OBJECT(colorsel), "color_changed",
			   GTK_SIGNAL_FUNC (color_changed_frame),
			   (gpointer)ac);
	
	gtk_widget_show(colorseldlg);
	gtk_grab_add (colorseldlg);
}

static void color_config_ok (GtkWidget *widget, gpointer gdata)
{
	gsglMainWindow *w = gsgl_inq_mainwin ();
	gsglConfigColor * gsglcolor = gsgl_inq_config_color (w->Library);

	gsglcolor->bg = Bg;
}

static void color_config_default (GtkWidget *widget, gpointer gdata)
{
	GdkColor gdk_color;
	GdkColormap *colormap;
	six_widgets *three = (six_widgets *) gdata;

	gtk_list_select_item (GTK_LIST (GTK_COMBO(three->one)->list), 1-DEFAULT_BG);

	colormap = gdk_window_get_colormap (three->two->window);
	gdk_color.red = (guint16) (DEFAULT_BOX_R*65535.0);
	gdk_color.green = (guint16) (DEFAULT_BOX_G*65535.0);
	gdk_color.blue = (guint16) (DEFAULT_BOX_B*65535.0);
	gdk_color_alloc (colormap, &gdk_color);
	gdk_window_set_background (three->two->window, &gdk_color);
	gdk_window_clear (three->two->window);

	colormap = gdk_window_get_colormap (three->three->window);
	gdk_color.red = (guint16) (DEFAULT_FRAME_R*65535.0);
	gdk_color.green = (guint16) (DEFAULT_FRAME_G*65535.0);
	gdk_color.blue = (guint16) (DEFAULT_FRAME_B*65535.0);
	gdk_color_alloc (colormap, &gdk_color);
	gdk_window_set_background (three->three->window, &gdk_color);
	gdk_window_clear (three->three->window);
}

static void color_config_cancel (GtkWidget *widget, gpointer gdata)
{
	gsglMainWindow *w = gsgl_inq_mainwin ();
	gsglConfigColor * gsglcolor = gsgl_inq_config_color (w->Library);

	gsglcolor->box[0] = Boxcolor_back.r;
	gsglcolor->box[1] = Boxcolor_back.g;
	gsglcolor->box[2] = Boxcolor_back.b;

	gsglcolor->frame[0] = Framecolor_back.r;
	gsglcolor->frame[1] = Framecolor_back.g;
	gsglcolor->frame[2] = Framecolor_back.b;
}

static void combofunc (GtkWidget *widget, gpointer gdata)
{
	const gchar *string;

	string = gtk_entry_get_text(GTK_ENTRY(widget));
	if (!strcmp (string, _("black")))
		Bg = GSGL_BACK_COLOR_BLACK;
	else
		Bg = GSGL_BACK_COLOR_WHITE;
}

void gsgl_gtk_config_color (gpointer gdata, guint action, GtkWidget *widget)
{
	GtkWidget *window;
	GtkWidget *combo;
	GtkWidget *hbox;
	GList *items = NULL;
	GtkWidget *area;
	three_widgets *three;
	gsglMainWindow *w = gsgl_inq_mainwin ();
	gsglConfigColor * gsglcolor = gsgl_inq_config_color (w->Library);
	
	if (gsgl_inq_i_am_a_server (w->Library)) {
		gsgl_tcp_server_send_command (GSGL_TCP_GUI_CONFIG_COLOR, w->Library);
		return;
	}
	three = (three_widgets *) gsgl_malloc (sizeof(three_widgets));
	
	window = gsgl_dialog_new (_("Configure color"), 5);
	
	/***********************/
	
	gsgl_gtk_spin_title (window, _("background color:"));
	
	hbox = gtk_hbox_new (FALSE, 0);
	gtk_box_set_spacing (GTK_BOX(GTK_DIALOG(window)->vbox), 5);
	gtk_container_add (GTK_CONTAINER(GTK_DIALOG(window)->vbox), hbox);
	gtk_widget_show (hbox);
	
	combo = gtk_combo_new ();
	items = g_list_append (items, _("white"));
	items = g_list_append (items, _("black"));
	
	three->one = combo;
	Bg = gsglcolor->bg; 
	
	gtk_combo_set_popdown_strings (GTK_COMBO(combo), items);
	gtk_list_select_item (GTK_LIST (GTK_COMBO(combo)->list), 1-Bg);
	gtk_entry_set_editable (GTK_ENTRY (GTK_COMBO (combo)->entry), FALSE);
	gtk_box_pack_end (GTK_BOX (hbox), combo, FALSE, TRUE, 0);
	gtk_signal_connect (GTK_OBJECT (GTK_COMBO (combo)->entry),
			    "changed", GTK_SIGNAL_FUNC (combofunc), NULL);
	gtk_widget_show (combo);
	
	/**********************/
	
	if (gsgl_inq_mode (w->Library) == GSGL_MODE_WINDOW)
		gtk_window_set_transient_for (GTK_WINDOW(window), GTK_WINDOW(gsgl_gtk_inq_appwindow ()));
	gtk_widget_show (window);
	
	/**********************/
	
	gsgl_gtk_spin_title (window, N_("selecting box color:"));
	
	hbox = gtk_hbox_new (FALSE, 0);
	gtk_container_add (GTK_CONTAINER(GTK_DIALOG(window)->vbox), hbox);
	
	area = gtk_drawing_area_new ();
	gtk_drawing_area_size (GTK_DRAWING_AREA(area), 60, 20);
	gtk_widget_set_events (area, GDK_BUTTON_PRESS_MASK);
	
	three->two = area;
	
	gtk_signal_connect (GTK_OBJECT(area), "event", 
			    GTK_SIGNAL_FUNC (area_event_box), (gpointer) area);
	
	gtk_box_pack_end (GTK_BOX (hbox), area, FALSE, FALSE, 10);
	gtk_widget_show (area);
	gtk_widget_show (hbox);
	
	Boxcolor_back.r = gsglcolor->box[0];
	Boxcolor_back.g = gsglcolor->box[1];
	Boxcolor_back.b = gsglcolor->box[2];
	
	gsgl_gtk_draw_area (area, gsglcolor->box);
	
	/**********************/
	
	gsgl_gtk_spin_title (window, N_("selected object's color:"));
	
	hbox = gtk_hbox_new (FALSE, 0);
	gtk_container_add (GTK_CONTAINER(GTK_DIALOG(window)->vbox), hbox);
	
	area = gtk_drawing_area_new ();
	
	three->three = area;
	
	gtk_drawing_area_size (GTK_DRAWING_AREA(area), 60, 20);
	gtk_widget_set_events (area, GDK_BUTTON_PRESS_MASK);
	
	gtk_signal_connect (GTK_OBJECT(area), "event",
			    GTK_SIGNAL_FUNC (area_event_frame), (gpointer)area);
	
	gtk_box_pack_end (GTK_BOX (hbox), area, FALSE, FALSE, 10);
	gtk_widget_show (area);
	gtk_widget_show (hbox);	
	
	Framecolor_back.r = gsglcolor->frame[0];
	Framecolor_back.g = gsglcolor->frame[1];
	Framecolor_back.b = gsglcolor->frame[2];
		
	gsgl_gtk_draw_area (area, gsglcolor->frame);
	
	/**********************/
	
	gsgl_button_on_dialog_new (_("OK"), window, color_config_ok, NULL, TRUE);
	gsgl_button_on_dialog_new_without_close (_("Default"), window, color_config_default, three, FALSE);
	gsgl_button_on_dialog_new (_("Cancel"), window, color_config_cancel, NULL, FALSE);
}

/****************************************************************************/

const gchar *Tmp_print = NULL;
const gchar *Tmp_man = NULL;
const gchar *Tmp_eps_vec = NULL;
const gchar *Tmp_eps_ras = NULL;
const gchar *Tmp_jpg = NULL;
const gchar *Tmp_png = NULL;

static void enter_callback (GtkWidget *entry, const char *type)
{
	if (type == "print") {
		Tmp_print = gtk_entry_get_text(GTK_ENTRY(entry));
	} else if (type == "man") {
		Tmp_man = gtk_entry_get_text(GTK_ENTRY(entry));
	} else if (type == "eps_vec") {
		Tmp_eps_vec = gtk_entry_get_text(GTK_ENTRY(entry));
	} else if (type == "eps_ras") {
		Tmp_eps_ras = gtk_entry_get_text(GTK_ENTRY(entry));
	} else if (type == "png") {
		Tmp_png = gtk_entry_get_text(GTK_ENTRY(entry));
	} else if (type == "jpg") {
		Tmp_jpg = gtk_entry_get_text(GTK_ENTRY(entry));
	}
}

static GtkWidget * command_entry (GtkWidget *window, char *text, char *command, char *type)
{
	GtkWidget *hbox;
	GtkWidget *entry;
	
	gsgl_gtk_spin_title (window, text);

	hbox = gtk_hbox_new (FALSE, 0);
	gtk_box_set_spacing (GTK_BOX(GTK_DIALOG(window)->vbox), 5);
	gtk_container_add (GTK_CONTAINER(GTK_DIALOG(window)->vbox), hbox);
	
	entry = gtk_entry_new ();
	gtk_entry_set_text (GTK_ENTRY (entry), command);
	gtk_box_pack_end (GTK_BOX (hbox), entry, TRUE, TRUE, 10);
	gtk_signal_connect (GTK_OBJECT (entry), "changed", GTK_SIGNAL_FUNC (enter_callback), type);
	gtk_widget_show (entry);
	gtk_widget_show (hbox);

	return entry;
}

static void command_set_default (GtkWidget *widget, gpointer gdata)
{
	six_widgets *six = (six_widgets *) gdata;

	gtk_entry_set_text (GTK_ENTRY (six->one), COMMAND_PRINT); 
	gtk_entry_set_text (GTK_ENTRY (six->two), COMMAND_MAN); 
	gtk_entry_set_text (GTK_ENTRY (six->three), COMMAND_EPS_VEC); 
	gtk_entry_set_text (GTK_ENTRY (six->four), COMMAND_EPS_RAS); 
	gtk_entry_set_text (GTK_ENTRY (six->five), COMMAND_JPG); 
	gtk_entry_set_text (GTK_ENTRY (six->six), COMMAND_PNG); 
}

static void command_ok (GtkWidget *widget, gpointer data)
{
	gsglMainWindow *w = gsgl_inq_mainwin ();

	if (Tmp_print != NULL) {
		gsgl_set_print_command (Tmp_print, w->Library);
		Tmp_print = NULL;
	}
	if (Tmp_man != NULL) {
		gsgl_set_man_command (Tmp_man, w->Library);
		Tmp_man = NULL;
	}
	if (Tmp_eps_vec != NULL) {
		gsgl_set_eps_vec_command (Tmp_eps_vec, w->Library);
		Tmp_eps_vec = NULL;
	}
	if (Tmp_eps_ras != NULL) {
		gsgl_set_eps_ras_command (Tmp_eps_ras, w->Library);
		Tmp_eps_ras = NULL;
	}
	if (Tmp_jpg != NULL) {
		gsgl_set_jpg_command (Tmp_jpg, w->Library);
		Tmp_jpg = NULL;
	}
	if (Tmp_png != NULL) {
		gsgl_set_png_command (Tmp_png, w->Library);
		Tmp_png = NULL;
	}
}

void gsgl_gtk_config_command (gpointer gdata, guint action, GtkWidget *widget)
{
	GtkWidget *window;
	six_widgets *six;
	gsglMainWindow *w = gsgl_inq_mainwin ();

	if (gsgl_inq_i_am_a_server (w->Library)) {
		gsgl_tcp_server_send_command (GSGL_TCP_GUI_CONFIG_COMMAND, w->Library);
		return;
	}
	six = (six_widgets *) gsgl_malloc (sizeof(six_widgets));
	
	window = gsgl_dialog_new (_("Configure command line"), 5);
	gtk_window_set_default_size (GTK_WINDOW (window), 450, 0);
	
	/***********************/
	
	six->one = command_entry (window, _("Print:"), gsgl_inq_print_command (w->Library), "print");
	six->two = command_entry (window, _("Browse manuals:"), gsgl_inq_man_command (w->Library), "man");
	six->three = command_entry (window, _("Export vector eps:"), gsgl_inq_eps_vec_command (w->Library), "eps_vec");
	six->four = command_entry (window, _("Export raster eps:"), gsgl_inq_eps_ras_command (w->Library), "eps_ras");
	six->five = command_entry (window, _("Export jpeg:"), gsgl_inq_jpg_command (w->Library), "jpg");
	six->six = command_entry (window, _("Export png:"), gsgl_inq_png_command (w->Library), "png");
	
	/***********************/
	
	gsgl_button_on_dialog_new (_("OK"), window, command_ok, NULL, TRUE);
	gsgl_button_on_dialog_new_without_close (_("Default"), window, command_set_default, six, FALSE);
	gsgl_button_on_dialog_new (_("Cancel"), window, 0, NULL, FALSE);
	if (gsgl_inq_mode (w->Library) == GSGL_MODE_WINDOW)
		gtk_window_set_transient_for (GTK_WINDOW(window), GTK_WINDOW(gsgl_gtk_inq_appwindow ()));
	gtk_widget_show (window);
}

static GtkWidget      *Spin_width;
static GtkWidget      *Spin_height;
static GtkWidget      *Spin_corner_x;
static GtkWidget      *Spin_corner_y;
static int            Tmp_window_width;
static int            Tmp_window_height;
static int            Tmp_corner_x;
static int            Tmp_corner_y;
static four_objects   Adjust;

static void window_ok (GtkWidget *widget, gpointer data)
{
	gsglMainWindow *w = gsgl_inq_mainwin ();

	gsgl_set_userdefined_winwidth (Tmp_window_width, w->Library);
	gsgl_set_userdefined_winheight (Tmp_window_height, w->Library);
	gsgl_set_userdefined_corner_x (Tmp_corner_x, w->Library);
	gsgl_set_userdefined_corner_y (Tmp_corner_y, w->Library);
}

static void window_set_default (GtkWidget *widget, gpointer data)
{
	Tmp_window_width = GSGLBASIC_WINDOWWIDTH;
	Tmp_window_height = GSGLBASIC_WINDOWHEIGHT;
	Tmp_corner_x = 0;
	Tmp_corner_y = 0;
	
	gtk_adjustment_set_value (GTK_ADJUSTMENT (Adjust.one), (float) Tmp_window_width);
	gtk_adjustment_set_value (GTK_ADJUSTMENT (Adjust.two), (float) Tmp_window_height);
	gtk_adjustment_set_value (GTK_ADJUSTMENT (Adjust.three), (float) Tmp_corner_x);
	gtk_adjustment_set_value (GTK_ADJUSTMENT (Adjust.four), (float) Tmp_corner_y);
}

static void spin_window_width (GtkAdjustment *adjustment, GtkWidget *spin)
{
	int w;
	
	w = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON(spin));
	Tmp_window_width = w;
}

static void spin_window_height (GtkAdjustment *adjustment, GtkWidget *spin)
{
	int w;
	
	w = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON(spin));
	Tmp_window_height = w;
}

static void spin_window_x (GtkAdjustment *adjustment, GtkWidget *spin)
{
	int w;
	
	w = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON(spin));
	Tmp_corner_x = w;
}

static void spin_window_y (GtkAdjustment *adjustment, GtkWidget *spin)
{
	int w;
	
	w = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON(spin));
	Tmp_corner_y = w;
}

void gsgl_gtk_config_window (gpointer gdata, guint action, GtkWidget *widget)
{
	GtkWidget *window;
	GtkObject *adjust;
	gsglMainWindow *w = gsgl_inq_mainwin ();

	if (gsgl_inq_i_am_a_server (w->Library)) {
		gsgl_tcp_server_send_command (GSGL_TCP_GUI_CONFIG_WINDOW, w->Library);
		return;
	}

	Tmp_window_width = gsgl_inq_userdefined_winwidth (w->Library);
	Tmp_window_height = gsgl_inq_userdefined_winheight (w->Library);
	Tmp_corner_x = gsgl_inq_userdefined_corner_x (w->Library);
	Tmp_corner_y = gsgl_inq_userdefined_corner_y (w->Library);
	
	window = gsgl_dialog_new (_("Configure window size"), 5);
	gtk_window_set_default_size (GTK_WINDOW (window), 0, 0);
	
	/*---------------*/
	
	adjust = gtk_adjustment_new (gsgl_inq_userdefined_winwidth (w->Library), 0.0,
				     gdk_screen_width (), 1.0, 10.0, 10.0);
	Adjust.one = adjust;
	Spin_width = gsgl_gtk_spin (window, adjust, _("window width : "), _("pixel"), 1.0, 0);
	gtk_adjustment_value_changed (GTK_ADJUSTMENT(adjust));
	gtk_signal_connect (GTK_OBJECT (adjust), "value-changed",
			    GTK_SIGNAL_FUNC (spin_window_width), Spin_width);
	
	/*---------------*/
	
	adjust = gtk_adjustment_new (gsgl_inq_userdefined_winheight (w->Library), 0.0,
				     gdk_screen_height (), 1.0, 10.0, 10.0);
	Adjust.two = adjust;
	Spin_height = gsgl_gtk_spin (window, adjust, _("window height : "), _("pixel"), 1.0, 0);
	gtk_adjustment_value_changed (GTK_ADJUSTMENT(adjust));
	gtk_signal_connect (GTK_OBJECT (adjust), "value-changed",
			    GTK_SIGNAL_FUNC (spin_window_height), Spin_height);
	
	/*---------------*/
		
	adjust = gtk_adjustment_new (gsgl_inq_userdefined_corner_x (w->Library), 0.0,
				     gdk_screen_width (), 1.0, 10.0, 10.0);
	Adjust.three = adjust;
	Spin_corner_x=gsgl_gtk_spin (window, adjust, _("left-upper corner (x) : "), _("pixel"), 1.0, 0);
	gtk_adjustment_value_changed (GTK_ADJUSTMENT(adjust));
	gtk_signal_connect (GTK_OBJECT (adjust), "value-changed",
			    GTK_SIGNAL_FUNC (spin_window_x), Spin_corner_x);
	
	/*---------------*/
		
	adjust = gtk_adjustment_new (gsgl_inq_userdefined_corner_y (w->Library), 0.0,
				     gdk_screen_height (), 1.0, 10.0, 10.0);
	Adjust.four = adjust;
	Spin_corner_y=gsgl_gtk_spin (window, adjust, _("left-upper corner (y) : "), _("pixel"), 1.0, 0);
	gtk_adjustment_value_changed (GTK_ADJUSTMENT(adjust));
	gtk_signal_connect (GTK_OBJECT (adjust), "value-changed",
			    GTK_SIGNAL_FUNC (spin_window_y), Spin_corner_y);
	
	/*---------------*/

	if (gsgl_inq_mode (w->Library) == GSGL_MODE_WINDOW)
		gtk_window_set_transient_for (GTK_WINDOW(window), GTK_WINDOW(gsgl_gtk_inq_appwindow ()));
	
	gsgl_button_on_dialog_new (_("OK"), window, window_ok, NULL, TRUE);
	gsgl_button_on_dialog_new_without_close (_("Default"), window, window_set_default, NULL, FALSE);
	gsgl_button_on_dialog_new (_("Cancel"), window, NULL, NULL, FALSE);
	gtk_widget_show (window);
}	

const gchar *Tmp_ttf[28] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
			    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 
			    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};

static void ttf_callback (GtkWidget *entry, int *type)
{
	if (0 <= *type && *type <= 27)
		Tmp_ttf [*type] = gtk_entry_get_text(GTK_ENTRY(entry));
}

static void ttf_entry (GtkWidget *table, char *text, int number, char *path, int num)
{
	GtkWidget *entry;
	GtkWidget *label;
	char str[6];
	int *ptype;
	
	ptype = (int *) gsgl_malloc_atomic (sizeof(int));
	ptype[0] = number-1000;

	label = gtk_label_new (text);
	gtk_widget_show (label);
	gtk_table_attach (GTK_TABLE (table), 
			  label,
			  0, 1,
			  num, num+1,
			  GTK_EXPAND, GTK_SHRINK,
			  5, 2);

	sprintf (str, "(%d)", number);
	label = gtk_label_new (str);
	gtk_widget_show (label);
	gtk_table_attach (GTK_TABLE (table), 
			  label,
			  1, 2,
			  num, num+1,
			  GTK_EXPAND, GTK_SHRINK,
			  5, 2);
	
	entry = gtk_entry_new ();
	gtk_entry_set_text (GTK_ENTRY (entry), path);
	gtk_widget_set_usize (GTK_WIDGET (entry), 350, 0);
	gtk_table_attach (GTK_TABLE (table), 
			  entry,
			  2, 3,
			  num, num+1,
			  GTK_EXPAND, GTK_SHRINK,
			  5, 2);
	gtk_signal_connect (GTK_OBJECT (GTK_ENTRY (entry)), "changed", 
			    GTK_SIGNAL_FUNC (ttf_callback), ptype);
	gtk_widget_show (entry);
}

static void ttf_ok (GtkWidget *widget, gpointer data)
{
	int i;
	gsglMainWindow *w = gsgl_inq_mainwin ();

	for (i = 0; i < 28; i++) {
		if (Tmp_ttf [i] != NULL) {
			gsgl_set_ttfname (i, Tmp_ttf [i], w->Library);
			Tmp_ttf [i] = NULL;
		}
	}
}

void gsgl_gtk_config_ttf (gpointer gdata, guint action, GtkWidget *widget)
{
	int i, j;
	GtkWidget *window;
	GtkWidget *note;
	GtkWidget *table = NULL;
	GtkWidget *label;
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
	char *title[] = {
		_("Alphabet"),
		_("Greek"),
		_("Japanese"),
		_("Bakoma"),
		_("Type1")
	};
	gsglMainWindow *w = gsgl_inq_mainwin ();

	if (gsgl_inq_i_am_a_server (w->Library)) {
		gsgl_tcp_server_send_command (GSGL_TCP_GUI_CONFIG_FONT, w->Library);
		return;
	}

	window = gsgl_dialog_new (_("Configure truetype fonts path"), 5);
	gtk_window_set_default_size (GTK_WINDOW (window), 500, 0);

	note = gtk_notebook_new ();

	gtk_box_pack_end (GTK_BOX(GTK_DIALOG(window)->vbox), note, FALSE, FALSE, 10);

	gtk_widget_show (note);
	for (i=0; i<5; i++) {
		if (i==0) { /* alphabet */
			table = gtk_table_new (14, 3, GSGL_FALSE);
			gtk_widget_show (table);
			for (j=0; j<14; j++) 
				ttf_entry (table, fontname[j], fontnum[j], 
					   gsgl_inq_ttfname (j, w->Library), j);
		} else if (i==1) { /* greek */
			table = gtk_table_new (5, 3, GSGL_FALSE);
			gtk_widget_show (table);
			for (j=0; j<5; j++) 
				ttf_entry (table, fontname[j+14], fontnum[j+14], 
					   gsgl_inq_ttfname (j+14, w->Library), j);
		} else if (i==2) { /* japanese */
			table = gtk_table_new (3, 3, GSGL_FALSE);
			gtk_widget_show (table);
			for (j=0; j<3; j++) 
				ttf_entry (table, fontname[j+19], fontnum[j+19], 
					   gsgl_inq_ttfname (j+19, w->Library), j);
		} else if (i==3) { /* bakoma */
			table = gtk_table_new (6, 3, GSGL_FALSE);
			gtk_widget_show (table);
			for (j=0; j<6; j++) 
				ttf_entry (table, fontname[j+22], fontnum[j+22], 
					   gsgl_inq_ttfname (j+22, w->Library), j);
		} else if (i==4) { /* Type1 */
			table = gtk_table_new (13, 3, GSGL_FALSE);
			gtk_widget_show (table);
			for (j=0; j<13; j++) 
				ttf_entry (table, fontname[j+28], fontnum[j+28], 
					   gsgl_inq_ttfname (j+28, w->Library), j);
		}
		label = gtk_label_new (title[i]);
		gtk_widget_show (label);
		gtk_notebook_append_page (GTK_NOTEBOOK (note), table, label);
	}
	gsgl_button_on_dialog_new (_("OK"), window, ttf_ok, NULL, TRUE);
	gsgl_button_on_dialog_new (_("Cancel"), window, NULL, NULL, FALSE);

	if (gsgl_inq_mode (w->Library) == GSGL_MODE_WINDOW)
		gtk_window_set_transient_for (GTK_WINDOW(window), GTK_WINDOW(gsgl_gtk_inq_appwindow ()));
	gtk_widget_show (window);
}
