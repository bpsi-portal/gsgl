#ifndef __fileio_read_h__
#define __fileio_read_h__

#ifndef SONYCISC
#include <unistd.h>
#include <stdlib.h>
#else
#define  R_OK  4
extern char *getenv();
#endif

#ifndef NULL
#define NULL ((void *)0)
#endif

typedef struct _gsglPageInfo {
	int   startpoint;
	int   pagefilesize;
	int   pagenumber;
} gsglPageInfo;

typedef struct _gsglFileInfo {
	unsigned int pagenum;
	unsigned int filesize;
	unsigned char *buffer;
	char filename[256];
	gsglboolean gzip;
	gsglPageInfo *pageinfo;
} gsglFileInfo;

gsglFileInfo * gsglfread_getfileinfo (int argc, char *argv[], int optind);
void gsglfread_page (unsigned int size, unsigned char *buffer, 
		     gsglboolean old_header, int inum, float matrix[16], gsglPage *p);
void gsglfread_showpage (GLint totalpage, gsglPageInfo Info[]);
void gsglfread_SetPicturePos (int m, int i, int j, GLfloat delta_x, GLfloat delta_y, 
			      float matrix[16]);
void gsglfread_getfilename (int argc, char *argv[], int optind, gsglFileInfo *info);
int  gsglfread_getphysicalpage (int totalpage,int *originalpagenum,
				int *physicalpagenum, gsglPageInfo Info[],
				int *istart, int *iend,int interactive);

#endif
