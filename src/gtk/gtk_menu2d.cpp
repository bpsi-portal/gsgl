#include "gsaf.h"
#include "gsgl.h"
#include <gtk/gtk.h>
#include "xwin.h"
#include "displaylist.h"
#include "gtk_base.h"
#include "callback.h"
#include "gtk_gsglwindow.h"
#include "gtk_menu2d.h"
#include "gtk_menu3d.h"
#include "gui_common.h"

static GtkWidget      *Spin_value;
static GtkWidget      *Spin_x;
static GtkWidget      *Spin_y;

static void gtk_restore (gsglboolean b)
{
	gtk_widget_set_sensitive (gtk_item_factory_get_widget
				  (gsgl_inq_mainwin ()->GMenu, N_("/Objects/Restore 2D")), b);
	gtk_widget_set_sensitive (gtk_item_factory_get_widget
				  (gsgl_inq_mainwin ()->GPopup, N_("/Restore 2D")), b);
}

void gui_2d_init_every_page ()
{
	gtk_restore (GSGL_FALSE);
}

void gsgl_gtk_2d_restore (gpointer gdata, guint action, GtkWidget *widget)
{
	gsglMainWindow *w = gsgl_inq_mainwin ();
	if (!gsgl_displaylist_inq_remade (w->Page)) return;

	gsgl_2d_restore (w->Page);	
	gtk_restore (GSGL_FALSE);
	gui_cursor_default (w);
}

void gsgl_gtk_2d_trans_mouse (gpointer gdata, guint action, GtkWidget *widget)
{
	gsglMainWindow *w = gsgl_inq_mainwin ();

	gsgl_no_select_objects (w->Page);

	gsgl_2d_trans (w->Page);
	gtk_restore (GSGL_TRUE);
	gui_cursor_translate (w);
}

void gsgl_trans_2d_set_default (GtkWidget *widget, gpointer gdata)
{
	if (Spin_x != NULL)
		gtk_spin_button_set_value (GTK_SPIN_BUTTON(Spin_x), 0.0);
	if (Spin_y != NULL)
		gtk_spin_button_set_value (GTK_SPIN_BUTTON(Spin_y), 0.0);
}

void gsgl_gtk_2d_scale (gpointer gdata, guint action, GtkWidget *widget)
{
        float x, y;
	gsglMainWindow *w = gsgl_inq_mainwin ();

	gsgl_utility_clean (w->Page);
	x = gtk_spin_button_get_value_as_float (GTK_SPIN_BUTTON(Spin_x));
	y = gtk_spin_button_get_value_as_float (GTK_SPIN_BUTTON(Spin_y));
	gsgl_2d_scale (x, y, w->Page);
	gtk_restore (GSGL_TRUE);
}

void gsgl_gtk_2d_scale (GtkWidget *w, gpointer gdata)
{
	gsgl_gtk_2d_scale (gdata, 0, w);
}

void gsgl_scale_2d_set_default (GtkWidget *widget, gpointer gdata)
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

void gsgl_scale_2d_set_default ()
{
	gsgl_scale_2d_set_default (NULL, NULL);
}

void gsgl_gtk_2d_scale_mouse (gpointer gdata, guint with_question, GtkWidget *widget)
{
	GtkWidget *window;
	GtkObject *adjust;
	gsglMainWindow *w = gsgl_inq_mainwin ();	

	gsgl_no_select_objects (w->Page);

	if (with_question) {
		window = gsgl_dialog_new(_("Scale 2D Objects"), 5);
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
		gsgl_button_on_dialog_new (_("OK"), window, gsgl_gtk_2d_scale, NULL, GSGL_TRUE);
		gsgl_button_on_dialog_new_without_close
			(_("Default"), window, gsgl_scale_2d_set_default, NULL, GSGL_FALSE);
		gsgl_button_on_dialog_new (_("Cancel"), window, gsgl_utility_clean,
					   w->Page, GSGL_FALSE);
	
		gtk_window_set_transient_for (GTK_WINDOW(window), GTK_WINDOW(gsgl_gtk_inq_appwindow ()));
		gtk_widget_show (window);
		gtk_grab_add (window);
	} else {
		gsgl_2d_scale (GSGL_PAPER_WIDTH*0.5, GSGL_PAPER_HEIGHT*0.5, w->Page);
		gtk_restore (GSGL_TRUE);
	}

	gui_cursor_scale (w);
}

void gsgl_rotate_2d_set_default (GtkWidget *widget, gpointer gdata)
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

void gsgl_gtk_2d_rotate_mouse (gpointer gdata, guint with_question, GtkWidget *widget)
{
	GtkWidget *window;
	GtkObject *adjust;
	gsglMainWindow *w = gsgl_inq_mainwin ();

	gsgl_no_select_objects (w->Page);
	
	if (with_question) {
		window = gsgl_dialog_new (_("Rotate 2D objects"), 5);
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
		gsgl_button_on_dialog_new (_("OK"), window, gsgl_gtk_2d_rotate, NULL, GSGL_TRUE);
		gsgl_button_on_dialog_new_without_close
			(_("Default"), window, gsgl_rotate_2d_set_default, NULL, GSGL_FALSE);
		gsgl_button_on_dialog_new (_("Cancel"), window, gsgl_utility_clean,
					   w->Page, GSGL_FALSE);
	
		gtk_window_set_transient_for (GTK_WINDOW(window), GTK_WINDOW(gsgl_gtk_inq_appwindow ()));
		gtk_widget_show (window);
		gtk_grab_add (window);
	} else {
		gsgl_2d_rotate (GSGL_PAPER_WIDTH*0.5, GSGL_PAPER_HEIGHT*0.5, w->Page);
		gtk_restore (GSGL_TRUE);
	}

	gui_cursor_rotate (w);
}

void gsgl_gtk_2d_rotate (GtkWidget *widget, gpointer gdata)
{
	float x, y;
	gsglMainWindow *w = gsgl_inq_mainwin ();

	gsgl_utility_clean (w->Page);
	x = gtk_spin_button_get_value_as_float (GTK_SPIN_BUTTON(Spin_x));
	y = gtk_spin_button_get_value_as_float (GTK_SPIN_BUTTON(Spin_y));
	gsgl_2d_rotate (x, y, w->Page);
	gtk_restore (GSGL_TRUE);
}

void gui_2d_sensitive (gboolean torf)
{
}

void gsgl_gtk_2d_select_obj (gpointer gdata, guint aciton, GtkWidget *widget)
{
	gsglMainWindow *w = gsgl_inq_mainwin ();

	gui_2d_sensitive (GSGL_TRUE);
	gui_3d_sensitive (GSGL_FALSE);

	gsgl_yes_select_objects (w->Page);
	gsgl_2d_select_objects (w->Page);
}
