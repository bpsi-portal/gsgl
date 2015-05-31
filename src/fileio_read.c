#include "gsaf.h"
#include "gsgl.h"
#include "xwin.h"
#include "fileio_base.h"
#include "fileio_read_base.h"
#include "fileio_read.h"
#include "fileformat.h"
#include "ogl.h"
#include "displaylist.h"
#include "matrix.h"
#include "clientstate.h"
#include "anime.h"
#include "base.h"
#include "drv_wrapper.h"
#include "drv_gl.h"
#include "utils.h"
#include "mem_list_read.h"
#include "iof.h"
#include "bsc1.h"
#include <zlib.h>

#define BUFFERSIZE  1000000

typedef struct _read_data
{
	unsigned char *loc;
	gsglReadFile *r;
	gsglboolean ignore_header;
	gsglboolean get_normal;
	gsglboolean set_normal_already;
	gsglboolean in_line;
	gsglboolean in_polygon;
	gsglboolean quit;
	gsglboolean in_old_fformat;
	gsglboolean old_header;
	int combine;
	float modelview_matrix [16];
	float stack_matrix[16];
	float old_matrix[16];
	double proj_matrix_inv[16];
	double viewport_inv[4];
} read_data;

static int gsglfread_GetFileSize (gsglFileInfo * fileinfo, gzFile gzfile, FILE *fp);
static void gsglfread_GetPageInfo (gsglPageInfo *Info, unsigned char *, int pagenum);
static void gsglfread_InitInverseMat (double *projMatrix_inv, double *viewport_inv, gsglPage *);
static void gsglfread_GetVertex3 (read_data *data, float *x, float *y, float *z);
static void gsglfread_GetProjectedVertex2 (read_data *data, float *x, float *y, gsglPage *);
static void gsglfread_GetProjectedVertex3 (read_data *data,float *x,float *y,float *z, gsglPage *);
static void gsglfread_vertex (read_data *data, gsglPage *, int datatype);
static void gsglfread_Rewind (gsglboolean gzip, gzFile gzfile, FILE *);
static void gsglfread_ReadFile (gsglFileInfo *fileinfo, gzFile gzfile, FILE *);

/*****************************************************/
/*          読み込むファイル名を問い合わせる         */
/*****************************************************/
void gsglfread_getfilename (int argc, char *argv[], int optind, gsglFileInfo *fileinfo)
{
	char str[256];
	char base[256];
	int  len;
	int  i;
	
	if (optind >= argc) {
		do {
			printf (" ## INPUT : FILENAME\n");
			for (i=0; i<256; i++)
				str[i] = '\0';
			fgets (str, 256, stdin);
			if (str != NULL) {
				strcpy (base, str);
			}
			len = strlen(base);
			base[len-1] = '\0'; 
		} while (len < 1);
	} else {
		strcpy (base, argv[optind]);
		len = strlen (base);
	}
	
	strcpy (fileinfo->filename, base);

	if (access (fileinfo->filename, R_OK)) {
		if (len < 6 || (len >= 6 && strcmp(&(fileinfo->filename[len-5]), ".gsgl")))
			sprintf (fileinfo->filename, "%s.gsgl", fileinfo->filename);
		
		if (access (fileinfo->filename, R_OK)) {
			if (len < 9 || (len >= 9 && strcmp(&(fileinfo->filename[len-8]), ".gsgl.gz")))
				sprintf (fileinfo->filename, "%s.gz", fileinfo->filename);
			if (access (fileinfo->filename, R_OK)) {
				gsgl_error ("gsglfread_getfilename", "File %s or %s.gsgl or %s.gsgl.gz is not found", base, base, base);
			}
		}
	}
	len = strlen (fileinfo->filename);
	if (strcmp(&(fileinfo->filename[len-3]), ".gz")) {
		fileinfo->gzip = GSGL_FALSE;
	} else {
		fileinfo->gzip = GSGL_TRUE;
	}
}

/******************************************************/
/*        中間ファイルのページ情報を取得する          */
/******************************************************/
static int gsglfread_GetFileSize (gsglFileInfo *fileinfo, gzFile gzfile, FILE *fp)
{
	unsigned char buffer[BUFFERSIZE];
	unsigned int  j=0, read=0, filesize=0;
	unsigned int  token;
	int  i=0;
	unsigned int  currentpoint=0;
	unsigned char *loc;
	gsglReadFile *r;
	gsglPageInfo Info;

	do {
		if (fileinfo->gzip) {
			if (gzfile == NULL) {
				gsgl_error ("gsglfread_GetFileSize", "gzfile == NULL");
			} else {
				read = gzread (gzfile, buffer, BUFFERSIZE);
			}
		} else {
			read = fread (buffer, 1, BUFFERSIZE, fp);
		}
		loc = buffer;
		filesize += read;
		if (filesize == currentpoint) break;
		if (currentpoint - j*BUFFERSIZE < BUFFERSIZE) {
			do {
				r = gsgl_init_read_file ();

				i++;
				loc -= j*BUFFERSIZE;
				loc += currentpoint + 12 /* GSAF/OpenGL0 */;
				loc = gsglfread_token (loc, &token, r); /* GSGL_FILE_BEGIN_PAGEINFO */
				loc = gsglfread_token (loc, &token, r); /* GSGL_FILE_PAGE_NUMBER */
				loc = gsglfread_token (loc, &token, r); /* page number */
				loc = gsglfread_token (loc, &token, r); /* GSGL_FILE_PAGE_SIZE */
				loc = gsglfread_token (loc, &token, r); /* filesize */
				Info.pagefilesize = token;
				loc = gsglfread_token (loc, &token, r); /* filesize */
				Info.pagefilesize += token*256*256;
				currentpoint += Info.pagefilesize;
				loc = buffer;
			} while (currentpoint < MIN(j*BUFFERSIZE+read, j*BUFFERSIZE+BUFFERSIZE));
		}
		j++ ;
	} while (read == BUFFERSIZE);  
	fileinfo->pagenum = i;
	return filesize;
}

/******************************************************/
/*        中間ファイルのページ情報を取得する          */
/******************************************************/
static void gsglfread_GetPageInfo (gsglPageInfo *Info, unsigned char *buffer, int pagenum)
{
	unsigned int  token;
	int  i=0;
	unsigned int offset=0;
	unsigned char *loc, *top;
	gsglReadFile *r;

	Info[0].startpoint = 0;
	Info[0].pagefilesize = 0;
	Info[0].pagenumber = 0;

	loc = buffer;
	do {
		r = gsgl_init_read_file ();
		top = loc;
		i++;

		Info[i].startpoint = offset;
		loc += 12 /* GSAF/OpenGL0 */;
		loc = gsglfread_token (loc, &token, r); /* GSGL_FILE_BEGIN_PAGEINFO */
		loc = gsglfread_token (loc, &token, r); /* GSGL_FILE_PAGE_NUMBER */
		loc = gsglfread_token (loc, &token, r); /* page number */
		Info[i].pagenumber = token;
		loc = gsglfread_token (loc, &token, r); /* GSGL_FILE_PAGE_SIZE */
		loc = gsglfread_token (loc, &token, r); /* filesize */
		Info[i].pagefilesize = token;
		loc = gsglfread_token (loc, &token, r); /* filesize */
		Info[i].pagefilesize += token*256*256;
		offset += Info[i].pagefilesize;
		loc = top + Info[i].pagefilesize;
	} while (i < pagenum);
}

gsglFileInfo * gsglfread_getfileinfo (int argc, char *argv[], int optind)
{
	int l, k;
	gsglFileInfo *fileinfo = NULL, *FileInfo;
	gzFile gzfile = NULL;
	FILE *fp = NULL;
	gsgllist *list = NULL;
	gsgllist *list1;

	FileInfo = gsgl_malloc (sizeof(gsglFileInfo));
	FileInfo->pagenum = 0;
	FileInfo->filesize = 0;
	FileInfo->buffer = NULL;

	/* get filesize and page number */
	do {
		fileinfo = gsgl_malloc (sizeof(gsglFileInfo));
		fileinfo->pagenum = 0;
		fileinfo->filesize = 0;
		fileinfo->gzip = GSGL_FALSE;
		fileinfo->buffer = NULL;

		list = gsgl_list_append (list, fileinfo);

		/* get input filename */
		gsglfread_getfilename (argc, argv, optind, fileinfo);
		if (fileinfo->gzip) {
			gzfile = gzopen (fileinfo->filename, "rb");
		} else {
			if ((fp = fopen(fileinfo->filename, "rb")) == NULL) {
				gsgl_error ("gsglfread_getfileinfo", "Could not open file %s", fileinfo->filename);
			}
		}
		fileinfo->filesize = gsglfread_GetFileSize (fileinfo, gzfile, fp);
		
		fileinfo->buffer = (unsigned char *) gsgl_malloc_atomic_ignore_off_page (fileinfo->filesize*sizeof (unsigned char));
		gsglfread_Rewind (fileinfo->gzip, gzfile, fp);
		gsglfread_ReadFile (fileinfo, gzfile, fp);
		if (fileinfo->gzip) {
			gzclose (gzfile);
		} else {
			fclose (fp);
		}
		
		optind++;
	} while (optind < argc);

	list1 = list;
	while (list1) {
		fileinfo = list1->data;
		FileInfo->pagenum += fileinfo->pagenum;
		FileInfo->filesize += fileinfo->filesize;
		list1 = list1->next;
	}

	FileInfo->pageinfo = gsgl_malloc (sizeof(gsglPageInfo)*(FileInfo->pagenum+1));
	FileInfo->buffer = gsgl_malloc_atomic_ignore_off_page 
		(FileInfo->filesize*sizeof(unsigned char));

	l = 0;
	list1 = list;
	while (list1) {
		fileinfo = list1->data;
		for (k = 0; k < fileinfo->filesize; k++) {
			FileInfo->buffer[l] = fileinfo->buffer[k];
			l++;
		}
		list1 = list1->next;
	}
	fileinfo = list->data;

	gsglfread_GetPageInfo (FileInfo->pageinfo, FileInfo->buffer, FileInfo->pagenum);
	strcpy (FileInfo->filename, fileinfo->filename);

	return FileInfo;
}

/**************************************************************/
/*         中間ファイルに含まれるページ数を表示する           */
/**************************************************************/   
void gsglfread_showpage (GLint totalpage, gsglPageInfo Info[])
{
	int i;
	
	for (i=1 ; i <= totalpage ; i++) {
		if (i == 1) {
			if (i == totalpage) printf(" ## PAGES : %3d\n", Info[i].pagenumber);
			else printf(" ## PAGES : %3d", Info[i].pagenumber);
		} else if (i == totalpage) {
			if (i%10 == 1) printf("            %3d\n",Info[i].pagenumber);
			else printf(",%3d\n",Info[i].pagenumber);
		} else if (i%10 == 0) {
			printf(",%3d,\n",Info[i].pagenumber);
		} else if (i%10 == 1) {
			printf("            %3d",Info[i].pagenumber);
		} else {
	    printf(",%3d",Info[i].pagenumber);
		}
	}
}

/***********************************************************************/
/* 複数のページを1ページに combine するための projection matrix を設定 */
/***********************************************************************/
void gsglfread_SetPicturePos (int m, int i, int j, float delta_x, float delta_y, float matrix[16])
{
	float top, bottom, left, right;
	float mat1[16], mat2[16], mat3[16];

	if (m == 0) {
		matrix[0] = matrix[5] = matrix[10] = matrix[15] = 1.0;
		matrix[1] = matrix[2] = matrix[3] = matrix[4] = 0.0;
		matrix[6] = matrix[7] = matrix[8] = matrix[9] = 0.0;
		matrix[11] = matrix[12] = matrix[13] = matrix[14] = 0.0;
		return;
	}
	if (m%2 == 0) {
		left = ((float) ( i -1 )) * delta_x;
		right = ((float) i) * delta_x;
		top = GSGL_PAPER_HEIGHT - ((float) j) * delta_y;
		bottom = GSGL_PAPER_HEIGHT - ((float) (j -1)) * delta_y;
	} else {
		left = GSGL_PAPER_WIDTH - ((float) j) * delta_x;
		right = GSGL_PAPER_WIDTH - ((float) (j - 1)) * delta_x;
		top = GSGL_PAPER_HEIGHT - ((float) i) * delta_y;
		bottom = GSGL_PAPER_HEIGHT - ((float) (i - 1)) * delta_y;
	}
	
	gsgl_inq_matrix_translate (2.0*left/GSGL_PAPER_WIDTH-1.0,
				   2.0*top/GSGL_PAPER_HEIGHT-1.0,
				   0.0, mat1);

	gsgl_inq_matrix_scale ((right-left)/GSGL_PAPER_WIDTH,
			       (bottom-top)/GSGL_PAPER_HEIGHT,
			       1.0, mat2);
	gsgl_mat16_mult_mat16 (mat1, mat2, mat3);

	gsgl_inq_matrix_translate (1.0, 1.0, 0.0, mat1);
	gsgl_mat16_mult_mat16 (mat3, mat1, mat2);

	if (m%2 == 1) {
		gsgl_inq_matrix_translate (-1.0, 1.0, 0.0, mat1);
		gsgl_mat16_mult_mat16 (mat2, mat1, mat3);

		gsgl_inq_matrix_rotate (90.0, 0.0, 0.0, -1.0, mat1);
		gsgl_mat16_mult_mat16 (mat3, mat1, mat2);
		
		gsgl_inq_matrix_translate (1.0, 1.0, 0.0, mat1);
		gsgl_mat16_mult_mat16 (mat2, mat1, mat3);

		for (i=0; i<16; i++) mat2[i] = mat3[i];
	}
	for (i=0; i<16; i++) matrix[i] = mat2[i];
}

/*********************************************************************/
/* 入力された logical page numver から physical page number を求める */
/*********************************************************************/
int gsglfread_getphysicalpage (int totalpage, int *logicalpage,
			       int *physicalpage, gsglPageInfo Info[],
			       int *istart, int *iend, int interactive)
{
	int i, j;
  
	if (interactive) {
		GSGLREADPAGENUMBER_F77 (logicalpage);
		if (*logicalpage == -1) return -1; /* Ctrl^D を入力 */
		else if ( *logicalpage == 0 ) { /* 0 を入力 */
			*istart = 1;
			*iend = totalpage;
			return 1;
		} else {
			for ( i = *physicalpage ; i <= totalpage ; i++ ) {
				/* 現行のページよりもあとのページから探す */
				if ( *logicalpage == Info[i].pagenumber ) {
					*physicalpage = i;
					*istart = i;
					*iend = i;
					return 1;
				}
			}
			/* 見つからない場合はじめから探す */
			for ( i = 1 ; i <= totalpage ; i++ ) {
				if ( *logicalpage == Info[i].pagenumber ) {
					*physicalpage = i;
					*istart = i;
					*iend = i;
					return 1;
				}
			}
			*istart = 1;
			*iend = 1;
		}
	} else { /* not interactive : オプション -a or -s or -e */
		if (*istart == 0) {
			*istart = 1;
			*iend = totalpage;
			return 1;
		} else {
			for (j = 1 ; j <= totalpage ; j++) {
				if (*istart == Info[j].pagenumber) {
					*istart = j;
					*physicalpage = j;
					break;
				}
			}
			for (j = totalpage ; j >= 1 ; j--) {
				if (*iend == Info[j].pagenumber) {
					*iend = j;
					return 1;
				}
			}
			*iend = totalpage;
			return 1;
		}
	}
	return 0;
}

/********************************************************/
/*               ファイルを読み込む                     */
/********************************************************/
static void gsglfread_ReadFile (gsglFileInfo *fileinfo, gzFile gzfile, FILE *fp)
{
	int read;
	
	if (fileinfo->gzip) {
		read = gzread (gzfile, fileinfo->buffer, fileinfo->filesize);
	} else {
		read = fread (fileinfo->buffer, sizeof(unsigned char), fileinfo->filesize, fp);
	}
	if (read != fileinfo->filesize)
		gsgl_error ("gsglfread_ReadFile", "File is broken (read=%d)", read);
}

/******************************************************/
/*                    rewind file                     */
/******************************************************/ 
static void gsglfread_Rewind (gsglboolean gzip, gzFile gzfile, FILE *fp)
{
	if (gzip) {
		gzrewind (gzfile);
	} else { 
		rewind (fp);
	}
}

#define GSGL_DEBUG(x)

static void read_token_fio (void *data1, unsigned int *token)
{
	read_data *data = data1;
	data->loc = gsglfread_token (data->loc, token, data->r);
}

static void gsgl_file_read_vertex2 (void *data1, gsglPage *p, READ_TOKEN)
{
	gsglvec3f point;
	read_data *data = data1;

	gsgl_file_format_read_float (data, &point.x, read_token);
	gsgl_file_format_read_float (data, &point.y, read_token);
	if (!data->ignore_header) {
		gsgl_drv_vertex2 (point.x, point.y, p);
		GSGL_DEBUG(fprintf (stderr, "%f %f\n", point.x, point.y));
	}
}

static void gsgl_file_read_vertex3 (void *data1, gsglPage *p, READ_TOKEN)
{
	float npoint1[3], npoint2[3], npoint3[3], normal[3];
	unsigned int token;
	read_data *data = data1;

	gsgl_file_format_read_float (data, &npoint1[0], read_token);
	gsgl_file_format_read_float (data, &npoint1[1], read_token);
	gsgl_file_format_read_float (data, &npoint1[2], read_token);
	if (!data->get_normal || data->set_normal_already) {
		gsgl_drv_vertex3 (npoint1[0], npoint1[1], npoint1[2], p);
		GSGL_DEBUG(fprintf (stderr, "%f %f %f\n", npoint1[0], npoint1[1], npoint1[2]));
	} else if (data->in_line) {
		gsgl_drv_glonly_normal (0.0, 0.0, 1.0, p);
		gsgl_drv_vertex3 (npoint1[0], npoint1[1], npoint1[2], p);
		data->set_normal_already = GSGL_TRUE;
	} else if (data->in_polygon) {
		read_token_fio (data, &token);
		gsgl_file_format_read_float (data, &npoint2[0], read_token);
		gsgl_file_format_read_float (data, &npoint2[1], read_token);
		gsgl_file_format_read_float (data, &npoint2[2], read_token);
		read_token_fio (data, &token);
		gsgl_file_format_read_float (data, &npoint3[0], read_token);
		gsgl_file_format_read_float (data, &npoint3[1], read_token);
		gsgl_file_format_read_float (data, &npoint3[2], read_token);

		gsgl_set_normal_vector2 (npoint1, npoint2, npoint3, normal);
		gsgl_drv_glonly_normal (normal[0], normal[1], normal[2], p);

		gsgl_drv_vertex3 (npoint1[0], npoint1[1], npoint1[2], p);
		gsgl_drv_vertex3 (npoint2[0], npoint2[1], npoint2[2], p);
		gsgl_drv_vertex3 (npoint3[0], npoint3[1], npoint3[2], p);
		data->set_normal_already = GSGL_TRUE;
		GSGL_DEBUG(fprintf (stderr, "%f %f %f\n", npoint1[0], npoint1[1], npoint1[2]));
		GSGL_DEBUG(fprintf (stderr, "%f %f %f\n", npoint2[0], npoint2[1], npoint2[2]));
		GSGL_DEBUG(fprintf (stderr, "%f %f %f\n", npoint3[0], npoint3[1], npoint3[2]));
	}
}

static void gsgl_file_read_begin_line (void *data1, gsglPage *p, READ_TOKEN)
{
	int coord;
	read_data *data = data1;

	if (!data->ignore_header) {
		data->in_line = GSGL_TRUE; /* backward compatibility */
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
}

static void gsgl_file_read_end_line (void *data1, gsglPage *p, READ_TOKEN)
{
	read_data *data = data1;

	if (!data->ignore_header) {
		data->in_line = GSGL_FALSE; /* backward compatibility */
		gsgl_drv_endlines (p);
		data->get_normal = GSGL_FALSE;
		data->set_normal_already = GSGL_FALSE;
	}
}

static void gsgl_file_read_begin_poly (void *data1, gsglPage *p, READ_TOKEN)
{
	int coord;
	read_data *data = data1;

	data->in_polygon = GSGL_TRUE; /* backward compatibility */
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

static void gsgl_file_read_end_poly (void *data1, gsglPage *p, READ_TOKEN)
{
	read_data *data = data1;

	data->in_polygon = GSGL_FALSE; /* backward compatibility */
	gsgl_drv_endpolygon (p);
	data->get_normal = GSGL_FALSE;
	data->set_normal_already = GSGL_FALSE;
}

static void gsgl_file_read_normal (void *data1, gsglPage *p, READ_TOKEN)
{
	float x, y, z;
	read_data *data = data1;

	gsgl_file_format_read_float (data, &x, read_token);
	gsgl_file_format_read_float (data, &y, read_token);
	gsgl_file_format_read_float (data, &z, read_token);
	if (!data->ignore_header) {
		gsgl_drv_normal (x, y, z, p);
		data->set_normal_already = GSGL_TRUE;
		GSGL_DEBUG(fprintf (stderr, "%f %f %f\n", x, y, z));
	}
}

static void gsgl_file_read_modelview (void *data1, gsglPage *p, READ_TOKEN)
{
	int i;
	read_data *data = data1;

	for (i=0; i<16 ; i++) {
		gsgl_file_format_read_float (data, &(data->modelview_matrix[i]), read_token);
	}
	GSGL_DEBUG(fprintf (stderr, "modelview\n"));
	GSGL_DEBUG(fprintf (stderr, "%f %f %f %f\n", 
			    data->modelview_matrix[0], data->modelview_matrix[4],
			    data->modelview_matrix[8], data->modelview_matrix[12]));
	GSGL_DEBUG(fprintf (stderr, "%f %f %f %f\n",
			    data->modelview_matrix[1], data->modelview_matrix[5],
			    data->modelview_matrix[9], data->modelview_matrix[13]));
	GSGL_DEBUG(fprintf (stderr, "%f %f %f %f\n", 
			    data->modelview_matrix[2], data->modelview_matrix[6],
			    data->modelview_matrix[10], data->modelview_matrix[14]));
	GSGL_DEBUG(fprintf (stderr, "%f %f %f %f\n",
			    data->modelview_matrix[3], data->modelview_matrix[7],
			    data->modelview_matrix[11], data->modelview_matrix[15]));
	if (!data->ignore_header)
		gsgl_modelview_matrix (data->modelview_matrix, p);
}

static void gsgl_file_read_rgb (void *data1, gsglPage *p, READ_TOKEN)
{
	int coord;
	unsigned int token;
	float rgb[3];
	read_data *data = data1;

	read_token_fio (data, &token);
	rgb[0] = gsgl_convert_token_to_0to1 (token);
	read_token_fio (data, &token);
	rgb[1] = gsgl_convert_token_to_0to1 (token);
	read_token_fio (data, &token);
	rgb[2] = gsgl_convert_token_to_0to1 (token);
	GSGL_DEBUG(fprintf (stderr, "rgb = %f %f %f\n", rgb[0], rgb[1], rgb[2]));
	if (data->in_line) { /* backward compatibility */
		/* ver0.3.8より前では begin line のあとに rgb がくることもある */
		gsgl_drv_endlines (p);
		gsgl_rgb (rgb[0], rgb[1], rgb[2], p);
		gsgl_coordinate_inq (&coord, p);
		if (coord == GSGL_COORD_NORMAL || coord == GSGL_COORD_2D) {
			gsgl_displaylist_begin (GSGL_LIST_2DOBJ, p);
		} else if (coord == GSGL_COORD_3D) {
			gsgl_displaylist_begin (GSGL_LIST_3DOBJ, p);
		}
		gsgl_drv_beginlines (p);
	} else if (data->in_polygon) {
		/* ver0.3.8より前では begin poly のあとに rgb がくることもある */
		gsgl_drv_endpolygon (p);
		gsgl_rgb (rgb[0], rgb[1], rgb[2], p);
		gsgl_coordinate_inq (&coord, p);
		if (coord == GSGL_COORD_NORMAL || coord == GSGL_COORD_2D) {
			gsgl_displaylist_begin (GSGL_LIST_2DOBJ, p);
		} else if (coord == GSGL_COORD_3D) {
			gsgl_displaylist_begin (GSGL_LIST_3DOBJ, p);
		}
		gsgl_drv_beginpolygon (p);
	} else {
		gsgl_rgb (rgb[0], rgb[1], rgb[2], p);
	}
}

static void gsgl_file_read_projection (void *data1, gsglPage *p, READ_TOKEN)
{
	int i;
	float proj[16];
	double dproj[16];
	float tmpmatrix[16];
	read_data *data = data1;

	for (i=0; i<16 ; i++) {
		gsgl_file_format_read_float (data, &proj[i], read_token);
		dproj[i] = (double) proj[i];
	}
	GSGL_DEBUG(fprintf (stderr, "%f %f %f %f\n", proj[0], proj[4], proj[8], proj[12]));
	GSGL_DEBUG(fprintf (stderr, "%f %f %f %f\n", proj[1], proj[5], proj[9], proj[13]));
	GSGL_DEBUG(fprintf (stderr, "%f %f %f %f\n", proj[2], proj[6], proj[10], proj[14]));
	GSGL_DEBUG(fprintf (stderr, "%f %f %f %f\n", proj[3], proj[7], proj[11], proj[15]));
	if (!data->ignore_header) {
		if (data->combine > 1) {
			gsgl_mat16_mult_mat16 (data->stack_matrix, proj, tmpmatrix);
			for (i=0; i<16; i++) proj[i] = tmpmatrix[i];
		}
		gsgl_projection_matrix (proj, p);
	}
	/**/
	gsgl_matrix_inverse (4, dproj);
	for (i=0; i<16; i++) data->proj_matrix_inv[i] = dproj[i];
	/**/
}

static void gsgl_file_read_begin_pageinfo (void *data, gsglPage *p, READ_TOKEN)
{
	unsigned int token;

	read_token_fio (data, &token); /* GSGL_FILE_PAGE_NUMBER */
	read_token_fio (data, &token); /* page number */
	read_token_fio (data, &token); /* GSGL_FILE_PAGE_SIZE */
	read_token_fio (data, &token); /* filesize */
	read_token_fio (data, &token); /* filesize */
}

static void gsgl_file_read_begin_head (void *data1, gsglPage *p, READ_TOKEN)
{
	read_data *data = data1;

	if (!data->old_header) {
		data->ignore_header = GSGL_TRUE;
	} else if (data->combine == 1) {
		gsgl_begin_header (p);
	}
}

static void gsgl_file_read_end_head (void *data1, gsglPage *p, READ_TOKEN)
{
	read_data *data = data1;

	if (!data->old_header) {
		data->ignore_header = GSGL_FALSE;
	} else if (data->combine == 1) {
		gsgl_end_header (p);
	}
}

static void gsgl_file_read_end_page (void *data1, gsglPage *p, READ_TOKEN)
{
	read_data *data = data1;

	data->quit = GSGL_TRUE;
}

static void gsgl_file_read_enable_client_vertex (void *data1, gsglPage *p, READ_TOKEN)
{
	unsigned int token;
	read_data *data = data1;

	read_token_fio (data, &token);
	if (token == GSGL_FILE_CLIENT_VERTEX_TYPE) {
		read_token_fio (data, &token);
		gsglfread_vertex (data, p, token);
	}
}

static void gsgl_file_read_vertex2_projctd (void *data1, gsglPage *p, READ_TOKEN)
{
	float x, y;
	read_data *data = data1;	

	gsglfread_GetProjectedVertex2 (data, &x, &y, p);
	gsgl_drv_vertex2 (x, y, p);
}

static void gsgl_file_read_vertex3_projctd (void *data1, gsglPage *p, READ_TOKEN)
{
	unsigned int token;
	float npoint1[3], npoint2[3], normal[3];
	float x,y,z;
	float nvec1[3], nvec2[3];
	read_data *data = data1;

	if (data->get_normal && !data->in_line) {
		data->get_normal = 0;
		gsglfread_GetProjectedVertex3 (data, &npoint1[0], &npoint1[1], &npoint1[2], p);
		read_token_fio (data, &token);/**/
		GSGL_DEBUG(fprintf (stderr, "%d\n", token));
		gsglfread_GetProjectedVertex3 (data, &npoint2[0], &npoint2[1], &npoint2[2], p);
		nvec1[0] = npoint2[0] - npoint1[0];
		nvec1[1] = npoint2[1] - npoint1[1];
		nvec1[2] = npoint2[2] - npoint1[2];
		read_token_fio (data, &token);/**/
		GSGL_DEBUG(fprintf (stderr, "%d\n", token));
		gsglfread_GetProjectedVertex3 (data, &x, &y, &z, p);
		nvec2[0] = x - npoint2[0];
		nvec2[1] = y - npoint2[1];
		nvec2[2] = z - npoint2[2];
		normal[0] = nvec1[1] * nvec2[2] - nvec2[1] * nvec1[2];
		normal[1] = nvec1[2] * nvec2[0] - nvec2[2] * nvec1[0];
		normal[2] = nvec1[0] * nvec2[1] - nvec2[0] * nvec1[1];
		gsgl_drv_glonly_normal (normal[0], normal[1], normal[2], p);
		gsgl_drv_vertex3 (npoint1[0], npoint1[1], npoint1[2], p);
		gsgl_drv_vertex3 (npoint2[0], npoint2[1], npoint2[2], p);
		gsgl_drv_vertex3 (x, y, z, p);
	} else {
		gsglfread_GetProjectedVertex3 (data, &x, &y, &z, p);
		gsgl_drv_vertex3 (x, y, z, p);
	}
}

static void gsgl_file_read_3d_2 (void *data1, gsglPage *p, READ_TOKEN)
{
	unsigned int token;
	float depth;
	read_data *data = data1;

	read_token_fio (data, &token);
	if (token == GSGL_FILE_3D_DEPTH) {
		gsgl_file_format_read_float (data, &depth, read_token);
		depth = GSGL_VIEW_DEPTH * depth;
	}
	read_token_fio (data, &token);
	if (token == GSGL_FILE_3D_PROJ) {
		read_token_fio (data, &token);
	}
	gsgl_projectiontype (token, gsgl_inq_page());
	gsgl_depth3d (depth, gsgl_inq_page());
}

static void gsgl_file_read_begin_object (void *data1, gsglPage *p, READ_TOKEN)
{
	gsgl_displaylist_begin (GSGL_LIST_STATUS, p);
	gsgl_drv_separate_objects (p);
}

static void gsgl_file_read_end_object (void *data1, gsglPage *p, READ_TOKEN)
{
	gsgl_displaylist_begin (GSGL_LIST_STATUS, p);
	gsgl_drv_separate_objects (p);
}

static void gsgl_file_read_normal_by_scal_prod (void *data1, gsglPage *p, READ_TOKEN)
{
	unsigned int token;
	float x[3],y[3],z[3];
	read_data *data = data1;

	read_token_fio (data, &token);
	if (token == GSGL_FILE_VERTEX3) { /* obsolute */
		gsglfread_GetVertex3(data, &x[0], &y[0], &z[0]);
		read_token_fio (data, &token);
		gsglfread_GetVertex3(data, &x[1], &y[1], &z[1]);
		/*gsgl_normal_by_scalerproduct (x, y, z);*/
	} else if (token == GSGL_FILE_VERTEX3_PRJCTD) {
		gsglfread_GetProjectedVertex3(data, &x[0], &y[0], &z[0], p);
		read_token_fio (data, &token);
		gsglfread_GetProjectedVertex3(data, &x[1], &y[1], &z[1], p);
		/*gsgl_normal_by_scalerproduct (x, y, z);*/
	}
}

static void gsgl_file_read_depth_by_vect_prod_old (void *data1, gsglPage *p, READ_TOKEN)
{
	unsigned int token;
	float x[3],y[3],z[3];
	read_data *data = data1;

	read_token_fio (data, &token);
	if (token == GSGL_FILE_VERTEX3) {
		gsglfread_GetVertex3 (data, &x[0], &y[0], &z[0]);
		read_token_fio (data, &token);
		gsglfread_GetVertex3 (data, &x[1], &y[1], &z[1]);
		read_token_fio (data, &token);
		gsglfread_GetVertex3 (data, &x[2], &y[2], &z[2]);
		gsgl_ps_depth_by_vectprod (x, y, z, p);
	} else if (token == GSGL_FILE_VERTEX3_PRJCTD) {
		gsglfread_GetProjectedVertex3 (data, &x[0], &y[0], &z[0], p);
		read_token_fio (data, &token);
		gsglfread_GetProjectedVertex3 (data, &x[1], &y[1], &z[1], p);
		read_token_fio (data, &token);
		gsglfread_GetProjectedVertex3 (data, &x[2], &y[2], &z[2], p);
		gsgl_ps_depth_by_vectprod (x, y, z, p);
	}
}

static void gsgl_file_read_depth_by_point_old (void *data1, gsglPage *p, READ_TOKEN)
{
	unsigned int token;
	gsglvec3f point;
	float x,y,z;
	read_data *data = data1;

	read_token_fio (data, &token);
	if (token == GSGL_FILE_VERTEX3) {
		gsglfread_GetVertex3 (data, &point.x, &point.y, &point.z);
		gsgl_ps_depth_by_point (point.x, point.y, point.z, p);
	} else if (token == GSGL_FILE_VERTEX3_PRJCTD) {
		gsglfread_GetProjectedVertex3 (data, &x, &y, &z, p);
		gsgl_ps_depth_by_point (x,y,z, p);
	}
}

static void gsgl_file_read_begin_fformat (void *data1, gsglPage *p, READ_TOKEN)
{
	int i;
	unsigned int token;
	read_data *data = data1;

	data->in_old_fformat = GSGL_TRUE; /* backward compatibility */
	read_token_fio(data, &token);
	if (token == FFORMAT_TYPE) {
		read_token_fio(data, &token);
	}
	for (i=0; i<16; i++) data->old_matrix[i] = data->modelview_matrix[i];
}

static void gsgl_file_read_end_fformat (void *data1, gsglPage *p, READ_TOKEN)
{
	read_data *data = data1;

	data->in_old_fformat = GSGL_FALSE; /* backward compatibility */
	gsgl_modelview_matrix (data->old_matrix, p);
}

static void gsgl_file_read_3d (void *data1, gsglPage *p, READ_TOKEN)
{
	unsigned int token;
	float depth;
	read_data *data = data1;

	read_token_fio (data, &token);
	depth = 10.0 + gsgl_convert_token_to_0to1 (token) * GSGL_VIEW_DEPTH;
	read_token_fio (data, &token);
	gsgl_projectiontype (token, gsgl_inq_page());
	gsgl_depth3d (depth, gsgl_inq_page());
}

static void gsgl_file_read_do_nothing (void *data1, gsglPage *p, READ_TOKEN)
{
}

static void gsgl_file_read_enable_depth_test (void *data1, gsglPage *p, READ_TOKEN)
{
	gsgl_coordinate_set (GSGL_COORD_3D, p);
	gsgl_displaylist_begin (GSGL_LIST_STATUS, p);
	gsgl_drv_3dmode (p);
}

static void gsgl_file_read_disable_depth_test (void *data1, gsglPage *p, READ_TOKEN)
{
	gsgl_coordinate_set (GSGL_COORD_2D, p);
	gsgl_displaylist_begin (GSGL_LIST_STATUS, p);
	gsgl_drv_2dmode (p);
}

static void gsgl_file_read_default (unsigned int token, void *data1, gsglPage *p, READ_TOKEN)
{
	gsgl_error ("gsglfread_page", "Unexpected token = %d", token);
}

static void gsgl_file_read_unexpected (void *data1, gsglPage *p, READ_TOKEN)
{
	gsgl_error ("gsglfread_page", "Unexpected token");
}

static void gsgl_file_read_gui_button (void *data, gsglPage *p, READ_TOKEN)
{
	guialldata *data0;
	labeldata *s;

	data0 = gsgl_file_format_read_gui_button_data (data, p, read_token);
	s = data0->specific;

	gsgl_gui_button (data0->common, s->text, NULL);
}

static void gsgl_file_read_gui_combo (void *data, gsglPage *p, READ_TOKEN)
{
	guialldata *data0;
	comboboxdata *s;

	data0 = gsgl_file_format_read_gui_combo_data (data, p, read_token);
	s = data0->specific;

	gsgl_gui_combobox (data0->common, s->items, s->text, s->current, NULL, 0);
}

static void gsgl_file_read_gui_spinf (void *data, gsglPage *p, READ_TOKEN)
{
	guialldata *data0;
	spinbuttondataf *s;

	data0 = gsgl_file_format_read_gui_spinf_data (data, p, read_token);
	s = data0->specific;

	gsgl_gui_spinbuttonf (data0->common, s->init, s->lower, s->upper, s->digits, NULL);
}

static void gsgl_file_read_gui_spini (void *data, gsglPage *p, READ_TOKEN)
{
	guialldata *data0;
	spinbuttondatai *s;

	data0 = gsgl_file_format_read_gui_spini_data (data, p, read_token);
	s = data0->specific;

	gsgl_gui_spinbuttoni (data0->common, s->init, s->lower, s->upper, NULL);
}


static void gsgl_file_read_gui_check (void *data, gsglPage *p, READ_TOKEN)
{
	guialldata *data0;
	checkbuttondata *s;

	data0 = gsgl_file_format_read_gui_check_data (data, p, read_token);
	s = data0->specific;

	gsgl_gui_checkbutton (data0->common, s->text, s->init, NULL);
}

static void gsgl_file_read_gui_edit (void *data, gsglPage *p, READ_TOKEN)
{
	guialldata *data0;
	lineeditdata *s;

	data0 = gsgl_file_format_read_gui_edit_data (data, p, read_token);
	s = data0->specific;

	gsgl_gui_lineedit (data0->common, s->max, s->text, NULL);
}

static void gsgl_file_read_gui_label (void *data, gsglPage *p, READ_TOKEN)
{
	guialldata *data0;
	labeldata *s;

	data0 = gsgl_file_format_read_gui_label_data (data, p, read_token);
	s = data0->specific;

	gsgl_gui_label (data0->common, s->text, s->ijust);
}

/******************************************************/
/*                   ページを読む                     */
/******************************************************/
void gsglfread_page (unsigned int size, unsigned char *buffer, gsglboolean old_header, 
		     int combine, float stack_matrix[16], gsglPage *page)
{
	int i;
	unsigned char    *loc, *end;
	float            modelview_matrix [16] = {1.0, 0.0, 0.0, 0.0,
						  0.0, 1.0, 0.0, 0.0,
						  0.0, 0.0, 1.0, 0.0,
						  0.0, 0.0, 0.0, 1.0};
	read_data        *data;

	/* 初期化 */
	data = (read_data *) gsgl_malloc (sizeof(read_data));

	data->r = gsgl_init_read_file ();
	data->ignore_header = GSGL_FALSE;
	data->get_normal = GSGL_FALSE;
	data->set_normal_already = GSGL_FALSE;
	data->in_line = GSGL_FALSE;
	data->in_polygon = GSGL_FALSE;
	data->quit = GSGL_FALSE;
	data->in_old_fformat = GSGL_FALSE;

	for (i=0; i<16; i++) {
		data->modelview_matrix[i] = modelview_matrix[i];
		data->stack_matrix[i] = stack_matrix[i];
	}

	data->combine = combine;
	data->old_header = old_header;
	gsglfread_InitInverseMat (data->proj_matrix_inv, data->viewport_inv, page);

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
	
	while (data->loc < end || data->r->surplus == 2) {
		gsgl_file_format_read_template 
			(page,
			 data, 
			 read_token_fio,
			 gsgl_file_format_read_inq_token, 
			 gsgl_file_read_vertex2, 
			 gsgl_file_read_vertex3,
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
			 gsgl_file_read_begin_line,
			 gsgl_file_read_end_line,
			 gsgl_file_read_begin_poly,
			 gsgl_file_read_end_poly,
			 gsgl_file_read_normal,	
			 gsgl_file_format_read_begin_triangles,
			 gsgl_file_format_read_end_triangles,
			 gsgl_file_format_read_begin_triangle_strip,
			 gsgl_file_format_read_end_triangle_strip,
			 gsgl_file_format_read_begin_triangle_fan,
			 gsgl_file_format_read_end_triangle_fan,
			 gsgl_file_format_read_clip_3d_fileonly,
			 gsgl_file_format_read_clip_unmovable,
			 gsgl_file_format_read_del_clip,
			 gsgl_file_read_modelview,
			 gsgl_file_read_rgb,
			 gsgl_file_format_read_array_element_4byte,
			 gsgl_file_format_read_rgba,
			 gsgl_file_format_read_indexcolor,
			 gsgl_file_format_read_linewidth,
			 gsgl_file_format_read_linewidth2,
			 gsgl_file_read_projection,
			 gsgl_file_format_read_3d_3,
			 gsgl_file_format_read_projection_type,
			 gsgl_file_format_read_enable_lighting,
			 gsgl_file_format_read_disable_lighting,
			 gsgl_file_read_begin_pageinfo,
			 gsgl_file_read_do_nothing,
			 gsgl_file_read_begin_head,
			 gsgl_file_read_end_head,
			 gsgl_file_read_do_nothing,
			 gsgl_file_read_end_page,
			 gsgl_file_format_read_begin_anime,
			 gsgl_file_format_read_end_anime,
			 gsgl_file_read_enable_client_vertex,
			 gsgl_file_format_read_enable_client_color,
			 gsgl_file_format_read_begin_gouraud_only,
			 gsgl_file_format_read_end_gouraud_only,
			 gsgl_file_format_read_depth_by_deep,
			 gsgl_file_format_read_depth_by_shallow,
			 gsgl_file_format_read_depth_by_mean,
			 gsgl_file_format_read_depth_by_point,
			 gsgl_file_format_read_depth_by_vec_prod,
			 gsgl_file_format_read_depth_by_vec_prod2,
			 gsgl_file_read_vertex2_projctd,
			 gsgl_file_read_vertex3_projctd,
			 gsgl_file_read_3d_2,
			 gsgl_file_read_begin_object,
			 gsgl_file_read_end_object,
			 gsgl_file_read_do_nothing,
			 gsgl_file_read_normal_by_scal_prod,
			 gsgl_file_read_depth_by_vect_prod_old,
			 gsgl_file_read_depth_by_point_old,
			 gsgl_file_read_begin_fformat,
			 gsgl_file_read_end_fformat,
			 gsgl_file_read_3d,
			 gsgl_file_read_do_nothing,
			 gsgl_file_read_do_nothing,
			 gsgl_file_read_do_nothing,
			 gsgl_file_read_do_nothing,
			 gsgl_file_read_do_nothing,
			 gsgl_file_read_do_nothing,
			 gsgl_file_read_do_nothing,
			 gsgl_file_read_do_nothing,
			 gsgl_file_read_enable_depth_test,
			 gsgl_file_read_disable_depth_test,
			 gsgl_file_read_do_nothing,
			 gsgl_file_read_do_nothing,
			 gsgl_file_format_read_pstext,
			 gsgl_file_read_unexpected,
			 gsgl_file_read_gui_button,
			 gsgl_file_read_gui_combo,
			 gsgl_file_read_gui_spinf,
			 gsgl_file_read_gui_spini,
			 gsgl_file_read_gui_check,
			 gsgl_file_read_gui_edit,
			 gsgl_file_read_gui_label,
			 gsgl_file_format_read_gui_external,
			 gsgl_file_read_default);
		if (data->quit) return;
	}
}

static void gsglfread_InitInverseMat (double *proj_matrix_inv, double *viewport_inv, gsglPage *p)
{
	int i;
	
	for (i=0; i<16; i++)
		proj_matrix_inv[i] = 0.0;
	
	proj_matrix_inv[0] = ((GLdouble) GSGL_PAPER_WIDTH)*0.5;
	proj_matrix_inv[5] = ((GLdouble) GSGL_PAPER_HEIGHT)*0.5;
	proj_matrix_inv[10] =-0.5*((GLdouble)(GSGL_VIEW_DEPTH));
	proj_matrix_inv[12] = ((GLdouble) GSGL_PAPER_WIDTH)*0.5;
	proj_matrix_inv[13] = ((GLdouble) GSGL_PAPER_HEIGHT)*0.5;  
	proj_matrix_inv[14] =0.5*((GLdouble)(-GSGL_VIEW_DEPTH)); 
	proj_matrix_inv[15] = 1.0;
	
	viewport_inv[0] = 0.0; viewport_inv[1] = 0.0; 
	viewport_inv[2] = 1.0/((GLdouble) gsgl_inq_window_width (p->win));
	viewport_inv[3] = 1.0/((GLdouble) gsgl_inq_window_height (p->win));
}

static void gsglfread_GetVertex3 (read_data *data, float *x, float *y, float *z)
{
	unsigned int token;

	read_token_fio (data, &token);
	*x = gsgl_convert_token_to_0to1 (token) * GSGL_PAPER_WIDTH;
	read_token_fio (data, &token);
	*y = gsgl_convert_token_to_0to1 (token) * GSGL_PAPER_HEIGHT;
	read_token_fio (data, &token);
	*z = gsgl_convert_token_to_0to1 (token) * (-GSGL_VIEW_DEPTH);
}

static void gsglfread_GetProjectedVertex2 (read_data *data, float *x, float *y, gsglPage *p)
{
	unsigned int token;
	float x1, y1, z1;
	
	read_token_fio (data, &token);
	x1 = gsgl_convert_token_to_0to1 (token) * gsgl_inq_window_width (p->win);
	read_token_fio (data, &token);
	y1 = gsgl_convert_token_to_0to1 (token) * gsgl_inq_window_height (p->win);
	gsgl_unproject_world_inv (x1, y1, 0.0, x, y, &z1, 
				  data->proj_matrix_inv, data->viewport_inv);
}

static void gsglfread_GetProjectedVertex3 (read_data *data, float *x, float *y, float *z,
					   gsglPage *p)
{
	unsigned int token;
	float x1, y1, z1;
	
	read_token_fio (data, &token);
	x1 = gsgl_convert_token_to_0to1 (token) * gsgl_inq_window_width (p->win);
	read_token_fio (data, &token);
	y1 = gsgl_convert_token_to_0to1 (token) * gsgl_inq_window_height (p->win);
	read_token_fio (data, &token);
	z1 = gsgl_convert_token_to_0to1 (token);
	gsgl_unproject_world_inv (x1, y1, z1, x, y, z, 
				  data->proj_matrix_inv, data->viewport_inv);
}

static void gsglfread_vertex (read_data *data, gsglPage *p, int datatype)
{
	gsgl_file_format_read_vertex_template (data, p, datatype, GSGL_TRUE,
					       read_token_fio, gsgl_file_format_read_float);
}
