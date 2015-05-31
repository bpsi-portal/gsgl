#AUTOMAKE_OPTIONS = foreign

SUBDIRS = .
DIST_SUBDIRS = . demo

lib_LTLIBRARIES = libgsgl.la

SRCSF = Aobsolute.f f77specific.f iof.f

SRCSC = text_texture.c configfile.c fileio_base.c fileio_read_base.c fileio_write.c internal_make.c quadrics.c clientstate.c callback.c anime.c displaylist.c matrix.c mem_list.c mem_list_read.c mem_list_write.c unix.c text.c harddisk.c drv_wrapper.c drv_gl.c drv_file.c base.c xwin.c ogl.c utils.c device.c tcpip.c fileformat.c getopt.c getopt1.c text_hersey.c text_freetype.c text_type1_kerning.c text_type1_size.c text_hersey_data.c fort77.c bsc1.c bsc2.c apl1.c bsc2d.c bsc3d.c apl3d.c utils2.c pers3d.c apl2.c cont1.c cont2.c spln.c rgbfunc.c apl3.c inline.c osmesa.c guiitems.c gui_common.cpp tcp_guiitems.c toolkit.cpp text_polygon.c clip.cpp
# register.c

HEADERC = ../include/gsgl.h anime.h device.h mem_list_read.h gettext.h ogl.h base.h displaylist.h mem_list_write.h gsaf.h callback.h drv_file.h fileformat.h tcpip.h clientstate.h drv_gl.h fileio_base.h gsglf.h harddisk.h text.h drv_wrapper.h fileio_read_base.h internal.h utils.h configfile.h mem_list.h fileio_write.h matrix.h xwin.h getopt.h text_hersey.h text_freetype.h text_type1_kerning.h text_type1_size.h text_hersey_data.h bsc1.h bsc2.h apl1.h bsc2d.h bsc3d.h apl3d.h utils2.h pers3d.h apl2.h cont1.h cont2.h spln.h rgbfunc.h unix.h textin.h inline.h osmesa.h guiitems.h gui_common.h libserver.h tcp_guiitems.h toolkit.h gsgltype.h text_texture.h
# register.h

HEADERF = gsglf.h iof.h

if BUILD_QT
GUIDIR = qt
SRCGTK = 
HEADERGTK = 
SRCQT = qt/qt_menuedit.cpp qt/qt_xwin.cpp qt/qt_gsglwindow.cpp qt/qt_drawarea.cpp qt/qt_utils.cpp  qt/qt_menu2d.cpp qt/qt_menu3d.cpp qt/qt_menupro.cpp qt/qt_tcp.cpp qt/qt_dialog.cpp qt/qt_anime.cpp qt/qt_guiitems.cpp
HEADERQT = qt/qt_menuedit.h qt/qt_xwin.h qt/qt_utils.h qt/qt_drawarea.h qt/qt_gsgltcp.h qt/qt_tcp.h qt/qt_guiitems.h qt/qt_gsglwindow.h.in qt/qt_gsglwindow.h.in qt/qt_dialog.h.in qt/qt_compat.h
nodist_libgsgl_la_SOURCES = qt/moc_qt_drawarea.cpp qt/moc_qt_gsglwindow.cpp qt/moc_qt_gsgltcp.cpp qt/moc_qt_dialog.cpp qt/moc_qt_guiitems.cpp qt/qt_dialog.h qt/qt_gsglwindow.h
else
GUIDIR = gtk
SRCGTK = gtk/gtk_menuedit.cpp gtk/gtk_gsglwindow.cpp gtk/gtk_base.c gtk/gtk_menu2d.cpp gtk/gtk_menu3d.cpp gtk/gtk_menupro.cpp gtk/gtk_config.cpp diacanvas.c gtk/gtk_guiitems.cpp gtk/gtk_tcp.cpp gtk/gtk_xwin.cpp gtk/gtk_anime.cpp
HEADERGTK = gtk/gtk_menu2d.h gtk/gtk_menu3d.h gtk/gtk_menupro.h gtk/gtk_base.h gtk/gtk_config.h diacanvas.h gtk/gtk_guiitems.h gtk/gtk_tcp.h gtk/gtk_xwin.h gtk/gtk_gsglwindow.h gtk/gtk_anime.h gtk/gtk_menuedit.h
SRCQT = 
HEADERQT = 
endif

qt/moc_qt_drawarea.cpp : qt/qt_drawarea.h
	$(MOC) qt/qt_drawarea.h -o qt/moc_qt_drawarea.cpp
qt/moc_qt_gsglwindow.cpp : qt/qt_gsglwindow.h
	$(MOC) qt/qt_gsglwindow.h -o qt/moc_qt_gsglwindow.cpp
qt/moc_qt_gsgltcp.cpp : qt/qt_gsgltcp.h
	$(MOC) qt/qt_gsgltcp.h -o qt/moc_qt_gsgltcp.cpp
qt/moc_qt_dialog.cpp : qt/qt_dialog.h
	$(MOC) qt/qt_dialog.h -o qt/moc_qt_dialog.cpp
qt/moc_qt_guiitems.cpp : qt/qt_guiitems.h
	$(MOC) qt/qt_guiitems.h -o qt/moc_qt_guiitems.cpp

EXTRA_DIST = gsprint.tp gsgl2mpeg.tp f77g.tp gsgl_header1.ps gsgl_header2.ps gsgl_header3.ps

MOC = @QT_MOC@
UIC = @QT_UIC@
AM_CFLAGS = @CFLAGS@ @GTK_CFLAGS@ @FT2_CFLAGS@ @X_CFLAGS@ -I$(includedir) -I../include -I$(GUIDIR)
AM_CXXFLAGS = @QT_CXXFLAGS@ $(AM_CFLAGS)

libgsgl_la_SOURCES = $(HEADERQT) $(HEADERGTK) $(SRCQT) $(SRCGTK) $(SRCSC) $(HEADERC) $(HEADERF) $(SRCSF)
libgsgl_la_LIBADD = @X_LIBS@ @X_PRE_LIBS@ @X_LIBS_X11@ @X_EXTRA_LIBS@ @LIBS@ @GTK_LIBS@ @QT_LIBS@ @FT2_LIBS@ @FLIBS@ @LOCAL_LIBGC_SRC@ @ZLIB_LIBS@ @LIBINTL@ 
libgsgl_la_LDFLAGS = -version-info $(GSGL_LIBTOOL)

bin_PROGRAMS = gsglview gsgl2ps gsglserver gsglanime2pages gsgl2raster

bin_SCRIPTS = gsglprint $(F77)gl gsgl2mpeg

gsglview_SOURCES = gsglview.c fileio_read.c fileio_read.h
gsglview_LDADD = libgsgl.la

gsgl2ps_SOURCES = gsgl2ps.c fileio_ps.c gsgl2ps.h fileio_read.c fileio_read.h
gsgl2ps_LDADD = libgsgl.la

gsgl2raster_SOURCES = gsgl2raster.c raster.c raster.h fileio_read.c fileio_read.h
gsgl2raster_LDADD = libgsgl.la @JPEG_LIBS@ @PNG_LIBS@

gsglserver_SOURCES = server.c libserver.c libserver.h fileio_read.c fileio_read.h
gsglserver_LDADD = libgsgl.la

gsglanime2pages_SOURCES = gsglanime2pages.c fileio_read.c fileio_read.h
gsglanime2pages_LDADD = libgsgl.la

PSHEADER1=$(libdir)/gsgl_header1.ps
PSHEADER2=$(libdir)/gsgl_header2.ps
PSHEADER3=$(libdir)/gsgl_header3.ps
AM_CPPFLAGS = -DLOCALEDIR=\"$(localedir)\" -DPSHEADER1=\"$(PSHEADER1)\" -DPSHEADER2=\"$(PSHEADER2)\" -DPSHEADER3=\"$(PSHEADER3)\"

gsglprint : gsprint.tp
	sed "s@BINPATH@$(bindir)@" gsprint.tp | \
	sed "s/LP/$(LP)/" > gsglprint

GSLIBS=-L$(libdir) -lgsgl
$(F77)gl : f77g.tp
	sed "s/FC/$(F77)/" f77g.tp | \
	sed "s@GSLIBS@$(GSLIBS)@" > $(F77)gl
gsgl2mpeg : gsgl2mpeg.tp
	sed "s@BINDIR@$(bindir)@" gsgl2mpeg.tp > gsgl2mpeg

install-exec-local : gsgl2ps gsgl2raster
	-rm -f $(bindir)/gsgl2eps
	-rm -f $(bindir)/gsgl2rastereps
	-rm -f $(bindir)/gsgl2png
	-rm -f $(bindir)/gsgl2jpeg
	ln $(bindir)/gsgl2ps $(bindir)/gsgl2eps
	ln $(bindir)/gsgl2raster $(bindir)/gsgl2rastereps
	ln $(bindir)/gsgl2raster $(bindir)/gsgl2png
	ln $(bindir)/gsgl2raster $(bindir)/gsgl2jpeg

install-data-local : gsgl_header1.ps gsgl_header2.ps gsgl_header3.ps
	$(LIBTOOL) --mode=install $(INSTALL_DATA) gsgl_header1.ps gsgl_header2.ps gsgl_header3.ps $(libdir)

distclean-local :
	-rm -f *~ qt/moc_*.cpp qt/qt_dialog.h qt/qt_gsglwindow.h

cleanf:
	-rm -f $(OBJF) *~

cleanc:
	-rm -f $(OBJC) *~
