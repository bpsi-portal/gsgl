#include "gsaf.h"
#include "gsgl.h"
#include "gsgl2ps.h"
#include "fileio_base.h"
#include "fileio_read.h"
#include "utils.h"
#include "getopt.h"
#include "bsc1.h"

char *basename(char *name)
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

int main (int argc, char *argv[])
{
	int        c, errflg;
	int        istart, iend, combine, interactive = 1, i, j, imax=0, jmax=0;
	GLubyte    *start, *buffer;
	int        found = 0, m = 0, dp=0;
	float      dx=0.0, dy=0.0;
	int        total, orig=0, phys=1;
	gsglPageInfo *Info;
	char       file[256],prog[256],base[256],outputname[256];
	int        filesize = 0;
	gsglboolean old_header = GSGL_FALSE;
	gsglboolean blank_header = GSGL_FALSE;
	int        specify = 0;
	int        rot, color, gouraud, paper;
	int        eps = 0, len;
	int        pageno=0;
	GLfloat    *feedbackBuffer;
	float      matrix[16];
	gsglFileInfo *fileinfo;
  	extern char *optarg;
	extern int optind, optopt;
	int option_index = 0;
        static struct option long_options[] = {
            {"a4", no_argument, 0, 0},
            {"b5", no_argument, 0, 0},
            {"all", no_argument, 0, 'a'},
            {"blank", no_argument, 0, 'b'},
            {"color", no_argument, 0, 'c'},
            {"gouraud", no_argument, 0, 'g'},
            {"help", no_argument, 0, 'h'},
            {"level3", no_argument, 0, '3'},
            {"rotate", no_argument, 0, 'r'},
            {"title", no_argument, 0, 't'}
        };

	istart = 1;
	iend = 999;
	combine = 1;
	errflg = 0;
	rot = 0;
	color = 2;
	gouraud = 0;
	paper = PAPER_B5;

	while ((c = getopt_long 
		(argc, argv, ":abce:ghmo:p:rs:tz3", long_options, &option_index)) != -1)
		switch (c) {
		case 0:
			if (!strcmp (long_options[option_index].name, "a4")) {
				paper = PAPER_A4;
			} else if (!strcmp (long_options[option_index].name, "b5")) {
				paper = PAPER_B5;
			}
			break;
		case 'a':
			istart = 0;
			interactive = 0;
			break;
		case 'b':
			blank_header = GSGL_TRUE;
			break;
		case 'c':
			color = 2;
			break;
		case 'e':
			if (istart == 0)
				istart = 1;
			iend = atoi(optarg);
			interactive = 0;
			break;
		case 'g': /* postscript Lv.2 gouraud shading */
			gouraud = 1;
			break;
		case 'h':
			errflg++;
			break;
		case 'm':
			color = 0;
			break;
		case 'o':
			specify = 1;
			strcpy(outputname,optarg);
			break;
		case 'p':
			combine = atoi(optarg);
			break;
		case 'r':
			rot = 1;
			break;
		case 's':
			istart = atoi(optarg);
			interactive = 0;
			break;
		case 't':
			old_header = GSGL_TRUE;
			break;
		case 'z':
			color = 1;
			break;
		case '3': /* postscript Lv.3 gouraud shading */
			gouraud = 3;
			break;
		case ':':        /* -end without arguments */
			fprintf (stderr, "Option -%c requires an argument\n", optopt);
			errflg++;
			break;
		case '?':
			fprintf(stderr, "Unrecognized option: - %c\n", optopt);
			errflg++;
		}
	
	strcpy (prog, argv[0]);
	if (!strcmp (basename (prog), "gsgl2eps")) eps = 1;
	
	/* error message */
	gsgl2ps_message (errflg, argv);

	fileinfo = gsglfread_getfileinfo (argc, argv, optind);

	buffer = fileinfo->buffer;
	Info = fileinfo->pageinfo;
	total = fileinfo->pagenum;
	filesize = fileinfo->filesize;
	strcpy (file, fileinfo->filename);

	putenv ("GSGL_DEVICE=PS");
	putenv ("GSAF_BGCOLOR=white");
	putenv ("GSGDP=!c");

	gsaf_gsopen ();

	len = strlen (file);
	for (i = 0; i < len-5; i++) base[i] = file[i];
	base[len-5] = '\0';
	
	if (!eps) {
		if (!specify) {
		      sprintf( file, "%s.ps", base);
		      gsgl2ps_open (interactive, rot, color, file, eps, gouraud, paper);
		} else {
		      gsgl2ps_open (1, rot, color, outputname, eps, gouraud, paper);
		}
	}

	if (interactive) gsglfread_showpage(total, Info);

	for (;;) {
		found = gsglfread_getphysicalpage
			(total, &orig, &phys, Info, &istart, &iend, interactive);

		if (found == -1) break;
		for (phys=istart; phys<=iend; phys++) {
			if (found) {
			    pageno++;
			    if (eps) {
			       if (!specify) {
				   sprintf (file, "%s-%d.eps", base, pageno);
				   gsgl2ps_open (interactive,rot,color,file,eps,gouraud,paper);
			       } else {
				       sprintf (file, "%s", outputname);
				       gsgl2ps_open (1, rot, color, file, eps, gouraud,paper);
			       }
			    }
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
				imax = pow(2.0, 0.5*m);
				jmax = pow(2.0, 0.5*m);
			    } else if (m%2 == 1) {
				/* 2^m = 2,8,32,... */
				dx = GSGL_PAPER_WIDTH/pow (2.0, 0.5*(m+1));
				dy = GSGL_PAPER_HEIGHT/pow (2.0, 0.5*(m-1));
				imax = pow(2.0, 0.5*(m-1));
				jmax = pow(2.0, 0.5*(m+1));	
			    }
			    
			    for (i = 1 ; i <= imax ; i++) {
				    for (j = 1 ; j <= jmax ; j++) {
					    if (m%2 == 0) {
						    dp = (i-1)*pow(2.0,0.5*m)+j;
					    } else if (m%2 == 1) {
						    dp = (i-1)*pow(2.0,0.5*(m+1)) + j;
					    }
					    if (dp+phys-1 <= total && dp <= combine) {
						    start = buffer+Info[phys+dp-1].startpoint;
						    gsglfread_SetPicturePos (m, i, j, dx, dy, matrix);
						    gsglfread_page (filesize, start, old_header, combine, matrix, gsgl_inq_page());
						    glLoadIdentity();
					    }
				    }
			    }
			    phys += combine - 1; 

			    gsaf_pagee ();
			    
			    feedbackBuffer = gsgl2ps_getfeedbackbuffer (gsgl_inq_lib ());
			    if (!eps) gsgl2ps_page (pageno, feedbackBuffer);
			    else gsgl2ps_page (1, feedbackBuffer);
			    gsgl2ps_freefeedbackbuffer (gsgl_inq_lib ());
			    
			    if (eps) gsgl2ps_close(1);
			} else {
				printf(" ## PAGE %3d NOT FOUND\n",orig);
			}
		}
		if (!interactive) break;
		found = 0;
	}
	
	if (!eps) gsgl2ps_close(pageno);
	gsaf_gsclos ();
	gsgl_free (buffer);
	return 0;
}
