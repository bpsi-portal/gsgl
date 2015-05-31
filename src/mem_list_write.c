#include "gsaf.h"
#include "fileformat.h"
#include "mem_list.h"
#include "fileio_write.h"

/***********************************************/
/*       Write out FileFormat to a file        */
/***********************************************/
void gsgl_mem_list_write (unsigned short *mem, unsigned int size, FILE *fp, gsglWriteToFile *w)
{
	unsigned int count;
	unsigned int token;

	count = 0;
	while (count < size) {
		count = gsgl_mem_list_read_token (mem, count, &token);
		gsglfwrite_write (token, fp, w);
	}
}
