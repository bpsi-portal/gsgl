#include "gsaf.h"
#include "gsgl.h"
#include "fileformat.h"
#include "utils.h"
#include "clientstate.h"
#include "base.h"
#include "drv_wrapper.h"
#include "anime.h"
#include "displaylist.h"
#include "matrix.h"
#include "text.h"
#include "bsc1.h"
#include "bsc2.h"

#define GSGL_DEBUG(x)

gsglPage * gsgl_file_format_read_template (
	gsglPage *p,
	void *user_data,
	READ_TOKEN,
	unsigned int (* func_inq_token)(void *, gsglPage *p, READ_TOKEN),
	void (*func_vertex2)(void *, gsglPage *p, READ_TOKEN),
	void (*func_vertex3)(void *, gsglPage *p, READ_TOKEN),
	void (*func_mode_normal)(void *, gsglPage *p, READ_TOKEN),
	void (*func_mode_2d)(void *, gsglPage *p, READ_TOKEN),
	void (*func_mode_3d)(void *, gsglPage *p, READ_TOKEN),
	void (*func_separate_objects)(void *, gsglPage *p, READ_TOKEN),
	void (*func_array_begin_polygon)(void *, gsglPage *p, READ_TOKEN),
	void (*func_array_end_polygon)(void *, gsglPage *p, READ_TOKEN),
	void (*func_array_begin_line)(void *, gsglPage *p, READ_TOKEN),
	void (*func_array_end_line)(void *, gsglPage *p, READ_TOKEN),
	void (*func_begin_point)(void *, gsglPage *p, READ_TOKEN),
	void (*func_end_point)(void *, gsglPage *p, READ_TOKEN),
	void (*func_begin_line)(void *, gsglPage *p, READ_TOKEN),
	void (*func_end_line)(void *, gsglPage *p, READ_TOKEN),
	void (*func_begin_poly)(void *, gsglPage *p, READ_TOKEN),
	void (*func_end_poly)(void *, gsglPage *p, READ_TOKEN),
	void (*func_normal)(void *, gsglPage *p, READ_TOKEN),
	void (*func_begin_triangles)(void *, gsglPage *p, READ_TOKEN),
	void (*func_end_triangles)(void *, gsglPage *p, READ_TOKEN),
	void (*func_begin_triangle_strip)(void *, gsglPage *p, READ_TOKEN),
	void (*func_end_triangle_strip)(void *, gsglPage *p, READ_TOKEN),
	void (*func_begin_triangle_fan)(void *, gsglPage *p, READ_TOKEN),
	void (*func_end_triangle_fan)(void *, gsglPage *p, READ_TOKEN),
	void (*func_clip_3d_fileonly)(void *, gsglPage *p, READ_TOKEN),
	void (*func_clip_unmovable)(void *, gsglPage *p, READ_TOKEN),
	void (*func_del_clip)(void *, gsglPage *p, READ_TOKEN),
	void (*func_modelview)(void *, gsglPage *p, READ_TOKEN),
	void (*func_rgb)(void *, gsglPage *p, READ_TOKEN),
	void (*func_array_element_4byte)(void *, gsglPage *p, READ_TOKEN),
	void (*func_rgba)(void *, gsglPage *p, READ_TOKEN),
	void (*func_indexcolor)(void *, gsglPage *p, READ_TOKEN),
	void (*func_linewidth)(void *, gsglPage *p, READ_TOKEN),
	void (*func_linewidth2)(void *, gsglPage *p, READ_TOKEN),
	void (*func_projection)(void *, gsglPage *p, READ_TOKEN),
	void (*func_3d_3)(void *, gsglPage *p, READ_TOKEN),
	void (*func_projection_type)(void *, gsglPage *p, READ_TOKEN),
	void (*func_enable_lighting)(void *, gsglPage *p, READ_TOKEN),
	void (*func_disable_lighting)(void *, gsglPage *p, READ_TOKEN),
	void (*func_begin_pageinfo)(void *, gsglPage *p, READ_TOKEN),
	void (*func_end_pageinfo)(void *, gsglPage *p, READ_TOKEN),
	void (*func_begin_head)(void *, gsglPage *p, READ_TOKEN),
	void (*func_end_head)(void *, gsglPage *p, READ_TOKEN),
	void (*func_begin_page)(void *, gsglPage *p, READ_TOKEN),
	void (*func_end_page)(void *, gsglPage *p, READ_TOKEN),
	void (*func_begin_anime)(void *, gsglPage *p, READ_TOKEN),
	void (*func_end_anime)(void *, gsglPage *p, READ_TOKEN),
	void (*func_enable_client_vertex)(void *, gsglPage *p, READ_TOKEN),
	void (*func_enable_client_color)(void *, gsglPage *p, READ_TOKEN),
	void (*func_begin_gouraud_only)(void *, gsglPage *p, READ_TOKEN),
	void (*func_end_gouraud_only)(void *, gsglPage *p, READ_TOKEN),
	void (*func_depth_by_deep)(void *, gsglPage *p, READ_TOKEN),
	void (*func_depth_by_shallow)(void *, gsglPage *p, READ_TOKEN),
	void (*func_depth_by_mean)(void *, gsglPage *p, READ_TOKEN),
	void (*func_depth_by_point)(void *, gsglPage *p, READ_TOKEN),
	void (*func_depth_by_vec_prod)(void *, gsglPage *p, READ_TOKEN),
	void (*func_depth_by_vec_prod2)(void *, gsglPage *p, READ_TOKEN),
	void (*func_vertex2_projctd)(void *, gsglPage *p, READ_TOKEN),
	void (*func_vertex3_projctd)(void *, gsglPage *p, READ_TOKEN),
	void (*func_3d_2)(void *, gsglPage *p, READ_TOKEN),
	void (*func_begin_object)(void *, gsglPage *p, READ_TOKEN),
	void (*func_end_object)(void *, gsglPage *p, READ_TOKEN),
	void (*func_normal_no_specify)(void *, gsglPage *p, READ_TOKEN),
	void (*func_normal_by_scal_prod)(void *, gsglPage *p, READ_TOKEN),
	void (*func_depth_by_vect_prod_old)(void *, gsglPage *p, READ_TOKEN),
	void (*func_depth_by_point_old)(void *, gsglPage *p, READ_TOKEN),
	void (*func_begin_fformat)(void *, gsglPage *p, READ_TOKEN),
	void (*func_end_fformat)(void *, gsglPage *p, READ_TOKEN),
	void (*func_3d)(void *, gsglPage *p, READ_TOKEN),
	void (*func_line_pattern_0)(void *, gsglPage *p, READ_TOKEN),
	void (*func_line_pattern_1)(void *, gsglPage *p, READ_TOKEN),
	void (*func_line_pattern_2)(void *, gsglPage *p, READ_TOKEN),
	void (*func_line_pattern_3)(void *, gsglPage *p, READ_TOKEN),
	void (*func_line_pattern_4)(void *, gsglPage *p, READ_TOKEN),
	void (*func_line_pattern_5)(void *, gsglPage *p, READ_TOKEN),
	void (*func_line_pattern_6)(void *, gsglPage *p, READ_TOKEN),
	void (*func_line_pattern_7)(void *, gsglPage *p, READ_TOKEN),
	void (*func_enable_depth_test)(void *, gsglPage *p, READ_TOKEN),
	void (*func_disable_depth_test)(void *, gsglPage *p, READ_TOKEN),
	void (*func_enable_polygonoffset)(void *, gsglPage *p, READ_TOKEN),
	void (*func_disable_polygonoffset)(void *, gsglPage *p, READ_TOKEN),
	void (*func_pstext)(void *, gsglPage *p, READ_TOKEN),
	void (*func_pstext_end)(void *, gsglPage *p, READ_TOKEN),
	void (*func_gui_button)(void *, gsglPage *p, READ_TOKEN),
	void (*func_gui_combo)(void *, gsglPage *p, READ_TOKEN),
	void (*func_gui_spinf)(void *, gsglPage *p, READ_TOKEN),
	void (*func_gui_spini)(void *, gsglPage *p, READ_TOKEN),
	void (*func_gui_check)(void *, gsglPage *p, READ_TOKEN),
	void (*func_gui_edit)(void *, gsglPage *p, READ_TOKEN),
	void (*func_gui_label)(void *, gsglPage *p, READ_TOKEN),
	void (*func_gui_external)(void *, gsglPage *p, READ_TOKEN),
	void (*func_default)(unsigned int, void *, gsglPage *p, READ_TOKEN))
{
	unsigned int token;

	token = func_inq_token (user_data, p, read_token);
	GSGL_DEBUG(fprintf (stderr, "token = %d\n", token));
	switch (token) {
	case GSGL_FILE_VERTEX2:
		GSGL_DEBUG(fprintf (stderr, "vertex2\n"));
		func_vertex2 (user_data, p, read_token);
		break;
	case GSGL_FILE_VERTEX3:
		GSGL_DEBUG(fprintf (stderr, "vertex3\n"));
		func_vertex3 (user_data, p, read_token);
		break;
	case GSGL_FILE_MODE_NORMAL:
		GSGL_DEBUG(fprintf (stderr, "normal mode\n"));
		func_mode_normal (user_data, p, read_token);
		break;
	case GSGL_FILE_MODE_2D:
		GSGL_DEBUG(fprintf (stderr, "2d mode\n"));
		func_mode_2d (user_data, p, read_token);
		break;
	case GSGL_FILE_MODE_3D:
		GSGL_DEBUG(fprintf (stderr, "3d mode\n"));
		func_mode_3d (user_data, p, read_token);
		break;
	case GSGL_FILE_SEPARATE_OBJECTS:
		GSGL_DEBUG(fprintf (stderr, "separate objects\n"));
		func_separate_objects (user_data, p, read_token);
		break;
	case GSGL_FILE_ARRAY_BEGIN_POLYGON:
		GSGL_DEBUG(fprintf (stderr, "array begin polygon\n"));
		func_array_begin_polygon (user_data, p, read_token);
		break;
	case GSGL_FILE_ARRAY_END_POLYGON:
		GSGL_DEBUG(fprintf (stderr, "array end polygon\n"));
		func_array_end_polygon (user_data, p, read_token);
		break;
	case GSGL_FILE_ARRAY_BEGIN_LINE:
		GSGL_DEBUG(fprintf (stderr, "array begin line\n"));
		func_array_begin_line (user_data, p, read_token);
		break;
	case GSGL_FILE_ARRAY_END_LINE:
		GSGL_DEBUG(fprintf (stderr, "array end line\n"));
		func_array_end_line (user_data, p, read_token);
		break;
	case GSGL_FILE_BEGIN_POINT:
		GSGL_DEBUG(fprintf (stderr, "begin point\n"));
		func_begin_point (user_data, p, read_token);
		break;
	case GSGL_FILE_END_POINT:
		GSGL_DEBUG(fprintf (stderr, "end point\n"));
		func_end_point (user_data, p, read_token);
		break;
	case GSGL_FILE_BEGIN_LINE:
		GSGL_DEBUG(fprintf (stderr, "begin line\n"));
		func_begin_line (user_data, p, read_token);
		break;
	case GSGL_FILE_END_LINE:
		GSGL_DEBUG(fprintf (stderr, "end line\n"));
		func_end_line (user_data, p, read_token);
		break;
	case GSGL_FILE_BEGIN_POLY:
		GSGL_DEBUG(fprintf (stderr, "begin poly\n"));
		func_begin_poly (user_data, p, read_token);
		break;
	case GSGL_FILE_END_POLY:
		GSGL_DEBUG(fprintf (stderr, "end poly\n"));
		func_end_poly (user_data, p, read_token);
		break;
	case GSGL_FILE_NORMAL:
		GSGL_DEBUG(fprintf (stderr, "normal vector\n"));
		func_normal (user_data, p, read_token);
		break;
	case GSGL_FILE_BEGIN_TRIANGLES:
		GSGL_DEBUG(fprintf (stderr, "begin triangles\n"));
		func_begin_triangles (user_data, p, read_token);
		break;
	case GSGL_FILE_END_TRIANGLES:
		GSGL_DEBUG(fprintf (stderr, "end triangles\n"));
		func_end_triangles (user_data, p, read_token);
		break;
	case GSGL_FILE_BEGIN_TRIANGLE_STRIP:
		GSGL_DEBUG(fprintf (stderr, "begin triangle strip\n"));
		func_begin_triangle_strip (user_data, p, read_token);
		break;
	case GSGL_FILE_END_TRIANGLE_STRIP:
		GSGL_DEBUG(fprintf (stderr, "end triangle strip\n"));
		func_end_triangle_strip (user_data, p, read_token);
		break;
	case GSGL_FILE_BEGIN_TRIANGLE_FAN:
		GSGL_DEBUG(fprintf (stderr, "begin triangle fan\n"));
		func_begin_triangle_fan (user_data, p, read_token);
		break;
	case GSGL_FILE_END_TRIANGLE_FAN:
		GSGL_DEBUG(fprintf (stderr, "end triangle fan\n"));
		func_end_triangle_fan (user_data, p, read_token);
		break;
	case GSGL_FILE_CLIP_3D_FILEONLY:
		GSGL_DEBUG(fprintf (stderr, "clip 3d fileonly\n"));
		func_clip_3d_fileonly (user_data, p, read_token);
		break;
	case GSGL_FILE_CLIP_UNMOVABLE:
		GSGL_DEBUG(fprintf (stderr, "clip unmovable\n"));
		func_clip_unmovable (user_data, p, read_token);
		break;
	case GSGL_FILE_DEL_CLIP_PLANE:
		GSGL_DEBUG(fprintf (stderr, "del clip\n"));
		func_del_clip (user_data, p, read_token);
		break;
	case GSGL_FILE_MODELVIEW:
		GSGL_DEBUG(fprintf (stderr, "modelview\n"));
		func_modelview (user_data, p, read_token);
		break;
	case GSGL_FILE_RGB:
		GSGL_DEBUG(fprintf (stderr, "rgb\n"));
		func_rgb (user_data, p, read_token);
		break;
	case GSGL_FILE_ARRAY_ELEMENT_4BYTE:
		GSGL_DEBUG(fprintf (stderr, "array element 4byte\n"));
		func_array_element_4byte (user_data, p, read_token);
		break;
	case GSGL_FILE_RGBA:
		GSGL_DEBUG(fprintf (stderr, "rgba\n"));
		func_rgba (user_data, p, read_token);
		break;
	case GSGL_FILE_INDEXCOLOR:
		GSGL_DEBUG(fprintf (stderr, "indexcolor\n"));
		func_indexcolor (user_data, p, read_token);
		break;
	case GSGL_FILE_LINEWIDTH:
		GSGL_DEBUG(fprintf (stderr, "linewidth (cm)\n"));
		func_linewidth (user_data, p, read_token);
		break;
	case GSGL_FILE_LINEWIDTH2:
		GSGL_DEBUG(fprintf (stderr, "linewidth2 (pt)\n"));
		func_linewidth2 (user_data, p, read_token);
		break;
	case GSGL_FILE_PROJECTION:
		GSGL_DEBUG(fprintf (stderr, "projection\n"));
		func_projection (user_data, p, read_token);
		break;
	case GSGL_FILE_3D_3:
		GSGL_DEBUG(fprintf (stderr, "3d_3\n"));
		func_3d_3 (user_data, p, read_token);
		break;
	case GSGL_FILE_PROJECTIONTYPE:
		GSGL_DEBUG(fprintf (stderr, "projection type\n"));
		func_projection_type (user_data, p, read_token);
		break;
	case GSGL_FILE_ENABLE_LIGHTING:
		GSGL_DEBUG(fprintf (stderr, "enable lighting\n"));
		func_enable_lighting (user_data, p, read_token);
		break;
	case GSGL_FILE_DISABLE_LIGHTING:
		GSGL_DEBUG(fprintf (stderr, "disable lighting\n"));
		func_disable_lighting (user_data, p, read_token);
		break;
	case GSGL_FILE_BEGIN_PAGEINFO:
		GSGL_DEBUG(fprintf (stderr, "begin pageinfo\n"));
		func_begin_pageinfo (user_data, p, read_token);
		p = gsgl_inq_page ();
		break;
	case GSGL_FILE_END_PAGEINFO: 
		GSGL_DEBUG(fprintf (stderr, "end pageinfo\n"));
		func_end_pageinfo (user_data, p, read_token);
		p = gsgl_inq_page ();
		break;
	case GSGL_FILE_BEGIN_HEAD:
		GSGL_DEBUG(fprintf (stderr, "begin head\n"));
		func_begin_head (user_data, p, read_token);
		p = gsgl_inq_page ();
		break;
	case GSGL_FILE_END_HEAD:
		GSGL_DEBUG(fprintf (stderr, "end head\n"));
		func_end_head (user_data, p, read_token);
		p = gsgl_inq_page ();
		break;
	case GSGL_FILE_BEGIN_PAGE:
		GSGL_DEBUG(fprintf (stderr, "begin page\n"));
		func_begin_page (user_data, p, read_token);
		p = gsgl_inq_page ();
		break;
	case GSGL_FILE_END_PAGE:
		GSGL_DEBUG(fprintf (stderr, "end page\n"));
		func_end_page (user_data, p, read_token);
		break;
	case GSGL_FILE_BEGIN_ANIME:
		GSGL_DEBUG(fprintf (stderr, "begin anime\n"));
		func_begin_anime (user_data, p, read_token);
		break;
	case GSGL_FILE_END_ANIME:
		GSGL_DEBUG(fprintf (stderr, "end anime\n"));
		func_end_anime (user_data, p, read_token);
		break;
	case GSGL_FILE_ENABLE_CLIENT_VERTEX:
		GSGL_DEBUG(fprintf (stderr, "enable client vertex\n"));
		func_enable_client_vertex (user_data, p, read_token);
		break;
	case GSGL_FILE_ENABLE_CLIENT_COLOR:
		GSGL_DEBUG(fprintf (stderr, "enable client color\n"));
		func_enable_client_color (user_data, p, read_token);
		break;
	case GSGL_FILE_PS_BEGIN_GOURAUD_ONLY:
		GSGL_DEBUG(fprintf (stderr, "begin gouraud only\n"));
		func_begin_gouraud_only (user_data, p, read_token);
		break;
	case GSGL_FILE_PS_END_GOURAUD_ONLY:
		GSGL_DEBUG(fprintf (stderr, "end gouraud only\n"));
		func_end_gouraud_only (user_data, p, read_token);
		break;
	case GSGL_FILE_PS_DEPTH_BY_DEEP:
		GSGL_DEBUG(fprintf (stderr, "depth by deep\n"));
		func_depth_by_deep (user_data, p, read_token);
		break;
	case GSGL_FILE_PS_DEPTH_BY_SHALLOW:
		GSGL_DEBUG(fprintf (stderr, "depth by shallow\n"));
		func_depth_by_shallow (user_data, p, read_token);
		break;
	case GSGL_FILE_PS_DEPTH_BY_MEAN:
		GSGL_DEBUG(fprintf (stderr, "depth by mean\n"));
		func_depth_by_mean (user_data, p, read_token);
		break;
	case GSGL_FILE_PS_DEPTH_BY_POINT:
		GSGL_DEBUG(fprintf (stderr, "depth by point\n"));
		func_depth_by_point (user_data, p, read_token);
		break;
	case GSGL_FILE_PS_DEPTH_BY_VECT_PROD:
		GSGL_DEBUG(fprintf (stderr, "depth by vector product\n"));
		func_depth_by_vec_prod (user_data, p, read_token);
		break;
	case GSGL_FILE_PS_DEPTH_BY_VECT_PROD2:
		GSGL_DEBUG(fprintf (stderr, "depth by vector product 2\n"));
		func_depth_by_vec_prod2 (user_data, p, read_token);
		break;
	case GSGL_FILE_PS_TEXT:
		GSGL_DEBUG(fprintf (stderr, "ps text\n"));		
		func_pstext (user_data, p, read_token);
		break;
	case GSGL_FILE_PS_TEXT_END:
		GSGL_DEBUG(fprintf (stderr, "ps text end\n"));		
		func_pstext_end (user_data, p, read_token);
		break;
	case GSGL_FILE_GUI_BUTTON_BEGIN:
		GSGL_DEBUG(fprintf (stderr, "gui button\n"));		
		func_gui_button (user_data, p, read_token);
		break;
	case GSGL_FILE_GUI_COMBO_BEGIN:
		GSGL_DEBUG(fprintf (stderr, "gui combo\n"));		
		func_gui_combo (user_data, p, read_token);
		break;
	case GSGL_FILE_GUI_SPINF_BEGIN:
		GSGL_DEBUG(fprintf (stderr, "gui spinf\n"));		
		func_gui_spinf (user_data, p, read_token);
		break;
	case GSGL_FILE_GUI_SPINI_BEGIN:
		GSGL_DEBUG(fprintf (stderr, "gui spini\n"));		
		func_gui_spini (user_data, p, read_token);
		break;
	case GSGL_FILE_GUI_CHECK_BEGIN:
		GSGL_DEBUG(fprintf (stderr, "gui check\n"));		
		func_gui_check (user_data, p, read_token);
		break;
	case GSGL_FILE_GUI_EDIT_BEGIN:
		GSGL_DEBUG(fprintf (stderr, "gui edit\n"));
		func_gui_edit (user_data, p, read_token);
		break;
	case GSGL_FILE_GUI_LABEL_BEGIN:
		GSGL_DEBUG(fprintf (stderr, "gui label\n"));
		func_gui_label (user_data, p, read_token);
		break;
	case GSGL_FILE_GUI_SET_EXTERNAL:
		GSGL_DEBUG(fprintf (stderr, "gui external\n"));
		func_gui_external (user_data, p, read_token);
		break;
	case GSGL_FILE_VERTEX2_PRJCTD: /* obsolute */
		GSGL_DEBUG(fprintf (stderr, "vertex2 projected\n"));
		func_vertex2_projctd (user_data, p, read_token);
		break;
	case GSGL_FILE_VERTEX3_PRJCTD: /* obsolute */
		GSGL_DEBUG(fprintf (stderr, "vertex3 projected\n"));
		func_vertex3_projctd (user_data, p, read_token);
		break;
	case GSGL_FILE_3D_2: /* obsolute */
		GSGL_DEBUG(fprintf (stderr, "3d 2\n"));
		func_3d_2 (user_data, p, read_token);
		break;      
	case GSGL_FILE_BEGIN_OBJECT: /* obsolute */
		GSGL_DEBUG(fprintf (stderr, "begin object\n"));
		func_begin_object (user_data, p, read_token);
		break;
	case GSGL_FILE_END_OBJECT: /* obsolute */
		GSGL_DEBUG(fprintf (stderr, "end object\n"));
		func_end_object (user_data, p, read_token);
		break;
	case GSGL_FILE_NORMAL_NO_SPECIFY: /* obsolute */
		GSGL_DEBUG(fprintf (stderr, "normal no specify\n"));
		func_normal_no_specify (user_data, p, read_token);
		break;
	case GSGL_FILE_NORMAL_BY_SCAL_PROD: /* obsolute */
		GSGL_DEBUG(fprintf (stderr, "normal by scal prod\n"));
		func_normal_by_scal_prod (user_data, p, read_token);
		break;
	case GSGL_FILE_PS_DEPTH_BY_VECT_PROD_OLD: /* obsolute */
		GSGL_DEBUG(fprintf (stderr, "depth by vect prod old\n"));
		func_depth_by_vect_prod_old (user_data, p, read_token);
		break;
	case GSGL_FILE_PS_DEPTH_BY_POINT_OLD: /* obsolute */
		GSGL_DEBUG(fprintf (stderr, "depth by point old\n"));
		func_depth_by_point_old (user_data, p, read_token);
		break;
	case GSGL_FILE_BEGIN_FFORMAT: /* obsolute */
		GSGL_DEBUG(fprintf (stderr, "begin fformat\n"));
		func_begin_fformat (user_data, p, read_token);
		break;
	case GSGL_FILE_END_FFORMAT: /* obsolute */
		GSGL_DEBUG(fprintf (stderr, "end fformat\n"));
		func_end_fformat (user_data, p, read_token);
		break;
	case GSGL_FILE_3D: /* obsolute */
		GSGL_DEBUG(fprintf (stderr, "3d\n"));
		func_3d (user_data, p, read_token);
		break;
	case GSGL_FILE_LINEPATTERN_0: /* obsolute */
		GSGL_DEBUG(fprintf (stderr, "line pattern 0\n"));
		func_line_pattern_0 (user_data, p, read_token);
		break;
	case GSGL_FILE_LINEPATTERN_1: /* obsolute */
		GSGL_DEBUG(fprintf (stderr, "line pattern 1\n"));
		func_line_pattern_1 (user_data, p, read_token);
		break;
	case GSGL_FILE_LINEPATTERN_2: /* obsolute */
		GSGL_DEBUG(fprintf (stderr, "line pattern 2\n"));
		func_line_pattern_2 (user_data, p, read_token);
		break;
	case GSGL_FILE_LINEPATTERN_3: /* obsolute */
		GSGL_DEBUG(fprintf (stderr, "line pattern 3\n"));
		func_line_pattern_3 (user_data, p, read_token);
		break;
	case GSGL_FILE_LINEPATTERN_4: /* obsolute */
		GSGL_DEBUG(fprintf (stderr, "line pattern 4\n"));
		func_line_pattern_4 (user_data, p, read_token);
		break;
	case GSGL_FILE_LINEPATTERN_5: /* obsolute */
		GSGL_DEBUG(fprintf (stderr, "line pattern 5\n"));
		func_line_pattern_5 (user_data, p, read_token);
		break;
	case GSGL_FILE_LINEPATTERN_6: /* obsolute */
		GSGL_DEBUG(fprintf (stderr, "line pattern 6\n"));
		func_line_pattern_6 (user_data, p, read_token);
		break;
	case GSGL_FILE_LINEPATTERN_7: /* obsolute */
		GSGL_DEBUG(fprintf (stderr, "line pattern 7\n"));
		func_line_pattern_7 (user_data, p, read_token);
		break;
	case GSGL_FILE_ENABLE_DEPTHTEST: /* obsolute *//* 3D mode */
		GSGL_DEBUG(fprintf (stderr, "enable depth test\n"));
		func_enable_depth_test (user_data, p, read_token);
		break;
	case GSGL_FILE_DISABLE_DEPTHTEST: /* obsolute *//* normal or 2D mode */
		GSGL_DEBUG(fprintf (stderr, "disable depth test\n"));
		func_disable_depth_test (user_data, p, read_token);
		break;
	case GSGL_FILE_ENABLE_POLYGONOFFSET: /* obsolute */
		GSGL_DEBUG(fprintf (stderr, "enable polygon offset\n"));
		func_enable_polygonoffset (user_data, p, read_token);
		break;
	case GSGL_FILE_DISABLE_POLYGONOFFSET: /* obsolute */
		GSGL_DEBUG(fprintf (stderr, "disable polygon offset\n"));
		func_disable_polygonoffset (user_data, p, read_token);
		break;
	default:
		GSGL_DEBUG(fprintf (stderr, "default\n"));
		func_default (token, user_data, p, read_token);
		break;
	}
	return p;
}

unsigned int gsgl_file_format_read_vertex_template (
	void *data, gsglPage *p, int datatype, gsglboolean allocate,
	READ_TOKEN, void (*func_float)(void *, float *, READ_TOKEN))
{
	unsigned int token, token1, token2;
	unsigned int nx, ny;
	int i, j;
	float *zdata = NULL, *xdata = NULL, *ydata = NULL;
	float xmin = 0.0, xmax = 0.0, ymin = 0.0, ymax = 0.0;
	int *idata = NULL;
	float *fdata = NULL;
	float dummy;
	unsigned int vertexsize;

	read_token (data, &token);
	if (token != GSGL_FILE_CLIENT_VERTEX_SIZE) gsgl_error ("gsgl_file_format_read_vertex_template", "token != GSGL_FILE_CLIENT_VERTEX_SIZE");
	read_token (data, &token);
	vertexsize = token;

	read_token (data, &token);
	if (token == GSGL_FILE_CLIENT_VERTEX_TOTALSIZE_2BYTE) {
		read_token (data, &token);
	} else if (token == GSGL_FILE_CLIENT_VERTEX_TOTALSIZE_4BYTE) {
		read_token (data, &token);
		read_token (data, &token);
	}
	read_token (data, &token);
	if (token == GSGL_FILE_CLIENT_VERTEX_NX_2BYTE) {
		read_token (data, &nx);
	} else if (token == GSGL_FILE_CLIENT_VERTEX_NX_4BYTE) {
		read_token (data, &token1);
		read_token (data, &token2);
		nx = gsgl_convert_tokens_to_4byte (token1, token2);
	}
	read_token (data, &token);
	if (token == GSGL_FILE_CLIENT_VERTEX_NY_2BYTE) {
		read_token (data, &ny);
	} else if (token == GSGL_FILE_CLIENT_VERTEX_NY_4BYTE) {
		read_token (data, &token1);
		read_token (data, &token2);
		ny = gsgl_convert_tokens_to_4byte (token1, token2);
	}
	switch (datatype) {
	case GSGL_DATA_ORTHO_EQUIV_INTERVAL:
		read_token (data, &token); /*GSGL_FILE_CLIENT_VERTEX_XMIN*/
		func_float (data, &xmin, read_token);
		read_token (data, &token); /*GSGL_FILE_CLIENT_VERTEX_XMAX*/
		func_float (data, &xmax, read_token);
		read_token (data, &token); /*GSGL_FILE_CLIENT_VERTEX_YMIN*/
		func_float (data, &ymin, read_token);
		read_token (data, &token); /*GSGL_FILE_CLIENT_VERTEX_YMAX*/
 		func_float (data, &ymax, read_token);
		break;
	case GSGL_DATA_ORTHO_UNEQU_INTERVAL:
	case GSGL_DATA_CYLIN_UNEQU_INTERVAL:
		if (allocate)
			xdata = (float *) malloc (nx*sizeof(float));
		for (i=0; i<nx; i++) {
			if (allocate)
				func_float (data, &xdata[i], read_token);
			else
				func_float (data, &dummy, read_token);
		}
		if (allocate)
			ydata = (float *) malloc (ny*sizeof (float));
		for (i=0; i<ny; i++) {
			if (allocate)
				func_float (data, &ydata[i], read_token);
			else
				func_float (data, &dummy, read_token);
		}
		break;
	case GSGL_DATA_CYLIN_EQUIV_INTERVAL:
		if (allocate)
			xdata = (float *) malloc (nx*sizeof (float));
		for (i=0; i<nx; i++) {
			if (allocate)
				func_float (data, &xdata[i], read_token);
			else
				func_float (data, &dummy, read_token);
		}
		break;
	case GSGL_DATA_X_Y_Z_2D:
		if (allocate)
			xdata = (float *) malloc (nx*ny*sizeof(float));
		for (i=0; i<nx; i++) {
			for (j=0; j<ny; j++) {
				if (allocate)
					func_float (data, &xdata[j*nx+i], read_token);
				else
					func_float (data, &dummy, read_token);
			}
		}
		if (allocate)
			ydata = (float *) malloc (nx*ny*sizeof (float));
		for (i=0; i<nx; i++) {
			for (j=0; j<ny; j++) {
				if (allocate)
					func_float (data, &ydata[j*nx+i], read_token);
				else
					func_float (data, &dummy, read_token);
			}
		}
		break;
	case GSGL_DATA_FEM_TRIANGLE:
		if (allocate)
			xdata = (float *) malloc (nx*sizeof (float));
		for (i=0; i<nx; i++) {
			if (allocate)
				func_float (data, &xdata[i], read_token);
			else
				func_float (data, &dummy, read_token);		
		}
		if (allocate)
			ydata = (float *) malloc (nx*sizeof (float));
		for (i=0; i<nx; i++) {
			if (allocate)
				func_float (data, &ydata[i], read_token);
			else
				func_float (data, &dummy, read_token);
		}
		break;
	default:
		break;
	}
	if (datatype != GSGL_DATA_FEM_TRIANGLE) {
		if (allocate) {
			zdata = (float *) malloc (nx*ny*sizeof (float));
			for (i=0; i<nx; i++) {
				for (j=0; j<ny; j++) {
					func_float (data, &zdata[j*nx+i], read_token);
					/* Fortran format memory array */
				}
			}
		} else {
			for (i=0; i<nx; i++) {
				for (j=0; j<ny; j++) {
					func_float (data, &dummy, read_token);
				}
			}
		}
	} else {
		if (allocate) {
			zdata = (float *) malloc (nx*sizeof (float));
			for (i=0; i<nx; i++)
				func_float (data, &zdata[i], read_token);
		} else {
			for (i=0; i<nx; i++)
				func_float (data, &dummy, read_token);
		}
		
		read_token (data, &token);
		if (token == GSGL_FILE_CLIENT_FEM_IELM_4BYTE) {
			if (allocate) {
				idata = (int *) malloc (3*ny*sizeof (int));
				for (i=0; i<ny*3; i++) {
					read_token (data, &token1);
					read_token (data, &token2);
					idata[i] = gsgl_convert_tokens_to_4byte (token1,token2);
				}
			} else {
				for (i=0; i<ny*3; i++) {
					read_token (data, &token);
					read_token (data, &token);
				}
			}
		}
	}

	if (allocate) {
		gsgl_pointer_set (datatype, nx, nx, ny, zdata, xdata, ydata, 
				  xmin, xmax, ymin, ymax, idata, fdata, p);
		if (zdata) { free (zdata); zdata=NULL; }
		if (xdata) { free (xdata); xdata=NULL; }
		if (ydata) { free (ydata); ydata=NULL; }
		if (idata) { free (idata); idata=NULL; }
	}
	return vertexsize;
}

unsigned int gsgl_file_format_read_inq_token (void *data, gsglPage *p, READ_TOKEN)
{
	unsigned int token;
	
	read_token (data, &token);
	return token;
}

void gsgl_file_format_read_float (void *data, float *x, READ_TOKEN)
{
	unsigned int token1, token2;
	read_token (data, &token1);
	read_token (data, &token2);
	*x = gsgl_convert_token_to_float (token1, token2);
}

void gsgl_file_format_read_float_754 (void *data, float *x, READ_TOKEN)
{
	unsigned int token1, token2;
	read_token (data, &token1);
	read_token (data, &token2);
	*x = gsgl_int4_to_float_ieee754 (token1, token2);
	/*printf ("token1 = %d, token2 = %d, x = %f\n", token1, token2, *x);*/
}

void gsgl_file_format_read_vertex2 (void *data, gsglPage *p, READ_TOKEN)
{
	gsglvec3f vec;

	gsgl_file_format_read_float (data, &vec.x, read_token);
	gsgl_file_format_read_float (data, &vec.y, read_token);
	gsgl_drv_vertex2 (vec.x, vec.y, p);
	GSGL_DEBUG(fprintf (stderr, "%f %f\n", vec.x, vec.y));
}

void gsgl_file_format_read_mode_normal (void *data, gsglPage *p, READ_TOKEN)
{
	gsgl_coordinate_set (GSGL_COORD_NORMAL, p);
	gsgl_displaylist_begin (GSGL_LIST_STATUS, p);
	gsgl_drv_enable_antialiasing (p);
	gsgl_drv_normalmode (p);
}

void gsgl_file_format_read_mode_2d (void *data, gsglPage *p, READ_TOKEN)
{
	gsgl_coordinate_set (GSGL_COORD_2D, p);
	gsgl_displaylist_begin (GSGL_LIST_STATUS, p);
	gsgl_drv_enable_antialiasing (p);
	gsgl_drv_2dmode (p);
}

void gsgl_file_format_read_mode_3d (void *data, gsglPage *p, READ_TOKEN)
{
	gsgl_coordinate_set (GSGL_COORD_3D, p);
	gsgl_displaylist_begin (GSGL_LIST_STATUS, p);
	gsgl_drv_disable_antialiasing (p);
	gsgl_drv_3dmode (p);
}

void gsgl_file_format_read_separate_objects (void *data, gsglPage *p, READ_TOKEN)
{
	gsgl_displaylist_begin (GSGL_LIST_STATUS, p);
	gsgl_drv_separate_objects (p);
}

void gsgl_file_format_read_array_begin_polygon (void *data, gsglPage *p, READ_TOKEN)
{
	int coord;

	gsgl_coordinate_inq (&coord, p);
	if (coord == GSGL_COORD_3D || coord == GSGL_COORD_TEXT3D) {
		gsgl_displaylist_begin (GSGL_LIST_3DOBJ_FILEONLY, p);
	} else {
		gsgl_displaylist_begin (GSGL_LIST_2DOBJ_FILEONLY, p);
	}
	gsgl_drv_glarray_begin_polygon (p);
}

void gsgl_file_format_read_array_end_polygon (void *data, gsglPage *p, READ_TOKEN)
{
	gsgl_drv_glarray_end_polygon (p);
}
	
void gsgl_file_format_read_array_begin_line (void *data, gsglPage *p, READ_TOKEN)
{
	int coord;

	gsgl_coordinate_inq (&coord, p);
	if (coord == GSGL_COORD_3D || coord == GSGL_COORD_TEXT3D) {
		gsgl_displaylist_begin (GSGL_LIST_3DOBJ_FILEONLY, p);
	} else {
		gsgl_displaylist_begin (GSGL_LIST_2DOBJ_FILEONLY, p);
	}
	gsgl_drv_glarray_begin_lines (p);
}
	
void gsgl_file_format_read_array_end_line (void *data, gsglPage *p, READ_TOKEN)
{
	gsgl_drv_glarray_end_lines (p);
}

void gsgl_file_format_read_begin_point (void *data1, gsglPage *p, READ_TOKEN)
{
	/*gsgl_begin_points (p);*/
}

void gsgl_file_format_read_end_point (void *data1, gsglPage *p, READ_TOKEN)
{
	/*gsgl_end_points (p)*/
}

void gsgl_file_format_read_begin_triangles (void *data, gsglPage *p, READ_TOKEN)
{
	int coord;

	gsgl_coordinate_inq (&coord, p);
	if (coord == GSGL_COORD_3D || coord == GSGL_COORD_TEXT3D) {
		gsgl_displaylist_begin (GSGL_LIST_3DOBJ, p);
	} else {
		gsgl_displaylist_begin (GSGL_LIST_2DOBJ, p);
	}
	gsgl_drv_begin_triangles (p);
}

void gsgl_file_format_read_end_triangles (void *data, gsglPage *p, READ_TOKEN)
{
	gsgl_drv_end_triangles (p);
}

void gsgl_file_format_read_begin_triangle_strip (void *data, gsglPage *p, READ_TOKEN)
{
	int coord;

	gsgl_coordinate_inq (&coord, p);
	if (coord == GSGL_COORD_3D || coord == GSGL_COORD_TEXT3D) {
		gsgl_displaylist_begin (GSGL_LIST_3DOBJ, p);
	} else {
		gsgl_displaylist_begin (GSGL_LIST_2DOBJ, p);
	}
	gsgl_drv_begin_triangle_strip (p);
}

void gsgl_file_format_read_end_triangle_strip (void *data, gsglPage *p, READ_TOKEN)
{
	gsgl_drv_end_triangle_strip (p);
}

void gsgl_file_format_read_begin_triangle_fan (void *data, gsglPage *p, READ_TOKEN)
{
	int coord;

	gsgl_coordinate_inq (&coord, p);
	if (coord == GSGL_COORD_3D || coord == GSGL_COORD_TEXT3D) {
		gsgl_displaylist_begin (GSGL_LIST_3DOBJ, p);
	} else {
		gsgl_displaylist_begin (GSGL_LIST_2DOBJ, p);
	}
	gsgl_drv_begin_triangle_fan (p);
}

void gsgl_file_format_read_end_triangle_fan (void *data, gsglPage *p, READ_TOKEN)
{
	gsgl_drv_end_triangle_fan (p);
}

void gsgl_file_format_read_clip_3d_fileonly (void *data, gsglPage *p, READ_TOKEN)
{
	unsigned int token, clip_plane;
	float eqn[4];
	int i;

	read_token (data, &token);
	if (token != GSGL_FILE_CLIP_NUM) return;
	read_token (data, &clip_plane);
	read_token (data, &token);
	if (token != GSGL_FILE_CLIP_COEFF) return;
	for (i=0; i<4; i++) {
		gsgl_file_format_read_float (data, &eqn[i], read_token);
	}
	gsgl_displaylist_begin (GSGL_LIST_3DOBJ_FILEONLY, p);
	gsgl_drv_clip_enable_3d_fileonly (clip_plane, eqn, p);
}

void gsgl_file_format_read_clip_unmovable (void *data, gsglPage *p, READ_TOKEN)
{
	unsigned int token, clip_plane;
	float eqn[4];
	int i;

	read_token (data, &token);
	if (token != GSGL_FILE_CLIP_NUM) return;
	read_token (data, &clip_plane);
	read_token (data, &token);
	if (token != GSGL_FILE_CLIP_COEFF) return;
	for (i=0; i<4; i++) {
		gsgl_file_format_read_float (data, &eqn[i], read_token);
	}
	gsgl_displaylist_begin (GSGL_LIST_STATUS, p);
	gsgl_drv_clip_enable_unmovable (clip_plane, eqn, p);
}

void gsgl_file_format_read_del_clip (void *data, gsglPage *p, READ_TOKEN)
{
	unsigned int token;
	
	read_token (data, &token);
	if (token != GSGL_FILE_DEL_CLIP_NUM) return;
	read_token (data, &token);
	gsgl_clip_disable (token, p);
}

void gsgl_file_format_read_modelview (void *data, gsglPage *p, READ_TOKEN)
{
	int i;
	float modelview_matrix[16];

	for (i=0; i<16 ; i++) {
		gsgl_file_format_read_float (data, &modelview_matrix[i], read_token);
	}
	GSGL_DEBUG(fprintf (stderr, "%f %f %f %f\n", 
			    modelview_matrix[0], modelview_matrix[4],
			    modelview_matrix[8], modelview_matrix[12]));
	GSGL_DEBUG(fprintf (stderr, "%f %f %f %f\n", 
			    modelview_matrix[1], modelview_matrix[5],
			    modelview_matrix[9], modelview_matrix[13]));
	GSGL_DEBUG(fprintf (stderr, "%f %f %f %f\n", 
			    modelview_matrix[2], modelview_matrix[6], 
			    modelview_matrix[10], modelview_matrix[14]));
	GSGL_DEBUG(fprintf (stderr, "%f %f %f %f\n", 
			    modelview_matrix[3], modelview_matrix[7], 
			    modelview_matrix[11], modelview_matrix[15]));
	gsgl_modelview_matrix (modelview_matrix, p);
}

void gsgl_file_format_read_rgb (void *data, gsglPage *p, READ_TOKEN)
{
	unsigned int token;
	float rgb[3];

	read_token (data, &token);
	rgb[0] = gsgl_convert_token_to_0to1 (token);
	read_token (data, &token);
	rgb[1] = gsgl_convert_token_to_0to1 (token);
	read_token (data, &token);
	rgb[2] = gsgl_convert_token_to_0to1 (token);
	GSGL_DEBUG(fprintf (stderr, "rgb = %f %f %f\n", rgb[0], rgb[1], rgb[2]));
	gsgl_rgb (rgb[0], rgb[1], rgb[2], p);
}

void gsgl_file_format_read_array_element_4byte (void *data, gsglPage *p, READ_TOKEN)
{
	unsigned int token, token1, token2;

	read_token (data, &token1);
	read_token (data, &token2);
	token = gsgl_convert_tokens_to_4byte (token1, token2);
	gsgl_glarray_element (token, p);
}

void gsgl_file_format_read_rgba (void *data, gsglPage *p, READ_TOKEN)
{
	unsigned int token;
	float rgb[3];

	read_token (data, &token);
	rgb[0] = gsgl_convert_token_to_0to1 (token);
	read_token (data, &token);
	rgb[1] = gsgl_convert_token_to_0to1 (token);
	read_token (data, &token);
	rgb[2] = gsgl_convert_token_to_0to1 (token);
	read_token (data, &token);
	GSGL_DEBUG(fprintf (stderr, "rgb = %f %f %f\n", rgb[0], rgb[1], rgb[2]));
	gsgl_rgb (rgb[0], rgb[1], rgb[2], p);
}

void gsgl_file_format_read_indexcolor (void *data, gsglPage *p, READ_TOKEN)
{
	unsigned int token;

	read_token (data, &token);
	GSGL_DEBUG(fprintf (stderr, "index = %d\n", token));
	gsgl_set_rs (-1.0, p);
	gsgl_color_index (token, p);
}

void gsgl_file_format_read_linewidth (void *data, gsglPage *p, READ_TOKEN)
{
	unsigned int token;

	read_token (data, &token);
	gsgl_linewidth (gsgl_convert_token_to_0to1 (token) * GSGL_PAPER_WIDTH, p);
}

void gsgl_file_format_read_linewidth2 (void *data, gsglPage *p, READ_TOKEN)
{
	unsigned int token;

	read_token (data, &token);
	gsgl_set_ws (-1.0, p);
	gsgl_linewidth_index (token, p);
}

void gsgl_file_format_read_projection (void *data, gsglPage *p, READ_TOKEN)
{
	int i;
	float proj[16];

	for (i=0; i<16 ; i++) {
		gsgl_file_format_read_float (data, &proj[i], read_token);
	}
	GSGL_DEBUG(fprintf (stderr, "%f %f %f %f\n", proj[0], proj[4], proj[8], proj[12]));
	GSGL_DEBUG(fprintf (stderr, "%f %f %f %f\n", proj[1], proj[5], proj[9], proj[13]));
	GSGL_DEBUG(fprintf (stderr, "%f %f %f %f\n", proj[2], proj[6], proj[10], proj[14]));
	GSGL_DEBUG(fprintf (stderr, "%f %f %f %f\n", proj[3], proj[7], proj[11], proj[15]));
	gsgl_projection_matrix (proj, p);
	gsgl_displaylist_end (p);
}

void gsgl_file_format_read_3d_3 (void *data, gsglPage *p, READ_TOKEN)
{
	unsigned int token;
	float depth;

	read_token (data, &token);
	if (token != GSGL_FILE_3D_DEPTH) return;
	gsgl_file_format_read_float (data, &depth, read_token);
	depth = GSGL_VIEW_DEPTH * depth;
	gsgl_depth3d (depth, p);
}

void gsgl_file_format_read_projection_type (void *data, gsglPage *p, READ_TOKEN)
{
	unsigned int token;

	read_token (data, &token);
	gsgl_projectiontype (token, p);
}

void gsgl_file_format_read_enable_lighting (void *data, gsglPage *p, READ_TOKEN)
{
	unsigned int token;

	read_token (data, &token);
	if (token != GSGL_FILE_LIGHTING_ENABLE_TYPE) return;
	read_token (data, &token);
	if (token != 1) return;
	read_token (data, &token);
	if (token != GSGL_FILE_LIGHTING_ENABLE_NUM) return;
	read_token (data, &token);
	gsgl_lighting_enable (p);
}

void gsgl_file_format_read_disable_lighting (void *data, gsglPage *p, READ_TOKEN)
{
	unsigned int token;

	read_token (data, &token);
	if (token != GSGL_FILE_LIGHTING_DISABLE_NUM) return;
	read_token (data, &token);
	gsgl_lighting_disable ();
}

void gsgl_file_format_read_begin_anime (void *data, gsglPage *p, READ_TOKEN)
{
	gsgl_animes ();
}

void gsgl_file_format_read_end_anime (void *data, gsglPage *p, READ_TOKEN)
{
	gsgl_animee ();
}

static void gsglSetColorPointer (unsigned int i, unsigned int total, float *rgb, gsglPage *p)
{
	gsgllist *list=NULL;
	GLfloat *data;
	
	if (i == 0) {
		if (total == 0) data = NULL;
		else data = malloc (total*sizeof (float));

		gsgl_clientstate_color_append (data, p);
	}
	if (total == 0) {
		gsgl_glarray3d_color (0, 0, NULL, p);
		return;
	}
	list = gsgl_inq_first_clientstate_color_pointer (p);
	list = gsgl_list_last (list);
	data = list->data;
	data[i+0] = rgb[0];
	data[i+1] = rgb[1];
	data[i+2] = rgb[2];
	if (i+2 == total-1) {
		gsgl_glarray3d_color (3, total, list->data, p);
	}
}

void gsgl_file_format_read_enable_client_color (void *data, gsglPage *p, READ_TOKEN)
{
	unsigned int token, bsize, totalsize=0, token1, token2;
	int i;
	float rgb[3];

	read_token (data, &token);
	/*if (token == GSGL_FILE_CLIENT_COLOR_SIZE) {*/
	read_token (data, &token);
	bsize = token;
	read_token (data, &token);
	if (token == GSGL_FILE_CLIENT_COLOR_TOTALSIZE_2BYTE) {
		read_token (data, &token);
		totalsize = token;
	} else if (token == GSGL_FILE_CLIENT_COLOR_TOTALSIZE_4BYTE) {
		read_token (data, &token1);
		read_token (data, &token2);
		totalsize = gsgl_convert_tokens_to_4byte (token1, token2);
	}
	if (bsize == 0 && totalsize == 0) {
		gsglSetColorPointer (0, 0, NULL, p);
	}
	for (i=0; i<totalsize; i++) {
		read_token (data, &token);
		if (i%3 == 0) {
			rgb[0] = gsgl_convert_token_to_0to1 (token);
		} else if (i%3 == 1) {
			rgb[1] = gsgl_convert_token_to_0to1 (token);
		} else if (i%3 == 2) {
			rgb[2] = gsgl_convert_token_to_0to1 (token);
			gsglSetColorPointer (i-2, totalsize, rgb, p);
		}
	}
}

void gsgl_file_format_read_begin_gouraud_only (void *data, gsglPage *p, READ_TOKEN)
{
	gsgl_ps_gouraudonly_begin (p);
}

void gsgl_file_format_read_end_gouraud_only (void *data, gsglPage *p, READ_TOKEN)
{
	gsgl_ps_gouraudonly_end (p);
}

void gsgl_file_format_read_depth_by_deep (void *data, gsglPage *p, READ_TOKEN)
{
	gsgl_ps_depth_by_deep (p);
}

void gsgl_file_format_read_depth_by_shallow (void *data, gsglPage *p, READ_TOKEN)
{
	gsgl_ps_depth_by_shallow (p);
}

void gsgl_file_format_read_depth_by_mean (void *data, gsglPage *p, READ_TOKEN)
{
	gsgl_ps_depth_by_mean (p);
}

void gsgl_file_format_read_depth_by_point (void *data, gsglPage *p, READ_TOKEN)
{
	gsglvec3f vec;

	gsgl_file_format_read_float (data, &vec.x, read_token);
	gsgl_file_format_read_float (data, &vec.y, read_token);
	gsgl_file_format_read_float (data, &vec.z, read_token);
	gsgl_ps_depth_by_point (vec.x, vec.y, vec.z, p);
}

void gsgl_file_format_read_depth_by_vec_prod (void *data, gsglPage *p, READ_TOKEN)
{
	float x[3], y[3], z[3];
	
	gsgl_file_format_read_float (data, &x[0], read_token);
	gsgl_file_format_read_float (data, &y[0], read_token);
	gsgl_file_format_read_float (data, &z[0], read_token);
	gsgl_file_format_read_float (data, &x[1], read_token);
	gsgl_file_format_read_float (data, &y[1], read_token);
	gsgl_file_format_read_float (data, &z[1], read_token);
	gsgl_file_format_read_float (data, &x[2], read_token);
	gsgl_file_format_read_float (data, &y[2], read_token);
	gsgl_file_format_read_float (data, &z[2], read_token);
	gsgl_ps_depth_by_vectprod (x, y, z, p);
}

void gsgl_file_format_read_depth_by_vec_prod2 (void *data, gsglPage *p, READ_TOKEN)
{
	float x[3], y[3], z[3];
	unsigned int npoints;
	float *xp, *yp, *zp;
	int i;
	
	gsgl_file_format_read_float (data, &x[0], read_token);
	gsgl_file_format_read_float (data, &y[0], read_token);
	gsgl_file_format_read_float (data, &z[0], read_token);
	gsgl_file_format_read_float (data, &x[1], read_token);
	gsgl_file_format_read_float (data, &y[1], read_token);
	gsgl_file_format_read_float (data, &z[1], read_token);
	gsgl_file_format_read_float (data, &x[2], read_token);
	gsgl_file_format_read_float (data, &y[2], read_token);
	gsgl_file_format_read_float (data, &z[2], read_token);
	read_token (data, &npoints);
	xp = (float *) gsgl_malloc_atomic (sizeof(float)*npoints);
	yp = (float *) gsgl_malloc_atomic (sizeof(float)*npoints);
	zp = (float *) gsgl_malloc_atomic (sizeof(float)*npoints);
	for (i=0; i<npoints; i++) {
		gsgl_file_format_read_float (data, &xp[i], read_token);
		gsgl_file_format_read_float (data, &yp[i], read_token);
		gsgl_file_format_read_float (data, &zp[i], read_token);
	}
	gsgl_ps_depth_by_vectprod2 (x, y, z, npoints, xp, yp, zp, p);
}

void gsgl_file_format_read_do_nothing (void *data, gsglPage *p, READ_TOKEN)
{
}

void gsgl_file_format_read_pstext (void *data, gsglPage *p, READ_TOKEN)
{
	unsigned int token;
	int ifnts, ifntr, i, nmax, dim;
	float chh, chw, chsp, angle, tilt, x, y; 
	int *iascs;

	read_token (data, &token);
	if (token != GSGL_FILE_PS_TEXT_FNT) gsgl_error ("gsgl_file_format_read_pstext", "token!=GSGL_FILE_PS_TEXT_FNT");
	read_token (data, &token);
	ifnts = token;
	read_token (data, &token);
	if (token != GSGL_FILE_PS_TEXT_FNT_DISPLAY) gsgl_error ("gsgl_file_format_read_pstext", "token!=GSGL_FILE_PS_TEXT_FNT_DISPLAY");
	read_token (data, &token);
	ifntr = token;
	read_token (data, &token);
	if (token != GSGL_FILE_PS_TEXT_CHARACTER_HEIGHT) gsgl_error ("gsgl_file_format_read_pstext", "token!=GSGL_FILE_PS_TEXT_CHARACTER_HEIGHT");
	gsgl_file_format_read_float (data, &chh, read_token);
	read_token (data, &token);
	if (token != GSGL_FILE_PS_TEXT_CHARACTER_WIDTH) gsgl_error ("gsgl_file_format_read_pstext", "token!=GSGL_FILE_PS_TEXT_CHARACTER_WIDTH");
	gsgl_file_format_read_float (data, &chw, read_token);
	read_token (data, &token);
	if (token != GSGL_FILE_PS_TEXT_CHARACTER_SPACE) gsgl_error ("gsgl_file_format_read_pstext", "token!=GSGL_FILE_PS_TEXT_CHARACTER_SPACE");
	gsgl_file_format_read_float (data, &chsp, read_token);
	read_token (data, &token);
	if (token != GSGL_FILE_PS_TEXT_CHARACTER_ANGLE) gsgl_error ("gsgl_file_format_read_pstext", "token!=GSGL_FILE_PS_TEXT_CHARACTER_ANGLE");
	gsgl_file_format_read_float (data, &angle, read_token);
	read_token (data, &token);
	if (token != GSGL_FILE_PS_TEXT_CHARACTER_TILT) gsgl_error ("gsgl_file_format_read_pstext", "token!=GSGL_FILE_PS_TEXT_CHARACTER_TILT");
	gsgl_file_format_read_float (data, &tilt, read_token);
	read_token (data, &token);
	if (token != GSGL_FILE_PS_TEXT_X) gsgl_error ("gsgl_file_format_read_pstext", "token!=GSGL_FILE_PS_TEXT_X");
	gsgl_file_format_read_float (data, &x, read_token);
	read_token (data, &token);
	if (token != GSGL_FILE_PS_TEXT_Y) gsgl_error ("gsgl_file_format_read_pstext", "token!=GSGL_FILE_PS_TEXT_Y");
	gsgl_file_format_read_float (data, &y, read_token);
	read_token (data, &token);
	if (token != GSGL_FILE_PS_TEXT_DIM) gsgl_error ("gsgl_file_format_read_pstext", "token!=GSGL_FILE_PS_TEXT_DIM");
	read_token (data, &token);
	dim = token;
	read_token (data, &token);	
	if (token != GSGL_FILE_PS_TEXT_CHARACTER_NUM) gsgl_error ("gsgl_file_format_read_pstext", "token!=GSGL_FILE_PS_TEXT_CHARACTER_NUM");
	read_token (data, &token);
	nmax = token;
	iascs = (int *) gsgl_malloc_atomic (sizeof (int) * nmax);
	read_token (data, &token);
	if (token != GSGL_FILE_PS_TEXT_TEXT) gsgl_error ("gsgl_file_format_read_pstext", "token!=GSGL_FILE_PS_TEXT_TEXT");
	for (i=0; i<nmax; i++) {
		read_token (data, &token);
		iascs[i] = token;
		GSGL_DEBUG(fprintf(stderr, "%c", iascs[i]));
	}
	GSGL_DEBUG(fprintf(stderr, "\n"));

	/*printf ("f %d %d %f %f %f %f %f %f %f %d\n",
	  ifnts, ifntr, chh, chw, chsp, angle, tilt, x, y, nmax);*/

	if (ifntr >= 500) gsgl_usetype1 ();
 
	gsglsetfnt (ifnts, p);
	gsglsetcharsize (chh, chw, chsp, angle, tilt, p);
	text_unicode_without_analysis (iascs, nmax, &x, &y, dim, p);
}

itemdata * gsgl_file_format_read_gui_common (void *data, gsglPage *p, READ_TOKEN)
{
	unsigned int token;
	itemdata *data0 = gsgl_malloc (sizeof(itemdata));

	read_token (data, &token);
	if (token != GSGL_FILE_GUI_GEO) gsgl_error ("gsgl_file_format_read_gui_common", "token!=GSGL_FILE_GUI_GEO");
	gsgl_file_format_read_float (data, &data0->x, read_token);
	gsgl_file_format_read_float (data, &data0->y, read_token);
	gsgl_file_format_read_float (data, &data0->w, read_token);
	gsgl_file_format_read_float (data, &data0->h, read_token);

	read_token (data, &token);
	if (token != GSGL_FILE_GUI_FNT) gsgl_error ("gsgl_file_format_read_gui_common", "token!=GSGL_FILE_GUI_FNT");
	read_token (data, &token);
	data0->fnt = token;
	
	read_token (data, &token);
	if (token != GSGL_FILE_GUI_CHR) gsgl_error ("gsgl_file_format_read_gui_common", "token!=GSGL_FILE_GUI_CHR");
	gsgl_file_format_read_float (data, &data0->chh, read_token);
	gsgl_file_format_read_float (data, &data0->chw, read_token);
	gsgl_file_format_read_float (data, &data0->chsp, read_token);
	gsgl_file_format_read_float (data, &data0->angl, read_token);
	gsgl_file_format_read_float (data, &data0->tilt, read_token);

	data0->p = p;
	
	return data0;
}

guialldata * gsgl_file_format_read_gui_button_data (void *data, gsglPage *p, READ_TOKEN)
{
	unsigned int token, num, i;
	guialldata *data0;
	buttondata *s;

	data0 = (guialldata *) gsgl_malloc (sizeof(guialldata));
	data0->common = gsgl_file_format_read_gui_common (data, p, read_token);
	s = (buttondata *) gsgl_malloc (sizeof(buttondata));
	data0->specific = s;

	read_token (data, &token);
	if (token != GSGL_FILE_GUI_BUTTON_LABEL) gsgl_error ("gsgl_file_format_read_gui_button", "token!=GSGL_FILE_GUI_BUTTON_LABEL");
	read_token (data, &num);
	s->text = (char *) gsgl_malloc_atomic ((num+1)*sizeof(char));
	for (i=0; i<num; i++) {
		read_token (data, &token);
		s->text[i] = token;
	}
	s->text[num] = '\0';
	s->callback = NULL;

	read_token (data, &token);
	if (token != GSGL_FILE_GUI_BUTTON_END) gsgl_error ("gsgl_file_format_read_gui_button", "token!=GSGL_FILE_GUI_BUTTON_END");

	return data0;
}

void gsgl_file_format_read_gui_button (void *data, gsglPage *p, READ_TOKEN)
{
	gsgl_file_format_read_gui_button_data (data, p, read_token);
}

guialldata * gsgl_file_format_read_gui_combo_data (void *data, gsglPage *p, READ_TOKEN)
{
	unsigned int token, num, i, j, items;
	guialldata *data0;
	comboboxdata *s;

	data0 = (guialldata *) gsgl_malloc (sizeof(guialldata));
	data0->common = gsgl_file_format_read_gui_common (data, p, read_token);	
	s = (comboboxdata *) gsgl_malloc (sizeof(comboboxdata));
	data0->specific = s;

	read_token (data, &token);
	if (token != GSGL_FILE_GUI_COMBO_ITEMS) gsgl_error ("gsgl_file_format_read_gui_combo", "token!=GSGL_FILE_GUI_COMBO_ITEMS");
	read_token (data, &items);
	s->items = items;
	s->text = (char **) gsgl_malloc (sizeof(char *)*items);
	for (i=0; i<items; i++) {
		read_token (data, &num);
		s->text[i] = (char *) gsgl_malloc_atomic (sizeof(char)*(num+1));
		for (j=0; j<num; j++) {
			read_token (data, &token);
			s->text[i][j] = token;
		}
		s->text[i][num] = '\0';
	}

	read_token (data, &token);
	if (token != GSGL_FILE_GUI_COMBO_CURRENT_ITEM) gsgl_error ("gsgl_file_format_read_gui_combo", "token!=GSGL_FILE_GUI_COMBO_CURRENT_ITEM");
	read_token (data, &token);
	s->current = token;
	s->fortran = 0;
	
	read_token (data, &token);
	if (token != GSGL_FILE_GUI_COMBO_END) gsgl_error ("gsgl_file_format_read_gui_combo", "token!=GSGL_FILE_GUI_COMBO_END");

	return data0;
}

void gsgl_file_format_read_gui_combo (void *data, gsglPage *p, READ_TOKEN)
{
	gsgl_file_format_read_gui_combo_data (data, p, read_token);
}

guialldata * gsgl_file_format_read_gui_spinf_data (void *data, gsglPage *p, READ_TOKEN)
{
	unsigned int token;
	guialldata *data0;
	spinbuttondataf *s;

	data0 = (guialldata *) gsgl_malloc (sizeof(guialldata));
	data0->common = gsgl_file_format_read_gui_common (data, p, read_token);	
	s = (spinbuttondataf *) gsgl_malloc (sizeof(spinbuttondataf));
	data0->specific = s;

	read_token (data, &token);
	if (token != GSGL_FILE_GUI_SPINF_INIT) gsgl_error ("gsgl_file_format_read_gui_spinf", "token!=GSGL_FILE_GUI_SPIN_INIT");
	gsgl_file_format_read_float (data, &s->init, read_token);
	
	read_token (data, &token);
	if (token != GSGL_FILE_GUI_SPINF_LOWER) gsgl_error ("gsgl_file_format_read_gui_spinf", "token!=GSGL_FILE_GUI_SPIN_LOWER");
	gsgl_file_format_read_float (data, &s->lower, read_token);

	read_token (data, &token);
	if (token != GSGL_FILE_GUI_SPINF_UPPER) gsgl_error ("gsgl_file_format_read_gui_spinf", "token!=GSGL_FILE_GUI_SPIN_UPPER");
	gsgl_file_format_read_float (data, &s->upper, read_token);

	read_token (data, &token);
	if (token != GSGL_FILE_GUI_SPINF_DIGITS) gsgl_error ("gsgl_file_format_read_gui_spinf", "token!=GSGL_FILE_GUI_SPIN_DIGITS");
	read_token (data, &token);
	s->digits = token;

	read_token (data, &token);
	if (token != GSGL_FILE_GUI_SPINF_END) gsgl_error ("gsgl_file_format_read_gui_spinf", "token!=GSGL_FILE_GUI_SPIN_END");

	return data0;
}

guialldata * gsgl_file_format_read_gui_spini_data (void *data, gsglPage *p, READ_TOKEN)
{
	unsigned int token, token2;
	guialldata *data0;
	spinbuttondatai *s;

	data0 = (guialldata *) gsgl_malloc (sizeof(guialldata));
	data0->common = gsgl_file_format_read_gui_common (data, p, read_token);	
	s = (spinbuttondatai *) gsgl_malloc (sizeof(spinbuttondatai));
	data0->specific = s;

	read_token (data, &token);
	if (token != GSGL_FILE_GUI_SPINI_INIT) gsgl_error ("gsgl_file_format_read_gui_spini", "token!=GSGL_FILE_GUI_SPIN_INIT");
	read_token (data, &token);
	read_token (data, &token2);
	s->init = gsgl_convert_tokens_to_4byte (token, token2) - 2147483647;
	
	read_token (data, &token);
	if (token != GSGL_FILE_GUI_SPINI_LOWER) gsgl_error ("gsgl_file_format_read_gui_spini", "token!=GSGL_FILE_GUI_SPIN_LOWER");
	read_token (data, &token);
	read_token (data, &token2);
	s->lower = gsgl_convert_tokens_to_4byte (token, token2) - 2147483647;

	read_token (data, &token);
	if (token != GSGL_FILE_GUI_SPINI_UPPER) gsgl_error ("gsgl_file_format_read_gui_spini", "token!=GSGL_FILE_GUI_SPIN_UPPER");
	read_token (data, &token);
	read_token (data, &token2);
	s->upper = gsgl_convert_tokens_to_4byte (token, token2) - 2147483647;

	read_token (data, &token);
	if (token != GSGL_FILE_GUI_SPINI_END) gsgl_error ("gsgl_file_format_read_gui_spini", "token!=GSGL_FILE_GUI_SPIN_END");

	return data0;
}

void gsgl_file_format_read_gui_spinf (void *data, gsglPage *p, READ_TOKEN)
{
	gsgl_file_format_read_gui_spinf_data (data, p, read_token);
}

void gsgl_file_format_read_gui_spini (void *data, gsglPage *p, READ_TOKEN)
{
	gsgl_file_format_read_gui_spini_data (data, p, read_token);
}

guialldata * gsgl_file_format_read_gui_check_data (void *data, gsglPage *p, READ_TOKEN)
{
	unsigned int token, i, num;
	guialldata *data0;
	checkbuttondata *s;

	data0 = (guialldata *) gsgl_malloc (sizeof(guialldata));
	data0->common = gsgl_file_format_read_gui_common (data, p, read_token);	
	s = (checkbuttondata *) gsgl_malloc (sizeof(checkbuttondata));
	data0->specific = s;

	read_token (data, &token);
	if (token != GSGL_FILE_GUI_CHECK_LABEL) gsgl_error ("gsgl_file_format_read_gui_check", "token!=GSGL_FILE_GUI_CHECK_LABEL");
	read_token (data, &num);
	s->text = (char *) gsgl_malloc_atomic ((num+1)*sizeof(char));
	for (i=0; i<num; i++) {
		read_token (data, &token);
		s->text[i] = token;
	}
	s->text[num] = '\0';

	read_token (data, &token);
	if (token != GSGL_FILE_GUI_CHECK_INIT) gsgl_error ("gsgl_file_format_read_gui_check", "token!=GSGL_FILE_GUI_CHECK_INIT");
	read_token (data, &token);
	s->init = token;

	read_token (data, &token);
	if (token != GSGL_FILE_GUI_CHECK_END) gsgl_error ("gsgl_file_format_read_gui_check", "token!=GSGL_FILE_GUI_CHECK_END");

	return data0;
}

void gsgl_file_format_read_gui_check (void *data, gsglPage *p, READ_TOKEN)
{
	gsgl_file_format_read_gui_check_data (data, p, read_token);
}

guialldata * gsgl_file_format_read_gui_edit_data (void *data, gsglPage *p, READ_TOKEN)
{
	unsigned int token, i, num;
	guialldata *data0;
	lineeditdata *s;

	data0 = (guialldata *) gsgl_malloc (sizeof(guialldata));
	data0->common = gsgl_file_format_read_gui_common (data, p, read_token);	
	s = (lineeditdata *) gsgl_malloc (sizeof(lineeditdata));
	data0->specific = s;

	read_token (data, &token);
	if (token != GSGL_FILE_GUI_EDIT_MAX) gsgl_error ("gsgl_file_format_read_gui_edit", "token!=GSGL_FILE_GUI_EDIT_MAX");
	read_token (data, &token);
	s->max = token;

	read_token (data, &token);
	if (token != GSGL_FILE_GUI_EDIT_TEXT) gsgl_error ("gsgl_file_format_read_gui_edit", "token!=GSGL_FILE_GUI_EDIT_TEXT");
	read_token (data, &num);
	s->text = (char *) gsgl_malloc_atomic (sizeof(char)*(num+1));
	for (i=0; i<num; i++) {
		read_token (data, &token);
		s->text[i] = token;
	}
	s->text[num] = '\0';

	read_token (data, &token);
	if (token != GSGL_FILE_GUI_EDIT_END) gsgl_error ("gsgl_file_format_read_gui_edit", "token!=GSGL_FILE_GUI_EDIT_END");

	return data0;
}

void gsgl_file_format_read_gui_edit (void *data, gsglPage *p, READ_TOKEN)
{
	gsgl_file_format_read_gui_edit_data (data, p, read_token);
}

guialldata * gsgl_file_format_read_gui_label_data (void *data, gsglPage *p, READ_TOKEN)
{
	unsigned int token, i, num;
	guialldata *data0;
	labeldata *s;

	data0 = (guialldata *) gsgl_malloc (sizeof(guialldata));
	data0->common = gsgl_file_format_read_gui_common (data, p, read_token);	
	s = (labeldata *) gsgl_malloc (sizeof(labeldata));
	data0->specific = s;

	read_token (data, &token);
	if (token != GSGL_FILE_GUI_LABEL_TEXT) gsgl_error ("gsgl_file_format_read_gui_label", "token!=GSGL_FILE_GUI_LABEL_TEXT");
	read_token (data, &num);
	s->text = (char *) gsgl_malloc_atomic (sizeof(char)*(num+1));
	for (i=0; i<num; i++) {
		read_token (data, &token);
		s->text[i] = token;
	}
	s->text[num] = '\0';

	read_token (data, &token);
	if (token != GSGL_FILE_GUI_LABEL_JUST) gsgl_error ("gsgl_file_format_read_gui_label", "token!=GSGL_FILE_GUI_LABEL_JUST");
	read_token (data, &token);
	s->ijust = token;

	read_token (data, &token);
	if (token != GSGL_FILE_GUI_LABEL_END) gsgl_error ("gsgl_file_format_read_gui_label", "token!=GSGL_FILE_GUI_LABEL_END");

	return data0;
}

void gsgl_file_format_read_gui_label (void *data, gsglPage *p, READ_TOKEN)
{
	gsgl_file_format_read_gui_label_data (data, p, read_token);
}

void gsgl_file_format_read_gui_external (void *data, gsglPage *p, READ_TOKEN)
{
	unsigned int token;

	read_token (data, &token);
	gsaf_guiexternal (token);
}

void gsgl_file_format_read_gui_external_ignore (void *data, gsglPage *p, READ_TOKEN)
{
	unsigned int token;

	read_token (data, &token);
}

