#include "gsaf.h"
#include "base.h"
#include "fileformat.h"
#include "fileio_base.h"
#include "fileio_write.h"
#include "ogl.h"
#include "displaylist.h"
#include "matrix.h"
#include "mem_list_write.h"
#include "device.h"
#include "xwin.h"
#include "anime.h"
#include "utils.h"
#include "guiitems.h"

const static unsigned int i1B=256, i2B=256*256;
const static unsigned int i6b=64, i12b=64*64, i18b=64*64*64;
const static char BASE64[64] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 
				'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 
				'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd',
				'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 
				'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x',
				'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7',
				'8', '9', '+', '/'};

gsglWriteToFile * gsgl_init_write_to_file()
{
	GLuint  i4 = 1;
	GLubyte *i1 = (GLubyte *) &i4;
	gsglWriteToFile *w;
	
	w = (gsglWriteToFile *) gsgl_malloc(sizeof(gsglWriteToFile));

	w->little_endian = *i1;
	w->byte = 0;
	w->surplus = 0;
	w->bytebuffer[0] = 0;
	w->bytebuffer[1] = 0;

	return w;
}

void gsgl_save(int *size, gsglPage *p)
{	
	gsglWriteToFile *w;

	if (gsgl_inq_open_tmp_file_for_anime(p)) {
		gsgl_copy_anime_file_to_save_file(p);
	} else {
		w = gsgl_init_write_to_file();
		gsgl_write_to_file(gsgl_get_fp_write(p->library), w, p);
		*size = w->byte;
		gsgl_free(w);
	}
}

typedef struct _data_write {
	FILE *fp;
	gsglWriteToFile *w;
} data_write;

static void write_to_file_internal(gsglDisplaylist *list, gsglPage *p, void *data1)
{
	data_write *data = (data_write *) data1;

	if (list->type == GSGL_LIST_LIGHTING_ENABLE) {
		if (gsgl_inq_lighting_is_enabled(p) && list->size != 0) {
				/*printf("enable lighting\n");*/
			gsgl_mem_list_write(list->mem_list, list->size, data->fp, data->w);
		}
	} else if (list->type == GSGL_LIST_LIGHTING_DISABLE) {
		if (gsgl_inq_lighting_is_enabled(p) && list->size != 0) {
				/*printf("disable lighting\n");*/
			gsgl_mem_list_write(list->mem_list, list->size, data->fp, data->w);
		}
	} else if (list->size != 0) {
		/*printf("else (type=%d)\n", list->type);*/
		gsgl_mem_list_write(list->mem_list, list->size, data->fp, data->w);
	}
}

void gsgl_write_to_file(FILE *fp, gsglWriteToFile *w, gsglPage *p)
{
	gsgllist *list;
	data_write *data;	
	unsigned int fsize[2];

	if (gsgl_displaylist_inq_remade(p)) {
		if (gsgl_inq_mode(p->library) == GSGL_MODE_TCPIP)
			gsgl_displaylist_remake_for_save_tcpip(p);
		else
			gsgl_displaylist_remake_for_save(p);
	}
	if (gsgl_displaylist_inq_guiitems(p))
		gsgl_guiitems_update_memlist(p);

	list = gsgl_displaylist_inq_start(p);

	fprintf(fp, "GSAF/OpenGL0");
	w->byte += 12;

	data = (data_write *) gsgl_malloc(sizeof(data_write));
	data->fp = fp;
	data->w = w;
	
	while (list) {	
		gsgl_lists_call_template(list->data, p, data, write_to_file_internal);
		list = list->next;
	}
	gsglfwrite_write(GSGL_FILE_END_PAGE, fp, w);
	if (w->surplus != 0) gsglfwrite_write(0, fp, w);
	if (w->surplus != 0) gsglfwrite_write(0, fp, w);
	
	/* write file size next to the page number */
	fseek(fp, -((long) (w->byte))+8+12, SEEK_CUR);
	/* 12bytes are "GSAF/OpenGL0", 8bytes are from here to the pagenumber */
	fsize[1] = (unsigned int) (w->byte/(256*256));
	fsize[0] = w->byte - 256 * 256 * fsize[1];
	gsglfwrite_write(GSGL_FILE_PAGE_SIZE, fp, w);
	gsglfwrite_write(fsize[0], fp, w);
	gsglfwrite_write(fsize[1], fp, w);
	fseek(fp, 0, SEEK_END);
	w->byte -= 8;
}

void gsglfwrite_write(unsigned int i, FILE *fp, gsglWriteToFile *w)
{
	unsigned short i2;
	unsigned char *i1;
	unsigned int   i4;
	unsigned int   c1, c2, c3, c4;
	
	w->surplus += 2;
	w->surplus %= 3;
	
	/*fprintf(stdout, "%d\n", i4);*/
	i2 = i;
	i1 = (unsigned char *) &i2;
	
	if (w->surplus == 2) {
		if (w->little_endian) {
			w->bytebuffer[0] = *i1; i1++;
			w->bytebuffer[1] = *i1;
		} else {
			w->bytebuffer[1] = *i1; i1++;
			w->bytebuffer[0] = *i1;
		}
	} else if (w->surplus == 1) {
		if (w->little_endian) {
			i4 = w->bytebuffer[0]+w->bytebuffer[1]*i1B+(*i1)*i2B;
			i1++;
			w->bytebuffer[0] = *i1;
		} else {
			i1++;
			i4 = w->bytebuffer[0]+w->bytebuffer[1]*i1B+(*i1)*i2B;
			i1--;
			w->bytebuffer[0] = *i1;
		}
		c4 = (unsigned int) i4/i18b;
		i4 -= c4*i18b;
		c3 = (unsigned int) i4/i12b;
		i4 -= c3*i12b;
		c2 = (unsigned int) i4/i6b;
		i4 -= c2*i6b;
		c1 = i4;
		fprintf(fp, "%c", BASE64[c1]);
		fprintf(fp, "%c", BASE64[c2]);
		fprintf(fp, "%c", BASE64[c3]);
		fprintf(fp, "%c", BASE64[c4]);
		w->byte += 4;
	} else if (w->surplus == 0) {
		if (w->little_endian) {
			i4 = w->bytebuffer[0]+(*i1)*i1B; i1++;
			i4 += (*i1)*i2B;
		} else {
			i1++;
			i4 = w->bytebuffer[0]+(*i1)*i1B;
			i1--;
			i4 += (*i1)*i2B;
		}
		c4 = (unsigned int) i4/i18b;
		i4 -= c4*i18b;
		c3 = (unsigned int) i4/i12b;
		i4 -= c3*i12b;
		c2 = (unsigned int) i4/i6b;
		i4 -= c2*i6b;
		c1 = i4;
		fprintf(fp, "%c", BASE64[c1]);
		fprintf(fp, "%c", BASE64[c2]);
		fprintf(fp, "%c", BASE64[c3]);
		fprintf(fp, "%c", BASE64[c4]);
		w->byte += 4;
	}
}
