#include "gsaf.h"
#include "drv_gl.h"
#include "drv_file.h"
#include "displaylist.h"
#include "mem_list.h"
#include "harddisk.h"
#include "tcpip.h"
#include "drv_wrapper.h"
#include "utils.h"
#include "device.h"
#include "toolkit.h"
#include "fileformat.h"

void gsgl_drivers_init (gsglPage *p)
{
	p->driver->Drv_gl = GSGL_FALSE;
	p->driver->Drv_mem = GSGL_FALSE;
	p->driver->Drv_hd = GSGL_FALSE;
	p->driver->Drv_tcp = GSGL_FALSE;
}
void gsgl_enable_drv_gl (gsglPage *p)
{
	p->driver->Drv_gl = GSGL_TRUE;
}

void gsgl_disable_drv_gl (gsglPage *p)
{
	p->driver->Drv_gl = GSGL_FALSE;
}

void gsgl_enable_drv_mem (gsglPage *p)
{
	p->driver->Drv_mem = GSGL_TRUE;
}

void gsgl_disable_drv_mem (gsglPage *p)
{
	p->driver->Drv_mem = GSGL_FALSE;
}

void gsgl_enable_drv_hd (gsglPage *p)
{
	p->driver->Drv_hd = GSGL_TRUE;
}

void gsgl_disable_drv_hd (gsglPage *p)
{
	p->driver->Drv_hd = GSGL_FALSE;
}

void gsgl_enable_drv_tcp (gsglPage *p)
{
	p->driver->Drv_tcp = GSGL_TRUE;
}

void gsgl_disable_drv_tcp (gsglPage *p)
{
	p->driver->Drv_tcp = GSGL_FALSE;
}

gsglboolean gsgl_inq_drv_gl (gsglPage *p)
{
	return p->driver->Drv_gl;
}

gsglboolean gsgl_inq_drv_mem (gsglPage *p)
{
	return p->driver->Drv_mem;
}

gsglboolean gsgl_inq_drv_hd (gsglPage *p)
{
	return p->driver->Drv_hd;
}

gsglboolean gsgl_inq_drv_tcp (gsglPage *p)
{
	return p->driver->Drv_tcp;
}

void gsgl_drv_pageinfo (gsglPage *p)
{
	if (p->driver->Drv_gl) gsgl_gl_pageinfo ();
	if (p->driver->Drv_mem) gsgl_file_pageinfo (gsgl_list_push, p);
	if (p->driver->Drv_hd) gsgl_file_pageinfo (gsgl_hd_push, p);
	if (p->driver->Drv_tcp) gsgl_file_pageinfo (gsgl_tcp_push, p);
}

GLuint gsgl_drv_glGenLists (GLsizei i, gsglPage *p)
{
	GLuint list;

	if (p->driver->Drv_gl) {
		list = glGenLists (i);
		if (!list) gsgl_error ("gsgl_drv_glGenLists", "OpenGL returned invalid gllist");
		return list;
	} else return 0;
}

void gsgl_drv_glNewList (GLuint list, GLenum mode, gsglPage *p)
{
	if (p->driver->Drv_gl) glNewList (list, mode);
}

void gsgl_drv_glEndList (gsglPage *p)
{
	if (p->driver->Drv_gl) glEndList ();
}

void gsgl_drv_begin_header (gsglPage *p)
{
	if (p->driver->Drv_gl) gsgl_gl_begin_header ();
	if (p->driver->Drv_mem) gsgl_file_begin_header (gsgl_list_push, p);
	if (p->driver->Drv_hd) gsgl_file_begin_header (gsgl_hd_push, p);
	if (p->driver->Drv_tcp) gsgl_file_begin_header (gsgl_tcp_push, p);
}

void gsgl_drv_end_header (gsglPage *p)
{
	if (p->driver->Drv_gl) gsgl_gl_end_header ();
	if (p->driver->Drv_mem) gsgl_file_end_header (gsgl_list_push, p);
	if (p->driver->Drv_hd) gsgl_file_end_header (gsgl_hd_push, p);
	if (p->driver->Drv_tcp) gsgl_file_end_header (gsgl_tcp_push, p);
}

void gsgl_drv_vertex2 (float x, float y, gsglPage *p)
{
	if (p->driver->Drv_gl) gsgl_gl_vertex2 (x, y);
	if (p->driver->Drv_mem) gsgl_file_vertex2 (x, y, gsgl_list_push, p);
	if (p->driver->Drv_hd) gsgl_file_vertex2 (x, y, gsgl_hd_push, p);
	if (p->driver->Drv_tcp) gsgl_file_vertex2 (x, y, gsgl_tcp_push, p);
}

void gsgl_drv_vertex3 (float x, float y, float z, gsglPage *p)
{
	if (p->driver->Drv_gl) gsgl_gl_vertex3 (x, y, z);
	if (p->driver->Drv_mem) gsgl_file_vertex3 (x, y, z, gsgl_list_push, p);
	if (p->driver->Drv_hd) gsgl_file_vertex3 (x, y, z, gsgl_hd_push, p);
	if (p->driver->Drv_tcp) gsgl_file_vertex3 (x, y, z, gsgl_tcp_push, p);
}

void gsgl_drv_beginlines (gsglPage *p)
{
	if (p->driver->Drv_gl) gsgl_gl_beginlines ();
	if (p->driver->Drv_mem) gsgl_file_beginlines (gsgl_list_push, p);
	if (p->driver->Drv_hd) gsgl_file_beginlines (gsgl_hd_push, p);
	if (p->driver->Drv_tcp) gsgl_file_beginlines (gsgl_tcp_push, p);
}

void gsgl_drv_endlines (gsglPage *p)
{
	if (p->driver->Drv_gl) gsgl_gl_endlines ();
	if (p->driver->Drv_mem) gsgl_file_endlines (gsgl_list_push, p);
	if (p->driver->Drv_hd) gsgl_file_endlines (gsgl_hd_push, p);
	if (p->driver->Drv_tcp) gsgl_file_endlines (gsgl_tcp_push, p);
}

void gsgl_drv_beginpolygon (gsglPage *p)
{
	if (p->driver->Drv_gl) gsgl_gl_beginpolygon ();
	if (p->driver->Drv_mem) gsgl_file_beginpolygon (gsgl_list_push, p);
	if (p->driver->Drv_hd) gsgl_file_beginpolygon (gsgl_hd_push, p);
	if (p->driver->Drv_tcp) gsgl_file_beginpolygon (gsgl_tcp_push, p);
}

void gsgl_drv_endpolygon (gsglPage *p)
{
	if (p->driver->Drv_gl) gsgl_gl_endpolygon ();
	if (p->driver->Drv_mem) gsgl_file_endpolygon (gsgl_list_push, p);
	if (p->driver->Drv_hd) gsgl_file_endpolygon (gsgl_hd_push, p);	
	if (p->driver->Drv_tcp) gsgl_file_endpolygon (gsgl_tcp_push, p);	
}

void gsgl_drv_beginrgbtriangle (gsglPage *p)
{
	if (p->driver->Drv_gl) gsgl_gl_beginrgbtriangle ();
	if (p->driver->Drv_mem) gsgl_file_beginrgbtriangle (gsgl_list_push, p);
	if (p->driver->Drv_hd) gsgl_file_beginrgbtriangle (gsgl_hd_push, p);
	if (p->driver->Drv_tcp) gsgl_file_beginrgbtriangle (gsgl_tcp_push, p);
}

void gsgl_drv_endrgbtriangle (gsglPage *p)
{
	if (p->driver->Drv_gl) gsgl_gl_endrgbtriangle ();
	if (p->driver->Drv_mem) gsgl_file_endrgbtriangle (gsgl_list_push, p);
	if (p->driver->Drv_hd) gsgl_file_endrgbtriangle (gsgl_hd_push, p);
	if (p->driver->Drv_tcp) gsgl_file_endrgbtriangle (gsgl_tcp_push, p);
}

/* used in only textunicode */
void gsgl_drv_begin_triangles (gsglPage *p)
{
	if (p->driver->Drv_gl) gsgl_gl_begin_triangles ();
	if (p->driver->Drv_mem) gsgl_file_begin_triangles (gsgl_list_push, p);
	if (p->driver->Drv_hd) gsgl_file_begin_triangles (gsgl_hd_push, p);
	if (p->driver->Drv_tcp) gsgl_file_begin_triangles (gsgl_tcp_push, p);
}

/* used in only text.c */
void gsgl_drv_end_triangles (gsglPage *p)
{
	if (p->driver->Drv_gl) gsgl_gl_end_triangles ();
	if (p->driver->Drv_mem) gsgl_file_end_triangles (gsgl_list_push, p);
	if (p->driver->Drv_hd) gsgl_file_end_triangles (gsgl_hd_push, p);
	if (p->driver->Drv_tcp) gsgl_file_end_triangles (gsgl_tcp_push, p);
}

/* used in only text.c */
void gsgl_drv_begin_triangle_strip (gsglPage *p)
{
	if (p->driver->Drv_gl) gsgl_gl_begin_triangle_strip ();
	if (p->driver->Drv_mem) gsgl_file_begin_triangle_strip (gsgl_list_push, p);
	if (p->driver->Drv_hd) gsgl_file_begin_triangle_strip (gsgl_hd_push, p);
	if (p->driver->Drv_tcp) gsgl_file_begin_triangle_strip (gsgl_tcp_push, p);
}

/* used in only text.c */
void gsgl_drv_end_triangle_strip (gsglPage *p)
{
	if (p->driver->Drv_gl) gsgl_gl_end_triangle_strip ();
	if (p->driver->Drv_mem) gsgl_file_end_triangle_strip (gsgl_list_push, p);
	if (p->driver->Drv_hd) gsgl_file_end_triangle_strip (gsgl_hd_push, p);
	if (p->driver->Drv_tcp) gsgl_file_end_triangle_strip (gsgl_tcp_push, p);
}

/* used in only text.c */
void gsgl_drv_begin_triangle_fan (gsglPage *p)
{
	if (p->driver->Drv_gl) gsgl_gl_begin_triangle_fan ();
	if (p->driver->Drv_mem) gsgl_file_begin_triangle_fan (gsgl_list_push, p);
	if (p->driver->Drv_hd) gsgl_file_begin_triangle_fan (gsgl_hd_push, p);
	if (p->driver->Drv_tcp) gsgl_file_begin_triangle_fan (gsgl_tcp_push, p);
}

/* used in only text.c */
void gsgl_drv_end_triangle_fan (gsglPage *p)
{
	if (p->driver->Drv_gl) gsgl_gl_end_triangle_fan ();
	if (p->driver->Drv_mem) gsgl_file_end_triangle_fan (gsgl_list_push, p);
	if (p->driver->Drv_hd) gsgl_file_end_triangle_fan (gsgl_hd_push, p);
	if (p->driver->Drv_tcp) gsgl_file_end_triangle_fan (gsgl_tcp_push, p);
}

void gsgl_drv_glonly_normal (float x, float y, float z, gsglPage *p)
{
	if (p->driver->Drv_gl) gsgl_gl_normal (x, y, z);
}

void gsgl_drv_normal (float x, float y, float z, gsglPage *p)
{
	if (p->driver->Drv_gl) gsgl_gl_normal (x, y, z);
	if (p->driver->Drv_mem) gsgl_file_normal (x, y, z, gsgl_list_push, p);
	if (p->driver->Drv_hd) gsgl_file_normal (x, y, z, gsgl_hd_push, p);
	if (p->driver->Drv_tcp) gsgl_file_normal (x, y, z, gsgl_tcp_push, p);
}

void gsgl_drv_rgb (float r, float g, float b, gsglPage *p)
{
	if (p->driver->Drv_gl) gsgl_gl_rgb (r, g, b);
	if (p->driver->Drv_mem) gsgl_file_rgb (r, g, b, gsgl_list_push, p);
	if (p->driver->Drv_hd) gsgl_file_rgb (r, g, b, gsgl_hd_push, p);
	if (p->driver->Drv_tcp) gsgl_file_rgb (r, g, b, gsgl_tcp_push, p);
}

void gsgl_drv_color_index (int i, int bgmode, gsglPage *p)
{
	if (p->driver->Drv_gl) gsgl_gl_color_index (i, bgmode);
	if (p->driver->Drv_mem) gsgl_file_color_index (i, bgmode, gsgl_list_push, p);
	if (p->driver->Drv_hd) gsgl_file_color_index (i, bgmode, gsgl_hd_push, p);
	if (p->driver->Drv_tcp) gsgl_file_color_index (i, bgmode, gsgl_tcp_push, p);
}

void gsgl_drv_linewidth (float w, gsglPage *p)
{
	if (p->driver->Drv_gl) gsgl_gl_linewidth (w, p);
	if (p->driver->Drv_mem) gsgl_file_linewidth (w, gsgl_list_push, p);
	if (p->driver->Drv_hd) gsgl_file_linewidth (w, gsgl_hd_push, p);
	if (p->driver->Drv_tcp) gsgl_file_linewidth (w, gsgl_tcp_push, p);
}

void gsgl_drv_linewidth_index (int i, gsglPage *p)
{
	if (p->driver->Drv_gl) gsgl_gl_linewidth_index (i);
	if (p->driver->Drv_mem) gsgl_file_linewidth_index (i, gsgl_list_push, p);
	if (p->driver->Drv_hd) gsgl_file_linewidth_index (i, gsgl_hd_push, p);
	if (p->driver->Drv_tcp) gsgl_file_linewidth_index (i, gsgl_tcp_push, p);
}

void gsgl_drv_normalmode (gsglPage *p)
{
	if (p->driver->Drv_gl) gsgl_gl_normalmode ();
	if (p->driver->Drv_mem) gsgl_file_normalmode (gsgl_list_push, p);
	if (p->driver->Drv_hd) gsgl_file_normalmode (gsgl_hd_push, p);	
	if (p->driver->Drv_tcp) gsgl_file_normalmode (gsgl_tcp_push, p);	
}

void gsgl_drv_2dmode (gsglPage *p)
{
	if (p->driver->Drv_gl) gsgl_gl_2dmode ();
	if (p->driver->Drv_mem) gsgl_file_2dmode (gsgl_list_push, p);
	if (p->driver->Drv_hd) gsgl_file_2dmode (gsgl_hd_push, p);
	if (p->driver->Drv_tcp) gsgl_file_2dmode (gsgl_tcp_push, p);
}

void gsgl_drv_3dmode (gsglPage *p)
{
	if (p->driver->Drv_gl) gsgl_gl_3dmode ();
	if (p->driver->Drv_mem) gsgl_file_3dmode (gsgl_list_push, p);
	if (p->driver->Drv_hd) gsgl_file_3dmode (gsgl_hd_push, p);
	if (p->driver->Drv_tcp) gsgl_file_3dmode (gsgl_tcp_push, p);
}

void gsgl_drv_modelview_matrix (float matrix[16], gsglPage *p)
{
	if (p->driver->Drv_gl) gsgl_gl_modelview_matrix (matrix);
	if (p->driver->Drv_mem) gsgl_file_modelview_matrix (matrix, gsgl_list_push, p);
	if (p->driver->Drv_hd) gsgl_file_modelview_matrix (matrix, gsgl_hd_push, p);
	if (p->driver->Drv_tcp) gsgl_file_modelview_matrix (matrix, gsgl_tcp_push, p);
}

void gsgl_drv_projection_matrix (float matrix[16], gsglPage *p)
{
	if (p->driver->Drv_gl) gsgl_gl_projection_matrix (matrix);
	if (p->driver->Drv_mem) gsgl_file_projection_matrix (matrix, gsgl_list_push, p);
	if (p->driver->Drv_hd) gsgl_file_projection_matrix (matrix, gsgl_hd_push, p);
	if (p->driver->Drv_tcp) gsgl_file_projection_matrix (matrix, gsgl_tcp_push, p);
}

void gsgl_drv_projection_type (int i, gsglPage *p)
{
	if (p->driver->Drv_gl) gsgl_gl_projection_type (i);
	if (p->driver->Drv_mem) gsgl_file_projection_type (i, gsgl_list_push, p);
	if (p->driver->Drv_hd) gsgl_file_projection_type (i, gsgl_hd_push, p);
	if (p->driver->Drv_tcp) gsgl_file_projection_type (i, gsgl_tcp_push, p);
}

void gsgl_drv_clip_enable_unmovable (int i /* 0 <= i <= 5 */, float eqn[4], gsglPage *p)
{
	if (p->driver->Drv_gl) gsgl_gl_clip_enable_unmovable (i, eqn);
	if (p->driver->Drv_mem) gsgl_file_clip_enable_unmovable (i, eqn, gsgl_list_push, p);
	if (p->driver->Drv_hd) gsgl_file_clip_enable_unmovable (i, eqn, gsgl_hd_push, p);
	if (p->driver->Drv_tcp) gsgl_file_clip_enable_unmovable (i, eqn, gsgl_tcp_push, p);
}

void gsgl_drv_clip_enable_3d_fileonly (int i /* 0 <= i <= 5 */, float eqn[4], gsglPage *p)
{
	if (p->driver->Drv_gl) gsgl_gl_clip_enable_3d_fileonly (i, eqn);
	if (p->driver->Drv_mem) gsgl_file_clip_enable_3d_fileonly (i, eqn, gsgl_list_push, p);
	if (p->driver->Drv_hd) gsgl_file_clip_enable_3d_fileonly (i, eqn, gsgl_hd_push, p);
	if (p->driver->Drv_tcp) gsgl_file_clip_enable_3d_fileonly (i, eqn, gsgl_tcp_push, p);
}

void gsgl_drv_clip_disable (int i, gsglPage *p)
{
	if (p->driver->Drv_gl) gsgl_gl_clip_disable (i);
	if (p->driver->Drv_mem) gsgl_file_clip_disable (i, gsgl_list_push, p);
	if (p->driver->Drv_hd) gsgl_file_clip_disable (i, gsgl_hd_push, p);
	if (p->driver->Drv_tcp) gsgl_file_clip_disable (i, gsgl_tcp_push, p);
}

void gsgl_drv_depth3d (float depth, gsglPage *p)
{
	if (p->driver->Drv_gl) gsgl_gl_depth3d (depth);
	if (p->driver->Drv_mem) gsgl_file_depth3d (depth, gsgl_list_push, p);
	if (p->driver->Drv_hd) gsgl_file_depth3d (depth, gsgl_hd_push, p);
	if (p->driver->Drv_tcp) gsgl_file_depth3d (depth, gsgl_tcp_push, p);
}

void gsgl_drv_lighting_enable (gsglPage *p)
{
	if (p->driver->Drv_gl) gsgl_gl_lighting_enable ();
	if (p->driver->Drv_mem) gsgl_file_lighting_enable (gsgl_list_push, p);
	if (p->driver->Drv_hd) gsgl_file_lighting_enable (gsgl_hd_push, p);
	if (p->driver->Drv_tcp) gsgl_file_lighting_enable (gsgl_tcp_push, p);
}

void gsgl_drv_lighting_disable (gsglPage *p)
{
	if (p->driver->Drv_gl) gsgl_gl_lighting_disable ();
	if (p->driver->Drv_mem) gsgl_file_lighting_disable (gsgl_list_push, p);
	if (p->driver->Drv_hd) gsgl_file_lighting_disable (gsgl_hd_push, p);
	if (p->driver->Drv_tcp) gsgl_file_lighting_disable (gsgl_tcp_push, p);
}

void gsgl_drv_separate_objects (gsglPage *p)
{
	if (p->driver->Drv_gl) gsgl_gl_separate_objects ();
	if (p->driver->Drv_mem) gsgl_file_separate_objects (gsgl_list_push, p);
	if (p->driver->Drv_hd) gsgl_file_separate_objects (gsgl_hd_push, p);
	if (p->driver->Drv_tcp) gsgl_file_separate_objects (gsgl_tcp_push, p);
}

void gsgl_drv_array_element (int i, gsglPage *p)
{
	if (p->driver->Drv_gl) gsgl_gl_array_element (i);
	if (p->driver->Drv_mem) gsgl_file_array_element (i, gsgl_list_push, p);
	if (p->driver->Drv_hd) gsgl_file_array_element (i, gsgl_hd_push, p);
	if (p->driver->Drv_tcp) gsgl_file_array_element (i, gsgl_tcp_push, p);
}

void gsgl_drv_glarray_begin_polygon (gsglPage *p)
{
	if (p->driver->Drv_gl) gsgl_gl_glarray_begin_polygon ();
	if (p->driver->Drv_mem) gsgl_file_glarray_begin_polygon (gsgl_list_push, p);
	if (p->driver->Drv_hd) gsgl_file_glarray_begin_polygon (gsgl_hd_push, p);
	if (p->driver->Drv_tcp) gsgl_file_glarray_begin_polygon (gsgl_tcp_push, p);
}

void gsgl_drv_glarray_end_polygon (gsglPage *p)
{
	if (p->driver->Drv_gl) gsgl_gl_glarray_end_polygon ();
	if (p->driver->Drv_mem) gsgl_file_glarray_end_polygon (gsgl_list_push, p);
	if (p->driver->Drv_hd) gsgl_file_glarray_end_polygon (gsgl_hd_push, p);
	if (p->driver->Drv_tcp) gsgl_file_glarray_end_polygon (gsgl_tcp_push, p);
}

void gsgl_drv_glarray_begin_lines (gsglPage *p)
{
	if (p->driver->Drv_gl) gsgl_gl_glarray_begin_lines ();
	if (p->driver->Drv_mem) gsgl_file_glarray_begin_lines (gsgl_list_push, p);
	if (p->driver->Drv_hd) gsgl_file_glarray_begin_lines (gsgl_hd_push, p);
	if (p->driver->Drv_tcp) gsgl_file_glarray_begin_lines (gsgl_tcp_push, p);
}

void gsgl_drv_glarray_end_lines (gsglPage *p)
{
	if (p->driver->Drv_gl) gsgl_gl_glarray_end_lines ();
	if (p->driver->Drv_mem) gsgl_file_glarray_end_lines (gsgl_list_push, p);
	if (p->driver->Drv_hd) gsgl_file_glarray_end_lines (gsgl_hd_push, p);
	if (p->driver->Drv_tcp) gsgl_file_glarray_end_lines (gsgl_tcp_push, p);
}

void gsgl_drv_glarray_vertex (int datatype, int size, unsigned int totalsize, float *buffer,
			      int nx, int ny, 
			      float *r, float *t,
			      float xmin, float xmax, float ymin, float ymax,
			      int *idata, float *fdata, gsglPage *p)
{
	if (p->driver->Drv_gl) gsgl_gl_glarray_vertex (datatype, size, totalsize, buffer,
					    nx, ny, 
					    r, t,
					    xmin, xmax, ymin, ymax,
					    idata, fdata);
	if (p->driver->Drv_mem) gsgl_file_glarray_vertex (datatype, size, totalsize, buffer,
					       nx, ny, 
					       r, t,
					       xmin, xmax, ymin, ymax,
					       idata, fdata,
					       gsgl_list_push, p);
	if (p->driver->Drv_hd) gsgl_file_glarray_vertex (datatype, size, totalsize, buffer,
					      nx, ny, 
					      r, t,
					      xmin, xmax, ymin, ymax,
					      idata, fdata,
					      gsgl_hd_push, p);
	if (p->driver->Drv_tcp) gsgl_file_glarray_vertex (datatype, size, totalsize, buffer,
					       nx, ny, 
					       r, t,
					       xmin, xmax, ymin, ymax,
					       idata, fdata,
					       gsgl_tcp_push, p);
}

void gsgl_drv_glarray_color (int size, unsigned int totalsize, float *buffer, gsglPage *p)
{
	if (p->driver->Drv_gl) gsgl_gl_glarray_color (size, totalsize, buffer);
	if (p->driver->Drv_mem) gsgl_file_glarray_color (size, totalsize, buffer, gsgl_list_push, p);
	if (p->driver->Drv_hd) gsgl_file_glarray_color (size, totalsize, buffer, gsgl_hd_push, p);
	if (p->driver->Drv_tcp) gsgl_file_glarray_color (size, totalsize, buffer, gsgl_tcp_push, p);
}

void gsgl_drv_glarray_normal (int datatype, int size, unsigned int totalsize, float *buffer, gsglPage *p)
{
	if (p->driver->Drv_gl) gsgl_gl_glarray_normal (datatype, size, totalsize, buffer);
	if (p->driver->Drv_mem) gsgl_file_glarray_normal (datatype, size, totalsize, buffer, gsgl_list_push, p);
	if (p->driver->Drv_hd) gsgl_file_glarray_normal (datatype, size, totalsize, buffer, gsgl_hd_push, p);	
	if (p->driver->Drv_tcp) gsgl_file_glarray_normal (datatype, size, totalsize, buffer, gsgl_tcp_push, p);	
}

void gsgl_drv_glarray_disable_vertex (gsglPage *p)
{
	if (p->driver->Drv_gl) gsgl_gl_glarray_disable_vertex ();
}

void gsgl_drv_glarray_disable_color (gsglPage *p)
{
	if (p->driver->Drv_gl) gsgl_gl_glarray_disable_color ();
}

void gsgl_drv_glarray_disable_normal (gsglPage *p)
{
	if (p->driver->Drv_gl) gsgl_gl_glarray_disable_normal ();
}

/* postscript */

void gsgl_drv_ps_gouraudonly_begin (gsglPage *p)
{
	if (p->driver->Drv_gl) gsgl_gl_ps_gouraudonly_begin ();
	if (p->driver->Drv_mem) gsgl_file_ps_gouraudonly_begin (gsgl_list_push, p);
	if (p->driver->Drv_hd) gsgl_file_ps_gouraudonly_begin (gsgl_hd_push, p);
	if (p->driver->Drv_tcp) gsgl_file_ps_gouraudonly_begin (gsgl_tcp_push, p);
}

void gsgl_drv_ps_gouraudonly_end (gsglPage *p)
{
	if (p->driver->Drv_gl) gsgl_gl_ps_gouraudonly_end ();
	if (p->driver->Drv_mem) gsgl_file_ps_gouraudonly_end (gsgl_list_push, p);
	if (p->driver->Drv_hd) gsgl_file_ps_gouraudonly_end (gsgl_hd_push, p);
	if (p->driver->Drv_tcp) gsgl_file_ps_gouraudonly_end (gsgl_tcp_push, p);
}

void gsgl_drv_ps_depth_by_deep (gsglPage *p)
{
	if (p->driver->Drv_gl) gsgl_gl_ps_depth_by_deep ();
	if (p->driver->Drv_mem) gsgl_file_ps_depth_by_deep (gsgl_list_push, p);
	if (p->driver->Drv_hd) gsgl_file_ps_depth_by_deep (gsgl_hd_push, p);
	if (p->driver->Drv_tcp) gsgl_file_ps_depth_by_deep (gsgl_tcp_push, p);
}

void gsgl_drv_ps_depth_by_shallow (gsglPage *p)
{
	if (p->driver->Drv_gl) gsgl_gl_ps_depth_by_shallow ();
	if (p->driver->Drv_mem) gsgl_file_ps_depth_by_shallow (gsgl_list_push, p);
	if (p->driver->Drv_hd) gsgl_file_ps_depth_by_shallow (gsgl_hd_push, p);
	if (p->driver->Drv_tcp) gsgl_file_ps_depth_by_shallow (gsgl_tcp_push, p);
}

void gsgl_drv_ps_depth_by_mean (gsglPage *p)
{
	if (p->driver->Drv_gl) gsgl_gl_ps_depth_by_mean ();
	if (p->driver->Drv_mem) gsgl_file_ps_depth_by_mean (gsgl_list_push, p);
	if (p->driver->Drv_hd) gsgl_file_ps_depth_by_mean (gsgl_hd_push, p);
	if (p->driver->Drv_tcp) gsgl_file_ps_depth_by_mean (gsgl_tcp_push, p);
}

void gsgl_drv_ps_depth_by_vectprod (float *x, float *y, float *z, gsglPage *p)
{
	if (p->driver->Drv_gl) gsgl_gl_ps_depth_by_vectprod (x, y, z);
	if (p->driver->Drv_mem) gsgl_file_ps_depth_by_vectprod (x, y, z, gsgl_list_push, p);
	if (p->driver->Drv_hd) gsgl_file_ps_depth_by_vectprod (x, y, z, gsgl_hd_push, p);
	if (p->driver->Drv_tcp) gsgl_file_ps_depth_by_vectprod (x, y, z, gsgl_tcp_push, p);
}

void gsgl_drv_ps_depth_by_vectprod2 (float *x, float *y, float *z,
				     unsigned int npoints, float *xp, float *yp, float *zp,
				     gsglPage *p)
{
	if (p->driver->Drv_gl) gsgl_gl_ps_depth_by_vectprod2 (x, y, z, npoints, xp, yp, zp);
	if (p->driver->Drv_mem) gsgl_file_ps_depth_by_vectprod2 (x, y, z, npoints, xp, yp, zp, gsgl_list_push, p);
	if (p->driver->Drv_hd) gsgl_file_ps_depth_by_vectprod2 (x, y, z, npoints, xp, yp, zp, gsgl_hd_push, p);
	if (p->driver->Drv_tcp) gsgl_file_ps_depth_by_vectprod2 (x, y, z, npoints, xp, yp, zp, gsgl_tcp_push, p);
}

void gsgl_drv_ps_depth_by_point (float x, float y, float z, gsglPage *p)
{
	if (p->driver->Drv_gl) gsgl_gl_ps_depth_by_point (x, y, z);
	if (p->driver->Drv_mem) gsgl_file_ps_depth_by_point (x, y, z, gsgl_list_push, p);
	if (p->driver->Drv_hd) gsgl_file_ps_depth_by_point (x, y, z, gsgl_hd_push, p);
	if (p->driver->Drv_tcp) gsgl_file_ps_depth_by_point (x, y, z, gsgl_tcp_push, p);
}

void gsgl_drv_beginanime (gsglPage *p)
{
	if (p->driver->Drv_gl) gsgl_gl_beginanime (p);
	if (p->driver->Drv_mem) gsgl_file_beginanime (gsgl_list_push, p);
	if (p->driver->Drv_hd) gsgl_file_beginanime (gsgl_hd_push, p);
	if (p->driver->Drv_tcp) gsgl_file_beginanime (gsgl_tcp_push, p);
}

void gsgl_drv_endanime (gsglPage *p)
{
	if (p->driver->Drv_gl) gsgl_gl_endanime (p);
	if (p->driver->Drv_mem) gsgl_file_endanime (gsgl_list_push, p);
	if (p->driver->Drv_hd) gsgl_file_endanime (gsgl_hd_push, p);
	if (p->driver->Drv_tcp) gsgl_file_endanime (gsgl_tcp_push, p);
}

void gsgl_drv_pstext (int ifnts, int ifntr,
		      float chh, float chw, float chsp, float angl, float tilt,
		      float x, float y, int dim, unsigned int *iascs, int nmax, 
		      gsglboolean tcpip_bitmap, gsglPage *p)
{
	int plus;
	if (tcpip_bitmap)
		plus = GSGL_TCP_BITMAP_FONT;
	else
		plus = 0;
	
	if (p->driver->Drv_gl)
		gsgl_gl_pstext (ifnts,ifntr,chh,chw,chsp,angl,tilt,x,y,dim,iascs,nmax);
	if (p->driver->Drv_mem)
		gsgl_file_pstext (ifnts,ifntr,chh,chw,chsp,angl,tilt,x,y,dim,iascs,nmax,gsgl_list_push, p);
	if (p->driver->Drv_hd)
		gsgl_file_pstext (ifnts,ifntr,chh,chw,chsp,angl,tilt,x,y,dim,iascs,nmax,gsgl_hd_push, p);
	if (p->driver->Drv_tcp)
		gsgl_file_pstext (ifnts,ifntr+plus,chh,chw,chsp,angl,tilt,x,y,dim,iascs,nmax,gsgl_tcp_push, p);
}

void gsgl_drv_pstext_end (gsglPage *p)
{
	if (p->driver->Drv_tcp) gsgl_file_pstext_end (gsgl_tcp_push, p);
}

void gsgl_drv_pstext_position (float *x, float *y, int nmax, gsglPage *p)
{
	if (p->driver->Drv_tcp) gsgl_file_pstext_position (x, y, nmax, gsgl_tcp_push, p);
}

void gsgl_drv_enable_antialiasing (gsglPage *p)
{
	if (p->driver->Drv_gl) gsgl_gl_enable_antialiasing (p);
}

void gsgl_drv_disable_antialiasing (gsglPage *p)
{
	if (p->driver->Drv_gl) gsgl_gl_disable_antialiasing (p);
}

void gsgl_drv_gui_label (guialldata * data)
{
	if (data->common->p->driver->Drv_gl) gsgl_tk_add_label (data);
	if (data->common->p->driver->Drv_mem) gsgl_file_gui_label (data, gsgl_list_push);
	if (data->common->p->driver->Drv_hd) gsgl_file_gui_label (data, gsgl_hd_push);
	if (data->common->p->driver->Drv_tcp) gsgl_file_gui_label (data, gsgl_tcp_push);
}

void gsgl_drv_gui_button (guialldata * data)
{
	if (data->common->p->driver->Drv_gl) gsgl_tk_add_button (data);
	if (data->common->p->driver->Drv_mem) gsgl_file_gui_button (data, gsgl_list_push);
	if (data->common->p->driver->Drv_hd) gsgl_file_gui_button (data, gsgl_hd_push);
	if (data->common->p->driver->Drv_tcp) gsgl_file_gui_button (data, gsgl_tcp_push);
}

void gsgl_drv_gui_checkbutton (guialldata * data)
{
	if (data->common->p->driver->Drv_gl) gsgl_tk_add_checkbutton (data);
	if (data->common->p->driver->Drv_mem) gsgl_file_gui_checkbutton (data, gsgl_list_push);
	if (data->common->p->driver->Drv_hd) gsgl_file_gui_checkbutton (data, gsgl_hd_push);
	if (data->common->p->driver->Drv_tcp) gsgl_file_gui_checkbutton (data, gsgl_tcp_push);
}

void gsgl_drv_gui_combobox (guialldata * data)
{
	if (data->common->p->driver->Drv_gl) gsgl_tk_add_combobox (data);
	if (data->common->p->driver->Drv_mem) gsgl_file_gui_combobox (data, gsgl_list_push);
	if (data->common->p->driver->Drv_hd) gsgl_file_gui_combobox (data, gsgl_hd_push);
	if (data->common->p->driver->Drv_tcp) gsgl_file_gui_combobox (data, gsgl_tcp_push);
}

void gsgl_drv_gui_lineedit (guialldata * data)
{
	if (data->common->p->driver->Drv_gl) gsgl_tk_add_lineedit (data);
	if (data->common->p->driver->Drv_mem) gsgl_file_gui_lineedit (data, gsgl_list_push);
	if (data->common->p->driver->Drv_hd) gsgl_file_gui_lineedit (data, gsgl_hd_push);
	if (data->common->p->driver->Drv_tcp) gsgl_file_gui_lineedit (data, gsgl_tcp_push);
}

void gsgl_drv_gui_spinbuttonf (guialldata * data)
{
	if (data->common->p->driver->Drv_gl) gsgl_tk_add_spinbuttonf (data);
	if (data->common->p->driver->Drv_mem) gsgl_file_gui_spinbuttonf (data, gsgl_list_push);
	if (data->common->p->driver->Drv_hd) gsgl_file_gui_spinbuttonf (data, gsgl_hd_push);
	if (data->common->p->driver->Drv_tcp) gsgl_file_gui_spinbuttonf (data, gsgl_tcp_push);	
}

void gsgl_drv_gui_spinbuttoni (guialldata * data)
{
	if (data->common->p->driver->Drv_gl) gsgl_tk_add_spinbuttoni (data);
	if (data->common->p->driver->Drv_mem) gsgl_file_gui_spinbuttoni (data, gsgl_list_push);
	if (data->common->p->driver->Drv_hd) gsgl_file_gui_spinbuttoni (data, gsgl_hd_push);
	if (data->common->p->driver->Drv_tcp) gsgl_file_gui_spinbuttoni (data, gsgl_tcp_push);	
}

void gsgl_drv_set_external_gui (gsglboolean b, gsglPage *p)
{
	if (p->driver->Drv_mem) gsgl_file_gui_set_external (b, gsgl_list_push, p);
	if (p->driver->Drv_hd) gsgl_file_gui_set_external (b, gsgl_hd_push, p);
	if (p->driver->Drv_tcp) gsgl_file_gui_set_external (b, gsgl_tcp_push, p);	
}
