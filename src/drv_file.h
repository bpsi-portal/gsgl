#ifndef __drv_file_h__
#define __drv_file_h__

#include "guiitems.h"

#ifdef __cplusplus
extern "C"{
#endif

void gsgl_file_push_float (float x, void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *);
void gsgl_file_pageinfo (void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *);
void gsgl_file_begin_header (void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *);
void gsgl_file_end_header (void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *);
void gsgl_file_vertex2 (float x, float y, void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *);
void gsgl_file_vertex3 (float x, float y, float z, void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *);
void gsgl_file_beginlines (void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *);
void gsgl_file_endlines (void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *);
void gsgl_file_beginpolygon (void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *);
void gsgl_file_endpolygon (void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *);
void gsgl_file_beginrgbtriangle (void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *);
void gsgl_file_endrgbtriangle (void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *);
void gsgl_file_begin_triangles (void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *);
void gsgl_file_end_triangles (void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *);
void gsgl_file_begin_triangle_strip (void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *);
void gsgl_file_end_triangle_strip (void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *);
void gsgl_file_begin_triangle_fan (void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *);
void gsgl_file_end_triangle_fan (void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *);
void gsgl_file_normal (float x, float y, float z, void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *);
void gsgl_file_rgb (float r, float g, float b, void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *);
void gsgl_file_color_index (int i, int bgmode, void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *);
void gsgl_file_linewidth (float w, void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *);
void gsgl_file_linewidth_index (int i, void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *);
void gsgl_file_normalmode (void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *);
void gsgl_file_2dmode (void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *);
void gsgl_file_3dmode (void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *);
void gsgl_file_modelview_matrix (GLfloat matrix[16], void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *);
void gsgl_file_projection_matrix (GLfloat matrix[16], void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *);
void gsgl_file_projection_type (int i, void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *);
void gsgl_file_clip_enable_unmovable (int i, float eqn[4], void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *);
void gsgl_file_clip_enable_3d_fileonly (int i, float eqn[4], void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *);
void gsgl_file_clip_disable (int i, void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *);
void gsgl_file_depth3d (float depth, void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *);
void gsgl_file_lighting_enable (void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *);
void gsgl_file_lighting_disable (void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *);
void gsgl_file_separate_objects (void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *);
void gsgl_file_array_element (int i, void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *);
void gsgl_file_glarray_begin_polygon (void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *);
void gsgl_file_glarray_end_polygon (void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *);
void gsgl_file_glarray_begin_lines (void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *);
void gsgl_file_glarray_end_lines (void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *);
void gsgl_file_glarray_vertex (int datatype, int size, unsigned int totalsize, float *buffer,
			       int nx, int ny, 
			       float *r, float *t,
			       float xmin, float xmax, float ymin, float ymax,
			       int *idata, float *fdata,
			       void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *);
void gsgl_file_glarray_color (int size, unsigned int totalsize, float *buffer, void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *);
void gsgl_file_glarray_normal (int datatype, int size, unsigned int totalsize, float *buffer, void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *);
void gsgl_file_ps_gouraudonly_begin (void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *);
void gsgl_file_ps_gouraudonly_end (void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *);
void gsgl_file_ps_depth_by_deep (void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *);
void gsgl_file_ps_depth_by_shallow (void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *);
void gsgl_file_ps_depth_by_mean (void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *);
void gsgl_file_ps_depth_by_vectprod (float *x, float *y, float *z, void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *);
void gsgl_file_ps_depth_by_vectprod2 (float *x, float *y, float *z, unsigned int npoints, float *xp, float *yp, float *zp, void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *);
void gsgl_file_ps_depth_by_point (float x, float y, float z, void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *);
void gsgl_file_beginanime (void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *);
void gsgl_file_endanime (void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *);
void gsgl_file_pstext (int ifnts, int ifntr,
		       float chh, float chw, float chsp, float angl, float tilt,
		       float x2, float y2, int dim, unsigned int *iascs, int nmax,
		       void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *);
void gsgl_file_pstext_end (void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *);
void gsgl_file_pstext_position (float *x, float *y, int nmax, void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *);
void gsgl_file_gui_label (guialldata *data, void (* gsgl_push)(unsigned int, gsglPage *));
void gsgl_file_gui_button (guialldata *data, void (* gsgl_push)(unsigned int, gsglPage *));
void gsgl_file_gui_checkbutton (guialldata *data, void (* gsgl_push)(unsigned int, gsglPage *));
void gsgl_file_gui_combobox (guialldata *data, void (* gsgl_push)(unsigned int, gsglPage *));
void gsgl_file_gui_lineedit (guialldata *data, void (* gsgl_push)(unsigned int, gsglPage *));
void gsgl_file_gui_spinbuttonf (guialldata *data, void (* gsgl_push)(unsigned int, gsglPage *));
void gsgl_file_gui_spinbuttoni (guialldata *data, void (* gsgl_push)(unsigned int, gsglPage *));
void gsgl_file_gui_set_external (gsglboolean b, void (* gsgl_push)(unsigned int, gsglPage *), gsglPage *);

#ifdef __cplusplus
}
#endif

#endif
