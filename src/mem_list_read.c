#include "gsaf.h"
#include "fileformat.h"
#include "ogl.h"
#include "mem_list_read.h"
#include "mem_list.h"
#include "displaylist.h"
#include "base.h"
#include "utils.h"
#include "drv_gl.h"
#include "clientstate.h"

#define GSGL_DEBUG(x)

typedef struct _mem_list_data {
	unsigned short *mem;
	unsigned int count;
	int read_array_count;
} mem_list_data;

static unsigned int gsgl_mem_list_read_vertex (mem_list_data *data, gsglPage *p, int datatype);

static void read_token_mem (void *data1, unsigned int *token)
{
	mem_list_data *data = data1;
	data->count = gsgl_mem_list_read_token (data->mem, data->count, token);
}

void gsgl_mem_list_read_array_element_4byte (void *data1, gsglPage *p, READ_TOKEN)
{
	unsigned int token, token1, token2;
	mem_list_data *data = data1;

	data->count = gsgl_mem_list_read_token (data->mem, data->count, &token1);
	data->count = gsgl_mem_list_read_token (data->mem, data->count, &token2);
	token = gsgl_convert_tokens_to_4byte (token1, token2);
	gsgl_gl_array_element (token);
}

void gsgl_mem_list_read_array_begin_polygon (void *data1, gsglPage *p, READ_TOKEN)
{
	gsgl_gl_glarray_begin_polygon ();
}

void gsgl_mem_list_read_array_end_polygon (void *data1, gsglPage *p, READ_TOKEN)
{
	gsgl_gl_glarray_end_polygon ();
}

void gsgl_mem_list_read_array_begin_line (void *data1, gsglPage *p, READ_TOKEN)
{
	gsgl_gl_glarray_begin_lines ();
}

void gsgl_mem_list_read_array_end_line (void *data1, gsglPage *p, READ_TOKEN)
{
	gsgl_gl_glarray_end_lines ();
}

void gsgl_mem_list_read_enable_client_vertex (void *data1, gsglPage *p, READ_TOKEN)
{
	mem_list_data *data = data1;
	unsigned int token, vertexsize = 0;
	gsgllist *list;
	int i;

	glDisableClientState (GL_VERTEX_ARRAY);
	glDisableClientState (GL_COLOR_ARRAY);
	glDisableClientState (GL_NORMAL_ARRAY);
	data->count = gsgl_mem_list_read_token (data->mem, data->count, &token);
	if (token == GSGL_FILE_CLIENT_VERTEX_TYPE) {
		data->count = gsgl_mem_list_read_token (data->mem, data->count, &token);
		vertexsize = gsgl_mem_list_read_vertex (data, p, token);
	}
	if (gsgl_inq_clientstate_vertex_enabled_num (p) >= 2) {
		list = gsgl_inq_first_clientstate_vertex_pointer (p);
		for (i=0; i<data->read_array_count; i++)
			list = list->next;
		glEnableClientState (GL_VERTEX_ARRAY);
		glVertexPointer (vertexsize, GL_FLOAT, 0, list->data);
		list = gsgl_inq_first_clientstate_normal_pointer (p);
		for (i=0; i<data->read_array_count; i++)
			list = list->next;
		glEnableClientState (GL_NORMAL_ARRAY);
		glNormalPointer (GL_FLOAT, 0, list->data);
		data->read_array_count++;
		if (data->read_array_count == gsgl_inq_clientstate_vertex_enabled_num (p))
			data->read_array_count = 0;
	} else {
		list = gsgl_inq_first_clientstate_vertex_pointer (p);
		glEnableClientState (GL_VERTEX_ARRAY);
		glVertexPointer (vertexsize, GL_FLOAT, 0, list->data);
		list = gsgl_inq_first_clientstate_normal_pointer (p);
		glEnableClientState (GL_NORMAL_ARRAY);
		glNormalPointer (GL_FLOAT, 0, list->data);
	}
}

void gsgl_mem_list_read_enable_client_color (void *data1, gsglPage *p, READ_TOKEN)
{
	mem_list_data *data = data1;
	unsigned int token, token1, token2, datasize=0, totalsize=0;
	gsgllist *list;
	int i, j;

	glDisableClientState (GL_COLOR_ARRAY);
	data->count = gsgl_mem_list_read_token (data->mem, data->count, &token);
	if (token == GSGL_FILE_CLIENT_COLOR_SIZE) {
		data->count = gsgl_mem_list_read_token (data->mem, data->count, &token);
		datasize = token;
		data->count = gsgl_mem_list_read_token (data->mem, data->count, &token);
		if (token == GSGL_FILE_CLIENT_COLOR_TOTALSIZE_2BYTE) {
			data->count = gsgl_mem_list_read_token (data->mem, data->count, &token);
			totalsize = token;
		} else if (token == GSGL_FILE_CLIENT_COLOR_TOTALSIZE_4BYTE) {
			data->count = gsgl_mem_list_read_token (data->mem, data->count, &token1);
			data->count = gsgl_mem_list_read_token (data->mem, data->count, &token2);
			totalsize = gsgl_convert_tokens_to_4byte (token1, token2);
		}
		
	}
	if (gsgl_inq_clientstate_vertex_enabled_num (p) >= 2) {
		list = gsgl_inq_first_clientstate_color_pointer (p);
		if (data->read_array_count == 0)
			j = gsgl_inq_clientstate_vertex_enabled_num (p) - 1;
		else
			j = data->read_array_count - 1;
		for (i=0; i<j; i++)
			list = list->next;
		if (list->data != NULL) {
			glEnableClientState (GL_COLOR_ARRAY);
			glColorPointer (datasize, GL_FLOAT, 0, list->data);
		} else {
			/*glDisableClientState(GL_COLOR_ARRAY);*/
		}
	} else {
		list = gsgl_inq_first_clientstate_color_pointer (p);
		if (list->data != NULL) {
			glEnableClientState (GL_COLOR_ARRAY);
			glColorPointer (datasize, GL_FLOAT, 0, list->data);
		}
	}
	for (i=0; i<totalsize; i++) {
		data->count = gsgl_mem_list_read_token (data->mem, data->count, &token1);
	}
}

void gsgl_mem_list_read_clip_3d_fileonly (void *data1, gsglPage *p, READ_TOKEN)
{
	mem_list_data *data = data1;
	unsigned int token, token1, token2;
	int i, clip_plane_num;
	float clip_plane_eqn[4];

	data->count = gsgl_mem_list_read_token (data->mem, data->count, &token);
	if (token != GSGL_FILE_CLIP_NUM) return;
	data->count = gsgl_mem_list_read_token (data->mem, data->count, &token);
	clip_plane_num = token;
	data->count = gsgl_mem_list_read_token (data->mem, data->count, &token);
	if (token != GSGL_FILE_CLIP_COEFF) return;
	for (i=0; i<4; i++) {
		data->count = gsgl_mem_list_read_token (data->mem, data->count, &token1);
		data->count = gsgl_mem_list_read_token (data->mem, data->count, &token2);
		clip_plane_eqn[i] = gsgl_convert_token_to_float (token1, token2);
	}
	gsgl_gl_clip_enable_3d_fileonly (clip_plane_num, clip_plane_eqn);
}

void gsgl_mem_list_read_begin_pageinfo (void *data1, gsglPage *p, READ_TOKEN)
{
	mem_list_data *data = data1;
	unsigned int token;

	/* GSGL_FILE_PAGE_NUMBER */
	data->count = gsgl_mem_list_read_token (data->mem, data->count, &token);
	/* gsgl_inq_page_number () */
	data->count = gsgl_mem_list_read_token (data->mem, data->count, &token);
	/* GSGL_FILE_PAGE_SIZE */
	data->count = gsgl_mem_list_read_token (data->mem, data->count, &token);
	/* 0 */
	data->count = gsgl_mem_list_read_token (data->mem, data->count, &token);
	/* 0 */
	data->count = gsgl_mem_list_read_token (data->mem, data->count, &token);
	/* GSGL_FILE_END_PAGEINFO */
	data->count = gsgl_mem_list_read_token (data->mem, data->count, &token);
}

void gsgl_mem_list_read_unexpected (void *data1, gsglPage *p, READ_TOKEN)
{
	gsgl_error ("gsgl_mem_list_read", "Unexpected token");
}

void gsgl_mem_list_read_default (unsigned int token, void *data1, gsglPage *p, READ_TOKEN)
{
	gsgl_error ("gsgl_mem_list_read", "Unknown token (%d)", token);
}

unsigned int gsgl_mem_list_read_inq_token (void *data1, gsglPage *p, READ_TOKEN)
{
	mem_list_data *data = data1;
	unsigned int token;
 
	data->count = gsgl_mem_list_read_token (data->mem, data->count, &token);
	GSGL_DEBUG(fprintf (stderr, "token = %d\n", token));
	return token;
}

/************************************************************/
/*    Read Fileformat which is a part of displaylist        */
/************************************************************/
void gsgl_mem_list_read (unsigned short *mem, unsigned int size, gsglPage *p)
{
	static int read_array_count=0;
	mem_list_data *data;

	GSGL_DEBUG(fprintf (stderr, "%p %d\n", mem, size));
	
	if (size <= 0 || !mem) return;

	data = (mem_list_data *) gsgl_malloc (sizeof(mem_list_data));
	data->mem = mem;
	data->count = 0;
	data->read_array_count = read_array_count;

	GSGL_DEBUG(fprintf (stderr, "start (size=%d)\n", size));

	while (data->count < size) {
		gsgl_file_format_read_template (p,
                                                data,
						read_token_mem,
						gsgl_mem_list_read_inq_token, 
						gsgl_mem_list_read_unexpected,
						gsgl_mem_list_read_unexpected,
						gsgl_mem_list_read_unexpected,
						gsgl_mem_list_read_unexpected,
						gsgl_mem_list_read_unexpected,
						gsgl_mem_list_read_unexpected,
						gsgl_mem_list_read_array_begin_polygon,
						gsgl_mem_list_read_array_end_polygon,
						gsgl_mem_list_read_array_begin_line,
						gsgl_mem_list_read_array_end_line,
						gsgl_mem_list_read_unexpected,
						gsgl_mem_list_read_unexpected,
						gsgl_mem_list_read_unexpected,
						gsgl_mem_list_read_unexpected,
						gsgl_mem_list_read_unexpected,
						gsgl_mem_list_read_unexpected,
						gsgl_mem_list_read_unexpected,
						gsgl_mem_list_read_unexpected,
						gsgl_mem_list_read_unexpected,
						gsgl_mem_list_read_unexpected,
						gsgl_mem_list_read_unexpected,
						gsgl_mem_list_read_unexpected,
						gsgl_mem_list_read_unexpected,
						gsgl_mem_list_read_clip_3d_fileonly,
						gsgl_mem_list_read_unexpected,
						gsgl_mem_list_read_unexpected,
						gsgl_mem_list_read_unexpected,
						gsgl_mem_list_read_unexpected,
						gsgl_mem_list_read_array_element_4byte,
						gsgl_mem_list_read_unexpected,
						gsgl_mem_list_read_unexpected,
						gsgl_mem_list_read_unexpected,
						gsgl_mem_list_read_unexpected,
						gsgl_mem_list_read_unexpected,
						gsgl_mem_list_read_unexpected,
						gsgl_mem_list_read_unexpected,
						gsgl_mem_list_read_unexpected,
						gsgl_mem_list_read_unexpected,
						gsgl_mem_list_read_begin_pageinfo,
						gsgl_mem_list_read_unexpected,
						gsgl_file_format_read_do_nothing,
						gsgl_file_format_read_do_nothing,
						gsgl_file_format_read_do_nothing,
						gsgl_mem_list_read_unexpected,
						gsgl_mem_list_read_unexpected,
						gsgl_mem_list_read_unexpected,
						gsgl_mem_list_read_enable_client_vertex,
						gsgl_mem_list_read_enable_client_color,
						gsgl_mem_list_read_unexpected,
						gsgl_mem_list_read_unexpected,
						gsgl_mem_list_read_unexpected,
						gsgl_mem_list_read_unexpected,
						gsgl_mem_list_read_unexpected,
						gsgl_mem_list_read_unexpected,
						gsgl_mem_list_read_unexpected,
						gsgl_mem_list_read_unexpected,
						gsgl_mem_list_read_unexpected,
						gsgl_mem_list_read_unexpected,
						gsgl_mem_list_read_unexpected,
						gsgl_mem_list_read_unexpected,
						gsgl_mem_list_read_unexpected,
						gsgl_mem_list_read_unexpected,
						gsgl_mem_list_read_unexpected,
						gsgl_mem_list_read_unexpected,
						gsgl_mem_list_read_unexpected,
						gsgl_mem_list_read_unexpected,
						gsgl_mem_list_read_unexpected,
						gsgl_mem_list_read_unexpected,
						gsgl_mem_list_read_unexpected,
						gsgl_mem_list_read_unexpected,
						gsgl_mem_list_read_unexpected,
						gsgl_mem_list_read_unexpected,
						gsgl_mem_list_read_unexpected,
						gsgl_mem_list_read_unexpected,
						gsgl_mem_list_read_unexpected,
						gsgl_mem_list_read_unexpected,
						gsgl_mem_list_read_unexpected,
						gsgl_mem_list_read_unexpected,
						gsgl_mem_list_read_unexpected,
						gsgl_mem_list_read_unexpected,
						gsgl_mem_list_read_unexpected,
						gsgl_mem_list_read_unexpected,
						gsgl_file_format_read_gui_button,
						gsgl_file_format_read_gui_combo,
						gsgl_file_format_read_gui_spinf,
						gsgl_file_format_read_gui_spini,
						gsgl_file_format_read_gui_check,
						gsgl_file_format_read_gui_edit,
						gsgl_file_format_read_gui_label,
						gsgl_file_format_read_gui_external_ignore,
						gsgl_mem_list_read_default);
	}
	read_array_count = data->read_array_count;
}

/*----------------------------------------------------------------------------*/
/*      Get a float from exponent part "tokens" and  "significand" token      */
/*----------------------------------------------------------------------------*/     
static unsigned int gsgl_mem_list_read_vertex (mem_list_data *data, gsglPage *p, int datatype)
{
	return gsgl_file_format_read_vertex_template (data, p, datatype, GSGL_FALSE,
						      read_token_mem, gsgl_file_format_read_float);
}
