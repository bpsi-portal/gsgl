#include "gsaf.h"
#include "gsgl.h"
#include "matrix.h"
#include "ogl.h"
#include "displaylist.h"
#include "base.h"
#include "bsc1.h"
#include "drv_wrapper.h"
#include "drv_gl.h"
#include "utils.h"
#include "xwin.h"

void gsgl_projmatrix_set_ortho (float eyepos, gsglPage *p)
{
	float mat1[16], mat2[16], mat3[16];
	int i;
	gsglMatrices *m = p->matrix;

	/*gsgl_displaylist_end ();*/

	gsgl_inq_matrix_orthogonal (-0.5*GSGL_PAPER_WIDTH,
				     0.5*GSGL_PAPER_WIDTH,
				    -0.5*GSGL_PAPER_HEIGHT,
				     0.5*GSGL_PAPER_HEIGHT,
				    eyepos, 
				    eyepos+GSGL_VIEW_DEPTH,
				    mat1);

	gsgl_inq_matrix_translate (-0.5*GSGL_PAPER_WIDTH, -0.5*GSGL_PAPER_HEIGHT, 
				   -eyepos-EYEPOS_DELTA, mat2);
	gsgl_mat16_mult_mat16 (mat1, mat2, mat3);

	for (i=0; i<16; i++)
		m->Matrix_orth[i] = mat3[i];
}

void gsgl_projmatrix_set_frustum (float eyepos, gsglPage *p)
{
	float mat1[16], mat2[16], mat3[16];
	int i;
	gsglMatrices *m = p->matrix;

	/*gsgl_displaylist_end ();*/

	gsgl_inq_matrix_frustum (-0.5*GSGL_PAPER_WIDTH, 
				  0.5*GSGL_PAPER_WIDTH,
				 -0.5*GSGL_PAPER_HEIGHT,
				  0.5*GSGL_PAPER_HEIGHT,
				 eyepos,
				 eyepos+GSGL_VIEW_DEPTH, 
				 mat1);

	gsgl_inq_matrix_translate (-0.5*GSGL_PAPER_WIDTH, -0.5*GSGL_PAPER_HEIGHT,
				   -eyepos-EYEPOS_DELTA, mat2);
	gsgl_mat16_mult_mat16 (mat1, mat2, mat3);

	for (i=0; i<16; i++)
		m->Matrix_frus[i] = mat3[i];
}

void gsgl_projmatrix_set (gsglPage *p)
{
	GLfloat tmp[16];
	gsglMatrices *m = p->matrix;

	gsgl_mat16_mult_mat16 (m->Matrix_imag, m->Matrix_view, tmp);

	if (gsgl_inq_projectiontype (p) == GSGL_ORTHOGONAL) {
		gsgl_mat16_mult_mat16 (tmp, m->Matrix_orth, m->Matrix_proj);
	} else if (gsgl_inq_projectiontype (p) == GSGL_FRUSTUM) {
		gsgl_mat16_mult_mat16 (tmp, m->Matrix_frus, m->Matrix_proj);
	}
}

/* Matrixのキャッシュの初期化 */
void gsgl_init_matrix_cache (gsglPage *p)
{
	int i;
	GLfloat matrix[16] = {1.0, 0.0, 0.0, 0.0, 
			      0.0, 1.0, 0.0, 0.0, 
			      0.0, 0.0, 1.0, 0.0, 
			      0.0, 0.0, 0.0, 1.0};
	gsglMatrices *m = p->matrix;

	for (i=0; i<16 ; i++) {
		m->Matrix_proj[i] = matrix[i];
		m->Matrix_view[i] = matrix[i];
		m->Matrix_imag[i] = matrix[i];
		m->Matrix_2d[i] = matrix[i];
		m->Matrix_3d[i] = matrix[i];
		m->Matrix_text2d[i] = matrix[i];
		m->Matrix_text3d[i] = matrix[i];
	}
	m->ProjectionScale = 1.0;
	m->CoordinateMode = GSGL_COORD_NULL;

	gsgl_projmatrix_set_ortho (GSGLBASIC_EYEPOSITION, p);
	gsgl_projmatrix_set_frustum (GSGLBASIC_EYEPOSITION, p);
	gsgl_projmatrix_set (p);

	m->Xoffset = 0.0; 
	m->Yoffset = 0.0;
	m->Xscale = 1.0;
	m->Yscale = 1.0;
}

gsglboolean gsgl_inq_view_matrix_is_unity (gsglPage *p)
{
	gsglboolean i = GSGL_TRUE, j;
	GLfloat matrix[16] = {1.0, 0.0, 0.0, 0.0, 
			      0.0, 1.0, 0.0, 0.0, 
			      0.0, 0.0, 1.0, 0.0, 
			      0.0, 0.0, 0.0, 1.0};
	gsglMatrices *m = p->matrix;
	
	for (j=0; j<16; j++)
		if (fabs(m->Matrix_view [j] - matrix [j]) > FLOAT_EPS) i = GSGL_FALSE;
	return i;
}

void gsgl_load_projection_matrix (gsglPage *p)
{
	gsglMatrices *m = p->matrix;

	gsgl_projection_matrix (m->Matrix_proj, p);
}

void gsgl_projmatrix_clean (gsglPage *p)
{
	gsglMatrices *m = p->matrix;

	if (gsgl_inq_projectiontype (p) == GSGL_ORTHOGONAL) {
		gsgl_projection_matrix (m->Matrix_orth, p);
	} else if (gsgl_inq_projectiontype (p) == GSGL_FRUSTUM) {
		gsgl_projection_matrix (m->Matrix_frus, p);
	}	
}

void gsgl_projmatrix_without_viewmatrix (gsglPage *p)
{
	GLfloat tmp[16];
	gsglMatrices *m = p->matrix;

	if (gsgl_inq_projectiontype (p) == GSGL_ORTHOGONAL) {
		gsgl_mat16_mult_mat16 (m->Matrix_imag, m->Matrix_orth, tmp);
	} else if (gsgl_inq_projectiontype (p) == GSGL_FRUSTUM) {
		gsgl_mat16_mult_mat16 (m->Matrix_imag, m->Matrix_frus, tmp);
	}
	gsgl_projection_matrix (tmp, p);
}

static void gsgl_projimage_matrix_mult (GLfloat matrix [16], gsglPage *p)
{
	int i, j;
	GLfloat tmp [16];
	int i1, i2;
	gsglMatrices *m = p->matrix;

	for (i = 0; i<16; i++) {
		i1 = i%4;
		i2 = i/4;
		tmp [i] = 0.0;
		for (j=0; j<4; j++) {
			tmp [i] = tmp [i] + matrix [i1+j*4] * m->Matrix_imag [j+i2*4];
		}
	}
	for (i = 0; i<16; i++) {
		m->Matrix_imag [i] = tmp [i];
	}
	gsgl_projmatrix_set (p);
	gsgl_load_projection_matrix (p);
}

void gsgl_projimg_translate (float dx, float dy)
{
	float tmp[16];
	gsglPage *p = gsgl_inq_page ();

	gsgl_displaylist_end (p);

	gsgl_inq_matrix_translate (2.0*dx/GSGL_PAPER_WIDTH, 2.0*dy/GSGL_PAPER_HEIGHT,
				   0.0, tmp);
	gsgl_projimage_matrix_mult (tmp, p);
}

void gsgl_projimg_scale (float xratio, float yratio)
{
	float tmp[16];
	gsglPage *p = gsgl_inq_page();
	gsglMatrices *m = p->matrix;

	gsgl_displaylist_end (p);

	gsgl_inq_matrix_scale (xratio, yratio, 1.0, tmp);
	gsgl_projimage_matrix_mult (tmp, p);

	m->ProjectionScale = sqrt(xratio * yratio);
	if (m->ProjectionScale <= 1.e-5) m->ProjectionScale = 1.0;
}

void gsgl_inq_projection_scale (float *scale, gsglPage *p)
{
	gsglMatrices *m = p->matrix;

	*scale = m->ProjectionScale;
}

void gsgl_projimg_rotate (float degree)
{
	float tmp[16];
	gsglPage *p = gsgl_inq_page();

	gsgl_displaylist_end (p);

	gsgl_inq_matrix_rotate (degree, 0.0, 0.0, 1.0, tmp);
	gsgl_projimage_matrix_mult (tmp, p);
}

void gsgl_projimg_identity ()
{
	int i;
	GLfloat identity[16] = {1.0, 0.0, 0.0, 0.0,
				0.0, 1.0, 0.0, 0.0,
				0.0, 0.0, 1.0, 0.0,
				0.0, 0.0, 0.0, 1.0};
	gsglPage *p = gsgl_inq_page();
	gsglMatrices *m = p->matrix;

	for (i=0; i<16; i++) m->Matrix_imag[i] = identity[i];
	gsgl_projmatrix_set (p);
	gsgl_load_projection_matrix (p);

	m->ProjectionScale = 1.0;
}

/**/

void gsgl_set_2d_coord (float pxmax, float pxmin, float pymax, float pymin,
			float gxmax, float gxmin, float gymax, float gymin,
			gsglPage *p)
{
	float mat1[16], mat2[16], mat3[16];
	int i;
	gsglMatrices *m = p->matrix;

	gsgl_displaylist_end (p);

	/* --- ここからは下から順に読んだ方が理解しやすい --- */

	/* --- 最後に視点を画面中央に戻す --- */
	gsgl_inq_matrix_translate (pxmin, pymin, 0.0, mat1);

	/* --- scale from data coordinate to cm (2D) coordnate --- */	
	gsgl_inq_matrix_scale ((pxmax-pxmin)/(gxmax-gxmin), (pymax-pymin)/(gymax-gymin), 1.0, mat2);
	gsgl_mat16_mult_mat16 (mat1, mat2, mat3);

	/* --- 左下の角を原点へ移動 --- */
	gsgl_inq_matrix_translate (-gxmin, -gymin, 0.0, mat1);
	gsgl_mat16_mult_mat16 (mat3, mat1, mat2);

	/* --- ここまで --- */
	for (i=0; i<16; i++) m->Matrix_2d[i] = mat2[i];

	gsgl_2dmode (p);
}

void gsgl_set_3d_coord (float wx, float wy, float depth,
			float xl, float yl, float zl, 
			float gxmax, float gxmin, 
			float gymax, float gymin, 
			float gzmax, float gzmin,
			float ox, float oy, float oz,
			float angle1, float vn1[3], float angle2, float vn2[3],
			gsglPage *p)
{
	float mat1[16], mat2[16], mat3[16];
	int i;
	gsglMatrices *m = p->matrix;

	gsgl_displaylist_end (p);

	/* --- ここからは下から読んだ方が理解しやすい --- */

	/* --- 最後に視点を画面中央に戻す --- */
	gsgl_inq_matrix_translate (wx, wy, -depth, mat1);

	/* --- rotate coordiante --- */
	gsgl_inq_matrix_rotate (angle2, vn2[0], vn2[1], vn2[2], mat2);
	gsgl_mat16_mult_mat16 (mat1, mat2, mat3);

	gsgl_inq_matrix_rotate (angle1, vn1[0], vn1[1], vn1[2], mat1);
	gsgl_mat16_mult_mat16 (mat3, mat1, mat2);

	/* --- scale from data coordinate to cm (3D) coordnate --- */
        gsgl_inq_matrix_scale (xl/(gxmax-gxmin), yl/(gymax-gymin), zl/(gzmax-gzmin), mat1);
	gsgl_mat16_mult_mat16 (mat2, mat1, mat3);

        /* まず(ox, oy, oz)を原点に持ってくる．*/
	gsgl_inq_matrix_translate (-ox, -oy, -oz, mat1);
	gsgl_mat16_mult_mat16 (mat3, mat1, mat2);

	/* --- ここまで --- */
	for (i=0; i<16; i++) m->Matrix_3d[i] = mat2[i];

	gsgl_3dmode (p);
}

void gsgl_set_text2d_matrix (float xworld, float yworld, gsglPage *p)
{
	float mat1[16];
	int i;
	gsglMatrices *m = p->matrix;

	gsgl_displaylist_end (p);

	gsgl_inq_matrix_translate (xworld, yworld, 0.0, mat1);
	
	for (i=0; i<16; i++) m->Matrix_text2d[i] = mat1[i];

	gsgl_text2dmode (p);
}

void gsgl_set_text2d_matrix_directly (float mat[16], gsglPage *p)
{
	int i;
	gsglMatrices *m = p->matrix;

	for (i=0; i<16; i++) m->Matrix_text2d[i] = mat[i];
}

void gsgl_set_text3d_matrix_directly (float mat[16], gsglPage *p)
{
	int i;
	gsglMatrices *m = p->matrix;

	for (i=0; i<16; i++) m->Matrix_text3d[i] = mat[i];
}


void gsgl_set_text3d_matrix (float xworld, float yworld, float zworld,
			     float angle1, float vn1[3], float angle2, float vn2[3],
			     float angle3, float vn3[3], float angle4, float vn4[3],
			     gsglPage *p)
{
	float mat1[16], mat2[16], mat3[16];
	int i;
	gsglMatrices *m = p->matrix;

	gsgl_displaylist_end (p);

        /* --- 最後に move3d の位置に平行移動 --- */
	gsgl_inq_matrix_translate (xworld, yworld, zworld, mat1);

	/* --- 次に3次元座標系に回転 --- */
	gsgl_inq_matrix_rotate (angle2, vn2[0], vn2[1], vn2[2], mat2);
	gsgl_mat16_mult_mat16 (mat1, mat2, mat3);

	gsgl_inq_matrix_rotate (angle1, vn1[0], vn1[1], vn1[2], mat1);
	gsgl_mat16_mult_mat16 (mat3, mat1, mat2);

	/* --- まずはじめに world 座標系で x-y 平面にあるテキストを --- */
	/* --- Set3DTextBaseLine で与えられた平面に射影 --- */
	gsgl_inq_matrix_rotate (angle4, vn4[0], vn4[1], vn4[2], mat1);
	gsgl_mat16_mult_mat16 (mat2, mat1, mat3);

	gsgl_inq_matrix_rotate (angle3, vn3[0], vn3[1], vn3[2], mat1);
	gsgl_mat16_mult_mat16 (mat3, mat1, mat2);

	for (i=0; i<16; i++) m->Matrix_text3d[i] = mat2[i];
	
	gsgl_text3dmode (p);
}

void gsgl_set_text3d_matrix_normal (float xworld, float yworld, float zworld, gsglPage *p)
{
	float mat1[16];
	int i;
	gsglMatrices *m = p->matrix;

	gsgl_displaylist_end (p);

	gsgl_inq_matrix_translate (xworld, yworld, zworld, mat1);
	
	for (i=0; i<16; i++) m->Matrix_text3d[i] = mat1[i];

	gsgl_text3dmode (p);
}

void gsglcleanprojmatrix ()
{
	gsgl_projmatrix_clean (gsgl_inq_page());
}

void gsgl_clean_view_matrix (gsglPage *p)
{
#ifdef GSGL_DEBUG_PROJECTIONMATRIX
	fprintf (stderr, "gsgl_clean_view_matrix\n");
#endif
	gsgl_projmatrix_without_viewmatrix (p);
}

void gsgl_matrix_inq_2dmatrix (float matrix[16], gsglPage *p)
{
	int i;
	gsglMatrices *m = p->matrix;
	
	for (i=0; i<16; i++) matrix[i] = m->Matrix_2d [i];
}

void gsgl_matrix_inq_3dmatrix (float matrix [16], gsglPage *p)
{
	int i;
	gsglMatrices *m = p->matrix;

	for (i=0; i<16; i++) matrix[i] = m->Matrix_3d [i];
}

void gsgl_matrix_inq_text2dmatrix (float matrix [16], gsglPage *p)
{
	int i;
	gsglMatrices *m = p->matrix;

	for (i=0; i<16; i++) matrix[i] = m->Matrix_text2d [i];
}

void gsgl_matrix_inq_text3dmatrix (float matrix [16], gsglPage *p)
{
	int i;
	gsglMatrices *m = p->matrix;

	for (i=0; i<16; i++) matrix[i] = m->Matrix_text3d [i];
}

void gsgl_coordinate_set (int i, gsglPage *p)
{
	gsglMatrices *m = p->matrix;

	m->CoordinateMode = i;
}

void gsgl_coordinate_inq (int *i, gsglPage *p) 
{
	gsglMatrices *m = p->matrix;

	*i = m->CoordinateMode;
}

/* 与えられた2次元の点を，World座標系に変換する */
void gsglproject2d (float x, float y, float *x_w, float *y_w, gsglPage *page)
{
	int       i;
	GLfloat   p0 [4];
	GLfloat   p [4] = {0.0, 0.0, 0.0, 0.0};
	gsglMatrices *m = page->matrix;  

	p0 [0] = x;
	p0 [1] = y;
	p0 [2] = 0.0;
	p0 [3] = 1.0;
	
	for (i = 0; i<16 ; i ++)
		p[i%4] = p[i%4] + m->Matrix_2d[i] * p0[i/4]; 

	*x_w = p[0];
	*y_w = p[1];
}

/* Transform a given point in world coordinates to 2d coordinates */
void gsglunproject2d (float xw, float yw, float *x2d, float *y2d, gsglPage *p)
{
	int       i;
	GLdouble  modelMatrix[16];
	GLdouble  projMatrix[16];
	GLint     viewport[4];
	GLdouble  x, y, z;
	gsglMatrices *m = p->matrix;

	for (i = 0; i<16 ; i ++) {
		projMatrix[i] = 0.0;
		modelMatrix[i] = (double) m->Matrix_2d[i];
	}
	projMatrix[0] = 2.0/((double) GSGL_PAPER_WIDTH);
	projMatrix[5] = 2.0/((double) GSGL_PAPER_HEIGHT);
	projMatrix[10] = -2.0/((double) GSGL_VIEW_DEPTH);
	projMatrix[12] = projMatrix[13] = projMatrix[14] = -1.0;
	projMatrix[15] = 1.0;
	
	viewport[0] = viewport[1] = 0;
	viewport[2] = viewport[3] = 1;
	
	i = gluUnProject ((double) (xw/GSGL_PAPER_WIDTH),
			  (double) (yw/GSGL_PAPER_HEIGHT),
			  (double) (0.0/(-GSGL_VIEW_DEPTH)), 
			  modelMatrix, projMatrix, viewport, &x, &y, &z);
	if (i == GL_FALSE) gsgl_warning ("gsglunproject2d", "GL_FALSE");

	*x2d = (GLfloat) x;
	*y2d = (GLfloat) y;
}

/* Transform a given point in text2d coordinates to world coordinates */
void gsglprojecttext2d (float x, float y, float *x2, float *y2, gsglPage *page)
{
	int       i;
	GLfloat   p0 [4];
	GLfloat   p [4] = {0.0, 0.0, 0.0, 0.0};
	gsglMatrices *m = page->matrix;  

	p0 [0] = x;
	p0 [1] = y;
	p0 [2] = 0.0;
	p0 [3] = 1.0;
	
	for (i = 0; i<16 ; i ++)
		p[i%4] = p[i%4] + m->Matrix_text2d[i] * p0[i/4]; 

	*x2 = p[0];
	*y2 = p[1];
}

/* 与えられた3次元の点を，World座標系に変換する */
void gsglproject3d (float x, float y, float z, float *x_w, float *y_w, float *z_w, gsglPage *page)
{
	int       i;
	GLfloat   p0[4];
	GLfloat   p[4] = {0.0, 0.0, 0.0, 0.0};
	gsglMatrices *m = page->matrix;
  
	p0[0] = x;
	p0[1] = y;
	p0[2] = z;
	p0[3] = 1.0;
	
	for (i = 0; i<16 ; i ++)
		p[i%4] = p[i%4] + m->Matrix_3d[i] * p0[i/4]; 

	*x_w = p[0];
	*y_w = p[1];
	*z_w = p[2];
}

/* 与えられた world 座標系の点を 3D 座標系に変換する */
void gsglunproject3d (float xw,  float yw,  float zw, float *x3d, float *y3d, float *z3d, gsglPage *p)
{
	int       i;
	GLdouble  modelMatrix[16];
	GLdouble  projMatrix[16];
	GLint     viewport[4];
	GLdouble  x, y, z;
	gsglMatrices *m = p->matrix;

	for (i = 0; i<16 ; i ++) {
		projMatrix[i] = 0.0;
		modelMatrix[i] = (double) m->Matrix_3d[i];
	}
	projMatrix[0] = 2.0/((double) GSGL_PAPER_WIDTH);
	projMatrix[5] = 2.0/((double) GSGL_PAPER_HEIGHT);
	projMatrix[10] = -2.0/((double) GSGL_VIEW_DEPTH);
	projMatrix[12] = projMatrix[13] = projMatrix[14] = -1.0;
	projMatrix[15] = 1.0;
	
	viewport[0] = viewport[1] = 0;
	viewport[2] = viewport[3] = 1;
	
	i = gluUnProject ((double) (xw/GSGL_PAPER_WIDTH),
			  (double) (yw/GSGL_PAPER_HEIGHT),
			  (double) (zw/(-GSGL_VIEW_DEPTH)), 
			  modelMatrix, projMatrix, viewport, &x, &y, &z);
	if (i == GL_FALSE) gsgl_warning ("gsglunproject3d", "GL_FALSE");

	*x3d = (GLfloat) x;
	*y3d = (GLfloat) y;
	*z3d = (GLfloat) z;
}

/* Transform a given point in text3d coordinates to world coordinates */
void gsglprojecttext3d (float x, float y, float *xw, float *yw, float *zw, gsglPage *page)
{
	int       i;
	GLfloat   p0 [4];
	GLfloat   p [4] = {0.0, 0.0, 0.0, 0.0};
	gsglMatrices *m = page->matrix;  

	p0 [0] = x;
	p0 [1] = y;
	p0 [2] = 0.0;
	p0 [3] = 1.0;
	
	for (i = 0; i<16 ; i ++)
		p[i%4] = p[i%4] + m->Matrix_text3d[i] * p0[i/4]; 

	*xw = p[0];
	*yw = p[1];
	*zw = p[2];
}

/* 与えられた World 座標系の点を Window 座標系に変換する */
/* modelview, projection ともにデフォルトの matrix を使用していることに注意 */
void gsgl_project_world (float xworld,  float yworld, float *xwin, float *ywin, gsglWindow *w)
{
	int       i;
	GLdouble  x, y, z;
	GLdouble  model[16];
	GLdouble  proj[16];
	GLint     view[4];
	GLfloat   mat1[16], mat2[16], mat3[16];

	gsgl_inq_matrix_orthogonal 
		(-0.5*GSGL_PAPER_WIDTH, 0.5*GSGL_PAPER_WIDTH,
		 -0.5*GSGL_PAPER_HEIGHT, 0.5*GSGL_PAPER_HEIGHT,
		 GSGLBASIC_EYEPOSITION, 
		 GSGLBASIC_EYEPOSITION+GSGL_VIEW_DEPTH, mat1);
                        
	gsgl_inq_matrix_translate 
		(-0.5*GSGL_PAPER_WIDTH, -0.5*GSGL_PAPER_HEIGHT, 
		 -GSGLBASIC_EYEPOSITION-EYEPOS_DELTA, mat2);
	gsgl_mat16_mult_mat16 (mat1, mat2, mat3);

	for (i=0; i<16; i ++) {
		model[i] = 0.0;
		proj[i] = mat3 [i];
	}
	
	model[0] = model[5] = model[10] = model[15] = 1.0;
	view[0] = view[1] = 0.0;
	view[2] = gsgl_inq_window_width (w);
	view[3] = gsgl_inq_window_height (w);

	gluProject ((double) xworld,(double) yworld, 0.0, 
		    model, proj, view, &x, &y, &z);
	
	*xwin = (GLfloat) x;
	*ywin = (GLfloat) y;
}

/* 与えられた window 座標系の点を World 座標系に変換する */
void gsgl_unproject_world (GLfloat xwin,  GLfloat ywin,  GLfloat zwin,
			   GLfloat *xworld, GLfloat *yworld, GLfloat *zworld,
			   GLdouble proj[], GLint viewport[])
{
	int       i;
	GLdouble  x, y, z;
	GLdouble  model[16];
	
	for (i=0; i<16; i ++)
		model[i] = 0.0;
	
	model[0] = model[5] = model[10] = model[15] = 1.0;
	gluUnProject ((double) xwin,(double) ywin, (double) zwin, 
		      model, proj, viewport,
		      &x, &y, &z);
	
	*xworld = (GLfloat) x;
	*yworld = (GLfloat) y;
	*zworld = (GLfloat) z;
}

/* 与えられた window 座標系の点を World 座標系に変換する */
/* Proj_inv は projection の逆行列 */
/* viewport_inv は view[0], view[1], 1/view[2], 1/view[3] */
void gsgl_unproject_world_inv (GLfloat xwin,  GLfloat ywin,  GLfloat zwin,
			       GLfloat *xworld, GLfloat *yworld, GLfloat *zworld,
			       GLdouble proj_inv[], GLdouble viewport_inv[])
{
	register int       i;
	GLdouble obj1[4] = {0.0, 0.0, 0.0, 1.0};
	GLdouble obj2[4] = {0.0, 0.0, 0.0, 0.0};
	
	obj1[0] = 2.0 * ((GLdouble)xwin-viewport_inv[0]) * viewport_inv[2] - 1.0;
	obj1[1] = 2.0 * ((GLdouble)ywin-viewport_inv[1]) * viewport_inv[3] - 1.0;
	obj1[2] = 2.0 * ((GLdouble) zwin) - 1.0;
	
	for (i = 0 ; i < 16 ; i++)
		obj2[i%4] = obj2[i%4] + obj1[i/4] * proj_inv[i]; 

	*xworld = (GLfloat) (obj2[0]/obj2[3]);
	*yworld = (GLfloat) (obj2[1]/obj2[3]);
	*zworld = (GLfloat) (obj2[2]/obj2[3]);
}

static double lu(int n, double **a, int *ip)  /* LU分解 */
{
    int i, j=0, k, ii, ik;
    double t, u, det;
    double *weight;

    weight = gsgl_malloc_atomic (sizeof(double) * n); /* weight[0..n-1] の記憶領域確保 */

    det = 0;                   /* 行列式 */
    for (k = 0; k < n; k++) {  /* 各行について */
        ip[k] = k;             /* 行交換情報の初期値 */
        u = 0;                 /* その行の絶対値最大の要素を求める */
        for (j = 0; j < n; j++) {
            t = fabs(a[k][j]);  if (t > u) u = t;
        }
        if (u == 0) goto EXIT; /* 0 なら行列はLU分解できない */
        weight[k] = 1 / u;     /* 最大絶対値の逆数 */
    }
    det = 1;                   /* 行列式の初期値 */
    for (k = 0; k < n; k++) {  /* 各行について */
        u = -1;
        for (i = k; i < n; i++) {  /* より下の各行について */
            ii = ip[i];            /* 重み×絶対値 が最大の行を見つける */
            t = fabs(a[ii][k]) * weight[ii];
            if (t > u) {  u = t;  j = i;  }
        }
        ik = ip[j];
        if (j != k) {
            ip[j] = ip[k];  ip[k] = ik;  /* 行番号を交換 */
            det = -det;  /* 行を交換すれば行列式の符号が変わる */
        }
        u = a[ik][k];  det *= u;  /* 対角成分 */
        if (u == 0) goto EXIT;    /* 0 なら行列はLU分解できない */
        for (i = k + 1; i < n; i++) {  /* Gauss消去法 */
            ii = ip[i];
            t = (a[ii][k] /= u);
            for (j = k + 1; j < n; j++)
                a[ii][j] -= t * a[ik][j];
        }
    }
EXIT:
    gsgl_free (weight);  /* 記憶領域を解放 */
    return det;           /* 戻り値は行列式 */
}

double gsgl_matrix_inverse (int n, double *b)
{
	int i, j, k, ii;
	double t, det;
	int *ip;   /* 行交換の情報 */
	double **a;
	double a_inv[4][4];
	
	a = gsgl_malloc ((4 + 1) * sizeof(void *));
	/*if (a == NULL);fprintf(stderr, "記憶領域不足 in gsglMatInv\n");*/
	for (i = 0; i < 4; i++) {
		a[i] = gsgl_malloc_atomic (sizeof(double) * 4);
		if (a[i] == NULL) {
			while (--i >= 0) gsgl_free (a[i]);
			gsgl_free (a);
		}
	}
	a[4] = NULL;  /* 行の数を自動判断するための工夫 */
	
	for (i=0 ; i<16; i++)
		a[i/4][i%4] = b[i];
	
	ip = gsgl_malloc_atomic (sizeof(int) * n);
	/*if (ip == NULL) fprintf(stderr, "記憶領域不足 in gsglMatInv\n");*/
	det = lu(n, a, ip);
	if (det != 0)
		for (k = 0; k < n; k++) {
			for (i = 0; i < n; i++) {
				ii = ip[i];  t = (ii == k);
				for (j = 0; j < i; j++)
					t -= a[ii][j] * a_inv[j][k];
				a_inv[i][k] = t;
			}
			for (i = n - 1; i >= 0; i--) {
				t = a_inv[i][k];  ii = ip[i];
				for (j = i + 1; j < n; j++)
					t -= a[ii][j] * a_inv[j][k];
				a_inv[i][k] = t / a[ii][i];
			}
		}
	gsgl_free (ip);
	b--;
	for (i=0; i<16;i++) {
		++b;
		*b = a_inv[i/4][i%4];
	}
	return det;
}

void gsgl_matrix_setvew (float pxmin, float pxmax, float pymin, float pymax,
			 float xmin, float xmax, float ymin, float ymax, gsglPage *p)
{
	float mat1[16], mat2[16], mat3[16];
	int i;
	gsglMatrices *m = p->matrix;

	gsgl_displaylist_end (p);

	gsgl_inq_matrix_translate (2.0*pxmin/GSGL_PAPER_WIDTH-1.0,
				   2.0*pymin/GSGL_PAPER_HEIGHT-1.0,
				   0.0, mat1);

	gsgl_inq_matrix_scale ((pxmax-pxmin)/(xmax-xmin), (pymax-pymin)/(ymax-ymin), 1.0, mat2);
	gsgl_mat16_mult_mat16 (mat1, mat2, mat3);

	gsgl_inq_matrix_translate (1.0-2.0*xmin/GSGL_PAPER_WIDTH,
				   1.0-2.0*ymin/GSGL_PAPER_HEIGHT,
				   0.0, mat1);
	gsgl_mat16_mult_mat16 (mat3, mat1, mat2);

	for (i=0; i<16; i++) m->Matrix_view[i] = mat2[i];

	gsgl_projmatrix_set (p);
	gsgl_load_projection_matrix (p);
}

void gsaf_setpage (float xoffset, float yoffset, float xscale, float yscale)
{
	gsglPage *p;
	gsglMatrices *m;
	float pxmin,pxmax,pymin,pymax,xmin,xmax,ymin,ymax;

	if (!gsgl_inq_page_is_open ()) return;

	p = gsgl_inq_page ();
	m = p->matrix;

	gsgl_projimg_identity ();
	gsgl_projimg_translate (0.0, 0.5);
	gsgl_projimg_scale (xscale, yscale);
	gsgl_projimg_translate (xoffset-12.8*(1.0-xscale), yoffset-9.5*(1.0-yscale)-0.5*yscale);

	gsaf_inqvew (&pxmin,&pxmax,&pymin,&pymax,&xmin,&xmax,&ymin,&ymax);
	gsaf_setvew (pxmin,pxmax,pymin,pymax,xmin,xmax,ymin,ymax);
	gsaf_offclp ();
	gsaf_setlin (0,2,7);
	gsaf_setchs (0.35,0.0);
	gsaf_setmks (0,0.2);
	gsaf_setfnt (0);
	gsaf_move (0.0,0.0);

	m->Xoffset = xoffset; 
	m->Yoffset = yoffset;
	m->Xscale = xscale;
	m->Yscale = yscale;
}

void gsaf_inqpage (float *xoffset, float *yoffset, float *xscale, float *yscale)
{
	gsglPage *p;
	gsglMatrices *m;

	if (!gsgl_inq_page_is_open ()) return;

	p = gsgl_inq_page ();
	m = p->matrix;

	*xoffset = m->Xoffset; 
	*yoffset = m->Yoffset;
	*xscale = m->Xscale;
	*yscale = m->Yscale;
}
