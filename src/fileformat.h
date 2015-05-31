#ifndef __fileformat_h__
#define __fileformat_h__

#include "guiitems.h"

#define   GSGL_FILE_BEGIN_PAGEINFO 1
#define     GSGL_FILE_PAGE_NUMBER    1
#define     GSGL_FILE_PAGE_SIZE      2
#define   GSGL_FILE_END_PAGEINFO   2
#define   GSGL_FILE_BEGIN_HEAD     3
#define   GSGL_FILE_END_HEAD       4
#define   GSGL_FILE_BEGIN_PAGE     5
#define   GSGL_FILE_END_PAGE       6
#define   GSGL_FILE_BEGIN_FFORMAT  7 /* obsolute since 0.3.50 */
#define   FFORMAT_TYPE            1  /* followed by 2(2D) or 3(3D) */
#define   GSGL_FILE_END_FFORMAT    8 /* obsolute since 0.3.50 */
#define   GSGL_FILE_BEGIN_OBJECT   9 /* obsolute since 0.3.50 */ 
#define   GSGL_FILE_END_OBJECT    10 /* obsolute since 0.3.50 */ 
#define     GSGL_FILE_END_OBJECT_MODELVIEW_MATRIX   20001
#define   GSGL_FILE_BEGIN_ANIME   11
#define   GSGL_FILE_END_ANIME     12
#define   GSGL_FILE_MODE_NORMAL   13 /* added in 0.3.50 */
#define   GSGL_FILE_MODE_2D       14 /* added in 0.3.50 */
#define   GSGL_FILE_MODE_3D       15 /* added in 0.3.50 */
#define   GSGL_FILE_SEPARATE_OBJECTS 16 /* added in 0.3.50 */

#define   GSGL_FILE_BEGIN_POINT     101
#define   GSGL_FILE_END_POINT       102
#define   GSGL_FILE_BEGIN_LINE      103
#define   GSGL_FILE_END_LINE        104
#define   GSGL_FILE_BEGIN_POLY      105
#define   GSGL_FILE_END_POLY        106
#define   GSGL_FILE_RGB             107
#define   GSGL_FILE_RGBA            108
#define   GSGL_FILE_VERTEX2         109
#define   GSGL_FILE_VERTEX3         110
#define   GSGL_FILE_VERTEX2_PRJCTD  111 /* obsolute since 0.3.50 */
#define   GSGL_FILE_VERTEX3_PRJCTD  112 /* obsolute since 0.3.50 */
#define   GSGL_FILE_ARRAY_ELEMENT_4BYTE 113
#define   GSGL_FILE_CLIP_3D_FILEONLY  114
#define     GSGL_FILE_CLIP_NUM          1
#define     GSGL_FILE_CLIP_COEFF        2
#define   GSGL_FILE_DEL_CLIP_PLANE  115
#define     GSGL_FILE_DEL_CLIP_NUM      1
#define   GSGL_FILE_BEGIN_TRIANGLES 116 /* added in 0.3.50 */
#define   GSGL_FILE_END_TRIANGLES   117 /* added in 0.3.50 */
#define   GSGL_FILE_BEGIN_TRIANGLE_STRIP 118 /* added in 0.3.50 */
#define   GSGL_FILE_END_TRIANGLE_STRIP   119 /* added in 0.3.50 */
#define   GSGL_FILE_BEGIN_TRIANGLE_FAN   120 /* added in 0.3.50 */
#define   GSGL_FILE_END_TRIANGLE_FAN     121 /* added in 0.3.50 */
#define   GSGL_FILE_CLIP_UNMOVABLE  122 /* added in 0.3.50 */
#define     GSGL_FILE_CLIP_NUM          1
#define     GSGL_FILE_CLIP_COEFF        2
#define   GSGL_FILE_NORMAL          123 /* added in 0.3.50 */ /* followed by 3 floats */

#define   GSGL_FILE_LINEPATTERN_0  201 /* obsolute since 0.3.50 */
#define   GSGL_FILE_LINEPATTERN_1  202 /* obsolute since 0.3.50 */
#define   GSGL_FILE_LINEPATTERN_2  203 /* obsolute since 0.3.50 */
#define   GSGL_FILE_LINEPATTERN_3  204 /* obsolute since 0.3.50 */
#define   GSGL_FILE_LINEPATTERN_4  205 /* obsolute since 0.3.50 */
#define   GSGL_FILE_LINEPATTERN_5  206 /* obsolute since 0.3.50 */
#define   GSGL_FILE_LINEPATTERN_6  207 /* obsolute since 0.3.50 */
#define   GSGL_FILE_LINEPATTERN_7  208 /* obsolute since 0.3.50 */
#define   GSGL_FILE_LINEWIDTH      209 /* width (cm) */
#define   GSGL_FILE_LINEWIDTH2     210 /* pixel in X, 0.5 point in ps */
#define   GSGL_FILE_INDEXCOLOR     211
#define   GSGL_FILE_PROJECTIONTYPE 212 /* added in 0.3.7 */
#define     GSGL_FILE_PROJ_ORTHOGONAL      0 /* added in 0.3.7 */
#define     GSGL_FILE_PROJ_PERSPECTIVE     1 /* added in 0.3.7 */
#define   GSGL_FILE_BEGIN_RGBTRG   213
#define   GSGL_FILE_END_RGBTRG     214

#define   GSGL_FILE_3D_3                   297 /* 3D object's info */ /* added in 0.3.7 */
#define     GSGL_FILE_3D_DEPTH                 1
#define   GSGL_FILE_3D_2                   298  /* obsolute since 0.3.7 */
#define     GSGL_FILE_3D_DEPTH                 1
#define     GSGL_FILE_3D_PROJ                  2
#define   GSGL_FILE_PROJECTION             299 /* followed by 16 components */
#define   GSGL_FILE_MODELVIEW              300 /* followed by 16 components */
#define   GSGL_FILE_3D                     301 /* obsolute */
#define   GSGL_FILE_ENABLE_DEPTHTEST       302 /* obsolute since 0.3.50 */
#define   GSGL_FILE_DISABLE_DEPTHTEST      303 /* obsolute since 0.3.50 */
#define   GSGL_FILE_ENABLE_POLYGONOFFSET   304 /* obsolute since 0.3.50 */
#define   GSGL_FILE_DISABLE_POLYGONOFFSET  305 /* obsolute since 0.3.50 */
#define   GSGL_FILE_ENABLE_LIGHTING        306
#define     GSGL_FILE_LIGHTING_ENABLE_TYPE      1 /* 1 : default */
#define     GSGL_FILE_LIGHTING_ENABLE_NUM       2 /* from 0 */
#define   GSGL_FILE_DISABLE_LIGHTING       307
#define     GSGL_FILE_LIGHTING_DISABLE_NUM      1 /* from 0 */
#define   GSGL_FILE_NORMAL_NO_SPECIFY      308
#define   GSGL_FILE_NORMAL_BY_SCAL_PROD    309

#define   GSGL_FILE_PS_BEGIN_GOURAUD_ONLY   501
#define   GSGL_FILE_PS_END_GOURAUD_ONLY     502
#define   GSGL_FILE_PS_DEPTH_BY_DEEP        503
#define   GSGL_FILE_PS_DEPTH_BY_SHALLOW     504
#define   GSGL_FILE_PS_DEPTH_BY_MEAN        505
#define   GSGL_FILE_PS_DEPTH_BY_VECT_PROD_OLD   506 /*3つの点 A,B,C に対して
						    (AB x BC)z > 0 なら DEEP,
						    < 0 なら SHALLOW,
						    = 0 なら MEAN. */
#define   GSGL_FILE_PS_DEPTH_BY_POINT_OLD       508
#define   GSGL_FILE_PS_DEPTH_BY_VECT_PROD   509 /*3つの点 A,B,C に対して
						    (AB x BC)z > 0 なら DEEP,
						    < 0 なら SHALLOW,
						    = 0 なら MEAN. */
#define   GSGL_FILE_PS_DEPTH_BY_VECT_PROD2  510 /*3つの点 A,B,C に対して
						    (AB x BC)z > 0 なら 1つ目,
						    < 0 なら 2つ目,
						    = 0 なら 3つ目の点の深さ. */
#define   GSGL_FILE_PS_DEPTH_BY_POINT       511
#define   GSGL_FILE_PS_TEXT                 512 /* added in 0.3.68 */
#define   GSGL_FILE_PS_TEXT_FNT             513 /* added in 0.3.68 */
#define   GSGL_FILE_PS_TEXT_FNT_DISPLAY     514 /* added in 0.3.68 */
#define   GSGL_FILE_PS_TEXT_CHARACTER_HEIGHT 515 /* added in 0.3.68 */
#define   GSGL_FILE_PS_TEXT_CHARACTER_WIDTH 516 /* added in 0.3.68 */
#define   GSGL_FILE_PS_TEXT_CHARACTER_SPACE 517 /* added in 0.3.68 */
#define   GSGL_FILE_PS_TEXT_CHARACTER_ANGLE 518 /* added in 0.3.68 */
#define   GSGL_FILE_PS_TEXT_CHARACTER_TILT  519 /* added in 0.3.68 */
#define   GSGL_FILE_PS_TEXT_X               520 /* added in 0.3.68 */
#define   GSGL_FILE_PS_TEXT_Y               521 /* added in 0.3.68 */
#define   GSGL_FILE_PS_TEXT_CHARACTER_NUM   522 /* added in 0.3.68 */
#define   GSGL_FILE_PS_TEXT_TEXT            523 /* added in 0.3.68 */
#define   GSGL_FILE_PS_TEXT_DIM             524 /* added in 0.3.68 */
#define   GSGL_FILE_PS_TEXT_END             525 /* added in 0.3.68 */
#define   GSGL_FILE_PS_TEXT_POSITION        526 /* added in 0.4.13 */
#define   GSGL_TCP_BITMAP_FONT            10000 /* added in 0.4.13 */

#define   GSGL_FILE_ENABLE_CLIENT_VERTEX    1000
#define     GSGL_FILE_CLIENT_VERTEX_TYPE       1
#define     GSGL_FILE_CLIENT_VERTEX_NX_2BYTE  11 /* obsolute since 0.3.51 */
#define     GSGL_FILE_CLIENT_VERTEX_NY_2BYTE  12 /* obsolute since 0.3.51 */
#define     GSGL_FILE_CLIENT_VERTEX_XMIN      13
#define     GSGL_FILE_CLIENT_VERTEX_XMAX      14
#define     GSGL_FILE_CLIENT_VERTEX_YMIN      15
#define     GSGL_FILE_CLIENT_VERTEX_YMAX      16
#define     GSGL_FILE_CLIENT_VERTEX_SIZE       2
#define     GSGL_FILE_CLIENT_VERTEX_TOTALSIZE_2BYTE  3 /* obsolute since 0.3.51 */
#define   GSGL_FILE_ENABLE_CLIENT_COLOR     1001
#define     GSGL_FILE_CLIENT_COLOR_SIZE        1
#define     GSGL_FILE_CLIENT_COLOR_TOTALSIZE_2BYTE   2 /* obsolute since 0.3.51 */
/*#define   GSGL_FILE_ENABLE_CLIENT_NORMAL    1002
  #define   CLIENT_NORMAL_TOTALSIZE              2*/
#define   GSGL_FILE_ARRAY_BEGIN_POLYGON     1003 /* added in 0.3.50 */
#define   GSGL_FILE_ARRAY_END_POLYGON       1004 /* added in 0.3.50 */
#define   GSGL_FILE_ARRAY_BEGIN_LINE        1005 /* added in 0.3.50 */
#define   GSGL_FILE_ARRAY_END_LINE          1006 /* added in 0.3.50 */
#define   GSGL_FILE_CLIENT_VERTEX_TOTALSIZE_4BYTE  1007 /* added in 0.3.51 */
#define   GSGL_FILE_CLIENT_COLOR_TOTALSIZE_4BYTE   1008 /* added in 0.3.51 */
#define   GSGL_FILE_CLIENT_FEM_IELM_4BYTE          1009 /* added in 0.3.51 */
#define   GSGL_FILE_CLIENT_VERTEX_NX_4BYTE         1010 /* added in 0.3.51 */
#define   GSGL_FILE_CLIENT_VERTEX_NY_4BYTE         1011 /* added in 0.3.51 */

#define   GSGL_FILE_GUI_GEO                 2001 /* added in 0.4.10 */
#define   GSGL_FILE_GUI_FNT                 2002 /* added in 0.4.10 */
#define   GSGL_FILE_GUI_CHR                 2003 /* added in 0.4.10 */
#define   GSGL_FILE_GUI_SET_EXTERNAL        2004 /* added in 0.4.10 */
#define   GSGL_FILE_GUI_BUTTON_BEGIN        2010 /* added in 0.4.10 */
#define   GSGL_FILE_GUI_BUTTON_LABEL        2011 /* added in 0.4.10 */
#define   GSGL_FILE_GUI_BUTTON_END          2012 /* added in 0.4.10 */
#define   GSGL_FILE_GUI_COMBO_BEGIN         2013 /* added in 0.4.10 */
#define   GSGL_FILE_GUI_COMBO_ITEMS         2014 /* added in 0.4.10 */
#define   GSGL_FILE_GUI_COMBO_CURRENT_ITEM  2015 /* added in 0.4.10 */
#define   GSGL_FILE_GUI_COMBO_END           2016 /* added in 0.4.10 */
#define   GSGL_FILE_GUI_SPINF_BEGIN          2017 /* added in 0.4.10 */
#define   GSGL_FILE_GUI_SPINF_INIT           2018 /* added in 0.4.10 */
#define   GSGL_FILE_GUI_SPINF_LOWER          2019 /* added in 0.4.10 */
#define   GSGL_FILE_GUI_SPINF_UPPER          2020 /* added in 0.4.10 */
#define   GSGL_FILE_GUI_SPINF_DIGITS         2021 /* added in 0.4.10 */
#define   GSGL_FILE_GUI_SPINF_END            2022 /* added in 0.4.10 */
#define   GSGL_FILE_GUI_CHECK_BEGIN         2023 /* added in 0.4.10 */
#define   GSGL_FILE_GUI_CHECK_LABEL         2024 /* added in 0.4.10 */
#define   GSGL_FILE_GUI_CHECK_INIT          2025 /* added in 0.4.10 */
#define   GSGL_FILE_GUI_CHECK_END           2026 /* added in 0.4.10 */
#define   GSGL_FILE_GUI_EDIT_BEGIN          2027 /* added in 0.4.10 */
#define   GSGL_FILE_GUI_EDIT_MAX            2028 /* added in 0.4.10 */
#define   GSGL_FILE_GUI_EDIT_TEXT           2029 /* added in 0.4.10 */
#define   GSGL_FILE_GUI_EDIT_END            2030 /* added in 0.4.10 */
#define   GSGL_FILE_GUI_LABEL_BEGIN         2031 /* added in 0.4.10 */
#define   GSGL_FILE_GUI_LABEL_TEXT          2032 /* added in 0.4.10 */
#define   GSGL_FILE_GUI_LABEL_JUST          2033 /* added in 0.4.10 */
#define   GSGL_FILE_GUI_LABEL_END           2034 /* added in 0.4.10 */
#define   GSGL_FILE_GUI_SPINI_BEGIN         2035 /* added in 0.4.10 */
#define   GSGL_FILE_GUI_SPINI_INIT          2036 /* added in 0.4.10 */
#define   GSGL_FILE_GUI_SPINI_LOWER         2037 /* added in 0.4.10 */
#define   GSGL_FILE_GUI_SPINI_UPPER         2038 /* added in 0.4.10 */
#define   GSGL_FILE_GUI_SPINI_END           2039 /* added in 0.4.10 */

#define   GSGL_TCP_OPEN                    30001 /* added in 0.3.51 */
#define   GSGL_TCP_CLOSE                   30002 /* added in 0.3.51 */
#define   GSGL_TCP_EXIT_FROM_PAGEE         30003 /* added in 0.3.51 */
#define   GSGL_TCP_QUIT                    30004 /* added in 0.3.51 */
#define   GSGL_TCP_PING                    30005 /* added in 0.3.51 */
#define   GSGL_TCP_REMADE                 30006 /* added in 0.3.51 */
#define   GSGL_TCP_REMADE_INFO            30007 /* added in 0.3.51 */
#define   GSGL_TCP_INTERUPTED              30008 /* added in 0.3.61 */

#define   GSGL_TCP_GUI_SAVE_AND_CLOSE      30105 /* added in 0.3.51 */
#define   GSGL_TCP_GUI_SAVE_AS             30106 /* added in 0.3.51 */
#define   GSGL_TCP_GUI_SAVE                30107 /* added in 0.3.51 */
#define   GSGL_TCP_GUI_EXPORT_VEC_EPS      30108 /* added in 0.3.51 */
#define   GSGL_TCP_GUI_EXPORT_RAS_EPS      30109 /* added in 0.3.51 */
#define   GSGL_TCP_GUI_EXPORT_PNG          30110 /* added in 0.3.51 */
#define   GSGL_TCP_GUI_EXPORT_JPEG         30111 /* added in 0.3.51 */
#define   GSGL_TCP_GUI_PRINT               30112 /* added in 0.3.51 */
#define   GSGL_TCP_GUI_DUMP                30113 /* added in 0.3.51 */
#define   GSGL_TCP_GUI_CONFIG_COLOR        30114 /* added in 0.3.51 */
#define   GSGL_TCP_GUI_CONFIG_COMMAND      30115 /* added in 0.3.51 */
#define   GSGL_TCP_GUI_CONFIG_FONT         30116 /* added in 0.3.51 */
#define   GSGL_TCP_GUI_CONFIG_WINDOW       30117 /* added in 0.3.51 */
#define   GSGL_TCP_NTH_FRAME               30118 /* added in 0.3.66 */
#define   GSGL_TCP_GUI_SAVE_THIS_FRAME     30119 /* added in 0.3.66 */
#define   GSGL_TCP_GUI_BEGIN_OBJECTS       30120 /* added in 0.4.10 */
#define   GSGL_TCP_GUI_END_OBJECTS         30121 /* added in 0.4.10 */
#define   GSGL_TCP_GUI_CLEAR_LIST          30122 /* added in 0.4.10 */
#define   GSGL_TCP_GUI_BUTTON_CLICKED      30123 /* added in 0.4.10 */
#define   GSGL_TCP_GUI_COMBO_CHANGED       30124 /* added in 0.4.10 */
#define   GSGL_TCP_GUI_COMBO_ITEM          30125 /* added in 0.4.10 */
#define   GSGL_TCP_GUI_SPINF_VALUECHANGED  30126 /* added in 0.4.10 */
#define   GSGL_TCP_GUI_SPINF_VALUE         30127 /* added in 0.4.10 */
#define   GSGL_TCP_GUI_CHECK_TOGGLED       30128 /* added in 0.4.10 */
#define   GSGL_TCP_GUI_CHECK_ACTIVE        30129 /* added in 0.4.10 */
#define   GSGL_TCP_GUI_EDIT_CHANGED        30130 /* added in 0.4.10 */
#define   GSGL_TCP_GUI_EDIT_TEXT           30131 /* added in 0.4.10 */
#define   GSGL_TCP_GUI_SPINI_VALUECHANGED  30132 /* added in 0.4.10 */
#define   GSGL_TCP_GUI_SPINI_VALUE         30133 /* added in 0.4.10 */
#define   GSGL_TCP_GUI_ADD_TEXT            30134 /* added in 0.4.16 */

#define READ_TOKEN void (*read_token)(void *, unsigned int*)

gsglPage * gsgl_file_format_read_template (
	gsglPage *p,
	void *user_data,
	READ_TOKEN,
	unsigned int (* func_inq_token)(void *, gsglPage *, READ_TOKEN),
	void (*func_vertex2)(void *, gsglPage *, READ_TOKEN),
	void (*func_vertex3)(void *, gsglPage *, READ_TOKEN),
	void (*func_mode_normal)(void *, gsglPage *, READ_TOKEN),
	void (*func_mode_2d)(void *, gsglPage *, READ_TOKEN),
	void (*func_mode_3d)(void *, gsglPage *, READ_TOKEN),
	void (*func_separate_objects)(void *, gsglPage *, READ_TOKEN),
	void (*func_array_begin_polygon)(void *, gsglPage *, READ_TOKEN),
	void (*func_array_end_polygon)(void *, gsglPage *, READ_TOKEN),
	void (*func_array_begin_line)(void *, gsglPage *, READ_TOKEN),
	void (*func_array_end_line)(void *, gsglPage *, READ_TOKEN),
	void (*func_begin_point)(void *, gsglPage *, READ_TOKEN),
	void (*func_end_point)(void *, gsglPage *, READ_TOKEN),
	void (*func_begin_line)(void *, gsglPage *, READ_TOKEN),
	void (*func_end_line)(void *, gsglPage *, READ_TOKEN),
	void (*func_begin_poly)(void *, gsglPage *, READ_TOKEN),
	void (*func_end_poly)(void *, gsglPage *, READ_TOKEN),
	void (*func_normal)(void *, gsglPage *, READ_TOKEN),
	void (*func_begin_triangles)(void *, gsglPage *, READ_TOKEN),
	void (*func_end_triangles)(void *, gsglPage *, READ_TOKEN),
	void (*func_begin_triangle_strip)(void *, gsglPage *, READ_TOKEN),
	void (*func_end_triangle_strip)(void *, gsglPage *, READ_TOKEN),
	void (*func_begin_triangle_fan)(void *, gsglPage *, READ_TOKEN),
	void (*func_end_triangle_fan)(void *, gsglPage *, READ_TOKEN),
	void (*func_clip_3d_fileonly)(void *, gsglPage *, READ_TOKEN),
	void (*func_clip_unmovable)(void *, gsglPage *, READ_TOKEN),
	void (*func_del_clip)(void *, gsglPage *, READ_TOKEN),
	void (*func_modelview)(void *, gsglPage *, READ_TOKEN),
	void (*func_rgb)(void *, gsglPage *, READ_TOKEN),
	void (*func_array_element_4byte)(void *, gsglPage *, READ_TOKEN),
	void (*func_rgba)(void *, gsglPage *, READ_TOKEN),
	void (*func_indexcolor)(void *, gsglPage *, READ_TOKEN),
	void (*func_linewidth)(void *, gsglPage *, READ_TOKEN),
	void (*func_linewidth2)(void *, gsglPage *, READ_TOKEN),
	void (*func_projection)(void *, gsglPage *, READ_TOKEN),
	void (*func_3d_3)(void *, gsglPage *, READ_TOKEN),
	void (*func_projection_type)(void *, gsglPage *, READ_TOKEN),
	void (*func_enable_lighting)(void *, gsglPage *, READ_TOKEN),
	void (*func_disable_lighting)(void *, gsglPage *, READ_TOKEN),
	void (*func_begin_pageinfo)(void *, gsglPage *, READ_TOKEN),
	void (*func_end_pageinfo)(void *, gsglPage *, READ_TOKEN),
	void (*func_begin_head)(void *, gsglPage *, READ_TOKEN),
	void (*func_end_head)(void *, gsglPage *, READ_TOKEN),
	void (*func_begin_page)(void *, gsglPage *, READ_TOKEN),
	void (*func_end_page)(void *, gsglPage *, READ_TOKEN),
	void (*func_begin_anime)(void *, gsglPage *, READ_TOKEN),
	void (*func_end_anime)(void *, gsglPage *, READ_TOKEN),
	void (*func_enable_client_vertex)(void *, gsglPage *, READ_TOKEN),
	void (*func_enable_client_color)(void *, gsglPage *, READ_TOKEN),
	void (*func_begin_gouraud_only)(void *, gsglPage *, READ_TOKEN),
	void (*func_end_gouraud_only)(void *, gsglPage *, READ_TOKEN),
	void (*func_depth_by_deep)(void *, gsglPage *, READ_TOKEN),
	void (*func_depth_by_shallow)(void *, gsglPage *, READ_TOKEN),
	void (*func_depth_by_mean)(void *, gsglPage *, READ_TOKEN),
	void (*func_depth_by_point)(void *, gsglPage *, READ_TOKEN),
	void (*func_depth_by_vec_prod)(void *, gsglPage *, READ_TOKEN),
	void (*func_depth_by_vec_prod2)(void *, gsglPage *, READ_TOKEN),
	void (*func_vertex2_projctd)(void *, gsglPage *, READ_TOKEN),
	void (*func_vertex3_projctd)(void *, gsglPage *, READ_TOKEN),
	void (*func_3d_2)(void *, gsglPage *, READ_TOKEN),
	void (*func_begin_object)(void *, gsglPage *, READ_TOKEN),
	void (*func_end_object)(void *, gsglPage *, READ_TOKEN),
	void (*func_normal_no_specify)(void *, gsglPage *, READ_TOKEN),
	void (*func_normal_by_scal_prod)(void *, gsglPage *, READ_TOKEN),
	void (*func_depth_by_vect_prod_old)(void *, gsglPage *, READ_TOKEN),
	void (*func_depth_by_point_old)(void *, gsglPage *, READ_TOKEN),
	void (*func_begin_fformat)(void *, gsglPage *, READ_TOKEN),
	void (*func_end_fformat)(void *, gsglPage *, READ_TOKEN),
	void (*func_3d)(void *, gsglPage *, READ_TOKEN),
	void (*func_line_pattern_0)(void *, gsglPage *, READ_TOKEN),
	void (*func_line_pattern_1)(void *, gsglPage *, READ_TOKEN),
	void (*func_line_pattern_2)(void *, gsglPage *, READ_TOKEN),
	void (*func_line_pattern_3)(void *, gsglPage *, READ_TOKEN),
	void (*func_line_pattern_4)(void *, gsglPage *, READ_TOKEN),
	void (*func_line_pattern_5)(void *, gsglPage *, READ_TOKEN),
	void (*func_line_pattern_6)(void *, gsglPage *, READ_TOKEN),
	void (*func_line_pattern_7)(void *, gsglPage *, READ_TOKEN),
	void (*func_enable_depth_test)(void *, gsglPage *, READ_TOKEN),
	void (*func_disable_depth_test)(void *, gsglPage *, READ_TOKEN),
	void (*func_enable_polygonoffset)(void *, gsglPage *, READ_TOKEN),
	void (*func_disable_polygonoffset)(void *, gsglPage *, READ_TOKEN),
	void (*func_pstext)(void *, gsglPage *, READ_TOKEN),
	void (*func_pstext_end)(void *, gsglPage *, READ_TOKEN),
	void (*func_gui_button)(void *, gsglPage *, READ_TOKEN),
	void (*func_gui_combo)(void *, gsglPage *, READ_TOKEN),
	void (*func_gui_spinf)(void *, gsglPage *, READ_TOKEN),
	void (*func_gui_spini)(void *, gsglPage *, READ_TOKEN),
	void (*func_gui_check)(void *, gsglPage *, READ_TOKEN),
	void (*func_gui_edit)(void *, gsglPage *, READ_TOKEN),
	void (*func_gui_label)(void *, gsglPage *, READ_TOKEN),
	void (*func_gui_external)(void *, gsglPage *, READ_TOKEN),
	void (*func_default)(unsigned int, void *, gsglPage *, READ_TOKEN));

unsigned int gsgl_file_format_read_vertex_template (void *data, gsglPage *, int datatype,
						    gsglboolean allocate,
						    READ_TOKEN,
						    void (*func_float)(void *, float *, READ_TOKEN));

unsigned int gsgl_file_format_read_inq_token (void *data, gsglPage *p, READ_TOKEN);
void gsgl_file_format_read_float (void *data, float *x, READ_TOKEN);
void gsgl_file_format_read_float_754 (void *data, float *x, READ_TOKEN);
void gsgl_file_format_read_mode_normal (void *data, gsglPage *, READ_TOKEN);
void gsgl_file_format_read_mode_2d (void *data, gsglPage *, READ_TOKEN);
void gsgl_file_format_read_mode_3d (void *data, gsglPage *, READ_TOKEN);
void gsgl_file_format_read_separate_objects (void *data, gsglPage *, READ_TOKEN);
void gsgl_file_format_read_array_begin_polygon (void *data, gsglPage *, READ_TOKEN);
void gsgl_file_format_read_array_end_polygon (void *data, gsglPage *, READ_TOKEN);
void gsgl_file_format_read_array_begin_line (void *data, gsglPage *, READ_TOKEN);
void gsgl_file_format_read_array_end_line (void *data, gsglPage *, READ_TOKEN);
void gsgl_file_format_read_array_element_4byte (void *data, gsglPage *, READ_TOKEN);
void gsgl_file_format_read_vertex2 (void *data, gsglPage *, READ_TOKEN);
void gsgl_file_format_read_begin_triangles (void *data, gsglPage *, READ_TOKEN);
void gsgl_file_format_read_end_triangles (void *data, gsglPage *, READ_TOKEN);
void gsgl_file_format_read_begin_triangle_strip (void *data, gsglPage *, READ_TOKEN);
void gsgl_file_format_read_end_triangle_strip (void *data, gsglPage *, READ_TOKEN);
void gsgl_file_format_read_begin_triangle_fan (void *data, gsglPage *, READ_TOKEN);
void gsgl_file_format_read_end_triangle_fan (void *data, gsglPage *, READ_TOKEN);
void gsgl_file_format_read_clip_3d_fileonly (void *data, gsglPage *, READ_TOKEN);
void gsgl_file_format_read_clip_unmovable (void *data, gsglPage *, READ_TOKEN);
void gsgl_file_format_read_del_clip (void *data, gsglPage *, READ_TOKEN);
void gsgl_file_format_read_modelview (void *data, gsglPage *, READ_TOKEN);
void gsgl_file_format_read_rgb (void *data, gsglPage *, READ_TOKEN);
void gsgl_file_format_read_rgba (void *data, gsglPage *, READ_TOKEN);
void gsgl_file_format_read_indexcolor (void *data, gsglPage *, READ_TOKEN);
void gsgl_file_format_read_linewidth (void *data, gsglPage *, READ_TOKEN);
void gsgl_file_format_read_linewidth2 (void *data, gsglPage *, READ_TOKEN);
void gsgl_file_format_read_projection (void *data, gsglPage *, READ_TOKEN);
void gsgl_file_format_read_3d_3 (void *data, gsglPage *, READ_TOKEN);
void gsgl_file_format_read_projection_type (void *data, gsglPage *, READ_TOKEN);
void gsgl_file_format_read_enable_lighting (void *data, gsglPage *, READ_TOKEN);
void gsgl_file_format_read_disable_lighting (void *data, gsglPage *, READ_TOKEN);
void gsgl_file_format_read_enable_client_color (void *data, gsglPage *, READ_TOKEN);
void gsgl_file_format_read_begin_gouraud_only (void *data, gsglPage *, READ_TOKEN);
void gsgl_file_format_read_end_gouraud_only (void *data, gsglPage *, READ_TOKEN);
void gsgl_file_format_read_depth_by_deep (void *data, gsglPage *, READ_TOKEN);
void gsgl_file_format_read_depth_by_shallow (void *data, gsglPage *, READ_TOKEN);
void gsgl_file_format_read_depth_by_mean (void *data, gsglPage *, READ_TOKEN);
void gsgl_file_format_read_depth_by_point (void *data, gsglPage *, READ_TOKEN);
void gsgl_file_format_read_depth_by_vec_prod (void *data, gsglPage *, READ_TOKEN);
void gsgl_file_format_read_depth_by_vec_prod2 (void *data, gsglPage *, READ_TOKEN);
void gsgl_file_format_read_begin_anime (void *data, gsglPage *, READ_TOKEN);
void gsgl_file_format_read_end_anime (void *data, gsglPage *, READ_TOKEN);
void gsgl_file_format_read_begin_point (void *data1, gsglPage *, READ_TOKEN);
void gsgl_file_format_read_end_point (void *data1, gsglPage *, READ_TOKEN);
void gsgl_file_format_read_do_nothing (void *data, gsglPage *, READ_TOKEN);
void gsgl_file_format_read_pstext (void *data, gsglPage *, READ_TOKEN);
itemdata * gsgl_file_format_read_gui_common (void *data, gsglPage *, READ_TOKEN);
guialldata * gsgl_file_format_read_gui_button_data (void *data, gsglPage *, READ_TOKEN);
guialldata * gsgl_file_format_read_gui_combo_data (void *data, gsglPage *, READ_TOKEN);
guialldata * gsgl_file_format_read_gui_spinf_data (void *data, gsglPage *, READ_TOKEN);
guialldata * gsgl_file_format_read_gui_spini_data (void *data, gsglPage *, READ_TOKEN);
guialldata * gsgl_file_format_read_gui_check_data (void *data, gsglPage *, READ_TOKEN);
guialldata * gsgl_file_format_read_gui_edit_data (void *data, gsglPage *, READ_TOKEN);
guialldata * gsgl_file_format_read_gui_label_data (void *data, gsglPage *, READ_TOKEN);
void gsgl_file_format_read_gui_button (void *data, gsglPage *, READ_TOKEN);
void gsgl_file_format_read_gui_combo (void *data, gsglPage *, READ_TOKEN);
void gsgl_file_format_read_gui_spinf (void *data, gsglPage *, READ_TOKEN);
void gsgl_file_format_read_gui_spini (void *data, gsglPage *, READ_TOKEN);
void gsgl_file_format_read_gui_check (void *data, gsglPage *, READ_TOKEN);
void gsgl_file_format_read_gui_edit (void *data, gsglPage *, READ_TOKEN);
void gsgl_file_format_read_gui_label (void *data, gsglPage *, READ_TOKEN);
void gsgl_file_format_read_gui_external (void *data, gsglPage *, READ_TOKEN);
void gsgl_file_format_read_gui_external_ignore (void *data, gsglPage *, READ_TOKEN);

#endif
