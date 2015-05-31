#ifndef __drv_wrapper_h__
#define __drv_wrapper_h__

#include "guiitems.h"

struct _gsglDrivers
{
	gsglboolean Drv_gl;
	gsglboolean Drv_mem;
	gsglboolean Drv_hd;
	gsglboolean Drv_tcp;
};

#ifdef __cplusplus
extern "C"{
#endif

void gsgl_drivers_init (gsglPage *p);
void gsgl_enable_drv_gl (gsglPage *);
void gsgl_disable_drv_gl (gsglPage *);
void gsgl_enable_drv_mem (gsglPage *);
void gsgl_disable_drv_mem (gsglPage *);
void gsgl_enable_drv_hd (gsglPage *);
void gsgl_disable_drv_hd (gsglPage *);
void gsgl_enable_drv_tcp (gsglPage *);
void gsgl_disable_drv_tcp (gsglPage *);
gsglboolean gsgl_inq_drv_gl (gsglPage *);
gsglboolean gsgl_inq_drv_mem (gsglPage *);
gsglboolean gsgl_inq_drv_hd (gsglPage *);
gsglboolean gsgl_inq_drv_tcp (gsglPage *);

GLuint gsgl_drv_glGenLists (GLsizei i, gsglPage *);
void gsgl_drv_glNewList (GLuint list, GLenum mode, gsglPage *);
void gsgl_drv_glEndList (gsglPage *);

void gsgl_drv_pageinfo (gsglPage *);
void gsgl_drv_begin_header (gsglPage *);
void gsgl_drv_end_header (gsglPage *);
void gsgl_drv_vertex2 (float x, float y, gsglPage *);
void gsgl_drv_vertex3 (float x, float y, float z, gsglPage *);
void gsgl_drv_beginlines (gsglPage *);
void gsgl_drv_endlines (gsglPage *);
void gsgl_drv_beginpolygon (gsglPage *);
void gsgl_drv_endpolygon (gsglPage *);
void gsgl_drv_beginrgbtriangle (gsglPage *);
void gsgl_drv_endrgbtriangle (gsglPage *);
void gsgl_drv_normal (float x, float y, float z, gsglPage *);
void gsgl_drv_rgb (float r, float g, float b, gsglPage *);
void gsgl_drv_color_index (int i, int bgmode, gsglPage *);
void gsgl_drv_linewidth (float w, gsglPage *);
void gsgl_drv_linewidth_index (int i, gsglPage *);
void gsgl_drv_normalmode (gsglPage *);
void gsgl_drv_2dmode (gsglPage *);
void gsgl_drv_3dmode (gsglPage *);
void gsgl_drv_modelview_matrix (GLfloat matrix[16], gsglPage *);
void gsgl_drv_projection_matrix (GLfloat matrix[16], gsglPage *);
void gsgl_drv_projection_type (int i, gsglPage *);
void gsgl_drv_clip_enable_unmovable (int i /* 0 <= i <= 5 */, float eqn[4], gsglPage *);
void gsgl_drv_clip_enable_3d_fileonly (int i /* 0 <= i <= 5 */, float eqn[4], gsglPage *);
void gsgl_drv_clip_disable (int i, gsglPage *);
void gsgl_drv_depth3d (float depth, gsglPage *);
void gsgl_drv_lighting_enable (gsglPage *);
void gsgl_drv_lighting_disable (gsglPage *);
void gsgl_drv_separate_objects (gsglPage *);
void gsgl_drv_array_element (int i, gsglPage *);
void gsgl_drv_begin_triangles (gsglPage *);
void gsgl_drv_end_triangles (gsglPage *);
void gsgl_drv_begin_triangle_strip (gsglPage *);
void gsgl_drv_end_triangle_strip (gsglPage *);
void gsgl_drv_begin_triangle_fan (gsglPage *);
void gsgl_drv_end_triangle_fan (gsglPage *);
void gsgl_drv_glonly_normal (float x, float y, float z, gsglPage *);
void gsgl_drv_normal (float x, float y, float z, gsglPage *);

void gsgl_drv_glarray_begin_polygon (gsglPage *);
void gsgl_drv_glarray_end_polygon (gsglPage *);
void gsgl_drv_glarray_begin_lines (gsglPage *);
void gsgl_drv_glarray_end_lines (gsglPage *);
void gsgl_drv_glarray_vertex (int datatype, int size, unsigned int totalsize, float *buffer,
			      int nx, int ny, 
			      float *r, float *t,
			      float xmin, float xmax, float ymin, float ymax,
			      int *idata, float *fdata, gsglPage *);
void gsgl_drv_glarray_color (int size, unsigned int totalsize, float *buffer, gsglPage *);
void gsgl_drv_glarray_normal (int datatype, int size, unsigned int totalsize, float *buffer, gsglPage *);
void gsgl_drv_glarray_disable_vertex (gsglPage *);
void gsgl_drv_glarray_disable_color (gsglPage *);
void gsgl_drv_glarray_disable_normal (gsglPage *);

void gsgl_drv_ps_gouraudonly_begin (gsglPage *);
void gsgl_drv_ps_gouraudonly_end (gsglPage *);
void gsgl_drv_ps_depth_by_deep (gsglPage *);
void gsgl_drv_ps_depth_by_shallow (gsglPage *);
void gsgl_drv_ps_depth_by_mean (gsglPage *);
void gsgl_drv_ps_depth_by_vectprod (float *x, float *y, float *z, gsglPage *);
void gsgl_drv_ps_depth_by_vectprod2 (float *x, float *y, float *z, 
				     unsigned int npoints, float *xp, float *yp, float *zp, gsglPage *);
void gsgl_drv_ps_depth_by_point (float x, float y, float z, gsglPage *);
void gsgl_drv_beginanime (gsglPage *p);
void gsgl_drv_endanime (gsglPage *);
void gsgl_drv_pstext (int ifnts, int ifntr,
		      float chh, float chw, float chsp, float angl, float tilt,
		      float x, float y, int dim, unsigned int *iascs, int nmax, gsglboolean tcpip_bitmap,
		      gsglPage *);
void gsgl_drv_pstext_end (gsglPage *);
void gsgl_drv_pstext_position (float *x, float *y, int nmax, gsglPage *);
void gsgl_drv_enable_antialiasing (gsglPage *);
void gsgl_drv_disable_antialiasing (gsglPage *);
void gsgl_drv_gui_label (guialldata *data);
void gsgl_drv_gui_button (guialldata *data);
void gsgl_drv_gui_checkbutton (guialldata *data);
void gsgl_drv_gui_combobox (guialldata *data);
void gsgl_drv_gui_lineedit (guialldata *data);
void gsgl_drv_gui_spinbuttonf (guialldata *data);
void gsgl_drv_gui_spinbuttoni (guialldata *data);
void gsgl_drv_set_external_gui (gsglboolean b, gsglPage *);

#ifdef __cplusplus
}
#endif

#endif
