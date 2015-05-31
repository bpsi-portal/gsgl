#include "gsaf.h"
#include "base.h"
#include "drv_wrapper.h"
#include "text.h"
#include "textin.h"
#include "text_freetype.h"
#include "utils.h"
#include "displaylist.h"
#include "matrix.h"

/* render actually */

static gsgllist *gsft_gsglpoint_append (gsgllist *list, float x, float y)
{
	gsglvec2f *new;

	new = (gsglvec2f *) gsgl_malloc (sizeof(gsglvec2f));
	new->x = x;
	new->y = y;

	return gsgl_list_append (list, new);
}

static gsgllist *gsft_vectorize_line (gsgllist *list, float x, float y)
{
	return gsft_gsglpoint_append (list, x, y);
}

#define B20(t) ((1-t)*(1-t))
#define B21(t) (2.0*t*(1-t))
#define B22(t) (t*t)

#define B30(t) ((1-t)*(1-t)*(1-t))
#define B31(t) (3.0*t*(1-t)*(1-t))
#define B32(t) (3.0*t*t*(1-t))
#define B33(t) (t*t*t)

static gsgllist *gsft_vectorize_conic_bezier (gsgllist *list, float ctrlpoints[3][2], gsglPage *p)
{
	int i;
	float x, y, t;
	int node = gsft_inq_node_num (p);
	
	for (i = 1; i <= node; i++) {
		t = (float) i / node;
		x =	+B20(t)*ctrlpoints[0][0]
			+B21(t)*ctrlpoints[1][0]
			+B22(t)*ctrlpoints[2][0];
		y =	+B20(t)*ctrlpoints[0][1]
			+B21(t)*ctrlpoints[1][1]
			+B22(t)*ctrlpoints[2][1];
		list = gsft_gsglpoint_append (list, x, y);
	}
	return list;
}

static gsgllist *gsft_vectorize_cubic_bezier (gsgllist *list, float ctrlpoints[4][2], gsglPage * p)
{
	int i;
	float x, y, t;
	int node = gsft_inq_node_num (p);
	
	for (i = 1; i <= node; i++) {
		t = (float) i / node;
		x = 	+B30(t)*ctrlpoints[0][0]
			+B31(t)*ctrlpoints[1][0]
			+B32(t)*ctrlpoints[2][0]
			+B33(t)*ctrlpoints[3][0];
		y =	+B30(t)*ctrlpoints[0][1]
			+B31(t)*ctrlpoints[1][1]
			+B32(t)*ctrlpoints[2][1]
			+B33(t)*ctrlpoints[3][1];
		list = gsft_gsglpoint_append (list, x, y);
	}
	return list;
}

#ifdef HAVE_FREETYPE2
static gsgllist *gsft_contour_vectorizer (int jmin, int jmax, FT_Outline outline,
					  float ox, float oy, float pixel2cm,
					  gsglPage *p)
{
	int pre_tag;
	float x, y, xp, yp, xpp, ypp, xppp, yppp;
	int j;
	float ctrlpoints [4][2];
	gsgllist *list = NULL;

	pre_tag = FT_Curve_Tag_On;
	x = outline.points[jmin].x / pixel2cm + ox;
	y = outline.points[jmin].y / pixel2cm + oy;
	xp = 0.0;
	yp = 0.0;
	xpp = 0.0;
	ypp = 0.0;
	list = gsft_gsglpoint_append (list, x, y);

	for (j = jmin + 1;j <= jmax; j++) {
		xppp = xpp;
		yppp = ypp;
		xpp = xp;
		ypp = yp;
		xp = x;
		yp = y;
		x = outline.points[j].x / pixel2cm + ox;
		y = outline.points[j].y / pixel2cm + oy;
		switch (FT_CURVE_TAG (outline.tags[j])) {
		case FT_Curve_Tag_Conic:
			if (pre_tag == FT_Curve_Tag_Conic) {

				ctrlpoints[0][0] = xpp;
				ctrlpoints[0][1] = ypp;
				ctrlpoints[1][0] = xp;
				ctrlpoints[1][1] = yp;
				ctrlpoints[2][0] = 0.5 * (x + xp);
				ctrlpoints[2][1] = 0.5 * (y + yp);
				
				list = gsft_vectorize_conic_bezier (list, ctrlpoints, p);
				
				xp = 0.5 * (x + xp);
				yp = 0.5 * (y + yp);
				
				pre_tag = FT_Curve_Tag_Conic;
			} else if (pre_tag == FT_Curve_Tag_On) {
				pre_tag = FT_Curve_Tag_Conic;
			}
			break;
		case FT_Curve_Tag_Cubic:
			if (pre_tag == FT_Curve_Tag_On) {
				pre_tag = FT_Curve_Tag_Cubic;
			} else if (pre_tag == FT_Curve_Tag_Cubic) {
				pre_tag = FT_Curve_Tag_Cubic;
			}
			break;
		case FT_Curve_Tag_On:
			if (pre_tag == FT_Curve_Tag_On) {
				list = gsft_vectorize_line (list, x, y);
			} else if (pre_tag == FT_Curve_Tag_Conic) {

				ctrlpoints[0][0] = xpp;
				ctrlpoints[0][1] = ypp;
				ctrlpoints[1][0] = xp;
				ctrlpoints[1][1] = yp;
				ctrlpoints[2][0] = x;
				ctrlpoints[2][1] = y;
				
				list = gsft_vectorize_conic_bezier (list, ctrlpoints, p);
			} else if (pre_tag == FT_Curve_Tag_Cubic) {

				ctrlpoints[0][0] = xppp;
				ctrlpoints[0][1] = yppp;
				ctrlpoints[1][0] = xpp;
				ctrlpoints[1][1] = ypp;
				ctrlpoints[2][0] = xp;
				ctrlpoints[2][1] = yp;
				ctrlpoints[3][0] = x;
				ctrlpoints[3][1] = y;

				list = gsft_vectorize_cubic_bezier (list, ctrlpoints, p);
			}

			pre_tag = FT_Curve_Tag_On;
			break;
		default:
			break;
		}
	}

	xppp = xpp;
	yppp = ypp;
	xpp = xp;
	ypp = yp;
	xp = x;
	yp = y;
	x = outline.points[jmin].x / pixel2cm + ox;
	y = outline.points[jmin].y / pixel2cm + oy;

	if (pre_tag == FT_Curve_Tag_On) {
		list = gsft_vectorize_line (list, x, y);
	} else if (pre_tag == FT_Curve_Tag_Conic) {
		ctrlpoints[0][0] = xpp;
		ctrlpoints[0][1] = ypp;
		ctrlpoints[1][0] = xp;
		ctrlpoints[1][1] = yp;
		ctrlpoints[2][0] = x;
		ctrlpoints[2][1] = y;
		
		list = gsft_vectorize_conic_bezier (list, ctrlpoints, p);
	} else if (pre_tag == FT_Curve_Tag_Cubic) {
		ctrlpoints[0][0] = xppp;
		ctrlpoints[0][1] = yppp;
		ctrlpoints[1][0] = xpp;
		ctrlpoints[1][1] = ypp;
		ctrlpoints[2][0] = xp;
		ctrlpoints[2][1] = yp;
		ctrlpoints[3][0] = x;
		ctrlpoints[3][1] = y;
		
		list = gsft_vectorize_cubic_bezier (list, ctrlpoints, p);
	}
	if (list == NULL) gsgl_error ("gsft_contour_vectorizer", "list == NULL");
	return list;
}
#endif

void gsft_render_vectorized_font (gsgloutline outline, float ox, float oy, 
				  float pixel2cm, int dimension, gsglPage *p) 
{
	int i;
	gsgllist *contours, *points;
	gsglvec2f *point;

	contours = outline.contours;
	for (i=0; i<outline.n_contours; i++) {

		points = contours->data;
		
		point = (gsglvec2f *) points->data;
		gsgl_move (point->x, point->y, p);
		while (points) {
			point = (gsglvec2f *) points->data;
			if (dimension == 1)
				gsgl_draw (point->x, point->y, p);
			else if (dimension == 2)
				gsgl_draw_text2d (point->x, point->y, p);
			else if (dimension == 3)
				gsgl_draw_text3d (point->x, point->y, p);

			points = points->next;
		}
		contours = contours->next;
	}
}

#ifndef CALLBACK 
#define CALLBACK
#endif

static void CALLBACK beginCallback (GLenum which, gsgllist *list)
{
	int i;
	int *dimension = list->data;
	gsglPage *p;

	list = list->next;
	p = list->data;

	gsgl_coordinate_inq (&i, p);
	if (*dimension == 1) {
		if (i != GSGL_COORD_NORMAL) gsgl_normalmode (p);
		gsgl_displaylist_begin (GSGL_LIST_2DOBJ, p);
	} else if (*dimension == 2) {
		if (i != GSGL_COORD_TEXT2D) gsgl_text2dmode (p);
		gsgl_displaylist_begin (GSGL_LIST_2DOBJ, p);
	} else if (*dimension == 3) {
		if (i != GSGL_COORD_TEXT3D) gsgl_text3dmode (p);
		gsgl_displaylist_begin (GSGL_LIST_3DOBJ, p);
	} else {
		gsgl_warning ("beginCallback", "dimension = %d", *dimension);
	}

	p->text->Triangle_type = which;

	switch (which) {
	case GL_TRIANGLES:
		gsgl_drv_begin_triangles (p);
		gsgl_drv_normal (0.0, 0.0, -1.0, p);
		break;
	case GL_TRIANGLE_STRIP:
		gsgl_drv_begin_triangle_strip (p);
		gsgl_drv_normal (0.0, 0.0, -1.0, p);
		break;
	case GL_TRIANGLE_FAN:
		gsgl_drv_begin_triangle_fan (p);
		gsgl_drv_normal (0.0, 0.0, -1.0, p);
		break;
	default:
		gsgl_warning ("beginCallback", 
			      "neither triangles nor triangle_stip nor triangle_fan");
	}
}

static void CALLBACK errorCallback (GLenum errorCode)
{
	const GLubyte *estring;
	
	estring = gluErrorString (errorCode);
	gsgl_error ("errorCallback", "Tessellation Error: %s", estring);
}

static void CALLBACK endCallback (gsgllist *list)
{
	gsglPage *p;
	list = list->next;
	p = list->data;

	switch (p->text->Triangle_type) {
	case GL_TRIANGLES:
		gsgl_drv_end_triangles (p);
		break;
	case GL_TRIANGLE_STRIP:
		gsgl_drv_end_triangle_strip (p);
		break;
	case GL_TRIANGLE_FAN:
		gsgl_drv_end_triangle_fan (p);
		break;
	default:
		gsgl_warning ("endCallback", 
			      "neither triangles nor triangle_stip nor triangle_fan");
	}
}

static void CALLBACK vertexCallback (GLvoid *vertex, gsgllist *list)
{
	gsglPage *p;
	const GLdouble *pointer;
	list = list->next;
	p = list->data;

	pointer = (GLdouble *) vertex;
	gsgl_drv_vertex2 ((GLfloat) pointer[0], (GLfloat) pointer[1], p);
}

static void CALLBACK combineCallback (GLdouble coords[3], GLdouble *vertex_data[4],
				      GLfloat weight[4], GLdouble **dataOut)
{
	GLdouble *vertex;

	vertex = (GLdouble *) gsgl_malloc_atomic (3 * sizeof(GLdouble));
	
	vertex[0] = coords[0];
	vertex[1] = coords[1];
	vertex[2] = coords[2];
	*dataOut = vertex;
}

void gsft_render_polygonized_font (gsgloutline outline, float org_x, float org_y,
				   float pixel2cm, int dimension, gsglPage *p) 
{
	int i, j;
	gsgllist *contours, *points;
	gsglvec2f *point;
	double *dpoints = NULL;
#ifdef HAVE_TESS
	GLUtesselator *tobj;
	gsgllist *list = NULL;

	tobj = gluNewTess();
	gluTessCallback(tobj, GLU_TESS_VERTEX_DATA, (GLvoid (*)()) vertexCallback);
	gluTessCallback(tobj, GLU_TESS_BEGIN_DATA, (GLvoid (*)()) beginCallback);
	gluTessCallback(tobj, GLU_TESS_END_DATA, (GLvoid (*)()) endCallback);
	gluTessCallback(tobj, GLU_TESS_ERROR, (GLvoid (*)()) errorCallback);
	gluTessCallback(tobj, GLU_TESS_COMBINE, (GLvoid (*)()) combineCallback);

	list = gsgl_list_append (list, &dimension);
	list = gsgl_list_append (list, p);

	gluTessBeginPolygon(tobj, list);

	contours = outline.contours;
	for (i=0; i<outline.n_contours; i++) {
		
		points = contours->data;
		j = gsgl_list_count (points);
		dpoints = (double *) gsgl_malloc_atomic (sizeof (double)*3*(j+1));

		j = 0;
		gluTessBeginContour(tobj);
		while (points) {
			point = (gsglvec2f *) points->data;
			
			dpoints[j*3+0] = point->x;
			dpoints[j*3+1] = point->y;
			dpoints[j*3+2] = 0.0;
			gluTessVertex(tobj, &dpoints[j*3], &dpoints[j*3]);

			points = points->next;
			j++;
		}
		gluTessEndContour(tobj);
		contours = contours->next;
	}

	gluTessEndPolygon(tobj);
	gluDeleteTess (tobj);
#endif
}

#ifdef HAVE_FREETYPE2
void gsft_get_gsgloutline_from_ft_outline (FT_Outline outline1, float org_x, float org_y, 
					   float pixel2cm, gsgloutline *outline2, gsglPage *p) 
{
	int i;
	int jmin, jmax;
	gsgllist *list = NULL;

	outline2->n_contours = outline1.n_contours;
	outline2->contours = NULL;

	for (i=0; i<outline1.n_contours; i++) {
		if (i == 0) jmin = 0;
		else jmin = outline1.contours[i-1] + 1;
		jmax = outline1.contours[i];
		
		list = gsft_contour_vectorizer (jmin, jmax, outline1, org_x, org_y, pixel2cm, p);

		outline2->contours = gsgl_list_append (outline2->contours, list);
	}
}
#endif
