#ifndef __matrix_h__
#define __matrix_h__

struct _gsglMatrices
{
	GLfloat Matrix_2d [16];   /* gdefin で指定される matrix */
	GLfloat Matrix_3d [16];   /* gdefin3d,set3dcoord_* で指定される matrix */
	GLfloat Matrix_orth [16]; /* */
	GLfloat Matrix_frus [16]; /* */
	GLfloat Matrix_imag [16]; /* gsglProjImage* で指定される matrix */
	GLfloat Matrix_view [16]; /* setvew で指定される matrix */
	GLfloat Matrix_proj [16]; /* matrix_orth or frus,matrix_view,matrix_image の積 */
	GLfloat Matrix_text2d [16];
	GLfloat Matrix_text3d [16];
	float ProjectionScale;
	float Xoffset; 
	float Yoffset;
	float Xscale;
	float Yscale;
	int CoordinateMode;
};

void gsgl_unproject_world (GLfloat xwin,  GLfloat ywin,  GLfloat zwin,
			   GLfloat *xworld, GLfloat *yworld, GLfloat *zworld,
			   GLdouble proj[], GLint viewport[]);
void gsgl_unproject_world_inv (GLfloat xwin,  GLfloat ywin,  GLfloat zwin,
			       GLfloat *xworld, GLfloat *yworld, GLfloat *zworld,
			       GLdouble proj_inv[], GLdouble viewport_inv[]);
/*void gsgl_projmatrix_frustum (GLfloat eyepos);
  void gsgl_projmatrix_ortho (GLfloat eyepos);*/
void gsgl_projmatrix_set_frustum (float eyepos, gsglPage *);
void gsgl_projmatrix_set_ortho (float eyepos, gsglPage *);
void gsgl_projmatrix_set (gsglPage *);
void gsgl_projmatrix_clean (gsglPage *);
void gsgl_coordinate_normal (void);
void gsgl_coordinate_set (int i, gsglPage *);
void gsgl_coordinate_inq (int *i, gsglPage *);
double gsgl_matrix_inverse (int n, double *a);
void gsgl_init_matrix_cache (gsglPage *);
void gsgl_matrix_inq_2dmatrix (float matrix[], gsglPage *);
void gsgl_matrix_inq_3dmatrix (float matrix[], gsglPage *);
void gsgl_matrix_inq_text2dmatrix (float matrix[], gsglPage *);
void gsgl_matrix_inq_text3dmatrix (float matrix[], gsglPage *);
void gsgl_matrix_setvew (float pxmin, float pxmax, float pymin, float pymax,
			 float xmin, float xmax, float ymin, float ymax, gsglPage *);
void gsgl_projimg_translate (float dx, float dy);
void gsgl_projimg_scale (float xratio, float yratio);
void gsgl_projimg_rotate (float degree);
void gsgl_projimg_identity (void);
void gsgl_set_2d_coord (float pxmax, float pxmin, float pymax, float pymin,
			float gxmax, float gxmin, float gymax, float gymin, gsglPage *);
void gsgl_set_3d_coord (float wx, float wy, float depth,
			float xl, float yl, float zl, 
			float gxmax, float gxmin, 
			float gymax, float gymin, 
			float gzmax, float gzmin,
			float ox, float oy, float oz,
			float angle1, float vn1[], float angle2, float vn2[], gsglPage *);
void gsgl_set_text2d_matrix (float xworld, float yworld, gsglPage *);
void gsgl_set_text3d_matrix (float xworld, float yworld, float zworld,
			     float angle1, float vn1[], float angle2, float vn2[],
			     float angle3, float vn3[], float angle4, float vn4[], gsglPage *);
void gsgl_set_text3d_matrix_normal (float xworld, float yworld, float zworld, gsglPage *);
void gsgl_load_projection_matrix (gsglPage *);
void gsgl_set_text2d_matrix_directly (float mat[], gsglPage *);
void gsgl_set_text3d_matrix_directly (float mat[], gsglPage *);
void gsglprojecttext3d (float x, float y, float *xw, float *yw, float *zw, gsglPage *);
void gsglunproject3d (float xw,  float yw,  float zw, float *x3d, float *y3d, float *z3d, gsglPage *);
void gsglproject3d (float x, float y, float z, float *x_w, float *y_w, float *z_w, gsglPage *);
void gsglprojecttext2d (float x, float y, float *x2, float *y2, gsglPage *);
void gsglunproject2d (float xw, float yw, float *x2d, float *y2d, gsglPage *);
void gsglproject2d (float x, float y, float *x_w, float *y_w, gsglPage *);
gsglboolean gsgl_inq_view_matrix_is_unity (gsglPage *);
void gsgl_inq_projection_scale (float *scale, gsglPage *);
void gsgl_clean_view_matrix (gsglPage *);
void gsgl_project_world (float xworld,  float yworld, float *xwin, float *ywin, gsglWindow *);

#endif
