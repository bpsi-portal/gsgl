#ifndef __base_h__
#define __base_h__

#include "guiitems.h"

#ifdef __cplusplus
extern "C"{
#endif

void gsgl_rgb (float r, float g, float b, gsglPage *);
void gsgl_linewidth (float w, gsglPage *);
void gsgl_linewidth_index (int iw, gsglPage *);
void gsgl_color_index (int icolor, gsglPage *);
void gsgl_projection_matrix (float matrix[], gsglPage *);
void gsgl_lighting_enable (gsglPage *);
void gsgl_lighting_disable (void);
void gsgl_modelview_matrix (float matrix[], gsglPage *);
void gsgl_normalmode (gsglPage *);
void gsgl_2dmode (gsglPage *);
void gsgl_3dmode (gsglPage *);
void gsgl_text2dmode (gsglPage *);
void gsgl_text3dmode (gsglPage *);
void gsgl_depth3d (float depth, gsglPage *);
void gsgl_separate_objects (gsglPage *);
void gsgl_move (float x, float y, gsglPage *);
void gsgl_inqpos (float *x, float *y, gsglPage *);
void gsgl_draw (float x, float y, gsglPage *);
void gsgl_draw_text2d (float x, float y, gsglPage *);
void gsgl_draw_text3d (float x, float y, gsglPage *);
void gsgl_lines (float *x, float *y, int4 n, gsglPage *);
void gsgl_polygon (float *x, float *y, int4 n, gsglPage *);
void gsgl_rgbtriangle (float *x, float *y, float *r, float *g, float *b, gsglPage *);
void gsgl_move2d (float x, float y, gsglPage *);
void gsgl_draw2d (float x, float y, gsglPage *);
void gsgl_inqpos2d (float *x, float *y, gsglPage *);
void gsgl_lines2d (float *x, float *y, int4 n, gsglPage *);
void gsgl_polygon2d (float *x, float *y, int4 n, gsglPage *);
void gsgl_rgbtriangle2d (float *x, float *y, float *r, float *g, float *b, gsglPage *);
void gsgl_move3d (float x, float y, float z, gsglPage *);
void gsgl_inqpos3d (float *x, float *y, float *z, gsglPage *);
void gsgl_draw3d (float x, float y, float z, gsglPage *);
void gsgl_draw3d_with_normal (float x, float y, float z, float nx, float ny, float nz, gsglPage *);
void gsgl_draw3d_with_normals (float x, float y, float z, float nx[], float ny[], float nz[], gsglPage *);
void gsgl_lines3d (float *x, float *y, float *z, int4 n, gsglPage *);
void gsgl_lines3d_with_normal (float *x, float *y, float *z, float nx, float ny, float nz, int4 n, gsglPage *);
void gsgl_lines3d_with_normals (float *x, float *y, float *z, float *nx, float *ny, float *nz, int4 n, gsglPage *);
void gsgl_polygon3d (float *x, float *y, float *z, int4 n, gsglPage *);
void gsgl_polygon3d_with_normal (float *x, float *y, float *z, float nx, float ny, float nz, int4 n, gsglPage *);
void gsgl_polygon3d_with_normals (float *x, float *y, float *z, float *nx, float *ny, float *nz, int4 n, gsglPage *);
void gsgl_rgbtriangle3d (float *x, float *y, float *z, float *r, float *g, float *b, gsglPage *);
void gsgl_rgbtriangle3d_with_normal (float *x, float *y, float *z, float *r, float *g, float *b, float nx, float ny, float nz, gsglPage *);
void gsgl_rgbtriangle3d_with_normals (float *x,float *y, float *z, float *r, float *g, float *b, float *nx, float *ny, float *nz, gsglPage *);
void gsgl_begin_header (gsglPage *);
void gsgl_end_header (gsglPage *);
void gsgl_glarray_element (int i, gsglPage *);
void gsgl_glarray3d_begin_polygon (gsglPage *);
void gsgl_glarray3d_end_polygon (gsglPage *);
void gsgl_glarray3d_begin_lines (gsglPage *);
void gsgl_glarray3d_end_lines (gsglPage *);
void gsgl_glarray3d_vertex (int datatype, int size, unsigned int totalsize, float *buffer,
			    int nx, int ny, 
			    float *r, float *t,
			    float xmin, float xmax, float ymin, float ymax,
			    int *idata, float *fdata, gsglPage *);
void gsgl_glarray3d_color (int size, unsigned int totalsize, float *buffer, gsglPage *);
void gsgl_glarray3d_normal (int datatype, int size, unsigned int totalsize, float *buffer, gsglPage *);
void gsgl_clip_enable (int ic, float eqn1[]);
void gsgl_clip_enable_normal (int ic, float eqn1[], gsglPage *);
void gsgl_clip_enable_2d (int ic, float eqn1[]);
void gsgl_clip_enable_3d (int ic, float eqn1[]);
void gsgl_clip_enable_3d_fileonly (int ic, float eqn1[], gsglPage *);
void gsgl_clip_disable (int ic, gsglPage *);
void gsgl_clip_delete (gsglPage *);
void gsgl_projectiontype (int projection, gsglPage *);
void gsgl_eyeposition (float eye, gsglPage *);
void gsgl_ps_gouraudonly_begin (gsglPage *);
void gsgl_ps_gouraudonly_end (gsglPage *);
void gsgl_ps_depth_by_deep (gsglPage *);
void gsgl_ps_depth_by_shallow (gsglPage *);
void gsgl_ps_depth_by_mean (gsglPage *);
void gsgl_ps_depth_by_vectprod (float *x, float *y, float *z, gsglPage *p);
void gsgl_ps_depth_by_vectprod2 (float *x, float *y, float *z,
				 unsigned int npoints, float *xp, float *yp, float *zp, 
				 gsglPage *p);
void gsgl_ps_depth_by_point (float x, float y, float z, gsglPage *p);
void gsgl_pstext (int ifnts, int ifntr, float chh,float chw,float chsp,float angl,float tilt,
		  float x, float y, int dim, unsigned int *iascs, int nmax, gsglboolean gl,
		  gsglPage *);

void gsgl_init_info_every_page (gsglPage *);
float gsgl_inq_depth3d (gsglPage *);
int gsgl_inq_projectiontype (gsglPage *);
gsglboolean gsgl_inq_3dobjects_exist (gsglPage *);
void gsgl_set_lighting_enable (gsglPage *);
void gsgl_set_lighting_disable (gsglPage *);
gsglboolean gsgl_inq_lighting_is_enabled (gsglPage *);
void gsgl_inq_eyepoint (float *x, float *y, float *z, gsglPage *);
void gsgl_lighting_enable_by_user (gsglPage *);
void gsgl_lighting_enable_temporary (void);
void gsgl_lighting_disable_temporary (void);

gsglboolean gsgl_inq_lighting_is_enabled_temporary (void);
gsglstatus gsgl_inq_all_status (gsglPage *);

void gsgl_gui_label (itemdata *, char *text, int ijust);
void gsgl_gui_button (itemdata *, char *text, void (*callback)(void));
void gsgl_gui_checkbutton (itemdata *, char *text, int init, void (*callback)(int *));
void gsgl_gui_combobox (itemdata *, int items, char *text[], int current, void (*callback)(int *), int fortran);
void gsgl_gui_lineedit (itemdata *, int max, char *text, void (*callback)(const char *, int *));
void gsgl_gui_spinbuttonf (itemdata *, float init, float lower, float upper, int digits, void (*callback)(float *));
void gsgl_gui_spinbuttoni (itemdata *, int init, int lower, int upper, void (*callback)(int *));

void gsaf_guilabel (float x, float y, float w, float h, char *text, int ijust);
void gsaf_guibutton (float x, float y, float w, float h, char *text, void (*callback)(void));
void gsaf_guicheckbutton (float x, float y, float w, float h, char *text, int init, void (*callback)(int *));
void gsaf_guicombobox (float x, float y, float w, float h, int items, char *text[], int current, void (*callback)(int *), int fortran);
void gsaf_guilineedit (float x, float y, float w, float h, int max, char *text, void (*callback)(const char *, int *));
void gsaf_guispinbuttonf (float x, float y, float w, float h, float init, float lower, float upper, int digits, void (*callback)(float *));
void gsaf_guispinbuttoni (float x, float y, float w, float h, int init, int lower, int upper, void (*callback)(int *));
void gsaf_guiexternal (gsglboolean b);

#ifdef __cplusplus
}
#endif

#endif
