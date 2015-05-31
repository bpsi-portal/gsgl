#include "gsaf.h"
#include "displaylist.h"
#include "utils.h"
#include "mem_list.h"
#include "drv_wrapper.h"

#define GSGL_DEBUG(x)

#define GSGL_FILEFORMAT_SIZE 50000 /* 100kB */
static void gsgl_mem_list_add_tokennum (gsglPage *);

/***************************/
/*  Initialize FileFormat  */ 
/***************************/
void gsgl_init_mem_list_every_page (gsglPage *p)
{
	p->buffer->memLargeBuffer = NULL;
	p->buffer->memCurrentList = NULL;
	p->buffer->memCurrentBuffer = NULL;
	p->buffer->memSize = 0;
}

void gsgl_mem_list_free_buffer (gsglPage *p)
{
	gsgllist *list;

	list = p->buffer->memLargeBuffer;
	while (list) {
		if (list->data) free (list->data);
		list = list->next;
	}
}

/*************************************/
/*  Get token number in FileFormat   */
/*************************************/
void gsgl_mem_list_inq_size_and_data (gsglDisplaylist *d, gsglPage *p)
{
	int i, n;
	gsgllist *list;

	GSGL_DEBUG(fprintf (stderr, "memlist inqsize\n"));

	d->size = p->buffer->memSize;
	if (!d->size || !gsgl_inq_drv_mem (p)) return;
	d->mem_list = (unsigned short *) malloc (d->size * sizeof(unsigned short));

	n = d->size / GSGL_FILEFORMAT_SIZE;
	list = p->buffer->memLargeBuffer;
	if (!list || !(list->data) || !d->mem_list) return;

	for (i=0; i<=n; i++) {
		if (i != n)
			memcpy (&(d->mem_list[i*GSGL_FILEFORMAT_SIZE]), 
				list->data, GSGL_FILEFORMAT_SIZE * sizeof(unsigned short));
		else
			memcpy (&(d->mem_list[i*GSGL_FILEFORMAT_SIZE]), 
				list->data, d->size%GSGL_FILEFORMAT_SIZE * sizeof(unsigned short));
		list = list->next;
	}
}

/*-------------------------------------------*/
/*  Check whether FileFormat buffer is fill  */
/*-------------------------------------------*/
static gsglboolean gsgl_mem_list_inq_buffer_is_full (gsglPage *p)
{
	int unsigned size;

	size = p->buffer->memSize;

	if (size % GSGL_FILEFORMAT_SIZE == 0 && size)
		return GSGL_TRUE;
	else
		return GSGL_FALSE;
}

/********************************/
/*  Allocate the 1st FileFormat */
/********************************/
void gsgl_mem_list_start (gsglPage *p)
{
	GSGL_DEBUG(fprintf (stderr, "memlist start\n"));
	p->buffer->memSize = 0;
	if (p->buffer->memLargeBuffer) {
		GSGL_DEBUG(fprintf (stderr, "large buffer != NULL\n"));
		p->buffer->memCurrentList = p->buffer->memLargeBuffer;
		p->buffer->memCurrentBuffer = p->buffer->memCurrentList->data;
	}
}

unsigned int gsgl_mem_list_read_token (unsigned short *mem, unsigned int count, unsigned int *token)
{
	*token = (unsigned int) mem[count];
	count++;
	return count;
}

/*---------------------------*/
/*  increment token number   */
/*---------------------------*/
static void gsgl_mem_list_add_tokennum (gsglPage *p)
{
	p->buffer->memSize++;
}

/*********************************/
/*  Put a token to FileFormat    */
/*********************************/
void gsgl_list_push (unsigned int token, gsglPage *p)
{
	unsigned short *data;
	gsglBuffers *b = p->buffer;

	if (!b->memLargeBuffer || (gsgl_mem_list_inq_buffer_is_full (p) && !b->memCurrentList->next)) {
		GSGL_DEBUG(fprintf (stderr, "new buffer\n"));
		data = (unsigned short *) malloc (GSGL_FILEFORMAT_SIZE * sizeof (unsigned short));
		b->memLargeBuffer = gsgl_list_append (b->memLargeBuffer, data);
		b->memCurrentList = gsgl_list_last (b->memLargeBuffer);
		b->memCurrentBuffer = b->memCurrentList->data;
	} else if (gsgl_mem_list_inq_buffer_is_full (p)) {
		GSGL_DEBUG(fprintf (stderr, "next buffer\n"));
		b->memCurrentList = b->memCurrentList->next;
		b->memCurrentBuffer = b->memCurrentList->data;
	}
	GSGL_DEBUG(fprintf (stderr, "token = %d Size = %d\n", token, b->memSize));
	b->memCurrentBuffer[b->memSize%GSGL_FILEFORMAT_SIZE] = (unsigned short) token;
	gsgl_mem_list_add_tokennum (p);
}

/***************************************/
/*  clear and free buffer from count   */
/***************************************/
void gsgl_mem_list_clear (unsigned int count)
{
	/*int i;
	  gsgllist *format, *tmp;
	  
	  Size = count;
	  ReadBufferNum = count/GSGL_FILEFORMAT_SIZE;
	  format = LargeBuffer;
	  for (i=0; i<ReadBufferNum; i++) {
	  if (format != NULL) {
	  format = format->next;
	  } else {
	  gsgl_error ("gsgl_mem_list_read_token", "format == NULL");
	  }
	  }
	  if (format == NULL) {
	  gsgl_error ("gsgl_mem_list_read_token", "format == NULL");
	  } else {
	  ReadBuffer = format->data;
	  }
	  tmp = format->next;
	  format->next = NULL;
	  
	  while (tmp != NULL) {
	  gsgl_free (tmp->data);
	  tmp = tmp->next;
	  }*/
}
