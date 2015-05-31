#include "gsaf.h"
#include "gsgl.h"
#include "base.h"
#include "matrix.h"
#include "drv_wrapper.h"
#include "drv_gl.h"
#include "displaylist.h"
#include "utils.h"
#include "configfile.h"
#include "clientstate.h"
#include "fileformat.h"
#include "ogl.h"
#include "bsc1.h"
#include "guiitems.h"
#include "xwin.h"
#include "device.h"
#include "tcp_guiitems.h"

void gsgl_init_info_every_page (gsglPage *p)
{
	int i;
	float identity[16] = {1.0, 0.0, 0.0, 0.0,
			      0.0, 1.0, 0.0, 0.0,
			      0.0, 0.0, 1.0, 0.0,
			      0.0, 0.0, 0.0, 1.0};
	gsgl3DInfo *Gsgl3DInfo1 = p->ThreeDInfo;

	Gsgl3DInfo1->Now3D = GSGL_FALSE;
	Gsgl3DInfo1->rotated = GSGL_FALSE;
	Gsgl3DInfo1->depth = 0.0;
	Gsgl3DInfo1->projection = DEFAULT_PROJECTIONTYPE;
	Gsgl3DInfo1->eyeposition = GSGLBASIC_EYEPOSITION;
	Gsgl3DInfo1->Lighting = GSGL_FALSE;
	p->base->Lighting_enabled_by_user = GSGL_FALSE;
	p->base->Lighting_enabled_temporary = GSGL_FALSE;

	for (i=0; i<16; i++) {
		p->base->Allstatus.modelview_matrix[i] = identity [i];
		p->base->Allstatus.projection_matrix[i] = identity [i];
	}
	for (i=0; i<6; i++)
		p->base->Allstatus.clip_plane[i] = -1;
        p->base->Allstatus.clip_plane_tmp = -1;
	if (gsgl_inq_mode(p->library) == GSGL_MODE_NO_GL ||
	    gsgl_inq_mode(p->library) == GSGL_MODE_TCPIP) {
		p->base->Allstatus.antialiasing = GSGL_FALSE;
	} else {
		if (gsgl_inq_antialiasing_now (p->win)) {
			p->base->Allstatus.antialiasing = GSGL_TRUE;
		} else {
			p->base->Allstatus.antialiasing = GSGL_FALSE;
		}
	}
}

gsglstatus gsgl_inq_all_status (gsglPage *p)
{
	gsgl3DInfo *Gsgl3DInfo1 = p->ThreeDInfo;

	p->base->Allstatus.indexcolor = p->base->LineColor;
	p->base->Allstatus.rgb.r = p->rgb.r;
	p->base->Allstatus.rgb.g = p->rgb.g;
	p->base->Allstatus.rgb.b = p->rgb.b;	

	p->base->Allstatus.linewidth_index = p->base->LineWidth;
	p->base->Allstatus.linewidth_cm = p->Ws;

	p->base->Allstatus.threeDinfo = *Gsgl3DInfo1;
	gsgl_coordinate_inq (&(p->base->Allstatus.coord_mode), p);
	p->base->Allstatus.lighting_enabled_by_user = p->base->Lighting_enabled_by_user;
        p->base->Allstatus.lighting_enabled_temporary = p->base->Lighting_enabled_temporary;
	
	return p->base->Allstatus;
}

float gsgl_inq_depth3d (gsglPage *p)
{
	return p->ThreeDInfo->depth;
}

int gsgl_inq_projectiontype (gsglPage *p)
{
	return p->ThreeDInfo->projection;
}

gsglboolean gsgl_inq_3dobjects_exist (gsglPage *p)
{
	return p->ThreeDInfo->Now3D;
}

void gsgl_set_lighting_enable (gsglPage *p)
{
	p->ThreeDInfo->Lighting = GSGL_TRUE;
}

void gsgl_set_lighting_disable (gsglPage *p)
{
	p->ThreeDInfo->Lighting = GSGL_FALSE;
}

gsglboolean gsgl_inq_lighting_is_enabled (gsglPage *p)
{
	return p->ThreeDInfo->Lighting;
}

/* World座標系における視点の座標を返す */
void gsgl_inq_eyepoint (float *x, float *y, float *z, gsglPage *p)
{
	*x = 0.5 * GSGL_PAPER_WIDTH;
	*y = 0.5 * GSGL_PAPER_HEIGHT;
	*z = p->ThreeDInfo->eyeposition;
}

/* utils */
static void gsgl_base_begin_objects_normal (gsglPage *p)
{
	int i;

	gsgl_coordinate_inq (&i, p);
	if (i != GSGL_COORD_NORMAL)
		gsgl_normalmode (p);

	gsgl_displaylist_begin (GSGL_LIST_2DOBJ, p);
}

static void gsgl_base_begin_objects_2d (gsglPage *p)
{
	int i;

	gsgl_coordinate_inq (&i, p);

	if (i != GSGL_COORD_2D)
		gsgl_2dmode (p);

	gsgl_displaylist_begin (GSGL_LIST_2DOBJ, p);
}

static void gsgl_base_begin_objects_3d (gsglPage *p)
{
	int i;

	gsgl_coordinate_inq (&i, p);
	if (i != GSGL_COORD_3D)
		gsgl_3dmode (p);

	gsgl_displaylist_begin (GSGL_LIST_3DOBJ, p);
}

static void gsgl_base_begin_objects_text2d (gsglPage *p)
{
	int i;

	gsgl_coordinate_inq (&i, p);
	if (i != GSGL_COORD_TEXT2D)
		gsgl_text2dmode (p);

	gsgl_displaylist_begin (GSGL_LIST_2DOBJ, p);
}

static void gsgl_base_begin_objects_text3d (gsglPage *p)
{
	int i;

	gsgl_coordinate_inq (&i, p);
	if (i != GSGL_COORD_TEXT3D)
		gsgl_text3dmode (p);

	gsgl_displaylist_begin (GSGL_LIST_3DOBJ, p);
}

static void gsgl_base_begin_objects_3d_fileonly (gsglPage *p)
{
	int i;

	gsgl_coordinate_inq (&i, p);
	if (i != GSGL_COORD_3D)
		gsgl_3dmode (p);

	gsgl_displaylist_begin (GSGL_LIST_3DOBJ_FILEONLY, p);
}

/* change status */

/* lighting */

void gsgl_lighting_enable (gsglPage *p)
{
	p->ThreeDInfo->Lighting = GSGL_TRUE;
	gsgl_displaylist_begin (GSGL_LIST_LIGHTING_ENABLE, p);
	gsgl_drv_lighting_enable (p);
}

void gsgl_lighting_enable_by_user (gsglPage *p)
{
	p->ThreeDInfo->Lighting = GSGL_TRUE;
	p->base->Lighting_enabled_by_user = GSGL_TRUE;
	p->base->Lighting_enabled_temporary = GSGL_TRUE;
	gsgl_displaylist_begin (GSGL_LIST_LIGHTING_ENABLE, p);
	gsgl_drv_lighting_enable (p);
}

void gsgl_enablelighting ()
{
	if (!gsgl_inq_page_is_open ()) return;

	gsgl_lighting_enable_by_user (gsgl_inq_page());
}

void gsgl_lighting_enable_temporary ()
{
	gsglPage *p = gsgl_inq_page ();
 
	p->base->Lighting_enabled_temporary = GSGL_TRUE;
	gsgl_displaylist_begin (GSGL_LIST_LIGHTING_ENABLE, p);
	gsgl_drv_lighting_enable (p);
}

void gsgl_lighting_disable ()
{
	gsglPage *p = gsgl_inq_page ();

	gsgl_displaylist_begin (GSGL_LIST_LIGHTING_DISABLE, p);
	gsgl_drv_lighting_disable (p);
}

void gsgl_lighting_disable_temporary ()
{
	gsglPage *p = gsgl_inq_page ();

	p->base->Lighting_enabled_temporary = GSGL_FALSE;
	gsgl_displaylist_begin (GSGL_LIST_LIGHTING_DISABLE, p);
	gsgl_drv_lighting_disable (p);
}

/* clipping */

void gsgl_clip_enable_normal (int ic, float eqn1[4], gsglPage *p)
{
	int i;

	if (ic<0 || ic>5) return;

	gsgl_coordinate_inq (&i, p);
	if (i != GSGL_COORD_NORMAL)
		gsgl_normalmode (p);

	gsgl_displaylist_begin (GSGL_LIST_STATUS, p);
	gsgl_drv_clip_enable_unmovable (ic, eqn1, p);

        p->base->Allstatus.clip_plane_tmp = ic;
	p->base->Allstatus.clip_plane[ic] = GSGL_FILE_CLIP_UNMOVABLE;
	for (i=0; i<4; i++)
		p->base->Allstatus.clip_eq[ic][i] = eqn1[i];
	for (i=0; i<16; i++)
		p->base->Allstatus.clip_proj [ic][i] = p->base->Allstatus.projection_matrix[i];
	for (i=0; i<16; i++)
		p->base->Allstatus.clip_model[ic][i] = p->base->Allstatus.modelview_matrix[i];
}

void gsgl_clip_enable_3d_fileonly (int ic, float eqn1[4], gsglPage *p)
{
	int i;

	if (ic<0 || ic>5) return;

	gsgl_coordinate_inq (&i, p);
	if (i != GSGL_COORD_3D)
		gsgl_3dmode (p);

	gsgl_displaylist_begin (GSGL_LIST_3DOBJ_FILEONLY, p);
	gsgl_drv_clip_enable_3d_fileonly (ic, eqn1, p);

        p->base->Allstatus.clip_plane_tmp = ic;
	p->base->Allstatus.clip_plane[ic] = GSGL_FILE_CLIP_3D_FILEONLY;
	for (i=0; i<4; i++)
		p->base->Allstatus.clip_eq[ic][i] = eqn1[i];
	for (i=0; i<16; i++) {
		p->base->Allstatus.clip_proj [ic][i] = p->base->Allstatus.projection_matrix[i];
		p->base->Allstatus.clip_model[ic][i] = p->base->Allstatus.modelview_matrix[i];
	}
}

void gsgl_clip_disable (int ic, gsglPage *p)
{
	if (ic<0 || ic>5) return;

	gsgl_displaylist_begin (GSGL_LIST_STATUS, p);
	gsgl_drv_clip_disable (ic, p);

	p->base->Allstatus.clip_plane [ic] = -1;
}

void gsgl_clip_delete (gsglPage *p)
{
	gsgl_clip_disable (0, p);
	gsgl_clip_disable (1, p);
	gsgl_clip_disable (2, p);
	gsgl_clip_disable (3, p);
}

/* projection matrix */

void gsgl_projection_matrix (float matrix[16], gsglPage *p)
{
	int i;

	gsgl_displaylist_begin (GSGL_LIST_PROJECTION, p);
	gsgl_drv_projection_matrix (matrix, p);
	gsgl_displaylist_end (p);

	for (i=0; i<16; i++)
		p->base->Allstatus.projection_matrix[i] = matrix[i];
}

void gsgl_projectiontype (int i, gsglPage *p)
{
	gsgl3DInfo *Gsgl3DInfo1 = p->ThreeDInfo;

	Gsgl3DInfo1->projection = i;
	gsgl_projmatrix_set (p);
	gsgl_load_projection_matrix (p);
}

void gsgl_eyeposition (float eye, gsglPage *p)
{
	p->ThreeDInfo->eyeposition = eye;
	if (p->ThreeDInfo->projection == GSGL_ORTHOGONAL) {
		gsgl_projmatrix_set_ortho (eye, p);
	} else {
		gsgl_projmatrix_set_frustum (eye, p);
	}
	gsgl_projmatrix_set (p);
	gsgl_load_projection_matrix (p);
}

/* modelview matrix & depth test, polygon offset */

static void gsgl_base_enable_antialiasing (gsglPage *p)
{
	if (p->win && !gsgl_inq_antialiasing_now (p->win)) {
		p->base->Allstatus.antialiasing = GSGL_TRUE;
		gsgl_drv_enable_antialiasing (p);
	}
}

static void gsgl_base_disable_antialiasing (gsglPage *p)
{
	if (p->win && gsgl_inq_antialiasing_now (p->win)) {
		p->base->Allstatus.antialiasing = GSGL_FALSE;
		gsgl_drv_disable_antialiasing (p);
	}
}

void gsgl_modelview_matrix (float *matrix, gsglPage *p)
{
	int i;
	gsgl_displaylist_begin (GSGL_LIST_MODELVIEW, p);
	gsgl_drv_modelview_matrix (matrix, p);
	gsgl_displaylist_end (p);

	for (i=0; i<16; i++)
		p->base->Allstatus.modelview_matrix[i] = matrix[i];
}

void gsgl_normalmode (gsglPage *p)
{
	GLfloat matrix[16] = {1.0, 0.0, 0.0, 0.0, 
			      0.0, 1.0, 0.0, 0.0, 
			      0.0, 0.0, 1.0, 0.0,
			      0.0, 0.0, 0.0, 1.0};
	int i;

	gsgl_coordinate_inq (&i, p);
	if (i != GSGL_COORD_NORMAL)
		gsgl_coordinate_set (GSGL_COORD_NORMAL, p);

	if (p->base->Lighting_enabled_by_user && p->base->Lighting_enabled_temporary)
		gsgl_lighting_disable_temporary ();

	gsgl_displaylist_begin (GSGL_LIST_STATUS, p);
	gsgl_base_enable_antialiasing (p);
	gsgl_drv_normalmode (p);
	gsgl_modelview_matrix (matrix, p);
}

void gsgl_2dmode (gsglPage *p)
{
	GLfloat matrix[16];
	int i;

	gsgl_coordinate_inq (&i, p);
	if (i != GSGL_COORD_2D)
		gsgl_coordinate_set (GSGL_COORD_2D, p);

	if (p->base->Lighting_enabled_by_user && p->base->Lighting_enabled_temporary)
		gsgl_lighting_disable_temporary ();

	gsgl_displaylist_begin (GSGL_LIST_STATUS, p);
	gsgl_base_enable_antialiasing (p);
	gsgl_drv_2dmode (p);	
	gsgl_matrix_inq_2dmatrix (matrix, p);
	gsgl_modelview_matrix (matrix, p);
}

void gsgl_3dmode (gsglPage *p)
{
	GLfloat matrix[16];
	int i;

	gsgl_coordinate_inq (&i, p);
	if (i != GSGL_COORD_3D)
		gsgl_coordinate_set (GSGL_COORD_3D, p);

	if (p->base->Lighting_enabled_by_user && !p->base->Lighting_enabled_temporary)
		gsgl_lighting_enable_temporary ();

	gsgl_displaylist_begin (GSGL_LIST_STATUS, p);
	gsgl_base_disable_antialiasing (p);
	gsgl_drv_3dmode (p);
	gsgl_matrix_inq_3dmatrix (matrix, p);
	gsgl_modelview_matrix (matrix, p);
}

void gsgl_text2dmode (gsglPage *p)
{
	GLfloat matrix[16];
	int i;

	gsgl_coordinate_inq (&i, p);
	if (i != GSGL_COORD_TEXT2D)
		gsgl_coordinate_set (GSGL_COORD_TEXT2D, p);

	if (p->base->Lighting_enabled_by_user && p->base->Lighting_enabled_temporary)
		gsgl_lighting_disable_temporary ();

	gsgl_displaylist_begin (GSGL_LIST_STATUS, p);
	gsgl_base_enable_antialiasing (p);
	gsgl_drv_2dmode (p);
	gsgl_matrix_inq_text2dmatrix (matrix, p);
	gsgl_modelview_matrix (matrix, p);
}

void gsgl_text3dmode (gsglPage *p)
{
	GLfloat matrix[16];
	int i;

	gsgl_coordinate_inq (&i, p);
	if (i != GSGL_COORD_TEXT3D)
		gsgl_coordinate_set (GSGL_COORD_TEXT3D, p);

	if (p->base->Lighting_enabled_by_user && !p->base->Lighting_enabled_temporary)
		gsgl_lighting_enable_temporary ();

	gsgl_displaylist_begin (GSGL_LIST_STATUS, p);
	gsgl_base_disable_antialiasing (p);
	gsgl_drv_3dmode (p);
	gsgl_matrix_inq_text3dmatrix (matrix, p);
	gsgl_modelview_matrix (matrix, p);
}

/* object's attribute */

void gsgl_rgb (float r, float g, float b, gsglPage *p)
{
	int ir, ig, ib;

	ir = (int) (r*255+0.5); /* nint */
	if (ir < 0) {
		ir=0;
	} else if (ir > 255) {
		ir=255;
	}

	ig = (int) (g*255+0.5); /* nint */
	if (ig < 0) {
		ig=0;
	} else if (ig > 255) {
		ig=255;
	}
	
	ib = (int) (b*255+0.5); /* nint */
	if (ib < 0) {
		ib=0;
	} else if (ib > 255) {
		ib=255;
	}
	
	gsgl_displaylist_begin (GSGL_LIST_STATUS, p);
	gsgl_drv_rgb (ir/255.0, ig/255.0, ib/255.0, p);

	p->rgb.r = r;
	p->rgb.g = g;
	p->rgb.b = b;
}

void gsgl_linewidth (float w, gsglPage *p)
{
	gsgl_displaylist_begin (GSGL_LIST_STATUS, p);
	gsgl_drv_linewidth (w, p);
	p->Ws = w;
}

void gsgl_linewidth_index (int iw, gsglPage *p)
{
	gsgl_displaylist_begin (GSGL_LIST_STATUS, p);
	gsgl_drv_linewidth_index (iw, p);
	p->base->LineWidth = iw;
}

void gsgl_color_index (int icolor, gsglPage *p)
{
	gsgl_displaylist_begin (GSGL_LIST_STATUS, p);
	gsgl_drv_color_index (icolor, gsgl_inq_bg_color (p->library), p);
	p->base->LineColor = icolor;
	p->base->Allstatus.rgb.r = -1.0;
}

/* 3d object's depth */

void gsgl_depth3d (float depth, gsglPage *p)
{
	gsgl3DInfo *Gsgl3DInfo1 = p->ThreeDInfo;

	Gsgl3DInfo1->Now3D = GSGL_TRUE;
	Gsgl3DInfo1->depth = depth;
	gsgl_displaylist_begin (GSGL_LIST_STATUS, p);
	gsgl_drv_depth3d (depth, p);
}

void gsgl_separate_objects (gsglPage *p)
{
	gsgl_displaylist_begin (GSGL_LIST_STATUS, p);
	gsgl_drv_separate_objects (p);
}

/* draw primitives */

void gsgl_move (float x, float y, gsglPage *p)
{
	p->base->Position.x = x;
	p->base->Position.y = y;
}

void gsgl_inqpos (float *x, float *y, gsglPage *p)
{
	*x = p->base->Position.x;
	*y = p->base->Position.y;
}

void gsgl_draw (float x, float y, gsglPage *p)
{
	gsgl_base_begin_objects_normal (p);
	gsgl_drv_beginlines (p);
	gsgl_drv_vertex2 (p->base->Position.x, p->base->Position.y, p);
	gsgl_drv_vertex2 (x, y, p);
	gsgl_drv_endlines (p);

	gsgl_move (x, y, p);
}

void gsgl_draw_text2d (float x, float y, gsglPage *p)
{
	gsgl_base_begin_objects_text2d (p);
	gsgl_drv_beginlines (p);
	gsgl_drv_vertex2 (p->base->Position.x, p->base->Position.y, p);
	gsgl_drv_vertex2 (x, y, p);
	gsgl_drv_endlines (p);

	gsgl_move (x, y, p);
}

void gsgl_draw_text3d (float x, float y, gsglPage *p)
{
	gsgl_base_begin_objects_text3d (p);
	gsgl_drv_beginlines (p);
	gsgl_drv_normal (0.0, 0.0, 1.0, p);
	gsgl_drv_vertex3 (p->base->Position.x, p->base->Position.y, 0.0, p);
	gsgl_drv_vertex3 (x, y, 0.0, p);
	gsgl_drv_endlines (p);

	gsgl_move (x, y, p);
}

void gsgl_lines (float *x, float *y, int4 n, gsglPage *p)
{
	int i;

	gsgl_base_begin_objects_normal (p);
	gsgl_drv_beginlines (p);
	for (i=0; i<n; i++)
		gsgl_drv_vertex2 (x[i], y[i], p);
	/* 2008/1/28 */
	/*if (x[0] != x[n-1] || y[0] != y[n-1])
	  gsgl_drv_vertex2 (x[0], y[0], p);*/
	gsgl_drv_endlines (p);

	gsgl_move (x[n-1], y[n-1], p);
}

void gsgl_polygon (float *x, float *y, int4 n, gsglPage *p)
{
	int i;

	if (n < 3) return;

	gsgl_base_begin_objects_normal (p);
	gsgl_drv_beginpolygon (p);
	for (i=0; i<n; i++)
		gsgl_drv_vertex2 (x[i], y[i], p);
	gsgl_drv_endpolygon (p);

	gsgl_move (x[n-1], y[n-1], p);
}

void gsgl_rgbtriangle (float *x, float *y, float *r, float *g, float *b, gsglPage *p)
{
	int i;

	gsgl_base_begin_objects_normal (p);
	gsgl_drv_beginrgbtriangle (p);
	for (i=0; i<3; i++) {
		gsgl_drv_rgb (r[i], g[i], b[i], p);
		gsgl_drv_vertex2 (x[i], y[i], p);	
	}
	gsgl_drv_endrgbtriangle (p);

	gsgl_move (x[2], y[2], p);
}

/* 2D mode basic functions. */

void gsgl_move2d (float x, float y, gsglPage *p)
{
	float xw, yw;

	p->base->Position2d.x = x;
	p->base->Position2d.y = y;

	gsglproject2d (x, y, &xw, &yw, p);
	gsgl_move (xw, yw, p);
}

void gsgl_inqpos2d (float *x, float *y, gsglPage *p)
{
	*x = p->base->Position2d.x;
	*y = p->base->Position2d.y;
}

void gsgl_draw2d (float x, float y, gsglPage *p)
{
	gsgl_base_begin_objects_2d (p);
	gsgl_drv_beginlines (p);
	gsgl_drv_vertex2 (p->base->Position2d.x, p->base->Position2d.y, p);
	gsgl_drv_vertex2 (x, y, p);
	gsgl_drv_endlines (p);

	gsgl_move2d (x, y, p);
}

void gsgl_lines2d (float *x, float *y, int4 n, gsglPage *p)
{
	int i;

	gsgl_base_begin_objects_2d (p);
	gsgl_drv_beginlines (p);
	for (i=0; i<n; i++)
		gsgl_drv_vertex2 (x[i], y[i], p);
	/* 2008/1/28 */
	/*if (x[0] != x[n-1] || y[0] != y[n-1])
	  gsgl_drv_vertex2 (x[0], y[0], p);*/
	gsgl_drv_endlines (p);

	gsgl_move2d (x[n-1], y[n-1], p);
}

void gsgl_polygon2d (float *x, float *y, int4 n, gsglPage *p)
{
	int i;
	if (n < 3) return;

	gsgl_base_begin_objects_2d (p);
	gsgl_drv_beginpolygon (p);
	for (i=0; i<n; i++)
		gsgl_drv_vertex2 (x[i], y[i], p);
	gsgl_drv_endpolygon (p);

	gsgl_move2d (x[n-1], y[n-1], p);
}

void gsgl_rgbtriangle2d (float *x, float *y, float *r, float *g, float *b, gsglPage *p)
{
	int i;

	gsgl_base_begin_objects_2d (p);
	gsgl_drv_beginrgbtriangle (p);
	for (i=0; i<3; i++) {
		gsgl_drv_rgb (r[i], g[i], b[i], p);
		gsgl_drv_vertex2 (x[i], y[i], p);
	}
	gsgl_drv_endrgbtriangle (p);

	gsgl_move2d (x[2], y[2], p);
}

/* 3D mode basic functions. */

void gsgl_move3d (float x, float y, float z, gsglPage *p)
{
	p->base->Position3d.x = x;
	p->base->Position3d.y = y;
	p->base->Position3d.z = z;
}

void gsgl_inqpos3d (float *x, float *y, float *z, gsglPage *p)
{
	*x = p->base->Position3d.x;
	*y = p->base->Position3d.y;
	*z = p->base->Position3d.z;
}

void gsgl_draw3d (float x, float y, float z, gsglPage *p)
{
	gsgl_base_begin_objects_3d (p);
	gsgl_drv_beginlines (p);
	gsgl_drv_glonly_normal (0.0, 0.0, 1.0, p);
	gsgl_drv_vertex3 (p->base->Position3d.x, p->base->Position3d.y, p->base->Position3d.z, p);
	gsgl_drv_vertex3 (x, y, z, p);
	gsgl_drv_endlines (p);

	gsgl_move3d (x, y, z, p);
}

void gsgl_draw3d_with_normal (float x, float y, float z, float nx, float ny, float nz, gsglPage *p)
{
	gsgl_base_begin_objects_3d (p);
	gsgl_drv_beginlines (p);
	gsgl_drv_normal (nx, ny, nz, p);
	gsgl_drv_vertex3 (p->base->Position3d.x, p->base->Position3d.y, p->base->Position3d.z, p);
	gsgl_drv_vertex3 (x, y, z, p);
	gsgl_drv_endlines (p);

	gsgl_move3d (x, y, z, p);
}

void gsgl_draw3d_with_normals (float x, float y, float z, float nx[2], float ny[2], float nz[2], gsglPage *p)
{
	gsgl_base_begin_objects_3d (p);
	gsgl_drv_beginlines (p);
	gsgl_drv_normal (nx[0], ny[0], nz[0], p);
	gsgl_drv_vertex3 (p->base->Position3d.x, p->base->Position3d.y, p->base->Position3d.z, p);
	gsgl_drv_normal (nx[1], ny[1], nz[1], p);
	gsgl_drv_vertex3 (x, y, z, p);
	gsgl_drv_endlines (p);

	gsgl_move3d (x, y, z, p);
}

void gsgl_lines3d (float *x, float *y, float *z, int4 n, gsglPage *p)
{
	int i;

	gsgl_base_begin_objects_3d (p);
	gsgl_drv_beginlines (p);
	gsgl_drv_glonly_normal (0.0, 0.0, 1.0, p);
	for (i=0; i<n; i++)
		gsgl_drv_vertex3 (x[i], y[i], z[i], p);
	if (x[0] != x[n-1] || y[0] != y[n-1] || z[0] != z[n-1])
		gsgl_drv_vertex3 (x[0], y[0], z[0], p);
	gsgl_drv_endlines (p);

	gsgl_move3d (x[n-1], y[n-1], z[n-1], p);
}

void gsgl_lines3d_with_normal (float *x, float *y, float *z,
			       float nx, float ny, float nz, int4 n, gsglPage *p)
{
	int i;

	gsgl_base_begin_objects_3d (p);
	gsgl_drv_beginlines (p);
	gsgl_drv_normal (nx, ny, nz, p);
	for (i=0; i<n; i++)
		gsgl_drv_vertex3 (x[i], y[i], z[i], p);
	if (x[0] != x[n-1] || y[0] != y[n-1] || z[0] != z[n-1])
		gsgl_drv_vertex3 (x[0], y[0], z[0], p);
	gsgl_drv_endlines (p);

	gsgl_move3d (x[n-1], y[n-1], z[n-1], p);
}

void gsgl_lines3d_with_normals (float *x, float *y, float *z, 
				float *nx, float *ny, float *nz, int4 n, gsglPage *p)
{
	int i;

	gsgl_base_begin_objects_3d (p);
	gsgl_drv_beginlines (p);
	for (i=0; i<n; i++) {
		gsgl_drv_normal (nx[i], ny[i], nz[i], p);
		gsgl_drv_vertex3 (x[i], y[i], z[i], p);
	}
	if (x[0] != x[n-1] || y[0] != y[n-1] || z[0] != z[n-1]) {
		gsgl_drv_normal (nx[0], ny[0], nz[0], p);
		gsgl_drv_vertex3 (x[0], y[0], z[0], p);
	}
	gsgl_drv_endlines (p);

	gsgl_move3d (x[n-1], y[n-1], z[n-1], p);
}

void gsgl_polygon3d (float *x, float *y, float *z, int4 n, gsglPage *p)
{
	int i;
	float normal[3];

	if (n < 3) return;

	gsgl_set_normal_vector1 (x, y, z, normal);

	gsgl_base_begin_objects_3d (p);
	gsgl_drv_beginpolygon (p);
	gsgl_drv_glonly_normal (normal[0], normal[1], normal[2], p);
	for (i=0; i<n; i++)
		gsgl_drv_vertex3 (x[i], y[i], z[i], p);
	gsgl_drv_endpolygon (p);

	gsgl_move3d (x[n-1], y[n-1], z[n-1], p);
}

void gsgl_polygon3d_with_normal (float *x, float *y, float *z,
				 float nx, float ny, float nz, int4 n, gsglPage *p)
{
	int i;

	if (n < 3) return;

	gsgl_base_begin_objects_3d (p);
	gsgl_drv_beginpolygon (p);
	gsgl_drv_normal (nx, ny, nz, p);
	for (i=0; i<n; i++)
		gsgl_drv_vertex3 (x[i], y[i], z[i], p);
	gsgl_drv_endpolygon (p);

	gsgl_move3d (x[n-1], y[n-1], z[n-1], p);
}

void gsgl_polygon3d_with_normals (float *x, float *y, float *z,
				  float *nx, float *ny, float *nz, int4 n, gsglPage *p)
{
	int i;

	if (n < 3) return;

	gsgl_base_begin_objects_3d (p);
	gsgl_drv_beginpolygon (p);
	for (i=0; i<n; i++) {
		gsgl_drv_normal (nx[i], ny[i], nz[i], p);
		gsgl_drv_vertex3 (x[i], y[i], z[i], p);
	}
	gsgl_drv_endpolygon (p);

	gsgl_move3d (x[n-1], y[n-1], z[n-1], p);
}

void gsgl_rgbtriangle3d (float *x, float *y, float *z, float *r, float *g, float *b, gsglPage *p)
{
	int i;
	float normal[3];

	gsgl_set_normal_vector1 (x, y, z, normal);

	gsgl_base_begin_objects_3d (p);
	gsgl_drv_beginrgbtriangle (p);
	gsgl_drv_glonly_normal (normal[0], normal[1], normal[2], p);
	for (i=0; i<3; i++) {
		gsgl_drv_rgb (r[i], g[i], b[i], p);
		gsgl_drv_vertex3 (x[i], y[i], z[i], p);
	}
	gsgl_drv_endrgbtriangle (p);

	gsgl_move3d (x[2], y[2], z[2], p);
}

void gsgl_rgbtriangle3d_with_normal (float *x, float *y, float *z, float *r, float *g, float *b,
				     float nx, float ny, float nz, gsglPage *p)
{
	int i;

	gsgl_base_begin_objects_3d (p);
	gsgl_drv_beginrgbtriangle (p);
	gsgl_drv_normal (nx, ny, nz, p);
	for (i=0; i<3; i++) {
		gsgl_drv_rgb (r[i], g[i], b[i], p);
		gsgl_drv_vertex3 (x[i], y[i], z[i], p);
	}
	gsgl_drv_endrgbtriangle (p);

	gsgl_move3d (x[2], y[2], z[2], p);
}

void gsgl_rgbtriangle3d_with_normals (float *x,float *y, float *z, float *r, float *g, float *b,
				      float *nx, float *ny, float *nz, gsglPage *p)
{
	int i;

	gsgl_base_begin_objects_3d (p);
	gsgl_drv_beginrgbtriangle (p);
	for (i=0; i<3; i++) {
		gsgl_drv_rgb (r[i], g[i], b[i], p);
		gsgl_drv_normal (nx[i], ny[i], nz[i], p);
		gsgl_drv_vertex3 (x[i], y[i], z[i], p);
	}
	gsgl_drv_endrgbtriangle (p);

	gsgl_move3d (x[2], y[2], z[2], p);
}

/* OpenGL Client State */

void gsgl_glarray_element (int i, gsglPage *p)
{
	gsgl_drv_array_element (i, p);
}

void gsgl_glarray3d_begin_polygon (gsglPage *p)
{
	gsgl_base_begin_objects_3d_fileonly (p);
	gsgl_drv_glarray_begin_polygon (p);
}

void gsgl_glarray3d_end_polygon (gsglPage *p)
{
	gsgl_base_begin_objects_3d_fileonly (p);
	gsgl_drv_glarray_end_polygon (p);
}

void gsgl_glarray3d_begin_lines (gsglPage *p)
{
	gsgl_base_begin_objects_3d_fileonly (p);
	gsgl_drv_glarray_begin_lines (p);
}

void gsgl_glarray3d_end_lines (gsglPage *p)
{
	gsgl_base_begin_objects_3d_fileonly (p);
	gsgl_drv_glarray_end_lines (p);
}

void gsgl_glarray3d_vertex (int datatype, int size, unsigned int totalsize, float *buffer,
			    int nx, int ny, 
			    float *r, float *t,
			    float xmin, float xmax, float ymin, float ymax,
			    int *idata, float *fdata,
			    gsglPage *p)
{
	gsgl_displaylist_begin (GSGL_LIST_3DOBJ_FILEONLY, p);
	gsgl_drv_glarray_vertex (datatype, size, totalsize, buffer,
				 nx, ny, 
				 r, t,
				 xmin, xmax, ymin, ymax,
				 idata, fdata, p);
}

void gsgl_glarray3d_color (int size, unsigned int totalsize, float *buffer, gsglPage *p)
{
	gsgl_displaylist_begin (GSGL_LIST_3DOBJ_FILEONLY, p);
	gsgl_drv_glarray_color (size, totalsize, buffer, p);
}

void gsgl_glarray3d_normal (int datatype, int size, unsigned int totalsize, float *buffer, gsglPage *p)
{
	gsgl_displaylist_begin (GSGL_LIST_3DOBJ_FILEONLY, p);
	gsgl_drv_glarray_normal (datatype, size, totalsize, buffer, p);
}

/* others */

void gsgl_begin_header (gsglPage *p)
{
	gsgl_displaylist_init (p);
	gsgl_drv_pageinfo (p);
	gsgl_displaylist_end (p);
	gsgl_displaylist_begin (GSGL_LIST_BEGIN_HEADER, p);
	gsgl_drv_begin_header (p);
	gsgl_displaylist_end (p);
}

void gsgl_end_header (gsglPage *p)
{
	gsgl_displaylist_end (p);
	gsgl_displaylist_begin (GSGL_LIST_END_HEADER, p);
	gsgl_drv_end_header (p);
	gsgl_displaylist_end (p);
}

/* postscript */

void gsgl_ps_gouraudonly_begin (gsglPage *p)
{
	gsgl_displaylist_begin (GSGL_LIST_STATUS, p);
	gsgl_drv_ps_gouraudonly_begin (p);
}

void gsgl_ps_gouraudonly_end (gsglPage *p)
{
	gsgl_displaylist_begin (GSGL_LIST_STATUS, p);
	gsgl_drv_ps_gouraudonly_end (p);
}

void gsgl_ps_depth_by_deep (gsglPage *p)
{
	gsgl_displaylist_begin (GSGL_LIST_STATUS, p);
	gsgl_drv_ps_depth_by_deep (p);
}

void gsgl_ps_depth_by_shallow (gsglPage *p)
{
	gsgl_displaylist_begin (GSGL_LIST_STATUS, p);
	gsgl_drv_ps_depth_by_shallow (p);
}

void gsgl_ps_depth_by_mean (gsglPage *p)
{
	gsgl_displaylist_begin (GSGL_LIST_STATUS, p);
	gsgl_drv_ps_depth_by_mean (p);
}

void gsgl_ps_depth_by_vectprod (float *x, float *y, float *z, gsglPage *p)
{
	int i;

	gsgl_coordinate_inq (&i, p);
	if (i != GSGL_COORD_3D)
		gsgl_3dmode (p);
	gsgl_displaylist_begin (GSGL_LIST_STATUS, p);
	gsgl_drv_ps_depth_by_vectprod (x, y, z, p);
}

void gsgl_ps_depth_by_vectprod2 (float *x, float *y, float *z,
				 unsigned int npoints, float *xp, float *yp, float *zp,
				 gsglPage *p)
{
	int i;

	gsgl_coordinate_inq (&i, p);
	if (i != GSGL_COORD_3D)
		gsgl_3dmode (p);
	gsgl_displaylist_begin (GSGL_LIST_STATUS, p);
	gsgl_drv_ps_depth_by_vectprod2 (x, y, z, npoints, xp, yp, zp, p);
}

void gsgl_ps_depth_by_point (float x, float y, float z, gsglPage *p)
{
	/* このtokenは後に続く3d objectsと同じdisplaylistに入らなければならない */
	gsgl_base_begin_objects_3d (p);
	gsgl_drv_ps_depth_by_point (x, y, z, p);
}

void gsgl_pstext (int ifnts, int ifntr, float chh,float chw, float chsp, float angl, float tilt,
		  float x, float y, int dim, unsigned int *iascs, int nmax, 
		  gsglboolean tcpip_bitmap, gsglPage *p)
{
	/*gsgl_displaylist_end (p);*/
	if (dim == 1) {
		gsgl_base_begin_objects_normal (p);
	} else if (dim == 2) {
		gsgl_set_text2d_matrix_directly (p->base->Allstatus.modelview_matrix, p);
		gsgl_base_begin_objects_text2d (p);
	} else if (dim == 3) {
		gsgl_set_text3d_matrix_directly (p->base->Allstatus.modelview_matrix, p);
		gsgl_base_begin_objects_text3d (p);
	} else {
		gsgl_error ("gsgl_pstext", "dim = %d", dim);
	}
	gsgl_drv_pstext (ifnts, ifntr, chh, chw, chsp, angl, tilt,
			 x, y, dim, iascs, nmax, tcpip_bitmap, p);
}

static void gui_objects (void (*func)(guialldata *), void (*func2)(guialldata *), guialldata *data)
{
	gsglboolean gl, mem, hd, list;
	gsgllist *dlist;
	gsglPage *p = data->common->p;

	if (!gsgl_inq_page_is_open ()) return;

	gsgl_displaylist_begin_guiitems (p);
	dlist = gsgl_displaylist_inq_last (p);
	data->common->displaylist = dlist->data;

	gl = gsgl_inq_drv_gl (p);
	if (gl) 
		gsgl_disable_drv_gl (p);
	func (data);
	if (gl) 
		gsgl_enable_drv_gl (p);

	if (gsgl_inq_mode (p->library) == GSGL_MODE_TCPIP)
		gsgl_guiitem_append_item (TK_TCPIP, data, NULL, NULL);

	if (gsgl_inq_guiitems_external (p) && gsgl_inq_mode (p->library) == GSGL_MODE_WINDOW) {
		func2 (data);
	} else if ((gl && !gsgl_inq_guiitems_external (p)) || 
		   gsgl_inq_devicetype (p->library) == GSGL_DEVICE_PS) {
		list = gsgl_inq_displaylist_is_enabled (p);
		mem = gsgl_inq_drv_mem (p);
		hd = gsgl_inq_drv_hd (p);

		if (mem) gsgl_disable_drv_mem (p);
		if (hd) gsgl_disable_drv_hd (p);
		if (list) gsgl_disable_displaylist (p);

		func (data);

		if (mem) gsgl_enable_drv_mem (p);
		if (hd) gsgl_enable_drv_hd (p);
		if (list) gsgl_enable_displaylist (p);
	}
	gsgl_displaylist_end (p);	
}

void gsgl_gui_label (itemdata *itemdata1, char *text, int ijust)
{
	guialldata *data;
	labeldata *s;

	data = (guialldata *) gsgl_malloc (sizeof(guialldata));
	data->common = itemdata1;

	s = (labeldata *) gsgl_malloc (sizeof(labeldata));
	s->text = text;
	s->ijust = ijust;

	data->specific = s;

	gui_objects (gsgl_drv_gui_label, gsgl_add_label_external, data);
}

void gsgl_gui_button (itemdata *itemdata1, char *text, void (*callback)(void))
{
	guialldata *data;
	buttondata *s;
	data = (guialldata *) gsgl_malloc (sizeof(guialldata));
	data->common = itemdata1;

	s = (buttondata *) gsgl_malloc (sizeof(buttondata));
	s->text = text;
	s->callback = callback;

	data->specific = s;

	gui_objects (gsgl_drv_gui_button, gsgl_add_button_external, data);
}

void gsgl_gui_checkbutton (itemdata *itemdata1, char *text, int init, void (*callback)(int *))
{
	guialldata *data;
	checkbuttondata *s;

	data = (guialldata *) gsgl_malloc (sizeof(guialldata));
	data->common = itemdata1;

	s = (checkbuttondata *) gsgl_malloc (sizeof(checkbuttondata));
	s->text = text;
	s->init = init;
	s->callback = callback;

	data->specific = s;

	gui_objects (gsgl_drv_gui_checkbutton, gsgl_add_checkbutton_external, data);
}

void gsgl_gui_combobox (itemdata *itemdata1, int items, char *text[], int current, void (*callback)(int *), int fortran)
{
	guialldata *data;
	comboboxdata *s;

	data = (guialldata *) gsgl_malloc (sizeof(guialldata));
	data->common = itemdata1;

	s = (comboboxdata *) gsgl_malloc (sizeof(comboboxdata));
	s->items = items;
	s->text = text;
	s->current = current;
	s->callback = callback;
	s->fortran = fortran;

	data->specific = s;

	gui_objects (gsgl_drv_gui_combobox, gsgl_add_combobox_external, data);
}

void gsgl_gui_lineedit (itemdata *itemdata1, int max, char *text, void (*callback)(const char *, int *))
{
	guialldata *data;
	lineeditdata *s;

	data = (guialldata *) gsgl_malloc (sizeof(guialldata));
	data->common = itemdata1;

	s = (lineeditdata *) gsgl_malloc (sizeof(lineeditdata));
	s->max = max;
	s->text = text;
	s->callback = callback;

	data->specific = s;

	gui_objects (gsgl_drv_gui_lineedit, gsgl_add_lineedit_external, data);
}

void gsgl_gui_spinbuttonf (itemdata *itemdata1, float init, float lower, float upper,
			   int digits, void (*callback)(float *))
{
	guialldata *data;
	spinbuttondataf *s;

	data = (guialldata *) gsgl_malloc (sizeof(guialldata));
	data->common = itemdata1;

	s = (spinbuttondataf *) gsgl_malloc (sizeof(spinbuttondataf));
	s->init = init;
	s->lower = lower;
	s->upper = upper;
	s->digits = digits;
	s->callback = callback;

	data->specific = s;

	gui_objects (gsgl_drv_gui_spinbuttonf, gsgl_add_spinbuttonf_external, data);
}

void gsgl_gui_spinbuttoni (itemdata *itemdata1, int init, int lower, int upper,
			   void (*callback)(int *))
{
	guialldata *data;
	spinbuttondatai *s;

	data = (guialldata *) gsgl_malloc (sizeof(guialldata));
	data->common = itemdata1;

	s = (spinbuttondatai *) gsgl_malloc (sizeof(spinbuttondatai));
	s->init = init;
	s->lower = lower;
	s->upper = upper;
	s->callback = callback;

	data->specific = s;

	gui_objects (gsgl_drv_gui_spinbuttoni, gsgl_add_spinbuttoni_external, data);
}

void gsaf_guilabel (float x, float y, float w, float h, char *text, int ijust)
{
	itemdata *data = gsgl_malloc (sizeof(itemdata));
	data->x = x;
	data->y = y;
	data->w = w;
	data->h = h;
	gsaf_inqfnt (&data->fnt);
	gsaf_inqchr (&data->chh, &data->chw, &data->chsp, &data->angl, &data->tilt);
	data->type = ITEM_LABEL;
	data->displaylist = NULL;
	data->p = gsgl_inq_page ();

	gsgl_gui_label (data, text, ijust);
}

void gsaf_guibutton (float x, float y, float w, float h, char *text, void (*callback)(void))
{
	itemdata *data = gsgl_malloc (sizeof(itemdata));
	data->x = x;
	data->y = y;
	data->w = w;
	data->h = h;
	gsaf_inqfnt (&data->fnt);
	gsaf_inqchr (&data->chh, &data->chw, &data->chsp, &data->angl, &data->tilt);
	data->type = ITEM_BUTTON;
	data->displaylist = NULL;
	data->p = gsgl_inq_page ();

	gsgl_gui_button (data, text, callback);
}

void gsaf_guicheckbutton (float x, float y, float w, float h, char *text, int init, void (*callback)(int *))
{
	itemdata *data = gsgl_malloc (sizeof(itemdata));
	data->x = x;
	data->y = y;
	data->w = w;
	data->h = h;
	gsaf_inqfnt (&data->fnt);
	gsaf_inqchr (&data->chh, &data->chw, &data->chsp, &data->angl, &data->tilt);
	data->type = ITEM_CHECK;
	data->displaylist = NULL;
	data->p = gsgl_inq_page ();

	gsgl_gui_checkbutton (data, text, init, callback);
}

void gsaf_guicombobox (float x, float y, float w, float h, int items, char *text[], int current, void (*callback)(int *), int fortran)
{
	itemdata *data = gsgl_malloc (sizeof(itemdata));
	data->x = x;
	data->y = y;
	data->w = w;
	data->h = h;
	gsaf_inqfnt (&data->fnt);
	gsaf_inqchr (&data->chh, &data->chw, &data->chsp, &data->angl, &data->tilt);
	data->type = ITEM_COMBO;
	data->displaylist = NULL;
	data->p = gsgl_inq_page ();

	gsgl_gui_combobox (data, items, text, current, callback, fortran);
}

void gsaf_guilineedit (float x, float y, float w, float h, int max, char *text, void (*callback)(const char *, int *))
{
	itemdata *data = gsgl_malloc (sizeof(itemdata));
	data->x = x;
	data->y = y;
	data->w = w;
	data->h = h;
	gsaf_inqfnt (&data->fnt);
	gsaf_inqchr (&data->chh, &data->chw, &data->chsp, &data->angl, &data->tilt);
	data->type = ITEM_EDIT;
	data->displaylist = NULL;
	data->p = gsgl_inq_page ();

	gsgl_gui_lineedit (data, max, text, callback);
}

void gsaf_guispinbuttonf (float x, float y, float w, float h, float init, float lower, float upper, int digits, void (*callback)(float *))
{
	itemdata *data = gsgl_malloc (sizeof(itemdata));
	data->x = x;
	data->y = y;
	data->w = w;
	data->h = h;
	gsaf_inqfnt (&data->fnt);
	gsaf_inqchr (&data->chh, &data->chw, &data->chsp, &data->angl, &data->tilt);
	data->type = ITEM_SPINF;
	data->displaylist = NULL;
	data->p = gsgl_inq_page ();

	gsgl_gui_spinbuttonf (data, init, lower, upper, digits, callback);
}

void gsaf_guispinbuttoni (float x, float y, float w, float h, int init, int lower, int upper, void (*callback)(int *))
{
	itemdata *data = gsgl_malloc (sizeof(itemdata));
	data->x = x;
	data->y = y;
	data->w = w;
	data->h = h;
	gsaf_inqfnt (&data->fnt);
	gsaf_inqchr (&data->chh, &data->chw, &data->chsp, &data->angl, &data->tilt);
	data->type = ITEM_SPINI;
	data->displaylist = NULL;
	data->p = gsgl_inq_page ();

	gsgl_gui_spinbuttoni (data, init, lower, upper, callback);
}

void gsaf_guiexternal (gsglboolean b)
{
	gsglPage *p = gsgl_inq_page ();

	gsgl_displaylist_begin_guiitems (p);
	gsgl_drv_set_external_gui (b, p);
	gsgl_set_guiitems_external (b, p);
	gsgl_displaylist_end (p);	
}
