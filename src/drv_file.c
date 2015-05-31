/* drivers using fileformat (mem, harddisk, tcpip) */
#include "gsaf.h"
#include "gsgl.h"
#include "displaylist.h"
#include "fileformat.h"
#include "device.h"
#include "utils.h"
#include "drv_file.h"

#define FACT16BIT 65535.0

/**************************************/
/*  Push a float to the file format   */
/**************************************/
void gsgl_file_push_float (float x, void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *p)
{
	int i, j;
	float a, b;
	/*float out;
	  
	  unsigned int u;
	  u = gsgl_float_to_int4_ieee754 (x);
	  out = gsgl_int4_to_float_ieee754 (u >> 16, u & 65535);
	  fprintf (stderr, "in = %f, out=%f\n", x, out);*/

	if (FACT16BIT*x == 0.0) {
		i = 0; b = 0.0; j=0;
		gsgl_push (0, p);
		gsgl_push (0, p);    
	} else if (x > 0.0) {
		/* exponent part */
		a = log10(x);
		if (a < 0.0) {
			i = -((int) a);
			j = i + 16383;
			gsgl_push(j, p);
			/* significand */
			b = x * pow (10.0, (double) i);
			gsgl_push((unsigned int) gsgl_convert_0to1_to_2byte (b), p);
		} else {
			i = ((int) a)+1;
			j = i;
			gsgl_push(j, p);
			/* significand */
			b = x * pow (10.0, (double) -i);
			gsgl_push((unsigned int) gsgl_convert_0to1_to_2byte (b), p);
		}
		/* significand */
	} else if (x < 0.0) {
		/* exponent part */
		a = log10(-x);
		if (a < 0.0) {
			i = -((int) a);
			j = i+49151;
			gsgl_push(j, p);
			/* significand */
			b = -x * pow (10.0, (double) i);
			gsgl_push((unsigned int) gsgl_convert_0to1_to_2byte (b), p);
		} else {
			i = ((int) a)+1;
			j = i+32767;
			gsgl_push(j, p);
			/* significand */
			b = -x * pow (10.0, (double) -i);
			gsgl_push((unsigned int) gsgl_convert_0to1_to_2byte (b), p);
		}
	}
}

void gsgl_file_push_float_754 (float x, void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *p)
{
	unsigned int i;

	i = gsgl_float_to_int4_ieee754 (x);
	gsgl_push ((i >> 16) % 65536, p);
	gsgl_push (i % 65536, p);
	/*printf ("i=%d, %d %d\n", i, (i >> 16) % 65536, (i >> 0) % 65536);*/
}

void gsgl_file_pageinfo (void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *p)
{
	gsgl_push (GSGL_FILE_BEGIN_PAGEINFO, p);
	gsgl_push (GSGL_FILE_PAGE_NUMBER, p);
	gsgl_push (gsgl_inq_page_number (p->library), p);
	gsgl_push (GSGL_FILE_PAGE_SIZE, p);
	gsgl_push (0, p);
	gsgl_push (0, p);
	gsgl_push (GSGL_FILE_END_PAGEINFO, p);
}

void gsgl_file_begin_header (void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *p)
{
	gsgl_push (GSGL_FILE_BEGIN_HEAD, p);
}

void gsgl_file_end_header (void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *p)
{
	gsgl_push (GSGL_FILE_END_HEAD, p);
	gsgl_push (GSGL_FILE_BEGIN_PAGE, p);
}

void gsgl_file_vertex2 (float x, float y, void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *p)
{
	gsgl_push (GSGL_FILE_VERTEX2, p);
	/* We can not normalize these values */
	gsgl_file_push_float (x, gsgl_push, p);
	gsgl_file_push_float (y, gsgl_push, p);
}

void gsgl_file_vertex3 (float x, float y, float z, void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *p)
{
	gsgl_push (GSGL_FILE_VERTEX3, p);
	/* We can not normalize these values */
	gsgl_file_push_float (x, gsgl_push, p);
	gsgl_file_push_float (y, gsgl_push, p);
	gsgl_file_push_float (z, gsgl_push, p);
}

void gsgl_file_beginlines (void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *p)
{
	gsgl_push (GSGL_FILE_BEGIN_LINE, p);
}

void gsgl_file_endlines (void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *p)
{
	gsgl_push (GSGL_FILE_END_LINE, p);
}

void gsgl_file_beginpolygon (void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *p)
{
	gsgl_push (GSGL_FILE_BEGIN_POLY, p);
}

void gsgl_file_endpolygon (void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *p)
{
	gsgl_push (GSGL_FILE_END_POLY, p);
}

void gsgl_file_beginrgbtriangle (void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *p)
{
	gsgl_push (GSGL_FILE_BEGIN_RGBTRG, p);
}

void gsgl_file_endrgbtriangle (void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *p)
{
	gsgl_push (GSGL_FILE_END_RGBTRG, p);
}

void gsgl_file_begin_triangles (void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *p)
{
	gsgl_push (GSGL_FILE_BEGIN_TRIANGLES, p);
}

void gsgl_file_end_triangles (void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *p)
{
	gsgl_push (GSGL_FILE_END_TRIANGLES, p);
}

void gsgl_file_begin_triangle_strip (void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *p)
{
	gsgl_push (GSGL_FILE_BEGIN_TRIANGLE_STRIP, p);
}

void gsgl_file_end_triangle_strip (void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *p)
{
	gsgl_push (GSGL_FILE_END_TRIANGLE_STRIP, p);
}

void gsgl_file_begin_triangle_fan (void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *p)
{
	gsgl_push (GSGL_FILE_BEGIN_TRIANGLE_FAN, p);
}

void gsgl_file_end_triangle_fan (void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *p)
{
	gsgl_push (GSGL_FILE_END_TRIANGLE_FAN, p);
}

void gsgl_file_normal (float x, float y, float z, void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *p)
{
	gsgl_push (GSGL_FILE_NORMAL, p);
	gsgl_file_push_float (x, gsgl_push, p);
	gsgl_file_push_float (y, gsgl_push, p);
	gsgl_file_push_float (z, gsgl_push, p);
}

void gsgl_file_rgb (float r, float g, float b, void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *p)
{
	unsigned int red, green, blue;

	red = gsgl_convert_0to1_to_2byte (r);
	green = gsgl_convert_0to1_to_2byte (g);
	blue = gsgl_convert_0to1_to_2byte (b);

	gsgl_push (GSGL_FILE_RGB, p);
	gsgl_push (red, p);
	gsgl_push (green, p);
	gsgl_push (blue, p);
}

void gsgl_file_color_index (int i, int bgmode, void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *p)
{
	gsgl_push (GSGL_FILE_INDEXCOLOR, p);
	gsgl_push (i, p);
}

void gsgl_file_linewidth (float w, void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *p)
{
	unsigned int linewidth;

	linewidth = gsgl_convert_0to1_to_2byte (w/GSGL_PAPER_WIDTH);
	
	gsgl_push (GSGL_FILE_LINEWIDTH, p);
	gsgl_push (linewidth, p);
}

void gsgl_file_linewidth_index (int i, void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *p)
{
	gsgl_push (GSGL_FILE_LINEWIDTH2, p);
	gsgl_push (i, p);
}

void gsgl_file_normalmode (void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *p)
{
	gsgl_push (GSGL_FILE_MODE_NORMAL, p);
}

void gsgl_file_2dmode (void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *p)
{
	gsgl_push (GSGL_FILE_MODE_2D, p);
}

void gsgl_file_3dmode (void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *p)
{
	gsgl_push (GSGL_FILE_MODE_3D, p);
}

void gsgl_file_modelview_matrix (GLfloat matrix[16], void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *p)
{
	int i;

	gsgl_push (GSGL_FILE_MODELVIEW, p);
	for (i=0; i<16; i++)
		gsgl_file_push_float (matrix[i], gsgl_push, p);
}

void gsgl_file_projection_matrix (GLfloat matrix[16], void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *p)
{
	int i;

	gsgl_push (GSGL_FILE_PROJECTION, p);
	for (i=0; i<16; i++)
		gsgl_file_push_float (matrix[i], gsgl_push, p);
}

void gsgl_file_projection_type (int i, void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *p)
{
	gsgl_push (GSGL_FILE_PROJECTIONTYPE, p);
	gsgl_push (i, p);
}

void gsgl_file_clip_enable_unmovable (int i, float eqn[4], void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *p)
{
	gsgl_push (GSGL_FILE_CLIP_UNMOVABLE, p);
	gsgl_push (GSGL_FILE_CLIP_NUM, p);
	gsgl_push (i, p);
	gsgl_push (GSGL_FILE_CLIP_COEFF, p);
	gsgl_file_push_float (eqn[0], gsgl_push, p);
	gsgl_file_push_float (eqn[1], gsgl_push, p);
	gsgl_file_push_float (eqn[2], gsgl_push, p);
	gsgl_file_push_float (eqn[3], gsgl_push, p);
}

void gsgl_file_clip_enable_3d_fileonly (int i, float eqn[4], void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *p)
{
	gsgl_push (GSGL_FILE_CLIP_3D_FILEONLY, p);
	gsgl_push (GSGL_FILE_CLIP_NUM, p);
	gsgl_push (i, p);
	gsgl_push (GSGL_FILE_CLIP_COEFF, p);
	gsgl_file_push_float (eqn[0], gsgl_push, p);
	gsgl_file_push_float (eqn[1], gsgl_push, p);
	gsgl_file_push_float (eqn[2], gsgl_push, p);
	gsgl_file_push_float (eqn[3], gsgl_push, p);
}

void gsgl_file_clip_disable (int i, void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *p)
{
	gsgl_push (GSGL_FILE_DEL_CLIP_PLANE, p);
	gsgl_push (GSGL_FILE_DEL_CLIP_NUM, p);
	gsgl_push (i, p);
}

void gsgl_file_depth3d (float depth, void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *p)
{
	gsgl_push (GSGL_FILE_3D_3, p);	
	gsgl_push (GSGL_FILE_3D_DEPTH, p);
	gsgl_file_push_float (depth / GSGL_VIEW_DEPTH, gsgl_push, p);
}

void gsgl_file_lighting_enable (void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *p)
{
	gsgl_push (GSGL_FILE_ENABLE_LIGHTING, p);
	gsgl_push (GSGL_FILE_LIGHTING_ENABLE_TYPE, p);
	gsgl_push (1 /* default */, p);
	gsgl_push (GSGL_FILE_LIGHTING_ENABLE_NUM, p);
	gsgl_push (0, p);
}

void gsgl_file_lighting_disable (void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *p)
{
	gsgl_push (GSGL_FILE_DISABLE_LIGHTING, p);
        gsgl_push (GSGL_FILE_LIGHTING_DISABLE_NUM, p);
        gsgl_push (0, p);
}

void gsgl_file_separate_objects (void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *p)
{
        gsgl_push (GSGL_FILE_SEPARATE_OBJECTS, p);
}

void gsgl_file_array_element (int i, void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *p)
{
	gsgl_push (GSGL_FILE_ARRAY_ELEMENT_4BYTE, p);
	gsgl_push ((unsigned int) gsgl_convert_4byteint_to_1st2byte (i), p);
	gsgl_push ((unsigned int) gsgl_convert_4byteint_to_2nd2byte (i), p);
}

void gsgl_file_glarray_begin_polygon (void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *p)
{
	gsgl_push (GSGL_FILE_ARRAY_BEGIN_POLYGON, p);
}

void gsgl_file_glarray_end_polygon (void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *p)
{
	gsgl_push (GSGL_FILE_ARRAY_END_POLYGON, p);
}

void gsgl_file_glarray_begin_lines (void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *p)
{
	gsgl_push (GSGL_FILE_ARRAY_BEGIN_LINE, p);
}

void gsgl_file_glarray_end_lines (void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *p)
{
	gsgl_push (GSGL_FILE_ARRAY_END_LINE, p);
}

void gsgl_file_glarray_vertex (int datatype, int size, unsigned int totalsize, float *buffer,
			       int nx, int ny, 
			       float *r, float *t,
			       float xmin, float xmax, float ymin, float ymax,
			       int *idata, float *fdata,
			       void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *p)
{
	unsigned int i;

	gsgl_push ((unsigned int) GSGL_FILE_ENABLE_CLIENT_VERTEX, p);
	gsgl_push ((unsigned int) GSGL_FILE_CLIENT_VERTEX_TYPE, p);
	gsgl_push ((unsigned int) datatype, p);
	gsgl_push ((unsigned int) GSGL_FILE_CLIENT_VERTEX_SIZE, p);
	gsgl_push ((unsigned int) size, p);
	gsgl_push ((unsigned int) GSGL_FILE_CLIENT_VERTEX_TOTALSIZE_4BYTE, p);
	gsgl_push ((unsigned int) gsgl_convert_4byteint_to_1st2byte (totalsize), p);
	gsgl_push ((unsigned int) gsgl_convert_4byteint_to_2nd2byte (totalsize), p);
	gsgl_push ((unsigned int) GSGL_FILE_CLIENT_VERTEX_NX_4BYTE, p);
	gsgl_push ((unsigned int) gsgl_convert_4byteint_to_1st2byte (nx), p);
	gsgl_push ((unsigned int) gsgl_convert_4byteint_to_2nd2byte (nx), p);
	gsgl_push ((unsigned int) GSGL_FILE_CLIENT_VERTEX_NY_4BYTE, p);
	gsgl_push ((unsigned int) gsgl_convert_4byteint_to_1st2byte (ny), p);
	gsgl_push ((unsigned int) gsgl_convert_4byteint_to_2nd2byte (ny), p);

	switch (datatype) {
	case GSGL_DATA_ORTHO_EQUIV_INTERVAL:
		gsgl_push ((unsigned int) GSGL_FILE_CLIENT_VERTEX_XMIN, p);
		gsgl_file_push_float (xmin, gsgl_push, p);
		gsgl_push ((unsigned int) GSGL_FILE_CLIENT_VERTEX_XMAX, p);
		gsgl_file_push_float (xmax, gsgl_push, p);
		gsgl_push ((unsigned int) GSGL_FILE_CLIENT_VERTEX_YMIN, p);
		gsgl_file_push_float (ymin, gsgl_push, p);
		gsgl_push ((unsigned int) GSGL_FILE_CLIENT_VERTEX_YMAX, p);
		gsgl_file_push_float (ymax, gsgl_push, p);
		break;
	case GSGL_DATA_ORTHO_UNEQU_INTERVAL:
		for (i = 0; i<nx; i++) gsgl_file_push_float (buffer[i*ny*3+0], gsgl_push, p);
		for (i = 0; i<ny; i++) gsgl_file_push_float (buffer[i*3+1], gsgl_push, p);
		break;
	case GSGL_DATA_CYLIN_EQUIV_INTERVAL:
		for (i = 0; i<nx; i++) gsgl_file_push_float (buffer[i*ny*3+0], gsgl_push, p);
		break;
	case GSGL_DATA_CYLIN_UNEQU_INTERVAL:
		for (i = 0; i<nx; i++) gsgl_file_push_float (r[i], gsgl_push, p);
		for (i = 0; i<ny; i++) gsgl_file_push_float (t[i], gsgl_push, p);
		break;
	case GSGL_DATA_X_Y_Z_2D:
		for (i = 0; i<totalsize; i++) 
			if (i%3 == 0) gsgl_file_push_float (buffer[i], gsgl_push, p);
		for (i = 0; i<totalsize; i++)
			if (i%3 == 1) gsgl_file_push_float (buffer[i], gsgl_push, p);
		break;
	case GSGL_DATA_FEM_TRIANGLE:
		for (i = 0; i<totalsize; i++)
			if (i%3 == 0) gsgl_file_push_float (buffer[i], gsgl_push, p);
		for (i = 0; i<totalsize; i++)
			if (i%3 == 1) gsgl_file_push_float (buffer[i], gsgl_push, p);
	default:
		break;
	}
	for (i=0; i<totalsize; i++)
		if (i%3 == 2) gsgl_file_push_float (buffer[i], gsgl_push, p);

	if (datatype == GSGL_DATA_FEM_TRIANGLE) {
		gsgl_push (GSGL_FILE_CLIENT_FEM_IELM_4BYTE, p);
		for (i=0; i<ny*3; i++) {
			gsgl_push ((unsigned int) gsgl_convert_4byteint_to_1st2byte (idata[i]), p);
			gsgl_push ((unsigned int) gsgl_convert_4byteint_to_2nd2byte (idata[i]), p);
		}			
	}
}

void gsgl_file_glarray_color (int size, unsigned int totalsize, float *buffer, void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *p)
{
	unsigned int i;

	if (size == 0 && totalsize == 0) {
		gsgl_push((unsigned int) GSGL_FILE_ENABLE_CLIENT_COLOR, p);
		gsgl_push((unsigned int) GSGL_FILE_CLIENT_COLOR_SIZE, p);
		gsgl_push((unsigned int) size, p);
		gsgl_push((unsigned int) GSGL_FILE_CLIENT_COLOR_TOTALSIZE_4BYTE, p);
		gsgl_push((unsigned int) 0, p);
		gsgl_push((unsigned int) 0, p);
	} else {
		gsgl_push((unsigned int) GSGL_FILE_ENABLE_CLIENT_COLOR, p);
		gsgl_push((unsigned int) GSGL_FILE_CLIENT_COLOR_SIZE, p);
		gsgl_push((unsigned int) size, p);
		gsgl_push((unsigned int) GSGL_FILE_CLIENT_COLOR_TOTALSIZE_4BYTE, p);
		gsgl_push((unsigned int) gsgl_convert_4byteint_to_1st2byte (totalsize), p);
		gsgl_push((unsigned int) gsgl_convert_4byteint_to_2nd2byte (totalsize), p);
		for (i=0; i<totalsize ;i++) {
			gsgl_push ((unsigned int) gsgl_convert_0to1_to_2byte (buffer[i]), p);
		}
	}
}

void gsgl_file_glarray_normal (int datatype, int size, unsigned int totalsize, float *buffer, void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *p)
{
	if (datatype == GSGL_DATA_ORTHO_EQUIV_INTERVAL) {
	} else if (datatype == GSGL_DATA_ORTHO_UNEQU_INTERVAL) {
	} else if (datatype == GSGL_DATA_CYLIN_EQUIV_INTERVAL) {
	} else if (datatype == GSGL_DATA_CYLIN_UNEQU_INTERVAL) {
	} else if (datatype == GSGL_DATA_X_Y_Z_2D) {
	} else {
	}
}

/* postscript */

void gsgl_file_ps_gouraudonly_begin (void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *p)
{
	gsgl_push (GSGL_FILE_PS_BEGIN_GOURAUD_ONLY, p);
}

void gsgl_file_ps_gouraudonly_end (void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *p)
{
	gsgl_push (GSGL_FILE_PS_END_GOURAUD_ONLY, p);
}

void gsgl_file_ps_depth_by_deep (void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *p)
{
	gsgl_push (GSGL_FILE_PS_DEPTH_BY_DEEP, p);
}

void gsgl_file_ps_depth_by_shallow (void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *p)
{
	gsgl_push (GSGL_FILE_PS_DEPTH_BY_SHALLOW, p);
}

void gsgl_file_ps_depth_by_mean (void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *p)
{
	gsgl_push (GSGL_FILE_PS_DEPTH_BY_MEAN, p);
}

void gsgl_file_ps_depth_by_vectprod (float *x, float *y, float *z, void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *p)
{
	gsgl_push (GSGL_FILE_PS_DEPTH_BY_VECT_PROD, p);
	gsgl_file_push_float (x[0], gsgl_push, p);
	gsgl_file_push_float (y[0], gsgl_push, p);
	gsgl_file_push_float (z[0], gsgl_push, p);
	gsgl_file_push_float (x[1], gsgl_push, p);
	gsgl_file_push_float (y[1], gsgl_push, p);
	gsgl_file_push_float (z[1], gsgl_push, p);
	gsgl_file_push_float (x[2], gsgl_push, p);
	gsgl_file_push_float (y[2], gsgl_push, p);
	gsgl_file_push_float (z[2], gsgl_push, p);
}

void gsgl_file_ps_depth_by_point (float x, float y, float z, void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *p)
{
	gsgl_push (GSGL_FILE_PS_DEPTH_BY_POINT, p);
	gsgl_file_push_float (x, gsgl_push, p);
	gsgl_file_push_float (y, gsgl_push, p);
	gsgl_file_push_float (z, gsgl_push, p);
}

void gsgl_file_ps_depth_by_vectprod2 (float *x, float *y, float *z, 
				      unsigned int npoints, float *xp, float *yp, float *zp,
				      void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *p)
{
	int i;

	gsgl_push (GSGL_FILE_PS_DEPTH_BY_VECT_PROD2, p);
	gsgl_file_push_float (x[0], gsgl_push, p);
	gsgl_file_push_float (y[0], gsgl_push, p);
	gsgl_file_push_float (z[0], gsgl_push, p);
	gsgl_file_push_float (x[1], gsgl_push, p);
	gsgl_file_push_float (y[1], gsgl_push, p);
	gsgl_file_push_float (z[1], gsgl_push, p);
	gsgl_file_push_float (x[2], gsgl_push, p);
	gsgl_file_push_float (y[2], gsgl_push, p);
	gsgl_file_push_float (z[2], gsgl_push, p);
	gsgl_push (npoints, p);
	for (i=0; i<npoints; i++) {
		gsgl_file_push_float (xp[i], gsgl_push, p);
		gsgl_file_push_float (yp[i], gsgl_push, p);
		gsgl_file_push_float (zp[i], gsgl_push, p);
	}
}

void gsgl_file_beginanime (void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *p)
{
	gsgl_push (GSGL_FILE_BEGIN_ANIME, p);
}

void gsgl_file_endanime (void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *p)
{
	gsgl_push (GSGL_FILE_END_ANIME, p);
}

void gsgl_file_pstext (int ifnts, int ifntr,
		       float chh, float chw, float chsp, float angle, float tilt,
		       float x, float y, int dim, unsigned int *iascs, int nmax, 
		       void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *p)
{
	int i;

	/*printf ("file %d %d %f %f %f %f %f %f %f %d %p\n",
	  ifnts, ifntr, chh, chw, chsp, angle, tilt, x, y, nmax, p);*/

	gsgl_push (GSGL_FILE_PS_TEXT, p);
	gsgl_push (GSGL_FILE_PS_TEXT_FNT, p);
	gsgl_push (ifnts, p);
	gsgl_push (GSGL_FILE_PS_TEXT_FNT_DISPLAY, p);
	gsgl_push (ifntr, p);
	gsgl_push (GSGL_FILE_PS_TEXT_CHARACTER_HEIGHT, p);
	gsgl_file_push_float (chh, gsgl_push, p);
	gsgl_push (GSGL_FILE_PS_TEXT_CHARACTER_WIDTH, p);
	gsgl_file_push_float (chw, gsgl_push, p);
	gsgl_push (GSGL_FILE_PS_TEXT_CHARACTER_SPACE, p);
	gsgl_file_push_float (chsp, gsgl_push, p);
	gsgl_push (GSGL_FILE_PS_TEXT_CHARACTER_ANGLE, p);
	gsgl_file_push_float (angle, gsgl_push, p);
	gsgl_push (GSGL_FILE_PS_TEXT_CHARACTER_TILT, p);
	gsgl_file_push_float (tilt, gsgl_push, p);
	gsgl_push (GSGL_FILE_PS_TEXT_X, p);
	gsgl_file_push_float (x, gsgl_push, p);
	gsgl_push (GSGL_FILE_PS_TEXT_Y, p);
	gsgl_file_push_float (y, gsgl_push, p);
	gsgl_push (GSGL_FILE_PS_TEXT_DIM, p);
	gsgl_push (dim, p);
	gsgl_push (GSGL_FILE_PS_TEXT_CHARACTER_NUM, p);
	gsgl_push (nmax, p);
	gsgl_push (GSGL_FILE_PS_TEXT_TEXT, p);
	for (i=0; i<nmax; i++)
		gsgl_push (iascs[i], p);
        
}

void gsgl_file_pstext_end (void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *p)
{
	gsgl_push (GSGL_FILE_PS_TEXT_END, p);
}

void gsgl_file_pstext_position (float *x, float *y, int nmax, void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *p)
{
	int i;
	
	gsgl_push (GSGL_FILE_PS_TEXT_POSITION, p);
	for (i=0; i<nmax; i++) {
		/*printf ("x[%d] = %f\n", i, x[i]);*/
		gsgl_file_push_float_754 (x[i], gsgl_push, p);
	}
	for (i=0; i<nmax; i++) {
		/*printf ("y[%d] = %f\n", i, y[i]);*/
		gsgl_file_push_float_754 (y[i], gsgl_push, p);
	}
}

static void gsgl_file_gui_common (itemdata *data, void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *p)
{
	gsgl_push (GSGL_FILE_GUI_GEO, p);
        gsgl_file_push_float (data->x, gsgl_push, p);
        gsgl_file_push_float (data->y, gsgl_push, p);
        gsgl_file_push_float (data->w, gsgl_push, p);
        gsgl_file_push_float (data->h, gsgl_push, p);
	gsgl_push (GSGL_FILE_GUI_FNT, p);
	gsgl_push (data->fnt, p);
	gsgl_push (GSGL_FILE_GUI_CHR, p);
	gsgl_file_push_float (data->chh, gsgl_push, p);
	gsgl_file_push_float (data->chw, gsgl_push, p);
	gsgl_file_push_float (data->chsp, gsgl_push, p);
	gsgl_file_push_float (data->angl, gsgl_push, p);
	gsgl_file_push_float (data->tilt, gsgl_push, p);
}

void gsgl_file_gui_label (guialldata *data, void (* gsgl_push)(unsigned int, gsglPage *))
{
        int i, l;
	labeldata *s;
	gsglPage *p = data->common->p;

	s= data->specific;
        l = strlen (s->text);

        gsgl_push (GSGL_FILE_GUI_LABEL_BEGIN, p);

	gsgl_file_gui_common (data->common, gsgl_push, p);

        gsgl_push (GSGL_FILE_GUI_LABEL_TEXT, p);
        gsgl_push (l, p);
        for (i=0; i<l; i++)
                gsgl_push ((unsigned int) s->text[i], p);
	gsgl_push (GSGL_FILE_GUI_LABEL_JUST, p);
	gsgl_push (s->ijust, p);
        gsgl_push (GSGL_FILE_GUI_LABEL_END, p);
}

void gsgl_file_gui_button (guialldata *data, void (* gsgl_push)(unsigned int, gsglPage *))
{
        int i, l;
	buttondata *s;
	gsglPage *p = data->common->p;

	s = data->specific;
        l = strlen (s->text);

        gsgl_push (GSGL_FILE_GUI_BUTTON_BEGIN, p);

	gsgl_file_gui_common (data->common, gsgl_push, p);

        gsgl_push (GSGL_FILE_GUI_BUTTON_LABEL, p);
        gsgl_push (l, p);
        for (i=0; i<l; i++)
                gsgl_push ((unsigned int) s->text[i], p);
        gsgl_push (GSGL_FILE_GUI_BUTTON_END, p);
}

void gsgl_file_gui_checkbutton (guialldata *data, void (* gsgl_push)(unsigned int, gsglPage *))
{
	int i, l;
	checkbuttondata *s;
	gsglPage *p = data->common->p;
	
	s = data->specific;
	l = strlen (s->text);
	
	gsgl_push (GSGL_FILE_GUI_CHECK_BEGIN, p);

	gsgl_file_gui_common (data->common, gsgl_push, p);

	gsgl_push (GSGL_FILE_GUI_CHECK_LABEL, p);
	gsgl_push (l, p);
	for (i=0; i<l; i++)
		gsgl_push ((unsigned int) s->text[i], p);	
	gsgl_push (GSGL_FILE_GUI_CHECK_INIT, p);
	gsgl_push (s->init, p);
	gsgl_push (GSGL_FILE_GUI_CHECK_END, p);
}

void gsgl_file_gui_combobox (guialldata *data, void (* gsgl_push)(unsigned int, gsglPage *))
{
	int i, j, l;
	char *label;
	comboboxdata *s;
	gsglPage *p = data->common->p;
	
	s = data->specific;

	gsgl_push (GSGL_FILE_GUI_COMBO_BEGIN, p);

	gsgl_file_gui_common (data->common, gsgl_push, p);

	gsgl_push (GSGL_FILE_GUI_COMBO_ITEMS, p);
	gsgl_push (s->items, p);

	for (i=0; i<s->items; i++) {
		label = s->text[i];
		l = strlen(label);
		gsgl_push (l, p);
		for (j=0; j<l; j++)
			gsgl_push ((unsigned int) label[j], p);
	}
	gsgl_push (GSGL_FILE_GUI_COMBO_CURRENT_ITEM, p);
	gsgl_push (s->current, p);
	gsgl_push (GSGL_FILE_GUI_COMBO_END, p);
}

void gsgl_file_gui_lineedit (guialldata *data, void (* gsgl_push)(unsigned int, gsglPage *))
{
	int i, l;
	lineeditdata *s;
	gsglPage *p = data->common->p;

	s = data->specific;
	l = strlen(s->text);

	gsgl_push (GSGL_FILE_GUI_EDIT_BEGIN, p);

	gsgl_file_gui_common (data->common, gsgl_push, p);

	gsgl_push (GSGL_FILE_GUI_EDIT_MAX, p);
	gsgl_push (s->max, p);	
	gsgl_push (GSGL_FILE_GUI_EDIT_TEXT, p);
	gsgl_push (l, p);
	for (i=0; i<l; i++)
		gsgl_push ((unsigned int) s->text[i], p);
	gsgl_push (GSGL_FILE_GUI_EDIT_END, p);
}

void gsgl_file_gui_spinbuttonf (guialldata *data, void (* gsgl_push)(unsigned int, gsglPage *))
{
	spinbuttondataf *s;
	gsglPage *p = data->common->p;

	s = data->specific;

	gsgl_push (GSGL_FILE_GUI_SPINF_BEGIN, p);

	gsgl_file_gui_common (data->common, gsgl_push, p);

	gsgl_push (GSGL_FILE_GUI_SPINF_INIT, p);
	gsgl_file_push_float (s->init, gsgl_push, p);
	gsgl_push (GSGL_FILE_GUI_SPINF_LOWER, p);
	gsgl_file_push_float (s->lower, gsgl_push, p);
	gsgl_push (GSGL_FILE_GUI_SPINF_UPPER, p);
	gsgl_file_push_float (s->upper, gsgl_push, p);
	gsgl_push (GSGL_FILE_GUI_SPINF_DIGITS, p);
	gsgl_push (s->digits, p);
	gsgl_push (GSGL_FILE_GUI_SPINF_END, p);
}

void gsgl_file_gui_spinbuttoni (guialldata *data, void (* gsgl_push)(unsigned int, gsglPage *))
{
	spinbuttondatai *s;
	unsigned int i;
	gsglPage *p = data->common->p;

	s = data->specific;

	gsgl_push (GSGL_FILE_GUI_SPINI_BEGIN, p);

	gsgl_file_gui_common (data->common, gsgl_push, p);
	i = s->init + 2147483647;
	gsgl_push (GSGL_FILE_GUI_SPINI_INIT, p);
	gsgl_push ((unsigned int) gsgl_convert_4byteint_to_1st2byte (i), p);
	gsgl_push ((unsigned int) gsgl_convert_4byteint_to_2nd2byte (i), p);
	gsgl_push (GSGL_FILE_GUI_SPINI_LOWER, p);
	i = s->lower + 2147483647;
	gsgl_push ((unsigned int) gsgl_convert_4byteint_to_1st2byte (i), p);
	gsgl_push ((unsigned int) gsgl_convert_4byteint_to_2nd2byte (i), p);
	gsgl_push (GSGL_FILE_GUI_SPINI_UPPER, p);
	i = s->upper + 2147483647;
	gsgl_push ((unsigned int) gsgl_convert_4byteint_to_1st2byte (i), p);
	gsgl_push ((unsigned int) gsgl_convert_4byteint_to_2nd2byte (i), p);
	gsgl_push (GSGL_FILE_GUI_SPINI_END, p);
}

void gsgl_file_gui_set_external (gsglboolean b, void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *p)
{
	gsgl_push (GSGL_FILE_GUI_SET_EXTERNAL, p);
	if (b)
		gsgl_push (1, p);
	else
		gsgl_push (0, p);
}
