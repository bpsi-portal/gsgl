#include "gsaf.h"
#include "matrix.h"
#include "configfile.h"
#include "device.h"
#include "ogl.h"
#include "utils.h"
#include "drv_gl.h"
#include "displaylist.h"
#include "xwin.h"
#ifdef USE_QT6
#include "qt_xwin.h"
#endif

void gsgl_enable_antialiasing (gsglWindow *w)
{
	if (w->Antialiasing) {
		glEnable (GL_POINT_SMOOTH);
		glEnable (GL_LINE_SMOOTH);
		glEnable (GL_BLEND);
		glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glHint (GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
	}
	w->AntialiasingNow = GSGL_TRUE;
}

void gsgl_disable_antialiasing (gsglWindow *w)
{
	if (w->Antialiasing) {
		glDisable (GL_POINT_SMOOTH);
		glDisable (GL_LINE_SMOOTH);
		/*glDisable (GL_BLEND);*/
	}
	w->AntialiasingNow = GSGL_FALSE;
}

gsglboolean gsgl_inq_antialiasing_now (gsglWindow *w)
{
	return w->AntialiasingNow;
}

void gsgl_init_gl_once_in_program (gsglWindow *w)
{
	float light_position0[] = {10.0, 10.0, 10.0, 0.0};
        float light_ambient[] = {0.3, 0.3, 0.3, 1.0};
        float light_diffuse[] = {0.5, 0.5, 0.5, 0.55};
        float light_specular[] = {0.15, 0.15, 0.15, 1.0};
	char *str;

#ifdef USE_QT6
	gsgl_qt_ensure_current ();
#endif
	gsgl_doublebuffer_disable (w);
	/*gsgl_doublebuffer_enable ();*/

	glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
	glDepthFunc (GL_LESS);
	glClearDepth (1.0);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();
	glPushMatrix (); /* 1st, 2nd stack are identity matrix */
	
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	glPushMatrix (); /* 1st, 2nd stack are identity matrix */
	glMatrixMode (GL_MODELVIEW);
	
	/* Lighting */
	glLightModeli (GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
        glLightfv (GL_LIGHT0, GL_POSITION, light_position0);
        glLightfv (GL_LIGHT0, GL_AMBIENT, light_ambient);
        glLightfv (GL_LIGHT0, GL_DIFFUSE, light_diffuse);
        glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, 100.0);
        glMaterialfv (GL_FRONT_AND_BACK, GL_SPECULAR,  light_specular);
        glColorMaterial (GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

	/* clear the screen */
	if (gsgl_inq_bg_color (w->library) == GSGL_BACK_COLOR_WHITE) {
		glClearColor(1.0, 1.0, 1.0, 1.0);
		glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
	} else {
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	}

	/* antialiasing */
	str = getenv ("GSGL_DISABLE_ANTIALIASING");
        if (str && strlen (str) > 0) {
		w->Antialiasing = GSGL_FALSE;
	} else if (gsgl_inq_devicetype (w->library) != GSGL_DEVICE_PS) {
		w->Antialiasing = GSGL_TRUE;
	} else {
		w->Antialiasing = GSGL_FALSE;
	}

	/* line stipple */
	str = getenv ("GSGL_DISABLE_LINESTIPPLE");
#if defined(USE_QT6) && defined(__APPLE__)
	/* Apple's OpenGL falls back to SW for line stipple; disable by default. */
	if (!str) str = "1";
#endif
        if (str && strlen (str) > 0) {
	} else {
		glEnable(GL_LINE_STIPPLE);
	}
	
	glViewport(0, 0, (GLsizei) gsgl_inq_window_width (w), (GLsizei) gsgl_inq_window_height (w));
}

void gsgl_init_gl_every_page (gsglPage *p)
{
#ifdef USE_QT6
	gsgl_qt_ensure_current ();
#endif
	glDisable(GL_CLIP_PLANE0);
	glDisable(GL_CLIP_PLANE1);
	glDisable(GL_CLIP_PLANE2);
	glDisable(GL_CLIP_PLANE3);
	glDisable(GL_CLIP_PLANE4);
	glDisable(GL_CLIP_PLANE5);

	glLineWidth (GSGLBASIC_LINEWIDTH);
	gsgl_gl_lighting_disable ();

	if (gsgl_inq_bg_color (p->library) == GSGL_BACK_COLOR_WHITE) {
		glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
	} else {
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	}

	glMatrixMode (GL_PROJECTION);
	glPopMatrix ();
	glLoadIdentity ();
	glPushMatrix ();
	glMatrixMode (GL_MODELVIEW);
	glPopMatrix ();
	glLoadIdentity ();
	glPushMatrix ();
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	p->win->AntialiasingNow = GSGL_FALSE;
}

void gsgl_erase (gsglPage *p)
{
        if (gsgl_inq_bg_color (p->library) == GSGL_BACK_COLOR_WHITE) {
                glClearColor (1.0, 1.0, 1.0, 1.0);
        } else {
                glClearColor (0.0, 0.0, 0.0, 0.0);
        }
}

void gsgl_doublebuffer_is_available (gsglLibrary *lib)
{
	lib->DoubleBuffer_is_available = GSGL_TRUE;
}

void gsgl_doublebuffer_is_unavailable (gsglLibrary *lib)
{
	lib->DoubleBuffer_is_available = GSGL_FALSE;	
}

gsglboolean gsgl_doublebuffer_is_enabled (gsglWindow *w)
{
	return w->DoubleBuffer;
}

void gsgl_doublebuffer_enable (gsglWindow *w)
{
	if (w->library->DoubleBuffer_is_available) {
#ifdef USE_QT6
		w->DoubleBuffer = GL_TRUE;
		return;
#else
		glDrawBuffer (GL_BACK);
		glReadBuffer (GL_BACK);
		w->DoubleBuffer = GL_TRUE;
#endif
	}
}

void gsgl_doublebuffer_disable (gsglWindow *w)
{
	if (w->library->DoubleBuffer_is_available) {
#ifdef USE_QT6
		w->DoubleBuffer = GL_FALSE;
		return;
#else
		glDrawBuffer (GL_FRONT);
		glReadBuffer (GL_FRONT);
		w->DoubleBuffer = GL_FALSE;
#endif
	}
}

void gsgl_redraw (gsglPage *p)
{
	/*glXWaitX ();*/
#ifdef USE_QT6
	gsgl_qt_ensure_current ();
#endif

	glMatrixMode (GL_PROJECTION);
	glPopMatrix ();
	glLoadIdentity ();
	glPushMatrix ();
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();

	gsgl_gl_lighting_disable ();
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/*if (Antialiasing)
	  gsgl_enable_antialiasing ();*/

	gsgl_lists_call (gsgl_displaylist_inq_start (p), 0, p);
	gsgl_flush (p->win);
}

void gsgl_free_select_buffer (gsglWindow *w)
{
	if (w->Select_buffer != NULL) 
		gsgl_free (w->Select_buffer);
	w->Select_buffer = NULL;
}

void gsgl_allocate_select_buffer (gsglWindow *w)
{
	w->Select_buffer = (GLuint *) gsgl_malloc_atomic (sizeof (GLuint) * 10000);
}

GLuint * gsgl_inq_select_buffer (gsglWindow *w)
{
	return w->Select_buffer;
}

int gsgl_redraw_with_name (int x, int y, int w, int h, gsglPage *p)
{
	GLint viewport[4];
	GLdouble width, height;

#ifdef USE_QT6
	gsgl_qt_ensure_current ();
#endif
	
	if (w < 7)
		width = 7.0;
	else
		width = w;

	if (h < 7)
		height = 7.0;
	else
		height = h;
	
	gsgl_displaylist_remake (p);
	gsgl_gl_lighting_disable ();
	
	/*glXWaitX ();*/

	glMatrixMode (GL_PROJECTION);
	glPopMatrix ();
	glLoadIdentity ();
	glGetIntegerv (GL_VIEWPORT, viewport);
	gluPickMatrix ((GLdouble) x, (GLdouble) (viewport[3]-y), width, height, viewport);
	glPushMatrix ();
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();

	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/*fprintf (stderr, "fixed gsgl_redraw_with_name\n");*/
	gsgl_free_select_buffer (p->win);
	gsgl_allocate_select_buffer (p->win);

	glSelectBuffer (10000, gsgl_inq_select_buffer (p->win));
	(void) glRenderMode (GL_SELECT);

	glInitNames ();
	glPushName (0);
	
	gsgl_lists_call (gsgl_displaylist_inq_start (p), GSGL_TRUE, p);
	
	/*gsgl_flush();*/
	/*glFlush ();*/

	return glRenderMode (GL_RENDER);
}
