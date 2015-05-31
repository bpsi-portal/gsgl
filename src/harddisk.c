#include "gsaf.h"
#include "fileformat.h"
#include "fileio_write.h"
#include "utils.h"
#include "fileio_base.h"
#include "mem_list_write.h"
#include "device.h"
#include "displaylist.h"
#include "base.h"
#include "anime.h"
#include "harddisk.h"

static void gsgl_hd_add_tokennum (gsglPage *);
static void gsgl_hd_init_tokennum (gsglPage *);
static void gsgl_hd_swap (gsglPage *);

#define GSGL_HDBUFFER_SIZE 2000000 /* 8000kB */
#define GSGL_DEBUG(x)

/*****************************/
/*     Buffer の初期化       */
/*****************************/
void gsgl_init_hd_every_page (gsglboolean use_hd, gsglPage *p)
{
	if (use_hd) {
		p->buffer->Hd = gsgl_get_fp_write (p->library);
		p->buffer->Write = gsgl_init_write_to_file ();
		gsgl_hd_init_tokennum (p);
		p->buffer->Wrote_signature = GSGL_FALSE;
		p->buffer->Hd_buffer = (unsigned short *) malloc (sizeof (unsigned short)*GSGL_HDBUFFER_SIZE);
	} else {
		p->buffer->Hd_buffer = NULL;
	}
}

typedef struct _animedata
{
	FILE *fp;
	gsglPage *p;
} animedata;

static void anime_internal (gsglDisplaylist *list, gsglPage *p, void *data1)
{
	animedata *data = (animedata *) data1;

	if (list->type == GSGL_LIST_LIGHTING_ENABLE) {
		if (gsgl_inq_lighting_is_enabled (data->p) && list->size != 0) {
			GSGL_DEBUG(fprintf (stderr, "enable lighting\n"));
			gsgl_mem_list_write (list->mem_list, list->size, data->fp, p->buffer->Write);
		}
	} else if (list->type == GSGL_LIST_LIGHTING_DISABLE) {
		if (gsgl_inq_lighting_is_enabled (data->p) && list->size != 0) {
			GSGL_DEBUG(fprintf (stderr, "disable lighting\n"));
			gsgl_mem_list_write (list->mem_list, list->size, data->fp, p->buffer->Write);
		}
	} else if (list->size != 0) {
		GSGL_DEBUG(fprintf (stderr, "else (type=%d)\n", list->type));
		gsgl_mem_list_write (list->mem_list, list->size, data->fp, p->buffer->Write);
	}
}

void gsgl_init_hd_for_anime (gsglPage *p)
{
	gsgllist *list;
	animedata *data;

	p->buffer->Hd = ANIME(p)->Fp;
	p->buffer->Write = gsgl_init_write_to_file ();
	gsgl_hd_init_tokennum (p);
	p->buffer->Wrote_signature = GSGL_TRUE;
	p->buffer->Hd_buffer = (unsigned short *) malloc (sizeof(unsigned short)*GSGL_HDBUFFER_SIZE);

	list = gsgl_displaylist_inq_start (p);

	fprintf (ANIME(p)->Fp, "GSAF/OpenGL0");
	p->buffer->Write->byte += 12;

	data = gsgl_malloc (sizeof(animedata));
	data->fp = ANIME(p)->Fp;
	data->p = p;

	while (list) {
		gsgl_lists_call_template (list->data, p, data, anime_internal);
		list = list->next;
	}
}

void gsgl_finish_hd_every_page (gsglPage *p)
{
	unsigned int fsize[2];

	if (p->buffer->Hd_buffer) {
		gsgl_hd_swap (p);

		gsglfwrite_write (GSGL_FILE_END_PAGE, p->buffer->Hd, p->buffer->Write);
		if (p->buffer->Write->surplus != 0) gsglfwrite_write (0, p->buffer->Hd, p->buffer->Write);
		if (p->buffer->Write->surplus != 0) gsglfwrite_write (0, p->buffer->Hd, p->buffer->Write);
		
		fseek (p->buffer->Hd, -((long)(p->buffer->Write->byte))+8+12, SEEK_CUR);
		fsize[1] = (unsigned int) (p->buffer->Write->byte/(256*256));
		fsize[0] = p->buffer->Write->byte - 256 * 256 * fsize[1];
		gsglfwrite_write (GSGL_FILE_PAGE_SIZE, p->buffer->Hd, p->buffer->Write);
		gsglfwrite_write (fsize[0], p->buffer->Hd, p->buffer->Write);
		gsglfwrite_write (fsize[1], p->buffer->Hd, p->buffer->Write);
		fseek (p->buffer->Hd, 0, SEEK_END);
		p->buffer->Write->byte -= 8;
		free (p->buffer->Hd_buffer);

		printf (" # PAGE %5d WAS SAVED.   (SAVED BYTES : %10.3f KB)\n",
			gsgl_inq_page_number (p->library), p->buffer->Write->byte/1000.0);
	}
}

void gsgl_end_hd_for_anime (gsglPage *p)
{
	gsgl_hd_swap (p);
	gsgl_hd_init_tokennum (p);
}

void gsgl_finish_hd_for_anime (gsglPage *p)
{
	unsigned int fsize[2];

	if (gsgl_inq_anime_is_used (p) && p->buffer->Hd_buffer != NULL) {
		gsgl_hd_swap (p);

		gsglfwrite_write (GSGL_FILE_END_PAGE, p->buffer->Hd, p->buffer->Write);
		if (p->buffer->Write->surplus != 0) gsglfwrite_write (0, p->buffer->Hd, p->buffer->Write);
		if (p->buffer->Write->surplus != 0) gsglfwrite_write (0, p->buffer->Hd, p->buffer->Write);
        
		fseek (p->buffer->Hd, -((long)(p->buffer->Write->byte))+8+12, SEEK_CUR);
		fsize[1] = (unsigned int) (p->buffer->Write->byte/(256*256));
		fsize[0] = p->buffer->Write->byte - 256 * 256 * fsize[1];
		gsglfwrite_write (GSGL_FILE_PAGE_SIZE, p->buffer->Hd, p->buffer->Write);
		gsglfwrite_write (fsize[0], p->buffer->Hd, p->buffer->Write);
		gsglfwrite_write (fsize[1], p->buffer->Hd, p->buffer->Write);
		fseek (p->buffer->Hd, 0, SEEK_END);
		p->buffer->Write->byte -= 8;
		free (p->buffer->Hd_buffer);
		p->buffer->Hd_buffer = NULL;
	}
}

unsigned long int gsgl_hd_ftell (int *surplus, gsglPage *p)
{
	int tmp;
	unsigned long int size = 0;

	tmp = ftell(p->buffer->Hd);
	if (tmp>=0) size = tmp;
	else gsgl_error ("gsgl_hd_ftell", "ftell returns negative");
        tmp = (p->buffer->Hd_token+1)*2+p->buffer->Write->surplus;
	size += (tmp/3)*4;
	*surplus = tmp%3;
	return size;
}

/**************************************************/
/*  hd bufferのmemoryに入れられたtoken数を求める  */
/**************************************************/
static int gsgl_hd_inq_token_num (gsglPage *p)
{
	return p->buffer->Hd_token;
}

/*****************************************************************/
/*  FileFormat のstackに入れられた token 数が満杯かどうか調べる  */
/*****************************************************************/
static gsglboolean gsgl_hd_inq_buffer_is_full (gsglPage *p)
{
	if (p->buffer->Hd_token == GSGL_HDBUFFER_SIZE-1)
		return GSGL_TRUE;
	else
		return GSGL_FALSE;
}

/*-----------------------------------------*/
/*  Hd_bufferに入れられたtoken数を1増やす  */
/*-----------------------------------------*/
static void gsgl_hd_add_tokennum (gsglPage *p)
{
	p->buffer->Hd_token++;
}

/*-----------------------------------------*/
/*  Hd_bufferに入れられたtoken数をリセット */
/*-----------------------------------------*/
static void gsgl_hd_init_tokennum (gsglPage *p)
{
	p->buffer->Hd_token = -1;
}

static void gsgl_hd_swap (gsglPage *p)
{
	int i;
	unsigned int token;

	if (!p->buffer->Wrote_signature) {
		fprintf (p->buffer->Hd, "GSAF/OpenGL0");
		p->buffer->Write->byte += 12;
		p->buffer->Wrote_signature = GSGL_TRUE;
	}
	for (i=0; i<=p->buffer->Hd_token; i++) {
		token = p->buffer->Hd_buffer[i];
		gsglfwrite_write (token, p->buffer->Hd, p->buffer->Write);
	}
}

/*********************************/
/*  Hd_bufferにtokenをpushする   */
/*********************************/
void gsgl_hd_push (unsigned int token, gsglPage *p)
{
	if (p->buffer->Hd_buffer == NULL) {
		gsgl_error ("gsgl_hd_push", "p->buffer->Hd_buffer == NULL");
		return;
	} else if (token > 65535) {
		gsgl_warning ("gsgl_hd_push", "token = %d > 65535", token);
		return;
	}
	gsgl_hd_add_tokennum (p);

	p->buffer->Hd_buffer[gsgl_hd_inq_token_num (p)] = token;

	if (gsgl_hd_inq_buffer_is_full (p)) {
		gsgl_hd_swap (p);
		gsgl_hd_init_tokennum (p);
	}
}
