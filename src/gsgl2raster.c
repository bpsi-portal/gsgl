#include "gsaf.h"
#include "gsgl.h"
#include "device.h"
#include "fileio_base.h"
#include "fileio_read.h"
#include "raster.h"
#include "utils.h"
#include "bsc1.h"

void * gsgl2raster_get_image (gsglLibrary *);
static void gsgl2raster_message(int errflg, char *argv[]);
static char *basename(char *name);

int main (int argc, char *argv[])
{
	int          type, c, errflg;
	int          istart, iend, combine, interactive = 1, i, j, imax=0, jmax=0;
	GLubyte      *start, *buffer;
	int          found = 0, m = 0, delta = 0;
	float        dx = 0.0, dy = 0.0;
	int          total, orig = 0, phys = 1;
	gsglPageInfo *info;
	gsglFileInfo *fileinfo;
	char         file[256], base[256], outputname[256];
	unsigned int filesize;
	gsglboolean  old_header = GSGL_FALSE, blank_header = GSGL_FALSE;
	int          specify_out = 0;
	int          color;
	int          len;
	void         *image_buffer;
	int          width = GSGLBASIC_WINDOWWIDTH;
	int          height = GSGLBASIC_WINDOWHEIGHT;
	int          pageno = 0;
	char         str[256];
	float        matrix[16];
	extern char *optarg;
	extern int optind, optopt;
	
	istart = 1;
	iend = 999;
	combine = 1;
	errflg = 0;
	color = 1;
	
	while ((c = getopt(argc, argv, ":abce:hmo:p:s:tH:W:")) != -1)
		switch (c) {    
		case 'a':
			istart = 0;
			interactive = 0;
			break;
		case 'b':
			blank_header = GSGL_TRUE;
			break;
		case 'c':
			color = 1;
			break;
		case 'e':
			if (istart == 0) istart = 1;
			iend = atoi(optarg);
			interactive = 0;
			break;
		case 'h':
			errflg++;
			break;
		case 'm':
			color = 0;
			break;
		case 'o':
			specify_out = 1;
			strcpy(outputname,optarg);
			break;
		case 'p':
			combine = atoi(optarg);
			break;
		case 's':
			istart = atoi(optarg);
			interactive = 0;
			break;
		case 't':
			old_header = GSGL_TRUE;
			break;
		case 'H':
			height = atoi(optarg);
			break;
		case 'W':
			width = atoi(optarg);
			break;
		case ':':        /* -end without arguments */
			fprintf(stderr, "Option -%c requires an argument\n", optopt);
			errflg++;
			break;
		case '?':
			fprintf(stderr, "Unrecognized option: - %c\n", optopt);
			errflg++;
		}
	
	type = RASTER_NON;
	if(!strcmp(basename(argv[0]),"gsgl2rastereps")) type = RASTER_EPS;
	if(!strcmp(basename(argv[0]),"gsgl2png")) type = RASTER_PNG;
	if(!strcmp(basename(argv[0]),"gsgl2jpeg")) type = RASTER_JPG;
	if (type == RASTER_NON)
		gsgl_error ("main", "Don't call this program directly");
	/* error message */
	gsgl2raster_message (errflg, argv);
	
	fileinfo = gsglfread_getfileinfo (argc, argv, optind);

	buffer = fileinfo->buffer;
	info = fileinfo->pageinfo;
	total = fileinfo->pagenum;
	filesize = fileinfo->filesize;
	strcpy (file, fileinfo->filename);

	putenv("GSGL_DEVICE=RASTER");
	sprintf (str, "GSGL_HEIGHT=%d", height);
	putenv(str);
	sprintf (str, "GSGL_WIDTH=%d", width);
	putenv(str);
	putenv("GSGDP=!c");

	gsaf_gsopen ();	
	if (interactive) gsglfread_showpage (total, info);
	
	len = strlen (file);
	for (i = 0; i<len-5; i++) base[i] = file[i];
	base[len-5] = '\0';
	for (;;) {
		found = gsglfread_getphysicalpage (total, &orig, &phys, info,
						   &istart, &iend, interactive);
		if (found == -1) break;
		if (found == 0) {
			printf(" ## PAGE %3d NOT FOUND\n", orig);
			iend = -1;
		}
		for (phys = istart; phys<=iend; phys++) {
			pageno ++;
			if (!specify_out) {
				if (type == RASTER_EPS)
					sprintf (file, "%s-%d.eps",base,pageno);
				if (type == RASTER_PNG)
					sprintf (file, "%s-%d.png",base,pageno);
				if (type == RASTER_JPG)
					sprintf (file, "%s-%d.jpg",base,pageno);
			} else
				sprintf (file, "%s", outputname);

			if (blank_header || (old_header && combine > 1)) {
				gsaf_gstitl ("//");
			} else if (old_header && combine == 1) {
				gsgl_keep_header ();
			}
			gsaf_pages ();
				/* 2^(m-1) <= combine <= 2^m */
			m = -1;
			do {
				m ++;
			} while (combine > pow(2.0, (double) m));
			
			if (m%2 == 0) {
				/* 2^m = 1,4,16,... */
				dx = GSGL_PAPER_WIDTH/pow (2.0, 0.5*m);
				dy = GSGL_PAPER_HEIGHT/pow (2.0, 0.5*m);
				imax = pow (2.0, 0.5*m);
				jmax = pow (2.0, 0.5*m);
			} else if (m%2 == 1) {
				/* 2^m = 2,8,32,... */
				dx = GSGL_PAPER_WIDTH/pow(2.0,0.5*(m+1));
				dy = GSGL_PAPER_HEIGHT/pow(2.0,0.5*(m-1));
				imax = pow(2.0, 0.5*(m-1));
				jmax = pow(2.0, 0.5*(m+1));	
			}
			
			for (i = 1; i <= imax; i++) {
			for (j = 1; j <= jmax; j++) {
			if (m%2 == 0) {
				delta = (i-1)*pow(2.0,0.5*m) + j;
			} else if (m%2 == 1) {
				delta = (i-1)*pow(2.0,0.5*(m+1)) + j;
			}
			if (delta+phys-1 <= total && delta <= combine) {
				start = buffer+info[phys+delta-1].startpoint;
				gsglfread_SetPicturePos (m, i, j, dx, dy, matrix);
				gsglfread_page (filesize, start, old_header, combine, matrix, gsgl_inq_page());
				glLoadIdentity();
			}
			}
			}
			phys += combine - 1; 
			gsaf_pagee ();
			
			image_buffer = gsgl2raster_get_image (gsgl_inq_lib ());
			gsgl2raster_open (file);
			gsgl2raster_page (type, color, width, height, image_buffer);
		}
		if (!interactive) break;
		found = 0;
	}
	
	gsaf_gsclos ();
	gsgl_free (buffer);
	return 0;
}

static char *basename (char *name)
{
	char *base;
	unsigned len;
	
	base = NULL;
	for (len = strlen (name); len > 0; len--) {
		if (name[len - 1] == '/') {
			base = name + len;
			break;
		}}
	if (!base)
		base = name;
	
	return base;
}

/*------------------------------------------------------------*/
/*                gsgl2raser Error message                    */
/*-------------------------------------------------------------*/
static void gsgl2raster_message(int errflg, char *argv[])
{
	if (errflg) {
		fprintf(stderr, "usage: %s [-a] [-s ps] [-e pe] [-p np] [-t] "
			"[-b] [-o output] [filename]\n", argv[0]);
		fprintf(stderr, "           -a    : show all page\n");
		fprintf(stderr, "           -s ps : show from page ps [1]\n");
		fprintf(stderr, "           -e pe : show until page pe [999]\n");
		fprintf(stderr, "           -p np : combine np pages on a sheet [1]\n");
		fprintf(stderr, "           -t    : keep original page title\n");
		fprintf(stderr, "           -b    : no page title\n");
		fprintf(stderr, "           -c    : color figure (valid only with gsgl2rastereps)\n");
		fprintf(stderr, "           -m    : monochrome figure (valid only with gsgl2rastereps)\n");
		fprintf(stderr, "           -H    : image height\n");
		fprintf(stderr, "           -W    : image width\n");
		fprintf(stderr, "           -o output : output file name\n");
		fprintf(stderr, "           filename  : if not specified, prompted\n");
		exit (2);
	}
}
