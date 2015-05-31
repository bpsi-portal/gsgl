#include "gsaf.h"
#include "gsgl.h"
#include <gtk/gtk.h>
#include "xwin.h"
#include "gtk_base.h"
#include "callback.h"
#include "gtk_menupro.h"
#include "gtk_gsglwindow.h"
#include "displaylist.h"
#include "gui_common.h"

static GtkWidget      *Spin_value;
static GtkWidget      *Spin_x;
static GtkWidget      *Spin_y;

static void gtk_restore (gsglboolean b)
{
	gsglMainWindow *w = gsgl_inq_mainwin ();

	gtk_widget_set_sensitive (gtk_item_factory_get_widget
				  (w->GMenu, N_("/Projected Image/Restore")), b);
	gtk_widget_set_sensitive (gtk_item_factory_get_widget
				  (w->GPopup, N_("/Restore Projected Image")), b);
}

void gui_projection_init_every_page ()
{
	gtk_restore (GSGL_FALSE);
}

void gsgl_gtk_proj_restore (gpointer gdata, guint action, GtkWidget *widget)
{
	gsglMainWindow *w = gsgl_inq_mainwin ();

	gsgl_proj_restore (w->Page);
	gtk_restore (GSGL_FALSE);
	gui_cursor_default (w);
}

void gsgl_gtk_proj_trans_mouse (gpointer gdata, guint action, GtkWidget *widget)
{
	gsglMainWindow *w = gsgl_inq_mainwin ();

	gsgl_no_select_objects (w->Page);

	gsgl_displaylist_remake (w->Page);
	gsgl_proj_trans (w->Page);
	gtk_restore (GSGL_TRUE);
	gui_cursor_translate (w);
}

void gsgl_trans_proj_set_default (GtkWidget *widget, gpointer gdata)
{
	if (Spin_x != NULL)
		gtk_spin_button_set_value (GTK_SPIN_BUTTON(Spin_x), 0.0);
	if (Spin_y != NULL)
		gtk_spin_button_set_value (GTK_SPIN_BUTTON(Spin_y), 0.0);
}

void gsgl_gtk_proj_scale (gpointer gdata, guint action, GtkWidget *widget)
{
        float x, y;
	gsglMainWindow *w = gsgl_inq_mainwin ();

	gsgl_utility_clean (w->Page);
	x = gtk_spin_button_get_value_as_float (GTK_SPIN_BUTTON(Spin_x));
	y = gtk_spin_button_get_value_as_float (GTK_SPIN_BUTTON(Spin_y));
	gsgl_proj_scale (x, y, w->Page);
	gtk_restore (GSGL_TRUE);
}

static void gsgl_gtk_proj_scale (GtkWidget *widget, gpointer gdata)
{
	gsgl_gtk_proj_scale (gdata, 0, widget);
}

void gsgl_scale_proj_set_default (GtkWidget *widget, gpointer gdata)
{
	if (Spin_value != NULL)
		gtk_spin_button_set_value (GTK_SPIN_BUTTON(Spin_value), 100.0);
	if (Spin_x != NULL)
		gtk_spin_button_set_value (GTK_SPIN_BUTTON(Spin_x),
					   GSGL_PAPER_WIDTH*0.5);
	if (Spin_y != NULL)
		gtk_spin_button_set_value (GTK_SPIN_BUTTON(Spin_y),
					   GSGL_PAPER_HEIGHT*0.5);
}

void gsgl_gtk_proj_scale_mouse (gpointer gdata, guint with_question, GtkWidget *widget)
{
	GtkWidget *window;
	GtkObject *adjust;
	gsglMainWindow *w = gsgl_inq_mainwin ();

	gsgl_no_select_objects (w->Page);
	gsgl_displaylist_remake (w->Page);
	
	if (with_question) {
		window = gsgl_dialog_new(_("Scale Projected Image"), 5);
		Spin_value = NULL;
		
		gsgl_gtk_spin_title (window, _("Center of Scaling : "));
		
		/*---------------*/
		adjust = gtk_adjustment_new (GSGL_PAPER_WIDTH*0.5, 0.0,
					     (float) GSGL_PAPER_WIDTH, 0.1,1.0,1.0);
		Spin_x = gsgl_gtk_spin (window, adjust, "x : ", "cm", 0.1, 1);
		gtk_adjustment_value_changed (GTK_ADJUSTMENT(adjust));
		gtk_signal_connect (GTK_OBJECT (adjust), "value-changed",
				    GTK_SIGNAL_FUNC (gsgl_spin_value_x), Spin_x);
		
		/*---------------*/
		adjust = gtk_adjustment_new (GSGL_PAPER_HEIGHT*0.5, 0.0,
					     (float) GSGL_PAPER_HEIGHT, 0.1,1.0,1.0);
		Spin_y = gsgl_gtk_spin (window, adjust, "y : ", "cm", 0.1, 1);
		gtk_adjustment_value_changed (GTK_ADJUSTMENT(adjust));
		gtk_signal_connect (GTK_OBJECT (adjust), "value-changed",
				    GTK_SIGNAL_FUNC (gsgl_spin_value_y), Spin_y);
		
		/*---------------*/
		gsgl_utility_cross (GSGL_PAPER_WIDTH*0.5, -1, -1, w->Page);
		gsgl_utility_cross (-1, GSGL_PAPER_HEIGHT*0.5, -1, w->Page);
		gsgl_utility_cross (-1, -1, 0, w->Page);
		/*---------------*/
		gsgl_button_on_dialog_new (_("OK"), window, gsgl_gtk_proj_scale, NULL, GSGL_TRUE);
		gsgl_button_on_dialog_new_without_close
			(_("Default"), window, gsgl_scale_proj_set_default, NULL, GSGL_FALSE);
		gsgl_button_on_dialog_new (_("Cancel"), window, gsgl_utility_clean,
					   NULL, GSGL_FALSE);

		gtk_window_set_transient_for (GTK_WINDOW(window), GTK_WINDOW(gsgl_gtk_inq_appwindow ()));
		gtk_widget_show (window);
		gtk_grab_add (window);
	} else {
		gsgl_proj_scale (GSGL_PAPER_WIDTH*0.5, GSGL_PAPER_HEIGHT*0.5, w->Page);
		gtk_restore (GSGL_TRUE);
	}

	gui_cursor_scale (w);
}

void gsgl_rotate_proj_set_default (GtkWidget *widget, gpointer gdata)
{
	if (Spin_value != NULL)
		gtk_spin_button_set_value (GTK_SPIN_BUTTON(Spin_value), 90.0);
	if (Spin_x != NULL)
		gtk_spin_button_set_value (GTK_SPIN_BUTTON(Spin_x),
					   GSGL_PAPER_WIDTH*0.5);
	if (Spin_y != NULL)
		gtk_spin_button_set_value (GTK_SPIN_BUTTON(Spin_y),
					   GSGL_PAPER_HEIGHT*0.5);
}

void gsgl_gtk_proj_rotate_mouse (gpointer gdata, guint with_question, GtkWidget *widget)
{
	GtkWidget *window;
	GtkObject *adjust;
	gsglMainWindow *w = gsgl_inq_mainwin ();
	
	gsgl_no_select_objects (w->Page);
	gsgl_displaylist_remake (w->Page);

	if (with_question) {
		window = gsgl_dialog_new (_("Rotate Projected Image"), 5);
		Spin_value = NULL;
		
		gsgl_gtk_spin_title (window, _("Center of Rotation : "));
		
		/*---------------*/
		adjust = gtk_adjustment_new (GSGL_PAPER_WIDTH*0.5, 0.0,
					     (float) GSGL_PAPER_WIDTH, 0.1,1.0,1.0);
		Spin_x = gsgl_gtk_spin (window, adjust, "x : ", "cm", 0.1, 1);
		gtk_adjustment_value_changed (GTK_ADJUSTMENT(adjust));
		gtk_signal_connect (GTK_OBJECT (adjust), "value-changed",
				    GTK_SIGNAL_FUNC (gsgl_spin_value_x), Spin_x);
		
		/*---------------*/
		adjust = gtk_adjustment_new (GSGL_PAPER_HEIGHT*0.5, 0.0,
					     (float) GSGL_PAPER_HEIGHT,0.1,1.0,1.0);
		Spin_y = gsgl_gtk_spin (window, adjust, "y : ", "cm", 0.1, 1);
		gtk_adjustment_value_changed (GTK_ADJUSTMENT(adjust));
		gtk_signal_connect (GTK_OBJECT (adjust), "value-changed",
				    GTK_SIGNAL_FUNC (gsgl_spin_value_y), Spin_y);
		
		/*---------------*/
		gsgl_utility_cross (GSGL_PAPER_WIDTH*0.5, -1, -1, w->Page);
		gsgl_utility_cross (-1, GSGL_PAPER_HEIGHT*0.5, -1, w->Page);
		gsgl_utility_cross (-1, -1, 0.0, w->Page);
		/*---------------*/
		gsgl_button_on_dialog_new (_("OK"), window,gsgl_gtk_proj_rotate, NULL, GSGL_TRUE);
		gsgl_button_on_dialog_new_without_close
			(_("Default"), window, gsgl_rotate_proj_set_default, NULL, GSGL_FALSE);
		gsgl_button_on_dialog_new (_("Cancel"), window, gsgl_utility_clean,
					   NULL, GSGL_FALSE);
		
		gtk_window_set_transient_for (GTK_WINDOW(window), GTK_WINDOW(gsgl_gtk_inq_appwindow ()));
		gtk_widget_show (window);
		gtk_grab_add (window);
	} else {
		gsgl_proj_rotate (GSGL_PAPER_WIDTH*0.5, GSGL_PAPER_HEIGHT*0.5, w->Page);
		gtk_restore (GSGL_TRUE);
	}

	gui_cursor_rotate (w);
}

void gsgl_gtk_proj_rotate (GtkWidget *widget, gpointer gdata)
{
	float x, y;
	gsglMainWindow *w = gsgl_inq_mainwin ();

	gsgl_utility_clean (w->Page);
	x = gtk_spin_button_get_value_as_float (GTK_SPIN_BUTTON(Spin_x));
	y = gtk_spin_button_get_value_as_float (GTK_SPIN_BUTTON(Spin_y));
	gsgl_proj_rotate (x, y, w->Page);
	gtk_restore (GSGL_TRUE);
}

void gsgl_gtk_sensitive_proj (gboolean torf)
{
	gsglMainWindow *w = gsgl_inq_mainwin ();

	gtk_widget_set_sensitive(gtk_item_factory_get_widget
				 (w->GMenu, N_("/Projectied Image/Rotate")), torf);
	gtk_widget_set_sensitive(gtk_item_factory_get_widget
				 (w->GMenu, N_("/Projectied Image/Scale")), torf);
	gtk_widget_set_sensitive(gtk_item_factory_get_widget
				 (w->GMenu, N_("/Projectied Image/Translate")), torf);
	gtk_widget_set_sensitive(gtk_item_factory_get_widget
				 (w->GPopup, N_("/Rotate Projected Image")), torf);
	gtk_widget_set_sensitive(gtk_item_factory_get_widget
				 (w->GPopup, N_("/Scale Projected Image")), torf);
	gtk_widget_set_sensitive(gtk_item_factory_get_widget
				 (w->GPopup, N_("/Translate Projected Image")), torf);
}
