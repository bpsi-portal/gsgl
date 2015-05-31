#include "gsaf.h"
#include "device.h"
#include "anime.h"
#include "xwin.h"
#include "ogl.h"
#include "displaylist.h"
#include "drv_wrapper.h"
#include "clientstate.h"
#include "utils.h"
#include "harddisk.h"
#include "tcpip.h"
#include "fileio_base.h"
#include "fileio_read_base.h"
#include "base.h"
#include "fileformat.h"
#include "matrix.h"
#include "callback.h"
#include "bsc1.h"
#include "mem_list.h"

#define GSGL_DEBUG(x)
#define GSGL_FRAMES(x) ((gsglframes *)(x->data))

typedef struct _gsglframes {
	gsglstatus status;
	unsigned long int start;
	int start_surplus;
	unsigned long int end;
	int end_surplus;
} gsglframes;

gsglframes *gsgl_frame_new ()
{
	gsglframes *frame;

	frame = (gsglframes *) gsgl_malloc (sizeof (gsglframes));
	frame->start = 0;
	frame->start_surplus = 0;
	frame->end = 0;
	frame->end_surplus = 0;

	return frame;
}

void gsgl_anime_use_anime (gsglPage *p)
{
	ANIME(p)->Anime = GSGL_TRUE;
}

void gsgl_anime_dont_use_anime (gsglPage *p)
{
	ANIME(p)->Anime = GSGL_FALSE;
}

gsglboolean gsgl_inq_anime_is_used (gsglPage *p)
{
	return ANIME(p)->Anime;
}

void gsgl_init_anime_every_page (gsglPage *p)
{
	ANIME(p)->Anime = GSGL_FALSE;
	ANIME(p)->Frames = NULL;
	ANIME(p)->LastList = NULL;
	ANIME(p)->ListBackUp = NULL;
	ANIME(p)->Frame_num = 0;
	ANIME(p)->Fp = NULL;
}

int gsgl_inq_current_frame_num (gsglPage *p)
{
	return ANIME(p)->Frame_num;
}

void gsgl_set_current_frame_num (int i, gsglPage *p)
{
	ANIME(p)->Frame_num = i;
}

gsglboolean gsgl_inq_open_tmp_file_for_anime (gsglPage *p)
{
	if (ANIME(p)->Fp)
		return GSGL_TRUE;
	else
		return GSGL_FALSE;
}

static void gsgl_anime_begin (gsglPage *p)
{
	int   fd;
	gsglframes *frame;

	if (gsgl_inq_devicetype (LIB(p)) == GSGL_DEVICE_PS ||
	    gsgl_inq_devicetype (LIB(p)) == GSGL_DEVICE_RASTER)
		gsgl_error ("gsgl_anime_begin", 
			    "Gsgl animation file can't be converted to image file.\n"
			 "Please use gsglanime2pages to separate animation frames into pages.");

	if (!ANIME(p)->Anime) {
		/*GSelect = GSGL_FALSE;*/
		gsgl_displaylist_end (p);
		ANIME(p)->LastList = gsgl_displaylist_inq_last (p);
		gsgl_disable_drv_mem (p);
		if (!gsgl_inq_drv_hd (p) && !ANIME(p)->Fp
		    && (gsgl_inq_mode (LIB(p)) == GSGL_MODE_WINDOW ||
			gsgl_inq_mode (LIB(p)) == GSGL_MODE_TCPIP)) { /* open temporary file */

			strcpy (ANIME(p)->Tmp, "/tmp/gsglXXXXXX");

			fd = mkstemp (ANIME(p)->Tmp);
			ANIME(p)->Fp = fdopen (fd, "w+");

			gsgl_init_hd_for_anime (p);
			gsgl_enable_drv_hd (p);
		}
	}
	if (ANIME(p)->Fp) {
		frame = gsgl_frame_new ();
		frame->start = gsgl_hd_ftell (&(frame->start_surplus), p);
		frame->status = gsgl_inq_all_status (p);

		ANIME(p)->Frames = gsgl_list_append (ANIME(p)->Frames, frame);
	}
	gsgl_drv_beginanime (p);
	ANIME(p)->Anime = GSGL_TRUE;
	gsgl_disable_displaylist (p);
}

void gsgl_animes (void)
{
	if (!gsgl_inq_page_is_open ()) return;

	gsgl_anime_begin (gsgl_inq_page());
}

void gsgl_anime_finish_every_page (gsglPage *p)
{
	if (ANIME(p)->Fp) {
		gsgl_finish_hd_for_anime (p);
		gsgl_enable_drv_mem (p);
		gsgl_disable_drv_hd (p);
		
		if (fclose (ANIME(p)->Fp))
			gsgl_warning ("failed to close tmp file", "gsgl_anime_finish_every_page");
		ANIME(p)->Frames = NULL;
		
		if (remove (ANIME(p)->Tmp))
			gsgl_warning ("failed to remove tmp file", "gsgl_anime_remove_tmpfile");

		ANIME(p)->Fp = NULL;
	}
}

void gsgl_copy_anime_file_to_save_file (gsglPage *p)
{
	FILE *fp;
	char buffer[256];

	if (ANIME(p)->Fp) {
		fp = fopen (ANIME(p)->Tmp, "r");

		while(!feof(fp)){
			fgets (buffer, 256, fp);
			fputs (buffer, gsgl_get_fp_write (LIB(p)));
		}
		if (fclose (fp))
			gsgl_warning ("gsgl_copy_anime_file_to_save_file",
				      "failed to close tmp file");
	}
}

static void gsgl_anime_end (gsglPage *p)
{
	gsgllist *list;
	gsglframes *frame;

	gsgl_drv_endanime (p);
	gsgl_clientstate_disable (p);
	if (ANIME(p)->Fp) {
		gsgl_end_hd_for_anime (p);
		list = gsgl_list_last (ANIME(p)->Frames);
		frame = GSGL_FRAMES(list);
		frame->end = gsgl_hd_ftell (&(frame->end_surplus), p);
	}
}

void gsgl_animee (void)
{
	if (!gsgl_inq_page_is_open ()) return;

	gsgl_anime_end (gsgl_inq_page());
}

typedef struct _frame_data {
	unsigned char *loc;
	gsglReadFile *r;
	gsglboolean get_normal;
	gsglboolean set_normal_already;
	gsglboolean in_line;
	gsglboolean in_polygon;
	gsglboolean quit;
} frame_data;

static void frame_token (void *data, unsigned int *token);
static void frame_unexpected (void *data1, gsglPage *, READ_TOKEN);
static void frame_vertex3 (void *data1, gsglPage *, READ_TOKEN);
static void frame_begin_line (void *data1, gsglPage *, READ_TOKEN);
static void frame_end_line (void *data1, gsglPage *, READ_TOKEN);
static void frame_begin_poly (void *data1, gsglPage *, READ_TOKEN);
static void frame_end_poly (void *data1, gsglPage *, READ_TOKEN);
static void frame_normal (void *data1, gsglPage *, READ_TOKEN);
static void frame_vertex (frame_data *data, gsglPage *p, int datatype);
static void frame_enable_client_vertex (void *data1, gsglPage *, READ_TOKEN);
static void frame_begin_anime (void *data1, gsglPage *, READ_TOKEN);
static void frame_end_anime (void *data1, gsglPage *, READ_TOKEN);
static void frame_default (unsigned int token, void *data, gsglPage *, READ_TOKEN);

unsigned int gsgl_anime_inq_frame_num (gsglPage *p)
{
	if (!ANIME(p)->Fp) return 0;	

	return gsgl_list_count (ANIME(p)->Frames);
}

static void restore_all_status (gsglstatus status, gsglPage *p)
{
	int i, j;
	float matrix[16];
	float eq[4];

        if (status.rgb.r > 0)
                gsgl_rgb (status.rgb.r, status.rgb.g, status.rgb.b, p);
	else if (status.indexcolor > 0)
                gsgl_color_index (status.indexcolor, p);
        
	if (status.linewidth_cm > 0)
		gsgl_linewidth (status.linewidth_cm, p);
	else if (status.linewidth_index > 0)
		gsgl_linewidth_index (status.linewidth_index, p);
        
        for (i=0; i<6; i++) {
                if (status.clip_plane[i] > 0) {
			for (j=0; j<16; j++) matrix[j] = status.clip_proj [i][j];
			gsgl_projection_matrix (matrix, p);
			for (j=0; j<16; j++) matrix[j] = status.clip_model [i][j];
			gsgl_modelview_matrix (matrix, p);
			
			for (j=0; j<4; j++)
				eq[j] = status.clip_eq[i][j];
			if (status.clip_plane[i] == GSGL_FILE_CLIP_UNMOVABLE)
				gsgl_clip_enable_normal (i, eq, p);
                        else if (status.clip_plane[i] == GSGL_FILE_CLIP_3D_FILEONLY)
				gsgl_clip_enable_normal (i, eq, p);
                }
        }
	gsgl_projectiontype (status.threeDinfo.projection, p);
        gsgl_projection_matrix (status.projection_matrix, p);
	
	if (status.coord_mode == GSGL_COORD_NORMAL)
		gsgl_normalmode (p);
	else if (status.coord_mode == GSGL_COORD_2D)
		gsgl_2dmode (p);
	else if (status.coord_mode == GSGL_COORD_3D)
		gsgl_3dmode (p);
	else if (status.coord_mode == GSGL_COORD_TEXT2D)
		gsgl_text2dmode (p);
	else if (status.coord_mode == GSGL_COORD_TEXT3D)
		gsgl_text3dmode (p);
	else
		gsgl_normalmode (p);

	gsgl_modelview_matrix (status.modelview_matrix, p);

        if (fabs(status.threeDinfo.depth) > FLOAT_EPS)
		gsgl_depth3d (status.threeDinfo.depth, p);
        
	if (status.lighting_enabled_by_user) {
		gsgl_lighting_enable_by_user (p);
		if (!status.lighting_enabled_temporary)
			gsgl_lighting_disable_temporary ();
	} else if (status.threeDinfo.Lighting)
		gsgl_lighting_enable (p);

	if (status.antialiasing) {
		gsgl_drv_enable_antialiasing (p);
	} else {
		gsgl_drv_disable_antialiasing (p);
	}
}

void gsgl_anime_nth_frame (unsigned int n, gsglPage *p)
{
	gsgllist *list;
	gsglframes *frame;
	unsigned char *buffer, *end;
	long int i;
	unsigned int token;
	frame_data *data;

	if (!ANIME(p)->Fp || ANIME(p)->Frames == NULL) return;

	if (gsgl_inq_i_am_a_server (LIB(p))) {
		gsgl_tcp_nth_frame (n, LIB(p));
	}

	list = gsgl_list_nth (ANIME(p)->Frames, n);
	frame = GSGL_FRAMES(list);

	gsgl_finish_hd_for_anime (p);
	gsgl_disable_drv_hd (p);
	gsgl_enable_drv_mem (p);
	gsgl_enable_displaylist (p);

	gsgl_displaylist_clear (ANIME(p)->LastList);
	ANIME(p)->LastList->next = NULL;

	if (frame->start_surplus == 2) frame->start -= 0;
	else if (frame->start_surplus == 1) frame->start -= 4;

	if (frame->end_surplus == 2) frame->end += 4;
	else if (frame->end_surplus == 1) frame->end += 4;
	
	if (fseek (ANIME(p)->Fp, frame->start, SEEK_SET)) 
		gsgl_error ("gsgl_anime_nth_frame", "can't seek temporary file");

	i = (frame->end)-(frame->start);
	buffer = (unsigned char *) malloc (i*sizeof(unsigned char));
	i = fread (buffer, 1, i, ANIME(p)->Fp);

	data = (frame_data *) gsgl_malloc (sizeof(frame_data));
	data->quit = GSGL_FALSE;
	data->loc = buffer;
	data->r = gsgl_init_read_file ();
	end = data->loc + i;
	
	if (frame->start_surplus == 2) data->loc = gsglfread_token (data->loc, &token, data->r);
	if (frame->start_surplus == 1) {
		data->loc = gsglfread_token (data->loc, &token, data->r);
		data->loc = gsglfread_token (data->loc, &token, data->r);
	}

	restore_all_status (frame->status, p);
	while (data->loc < end || (data->r->surplus == 2 && frame->end_surplus == 0)) {
		gsgl_file_format_read_template (p,
                                                data,
						frame_token,
						gsgl_file_format_read_inq_token, 
						gsgl_file_format_read_vertex2,
						frame_vertex3,
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
						frame_begin_line,
						frame_end_line,
						frame_begin_poly,
						frame_end_poly,
						frame_normal,
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
						frame_unexpected,
						frame_unexpected,
						frame_unexpected,
						frame_unexpected,
						frame_unexpected,
						frame_unexpected,
						frame_begin_anime,
						frame_end_anime,
						frame_enable_client_vertex,
						gsgl_file_format_read_enable_client_color,
						gsgl_file_format_read_begin_gouraud_only,
						gsgl_file_format_read_end_gouraud_only,
						gsgl_file_format_read_depth_by_deep,
						gsgl_file_format_read_depth_by_shallow,
						gsgl_file_format_read_depth_by_mean,
						gsgl_file_format_read_depth_by_point,
						gsgl_file_format_read_depth_by_vec_prod,
						gsgl_file_format_read_depth_by_vec_prod2,
						frame_unexpected,
						frame_unexpected,
						frame_unexpected,
						frame_unexpected,
						frame_unexpected,
						frame_unexpected,
						frame_unexpected,
						frame_unexpected,
						frame_unexpected,
						frame_unexpected,
						frame_unexpected,
						frame_unexpected,
						frame_unexpected,
						frame_unexpected,
						frame_unexpected,
						frame_unexpected,
						frame_unexpected,
						frame_unexpected,
						frame_unexpected,
						frame_unexpected,
						frame_unexpected,
						frame_unexpected,
						frame_unexpected,
						frame_unexpected,
						gsgl_file_format_read_pstext,
						frame_unexpected,
						gsgl_file_format_read_gui_button,
						gsgl_file_format_read_gui_combo,
						gsgl_file_format_read_gui_spinf,
						gsgl_file_format_read_gui_spini,
						gsgl_file_format_read_gui_check,
						gsgl_file_format_read_gui_edit,
						gsgl_file_format_read_gui_label,
						gsgl_file_format_read_gui_external_ignore,
						frame_default);
		if (data->quit) break;
	}
	gsgl_displaylist_end (p);
	free (buffer);
	gsgl_free (data);

	if (frame->start_surplus == 2) frame->start += 0;
	else if (frame->start_surplus == 1) frame->start += 4;

	if (frame->end_surplus == 2) frame->end -= 4;
	else if (frame->end_surplus == 1) frame->end -= 4;
}

void gsgl_anime_backup_displaylist (gsglPage *p)
{
	gsgllist *list;
	gsglDisplaylist *d;

	gsgl_enable_displaylist (p);
	list = gsgl_displaylist_inq_start (p);
	list = list->next;

	ANIME(p)->ListBackUp = NULL;
	while (list != ANIME(p)->LastList && list != NULL) {
		d = gsgl_displaylist_new (p);
		ANIME(p)->ListBackUp = gsgl_list_append (ANIME(p)->ListBackUp, d);
		gsgl_displaylist_copy (list->data, d);
		
		list = list->next;
	}
	if (list == ANIME(p)->LastList) {
		d = gsgl_displaylist_new (p);
		ANIME(p)->ListBackUp = gsgl_list_append (ANIME(p)->ListBackUp, d);
		gsgl_displaylist_copy (list->data, d);
	}
}

gsglboolean gsgl_anime_init_replay (gsglPage *p)
{
	int n;
	gsgllist *list;

	n = gsgl_anime_inq_frame_num (p);
	if (n<1) return GSGL_FALSE;

	if (gsgl_displaylist_inq_remade (p)) {
		gsgl_displaylist_init_remade (p);
		gsgl_free_select_buffer (WIN(p));
		gsgl_init_select (p);

		list = gsgl_displaylist_inq_start (p);
		list->next = ANIME(p)->ListBackUp;
		ANIME(p)->LastList = gsgl_list_last (list);
		gsgl_anime_backup_displaylist (p);
	}
	return GSGL_TRUE;
}

static void frame_token (void *data1, unsigned int *token)
{
	frame_data *data = data1;
	data->loc = gsglfread_token (data->loc, token, data->r);
}

static void frame_unexpected (void *data1, gsglPage *p, READ_TOKEN)
{
	gsgl_error ("gsgl_anime_nth_frame", "Unexpected token");
}

static void frame_vertex3 (void *data1, gsglPage *p, READ_TOKEN)
{
	frame_data *data = data1;
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
		frame_token (data, &token);
		gsgl_file_format_read_float (data, &p2[0], read_token);
		gsgl_file_format_read_float (data, &p2[1], read_token);
		gsgl_file_format_read_float (data, &p2[2], read_token);
		frame_token (data, &token);
		gsgl_file_format_read_float (data, &p3[0], read_token);
		gsgl_file_format_read_float (data, &p3[1], read_token);
		gsgl_file_format_read_float (data, &p3[2], read_token);
		gsgl_set_normal_vector2 (p1, p2, p3, normal);
		gsgl_drv_glonly_normal (normal[0],normal[1],normal[2], p);
		gsgl_drv_vertex3 (p1[0], p1[1], p1[2], p);
		gsgl_drv_vertex3 (p2[0], p2[1], p2[2], p);
		gsgl_drv_vertex3 (p3[0], p3[1], p3[2], p);
		data->set_normal_already = GSGL_TRUE;
		GSGL_DEBUG(fprintf (stderr, "%f %f %f\n", p1[0], p1[1], p1[2]));
		GSGL_DEBUG(fprintf (stderr, "%f %f %f\n", p2[0], p2[1], p2[2]));
		GSGL_DEBUG(fprintf (stderr, "%f %f %f\n", p3[0], p3[1], p3[2]));
	}
}

static void frame_begin_line (void *data1, gsglPage *p, READ_TOKEN)
{
	frame_data *data = data1;
	int coord;

	data->in_line = GSGL_TRUE;
	gsgl_coordinate_inq (&coord, gsgl_inq_page());
	if (coord == GSGL_COORD_NORMAL || coord == GSGL_COORD_2D) {
		gsgl_displaylist_begin (GSGL_LIST_2DOBJ, p);
	} else if (coord == GSGL_COORD_3D) {
		data->get_normal = GSGL_TRUE;
		data->set_normal_already = GSGL_FALSE;
		gsgl_displaylist_begin (GSGL_LIST_3DOBJ, p);
	}
	gsgl_drv_beginlines (p);
}

static void frame_end_line (void *data1, gsglPage *p, READ_TOKEN)
{
	frame_data *data = data1;

	data->in_line = GSGL_FALSE;
	gsgl_drv_endlines (p);
	data->get_normal = GSGL_FALSE;
	data->set_normal_already = GSGL_FALSE;
}

static void frame_begin_poly (void *data1, gsglPage *p, READ_TOKEN)
{
	frame_data *data = data1;
	int coord;

	data->in_polygon = GSGL_TRUE;
	gsgl_coordinate_inq (&coord, gsgl_inq_page());
	if (coord == GSGL_COORD_NORMAL || coord == GSGL_COORD_2D) {
		gsgl_displaylist_begin (GSGL_LIST_2DOBJ, p);
	} else if (coord == GSGL_COORD_3D) {
		data->get_normal = GSGL_TRUE;
		data->set_normal_already = GSGL_FALSE;
		gsgl_displaylist_begin (GSGL_LIST_3DOBJ, p);
	}
	gsgl_drv_beginpolygon (p);
}

static void frame_end_poly (void *data1, gsglPage *p, READ_TOKEN)
{
	frame_data *data = data1;

	data->in_polygon = GSGL_FALSE;
	gsgl_drv_endpolygon (p);
	data->get_normal = GSGL_FALSE;
	data->set_normal_already = GSGL_FALSE;
}

static void frame_normal (void *data1, gsglPage *p, READ_TOKEN)
{
	frame_data *data = data1;
	gsglvec3f vec;

	gsgl_file_format_read_float (data, &vec.x, read_token);
	gsgl_file_format_read_float (data, &vec.y, read_token);
	gsgl_file_format_read_float (data, &vec.z, read_token);
	gsgl_drv_normal (vec.x, vec.y, vec.z, p);
	data->set_normal_already = GSGL_TRUE;
	GSGL_DEBUG(fprintf (stderr, "%f %f %f\n", vec.x, vec.y, vec.z));
}

static void frame_vertex (frame_data *data, gsglPage *p, int datatype)
{
	gsgl_file_format_read_vertex_template (data, p, datatype, GSGL_TRUE,
					       frame_token, gsgl_file_format_read_float);
}

static void frame_enable_client_vertex (void *data1, gsglPage *p, READ_TOKEN)
{
	frame_data *data = data1;
	unsigned int token;

	frame_token (data, &token);
	if (token == GSGL_FILE_CLIENT_VERTEX_TYPE) {
		frame_token (data, &token);
		frame_vertex (data, p, token);
	}
}

static void frame_begin_anime (void *data1, gsglPage *p, READ_TOKEN)
{
	/*frame_data *data = data1;
	  unsigned int token;*/
}

static void frame_end_anime (void *data1, gsglPage *p, READ_TOKEN)
{
	frame_data *data = data1;
	data->quit = GSGL_TRUE;
}

static void frame_default (unsigned int token, void *data, gsglPage *p, READ_TOKEN)
{
	gsgl_error ("gsgl_anime_nth_frame", "Unexpected token %d", token);
}
