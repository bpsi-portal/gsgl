#ifndef __drv_gl_h__
#define __drv_gl_h__

void gsgl_gl_pageinfo (void);
void gsgl_gl_begin_header (void);
void gsgl_gl_end_header (void);
void gsgl_gl_vertex2 (float x, float y);
void gsgl_gl_vertex3 (float x, float y, float z);
void gsgl_gl_beginlines (void);
void gsgl_gl_endlines (void);
void gsgl_gl_beginpolygon (void);
void gsgl_gl_endpolygon (void);
void gsgl_gl_beginrgbtriangle (void);
void gsgl_gl_endrgbtriangle (void);
void gsgl_gl_rgb (float r, float g, float b);
void gsgl_gl_color_index (int i, int bgmode);
void gsgl_gl_linewidth (float w, gsglPage *p);
void gsgl_gl_linewidth_index (int i);
void gsgl_gl_normalmode (void);
void gsgl_gl_2dmode (void);
void gsgl_gl_3dmode (void);
void gsgl_gl_modelview_matrix (float matrix[16]);
void gsgl_gl_projection_matrix (float matrix[16]);
void gsgl_gl_projection_type (int i);
void gsgl_gl_clip_enable_unmovable (int i, float eqn[4]);
void gsgl_gl_clip_enable_3d_fileonly (int i, float eqn[4]);
void gsgl_gl_clip_disable (int i);
void gsgl_gl_depth3d (float depth);
void gsgl_gl_lighting_enable (void);
void gsgl_gl_lighting_disable (void);
void gsgl_gl_separate_objects (void);
void gsgl_gl_array_element (int i);
void gsgl_gl_glarray_begin_polygon (void);
void gsgl_gl_glarray_end_polygon (void);
void gsgl_gl_glarray_begin_lines (void);
void gsgl_gl_glarray_end_lines (void);
void gsgl_gl_begin_triangles (void);
void gsgl_gl_end_triangles (void);
void gsgl_gl_begin_triangle_strip (void);
void gsgl_gl_end_triangle_strip (void);
void gsgl_gl_begin_triangle_fan (void);
void gsgl_gl_end_triangle_fan (void);

void gsgl_gl_glarray_vertex (int datatype, int size, unsigned int totalsize, 
			     float *buffer,
			     int nx, int ny, 
			     float *r, float *t,
			     float xmin, float xmax, float ymin, float ymax,
			     int *idata, float *fdata);
void gsgl_gl_glarray_color (int size, unsigned int totalsize, float *buffer);
void gsgl_gl_glarray_normal (int datatype, int size, unsigned int totalsize, float *buffer);
void gsgl_gl_glarray_disable_vertex (void);
void gsgl_gl_glarray_disable_color (void);
void gsgl_gl_glarray_disable_normal (void);

void gsgl_gl_normal (float x, float y, float z);
void gsgl_gl_ps_gouraudonly_begin (void);
void gsgl_gl_ps_gouraudonly_end (void);
void gsgl_gl_ps_depth_by_deep (void);
void gsgl_gl_ps_depth_by_shallow (void);
void gsgl_gl_ps_depth_by_mean (void);
void gsgl_gl_ps_depth_by_vectprod (float *x, float *y, float *z);
void gsgl_gl_ps_depth_by_vectprod2 (float *x, float *y, float *z,
				    unsigned int npoints, float *xp, float *yp, float *zp);
void gsgl_gl_ps_depth_by_point (float x, float y, float z);
void gsgl_gl_beginanime (gsglPage *);
void gsgl_gl_endanime (gsglPage *);
void gsgl_gl_pstext (int ifnts, int ifntr,
		     float chh, float chw, float chsp, float angl, float tilt,
		     float x, float y, int dim, unsigned int *iascs, int nmax);
void gsgl_gl_enable_antialiasing (gsglPage *);
void gsgl_gl_disable_antialiasing (gsglPage *);

#endif
