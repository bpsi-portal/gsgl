#ifndef __qt_drawarea_h
#define __qt_drawarea_h
#include "qt_compat.h"
#include <QOpenGLWidget>
#include "gsgltype.h"
class QKeyEvent;
class QSize;

class GLDrawArea : public QOpenGLWidget
{
Q_OBJECT

public:
        GLDrawArea (gsglWindow *, QWidget *parent=0, WFLAGS f=WFLAGS());
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
