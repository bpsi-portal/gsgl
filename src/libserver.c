#include "gsaf.h"
#include "gsgl.h"
#include "utils.h"
#include "fileformat.h"
#include "libserver.h"
#include "guiitems.h"
#include "base.h"
#include "drv_wrapper.h"
#include "displaylist.h"
#include "matrix.h"
#include "clientstate.h"
#include "tcpip.h"
#include "configfile.h"
#include "bsc1.h"
#include "text.h"
#include "textin.h"
#include "text_texture.h"

static void read_vertex (server_data *data, gsglPage *, int datatype);

#define GSGL_READ  (0)
#define GSGL_WRITE (1)
#define GSGL_DEBUG(x)

int gsgl_server_read_token1 (int connected_socket, unsigned int *token)
{
	unsigned char buf[2];
	int read_size;

	if ((read_size = read (connected_socket, buf, 2)) != 2) {
		*token = 0;
	} else {
		*token = buf[0]*256 + buf[1];
	}
	return read_size;
}

static void read_token_tcp (void *data1, unsigned int *token)
{
	server_data *data = data1;
	gsgl_server_read_token1 (data->socket, token);
}

static void write_token (int connected_socket, unsigned int token)
{
	unsigned char buf[2];
	int write_size;

	buf[0] = token / 256;
	buf[1] = token - buf[0] * 256;

	if ((write_size = write (connected_socket, buf, 2)) < 0) {
		gsgl_error ("write_token", "write returns %d", write_size); 
	}
}

static unsigned int server_inq_token (void *data1, gsglPage *p, READ_TOKEN)
{
	server_data *data = data1;
	return data->token;
}

static void server_begin_pageinfo (void *data1, gsglPage *p, READ_TOKEN)
{
	server_data *data = data1;
	unsigned int token;

	read_token_tcp (data, &token);
	read_token_tcp (data, &token);
	read_token_tcp (data, &token);
	read_token_tcp (data, &token);
	read_token_tcp (data, &token);
	read_token_tcp (data, &token);
	gsgl_keep_header ();
	gsaf_pages ();
}

static void server_begin_head (void *data1, gsglPage *p, READ_TOKEN)
{
	gsgl_begin_header (p);
}

static void server_end_head (void *data1, gsglPage *p, READ_TOKEN)
{
	gsgl_end_header (p);
}

static void server_begin_page (void *data1, gsglPage *p, READ_TOKEN)
{
}

static void server_end_page (void *data1, gsglPage *p, READ_TOKEN)
{
	server_data *data = data1;
	unsigned int token;

	gsaf_pagee ();
	/* this loop is needed when a user selected (F)ile in menu. */
	write_token (data->socket, GSGL_TCP_EXIT_FROM_PAGEE);
	do {
		read_token_tcp (data, &token);
		switch (token) {
		case GSGL_TCP_REMADE_INFO:
			gsgl_tcp_send_list_info_to_client (gsgl_inq_page());
			break;
		case GSGL_TCP_EXIT_FROM_PAGEE:
			break;
		default:
			gsgl_error ("interpret", "unexpected token in pagee %d", token);
		}
	} while (token != GSGL_TCP_EXIT_FROM_PAGEE);
}

static void server_vertex3 (void *data1, gsglPage *p, READ_TOKEN)
{
	server_data *data = data1;
	unsigned int token;
	float p1[3], p2[3], p3[3], normal[3];

	gsgl_file_format_read_float (data, &p1[0], read_token);
	gsgl_file_format_read_float (data, &p1[1], read_token);
	gsgl_file_format_read_float (data, &p1[2], read_token);
	if (!data->get_normal || data->set_normal_already) {
		gsgl_drv_vertex3 (p1[0], p1[1], p1[2], p);
		GSGL_DEBUG(fprintf (stderr, "%f %f %f\n", p1[0], p1[1], p1[2]));
	} else if (data->in_line) {
		gsgl_drv_glonly_normal (0.0, 0.0, 1.0, p);
		gsgl_drv_vertex3 (p1[0], p1[1], p1[2], p);
		data->set_normal_already = GSGL_TRUE;
	} else if (data->in_polygon) {
		read_token_tcp (data, &token);
		gsgl_file_format_read_float (data, &p2[0], read_token);
		gsgl_file_format_read_float (data, &p2[1], read_token);
		gsgl_file_format_read_float (data, &p2[2], read_token);
		read_token_tcp (data, &token);
		gsgl_file_format_read_float (data, &p3[0], read_token);
		gsgl_file_format_read_float (data, &p3[1], read_token);
		gsgl_file_format_read_float (data, &p3[2], read_token);
		gsgl_set_normal_vector2 (p1, p2, p3, normal);
		gsgl_drv_glonly_normal (normal[0],normal[1],normal[2],p);
		gsgl_drv_vertex3 (p1[0], p1[1], p1[2], p);
		gsgl_drv_vertex3 (p2[0], p2[1], p2[2], p);
		gsgl_drv_vertex3 (p3[0], p3[1], p3[2], p);
		data->set_normal_already = GSGL_TRUE;
		GSGL_DEBUG(fprintf (stderr, "%f %f %f\n", p1[0], p1[1], p1[2]));
		GSGL_DEBUG(fprintf (stderr, "%f %f %f\n", p2[0], p2[1], p2[2]));
		GSGL_DEBUG(fprintf (stderr, "%f %f %f\n", p3[0], p3[1], p3[2]));
	}
}

static void server_begin_line (void *data1, gsglPage *p, READ_TOKEN)
{
	server_data *data = data1;
	int coord;

	data->in_line = GSGL_TRUE;
	gsgl_coordinate_inq (&coord, p);
	if (coord == GSGL_COORD_NORMAL || coord == GSGL_COORD_2D) {
		gsgl_displaylist_begin (GSGL_LIST_2DOBJ, p);
	} else if (coord == GSGL_COORD_3D) {
		data->get_normal = GSGL_TRUE;
		data->set_normal_already = GSGL_FALSE;
		gsgl_displaylist_begin (GSGL_LIST_3DOBJ, p);
	}
	gsgl_drv_beginlines (p);
}

static void server_end_line (void *data1, gsglPage *p, READ_TOKEN)
{
	server_data *data = data1;

	data->in_line = GSGL_FALSE;
	gsgl_drv_endlines (p);
	data->get_normal = GSGL_FALSE;
	data->set_normal_already = GSGL_FALSE;
}

static void server_begin_poly (void *data1, gsglPage *p, READ_TOKEN)
{
	server_data *data = data1;
	int coord;

	data->in_polygon = GSGL_TRUE;
	gsgl_coordinate_inq (&coord, p);
	if (coord == GSGL_COORD_NORMAL || coord == GSGL_COORD_2D) {
		gsgl_displaylist_begin (GSGL_LIST_2DOBJ, p);
	} else if (coord == GSGL_COORD_3D) {
		data->get_normal = GSGL_TRUE;
		data->set_normal_already = GSGL_FALSE;
		gsgl_displaylist_begin (GSGL_LIST_3DOBJ, p);
	}
	gsgl_drv_beginpolygon (p);
}

static void server_end_poly (void *data1, gsglPage *p, READ_TOKEN)
{
	server_data *data = data1;

	data->in_polygon = GSGL_FALSE;
	gsgl_drv_endpolygon (p);
	data->get_normal = GSGL_FALSE;
	data->set_normal_already = GSGL_FALSE;
}

static void server_normal (void *data1, gsglPage *p, READ_TOKEN)
{
	server_data *data = data1;
	gsglvec3f vec;

	gsgl_file_format_read_float (data, &vec.x, read_token);
	gsgl_file_format_read_float (data, &vec.y, read_token);
	gsgl_file_format_read_float (data, &vec.z, read_token);
	gsgl_drv_normal (vec.x, vec.y, vec.z, p);
	data->set_normal_already = GSGL_TRUE;
	GSGL_DEBUG(fprintf (stderr, "%f %f %f\n", vec.x, vec.y, vec.z));
}

static void server_enable_client_vertex (void *data1, gsglPage *p, READ_TOKEN)
{
	server_data *data = data1;
	unsigned int token;

	read_token_tcp (data, &token);
	if (token == GSGL_FILE_CLIENT_VERTEX_TYPE) {
		read_token_tcp (data, &token);
		read_vertex (data, p, token);
	}
}

static void server_unexpected (void *data1, gsglPage *p, READ_TOKEN)
{
	server_data *data = data1;
	gsgl_warning ("interpret", "Unexpected token %d", data->token);
}

static void server_pstext (void *data1, gsglPage *p, READ_TOKEN)
{
	unsigned int token;
	unsigned int ifnts, ifntr, nmax, dim;
	int i;
	float chh, chw, chsp, angle, tilt, x, y, *vx, *vy; 
	unsigned int *iascs;
	server_data *data = data1;

	read_token (data, &token);
	if (token != GSGL_FILE_PS_TEXT_FNT) gsgl_error ("server_pstext", "token!=GSGL_FILE_PS_TEXT_FNT");
	read_token (data, &ifnts);
	read_token (data, &token);
	if (token != GSGL_FILE_PS_TEXT_FNT_DISPLAY) gsgl_error ("server_pstext", "token!=GSGL_FILE_PS_TEXT_FNT_DISPLAY");
	read_token (data, &ifntr);
	read_token (data, &token);
	if (token != GSGL_FILE_PS_TEXT_CHARACTER_HEIGHT) gsgl_error ("server_pstext", "token!=GSGL_FILE_PS_TEXT_CHARACTER_HEIGHT");
	gsgl_file_format_read_float (data, &chh, read_token);
	read_token (data, &token);
	if (token != GSGL_FILE_PS_TEXT_CHARACTER_WIDTH) gsgl_error ("server_pstext", "token!=GSGL_FILE_PS_TEXT_CHARACTER_WIDTH");
	gsgl_file_format_read_float (data, &chw, read_token);
	read_token (data, &token);
	if (token != GSGL_FILE_PS_TEXT_CHARACTER_SPACE) gsgl_error ("server_pstext", "token!=GSGL_FILE_PS_TEXT_CHARACTER_SPACE");
	gsgl_file_format_read_float (data, &chsp, read_token);
	read_token (data, &token);
	if (token != GSGL_FILE_PS_TEXT_CHARACTER_ANGLE) gsgl_error ("server_pstext", "token!=GSGL_FILE_PS_TEXT_CHARACTER_ANGLE");
	gsgl_file_format_read_float (data, &angle, read_token);
	read_token (data, &token);
	if (token != GSGL_FILE_PS_TEXT_CHARACTER_TILT) gsgl_error ("server_pstext", "token!=GSGL_FILE_PS_TEXT_CHARACTER_TILT");
	gsgl_file_format_read_float (data, &tilt, read_token);
	read_token (data, &token);
	if (token != GSGL_FILE_PS_TEXT_X) gsgl_error ("server_pstext", "token!=GSGL_FILE_PS_TEXT_X");
	gsgl_file_format_read_float (data, &x, read_token);
	read_token (data, &token);
	if (token != GSGL_FILE_PS_TEXT_Y) gsgl_error ("server_pstext", "token!=GSGL_FILE_PS_TEXT_Y");
	gsgl_file_format_read_float (data, &y, read_token);
	read_token (data, &token);
	if (token != GSGL_FILE_PS_TEXT_DIM) gsgl_error ("server_pstext", "token!=GSGL_FILE_PS_TEXT_DIM");
	read_token (data, &dim);
	read_token (data, &token);
	if (token != GSGL_FILE_PS_TEXT_CHARACTER_NUM) gsgl_error ("server_pstext", "token!=GSGL_FILE_PS_TEXT_CHARACTER_NUM");
	read_token (data, &nmax);
	iascs = (unsigned int *) gsgl_malloc_atomic (sizeof (unsigned int) * nmax);
	read_token (data, &token);
	if (token != GSGL_FILE_PS_TEXT_TEXT) gsgl_error ("server_pstext", "token!=GSGL_FILE_PS_TEXT_TEXT");
	for (i=0; i<nmax; i++) {
		read_token (data, &(iascs[i]));
		GSGL_DEBUG(fprintf(stderr, "%c", iascs[i]));
	}
	GSGL_DEBUG(fprintf(stderr, "\n"));

	/*fprintf (stderr, "%d %d %f %f %f %f %f %f %f %d %d\n",
	  ifnts, ifntr, chh, chw, chsp, angle, tilt, x, y, nmax, dim);*/
	
	if (ifntr >= GSGL_TCP_BITMAP_FONT) { /* bitmap font */

		vx = (float *) gsgl_malloc (sizeof(float) * nmax);
		vy = (float *) gsgl_malloc (sizeof(float) * nmax);
		read_token (data, &token);
		if (token != GSGL_FILE_PS_TEXT_POSITION)
			gsgl_error ("server_pstext", "token!=GSGL_FILE_PS_TEXT_POSITION");
		for (i=0; i<nmax; i++) {
			gsgl_file_format_read_float_754 (data, &(vx[i]), read_token);
			/*printf ("x[%d] = %f\n", i, vx[i]);*/
		}
		for (i=0; i<nmax; i++) {
			gsgl_file_format_read_float_754 (data, &(vy[i]), read_token);
			/*printf ("y[%d] = %f\n", i, vy[i]);*/
		}

		gsgl_pstext (ifnts, ifntr-GSGL_TCP_BITMAP_FONT, chh, chw, chsp, angle, tilt,
			     x, y, dim, iascs, nmax, GSGL_FALSE, p);
		
		gsgl_use_bitmap_font_explicit (GSGL_TRUE, p);
			
		gsgl_render_bitmap_server (ifnts, chh, chw, chsp, angle, tilt,
					   vx, vy, iascs, nmax, p);
		
	} else {
		gsgl_use_bitmap_font_explicit (GSGL_FALSE, p);
		gsgl_pstext (ifnts, ifntr, chh, chw, chsp, angle, tilt,
			     x, y, dim, iascs, nmax, GSGL_FALSE, p);
	}

	data->List = gsgl_inq_displaylist_is_enabled (p);
	data->Mem = gsgl_inq_drv_mem (p);
	data->Hd = gsgl_inq_drv_hd (p);

	if (data->Mem) gsgl_disable_drv_mem (p);
	if (data->Hd) gsgl_disable_drv_hd (p);
	if (data->List) gsgl_disable_displaylist (p);
}

static void server_pstext_end (void *data1, gsglPage *p, READ_TOKEN)
{
	server_data *data = data1;

	if (data->Mem) gsgl_enable_drv_mem (p);
	if (data->Hd) gsgl_enable_drv_hd (p);
	if (data->List) gsgl_enable_displaylist (p);
}

static void server_gui_button (void *data, gsglPage *p, READ_TOKEN)
{
	guialldata *data0;
	buttondata *s;

	data0 = gsgl_file_format_read_gui_button_data (data, p, read_token);
	s = data0->specific;
	gsgl_gui_button (data0->common, s->text, NULL);
}

static void server_gui_combo (void *data, gsglPage *p, READ_TOKEN)
{
	guialldata *data0;
	comboboxdata *s;

	data0 = gsgl_file_format_read_gui_combo_data (data, p, read_token);
	s = data0->specific;
	gsgl_gui_combobox (data0->common, s->items, s->text, s->current, NULL, 0);
}

static void server_gui_spinf (void *data, gsglPage *p, READ_TOKEN)
{
	guialldata *data0;
	spinbuttondataf *s;

	data0 = gsgl_file_format_read_gui_spinf_data (data, p, read_token);
	s = data0->specific;
	gsgl_gui_spinbuttonf (data0->common, s->init, s->lower, s->upper, s->digits, NULL);
}

static void server_gui_spini (void *data, gsglPage *p, READ_TOKEN)
{
	guialldata *data0;
	spinbuttondatai *s;

	data0 = gsgl_file_format_read_gui_spini_data (data, p, read_token);
	s = data0->specific;
	gsgl_gui_spinbuttoni (data0->common, s->init, s->lower, s->upper, NULL);
}

static void server_gui_check (void *data, gsglPage *p, READ_TOKEN)
{
	guialldata *data0;
	checkbuttondata *s;

	data0 = gsgl_file_format_read_gui_check_data (data, p, read_token);
	s = data0->specific;
	gsgl_gui_checkbutton (data0->common, s->text, s->init, NULL);
}

static void server_gui_edit (void *data, gsglPage *p, READ_TOKEN)
{
	guialldata *data0;
	lineeditdata *s;

	data0 = gsgl_file_format_read_gui_edit_data (data, p, read_token);
	s = data0->specific;
	gsgl_gui_lineedit (data0->common, s->max, s->text, NULL);
}

static void server_gui_label (void *data, gsglPage *p, READ_TOKEN)
{
	guialldata *data0;
	labeldata *s;

	data0 = gsgl_file_format_read_gui_label_data (data, p, read_token);
	s = data0->specific;
	gsgl_gui_label (data0->common, s->text, s->ijust);
}

gsglPage * gsgl_server_interpret (server_data *data, gsglPage *p, 
				  void (*func_default)(unsigned int, void *, gsglPage *, READ_TOKEN))
{
	return gsgl_file_format_read_template (p,
					data,
					read_token_tcp,
					server_inq_token, 
					gsgl_file_format_read_vertex2,
					server_vertex3,
					gsgl_file_format_read_mode_normal,
					gsgl_file_format_read_mode_2d,
					gsgl_file_format_read_mode_3d,
					gsgl_file_format_read_separate_objects,
					gsgl_file_format_read_array_begin_polygon,
					gsgl_file_format_read_array_end_polygon,
					gsgl_file_format_read_array_begin_line,
					gsgl_file_format_read_array_end_line,
					gsgl_file_format_read_begin_point,
					gsgl_file_format_read_end_point,
					server_begin_line,
					server_end_line,
					server_begin_poly,
					server_end_poly,
					server_normal,
					gsgl_file_format_read_begin_triangles,
					gsgl_file_format_read_end_triangles,
					gsgl_file_format_read_begin_triangle_strip,
					gsgl_file_format_read_end_triangle_strip,
					gsgl_file_format_read_begin_triangle_fan,
					gsgl_file_format_read_end_triangle_fan,
					gsgl_file_format_read_clip_3d_fileonly,
					gsgl_file_format_read_clip_unmovable,
					gsgl_file_format_read_del_clip,
					gsgl_file_format_read_modelview,
					gsgl_file_format_read_rgb,
					gsgl_file_format_read_array_element_4byte,
					gsgl_file_format_read_rgba,
					gsgl_file_format_read_indexcolor,
					gsgl_file_format_read_linewidth,
					gsgl_file_format_read_linewidth2,
					gsgl_file_format_read_projection,
					gsgl_file_format_read_3d_3,
					gsgl_file_format_read_projection_type,
					gsgl_file_format_read_enable_lighting,
					gsgl_file_format_read_disable_lighting,
					server_begin_pageinfo,
					gsgl_file_format_read_do_nothing,
					server_begin_head,
					server_end_head,
					server_begin_page,
					server_end_page,
					gsgl_file_format_read_begin_anime,
					gsgl_file_format_read_end_anime,
					server_enable_client_vertex,
					gsgl_file_format_read_enable_client_color,
					gsgl_file_format_read_begin_gouraud_only,
					gsgl_file_format_read_end_gouraud_only,
					gsgl_file_format_read_depth_by_deep,
					gsgl_file_format_read_depth_by_shallow,
					gsgl_file_format_read_depth_by_mean,
					gsgl_file_format_read_depth_by_point,
					gsgl_file_format_read_depth_by_vec_prod,
					gsgl_file_format_read_depth_by_vec_prod2,
					server_unexpected,
					server_unexpected,
					server_unexpected,
					server_unexpected,
					server_unexpected,
					server_unexpected,
					server_unexpected,
					server_unexpected,
					server_unexpected,
					server_unexpected,
					server_unexpected,
					server_unexpected,
					server_unexpected,
					server_unexpected,
					server_unexpected,
					server_unexpected,
					server_unexpected,
					server_unexpected,
					server_unexpected,
					server_unexpected,
					server_unexpected,
					server_unexpected,
					server_unexpected,
					server_unexpected,
					server_pstext,
					server_pstext_end,
					server_gui_button,
					server_gui_combo,
					server_gui_spinf,
					server_gui_spini,
					server_gui_check,
					server_gui_edit,
					server_gui_label,
					gsgl_file_format_read_gui_external,
					func_default);
}

static void read_vertex (server_data *data, gsglPage *p, int datatype)
{
	gsgl_file_format_read_vertex_template (data, p, datatype, GSGL_TRUE,
					       read_token_tcp, gsgl_file_format_read_float);
}
