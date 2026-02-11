#include "gsaf.h"
#include "gsgl.h"
#include "xwin.h"
#include "drv_gl.h"
#include "internal.h"
#include "ogl.h"
#include "displaylist.h"
#include "configfile.h"
#include "utils.h"
#include "drv_wrapper.h"
#include "device.h"
#ifdef USE_QT6
#include "qt_xwin.h"
#endif

const static GLfloat IndexColor[2][8][3] = {{{0.0, 0.0, 0.0},
					     {78.0/255.0, 78.0/255.0, 1.0},
					     {1.0, 78.0/255.0, 78.0/255.0},
					     {1.0, 78.0/255.0, 1.0},
					     {0.0, 1.0, 0.0},
					     {0.0, 1.0, 1.0},
					     {1.0, 1.0, 0.0},
					     {1.0, 1.0, 1.0}},
					    {{1.0, 1.0, 1.0},
					     {0.0, 215.0/255.0, 234.0/255.0},
					     {234.0/255.0, 215.0/255.0, 0.0},
					     {1.0, 0.0, 1.0},
					     {0.0, 195.0/255.0, 0.0},
					     {0.0, 0.0, 1.0},
					     {1.0, 0.0, 0.0},
					     {0.0, 0.0, 0.0}}};

void gsgl_gl_pageinfo ()
{
	/* have nothing to do */
}

void gsgl_gl_begin_header ()
{
	/* have nothing to do */
}

void gsgl_gl_end_header ()
{
	/* have nothing to do */
}

void gsgl_gl_vertex2 (float x, float y)
{
	glVertex2f (x, y);
}

void gsgl_gl_vertex3 (float x, float y, float z)
{
	glVertex3f (x, y, z);
}

void gsgl_gl_beginlines ()
{
	glBegin (GL_LINE_STRIP);
}

void gsgl_gl_endlines ()
{
	glEnd ();
}

void gsgl_gl_beginpolygon ()
{
	glBegin (GL_POLYGON);
}

void gsgl_gl_endpolygon ()
{
	glEnd ();
}

void gsgl_gl_beginrgbtriangle ()
{
	glBegin (GL_POLYGON);
}

void gsgl_gl_endrgbtriangle ()
{
	glEnd ();
}

void gsgl_gl_begin_triangles ()
{
	glBegin (GL_TRIANGLES);
}

void gsgl_gl_end_triangles ()
{
	glEnd ();
}

void gsgl_gl_begin_triangle_strip ()
{
	glBegin (GL_TRIANGLE_STRIP);
}

void gsgl_gl_end_triangle_strip ()
{
	glEnd ();
}

void gsgl_gl_begin_triangle_fan ()
{
	glBegin (GL_TRIANGLE_FAN);
}

void gsgl_gl_end_triangle_fan ()
{
	glEnd ();
}

void gsgl_gl_rgb (float r, float g, float b)
{
	glColor3f (r, g, b);
}

void gsgl_gl_color_index (int i, int bgmode)
{
	glColor3f (IndexColor[bgmode][i][0], 
                   IndexColor[bgmode][i][1], 
                   IndexColor[bgmode][i][2]);
}

void gsgl_gl_linewidth (float w /* cm */, gsglPage *p)
{
        float line_width /* pixel */;

	glPassThrough (GSGLINTERNAL_LINEWIDTH);
	glPassThrough (w);

        if (w == 0.0) {
                glLineWidth (1.0f);
	} else {
		line_width = w * ((GLfloat) gsgl_inq_window_width (p->win))/GSGL_PAPER_WIDTH;
                glLineWidth (line_width);
	}
}

void gsgl_gl_linewidth_index (int i)
{
	glPassThrough (GSGLINTERNAL_LINEWIDTH2);
	glPassThrough (MAX(1,i));
	glLineWidth ((GLfloat) MAX(1, i*0.5));
}

void gsgl_gl_normalmode ()
{
	glDisable (GL_DEPTH_TEST);
	glDisable (GL_POLYGON_OFFSET_FILL);
	glPassThrough ((GLfloat) GSGLINTERNAL_NORMAL_MODE);
}

void gsgl_gl_2dmode ()
{
	glDisable (GL_DEPTH_TEST);
	glDisable (GL_POLYGON_OFFSET_FILL);
	glPassThrough ((GLfloat) GSGLINTERNAL_2D_MODE);
}

void gsgl_gl_3dmode ()
{
	glEnable (GL_DEPTH_TEST);
	glEnable (GL_POLYGON_OFFSET_FILL);
	glPolygonOffset (0.5, 0.5);
	glPassThrough ((GLfloat) GSGLINTERNAL_3D_MODE);
}

void gsgl_gl_modelview_matrix (float matrix[16])
{
	int i;

	glLoadMatrixf (matrix);
	glPassThrough ((GLfloat) GSGLINTERNAL_MODELVIEW_MATRIX);
	for (i=0; i<16; i++)
		glPassThrough ((GLfloat) matrix[i]);
}

void gsgl_gl_projection_matrix (float matrix[16])
{
	int i;

	glMatrixMode (GL_PROJECTION);
	glPopMatrix ();
	glPushMatrix ();
	glMultMatrixf (matrix);
	glMatrixMode (GL_MODELVIEW);
	glPassThrough ((GLfloat) GSGLINTERNAL_PROJECTION_MATRIX);
	for (i=0; i<16; i++)
		glPassThrough ((GLfloat) matrix[i]);
}

void gsgl_gl_projection_type (int i)
{
}

void gsgl_gl_clip_enable_unmovable (int i, float eqn1[4])
{
	int j;
        GLdouble eqn[4];
                
	glLoadIdentity();

        for (j=0; j<4; j++)
                eqn[j] = (GLdouble) eqn1[j];
        
	switch (i) {
	case 0: 
                glClipPlane(GL_CLIP_PLANE0, eqn);
                glEnable(GL_CLIP_PLANE0);
		break;
	case 1:
                glClipPlane(GL_CLIP_PLANE1, eqn);
                glEnable(GL_CLIP_PLANE1);
		break;
	case 2:
                glClipPlane(GL_CLIP_PLANE2, eqn);
                glEnable(GL_CLIP_PLANE2);
		break;
	case 3:
                glClipPlane(GL_CLIP_PLANE3, eqn);
                glEnable(GL_CLIP_PLANE3);
		break;
        case 4:
                glClipPlane(GL_CLIP_PLANE4, eqn);
                glEnable(GL_CLIP_PLANE4);
		break;
        case 5:
                glClipPlane(GL_CLIP_PLANE5, eqn);
                glEnable(GL_CLIP_PLANE5);
		break;
	default:
		gsgl_warning ("gsgl_gl_clip_enable_unmovable", "i should be 0 <= i <= 5");
        }
}

void gsgl_gl_clip_enable_3d_fileonly (int i, float eqn1[4])
{
	int j;
        GLdouble eqn[4];
               
        for (j=0; j<4; j++)
                eqn[j] = (GLdouble) eqn1[j];
        
	switch (i) {
	case 0: 
                glClipPlane(GL_CLIP_PLANE0, eqn);
                glEnable(GL_CLIP_PLANE0);
		break;
	case 1:
                glClipPlane(GL_CLIP_PLANE1, eqn);
                glEnable(GL_CLIP_PLANE1);
		break;
	case 2:
                glClipPlane(GL_CLIP_PLANE2, eqn);
                glEnable(GL_CLIP_PLANE2);
		break;
	case 3:
                glClipPlane(GL_CLIP_PLANE3, eqn);
                glEnable(GL_CLIP_PLANE3);
		break;
        case 4:
                glClipPlane(GL_CLIP_PLANE4, eqn);
                glEnable(GL_CLIP_PLANE4);
		break;
        case 5:
                glClipPlane(GL_CLIP_PLANE5, eqn);
                glEnable(GL_CLIP_PLANE5);
		break;
	default:
		gsgl_warning ("gsgl_gl_clip_enable_3d_fileonly", "i should be 0 <= i <= 5");
        }

}

void gsgl_gl_clip_disable (int i /* 0 <= i <= 5 */) 
{
	switch (i) {
	case 0: 
                glDisable(GL_CLIP_PLANE0);
		break;
	case 1:
                glDisable(GL_CLIP_PLANE1);
		break;
        case 2:
                glDisable(GL_CLIP_PLANE2);
		break;
        case 3:
                glDisable(GL_CLIP_PLANE3);
		break;
        case 4:
                glDisable(GL_CLIP_PLANE4);
		break;
        case 5:
                glDisable(GL_CLIP_PLANE5);
		break;
	default:
		gsgl_warning ("gsgl_gl_clip_disable", "i should be 0 <= i <= 5");
        }
}

void gsgl_gl_depth3d (float depth)
{
	/* have nothing to do. */
}

void gsgl_gl_lighting_enable ()
{
        glEnable (GL_LIGHTING);
        glEnable (GL_COLOR_MATERIAL);
        glEnable (GL_LIGHT0);
        glEnable (GL_NORMALIZE);
	glEnable (GL_AUTO_NORMAL);
}

void gsgl_gl_lighting_disable ()
{
        glDisable (GL_LIGHTING);
        glDisable (GL_LIGHT0);
        /*glLightModeli (GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);*/
        glDisable (GL_COLOR_MATERIAL);
        glDisable (GL_NORMALIZE);
}

void gsgl_gl_separate_objects ()
{
	/* have nothing to do */
}

void gsgl_gl_normal (float x, float y, float z)
{
	glNormal3f (x, y, z);
}

void gsgl_gl_array_element (int i)
{
	glArrayElement (i);
}

void gsgl_gl_glarray_begin_polygon ()
{
	glBegin (GL_POLYGON);
}

void gsgl_gl_glarray_end_polygon ()
{
	glEnd ();
}

void gsgl_gl_glarray_begin_lines ()
{
	glBegin (GL_LINE_STRIP);
}

void gsgl_gl_glarray_end_lines ()
{
	glEnd ();
}

void gsgl_gl_glarray_vertex (int datatype, int size, unsigned int totalsize, float *buffer,
			     int nx, int ny, 
			     float *r, float *t,
			     float xmin, float xmax, float ymin, float ymax,
			     int *idata, float *fdata)
{
	glEnableClientState (GL_VERTEX_ARRAY);
	glVertexPointer (size, GL_FLOAT, 0, buffer);
}

void gsgl_gl_glarray_color (int size, unsigned int totalsize, float *buffer)
{
	if (size != 0 && totalsize != 0) {
		glEnableClientState (GL_COLOR_ARRAY);
		glColorPointer (size, GL_FLOAT, 0, buffer);
	}
}

void gsgl_gl_glarray_normal (int datatype, int size, unsigned int totalsize, float *buffer)
{
	glEnableClientState (GL_NORMAL_ARRAY);
	glNormalPointer (GL_FLOAT, 0, buffer);
}

void gsgl_gl_glarray_disable_vertex ()
{
	glDisableClientState (GL_VERTEX_ARRAY);
}

void gsgl_gl_glarray_disable_color ()
{
	glDisableClientState (GL_COLOR_ARRAY);
}

void gsgl_gl_glarray_disable_normal ()
{
	glDisableClientState (GL_NORMAL_ARRAY);
}

/* postscript */

void gsgl_gl_ps_gouraudonly_begin ()
{
	glPassThrough ((GLfloat) GSGLINTERNALPS_BEGIN_GOURAUD_ONLY);
}

void gsgl_gl_ps_gouraudonly_end ()
{
	glPassThrough ((GLfloat) GSGLINTERNALPS_END_GOURAUD_ONLY);
}

void gsgl_gl_ps_depth_by_deep ()
{
	glPassThrough ((GLfloat) GSGLINTERNALPS_DEPTH_BY_DEEP);
}

void gsgl_gl_ps_depth_by_shallow ()
{
	glPassThrough ((GLfloat) GSGLINTERNALPS_DEPTH_BY_SHALLOW);
}

void gsgl_gl_ps_depth_by_mean ()
{
	glPassThrough ((GLfloat) GSGLINTERNALPS_DEPTH_BY_MEAN);
}

void gsgl_gl_ps_depth_by_vectprod (float *x, float *y, float *z)
{
	glPassThrough ((GLfloat) GSGLINTERNALPS_DEPTH_BY_VECT_PROD);
	glPassThrough ((GLfloat) x[0]);
	glPassThrough ((GLfloat) y[0]);
	glPassThrough ((GLfloat) z[0]);
	glPassThrough ((GLfloat) x[1]);
	glPassThrough ((GLfloat) y[1]);
	glPassThrough ((GLfloat) z[1]);
	glPassThrough ((GLfloat) x[2]);
	glPassThrough ((GLfloat) y[2]);
	glPassThrough ((GLfloat) z[2]);
}

void gsgl_gl_ps_depth_by_vectprod2 (float *x, float *y, float *z,
				    unsigned int npoints, float *xp, float *yp, float *zp)
{
	int i;

	glPassThrough ((GLfloat) GSGLINTERNALPS_DEPTH_BY_VECT_PROD2);
	glPassThrough ((GLfloat) x[0]);
	glPassThrough ((GLfloat) y[0]);
	glPassThrough ((GLfloat) z[0]);
	glPassThrough ((GLfloat) x[1]);
	glPassThrough ((GLfloat) y[1]);
	glPassThrough ((GLfloat) z[1]);
	glPassThrough ((GLfloat) x[2]);
	glPassThrough ((GLfloat) y[2]);
	glPassThrough ((GLfloat) z[2]);
	glPassThrough ((GLfloat) (npoints+0.01));
	for (i=0; i<npoints; i++) {
		glPassThrough ((GLfloat) xp[i]);
		glPassThrough ((GLfloat) yp[i]);
		glPassThrough ((GLfloat) zp[i]);
	}
}

void gsgl_gl_ps_depth_by_point (float x, float y, float z)
{
	glPassThrough ((GLfloat) GSGLINTERNALPS_DEPTH_BY_POINT);
	glPassThrough ((GLfloat) x);
	glPassThrough ((GLfloat) y);
	glPassThrough ((GLfloat) z);
	/*fprintf (stderr, "drv_gl %f %f %f\n", x, y, z);*/
}

void gsgl_gl_beginanime (gsglPage *p)
{
#ifdef USE_QT6
	gsgl_qt_ensure_current ();
#endif
	if (!gsgl_doublebuffer_is_enabled (p->win)) gsgl_doublebuffer_enable (p->win);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glPushAttrib (GL_ALL_ATTRIB_BITS);
	glPushMatrix ();
	glLoadIdentity ();
	glPushMatrix ();
	glMatrixMode (GL_PROJECTION);
	glPushMatrix ();	
	glLoadIdentity ();
	glPushMatrix ();
	glMatrixMode (GL_MODELVIEW);
	
	if (gsgl_inq_bg_color (p->library) == GSGL_BACK_COLOR_WHITE)
		glColor3f (0.0, 0.0, 0.0);
	else if (gsgl_inq_bg_color (p->library) == GSGL_BACK_COLOR_BLACK)
		glColor3f (1.0, 1.0, 1.0);
	
	gsgl_lists_call_header (p);

	glPopAttrib ();
	glPopMatrix ();
	glPopMatrix ();
	glMatrixMode (GL_PROJECTION);
	glPopMatrix ();
	glPopMatrix ();
	glMatrixMode (GL_MODELVIEW);
}

void gsgl_gl_endanime (gsglPage *p)
{
#ifdef USE_QT6
	gsgl_qt_ensure_current ();
#endif
	gsgl_flush (p->win);
#ifdef USE_QT6
	/* Pump events so animation frames are presented while the app is busy. */
	gsgl_qt_process_events (1);
#endif
}

void gsgl_gl_pstext (int ifnts, int ifntr,
		     float chh, float chw, float chsp, float angl, float tilt,
		     float x, float y, int dim, unsigned int *iascs, int nmax)
{
	int i;
	float rgba[4];

	/*printf ("g %d %d %f %f %f %f %f %f %f %d\n",
	  ifnts, ifntr, chh, chw, chsp, angl, tilt, x, y, nmax);*/
	glPassThrough (GSGLINTERNAL_PSTEXT);
	glPassThrough (ifnts);
	glPassThrough (ifntr);
	glPassThrough (chh);
	glPassThrough (chw);
	glPassThrough (chsp);
	glPassThrough (angl);
	glPassThrough (tilt);
	glPassThrough (x);
	glPassThrough (y);
	glGetFloatv (GL_CURRENT_COLOR, rgba);
	glPassThrough (rgba[0]);
	glPassThrough (rgba[1]);
	glPassThrough (rgba[2]);
	/*printf ("%f %f %f\n", rgba[0], rgba[1], rgba[2]);*/
	glPassThrough (nmax);
	for (i=0; i<nmax; i++) {
		glPassThrough (iascs[i]);
		/*printf ("%d %d %d\n", nmax, i, iascs[i]);*/
	}
}

void gsgl_gl_enable_antialiasing (gsglPage *p)
{
	gsgl_enable_antialiasing (p->win);
}

void gsgl_gl_disable_antialiasing (gsglPage *p)
{
	gsgl_disable_antialiasing (p->win);
}
