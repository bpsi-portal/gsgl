#include "gsaf.h"
#include "gsgl.h"
#include "fileio_read.h"
#include "fileio_base.h"
#include "bsc1.h"
#include "utils.h"

static void gsglView_message(GLint errflg, char *argv[]);

int main (int argc, char *argv[])
{
	int          c, errflg;
	int          istart, iend, combine;
	int          interactive = 1;
	int          i, j, imax=0, jmax=0;
	unsigned char *startpoint;
	int          found = 0, m = 0, delta_page=0;
	float        delta_x=0.0, delta_y=0.0;
	unsigned char *buffer;
	int          filesize=0;
	int          old_header = GSGL_FALSE;
	int          blank_header = GSGL_FALSE;
	int          total, orig=0, phys=1;
	float        matrix[16];
	gsglPageInfo *Info;
	gsglFileInfo *fileinfo;
	char         *str;
	extern char *optarg;
	extern int optind, optopt;
	
	istart = 1;
	iend = 999;
	combine = 1;
	errflg = 0;
	
	while ((c = getopt(argc, argv, ":abe:p:s:t")) != -1)
		switch (c) {
		case 'a':
			istart = 0;
			interactive = 0;
			break;
		case 'b':
			blank_header = GSGL_TRUE;
			break;
		case 'e':
			if (istart == 0)
				istart = 1;
			iend = atoi(optarg);
			interactive = 0;
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
		case ':':        /* -ens without arguments */
			fprintf(stderr, "Option -%c requires an argument\n", optopt);
			errflg++;
			break;
		case '?':
			fprintf(stderr, "Unrecognized option: - %c\n", optopt);
			errflg++;
		}
	
	/* error message */
	gsglView_message (errflg, argv);
	
	fileinfo = gsglfread_getfileinfo (argc, argv, optind);

	buffer = fileinfo->buffer;
	Info = fileinfo->pageinfo;
	total = fileinfo->pagenum;
	filesize = fileinfo->filesize;
	str = (char *) gsgl_malloc (sizeof(char)*(25+strlen(fileinfo->filename)+1));
	sprintf (str, "GSGL_TITLE=GSAF/OpenGL (%s)", fileinfo->filename);
	putenv (str);

	gsaf_gsopen ();
	if (interactive) gsglfread_showpage (total, Info);

	for (;;) {
		
	    found = gsglfread_getphysicalpage (total, &orig, &phys, Info, &istart, &iend, interactive);
	    if (found == -1) break;
	    for (phys=istart ; phys<=iend ;phys++){
		if (found) {
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
			/* 2^m = 4,16,64,... */
			delta_x = GSGL_PAPER_WIDTH/pow (2.0, 0.5*(double) m);
			delta_y = GSGL_PAPER_HEIGHT/pow (2.0, 0.5*(double)m);
			imax = pow(2.0, 0.5*(float) m);
			jmax = pow(2.0, 0.5*(float) m);
		    } else if (m%2 == 1) {
			/* 2^m = 2,8,32,... */
			delta_x = GSGL_PAPER_WIDTH/pow(2.0,0.5*(double)(m+1));
			delta_y = GSGL_PAPER_HEIGHT/pow(2.0,0.5*(double)(m-1));
			imax = pow(2.0, 0.5*((double)m-1.0));
			jmax = pow(2.0, 0.5*((double)m+1.0));	
		    }
		    
		    for (i = 1 ; i <= imax ; i++) {
			for (j = 1 ; j <= jmax ; j++) {
			    if (m%2 == 0) {
				    delta_page = (i-1)*pow(2.0,0.5*(double) m) + j;
			    } else if (m%2 == 1) {
				    delta_page = (i-1)*pow(2.0,0.5*((double) m + 1.0 )) + j;
			    }
			    if (delta_page+phys-1 <= total&&delta_page<=combine){
				startpoint = buffer+Info[phys+delta_page-1].startpoint;
				gsglfread_SetPicturePos (m, i, j, delta_x, delta_y, matrix);
				gsglfread_page (filesize, startpoint, old_header, combine, matrix, gsgl_inq_page());
				/*glLoadIdentity ();*/
			    }
			}
		    }
		    phys += combine - 1; 
		    gsaf_pagee ();
		} else {
			printf(" ## PAGE %3d NOT FOUND\n",orig);
		}
	    }
	    if (!interactive) break;
	    found = 0;
	}
	gsaf_gsclos ();
	return 0;
}

static void gsglView_message(GLint errflg, char *argv[])
{
	if (errflg) {
		fprintf (stderr, "usage: %s [-a] [-s ps] [-e pe] [-p np] [-t] [-b] [filename]\n", argv[0]);
		fprintf (stderr, "           -a    : show all page\n");
		fprintf (stderr, "           -s ps : show from page ps [1]\n");
		fprintf (stderr, "           -e pe : show until page pe [999]\n");
		fprintf (stderr, "           -p np : combine np pages on a sheet [1]\n");
		fprintf (stderr, "           -t    : keep original page title\n");
		fprintf (stderr, "           -b    : no page title\n");
		fprintf (stderr, "           filename : if not specified, prompted\n");
		exit (1);
	}
}
