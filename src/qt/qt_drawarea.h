#ifndef __qt_drawarea_h
#define __qt_drawarea_h
#include "qt_compat.h"
#ifdef USE_QT3
#    include <qgl.h>
#elif defined USE_QT4
#    include <QtOpenGL/QGLWidget>
#endif
#include "gsgltype.h"
class QKeyEvent;
class QSize;

class GLDrawArea : public QGLWidget
{
Q_OBJECT

public:
        GLDrawArea (gsglWindow *, const QGLFormat &format, QWidget *parent=0, const char *name=0, const QGLWidget *shareWidget=0, WFLAGS f=0);
        ~GLDrawArea ();
	void swapBuffers ();
	void set_page (gsglPage *);
	gsglLibrary *lib;
	gsglWindow *win;
	gsglPage *page;

protected:
        void initializeGL ();
        void paintGL ();
        void resizeGL (int w, int h);
	void keyPressEvent (QKeyEvent *e);
	void keyReleaseEvent (QKeyEvent *e);
	void mousePressEvent (QMouseEvent *e);
	void mouseReleaseEvent (QMouseEvent *e);
	void mouseMoveEvent (QMouseEvent *e);

private:
	int xkey;
	int ykey;
	int key_is_pressed;
	int Xstart;
	int Ystart;
};

#endif
