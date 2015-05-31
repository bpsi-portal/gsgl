#include "gsaf.h"
#include "gsgl.h"
#include "fileio_base.h"
#include "fileio_read_base.h"
#include "fileio_read.h"
#include "fileformat.h"
#include "fileio_write.h"
#include "utils.h"
#include "unix.h"

static void gsglanime_message (int errflg, char *argv[]);
static void anime2pages (unsigned int size, unsigned char *buffer, gsglboolean extra, FILE *fp, gsglWriteToFile *w);

int main (int argc, char *argv[])
{
	int          c, errflg;
	int          istart, iend;
	int          interactive = 1;
	char         filename[256];
	char         *out = NULL;
	unsigned char *startpoint;
	int          found = 0;
	unsigned char *buffer;
	int          filesize=0;
	int          total, orig=0, phys=1;
	gsglPageInfo *Info;
	gsglFileInfo *fileinfo;
	FILE         *fp;
	gsglWriteToFile *w;
	int ny, nmo, nd, nh, nmi, ns;
	gsglboolean  extra = GSGL_FALSE;

	extern char *optarg;
	extern int optind, optopt;
	
	istart = 1;
	iend = 999;
	errflg = 0;
	
	while ((c = getopt(argc, argv, ":ae:o:s:x")) != -1)
		switch (c) {
		case 'a':
			istart = 0;
			interactive = 0;
			break;
		case 'e':
			if (istart == 0)
				istart = 1;
			iend = atoi(optarg);
			interactive = 0;
			break;
		case 's':
			istart = atoi(optarg);
			interactive = 0;
			break;
		case 'o':
			out = (char *) gsgl_malloc_atomic (sizeof (char) * strlen (optarg));
			strcpy(out, optarg);
			break;
		case 'x':
			extra = GSGL_TRUE;
			break;
		case ':':        /* -ens without arguments */
			fprintf(stderr, "Option -%c requires an argument\n", optopt);
			errflg++;
			break;
		case '?':
			fprintf(stderr, "Unrecognized option: - %c\n", optopt);
			errflg++;
		}
	
	/* error message */
	gsglanime_message (errflg, argv);

	fileinfo = gsglfread_getfileinfo (argc, argv, optind);

	buffer = fileinfo->buffer;
	Info = fileinfo->pageinfo;
	total = fileinfo->pagenum;
	filesize = fileinfo->filesize;
	strcpy (filename, fileinfo->filename);

	if (interactive) gsglfread_showpage (total, Info);

	w = gsgl_init_write_to_file ();
	if (!interactive) {
		fp = stdout;
	} else if (out != NULL) {
		if ((fp = fopen(out, "wb")) == NULL) {
			gsgl_error ("gsglanime2pages", "Could not open file %s", out);
			exit (1);
		}
	} else {
		gsaf_gudate (&ny, &nmo, &nd, &nh, &nmi, &ns);
		out = (char *) gsgl_malloc_atomic (sizeof (char) * (strlen (filename)+9));
		sprintf (out, "%s.%02d%02d%02d%02d", filename, nmo, nd, nh, nmi);
		if ((fp = fopen(out, "wb")) == NULL) {
			gsgl_error ("gsglanime2pages", "Could not open file %s", out);
			exit (1);
		}		
	}

	for (;;) {		
		found = gsglfread_getphysicalpage (total, &orig, &phys, Info, &istart, &iend, interactive);
		if (found == -1) break;
		for (phys=istart; phys<=iend; phys++){
			if (found) {
				startpoint = buffer+Info[phys].startpoint;
				anime2pages (filesize, startpoint, extra, fp, w);
			} else
				printf(" ## PAGE %3d NOT FOUND\n", orig);
		}
		if (!interactive) break;
		found = 0;
	}
	return 0;
}

static void gsglanime_message (int errflg, char *argv[])
{
	if (errflg) {
		fprintf (stderr, "usage: %s [-a] [-s ps] [-e pe] [filename]\n", argv[0]);
		fprintf (stderr, "           -a    : transform all page\n");
		fprintf (stderr, "           -s ps : transform from page ps [1]\n");
		fprintf (stderr, "           -e pe : transform until page pe [999]\n");
		fprintf (stderr, "           -x    : add an extra page as opening page\n");
		fprintf (stderr, "           filename : if not specified, prompted\n");
		exit (1);
	}
}

typedef struct _anime_data {
	FILE *fp;
	gsglWriteToFile *w;
	gsglReadFile *r;
	unsigned char *loc; 	
	gsglboolean in_anime;
	gsglboolean in_header;
	gsglboolean lighting;
	gsglboolean quit;
	int coord_mode;
	int projection;
	int indexcolor;
	int linewidth_cm;
	int linewidth_index;
	unsigned int pageno;
	unsigned int depth[2];
	unsigned int rgb[3];
	int clip_plane[6];
	unsigned int clip_eq[6][8];
	unsigned int clip_proj [6][32];
	unsigned int clip_model[6][32];
	unsigned int projection_matrix[32];
	unsigned int modelview_matrix[32];
} anime_data;

static void anime_vertex (anime_data *data, gsglPage *, int datatype);

#define GSGL_DEBUG(x)
#define TWOBYTE 65535

static void readwrite (void *data1, unsigned int *token)
{
	anime_data *data = data1;
	data->loc = gsglfread_token (data->loc, token, data->r);
	if (data->in_anime) gsglfwrite_write (*token, data->fp, data->w);
}

static void anime_read (anime_data *data, unsigned int *token)
{
	data->loc = gsglfread_token (data->loc, token, data->r);
} 

#define BUFFER_SIZE 10000 /* 20kB */

static unsigned int Count; 
static gsgllist *Buffer;

static void put_to_buffer (unsigned int token)
{
	gsgllist *buffer;
	unsigned short *data;

	if (Count == 0 || Count == BUFFER_SIZE) {
		data = (unsigned short *) gsgl_malloc_atomic (sizeof (unsigned short) * BUFFER_SIZE);
		Buffer = gsgl_list_append (Buffer, data);
	}
	if (Count == BUFFER_SIZE) Count = 0;

	buffer = gsgl_list_last(Buffer);
	data = buffer->data;
	data[Count] = (unsigned short) token;
	Count++;
}

static void write_out_buffer (FILE *fp, gsglWriteToFile *w)
{
	int i;
	gsgllist *buffer;
	unsigned short *data;
	
	/*fprintf (stderr, "%d\n", Count);*/

	buffer = Buffer;
	if (buffer == NULL) return;
	
	do {
		/*fprintf (stderr, "pointer = %d\n", (int) buffer);*/
		data = buffer->data;
		if (buffer->next == NULL) {
			for (i=0; i<Count; i++)
				gsglfwrite_write (data[i], fp, w);
		} else {
			for (i=0; i<BUFFER_SIZE; i++)
				gsglfwrite_write (data[i], fp, w);
		}
		buffer = buffer->next;
	} while (buffer != NULL);
}

static void anime_vertex2 (void *data1, gsglPage *p, READ_TOKEN)
{
	anime_data *data = data1;
	unsigned int token;

	if (data->in_anime) gsglfwrite_write (GSGL_FILE_VERTEX2, data->fp, data->w);
	if (data->in_header) put_to_buffer (GSGL_FILE_VERTEX2);
	readwrite (data, &token);
	if (data->in_header) put_to_buffer (token);
	readwrite (data, &token);
	if (data->in_header) put_to_buffer (token);
	readwrite (data, &token);
	if (data->in_header) put_to_buffer (token);
	readwrite (data, &token);
	if (data->in_header) put_to_buffer (token);
}

static void anime_vertex3 (void *data1, gsglPage *p, READ_TOKEN)
{
	anime_data *data = data1;
	unsigned int token;

	if (data->in_anime) gsglfwrite_write (GSGL_FILE_VERTEX3, data->fp, data->w);
	readwrite (data, &token);
	readwrite (data, &token);
	readwrite (data, &token);
	readwrite (data, &token);
	readwrite (data, &token);
	readwrite (data, &token);
}

static void anime_mode_normal (void *data1, gsglPage *p, READ_TOKEN)
{
	anime_data *data = data1;
	
	data->coord_mode = GSGL_FILE_MODE_NORMAL;
	if (data->in_anime) gsglfwrite_write (GSGL_FILE_MODE_NORMAL, data->fp, data->w);
	if (data->in_header) put_to_buffer (GSGL_FILE_MODE_NORMAL);
}

static void anime_mode_2d (void *data1, gsglPage *p, READ_TOKEN)
{
	anime_data *data = data1;

	data->coord_mode = GSGL_FILE_MODE_2D;
	if (data->in_anime) gsglfwrite_write (GSGL_FILE_MODE_2D, data->fp, data->w);
}

static void anime_mode_3d (void *data1, gsglPage *p, READ_TOKEN)
{
	anime_data *data = data1;

	data->coord_mode = GSGL_FILE_MODE_3D;
	if (data->in_anime) gsglfwrite_write (GSGL_FILE_MODE_3D, data->fp, data->w);
}

static void anime_separate_objects (void *data1, gsglPage *p, READ_TOKEN)
{
	anime_data *data = data1;
	if (data->in_anime) gsglfwrite_write (GSGL_FILE_SEPARATE_OBJECTS, data->fp, data->w);
	if (data->in_header) put_to_buffer (GSGL_FILE_SEPARATE_OBJECTS);
}

static void anime_array_begin_polygon (void *data1, gsglPage *p, READ_TOKEN)
{
	anime_data *data = data1;
	if (data->in_anime) gsglfwrite_write (GSGL_FILE_ARRAY_BEGIN_POLYGON, data->fp, data->w);
}

static void anime_array_end_polygon (void *data1, gsglPage *p, READ_TOKEN)
{
	anime_data *data = data1;
	if (data->in_anime) gsglfwrite_write (GSGL_FILE_ARRAY_END_POLYGON, data->fp, data->w);
}

static void anime_array_begin_line (void *data1, gsglPage *p, READ_TOKEN)
{
	anime_data *data = data1;
	if (data->in_anime) gsglfwrite_write (GSGL_FILE_ARRAY_BEGIN_LINE, data->fp, data->w);
}

static void anime_array_end_line (void *data1, gsglPage *p, READ_TOKEN)
{
	anime_data *data = data1;
	if (data->in_anime) gsglfwrite_write (GSGL_FILE_ARRAY_END_LINE, data->fp, data->w);
}

static void anime_begin_point (void *data1, gsglPage *p, READ_TOKEN)
{
	anime_data *data = data1;
	if (data->in_anime) gsglfwrite_write (GSGL_FILE_BEGIN_POINT, data->fp, data->w);
}

static void anime_end_point (void *data1, gsglPage *p, READ_TOKEN)
{
	anime_data *data = data1;
	if (data->in_anime) gsglfwrite_write (GSGL_FILE_END_POINT, data->fp, data->w);
}

static void anime_begin_line (void *data1, gsglPage *p, READ_TOKEN)
{
	anime_data *data = data1;
	if (data->in_anime) gsglfwrite_write (GSGL_FILE_BEGIN_LINE, data->fp, data->w);
	if (data->in_header) put_to_buffer (GSGL_FILE_BEGIN_LINE);
}

static void anime_end_line (void *data1, gsglPage *p, READ_TOKEN)
{
	anime_data *data = data1;
	if (data->in_anime) gsglfwrite_write (GSGL_FILE_END_LINE, data->fp, data->w);
	if (data->in_header) put_to_buffer (GSGL_FILE_END_LINE);
}

static void anime_begin_poly (void *data1, gsglPage *p, READ_TOKEN)
{
	anime_data *data = data1;
	if (data->in_anime) gsglfwrite_write (GSGL_FILE_BEGIN_POLY, data->fp, data->w);
}

static void anime_end_poly (void *data1, gsglPage *p, READ_TOKEN)
{
	anime_data *data = data1;
	if (data->in_anime) gsglfwrite_write (GSGL_FILE_END_POLY, data->fp, data->w);
}	

static void anime_normal (void *data1, gsglPage *p, READ_TOKEN)
{
	anime_data *data = data1;
	unsigned int token;

	if (data->in_anime) gsglfwrite_write (GSGL_FILE_NORMAL, data->fp, data->w);
	readwrite (data, &token);
	readwrite (data, &token);
	readwrite (data, &token);
	readwrite (data, &token);
	readwrite (data, &token);
	readwrite (data, &token);
}

static void anime_begin_triangles (void *data1, gsglPage *p, READ_TOKEN)
{
	anime_data *data = data1;
	if (data->in_anime) gsglfwrite_write (GSGL_FILE_BEGIN_TRIANGLES, data->fp, data->w);
}

static void anime_end_triangles (void *data1, gsglPage *p, READ_TOKEN)
{
	anime_data *data = data1;
	if (data->in_anime) gsglfwrite_write (GSGL_FILE_END_TRIANGLES, data->fp, data->w);
}

static void anime_begin_triangle_strip (void *data1, gsglPage *p, READ_TOKEN)
{
	anime_data *data = data1;
	if (data->in_anime) gsglfwrite_write (GSGL_FILE_BEGIN_TRIANGLES, data->fp, data->w);
}

static void anime_end_triangle_strip (void *data1, gsglPage *p, READ_TOKEN)
{
	anime_data *data = data1;
	if (data->in_anime) gsglfwrite_write (GSGL_FILE_END_TRIANGLES, data->fp, data->w);
}

static void anime_begin_triangle_fan (void *data1, gsglPage *p, READ_TOKEN)
{
	anime_data *data = data1;
	if (data->in_anime) gsglfwrite_write (GSGL_FILE_BEGIN_TRIANGLE_FAN, data->fp, data->w);
}

static void anime_end_triangle_fan (void *data1, gsglPage *p, READ_TOKEN)
{
	anime_data *data = data1;
	if (data->in_anime) gsglfwrite_write (GSGL_FILE_END_TRIANGLE_FAN, data->fp, data->w);
}

static void anime_clip_3d_fileonly (void *data1, gsglPage *p, READ_TOKEN)
{
	anime_data *data = data1;
	unsigned int token;
	unsigned int clip_plane_tmp;
	int i;

	if (data->in_anime) gsglfwrite_write (GSGL_FILE_CLIP_3D_FILEONLY, data->fp, data->w);
	anime_read (data, &token);
	if (token != GSGL_FILE_CLIP_NUM) return;
	if (data->in_anime) gsglfwrite_write (token, data->fp, data->w);
	anime_read (data, &clip_plane_tmp);
	data->clip_plane[clip_plane_tmp] = GSGL_FILE_CLIP_3D_FILEONLY;
	if (data->in_anime) gsglfwrite_write (clip_plane_tmp, data->fp, data->w);
	anime_read (data, &token);
	if (token != GSGL_FILE_CLIP_COEFF) return;
	if (data->in_anime) gsglfwrite_write (token, data->fp, data->w);
	for (i=0; i<4; i++) {
		/*loc = gsglfread_get_float (loc, &eqn[i]);*/
		anime_read (data, &token);
		data->clip_eq [clip_plane_tmp][2*i+0] = token;
		if (data->in_anime) gsglfwrite_write (token, data->fp, data->w);
		anime_read (data, &token);
		data->clip_eq [clip_plane_tmp][2*i+1] = token;
		if (data->in_anime) gsglfwrite_write (token, data->fp, data->w);
	}
	for (i=0; i<32; i++) {
		data->clip_proj [clip_plane_tmp][i] = data->projection_matrix[i];
		data->clip_model[clip_plane_tmp][i] = data->modelview_matrix[i];
	}
}

static void anime_clip_unmovable (void *data1, gsglPage *p, READ_TOKEN)
{
	anime_data *data = data1;
	unsigned int token;
	unsigned int clip_plane_tmp;
	int i;

	if (data->in_anime) gsglfwrite_write (GSGL_FILE_CLIP_UNMOVABLE, data->fp, data->w);
	anime_read (data, &token);
	if (token != GSGL_FILE_CLIP_NUM) return;
	if (data->in_anime) gsglfwrite_write (token, data->fp, data->w);
	anime_read (data, &clip_plane_tmp);
	data->clip_plane [clip_plane_tmp] = GSGL_FILE_CLIP_UNMOVABLE;
	if (data->in_anime) gsglfwrite_write (clip_plane_tmp, data->fp, data->w);
	anime_read (data, &token);
	if (token != GSGL_FILE_CLIP_COEFF) return;
	if (data->in_anime) gsglfwrite_write (token, data->fp, data->w);
	for (i=0; i<4; i++) {
		/*loc = gsglfread_get_float (loc, &eqn[i]);*/
		readwrite (data, &token);
		data->clip_eq [clip_plane_tmp][2*i+0] = token;			
		readwrite (data, &token);
		data->clip_eq [clip_plane_tmp][2*i+1] = token;
	}
	for (i=0; i<32; i++) {
		data->clip_proj [clip_plane_tmp][i] = data->projection_matrix[i];
		data->clip_model[clip_plane_tmp][i] = data->modelview_matrix[i];
	}
}

static void anime_del_clip (void *data1, gsglPage *p, READ_TOKEN)
{
	anime_data *data = data1;
	unsigned int token;

	if (data->in_anime) gsglfwrite_write (GSGL_FILE_DEL_CLIP_PLANE, data->fp, data->w);
	anime_read (data, &token);
	if (token != GSGL_FILE_DEL_CLIP_NUM) return;
	if (data->in_anime) gsglfwrite_write (token, data->fp, data->w);
	readwrite (data, &token);
	data->clip_plane [token] = -1;
}

static void anime_modelview (void *data1, gsglPage *p, READ_TOKEN)
{
	anime_data *data = data1;
	unsigned int token;
	int i;

	if (data->in_anime) gsglfwrite_write (GSGL_FILE_MODELVIEW, data->fp, data->w);
	if (data->in_header) put_to_buffer (GSGL_FILE_MODELVIEW);
	for (i=0; i<16 ; i++) {
		readwrite (data, &token);
		if (data->in_header) put_to_buffer (token);
		data->modelview_matrix [i*2+0] = token;
		readwrite (data, &token);
		if (data->in_header) put_to_buffer (token);
		data->modelview_matrix [i*2+1] = token;
		/*loc = gsglfread_get_float (loc, &modelview_matrix[i]);*/
	}
}

static void anime_rgb (void *data1, gsglPage *p, READ_TOKEN)
{
	anime_data *data = data1;
	unsigned int token;

	if (data->in_anime) gsglfwrite_write (GSGL_FILE_RGB, data->fp, data->w);
	readwrite (data, &token);
	data->rgb[0] = token;
	readwrite (data, &token);
	data->rgb[1] = token;
	readwrite (data, &token);
	data->rgb[2] = token;
	data->indexcolor = -1;
}

static void anime_array_element_4byte (void *data1, gsglPage *p, READ_TOKEN)
{
	anime_data *data = data1;
	unsigned int token;

	if (data->in_anime) gsglfwrite_write (GSGL_FILE_ARRAY_ELEMENT_4BYTE, data->fp, data->w);
	readwrite (data, &token);
	readwrite (data, &token);
}

static void anime_rgba (void *data1, gsglPage *p, READ_TOKEN)
{
	anime_data *data = data1;
	unsigned int token;

	if (data->in_anime) gsglfwrite_write (GSGL_FILE_RGBA, data->fp, data->w);
	readwrite (data, &token);
	data->rgb[0] = token;
	readwrite (data, &token);
	data->rgb[1] = token;
	readwrite (data, &token);
	data->rgb[2] = token;
	anime_read (data, &token);
	data->indexcolor = -1;
	/*rgba[3] = gsgl_convert_token_to_0to1 (token2);*/
}

static void anime_indexcolor (void *data1, gsglPage *p, READ_TOKEN)
{
	anime_data *data = data1;
	unsigned int token;

	if (data->in_anime) gsglfwrite_write (GSGL_FILE_INDEXCOLOR, data->fp, data->w);
	if (data->in_header) put_to_buffer (GSGL_FILE_INDEXCOLOR);
	readwrite (data, &token);
	if (data->in_header) put_to_buffer (token);
	data->indexcolor = token;
}

static void anime_linewidth (void *data1, gsglPage *p, READ_TOKEN)
{
	anime_data *data = data1;
	unsigned int token;

	if (data->in_anime) gsglfwrite_write (GSGL_FILE_LINEWIDTH, data->fp, data->w);
	if (data->in_header) put_to_buffer (GSGL_FILE_LINEWIDTH);
	readwrite (data, &token);
	if (data->in_header) put_to_buffer (token);
	data->linewidth_cm = token;
	data->linewidth_index = -1;
}

static void anime_linewidth2 (void *data1, gsglPage *p, READ_TOKEN)
{
	anime_data *data = data1;
	unsigned int token;

	if (data->in_anime) gsglfwrite_write (GSGL_FILE_LINEWIDTH2, data->fp, data->w);
	if (data->in_header) put_to_buffer (GSGL_FILE_LINEWIDTH2);
	readwrite (data, &token);
	if (data->in_header) put_to_buffer (token);
	data->linewidth_cm = -1;
	data->linewidth_index = token;
}

static void anime_projection (void *data1, gsglPage *p, READ_TOKEN)
{
	int i;
	anime_data *data = data1;
	unsigned int token;

	if (data->in_anime) gsglfwrite_write (GSGL_FILE_PROJECTION, data->fp, data->w);
	if (data->in_header) put_to_buffer (GSGL_FILE_PROJECTION);
	for (i=0; i<16 ; i++) {
		readwrite (data, &token);
		if (data->in_header) put_to_buffer (token);
		data->projection_matrix[i*2+0] = token;
		readwrite (data, &token);
		if (data->in_header) put_to_buffer (token);
		data->projection_matrix[i*2+1] = token;
	}
}

static void anime_3d_3 (void *data1, gsglPage *p, READ_TOKEN)
{
	anime_data *data = data1;
	unsigned int token;

	if (data->in_anime) gsglfwrite_write (GSGL_FILE_3D_3, data->fp, data->w);
	anime_read (data, &token);
	if (token != GSGL_FILE_3D_DEPTH) return;
	if (data->in_anime) gsglfwrite_write (token, data->fp, data->w);
	readwrite (data, &token);
	data->depth[0] = token;
	readwrite (data, &token);
	data->depth[1] = token;
}

static void anime_projection_type (void *data1, gsglPage *p, READ_TOKEN)
{
	anime_data *data = data1;
	unsigned int token;

	if (data->in_anime) gsglfwrite_write (GSGL_FILE_PROJECTIONTYPE, data->fp, data->w);
	readwrite (data, &token);
	data->projection = token;
}

static void anime_enable_lighting (void *data1, gsglPage *p, READ_TOKEN)
{
	anime_data *data = data1;
	unsigned int token;

	data->lighting = GSGL_TRUE;
	if (data->in_anime) gsglfwrite_write (GSGL_FILE_ENABLE_LIGHTING, data->fp, data->w);
	anime_read (data, &token);
	if (token != GSGL_FILE_LIGHTING_ENABLE_TYPE) return;
	if (data->in_anime) gsglfwrite_write (token, data->fp, data->w);
	anime_read (data, &token);
	if (token != 1) return;
	if (data->in_anime) gsglfwrite_write (token, data->fp, data->w);
	anime_read (data, &token);
	if (token != GSGL_FILE_LIGHTING_ENABLE_NUM) return;
	if (data->in_anime) gsglfwrite_write (token, data->fp, data->w);
	readwrite (data, &token);
}

static void anime_disable_lighting (void *data1, gsglPage *p, READ_TOKEN)
{
	anime_data *data = data1;
	unsigned int token;

	data->lighting = GSGL_FALSE;
	if (data->in_anime) gsglfwrite_write (GSGL_FILE_DISABLE_LIGHTING, data->fp, data->w);
	anime_read (data, &token);
	if (token != GSGL_FILE_LIGHTING_DISABLE_NUM) return;
	if (data->in_anime) gsglfwrite_write (token, data->fp, data->w);
	readwrite (data, &token);
}

static void anime_begin_pageinfo (void *data1, gsglPage *p, READ_TOKEN)
{
	anime_data *data = data1;
	unsigned int token;

	anime_read (data, &token); /* GSGL_FILE_PAGE_NUMBER */
	anime_read (data, &token); /* page number */
	anime_read (data, &token); /* GSGL_FILE_PAGE_SIZE */
	anime_read (data, &token); /* filesize */
	anime_read (data, &token); /* filesize */
}

static void anime_do_nothing (void *data1, gsglPage *p, READ_TOKEN)
{
}

static void anime_begin_head (void *data1, gsglPage *p, READ_TOKEN)
{
	anime_data *data = data1;

	data->in_header = GSGL_TRUE;
}

static void anime_end_head (void *data1, gsglPage *p, READ_TOKEN)
{
	anime_data *data = data1;

	data->in_header = GSGL_FALSE;
}

static void anime_begin_anime (void *data1, gsglPage *p, READ_TOKEN)
{
	anime_data *data = data1;
	int i, j;

	data->in_anime = GSGL_TRUE;
	fprintf (data->fp, "GSAF/OpenGL0");
	data->w->byte = 12;
	gsglfwrite_write (GSGL_FILE_BEGIN_PAGEINFO, data->fp, data->w);
	gsglfwrite_write (GSGL_FILE_PAGE_NUMBER, data->fp, data->w);
	data->pageno++;
	gsglfwrite_write (data->pageno, data->fp, data->w);
	gsglfwrite_write (GSGL_FILE_PAGE_SIZE, data->fp, data->w);
	gsglfwrite_write (0, data->fp, data->w);
	gsglfwrite_write (0, data->fp, data->w);
	gsglfwrite_write (GSGL_FILE_END_PAGEINFO, data->fp, data->w);
	gsglfwrite_write (GSGL_FILE_BEGIN_HEAD, data->fp, data->w);
	
	write_out_buffer (data->fp, data->w);
	
	gsglfwrite_write (GSGL_FILE_END_HEAD, data->fp, data->w);
	gsglfwrite_write (GSGL_FILE_BEGIN_PAGE, data->fp, data->w);
	
	if (data->indexcolor > 0) {
		if (data->in_anime) gsglfwrite_write (GSGL_FILE_INDEXCOLOR, data->fp, data->w);
		if (data->in_anime) gsglfwrite_write (data->indexcolor, data->fp, data->w);
	} else {
		if (data->in_anime) gsglfwrite_write (GSGL_FILE_RGB, data->fp, data->w);
		if (data->in_anime) gsglfwrite_write (data->rgb[0], data->fp, data->w);
		if (data->in_anime) gsglfwrite_write (data->rgb[1], data->fp, data->w);
		if (data->in_anime) gsglfwrite_write (data->rgb[2], data->fp, data->w);
	}
	
	if (data->linewidth_index > 0) {
		if (data->in_anime) gsglfwrite_write (GSGL_FILE_LINEWIDTH2, data->fp, data->w);
		if (data->in_anime) gsglfwrite_write (data->linewidth_index, data->fp, data->w);
	} else if (data->linewidth_cm > 0) {
		if (data->in_anime) gsglfwrite_write (GSGL_FILE_LINEWIDTH, data->fp, data->w);
		if (data->in_anime) gsglfwrite_write (data->linewidth_cm, data->fp, data->w);				
	}
	
	for (i=0; i<6; i++) {
		if (data->clip_plane[i] > 0) {
			if (data->in_anime) gsglfwrite_write (GSGL_FILE_PROJECTION, data->fp, data->w);
			for (j=0; j<16; j++) {
				if (data->in_anime) gsglfwrite_write (data->clip_proj [i][j], data->fp, data->w);
			}
			if (data->in_anime) gsglfwrite_write (GSGL_FILE_MODELVIEW, data->fp, data->w);
			for (j=0; j<16; j++) {
				if (data->in_anime) gsglfwrite_write (data->clip_model[i][j], data->fp, data->w);
			}
			
			if (data->in_anime) gsglfwrite_write (data->clip_plane[i], data->fp, data->w);
			if (data->in_anime) gsglfwrite_write (GSGL_FILE_CLIP_NUM, data->fp, data->w);
			if (data->in_anime) gsglfwrite_write (i, data->fp, data->w);
			if (data->in_anime) gsglfwrite_write (GSGL_FILE_CLIP_COEFF, data->fp, data->w);
			for (j=0; j<8; j++) {
				if (data->in_anime) gsglfwrite_write (data->clip_eq [i][j], data->fp, data->w);
			}
			
		}
	}
	
	if (data->in_anime) gsglfwrite_write (GSGL_FILE_PROJECTIONTYPE, data->fp, data->w);
	if (data->in_anime) gsglfwrite_write (data->projection, data->fp, data->w);
	
	if (data->in_anime) gsglfwrite_write (GSGL_FILE_PROJECTION, data->fp, data->w);
	for (i=0; i<16; i++) {
		if (data->in_anime) gsglfwrite_write (data->projection_matrix[i*2+0], data->fp, data->w);
		if (data->in_anime) gsglfwrite_write (data->projection_matrix[i*2+1], data->fp, data->w);
	}
	
	if (data->in_anime) gsglfwrite_write (GSGL_FILE_MODELVIEW, data->fp, data->w);
	for (i=0; i<16; i++) {
		if (data->in_anime) gsglfwrite_write (data->modelview_matrix[i*2+0], data->fp, data->w);
		if (data->in_anime) gsglfwrite_write (data->modelview_matrix[i*2+1], data->fp, data->w);
	}
	
	if (data->in_anime) gsglfwrite_write (data->coord_mode, data->fp, data->w);
	
	if (data->depth[0] != 0 || data->depth[1] != 0) {
		if (data->in_anime) gsglfwrite_write (GSGL_FILE_3D_3, data->fp, data->w);
		if (data->in_anime) gsglfwrite_write (GSGL_FILE_3D_DEPTH, data->fp, data->w);
		if (data->in_anime) gsglfwrite_write (data->depth[0], data->fp, data->w);
		if (data->in_anime) gsglfwrite_write (data->depth[1], data->fp, data->w);
	}
	
	if (data->lighting) {
		if (data->in_anime) gsglfwrite_write (GSGL_FILE_ENABLE_LIGHTING, data->fp, data->w);
		if (data->in_anime) gsglfwrite_write (GSGL_FILE_LIGHTING_ENABLE_TYPE, data->fp, data->w);
		if (data->in_anime) gsglfwrite_write (1, data->fp, data->w);
		if (data->in_anime) gsglfwrite_write (GSGL_FILE_LIGHTING_ENABLE_NUM, data->fp, data->w);
		if (data->in_anime) gsglfwrite_write (0, data->fp, data->w);
	}
}

static void anime_end_anime (void *data1, gsglPage *p, READ_TOKEN)
{
	anime_data *data = data1;
	unsigned int fsize[2];

	data->in_anime = GSGL_FALSE;
	gsglfwrite_write (GSGL_FILE_END_PAGE, data->fp, data->w);
	if (data->w->surplus != 0) gsglfwrite_write (0, data->fp, data->w);
	if (data->w->surplus != 0) gsglfwrite_write (0, data->fp, data->w);
	
	/* write filesize next to the page number */
	fseek (data->fp, -((long) (data->w->byte))+8+12, SEEK_CUR);
	/* 12byte are  "GSAF/OpenGL0", 8bytes are from ther to the pagenumber */
	fsize[1] = (unsigned int) (data->w->byte/(256*256));
	fsize[0] = data->w->byte - 256 * 256 * fsize[1];
	gsglfwrite_write (GSGL_FILE_PAGE_SIZE, data->fp, data->w);
	gsglfwrite_write (fsize[0], data->fp, data->w);
	gsglfwrite_write (fsize[1], data->fp, data->w);
	fseek (data->fp, 0, SEEK_END);
	data->w->byte -= 8;
}

static void anime_enable_client_vertex (void *data1, gsglPage *p, READ_TOKEN)
{
	anime_data *data = data1;
	unsigned int token;

	if (data->in_anime) gsglfwrite_write (GSGL_FILE_ENABLE_CLIENT_VERTEX, data->fp, data->w);
	anime_read (data, &token);
	if (token == GSGL_FILE_CLIENT_VERTEX_TYPE) {
		if (data->in_anime) gsglfwrite_write (token, data->fp, data->w);
		readwrite (data, &token);
		anime_vertex (data, p, token);
	}
}

static void anime_enable_client_color (void *data1, gsglPage *p, READ_TOKEN)
{
	anime_data *data = data1;
	unsigned int token, token1, token2;
	unsigned int totalsize = 0;
	int i;

	if (data->in_anime) gsglfwrite_write (GSGL_FILE_ENABLE_CLIENT_COLOR, data->fp, data->w);
	readwrite (data, &token);
	/*if (token == GSGL_FILE_CLIENT_COLOR_SIZE) {*/
	readwrite (data, &token);
	anime_read (data, &token);
	if (token == GSGL_FILE_CLIENT_COLOR_TOTALSIZE_2BYTE) {
		if (data->in_anime) gsglfwrite_write (token, data->fp, data->w);
		readwrite (data, &token);
		totalsize = token;
	} else if (token == GSGL_FILE_CLIENT_COLOR_TOTALSIZE_4BYTE) {
		if (data->in_anime) gsglfwrite_write (token, data->fp, data->w);
		readwrite (data, &token1);
		readwrite (data, &token2);
		totalsize = gsgl_convert_tokens_to_4byte (token1, token2);
	}
	for (i=0; i<totalsize; i++) {
		readwrite (data, &token);
	}
}

static void anime_begin_gouraud_only (void *data1, gsglPage *p, READ_TOKEN)
{
	anime_data *data = data1;
	if (data->in_anime) gsglfwrite_write (GSGL_FILE_PS_BEGIN_GOURAUD_ONLY, data->fp, data->w);
}

static void anime_end_gouraud_only (void *data1, gsglPage *p, READ_TOKEN)
{
	anime_data *data = data1;
	if (data->in_anime) gsglfwrite_write (GSGL_FILE_PS_END_GOURAUD_ONLY, data->fp, data->w);
}

static void anime_depth_by_deep (void *data1, gsglPage *p, READ_TOKEN)
{
	anime_data *data = data1;
	if (data->in_anime) gsglfwrite_write (GSGL_FILE_PS_DEPTH_BY_DEEP, data->fp, data->w);
}

static void anime_depth_by_shallow (void *data1, gsglPage *p, READ_TOKEN)
{
	anime_data *data = data1;
	if (data->in_anime) gsglfwrite_write (GSGL_FILE_PS_DEPTH_BY_SHALLOW, data->fp, data->w);
}

static void anime_depth_by_mean (void *data1, gsglPage *p, READ_TOKEN)
{
	anime_data *data = data1;
	if (data->in_anime) gsglfwrite_write (GSGL_FILE_PS_DEPTH_BY_MEAN, data->fp, data->w);
}

static void anime_depth_by_point (void *data1, gsglPage *p, READ_TOKEN)
{
	anime_data *data = data1;
	unsigned int token;

	if (data->in_anime) gsglfwrite_write (GSGL_FILE_PS_DEPTH_BY_POINT, data->fp, data->w);
	/*loc = gsglfread_get_float (loc, &point.x);*/
	readwrite (data, &token);
	readwrite (data, &token);
	/*loc = gsglfread_get_float (loc, &point.y);*/
	readwrite (data, &token);
	readwrite (data, &token);
	/*loc = gsglfread_get_float (loc, &point.z);*/
	readwrite (data, &token);
	readwrite (data, &token);
}

static void anime_depth_by_vec_prod (void *data1, gsglPage *p, READ_TOKEN)
{
	anime_data *data = data1;
	unsigned int token;
	if (data->in_anime) gsglfwrite_write (GSGL_FILE_PS_DEPTH_BY_VECT_PROD, data->fp, data->w);
	/*loc = gsglfread_get_float (loc, &x[0]);*/
	readwrite (data, &token);
	readwrite (data, &token);
	/*loc = gsglfread_get_float (loc, &y[0]);*/
	readwrite (data, &token);
	readwrite (data, &token);
	/*loc = gsglfread_get_float (loc, &z[0]);*/
	readwrite (data, &token);
	readwrite (data, &token);
	/*loc = gsglfread_get_float (loc, &x[1]);*/
	readwrite (data, &token);
	readwrite (data, &token);
	/*loc = gsglfread_get_float (loc, &y[1]);*/
	readwrite (data, &token);
	readwrite (data, &token);
	/*loc = gsglfread_get_float (loc, &z[1]);*/
	readwrite (data, &token);
	readwrite (data, &token);
	/*loc = gsglfread_get_float (loc, &x[2]);*/
	readwrite (data, &token);
	readwrite (data, &token);
	/*loc = gsglfread_get_float (loc, &y[2]);*/
	readwrite (data, &token);
	readwrite (data, &token);
	/*loc = gsglfread_get_float (loc, &z[2]);*/
	readwrite (data, &token);
	readwrite (data, &token);
}

static void anime_depth_by_vec_prod2 (void *data1, gsglPage *p, READ_TOKEN)
{
	int i;
	anime_data *data = data1;
	unsigned int token;

	if (data->in_anime) gsglfwrite_write (GSGL_FILE_PS_DEPTH_BY_VECT_PROD2, data->fp, data->w);
	/*loc = gsglfread_get_float (loc, &x[0]);*/
	readwrite (data, &token);
	readwrite (data, &token);
	/*loc = gsglfread_get_float (loc, &y[0]);*/
	readwrite (data, &token);
	readwrite (data, &token);
	/*loc = gsglfread_get_float (loc, &z[0]);*/
	readwrite (data, &token);
	readwrite (data, &token);
	/*loc = gsglfread_get_float (loc, &x[1]);*/
	readwrite (data, &token);
	readwrite (data, &token);
	/*loc = gsglfread_get_float (loc, &y[1]);*/
	readwrite (data, &token);
	readwrite (data, &token);
	/*loc = gsglfread_get_float (loc, &z[1]);*/
	readwrite (data, &token);
	readwrite (data, &token);
	/*loc = gsglfread_get_float (loc, &x[2]);*/
	readwrite (data, &token);
	readwrite (data, &token);
	/*loc = gsglfread_get_float (loc, &y[2]);*/
	readwrite (data, &token);
	readwrite (data, &token);
	/*loc = gsglfread_get_float (loc, &z[2]);*/
	readwrite (data, &token);
	readwrite (data, &token);
	/*read_token (data, &npoints);*/
	readwrite (data, &token);
	for (i=0; i<token; i++) {
		/*read_float (data, &xp[i], read_token);*/
		readwrite (data, &token);
		readwrite (data, &token);
		/*read_float (data, &yp[i], read_token);*/
		readwrite (data, &token);
		readwrite (data, &token);
		/*read_float (data, &zp[i], read_token);*/
		readwrite (data, &token);
		readwrite (data, &token);
	}
}

static void anime_end_page (void *data1, gsglPage *p, READ_TOKEN)
{
	anime_data *data = data1;

	data->quit = GSGL_TRUE;
}

static void anime_default (unsigned int token, void *data1, gsglPage *p, READ_TOKEN)
{
	gsgl_error ("anime2pages", "Unexpected token = %d", token);
}

static void anime_unexpected (void *data1, gsglPage *p, READ_TOKEN)
{
	gsgl_error ("anime2pages", "Unexpected token");
}

static unsigned int anime_inq_token (void *data1, gsglPage *p, READ_TOKEN)
{
	anime_data *data = data1;
	unsigned int token;

	anime_read (data, &token);
	GSGL_DEBUG(fprintf (stderr, "%d\n", token));
	return token;
}

static void anime_pstext (void *data1, gsglPage *p, READ_TOKEN)
{
	anime_data *data = data1;

	unsigned int token;
	int i, nmax;

	if (data->in_anime) gsglfwrite_write (GSGL_FILE_PS_TEXT, data->fp, data->w);
	readwrite (data1, &token); /* GSGL_FILE_PS_TEXT_FNT */
	readwrite (data1, &token); /* ifnts */
	readwrite (data1, &token); /* GSGL_FILE_PS_TEXT_FNT_DISPLAY */
	readwrite (data1, &token); /* ifntr */
	readwrite (data1, &token); /* GSGL_FILE_PS_TEXT_CHARACTER_HEIGHT */
	readwrite (data1, &token);
	readwrite (data1, &token);
	readwrite (data1, &token); /* GSGL_FILE_PS_TEXT_CHARACTER_WIDTH */
	readwrite (data1, &token);
	readwrite (data1, &token);
	readwrite (data1, &token); /* GSGL_FILE_PS_TEXT_CHARACTER_SPACE */
	readwrite (data1, &token);
	readwrite (data1, &token);
	readwrite (data1, &token); /* GSGL_FILE_PS_TEXT_CHARACTER_ANGLE */
	readwrite (data1, &token);
	readwrite (data1, &token);
	readwrite (data1, &token); /* GSGL_FILE_PS_TEXT_CHARACTER_TILT */
	readwrite (data1, &token);
	readwrite (data1, &token);
	readwrite (data1, &token); /* GSGL_FILE_PS_TEXT_X */
	readwrite (data1, &token);
	readwrite (data1, &token);
	readwrite (data1, &token); /* GSGL_FILE_PS_TEXT_Y */
	readwrite (data1, &token);
	readwrite (data1, &token);
	readwrite (data1, &token); /* GSGL_FILE_PS_TEXT_DIM */
	readwrite (data1, &token); /* dim */
	readwrite (data1, &token); /* GSGL_FILE_PS_TEXT_CHARACTER_NUM */
	readwrite (data1, &token); nmax = token;
	readwrite (data1, &token); /* GSGL_FILE_PS_TEXT_TEXT */
	for (i=0; i<nmax; i++) {
		readwrite (data1, &token);
	}
}

/******************************************************/
/*                   Read a page                     */
/******************************************************/
static void anime2pages (unsigned int size, unsigned char *buffer, gsglboolean extra, FILE *fp, gsglWriteToFile *w)
{
	unsigned char *loc, *end;
	unsigned int identity [32] = {0, TWOBYTE, 0, 0, 0, 0, 0, 0,
				      0, 0, 0, TWOBYTE, 0, 0, 0, 0,
				      0, 0, 0, 0, 0, TWOBYTE, 0, 0,
				      0, 0, 0, 0, 0, 0, 0, TWOBYTE};
	unsigned int  fsize[2];
	int i;
	anime_data *data;

	/*  Initialize */
	Count = 0;
	Buffer = NULL;

	data = (anime_data *) gsgl_malloc (sizeof (anime_data));
	data->r = gsgl_init_read_file ();
	data->in_anime = GSGL_FALSE;
	data->in_header = GSGL_FALSE;
	data->lighting = GSGL_FALSE;
	data->quit = GSGL_FALSE;
	data->fp = fp;
	data->w = w;

	data->coord_mode = GSGL_FILE_MODE_NORMAL;
	data->projection = DEFAULT_PROJECTIONTYPE; 
	data->indexcolor = DEFAULT_COLOR;
	data->linewidth_cm = -1;
	data->linewidth_index = DEFAULT_LINEWIDTH;
	data->pageno = 0;
	data->depth[0] = 0;
	data->depth[1] = 0;
	for (i=0; i<3; i++)
		data->rgb[i] = 0;
	for (i=0; i<6; i++)
		data->clip_plane[i] = -1;
	for (i=0; i<32; i++)
		data->projection_matrix[i] = identity[i];
	for (i=0; i<32; i++)
		data->modelview_matrix[i] = identity[i];

	data->loc = buffer;
	loc = buffer;
	end = loc + size;
	
	if (loc[0] != 'G' || loc[1] != 'S' || loc[2] != 'A' || loc[3] != 'F' ||
	    loc[4] != '/' || loc[5] != 'O' || loc[6] != 'p' || loc[7] != 'e' || 
	    loc[8] != 'n' || loc[9] != 'G' || loc[10] != 'L' || loc[11] != '0') {
		printf("Incorrect signature. Page must begin with \"GSAF/OpenGL0\".\n");
		exit(2);
	}
	
	data->loc += 12;
	if (extra) {
		fprintf (data->fp, "GSAF/OpenGL0");
		data->w->byte = 12;
		gsglfwrite_write (GSGL_FILE_BEGIN_PAGEINFO, data->fp, data->w);
		gsglfwrite_write (GSGL_FILE_PAGE_NUMBER, data->fp, data->w);
		data->pageno++;
		gsglfwrite_write (data->pageno, data->fp, data->w);
		gsglfwrite_write (GSGL_FILE_PAGE_SIZE, data->fp, data->w);
		gsglfwrite_write (0, data->fp, data->w);
		gsglfwrite_write (0, data->fp, data->w);
		gsglfwrite_write (GSGL_FILE_END_PAGEINFO, data->fp, data->w);
		gsglfwrite_write (GSGL_FILE_BEGIN_HEAD, data->fp, data->w);
		gsglfwrite_write (GSGL_FILE_END_HEAD, data->fp, data->w);
		gsglfwrite_write (GSGL_FILE_BEGIN_PAGE, data->fp, data->w);

		gsglfwrite_write (GSGL_FILE_END_PAGE, data->fp, data->w);
		if (data->w->surplus != 0) gsglfwrite_write (0, data->fp, data->w);
		if (data->w->surplus != 0) gsglfwrite_write (0, data->fp, data->w);

		/* write filesize next to the page number */
		fseek (data->fp, -((long) (w->byte))+8+12, SEEK_CUR);
		/* 12 are "GSAF/OpenGL0", 8 are from there to the pagenumber */
		fsize[1] = (unsigned int) (w->byte/(256*256));
		fsize[0] = data->w->byte - 256 * 256 * fsize[1];
		gsglfwrite_write (GSGL_FILE_PAGE_SIZE, data->fp, data->w);
		gsglfwrite_write (fsize[0], data->fp, data->w);
		gsglfwrite_write (fsize[1], data->fp, data->w);
		fseek (data->fp, 0, SEEK_END);
		data->w->byte -= 8;
	}
	
	while (data->loc < end || data->r->surplus == 2) {
		GSGL_DEBUG(fprintf (stderr, "%d %d\n", (int)data->loc, (int)end));
		GSGL_DEBUG(if (data->loc >= end) fprintf (stderr, "extra token"));
		if (data->quit) return;
		gsgl_file_format_read_template (NULL,
						data,
						readwrite,
						anime_inq_token, 
						anime_vertex2,
						anime_vertex3,
						anime_mode_normal,
						anime_mode_2d,
						anime_mode_3d,
						anime_separate_objects,
						anime_array_begin_polygon,
						anime_array_end_polygon,
						anime_array_begin_line,
						anime_array_end_line,
						anime_begin_point,
						anime_end_point,
						anime_begin_line,
						anime_end_line,
						anime_begin_poly,
						anime_end_poly,
						anime_normal,
						anime_begin_triangles,
						anime_end_triangles,
						anime_begin_triangle_strip,
						anime_end_triangle_strip,
						anime_begin_triangle_fan,
						anime_end_triangle_fan,
						anime_clip_3d_fileonly,
						anime_clip_unmovable,
						anime_del_clip,
						anime_modelview,
						anime_rgb,
						anime_array_element_4byte,
						anime_rgba,
						anime_indexcolor,
						anime_linewidth,
						anime_linewidth2,
						anime_projection,
						anime_3d_3,
						anime_projection_type,
						anime_enable_lighting,
						anime_disable_lighting,
						anime_begin_pageinfo,
						anime_do_nothing,
						anime_begin_head,
						anime_end_head,
						anime_do_nothing,
						anime_end_page,
						anime_begin_anime,
						anime_end_anime,
						anime_enable_client_vertex,
						anime_enable_client_color,
						anime_begin_gouraud_only,
						anime_end_gouraud_only,
						anime_depth_by_deep,
						anime_depth_by_shallow,
						anime_depth_by_mean,
						anime_depth_by_point,
						anime_depth_by_vec_prod,
						anime_depth_by_vec_prod2,
						anime_unexpected,
						anime_unexpected,
						anime_unexpected,
						anime_unexpected,
						anime_unexpected,
						anime_unexpected,
						anime_unexpected,
						anime_unexpected,
						anime_unexpected,
						anime_unexpected,
						anime_unexpected,
						anime_unexpected,
						anime_unexpected,
						anime_unexpected,
						anime_unexpected,
						anime_unexpected,
						anime_unexpected,
						anime_unexpected,
						anime_unexpected,
						anime_unexpected,
						anime_unexpected,
						anime_unexpected,
						anime_unexpected,
						anime_unexpected,
						anime_pstext,
						anime_unexpected,
						gsgl_file_format_read_gui_button,
						gsgl_file_format_read_gui_combo,
						gsgl_file_format_read_gui_spinf,
						gsgl_file_format_read_gui_spini,
						gsgl_file_format_read_gui_check,
						gsgl_file_format_read_gui_edit,
						gsgl_file_format_read_gui_label,
						gsgl_file_format_read_gui_external_ignore,
						anime_default);
	}
}

static void anime_float (void *data1, float *dummy, READ_TOKEN)
{
	anime_data *data = data1;
	unsigned int token;

	readwrite (data, &token);
	readwrite (data, &token);
	*dummy = 0.0;
}

static void temp_read (void *data1, unsigned int *token)
{
	anime_data *data = data1;
	readwrite (data, token);
}

static void anime_vertex (anime_data *data, gsglPage *p, int datatype)
{
	gsgl_file_format_read_vertex_template (data, p, datatype, GSGL_FALSE,
					       temp_read, anime_float);
}
