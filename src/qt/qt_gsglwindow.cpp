#include "qt_compat.h"
#include <QFile>
#include <QFileDialog>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QApplication>
#include <QGuiApplication>
#include <QKeySequence>
#include <QMessageBox>
#include <QCursor>
#include <QInputDialog>
#include <QLabel>
#include <QContextMenuEvent>
#include <QCloseEvent>
#include <QScreen>
#include <QElapsedTimer>
#include <QVBoxLayout>
#include "qt_gsglwindow.h"
#include "qt_drawarea.h"
#include "qt_dialog.h"
#include "gsaf.h"
#include "xwin.h"
#include "base.h"
#include "bsc1.h"
#include "anime.h"
#include "callback.h"
#include "configfile.h"
#include "device.h"
#include "fileformat.h"
#include "fileio_base.h"
#include "fileio_write.h"
#include "ogl.h"
#include "tcpip.h"
#include "utils.h"
#include "gui_common.h"
#include "qt_xwin.h"
#include <iostream>

#define TR(x) QString::fromUtf8(_(x))

gsglMainWindow::gsglMainWindow (gsglWindow *window, QWidget* parent, WFLAGS fl)
	: QMainWindow (parent, fl)
{
	save_this_frame = 0;
	Win = window;
	Library = window->library;
	Add_text = GSGL_FALSE;
        Add_text_dialog = NULL;

	setWindowTitle (gsgl_window_title());

	if (gsgl_inq_userdefined_window (Library)) {
		setGeometry (gsgl_inq_userdefined_corner_x (Library), 
			     gsgl_inq_userdefined_corner_y (Library), 0, 0);
	} else {
		QScreen *screen = QGuiApplication::primaryScreen();
		QRect screenRect = screen ? screen->availableGeometry() : QRect(0, 0, Library->winwidth0, Library->winheight0);
		int width = screenRect.width() - Library->winwidth0;
		int height = screenRect.height() - Library->winheight0;
		setGeometry (screenRect.x() + width/2, screenRect.y() + height/2, 0, 0);
	}

	vbox = new QWidget (this);
	QVBoxLayout *vboxLayout = new QVBoxLayout (vbox);
	vboxLayout->setContentsMargins (0, 0, 0, 0);
	setCentralWidget (vbox);

	gl = new GLDrawArea (Win, vbox);
	vboxLayout->addWidget (gl);

	menubar = new QMenuBar (this);
	setMenuBar (menubar);

	exportMenu = new QMenu (this);
	exportMenu->setTearOffEnabled (true);
	exportMenu->addAction (TR("EPS (Vector)"), this, SLOT(export_eps_vec()));
	exportMenu->addAction (TR("EPS (Raster)"), this, SLOT(export_eps_ras()));
	exportMenu->addAction (TR("PNG"), this, SLOT(export_png()));
	exportMenu->addAction (TR("JPEG"), this, SLOT(export_jpeg()));

	fileMenu = new QMenu (TR("&File"), this);
	fileMenu->setTearOffEnabled (true);
	id_file[0] = fileMenu->addAction (TR("Save As ..."), this, SLOT(save_as()));
	id_file[0]->setShortcut (QKeySequence(Qt::Key_A));
	id_file[1] = fileMenu->addAction (TR("Save"), this, SLOT(save()));
	id_file[1]->setShortcut (QKeySequence(Qt::Key_S));
	id_file[2] = fileMenu->addAction (TR("Save and Close"), this, SLOT(save_close()));
	id_file[2]->setShortcut (QKeySequence(Qt::Key_Y));
	id_file[3] = fileMenu->addAction (TR("Save This Frame"), this, SLOT(save_frame()));
	id_file[4] = fileMenu->addMenu (exportMenu);
	fileMenu->addSeparator ();
	id_file[5] = fileMenu->addAction (TR("Print ..."), this, SLOT(print()));
	id_file[5]->setShortcut (QKeySequence(Qt::Key_P));
	fileMenu->addSeparator ();
	id_file[6] = fileMenu->addAction (TR("Dump Dot Image"), this, SLOT(dump()));
	id_file[6]->setShortcut (QKeySequence(Qt::Key_D));
	fileMenu->addSeparator ();
	id_file[7] = fileMenu->addAction (TR("Close"), qApp, SLOT(quit()));
	id_file[7]->setShortcut (QKeySequence(Qt::Key_C));
	id_file[8] = fileMenu->addAction (TR("Quit"), this, SLOT(quit_app()));
	id_file[8]->setShortcut (QKeySequence(Qt::Key_Q));

	editMenu = new QMenu (TR("&Edit"), this);
	editMenu->setTearOffEnabled (true);
	id_edit[0] = editMenu->addAction (TR("Pointer"), this, SLOT(select_obj()));
	id_edit[0]->setShortcut (QKeySequence(Qt::Key_Escape));
	id_edit[1] = editMenu->addAction (TR("Select All 2D Objects"), this, SLOT(select_all_2d_obj()));
	id_edit[1]->setShortcut (QKeySequence(Qt::Key_2));
	id_edit[2] = editMenu->addAction (TR("Select All 3D Objects"), this, SLOT(select_all_3d_obj()));
	id_edit[2]->setShortcut (QKeySequence(Qt::Key_3));
	id_edit[3] = editMenu->addAction (TR("Add Lines"), this, SLOT(add_lines()));
	id_edit[4] = editMenu->addAction (TR("Add Texts"), this, SLOT(add_texts()));

	objectsMenu = new QMenu (TR("&Objects"), this);
	objectsMenu->setTearOffEnabled (true);
	id_obj[0] = objectsMenu->addAction (TR("Rotate"), this, SLOT(obj_rotate()));
	id_obj[0]->setShortcut (QKeySequence(Qt::Key_R));
	id_obj[1] = objectsMenu->addAction (TR("Scale"), this, SLOT(obj_scale_question()));
	id_obj[2] = objectsMenu->addAction (TR("Translate (x,y)"), this, SLOT(obj_translate_xy()));
	id_obj[2]->setShortcut (QKeySequence(Qt::Key_T));
	id_obj[3] = objectsMenu->addAction (TR("Translate (z)"), this, SLOT(obj_translate_z()));
	objectsMenu->addSeparator ();
	id_obj[4] = objectsMenu->addAction (TR("Toggle Lighting"), this, SLOT(lighting()));
	id_obj[4]->setShortcut (QKeySequence(Qt::Key_L));
	objectsMenu->addSeparator ();
	id_obj[5] = objectsMenu->addAction (TR("Restore 2D"), this, SLOT(qt_2d_restore()));
	id_obj[5]->setShortcut (QKeySequence(Qt::CTRL | Qt::Key_2));
	id_obj[6] = objectsMenu->addAction (TR("Restore 3D"), this, SLOT(qt_3d_restore()));
	id_obj[6]->setShortcut (QKeySequence(Qt::CTRL | Qt::Key_3));

	projectionMenu = new QMenu (TR("&Projectied Image"), this);
	projectionMenu->setTearOffEnabled (true);
	id_pro[0] = projectionMenu->addAction (TR("Rotate"), this, SLOT(proj_rotate()));
	id_pro[0]->setShortcut (QKeySequence(Qt::CTRL | Qt::Key_R));
	id_pro[1] = projectionMenu->addAction (TR("Scale"), this, SLOT(proj_scale_question()));
	id_pro[2] = projectionMenu->addAction (TR("Translate"), this, SLOT(proj_translate()));
	id_pro[2]->setShortcut (QKeySequence(Qt::CTRL | Qt::Key_T));
	projectionMenu->addSeparator ();
	id_pro[3] = projectionMenu->addAction (TR("Restore"), this, SLOT(qt_proj_restore()));
	id_pro[3]->setShortcut (QKeySequence(Qt::CTRL | Qt::Key_P));

	animeMenu = new QMenu (TR("&Animation"), this);
	animeMenu->setTearOffEnabled (true);
	id_anime[0] = animeMenu->addAction (TR("Replay"), this, SLOT(replay1()));
	id_anime[1] = animeMenu->addAction (TR("Replay (Slow)"), this, SLOT(replay2()));
	id_anime[2] = animeMenu->addAction (TR("Replay (Very Slow)"), this, SLOT(replay3()));
	id_anime[3] = animeMenu->addAction (TR("Single Frame Forward"), this, SLOT(anime_forward()));
	id_anime[4] = animeMenu->addAction (TR("Reverse Play"), this, SLOT(reverse_replay1()));
	id_anime[5] = animeMenu->addAction (TR("Reverse Play (Slow)"), this, SLOT(reverse_replay2()));
	id_anime[6] = animeMenu->addAction (TR("Reverse Play (Very Slow)"), this, SLOT(reverse_replay3()));
	id_anime[7] = animeMenu->addAction (TR("Single Frame Reverse"), this, SLOT(anime_backward()));
	id_anime[8] = animeMenu->addAction (TR("Pause"), this, SLOT(anime_pause()));

	configMenu = new QMenu (TR("&Config"), this);
	configMenu->setTearOffEnabled (true);
	configMenu->addAction (TR("Color"), this, SLOT(color()));
	configMenu->addAction (TR("Command Line"), this, SLOT(command()));
	configMenu->addAction (TR("TrueType Font"), this, SLOT(font()));
	configMenu->addAction (TR("Window Size"), this, SLOT(windowsize()));

	helpMenu = new QMenu (TR("&Help"), this);
	helpMenu->setTearOffEnabled (true);
	QAction *actKey = helpMenu->addAction (TR("Key Bindings"), this, SLOT(keybindings()));
	actKey->setShortcut (QKeySequence(Qt::Key_H));
	QAction *actManual = helpMenu->addAction (TR("Manual"), this, SLOT(manual()));
	actManual->setShortcut (QKeySequence(Qt::Key_M));
	helpMenu->addSeparator ();
	helpMenu->addAction (TR("About"), this, SLOT(about()));

	// menu bar //
	menubar->addMenu (fileMenu);
	menubar->addMenu (editMenu);
	menubar->addMenu (objectsMenu);
	menubar->addMenu (projectionMenu);
	menubar->addMenu (animeMenu);
	menubar->addMenu (configMenu);
	menubar->addSeparator ();
	menubar->addMenu (helpMenu);
	id_menu[0] = editMenu->menuAction();
	id_menu[1] = objectsMenu->menuAction();
	id_menu[2] = projectionMenu->menuAction();

	// pop up menu //
	contextMenu = new QMenu (this);
	Q_CHECK_PTR (contextMenu);

	id_pop_edit[0] = contextMenu->addAction (TR("Pointer"), this, SLOT(select_obj()));
	id_pop_edit[0]->setShortcut (QKeySequence(Qt::Key_Escape));
	id_pop_edit[1] = contextMenu->addAction (TR("Select All 2D Objects"), this, SLOT(select_all_2d_obj()));
	id_pop_edit[1]->setShortcut (QKeySequence(Qt::Key_2));
	id_pop_edit[2] = contextMenu->addAction (TR("Select All 3D Objects"), this, SLOT(select_all_3d_obj()));
	id_pop_edit[2]->setShortcut (QKeySequence(Qt::Key_3));
	contextMenu->addSeparator ();
	id_pop_obj[0] = contextMenu->addAction (TR("Rotate"), this, SLOT(obj_rotate()));
	id_pop_obj[0]->setShortcut (QKeySequence(Qt::Key_R));
	id_pop_obj[1] = contextMenu->addAction (TR("Scale"), this, SLOT(obj_scale()));
	id_pop_obj[1]->setShortcut (QKeySequence(Qt::Key_E));
	id_pop_obj[2] = contextMenu->addAction (TR("Translate (x,y)"), this, SLOT(obj_translate_xy()));
	id_pop_obj[2]->setShortcut (QKeySequence(Qt::Key_T));
	id_pop_obj[3] = contextMenu->addAction (TR("Translate (z)"), this, SLOT(obj_translate_z()));
	id_pop_obj[4] = contextMenu->addAction (TR("Toggle Lighting"), this, SLOT(lighting()));
	id_pop_obj[4]->setShortcut (QKeySequence(Qt::Key_L));
	id_pop_obj[5] = contextMenu->addAction (TR("Restore 2D"), this, SLOT(qt_2d_restore()));
	id_pop_obj[5]->setShortcut (QKeySequence(Qt::CTRL | Qt::Key_2));
	id_pop_obj[6] = contextMenu->addAction (TR("Restore 3D"), this, SLOT(qt_3d_restore()));
	id_pop_obj[6]->setShortcut (QKeySequence(Qt::CTRL | Qt::Key_3));
	contextMenu->addSeparator ();
	id_pop_pro[0] = contextMenu->addAction (TR("Rotate Projected Image"), this, SLOT(proj_rotate()));
	id_pop_pro[0]->setShortcut (QKeySequence(Qt::CTRL | Qt::Key_R));
	id_pop_pro[1] = contextMenu->addAction (TR("Scale Projected Image"), this, SLOT(proj_scale()));
	id_pop_pro[1]->setShortcut (QKeySequence(Qt::CTRL | Qt::Key_E));
	id_pop_pro[2] = contextMenu->addAction (TR("Translate Projected Image"), this, SLOT(proj_translate()));
	id_pop_pro[2]->setShortcut (QKeySequence(Qt::CTRL | Qt::Key_T));
	id_pop_pro[3] = contextMenu->addAction (TR("Restore Projected Image"), this, SLOT(qt_proj_restore()));
	id_pop_pro[3]->setShortcut (QKeySequence(Qt::CTRL | Qt::Key_P));

	gui_2d_sensitive (false);
	gui_3d_sensitive (false);
	gui_sensitive_have_3d (false);
}

gsglMainWindow::~gsglMainWindow ()
{
        delete fileMenu;
        delete exportMenu;
        delete editMenu;
        delete objectsMenu;
        delete projectionMenu;
        delete animeMenu;
        delete configMenu;
        delete helpMenu;
        delete contextMenu;
        delete menubar;
	delete gl;
	delete vbox;
}

void gsglMainWindow::closeEvent (QCloseEvent *event)
{
	gui_exit_loop ();
	event->accept();
}

void gsglMainWindow::contextMenuEvent (QContextMenuEvent *e)
{
	if (Add_text) return;
	contextMenu->exec (e->globalPos());
}

void gsglMainWindow::saving_message ()
{
	// message window
	int el;
	QDialog * window = new QDialog (this);
	window->setModal (true);
	QLabel *label = new QLabel (window);
	label->setText (TR("Saving now ..."));

	window->show ();
	qApp->processEvents ();
	QElapsedTimer timer;
	timer.start();
	//

	if (gsgl_inq_open_tmp_file_for_anime (Page) && !save_this_frame) {
		gsgl_copy_anime_file_to_save_file (Page);
	} else {
		gsglWriteToFile *w;

		w = gsgl_init_write_to_file ();	
		gsgl_write_to_file (gsgl_get_fp_write(Library), w, Page);
		gsgl_free (w);
	}
	if (save_this_frame) 
		save_this_frame = 0;

	// message window
	el = static_cast<int>(timer.elapsed());
	if (el < 100 /* 0.1sec */) {
		usleep (100000 - el * 1000);
	}
	qApp->processEvents ();
	window->hide ();
	delete label;
	delete window;
	// timer is stack-allocated
	//
}

void gsglMainWindow::save ()
{
	if (gsgl_inq_i_am_a_server (Page->library)) {
		gsgl_tcp_server_send_command (GSGL_TCP_GUI_SAVE, Page->library);
	} else {
		if (gsgl_inq_savefile_opened ()) {
			saving_message ();
		} else {
			save_as ();
		}
	}
}

void gsglMainWindow::open_and_save (const QString & fileName)
{
	int open = 1;
	QString s;

	if (gsgl_inq_savefile_opened ()) 
		gsgl_file_write_close (Library);
	open = gsgl_file_write_open (fileName.toUtf8().constData(), Library);
	if (open != 0) {
		s = QString("Can not open file %1").arg(fileName);
		QMessageBox::warning (this, TR("Can not open a file"), s, 
				      QMessageBox::Ok);
	} else {
		saving_message ();
	}
}

void gsglMainWindow::save_as ()
{
	if (gsgl_inq_i_am_a_server (Page->library)) {
		gsgl_tcp_server_send_command (GSGL_TCP_GUI_SAVE_AS, Page->library);
	} else {
		QString fileName = QFileDialog::getSaveFileName(
			this,
			TR("Save As ..."),
			QString(),
			TR("Gsgl Files (*.gsgl)")
		);
		if (fileName.isEmpty())
			return;
		if (!fileName.endsWith(".gsgl")) {
			fileName += ".gsgl";
		}
		if (QFile::exists(fileName)) {
			QMessageBox::StandardButton ret =
				QMessageBox::question (this, TR("Overwrite?"),
					TR("Old file is going to be overwritten.\nAre you sure ?"),
					QMessageBox::Ok | QMessageBox::Cancel,
					QMessageBox::Cancel);
			if (ret != QMessageBox::Ok)
				return;
		}
		open_and_save (fileName);
	}
}

void gsglMainWindow::save_close ()
{
	if (gsgl_inq_i_am_a_server (Page->library)) {
		gsgl_tcp_server_send_command (GSGL_TCP_GUI_SAVE_AND_CLOSE, Page->library);
        } else {
		save();
		if (gsgl_inq_mode (Library) == GSGL_MODE_WINDOW) {
			//qApp->processEvents ();
			qApp->quit ();
		} else if (gsgl_inq_mode (Library) == GSGL_MODE_TCPIP) {
			gsgl_tcp_push (GSGL_TCP_GUI_SAVE_AND_CLOSE, Page);
		}
        }
}

void gsglMainWindow::save_frame ()
{
	save_this_frame = 1;
	if (gsgl_inq_i_am_a_server (Page->library)) {
		gsgl_tcp_server_send_command (GSGL_TCP_GUI_SAVE_THIS_FRAME, Page->library);
	} else {
		save ();
	}
}

void gsglMainWindow::export_command (enum export_type type, QString fileName)
{
	bool ok = false;
	QString cap;
	QString lab;
	QString com;

	if (type == EPS_VEC) {
		cap = TR("Export EPS");
		lab = TR("Enter the command line generates eps.\n"
			"('%s' will be replaced by a file name)");
		com = gsgl_inq_eps_vec_command (Library);
	} else if (type == EPS_RAS) {
		cap = TR("Export EPS");
		lab = TR("Enter the command line generates eps.\n"
			"('%s' will be replaced by a file name)");
		com = gsgl_inq_eps_ras_command (Library);
	} else if (type == PNG) {
		cap = TR("Export PNG");
		lab = TR("Enter the command line generates png.\n"
			"('%s' will be replaced by a file name)");
		com = gsgl_inq_png_command (Library);
	} else if (type == JPEG) {
		cap = TR("Export JPEG");
		lab = TR("Enter the command line generates jpeg.\n"
			"('%s' will be replaced by a file name)");
		com = gsgl_inq_jpg_command (Library);
	}
	QString text = QInputDialog::getText(this, cap, lab,
		QLineEdit::Normal, com, &ok);
	
	if ( ok && !text.isEmpty() ) {
		// message window
		int el;
		QDialog * window = new QDialog ();
		QLabel *label = new QLabel (window);
		label->setText (TR("Exporting now ..."));
 
		window->show ();
		qApp->processEvents ();
		QElapsedTimer timer;
		timer.start();
		//
		QString   com2;
		FILE      *fp;
		char      tmp[] = "/tmp/gsglXXXXXX";
		int       fd;
		gsglWriteToFile *w;

		fd = mkstemp (tmp);
		fp = fdopen (fd, "w"); 

		w = gsgl_init_write_to_file ();		
		gsgl_write_to_file (fp, w, Page);

		if (fclose (fp))
		      gsgl_warning ("gsglMainWindow::export_command", "failed to close tmp file");
		
		com2 = QString::asprintf (text.toUtf8().constData(), fileName.toUtf8().constData(), tmp);
		system (com2.toUtf8().constData());

		if (remove (tmp))
		      gsgl_warning ("gsglMainWindow::export_command", "failed to remove tmp file");	

		// message window
		el = static_cast<int>(timer.elapsed());
		if (el < 100 /* 0.1sec */) {
			usleep (100000 - el * 1000);
		}
		window->hide ();
		qApp->processEvents ();
		delete label;
		delete window;
		// timer is stack-allocated
		//
	}
}

void gsglMainWindow::export_common (enum export_type type)
{
	if (gsgl_inq_i_am_a_server (Page->library)) {
		if (type == EPS_VEC) {
			gsgl_tcp_server_send_command (GSGL_TCP_GUI_EXPORT_VEC_EPS, Page->library);
		} else if (type == EPS_RAS) {
			gsgl_tcp_server_send_command (GSGL_TCP_GUI_EXPORT_RAS_EPS, Page->library);
		} else if (type == PNG) {
			gsgl_tcp_server_send_command (GSGL_TCP_GUI_EXPORT_PNG, Page->library);
		} else if (type == JPEG) {
			gsgl_tcp_server_send_command (GSGL_TCP_GUI_EXPORT_JPEG, Page->library);
		}
	} else {
		QString filter;
		if (type == EPS_VEC || type == EPS_RAS) {
			filter = TR("EPS (Encapslated PostScript) Files (*.eps)");
		} else if (type == PNG) {
			filter = TR("PNG (Portable Network Graphics) Files (*.png)");
		} else if (type == JPEG) {
			filter = TR("JPEG (Joint Photographic Experts Group) Files (*.jpg)");
		}

		QString fileName = QFileDialog::getSaveFileName(
			this,
			TR("Export as ..."),
			QString(),
			filter
		);
		if (fileName.isEmpty())
			return;

		if (type == EPS_VEC || type == EPS_RAS) {
			if (!fileName.endsWith ( ".eps" )) {
				fileName += ".eps";
			}
		} else if (type == PNG) {
			if (!fileName.endsWith ( ".png" )) {
				fileName += ".png";
			}
		} else if (type == JPEG) {
			if (!fileName.endsWith ( ".jpg" )) {
				fileName += ".jpg";
			}
		}
		if (QFile::exists(fileName)) {
			QMessageBox::StandardButton ret =
				QMessageBox::question (this, TR("Overwrite?"),
					TR("Old file is going to be overwritten.\nAre you sure ?"),
					QMessageBox::Ok | QMessageBox::Cancel,
					QMessageBox::Cancel);
			if (ret != QMessageBox::Ok)
				return;
		}
		export_command (type, fileName);
	}
}

void gsglMainWindow::export_eps_vec ()
{
	export_common (EPS_VEC);
}

void gsglMainWindow::export_eps_ras ()
{
	export_common (EPS_RAS);
}

void gsglMainWindow::export_png ()
{
	export_common (PNG);
}

void gsglMainWindow::export_jpeg ()
{
	export_common (JPEG);
}

void gsglMainWindow::print ()
{
	if (gsgl_inq_i_am_a_server (Page->library)) {
		gsgl_tcp_server_send_command (GSGL_TCP_GUI_PRINT, Page->library);
		return;
	}

	bool ok = false;
	QString cap;
	QString lab;
	QString com;

	cap = TR("Print ...");
	lab = TR("Enter the command line print a gsgl file.\n"
		"('%s' will be replaced by a file name)");
	com = gsgl_inq_print_command (Library);
	QString text = QInputDialog::getText(this, cap, lab,
		QLineEdit::Normal, com, &ok );
	
	if ( ok && !text.isEmpty() ) {		
		// user entered something and pressed OK
		QString   com2;
		FILE      *fp;
		char      tmp[] = "/tmp/gsglXXXXXX";
		int       fd;
		gsglWriteToFile *w;

		fd = mkstemp (tmp);
		fp = fdopen (fd, "w"); 

		w = gsgl_init_write_to_file ();		
		gsgl_write_to_file (fp, w, Page);

		if (fclose (fp))
			gsgl_warning ("gsglMainWindow::print", "failed to close tmp file");
		
		com2 = QString::asprintf (text.toUtf8().constData(), tmp);
		system (com2.toUtf8().constData());

		if (remove (tmp))
			gsgl_warning ("gsglMainWindow::print", "failed to remove tmp file");	
	}
}

void gsglMainWindow::dump ()
{
	if (gsgl_inq_i_am_a_server (Page->library)) {
		gsgl_tcp_dump_window (Page->win, Page->library);
	} else {
		if (Page && Page->win)
			gsgl_dumpwin (Page->win, Page);
	}
}

void gsglMainWindow::quit_immediately ()
{
	gsgl_gui_quit (exit, Page);
}

void gsglMainWindow::quit_app ()
{
	QMessageBox::StandardButton ret =
		QMessageBox::question (this, TR("Quit?"), TR("Are you sure to quit?"),
				       QMessageBox::Ok | QMessageBox::Cancel,
				       QMessageBox::Cancel);
	if (ret == QMessageBox::Ok) {
		quit_immediately ();
	}
}

void gsglMainWindow::select_obj()
{
	gsgl_gui_pointer (Page, GSGL_TRUE);
	gsgl_yes_select_objects (Page);
	gui_cursor_default (this);
}

void gsglMainWindow::select_all_2d_obj()
{
	gui_2d_sensitive (true);
	gui_3d_sensitive (false);

	gsgl_yes_select_objects (Page);
	gsgl_2d_select_objects (Page);
}

void gsglMainWindow::select_all_3d_obj()
{
	gui_3d_sensitive (true);
	gui_2d_sensitive (false);

	gsgl_yes_select_objects (Page);
	gsgl_3d_select_objects (Page);
}

void gsglMainWindow::obj_rotate()
{
	if (gsgl_2d_is_selected (Page)) {
		qt_2d_rotate (false);
	} else if (gsgl_3d_is_selected (Page)) {
		qt_3d_rotate (false);
	} else if (gsgl_inq_3dobjects_exist (Page)) {
		gsgl_3d_select_objects_without_color (Page);
		qt_3d_rotate (false);
	} else {
		gsgl_2d_select_objects_without_color (Page);
		qt_2d_rotate (false);
	}
}

void gsglMainWindow::obj_scale_common (bool b)
{
	if (gsgl_2d_is_selected (Page)) {
		qt_2d_scale (b);
	} else if (gsgl_3d_is_selected (Page)) {
		qt_3d_scale (b);
	} else if (gsgl_inq_3dobjects_exist (Page)) {
		gsgl_3d_select_objects_without_color (Page);
		qt_3d_scale (b);
	} else {
		gsgl_2d_select_objects_without_color (Page);
		qt_2d_scale (b);
	}
}

void gsglMainWindow::obj_scale()
{
	obj_scale_common (false);
}

void gsglMainWindow::obj_scale_question()
{
	obj_scale_common (true);
}
	
void gsglMainWindow::obj_translate_xy()
{
	if (gsgl_2d_is_selected (Page)) {
		qt_2d_trans ();
	} else if (gsgl_3d_is_selected (Page)) {
		qt_3d_trans_xy ();
	} else if (gsgl_inq_3dobjects_exist (Page)) {
		gsgl_3d_select_objects_without_color (Page);
		qt_3d_trans_xy ();
	} else {
		gsgl_2d_select_objects_without_color (Page);
		qt_2d_trans ();
	}
}

void gsglMainWindow::obj_translate_z()
{
	if (gsgl_2d_is_selected (Page)) {
	} else if (gsgl_3d_is_selected (Page)) {
		qt_3d_trans_z ();
	} else if (gsgl_inq_3dobjects_exist (Page)) {
		gsgl_3d_select_objects_without_color (Page);
		qt_3d_trans_z ();
	} else {
	}
}

void gsglMainWindow::proj_rotate()
{
	qt_proj_rotate (false);
}

void gsglMainWindow::proj_scale()
{
	qt_proj_scale (false);
}

void gsglMainWindow::proj_scale_question ()
{
	qt_proj_scale (true);
}

void gsglMainWindow::proj_translate()
{
	qt_proj_translate ();
}

void gsglMainWindow::qt_selection_sensitive (bool b)
{
	int i;

	for (i=0; i<3; i++)
		if (id_pop_edit[i]) id_pop_edit[i]->setEnabled (b);
	for (i=0; i<7; i++)
		if (id_pop_obj[i]) id_pop_obj[i]->setEnabled (b);
	for (i=0; i<4; i++)
		if (id_pop_pro[i]) id_pop_pro[i]->setEnabled (b);

	for (i=0; i<3; i++)
		if (id_menu[i]) id_menu[i]->setEnabled (b);
}

void gsglMainWindow::replay1 ()
{
	anime_replay (2);
}

void gsglMainWindow::replay2 ()
{
	anime_replay (100);
}

void gsglMainWindow::replay3 ()
{
	anime_replay (500);
}

void gsglMainWindow::reverse_replay1 ()
{
	anime_reverse_replay (2);
}

void gsglMainWindow::reverse_replay2 ()
{
	anime_reverse_replay (100);
}

void gsglMainWindow::reverse_replay3 ()
{
	anime_reverse_replay (500);
}

void gsglMainWindow::color ()
{
	if (gsgl_inq_i_am_a_server (Page->library)) {
		gsgl_tcp_server_send_command (GSGL_TCP_GUI_CONFIG_COLOR, Page->library);
		return;
	}
	gsglColorDialog *d = new gsglColorDialog (Page, this);
	if (d->exec () == QDialog::Accepted) {
		gsglConfigColor *c = gsgl_inq_config_color (Library);
		d->get_result (&(c->bg), c->box, c->frame);
	}
	delete d;
}

void gsglMainWindow::command ()
{
	if (gsgl_inq_i_am_a_server (Page->library)) {
		gsgl_tcp_server_send_command (GSGL_TCP_GUI_CONFIG_COMMAND, Page->library);
		return;
	}
	gsglCommandDialog *d = new gsglCommandDialog (Page, this);
	if (d->exec () == QDialog::Accepted) {
		d->get_result ();
	}
	delete d;
}

void gsglMainWindow::font ()
{
	if (gsgl_inq_i_am_a_server (Page->library)) {
		gsgl_tcp_server_send_command (GSGL_TCP_GUI_CONFIG_FONT, Page->library);
		return;
	}
	gsglFontsDialog *d = new gsglFontsDialog (Page, this);
	if (d->exec () == QDialog::Accepted) {
		d->get_result ();
	}
	delete d;
}

void gsglMainWindow::windowsize ()
{
	if (gsgl_inq_i_am_a_server (Page->library)) {
		gsgl_tcp_server_send_command (GSGL_TCP_GUI_CONFIG_WINDOW, Page->library);
		return;
	}
	gsglWindowSizeDialog *d = new gsglWindowSizeDialog (Page, this);
	if (d->exec () == QDialog::Accepted) {
		d->get_result ();
	}
	delete d;
}

void gsglMainWindow::keybindings()
{
	QMessageBox::about (this, TR("Key bindings"), 
			    "    c           : continue.\n"
			    "    return      : same as c.\n"
			    "    n           : same as c.\n" 
			    "    a           : open file and save this page.\n"
			    "    s           : save this page, open file if not yet.\n"
			    "    y           : save this page and close window.\n" 
			    "    d           : dump dot image. (x)\n"
			    "    h           : this informations.\n"
			    "    p           : print out this page.\n"	
			    "    q           : quit run."); 
}

void gsglMainWindow::manual ()
{
	system (gsgl_inq_man_command (Library));
}

void gsglMainWindow::about ()
{
	QMessageBox::about (this, TR("About"), 
			    "GSAF/OpenGL\n"
			    "Copyright (C) 2001-2004 AKUTSU Taku, FUKUYAMA Atsushi\n"
			    "<akutsu@p-grp.nucleng.kyoto-u.ac.jp>\n"
			    "http://p-grp.nucleng.kyoto-u.ac.jp/~akutsu/gsgl.html\n"
			    "\n"
			    "Copyright of GSAF system: A. Fukuyama, Kyoto University\n"
			    "Copyright of GSAF 3D package (A*.f): T. Akutsu, Kyoto University\n"
			    "Copytight of adb_ill_1.1: Adobe Systems, inc\n"
			    "pgfont data was derived from Hershey character set used in PGPLOT.\n"
			    "\n"
			    "This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License\n"
			    " as published by the Free Software Foundation; either version 2, or (at your option) any later version.\n"
			    "\n"
			    "This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;\n"
			    "without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n"
			    " See the GNU General Public License for more details.\n"
			    "\n"
			    "You should have received a copy of the GNU General Public License along with this program;\n"
			    " if not, write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.");
}
