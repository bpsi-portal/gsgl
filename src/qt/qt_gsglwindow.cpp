#include "qt_compat.h"
#include <qgl.h>
#include <qfile.h>
#ifdef USE_QT3
#    include <qfiledialog.h>
#    include <qmainwindow.h>
#    include <qvbox.h>
#else
#    include <q3filedialog.h>
#    include <q3mainwindow.h>
#    include <q3vbox.h>
#    include <QLabel>
#    include <QContextMenuEvent>
#endif
#include <qmenubar.h>
#include <qapplication.h>
#include <qmessagebox.h>
#include <qcursor.h>
#include <qinputdialog.h>
#include <qpaintdevice.h>
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
#include <iostream>

#define TR(x) QString::fromUtf8(_(x))

#ifdef USE_QT3
gsglMainWindow::gsglMainWindow (gsglWindow *window, QWidget* parent, const char* name, WFLAGS fl)
	: QMainWindow (parent, name, fl)
#elif defined USE_QT4
gsglMainWindow::gsglMainWindow (gsglWindow *window, QWidget* parent, const char* name, WFLAGS fl)
	: Q3MainWindow (parent, name, fl)
#endif
{
	save_this_frame = 0;
	Win = window;
	Library = window->library;
	Add_text = GSGL_FALSE;
        Add_text_dialog = NULL;

	setCaption (gsgl_window_title());

	if (gsgl_inq_userdefined_window (Library)) {
		setGeometry (gsgl_inq_userdefined_corner_x (Library), 
			     gsgl_inq_userdefined_corner_y (Library), 0, 0);
	} else {
		int width =DisplayWidth(QPaintDevice::x11Display(),QPaintDevice::x11Screen());
		int height=DisplayHeight(QPaintDevice::x11Display(),QPaintDevice::x11Screen());
		width -= Library->winwidth0;
		height -= Library->winheight0;
		setGeometry (width/2, height/2, 0, 0);
	}

#ifdef USE_QT3	
	vbox = new QVBox (this);
#elif defined USE_QT4
	vbox = new Q3VBox (this);
#endif
	setCentralWidget (vbox);

	QGLFormat f;
	f.setRgba (TRUE); 
	f.setDepth (TRUE);
	f.setDoubleBuffer (TRUE);
	f.setDirectRendering (TRUE);
	gl = new GLDrawArea (Win, f, vbox);

	menubar = new QMenuBar (this);

#define ins0(x,t,s) x->insertItem (t, this, SLOT(s))
#define ins1(x,t,s,k) x->insertItem (t, this, SLOT(s), k)
#ifdef USE_QT3
	exportMenu = new QPopupMenu (this);
#else
	exportMenu = new Q3PopupMenu (this);
#endif
	exportMenu->insertTearOffHandle();
	ins0(exportMenu, TR("EPS (Vector)"), export_eps_vec());
	ins0(exportMenu, TR("EPS (Raster)"), export_eps_ras());
	ins0(exportMenu, TR("PNG"), export_png());
	ins0(exportMenu, TR("JPEG"), export_jpeg());

#ifdef USE_QT3
	fileMenu = new QPopupMenu (this);
#else
	fileMenu = new Q3PopupMenu (this);
#endif
	fileMenu->insertTearOffHandle();
	id_file[0] = ins1(fileMenu, TR("Save As ..."), save_as(), Qt::Key_A);
	id_file[1] = ins1(fileMenu, TR("Save"), save(), Qt::Key_S);
	id_file[2] = ins1(fileMenu, TR("Save and Close"), save_close(), Qt::Key_Y);
	id_file[3] = ins0(fileMenu, TR("Save This Frame"), save_frame());
	id_file[4] = fileMenu->insertItem (TR("Export"), exportMenu);
	fileMenu->insertSeparator ();
	id_file[5] = ins1(fileMenu, TR("Print ..."), print(), Qt::Key_P);
	fileMenu->insertSeparator ();
	id_file[6] = ins1(fileMenu, TR("Dump Dot Image"), dump(), Qt::Key_D);
	fileMenu->insertSeparator ();
	id_file[7] = fileMenu->insertItem (TR("Close"), qApp, SLOT(quit()), Qt::Key_C);
	id_file[8] = ins1(fileMenu, TR("Quit"), quit_app(), Qt::Key_Q);

#ifdef USE_QT3
	editMenu = new QPopupMenu (this);
#else
	editMenu = new Q3PopupMenu (this);
#endif
	editMenu->insertTearOffHandle();
	id_edit[0] = ins1(editMenu, TR("Pointer"), select_obj(), Qt::Key_Escape);	
	id_edit[1] = ins1(editMenu, TR("Select All 2D Objects"), select_all_2d_obj(), Qt::Key_2);	
	id_edit[2] = ins1(editMenu, TR("Select All 3D Objects"), select_all_3d_obj(), Qt::Key_3);	
	id_edit[3] = ins0(editMenu, TR("Add Lines"), add_lines());
	id_edit[4] = ins0(editMenu, TR("Add Texts"), add_texts());

#ifdef USE_QT3
	objectsMenu = new QPopupMenu (this);
#else
	objectsMenu = new Q3PopupMenu (this);
#endif
	objectsMenu->insertTearOffHandle();
	id_obj[0] = ins1(objectsMenu, TR("Rotate"), obj_rotate(), Qt::Key_R);
	id_obj[1] = ins0(objectsMenu, TR("Scale"), obj_scale_question());
	id_obj[2] = ins1(objectsMenu, TR("Translate (x,y)"), obj_translate_xy(), Qt::Key_T);
	id_obj[3] = ins0(objectsMenu, TR("Translate (z)"), obj_translate_z());
	objectsMenu->insertSeparator ();
	id_obj[4] = ins1(objectsMenu, TR("Toggle Lighting"), lighting(), Qt::Key_L);
	objectsMenu->insertSeparator ();
	id_obj[5] = ins1(objectsMenu, TR("Restore 2D"), qt_2d_restore(), Qt::CTRL+Qt::Key_2);
	id_obj[6] = ins1(objectsMenu, TR("Restore 3D"), qt_3d_restore(), Qt::CTRL+Qt::Key_3);

#ifdef USE_QT3
	projectionMenu = new QPopupMenu (this);
#else
	projectionMenu = new Q3PopupMenu (this);
#endif
	projectionMenu->insertTearOffHandle();
	id_pro[0] = ins1(projectionMenu, TR("Rotate"), proj_rotate(), Qt::CTRL+Qt::Key_R);
	id_pro[1] = ins0(projectionMenu, TR("Scale"), proj_scale_question());
	id_pro[2] = ins1(projectionMenu, TR("Translate"), proj_translate(), Qt::CTRL+Qt::Key_T);
	projectionMenu->insertSeparator ();
	id_pro[3] = ins1(projectionMenu, TR("Restore"), qt_proj_restore(), Qt::CTRL+Qt::Key_P);

#ifdef USE_QT3
	animeMenu = new QPopupMenu (this);
#else
	animeMenu = new Q3PopupMenu (this);
#endif
	animeMenu->insertTearOffHandle();
	id_anime[0] = ins0(animeMenu, TR("Replay"), replay1());
	id_anime[1] = ins0(animeMenu, TR("Replay (Slow)"), replay2());
	id_anime[2] = ins0(animeMenu, TR("Replay (Very Slow)"), replay3());
	id_anime[3] = ins0(animeMenu, TR("Single Frame Forward"), anime_forward());
	id_anime[4] = ins0(animeMenu, TR("Reverse Play"), reverse_replay1());
	id_anime[5] = ins0(animeMenu, TR("Reverse Play (Slow)"), reverse_replay2());
	id_anime[6] = ins0(animeMenu, TR("Reverse Play (Very Slow)"), reverse_replay3());
	id_anime[7] = ins0(animeMenu, TR("Single Frame Reverse"), anime_backward());
	id_anime[8] = ins0(animeMenu, TR("Pause"), anime_pause());

#ifdef USE_QT3
	configMenu = new QPopupMenu (this);
#else
	configMenu = new Q3PopupMenu (this);
#endif
	configMenu->insertTearOffHandle();
	ins0(configMenu, TR("Color"), color());
	ins0(configMenu, TR("Command Line"), command());
	ins0(configMenu, TR("TrueType Font"), font());
	ins0(configMenu, TR("Window Size"), windowsize());

#ifdef USE_QT3
	helpMenu = new QPopupMenu (this);
#else
	helpMenu = new Q3PopupMenu (this);
#endif
	helpMenu->insertTearOffHandle();
	ins1(helpMenu, TR("Key Bindings"), keybindings(), Qt::Key_H);
	ins1(helpMenu, TR("Manual"), manual(), Qt::Key_M);
	helpMenu->insertSeparator ();
	ins0(helpMenu, TR("About"), about());

	// menu bar //
	menubar->insertItem (TR("&File"), fileMenu);
	id_menu[0] = menubar->insertItem (TR("&Edit"), editMenu);
	id_menu[1] = menubar->insertItem (TR("&Objects"), objectsMenu);
	id_menu[2] = menubar->insertItem (TR("&Projectied Image"), projectionMenu);
	menubar->insertItem (TR("&Animation"), animeMenu);
	menubar->insertItem (TR("&Config"), configMenu);
	menubar->insertSeparator ();
	menubar->insertItem (TR("&Help"), helpMenu);

	// pop up menu //
#ifdef USE_QT3
	contextMenu = new QPopupMenu (this);
#else
	contextMenu = new Q3PopupMenu (this);
#endif
	Q_CHECK_PTR (contextMenu);

	id_pop_edit[0] = ins1(contextMenu, TR("Pointer"), select_obj(), Qt::Key_Escape);	
	id_pop_edit[1] = ins1(contextMenu, TR("Select All 2D Objects"), select_all_2d_obj(), Qt::Key_2);
	id_pop_edit[2] = ins1(contextMenu, TR("Select All 3D Objects"), select_all_3d_obj(), Qt::Key_3);	
	contextMenu->insertSeparator ();
	id_pop_obj[0] = ins1(contextMenu, TR("Rotate"), obj_rotate(), Qt::Key_R);
	id_pop_obj[1] = ins1(contextMenu, TR("Scale"), obj_scale(), Qt::Key_E);
	id_pop_obj[2] = ins1(contextMenu, TR("Translate (x,y)"), obj_translate_xy(), Qt::Key_T);
	id_pop_obj[3] = ins0(contextMenu, TR("Translate (z)"), obj_translate_z());
	id_pop_obj[4] = ins1(contextMenu, TR("Toggle Lighting"), lighting(), Qt::Key_L);
	id_pop_obj[5] = ins1(contextMenu, TR("Restore 2D"), qt_2d_restore(), Qt::CTRL+Qt::Key_2);
	id_pop_obj[6] = ins1(contextMenu, TR("Restore 3D"), qt_3d_restore(), Qt::CTRL+Qt::Key_3);
	contextMenu->insertSeparator ();
	id_pop_pro[0] = ins1(contextMenu, TR("Rotate Projected Image"), proj_rotate(), Qt::CTRL+Qt::Key_R);
	id_pop_pro[1] = ins1(contextMenu, TR("Scale Projected Image"), proj_scale(), Qt::CTRL+Qt::Key_E);
	id_pop_pro[2] = ins1(contextMenu, TR("Translate Projected Image"), proj_translate(), Qt::CTRL+Qt::Key_T);
	id_pop_pro[3] = ins1(contextMenu, TR("Restore Projected Image"), qt_proj_restore(), Qt::CTRL+Qt::Key_P);

	gui_2d_sensitive (FALSE);
	gui_3d_sensitive (FALSE);
	gui_sensitive_have_3d (FALSE);
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

void gsglMainWindow::contextMenuEvent (QContextMenuEvent *e)
{
	if (Add_text) return;
	contextMenu->exec (QCursor::pos());
}

void gsglMainWindow::saving_message ()
{
	// message window
	int el;
	QDialog * window = new QDialog (NULL, NULL, TRUE);
	QLabel *label = new QLabel (window);
	label->setText (TR("Saving now ..."));

	window->show ();
	qApp->processEvents ();
	QTime *t = new QTime ();
	t->start ();
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
	el = t->elapsed ();
	if (el < 100 /* 0.1sec */) {
		usleep (100000 - el * 1000);
	}
	qApp->processEvents ();
	window->hide ();
	delete label;
	delete window;
	delete t;
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
	open = gsgl_file_write_open (fileName.latin1(), Library);
	if (open != 0) {
		s.sprintf ("Can not open file %s", fileName.latin1());
		QMessageBox::warning (this, TR("Can not open a file"), s, 
				      QMessageBox::Ok,  QMessageBox::NoButton);
	} else {
		saving_message ();
	}
}

void gsglMainWindow::save_as ()
{
	if (gsgl_inq_i_am_a_server (Page->library)) {
		gsgl_tcp_server_send_command (GSGL_TCP_GUI_SAVE_AS, Page->library);
	} else {
#ifdef USE_QT3
		QFileDialog * fd = new QFileDialog( this, TR("Save As ..."), TRUE );
		fd->setMode (QFileDialog::AnyFile);
		fd->addFilter ("Gsgl Files (*.gsgl)");
		fd->setViewMode (QFileDialog::List);
#else
		Q3FileDialog * fd = new Q3FileDialog( this, TR("Save As ..."), TRUE );
		fd->setMode (Q3FileDialog::AnyFile);
		fd->addFilter ("Gsgl Files (*.gsgl)");
		fd->setViewMode (Q3FileDialog::List);
#endif

		QString fileName;
		if ( fd->exec() == QDialog::Accepted ) {
			fileName = fd->selectedFile();
			if (!fileName.endsWith ( ".gsgl" )) {
				fileName += ".gsgl";
			}
			if (QFile::exists(fileName)) {
				if (!QMessageBox::question (this, TR("Overwrite?"),
				        TR("Old file is going to be overwritten.\n"
					  "Are you sure ?"), 
				        TR("OK"), TR("Cancel"), QString::null, 0, 1)) {
					open_and_save (fileName);
				}
			} else {
				open_and_save (fileName);
			}
		}
		delete fd;
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
			qApp->exit_loop ();
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
	bool ok = FALSE;
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
	QString text = QInputDialog::getText(cap, lab,
		QLineEdit::Normal, com, &ok, this);
	
	if ( ok && !text.isEmpty() ) {
		// message window
		int el;
		QDialog * window = new QDialog ();
		QLabel *label = new QLabel (window);
		label->setText (TR("Exporting now ..."));
 
		window->show ();
		qApp->processEvents ();
		QTime *t = new QTime ();
		t->start ();
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
		
		com2.sprintf (text.latin1(), fileName.latin1(), tmp);
		system (com2.latin1());

		if (remove (tmp))
		      gsgl_warning ("gsglMainWindow::export_command", "failed to remove tmp file");	

		// message window
		el = t->elapsed ();
		if (el < 100 /* 0.1sec */) {
			usleep (100000 - el * 1000);
		}
		window->hide ();
		qApp->processEvents ();
		delete label;
		delete window;
		delete t;
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

#ifdef USE_QT3
		QFileDialog * fd = new QFileDialog( this, TR("Export as ..."), TRUE );
		fd->setMode (QFileDialog::AnyFile);
#else
		Q3FileDialog * fd = new Q3FileDialog( this, TR("Export as ..."), TRUE );
		fd->setMode (Q3FileDialog::AnyFile);
#endif
		
		if (type == EPS_VEC || type == EPS_RAS) {
			fd->addFilter ("EPS (Encapslated PostScript) Files (*.eps)");
		} else if (type == PNG) {
			fd->addFilter ("PNG (Portable Network Graphics) Files (*.png)");
		} else if (type == JPEG) {
			fd->addFilter ("JPEG (Joint Photographic Experts Group) Files (*.jpg)");
		}
#ifdef USE_QT3
		fd->setViewMode (QFileDialog::List);
#else
		fd->setViewMode (Q3FileDialog::List);
#endif

		QString fileName;
		if ( fd->exec() == QDialog::Accepted ) {
			fileName = fd->selectedFile();
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
				if (!QMessageBox::question (this, TR("Overwrite?"),
				    TR("Old file is going to be overwritten.\n"
				    "Are you sure ?"), 
				    TR("OK"), TR("Cancel"), QString::null, 0, 1)) {
					export_command (type, fileName);
				}
			} else {
				export_command (type, fileName);
			}
		}
		delete fd;
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

	bool ok = FALSE;
	QString cap;
	QString lab;
	QString com;

	cap = TR("Print ...");
	lab = TR("Enter the command line print a gsgl file.\n"
		"('%s' will be replaced by a file name)");
	com = gsgl_inq_print_command (Library);
	QString text = QInputDialog::getText(cap, lab,
		QLineEdit::Normal, com, &ok, this );
	
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
		
		com2.sprintf (text.latin1(), tmp);
		system (com2.latin1());

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
	if (!QMessageBox::question (this, TR("Quit?"), TR("Are you sure to quit?"), 
				    TR("OK"), TR("Cancel"), QString::null, 0, 1)) {
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
	gui_2d_sensitive (TRUE);
	gui_3d_sensitive (FALSE);

	gsgl_yes_select_objects (Page);
	gsgl_2d_select_objects (Page);
}

void gsglMainWindow::select_all_3d_obj()
{
	gui_3d_sensitive (TRUE);
	gui_2d_sensitive (FALSE);

	gsgl_yes_select_objects (Page);
	gsgl_3d_select_objects (Page);
}

void gsglMainWindow::obj_rotate()
{
	if (gsgl_2d_is_selected (Page)) {
		qt_2d_rotate (FALSE);
	} else if (gsgl_3d_is_selected (Page)) {
		qt_3d_rotate (FALSE);
	} else if (gsgl_inq_3dobjects_exist (Page)) {
		gsgl_3d_select_objects_without_color (Page);
		qt_3d_rotate (FALSE);
	} else {
		gsgl_2d_select_objects_without_color (Page);
		qt_2d_rotate (FALSE);
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
	obj_scale_common (FALSE);
}

void gsglMainWindow::obj_scale_question()
{
	obj_scale_common (TRUE);
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
	qt_proj_rotate (FALSE);
}

void gsglMainWindow::proj_scale()
{
	qt_proj_scale (FALSE);
}

void gsglMainWindow::proj_scale_question ()
{
	qt_proj_scale (TRUE);
}

void gsglMainWindow::proj_translate()
{
	qt_proj_translate ();
}

void gsglMainWindow::qt_selection_sensitive (bool b)
{
	int i;

	for (i=0; i<3; i++)
		contextMenu->setItemEnabled (id_pop_edit[i], b);
	for (i=0; i<7; i++)
		contextMenu->setItemEnabled (id_pop_obj[i], b);
	for (i=0; i<4; i++)
		contextMenu->setItemEnabled (id_pop_pro[i], b);

	for (i=0; i<3; i++)
		menubar->setItemEnabled (id_menu[i], b);
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
