#include "gsaf.h"
#include "gsgl.h"
#include "internal.h"
#include "gsgl2ps.h"
#include "utils.h"
#include <ctype.h>

static FILE  *psfile;

/********************************************************************/
/*          Internal Format を Postscript File へ変換する           */
/********************************************************************/

#define GSGL2PS_HEADER 1
#define GSGL2PS_3DPRIM_NUM 2
#define GSGL2PS_3DPRIM_DEPTH 3
#define GSGL2PS_2D 4
#define GSGL2PS_3D 5

#define GSGL_DEPTH_BY_DEEP    0
#define GSGL_DEPTH_BY_SHALLOW 1
#define GSGL_DEPTH_BY_MEAN    2
#define GSGL_DEPTH_BY_POINT   3

#define CM (72.0/2.54)
#define DEG (M_PI/180.0)

typedef struct _DepthIndex {
	GLfloat *ptr;
	GLfloat depth;
	GLfloat linewidth;
	GLint   OnlyGouraud;
	float   modelview[16];
	float   projection[16];
} DepthIndex;

typedef struct _Psstatus {
	float pixel2point;
	gsglboolean now3d;
	gsglboolean graudonly;
	float       linewidth;
	gsglrgb     polycolor;
	gsglrgb     linecolor;
	float       offset;
	int nprimitives;
	int item;
	DepthIndex *prims;
	int depthtype;
	float modelview[16];
	float projection[16];
} Psstatus;

typedef struct _Feedback3Dcolor {
	GLfloat x;
	GLfloat y;
	GLfloat z;
	GLfloat red;
	GLfloat green;
	GLfloat blue;
	GLfloat alpha;
} Feedback3Dcolor;

static gsglboolean   g_ifused [13]; /* list of used ps fonts */
static GLint   g_icolor = 0, g_irotate = 0, g_gouraud = 0, Paper = PAPER_B5; 
static gsglboolean Eps;
static GLint   g_imv = 0, g_ipoly = 0, g_mpoly = 0;
static GLfloat g_xpos_line, g_ypos_line, g_xpos_poly[1024], g_ypos_poly[1024];
static GLfloat g_depth_of_prim = -(GSGLBASIC_EYEPOSITION+GSGL_VIEW_DEPTH);

void gsgl2ps_page (int pageno, GLfloat *feedbackBuffer);
static GLfloat *gsgl2ps_page_header (GLfloat *loc);
static GLfloat *gsgl2ps_page_prim (GLfloat *loc, int dim, Psstatus *status);
static void gsgl2ps_page_2D (GLfloat *loc);
static void gsgl2ps_page_3D (GLfloat *loc);
static GLfloat ps_x (GLfloat x, GLfloat y, Psstatus *);
static GLfloat ps_y (GLfloat x, GLfloat y, Psstatus *);
static void Unexpected_token (char token[], char function[]);
static int  gsgl2ps_3dprim_num (GLfloat *loc, Psstatus *);
static void gsgl2ps_poly_header (float, float, float, Psstatus *);
static void gsgl2ps_drawpoly_buffer (Psstatus *);
static void gsgl2ps_drawpoly (Feedback3Dcolor vertex[], int nvertices, Psstatus *);
static void gsgl2ps_drawline (Feedback3Dcolor vertex[], Psstatus *);
static void gsgl2ps_page_start (int pageno);
static void gsgl2ps_page_end (void);
static int gsgl2ps_3dprim_depth (GLfloat *loc, Psstatus *status);
static GLfloat * gsgl2ps_Line (int type, GLfloat *loc, Psstatus *);
static GLfloat * gsgl2ps_Poly (int type, GLfloat *loc, Psstatus *);
static GLfloat * gsgl2ps_Point (int type, GLfloat *loc, Psstatus *);
static GLfloat *  gsgl2ps_PassThrough (int type, GLfloat *loc, GLint *iexit, Psstatus *);
static void gsgl2ps_Line_base (Feedback3Dcolor  *vertex, Psstatus *);
static void gsgl2ps_Poly_base (Feedback3Dcolor  *vertex, int nvertices, Psstatus *);
static int gsgl2ps_check_groud (Feedback3Dcolor  *vertex, int nvertices,
				int *color_not_change_in_poly, Psstatus *status);
static GLfloat * gsgl2ps_pstext (int type, GLfloat *loc, Psstatus *status);

/*******************************************************************/
/*                   Error messages of gsgl2ps                     */
/*******************************************************************/
void gsgl2ps_message (GLint errflg, char *argv[])
{
	if (errflg) {
		fprintf(stderr, "usage: %s [-atbrcmgz3] [-s ps] [-e pe] [-p np]"
			"[-o output] [filename]\n", argv[0]);
		fprintf(stderr, "           -a    : convert all pages\n");
		fprintf(stderr, "           -s ps : convert from page ps [1]\n");
		fprintf(stderr, "           -e pe : convert until page pe [999]\n");
		fprintf(stderr, "           -g    : gouraud shading (postscirpt Lv.2)\n");
		fprintf(stderr, "           -h    : print this message\n");
		fprintf(stderr, "           -3    : gouraud shading (postscirpt Lv.3)\n");
		fprintf(stderr, "           -p np : combine np pages on a sheet [1]\n");
		fprintf(stderr, "           -t    : keep original page title\n");
		fprintf(stderr, "           -b    : no page title\n");
		fprintf(stderr, "           -r    : rotate figure \n");
		fprintf(stderr, "           -c    : color figure (default)\n");
		fprintf(stderr, "           -z    : gray figure \n");
		fprintf(stderr, "           -m    : monochrome figure \n");
		fprintf(stderr, "           -o output : specify output file name\n");
		fprintf(stderr, "           --a4      : fit the image to a4 paper\n");
		fprintf(stderr, "           --b5      : fit the image to b5 paper (default)\n");
		fprintf(stderr, "           --all     : same as -a\n");
		fprintf(stderr, "           --blank   : same as -b\n");
		fprintf(stderr, "           --color   : same as -c\n");
		fprintf(stderr, "           --gouraud : same as -g\n");
		fprintf(stderr, "           --help    : same as -h\n");
		fprintf(stderr, "           --level3  : same as -3\n");
		fprintf(stderr, "           --rotate  : same as -r\n");
		fprintf(stderr, "           --title   : same as -t\n");
		fprintf(stderr, "           filename  : if not specified, prompted\n");
		exit (2);
	}
}

/************************************************************************/
/*                    Open a postscript file                            */
/************************************************************************/
void gsgl2ps_open (int interactive, int irotate, int icolor, char filename[],
		   int eps, int gouraud, int paper)
{
	int i;
	char *timestr, buffer[256];
	FILE *hdfile;
	time_t nseconds;
	
	g_irotate = irotate;
	g_icolor = icolor;
	Eps = eps;
	g_gouraud = gouraud;
	Paper = paper;

	if (interactive || eps) {
		psfile = fopen (filename,"w");
	} else {
		psfile = stdout;
	}
	
	if (eps) {
		if (gouraud == 3) {
			fprintf(psfile,"%%!PS-Adobe-3.0 EPSF-1.2\n");
		} else {
			fprintf(psfile,"%%!PS-Adobe-2.0 EPSF-1.2\n");
		}
	} else {
		if (gouraud == 3) {
			fprintf(psfile,"%%!PS-Adobe-3.0\n");
		} else {
			fprintf(psfile,"%%!PS-Adobe-2.0\n");
		}
	}
	fprintf(psfile,"%%%%Creator: GSAF/OpenGL \n");
	fprintf(psfile,"%%%%Title: (%s)\n",filename);
	nseconds = time (NULL);
	timestr = ctime (&nseconds);
	timestr[strlen(timestr)-1] = 0;
	fprintf(psfile, "%%%%Creation date: (%s)\n", timestr); 
	fprintf(psfile, "%%%%DocumentProcSets: Adobe_Illustrator_1.1 0 0\n");
	fprintf(psfile, "%%%%DocumentSuppliedProcSets: Adobe_Illustrator_1.1 0 0\n");
	fprintf(psfile, "%%%%Pages: (atend)\n");
	if (Paper == PAPER_B5) {
		if (g_irotate == 1) fprintf(psfile, "%%%%BoundingBox: 56 28 782 570\n");
		else fprintf(psfile, "%%%%BoundingBox: 28 56 570 782\n");
	} else if  (Paper == PAPER_A4) {
		/*if (g_irotate == 1) fprintf(psfile, "%%%%BoundingBox: 0 0 842 596\n");
		  else fprintf(psfile, "%%%%BoundingBox: 0 0 596 842\n");*/
		if (g_irotate == 1) fprintf(psfile, "%%%%BoundingBox: 24 3 818 593\n");
		else fprintf(psfile, "%%%%BoundingBox: 3 24 593 818\n");
	} else {
		if (g_irotate == 1) fprintf(psfile, "%%%%BoundingBox: 56 28 782 570\n");
		else fprintf(psfile, "%%%%BoundingBox: 28 56 570 782\n");
	}
	fprintf(psfile, "%%%%DocumentFonts: (atend)\n");
	if (g_icolor == 2) fprintf(psfile, "%%%%DocumentProcessColors: Color\n");
	else fprintf(psfile, "%%%%DocumentProcessColors: Black\n");
	fprintf(psfile, "%%%%EndComments\n");
  
	hdfile = fopen(PSHEADER1,"r");
	if(!hdfile) {
		printf("file gsgl_header1.ps cannot be found.");
		exit(1);
	}
	fgets(buffer,256,hdfile);
  
	while(!feof(hdfile)){
		fputs(buffer,psfile);
		fgets(buffer,256,hdfile);
	}
	fclose(hdfile);
	
	if (gouraud == 1) {
		fprintf(psfile,"/threshold 0.03 def \n");
		hdfile = fopen(PSHEADER2, "r");
		if(!hdfile) {
			printf("file gsgl_header2.ps cannot be found.");
			exit(1);
		}
		fgets(buffer,256,hdfile);
		
		while(!feof(hdfile)){
			fputs(buffer,psfile);
			fgets(buffer,256,hdfile);
		}
		fclose(hdfile);  
	}
	
	if (gouraud == 3) {
		hdfile = fopen (PSHEADER3, "r");
		if(!hdfile) {
			printf ("file gsgl_header3.ps cannot be found.");
			exit (1);
		}
		fgets (buffer, 256, hdfile);
		
		while (!feof(hdfile)) {
			fputs (buffer, psfile);
			fgets (buffer, 256, hdfile);
		}
		fclose (hdfile);  
	}	
	for (i=0; i<13; i++) g_ifused[i] = GSGL_FALSE;
}

/*********************************************************************/
/*                  Close a postscript file                          */
/*********************************************************************/
void gsgl2ps_close (int pageno)
{
	fprintf(psfile,"%%%%Trailer\n");
	fprintf(psfile,"%%%%DocumentFonts: ");
	if (g_ifused[0]) fprintf(psfile,"Times-Roman ");
	if (g_ifused[1]) fprintf(psfile,"Times-Italic ");
	if (g_ifused[2]) fprintf(psfile,"Times-Bold ");
	if (g_ifused[3]) fprintf(psfile,"Times-BoldItalic ");
	if (g_ifused[4]) fprintf(psfile,"Helvetica ");
	if (g_ifused[5]) fprintf(psfile,"Helvetica-Oblique ");
	if (g_ifused[6]) fprintf(psfile,"Helvetica-Bold ");
	if (g_ifused[7]) fprintf(psfile,"Helvetica-BoldOblique ");
	if (g_ifused[8]) fprintf(psfile,"Courier ");
	if (g_ifused[9]) fprintf(psfile,"Courier-Oblique ");
	if (g_ifused[10]) fprintf(psfile,"Courier-Bold ");
	if (g_ifused[11]) fprintf(psfile,"Courier-BoldOblique ");
	if (g_ifused[12]) fprintf(psfile,"Symbol ");
	fprintf(psfile,"\n");

	fprintf(psfile,"%%%%Pages: %d\n",pageno);
	fprintf(psfile,"_E end\n%%%%EOF\n");

	fclose(psfile);
	psfile = NULL;
} 

/*--------------------------------------------------------------*/
/*                      Begining of a page                      */
/*--------------------------------------------------------------*/ 
static void gsgl2ps_page_start (int pageno)
{
	fprintf (psfile, "%%%%Page: %d %d\n", pageno, pageno + 1);
	fprintf (psfile, "0 i 2 J 0 j 1 w 4 M []0 d\n");
}

/*--------------------------------------------------------------*/
/*                      End of a page                           */
/*--------------------------------------------------------------*/ 
static void gsgl2ps_page_end (void)
{
	if (!Eps) fprintf (psfile, "showpage\n");  
}

Psstatus * psstatus_new (void)
{
	Psstatus *status;
	int i;

	status = (Psstatus *) gsgl_malloc (sizeof (Psstatus));
	
	if (Paper == PAPER_B5) {
		/*182×257*/
		status->pixel2point = (72.0/2.54) * GSGL_PAPER_WIDTH / GSGLBASIC_WINDOWWIDTH;
	} else if (Paper == PAPER_A4) {
		/*210x297*/
		status->pixel2point = (72.0/2.54)*(21.0-0.2) / GSGLBASIC_WINDOWHEIGHT;
	} else {
		status->pixel2point = (72.0/2.54)*GSGL_PAPER_WIDTH / GSGLBASIC_WINDOWWIDTH;
	}
	status->now3d = GSGL_FALSE;
	status->graudonly = GSGL_FALSE;
	status->linewidth = LINEWIDTH;
	status->polycolor.r = -1.0;
	status->polycolor.g = -1.0;
	status->polycolor.b = -1.0;
	status->linecolor.r = -1.0;
	status->linecolor.g = -1.0;
	status->linecolor.b = -1.0;
	status->nprimitives = 0;
	status->item = 0;
	status->prims = NULL;
	status->offset = 0.0;
	status->depthtype = GSGL_DEPTH_BY_DEEP;
	for (i=0; i<16; i++) {
		if (i%5 == 0) status->modelview[i] = 1.0;
		else status->modelview[i] = 0.0;
	}

	return status;
}

/************************************************************************/
/*                   Write out a postscript file                        */
/************************************************************************/
void gsgl2ps_page (int pageno, GLfloat *feedbackBuffer)
{
	GLfloat     *loc;
	
  	gsgl2ps_page_start (pageno);

	loc = gsgl2ps_page_header (feedbackBuffer);
	gsgl2ps_page_3D (loc);
	gsgl2ps_page_2D (loc);
  
	gsgl2ps_page_end ();
}

/*---------------------------------------------------*/
/*               Write out the header                */
/*---------------------------------------------------*/
static GLfloat * gsgl2ps_page_header (GLfloat *buffer)
{
	int     token;
	GLfloat *loc;
	int     iexit = 0;
	Psstatus *status;

	loc = buffer;
	status = psstatus_new ();
	while (1) {
		token = *loc;
		loc++;
		switch (token) {
		case GL_LINE_RESET_TOKEN:
		case GL_LINE_TOKEN:
			loc = gsgl2ps_Line (GSGL2PS_HEADER, loc, status);
			break;
		case GL_POLYGON_TOKEN:
			loc = gsgl2ps_Poly (GSGL2PS_HEADER, loc, status);
			break;
		case GL_POINT_TOKEN:
			loc = gsgl2ps_Point (GSGL2PS_HEADER, loc, status);
			break;
		case GL_PASS_THROUGH_TOKEN:
			loc = gsgl2ps_PassThrough (GSGL2PS_HEADER, loc, &iexit, status);
			if (iexit) return loc;
			break;
		default:
			gsgl_error ("gsgl2ps_page_header", "Unexpected token (%d).\n", token);
		}
	}
}

/*---------------------------------------------------*/
/*                 Draw 2D Objects                   */
/*---------------------------------------------------*/
static void gsgl2ps_page_2D (GLfloat *location)
{
	GLfloat  *loc;
	Psstatus *status;

	loc = location;
	status = psstatus_new ();

	while (loc != NULL) {
		loc = gsgl2ps_page_prim (loc, 2/*2D*/, status);
	}
}

static int compare (const void *a, const void *b)
{
	DepthIndex *p1 = (DepthIndex *) a;
	DepthIndex *p2 = (DepthIndex *) b;
	GLfloat diff = p2->depth - p1->depth;
	
	if (diff > 0.0) {
		return 1;
	} else if (diff < 0.0) {
		return -1;
	} else {
		return 0;
	}
}

static int comparef (const void *a, const void *b)
{
	float diff, *c, *d;
	c = (float *)a;
	d = (float *)b;

	diff = *c - *d;
	
	if (diff > 0.0) {
		return 1;
	} else if (diff < 0.0) {
		return -1;
	} else {
		return 0;
	}
}

/*---------------------------------------------------*/
/*                 Draw 3D Objects                   */
/*---------------------------------------------------*/
static void gsgl2ps_page_3D (GLfloat *location)
{
	GLfloat *loc;
	int nprimitives = 0;
	int item;
	Psstatus *status; 
	DepthIndex *prims;
	int i;

	loc = location;

	status = psstatus_new ();
	nprimitives = gsgl2ps_3dprim_num (loc, status);  
  
	if (nprimitives == 0) return;

	status = psstatus_new ();
	status->nprimitives = nprimitives;
	status->prims = (DepthIndex *) gsgl_malloc (sizeof (DepthIndex) * nprimitives);
	gsgl2ps_3dprim_depth (loc, status);
	prims = status->prims;

	/*for (i=0; i<nprimitives; i++) fprintf(stderr,"%f\n",prims[i].depth);*/
	qsort (status->prims, nprimitives, sizeof(DepthIndex), compare);
	/*for (i=0; i<nprimitives; i++) fprintf(stderr,"%f\n", prims[i].depth);*/

	status = psstatus_new ();
	status->now3d = GSGL_TRUE;
	for (item = 0; item < nprimitives; item++) {
		loc = prims[item].ptr;
		status->linewidth = prims[item].linewidth;
		status->graudonly = prims[item].OnlyGouraud;
		for (i=0; i<16; i++) {
			status->projection[i] = prims[item].projection[i];
			status->modelview[i] = prims[item].modelview[i];
		}
		gsgl2ps_page_prim (loc, 3/*3D*/, status);
	}
	if (g_imv) {
		fprintf (psfile,"S\n");
		g_imv = 0;
	}
	if (g_ipoly) gsgl2ps_drawpoly_buffer (status);
	gsgl_free (status->prims);
}

/*---------------------------------------------------*/
/*                  Count 3D Objects                 */
/*---------------------------------------------------*/
static int gsgl2ps_3dprim_num (GLfloat *loc, Psstatus *status)
{
	int token;
	int iexit = 0;

	while (1) {
		token = *loc;
		loc++;
		switch (token) {
		case GL_LINE_TOKEN:
		case GL_LINE_RESET_TOKEN:
			loc = gsgl2ps_Line (GSGL2PS_3DPRIM_NUM, loc, status);
			break;
		case GL_POLYGON_TOKEN:
			loc = gsgl2ps_Poly (GSGL2PS_3DPRIM_NUM, loc, status);
			break;
		case GL_POINT_TOKEN:
			loc = gsgl2ps_Point (GSGL2PS_3DPRIM_NUM, loc, status);
			break;
		case GL_PASS_THROUGH_TOKEN:
			loc = gsgl2ps_PassThrough (GSGL2PS_3DPRIM_NUM, loc, &iexit, status);
			if (iexit) return status->nprimitives;
			break;
		default:
			gsgl_error ("gsgl2ps_3dprim_num", "Unexpected token (%d).\n", token);
		}
	}
}

/*---------------------------------------------------*/
/*            Check the depth of 3D Objects          */
/*---------------------------------------------------*/
static int gsgl2ps_3dprim_depth (GLfloat *buffer, Psstatus *status)
{
	int token;
	int iexit = 0;
	GLfloat *loc;

	loc = buffer;
	while (1) {
		token = *loc;
		loc++;
		switch (token) {
		case GL_LINE_TOKEN:
		case GL_LINE_RESET_TOKEN:
			loc = gsgl2ps_Line (GSGL2PS_3DPRIM_DEPTH, loc, status);
			break;
		case GL_POLYGON_TOKEN:
			loc = gsgl2ps_Poly (GSGL2PS_3DPRIM_DEPTH, loc, status);
			break;
		case GL_POINT_TOKEN:
			loc = gsgl2ps_Point (GSGL2PS_3DPRIM_DEPTH, loc, status);
			break;
		case GL_PASS_THROUGH_TOKEN:
			loc = gsgl2ps_PassThrough (GSGL2PS_3DPRIM_DEPTH, loc, &iexit, status);
			break;
		default:
			gsgl_error ("gsgl2ps_3dprim_depth", "Unexpected token (%d).\n", token);
		}
		if (status->item == status->nprimitives) return 0;
	}
}

/*---------------------------------------------------*/
/*                 Draw Primitives                   */
/*---------------------------------------------------*/
GLfloat * gsgl2ps_page_prim (GLfloat *loc, int dim, Psstatus *status)
{
	int token;
	int iexit = 0;
	int type = GSGL2PS_2D;
	
	token = *loc;
	loc ++;
	
	if (dim == 2) type = GSGL2PS_2D;
	if (dim == 3) type = GSGL2PS_3D;

	switch (token) {
	case GL_LINE_RESET_TOKEN:
	case GL_LINE_TOKEN:
		loc = gsgl2ps_Line (type, loc, status);
		break;
	case GL_POLYGON_TOKEN:
		loc = gsgl2ps_Poly (type, loc, status);
		break;
	case GL_POINT_TOKEN:
		loc = gsgl2ps_Point (type, loc, status);
		break;
	case GL_PASS_THROUGH_TOKEN:
		loc = gsgl2ps_PassThrough (type, loc, &iexit, status);
		break;
	default:
		gsgl_error ("gsgl2ps_page_prim", "Unexpected token (%d).\n", token);
	}
	return loc;
}

/*----------------------------------------------------------*/
/*       Transport x coordinate from pixel to point         */
/*----------------------------------------------------------*/
static GLfloat ps_x (GLfloat x, GLfloat y, Psstatus *status)
{
	if (Paper == PAPER_A4) {
		if (!g_irotate) return (593.0 - y * status->pixel2point);
		else return (x * status->pixel2point + 24.0);
	} else {
		if (!g_irotate) return (570.0 - y * status->pixel2point);
		else return (x * status->pixel2point + 56.0);
	}
}

/*----------------------------------------------------------*/
/*       Transport y coordinate from pixel to point         */
/*----------------------------------------------------------*/
static GLfloat ps_y (GLfloat x, GLfloat y, Psstatus *status)
{
	if (Paper == PAPER_A4) {
		if (!g_irotate) return (x * status->pixel2point + 24.0);
		else return (y * status->pixel2point + 3.0);
	} else {
		if (!g_irotate) return (x * status->pixel2point + 56.0);
		else return (y * status->pixel2point + 28.0);
	}
}

static void color (float cred, float cgreen, float cblue, int flag)
{
	float cblack;

	cblack = 1.0 - (0.15 * cblue + 0.30 * cred + 0.55 * cgreen);
	switch(flag) {
        /* move, draw */
	case 0:
		if(g_icolor == 2) {
			fprintf(psfile,"%1.3f G\n",1.0-cblack);
			fprintf(psfile,"%1.3f %1.3f %1.3f 0 K\n",
				1.0-cred,1.0-cgreen,1.0-cblue);
		} else if(g_icolor == 0) {
			fprintf(psfile,"0 G\n");
			fprintf(psfile,"0 0 0 1 K\n");
		} else {
			fprintf(psfile,"%1.3f G\n",1.0-cblack);
			fprintf(psfile,"0 0 0 %1.3f K\n",cblack);
		}
		break;
        /* lines */
	case 1:
		if(g_icolor == 2)	{
			fprintf(psfile,"%1.3f G\n",1.0-cblack);
			fprintf(psfile,"%1.3f %1.3f %1.3f 0 K\n",
				1.0-cred,1.0-cgreen,1.0-cblue);
			fprintf(psfile,"1 g\n");
			fprintf(psfile,"0 0 0 0 k\n");
		} else if(g_icolor == 0) {
			fprintf(psfile,"0 G\n");
			fprintf(psfile,"0 0 0 1 K\n");
			fprintf(psfile,"1 g\n");
			fprintf(psfile,"0 0 0 0 k\n");
		} else {
			fprintf(psfile,"%1.3f G\n",1.0-cblack);
			fprintf(psfile,"0 0 0 %1.3f K\n",cblack);
			fprintf(psfile,"1 g\n");
			fprintf(psfile,"0 0 0 0 k\n");
		}
		break;
        /* polygon */
	case 2:
		if(g_icolor == 2)	{
			fprintf(psfile,"%1.3f G\n",1.0-cblack);
			fprintf(psfile,"%1.3f %1.3f %1.3f 0 K\n",
				1.0-cred,1.0-cgreen,1.0-cblue);
			fprintf(psfile,"%1.3f g\n",1.0-cblack);
			fprintf(psfile,"%1.3f %1.3f %1.3f 0 k\n",
				1.0-cred,1.0-cgreen,1.0-cblue);
		} else if(g_icolor == 0) {
			fprintf(psfile,"0 G\n");
			fprintf(psfile,"0 0 0 1 K\n");
			fprintf(psfile,"0 g\n");
			fprintf(psfile,"0 0 0 1 k\n");
		} else {
			fprintf(psfile,"%1.3f G\n",1.0-cblack);
			fprintf(psfile,"0 0 0 %1.3f K\n",cblack);
			fprintf(psfile,"%1.3f g\n",1.0-cblack);
			fprintf(psfile,"0 0 0 %1.3f k\n",cblack);
		}
		break;
        /* text */
	case 3:
		if(g_icolor == 2)	{
			fprintf(psfile,"%1.3f g\n",1.0-cblack);
			fprintf(psfile,"%1.3f %1.3f %1.3f 0 k\n",
				1.0-cred,1.0-cgreen,1.0-cblue);
		} else if(g_icolor == 0) {
			fprintf(psfile,"0 g\n");
			fprintf(psfile,"0 0 0 1 k\n");
		} else {
			fprintf(psfile,"%1.3f g\n",1.0-cblack);
			fprintf(psfile,"0 0 0 %1.3f k\n",cblack);
		}
		break;
	}
}

/*------------------------------------------------------------*/
/*                Error for Unexpected token                 */
/*------------------------------------------------------------*/
static void Unexpected_token (char *token, char *function)
{
	gsgl_error (function, "unknown token = %s", token);
	exit(1);
}

/*------------------------------------------------------------*/
/*               processing of GL_LINE_TOKEN                  */
/*------------------------------------------------------------*/
static GLfloat * gsgl2ps_Line (int type, GLfloat *loc, Psstatus *status)
{
	Feedback3Dcolor  *vertex;
	int item;
	
	vertex = (Feedback3Dcolor *) loc;
	switch (type) {
	case GSGL2PS_HEADER:
		gsgl2ps_Line_base (vertex, status);
		break;
	case GSGL2PS_3DPRIM_NUM:
		if (status->now3d) (status->nprimitives)++;
		break;
	case GSGL2PS_3DPRIM_DEPTH:
		if (status->now3d) {
			item = status->item;
			status->prims[item].ptr = loc-1;
			
			switch (status->depthtype) {
			case GSGL_DEPTH_BY_DEEP:
				status->prims[item].depth = MAX(vertex[0].z, vertex[1].z);
				break;
			case GSGL_DEPTH_BY_SHALLOW:
				status->prims[item].depth = MIN(vertex[0].z, vertex[1].z);
				break;
			case GSGL_DEPTH_BY_MEAN:
				status->prims[item].depth = 0.5*(vertex[0].z+vertex[1].z);
				break;
			case GSGL_DEPTH_BY_POINT:
				status->prims[item].depth = g_depth_of_prim;
				break;
			default:
				break;
			}

			status->prims[item].linewidth = status->linewidth;
			status->prims[item].OnlyGouraud = status->graudonly;
			(status->item)++;
		}
		break;
	case GSGL2PS_2D:
		if (!status->now3d) gsgl2ps_Line_base (vertex, status);
		break;
	case GSGL2PS_3D:
		if (status->now3d) gsgl2ps_Line_base (vertex, status);
		break;
	default:
		break;
	}
	loc += 14;
	return loc;
}

/*------------------------------------------------------------*/
/*                       Draw a Line                          */
/*------------------------------------------------------------*/
static void gsgl2ps_Line_base (Feedback3Dcolor *vertex, Psstatus *status)
{
	if (g_ipoly) {
		gsgl2ps_drawpoly_buffer (status);
	}
	if (status->linecolor.r != vertex[0].red || status->linecolor.g != vertex[0].green ||
	    status->linecolor.b != vertex[0].blue || !g_imv) { 
		/*前のtokenが直線でないか線の色が変わった場合*/
		gsgl2ps_drawline (vertex, status);
	} else {
		/*前のtokenが直線で直線がつながっている場合*/
		if (fabs (g_xpos_line-ps_x (vertex[0].x, vertex[0].y, status)) < POS_EPSILON &&
		    fabs (g_ypos_line-ps_y (vertex[0].x, vertex[0].y, status)) < POS_EPSILON){
			g_xpos_line = ps_x (vertex[1].x, vertex[1].y, status);
			g_ypos_line = ps_y (vertex[1].x, vertex[1].y, status);
			fprintf (psfile, "%1.3f %1.3f L\n", g_xpos_line, g_ypos_line);
			g_imv = 1;
		} else {
			/*前のtokenが直線で色も変わっていないが直線がつながっていない場合*/
			gsgl2ps_drawline (vertex, status);
		}
	}
}

/*---------------------------------------------------------*/
/*                    Draw a line                          */
/*---------------------------------------------------------*/
static void gsgl2ps_drawline (Feedback3Dcolor vertex[], Psstatus *status)
{
	int   i;
	
	if(g_imv) fprintf (psfile, "S\n");
	fprintf(psfile, "%% Lines\n");
	fprintf(psfile, "%1.3f w\n", MAX(status->linewidth, 0.25));
	
	status->linecolor.r = vertex[0].red ;
	status->linecolor.g = vertex[0].green ;
	status->linecolor.b = vertex[0].blue ;
  
	color (status->linecolor.r, status->linecolor.g, status->linecolor.b, 1);
	
	for (i = 0; i <= 1; i++) {
		g_xpos_line = ps_x (vertex[i].x, vertex[i].y, status);
		g_ypos_line = ps_y (vertex[i].x, vertex[i].y, status);
		if (i == 0) fprintf (psfile, "%1.3f %1.3f m\n", g_xpos_line, g_ypos_line);
		else fprintf(psfile, "%1.3f %1.3f L\n", g_xpos_line, g_ypos_line);
	}
	g_imv = 1;
}

/*------------------------------------------------------------*/
/*                processing of GL_POLYGON_TOKEN              */
/*------------------------------------------------------------*/
static GLfloat * gsgl2ps_Poly (int type, GLfloat *loc, Psstatus *status)
{
	Feedback3Dcolor  *vertex;
	int i, nvertices, item;
	GLfloat depthmax, depthmin;
  
	nvertices = *loc;
	loc++;
	if (nvertices > 0) vertex = (Feedback3Dcolor *) loc;
	else return loc;
	
	switch (type) {
	case GSGL2PS_HEADER:
		Unexpected_token ("GL_POLYGON_TOKEN", "gsgl2ps_page_header");
		break;
	case GSGL2PS_3DPRIM_NUM:
		if (status->now3d) (status->nprimitives)++;
		break;
	case GSGL2PS_3DPRIM_DEPTH:
		if (status->now3d) {
			item = status->item;
			status->prims[item].ptr = loc-2;

			switch (status->depthtype) {
			case GSGL_DEPTH_BY_DEEP:
				depthmax = vertex[0].z;
				for (i = 1; i < nvertices; i++) {
					depthmax = MAX(depthmax, vertex[i].z);
				}
				status->prims[item].depth = depthmax + status->offset;
				break;
			case GSGL_DEPTH_BY_SHALLOW:
				depthmin = vertex[0].z;
				for (i = 1; i < nvertices; i++) {
					depthmin = MIN(depthmin, vertex[i].z);
				}
				status->prims[item].depth = depthmin + status->offset;
				break;
			case GSGL_DEPTH_BY_MEAN:
				depthmin = vertex[0].z;
				for (i = 1; i < nvertices; i++) {
					depthmin = depthmin + vertex[i].z;
				}
				status->prims[item].depth = depthmin/nvertices + status->offset;
				break;
			case GSGL_DEPTH_BY_POINT:
				status->prims[item].depth = g_depth_of_prim + status->offset;
				break;
			default:
				break;
			}
			status->prims[item].linewidth = status->linewidth;
			status->prims[item].OnlyGouraud = status->graudonly;
			(status->item)++;
		}
		break;
	case GSGL2PS_2D:
		if (!status->now3d) gsgl2ps_Poly_base (vertex, nvertices, status);
		break;
	case GSGL2PS_3D:
		if (status->now3d) gsgl2ps_Poly_base (vertex, nvertices, status);
		break;
	default:
		break;
	}
	loc += nvertices * 7;
	return loc;
}

/*------------------------------------------------------------*/
/*                     Draw a Polygon                         */
/*------------------------------------------------------------*/
static void gsgl2ps_Poly_base (Feedback3Dcolor *vertex, int nvertices, Psstatus *status)
{
	int      gouraud = 0;
	int      i, l, l_old, j, k[1024], i_new=0;
	GLfloat  temp_x=0.0, temp_y=0.0;
	int color_not_change_in_poly=0;
	GLfloat  red, green, blue;
	GLfloat  xpos, ypos;

	if (g_imv) fprintf(psfile, "S\n");
	g_imv = 0;
	if (nvertices == 3) {
		if (fabs (status->pixel2point * (vertex[0].x - vertex[1].x)) < POS_EPSILON &&
		    fabs (status->pixel2point * (vertex[0].y - vertex[1].y)) < POS_EPSILON &&
		    fabs (status->pixel2point * (vertex[2].x - vertex[1].x)) < POS_EPSILON &&
		    fabs (status->pixel2point * (vertex[2].y - vertex[1].y)) < POS_EPSILON)
			return;
	}
	gouraud = gsgl2ps_check_groud (vertex, nvertices, &color_not_change_in_poly, status);
	if (gouraud) {
		if(g_ipoly) gsgl2ps_drawpoly_buffer (status);
		/* Break polygon into "nvertices-2" triangle fans. */
		if (g_gouraud == 1) {
			/*if (status->now3d) fprintf(psfile,"%1.3f w\n", 0.0);
			  else fprintf(psfile,"%1.3f w\n", status->linewidth);*/
			fprintf(psfile,"%1.3f w\n", 0.0);
			for (i = 0; i < nvertices - 2; i++) {
				fprintf(psfile, "[%g %g %g %g %g %g]",
					ps_x (vertex[0].x, vertex[0].y, status),
					ps_x (vertex[i+1].x, vertex[i+1].y, status),
					ps_x (vertex[i+2].x, vertex[i+2].y, status),
					ps_y (vertex[0].x, vertex[0].y, status),
					ps_y (vertex[i+1].x, vertex[i+1].y, status),
					ps_y (vertex[i+2].x, vertex[i+2].y, status));
				fprintf(psfile, " [%g %g %g] [%g %g %g] "
					"[%g %g %g] gouraudtriangle\n",
					vertex[0].red, vertex[0].green, vertex[0].blue,
					vertex[i+1].red, vertex[i+1].green, vertex[i+1].blue,
					vertex[i+2].red, vertex[i+2].green, vertex[i+2].blue);
				gouraud = 0;
			}
		} else if (g_gouraud==3) {
			/*if (status->now3d) fprintf(psfile,"%1.3f w\n", 0.0);
			  else fprintf(psfile,"%1.3f w\n", status->linewidth);*/
			fprintf(psfile,"%1.3f w\n", 0.0);
			for (i = 0; i < nvertices - 2; i++) {
			fprintf(psfile, "gourauds [0 %g %g %g %g %g 0 %g %g %g %g %g"
				" 0 %g %g %g %g %g ] gouraude\n",
				ps_x (vertex[0].x, vertex[0].y, status),
				ps_y (vertex[0].x, vertex[0].y, status),
				vertex[0].red, vertex[0].green, vertex[0].blue,
				ps_x (vertex[i+1].x, vertex[i+1].y, status),
				ps_y (vertex[i+1].x, vertex[i+1].y, status),
				vertex[i + 1].red, vertex[i + 1].green, vertex[i + 1].blue,
				ps_x (vertex[i+2].x, vertex[i+2].y, status),
				ps_y (vertex[i+2].x, vertex[i+2].y, status),
				vertex[i + 2].red, vertex[i + 2].green, vertex[i + 2].blue);
			gouraud = 0;
			}
		} else {
			red = 0.0;
			green = 0.0;
			blue = 0.0;
			gouraud = 0;
			for (i = 0; i < nvertices; i++) {
				red = red + vertex[i].red;
				green = green + vertex[i].green;
				blue = blue + vertex[i].blue;
			}
			red = red / (float) nvertices;
			green = green / (float) nvertices;
			blue = blue / (float) nvertices;
			gsgl2ps_poly_header (red, green, blue, status);
			for (i = 0; i < nvertices; i++) {
				xpos = ps_x(vertex[i].x, vertex[i].y, status);
				ypos = ps_y(vertex[i].x, vertex[i].y, status);
				if (i == 0)
				     fprintf(psfile,"%1.3f %1.3f m\n", xpos, ypos);
				else
				     fprintf(psfile,"%1.3f %1.3f L\n", xpos, ypos);
			}
			fprintf(psfile,"b\n");
			g_imv = 0;
			g_ipoly = 0;
		}
	} else if (!color_not_change_in_poly || nvertices != 3 || !g_ipoly) {
		gsgl2ps_drawpoly (vertex, nvertices, status);
	} else if (nvertices == 3) {
		l = 0;
		l_old = 0;/*バッファーにある座標と現行の三角形の共通点を数える*/
		for (i=0; i < nvertices ; i ++) {
			for (j=0; j < g_mpoly ; j ++) {
				if (fabs(g_xpos_poly[j]-ps_x(vertex[i].x, vertex[i].y, status))
				    < POS_EPSILON &&
				    fabs(g_ypos_poly[j]-ps_y(vertex[i].x, vertex[i].y, status))
				    < POS_EPSILON) {
					k[l] = j;
					l ++;
				}
			}
			if (l == l_old) i_new = i;
			l_old = l;
		}
		if (l == 2) {
			if (fabs(k[1] - k[0]) != 1 && fabs(k[1] - k[0]) != g_mpoly - 1) {
				gsgl2ps_drawpoly (vertex, nvertices, status);
			} else {
				g_xpos_poly[g_mpoly] = ps_x(vertex[i_new].x,vertex[i_new].y, status);
				g_ypos_poly[g_mpoly] = ps_y(vertex[i_new].x,vertex[i_new].y, status);
				g_ipoly = 1;
				if (MIN(k[0], k[1]) == 0 && MAX(k[0], k[1]) == g_mpoly-1) {}
				else {
					for (i = g_mpoly ; i > MIN(k[0], k[1]) ; i--) {
						if (i == g_mpoly) {
							temp_x = g_xpos_poly[g_mpoly];
							temp_y = g_ypos_poly[g_mpoly];
							g_xpos_poly[i] = g_xpos_poly[i - 1];
							g_ypos_poly[i] = g_ypos_poly[i - 1];
						} else if (i == MIN(k[0], k[1]) + 1) {
							g_xpos_poly[i] = temp_x;
							g_ypos_poly[i] = temp_y;
						} else {
							g_xpos_poly[i] = g_xpos_poly[i - 1];
							g_ypos_poly[i] = g_ypos_poly[i - 1];
						}
					}
				}
				g_mpoly ++;
			}
		} else if (l > 3) { /* == 3 を入れるとだめ */
			gsgl2ps_drawpoly_buffer (status);
		} else {
			gsgl2ps_drawpoly (vertex, nvertices, status);
		}
	}
}

static int gsgl2ps_check_groud (Feedback3Dcolor  *vertex, int nvertices, 
				int *color_not_change_in_poly, Psstatus *status)
{
	float red_buffer = -1.0, blue_buffer = -1.0, green_buffer = -1.0;
	int i;
	gsglboolean gouraud = GSGL_FALSE;
	
	if (status->graudonly) {
		gouraud = GSGL_TRUE;
	} else {
		*color_not_change_in_poly = 1;
		for (i=0 ; i < nvertices ; i++) {
			if (status->polycolor.r != vertex[i].red ||
			    status->polycolor.g != vertex[i].green ||
			    status->polycolor.b != vertex[i].blue) {
				*color_not_change_in_poly = 0;
				break;
			}
		}
		if (!(*color_not_change_in_poly)) {
			red_buffer = status->polycolor.r;
			green_buffer = status->polycolor.g;
			blue_buffer = status->polycolor.b;
			for (i=0 ; i < nvertices ; i++) {
				if (i == 0) {
					red_buffer = vertex[i].red;
					green_buffer = vertex[i].green;
					blue_buffer = vertex[i].blue;
				}
				if (red_buffer != vertex[i].red ||
				    green_buffer != vertex[i].green ||
				    blue_buffer != vertex[i].blue ) {
					gouraud = 1;
					break;
				}
			}
		}
	}
	return gouraud;
}

static void gsgl2ps_drawpoly (Feedback3Dcolor vertex[], int nvertices, Psstatus *status)
{
	int      i;
	GLfloat  xpos, ypos;
	
	if(g_ipoly) gsgl2ps_drawpoly_buffer (status);
	
	status->polycolor.r = vertex[0].red;
	status->polycolor.g = vertex[0].green;
	status->polycolor.b = vertex[0].blue;
	
	if (nvertices != 3) {
		gsgl2ps_poly_header (status->polycolor.r,
				     status->polycolor.g,
				     status->polycolor.b,
				     status);
		for (i = 0; i < nvertices; i++) {
			xpos = ps_x (vertex[i].x, vertex[i].y, status);
			ypos = ps_y (vertex[i].x, vertex[i].y, status);
			if (i == 0)
				fprintf (psfile,"%1.3f %1.3f m\n", xpos, ypos);
			else
				fprintf (psfile,"%1.3f %1.3f L\n", xpos, ypos);
		}
		g_imv = 0;
		g_ipoly = 0;
	} else if (nvertices == 3) {
		for (i = 0; i < nvertices; i++) {
			g_xpos_poly[i] = ps_x (vertex[i].x, vertex[i].y, status);
			g_ypos_poly[i] = ps_y (vertex[i].x, vertex[i].y, status);
		}
		g_imv = 0;
		g_ipoly = 1;
		g_mpoly = 3;
		if (g_xpos_poly[0] == g_xpos_poly[1] &&
		    g_ypos_poly[0] == g_ypos_poly[1] &&
		    g_xpos_poly[0] == g_xpos_poly[2] &&
		    g_ypos_poly[0] == g_ypos_poly[2]){
			g_mpoly -= 2;
		} else if (g_xpos_poly[0] == g_xpos_poly[1] &&
			   g_ypos_poly[0] == g_ypos_poly[1]){
			g_xpos_poly[1] = g_xpos_poly[2];
			g_ypos_poly[1] = g_ypos_poly[2];
			g_mpoly--;
		} else if (g_xpos_poly[1] == g_xpos_poly[2] &&
			   g_ypos_poly[1] == g_ypos_poly[2]){
			g_mpoly--;
		} else if (g_xpos_poly[0] == g_xpos_poly[2] &&
			   g_ypos_poly[0] == g_ypos_poly[2]){
			g_xpos_poly[0] = g_xpos_poly[1];
			g_ypos_poly[0] = g_ypos_poly[1];	
			g_xpos_poly[1] = g_xpos_poly[2];
			g_ypos_poly[1] = g_ypos_poly[2];	
			g_mpoly--;
		}
	}
}

static void gsgl2ps_drawpoly_buffer (Psstatus *status)
{
	int   i;
  
	/*if (g_mpoly == 1 || g_mpoly == 2) fprintf(stderr,"m is too small\n");*/
	if (g_mpoly >= 3) {
		gsgl2ps_poly_header (status->polycolor.r,
				     status->polycolor.g,
				     status->polycolor.b,
				     status);
		for (i=0; i < g_mpoly ; i++) { 
			if (i == 0) {
				fprintf (psfile, "%1.3f %1.3f m\n", g_xpos_poly[i], g_ypos_poly[i]);
			} else {
				fprintf (psfile, "%1.3f %1.3f L\n", g_xpos_poly[i], g_ypos_poly[i]);
			}
		}
		fprintf(psfile,"b\n");
	}
	g_mpoly = 0;
	g_ipoly = 0;
}

static void gsgl2ps_poly_header (float red, float green, float blue, Psstatus *status)
{
	fprintf(psfile,"%% Poly\n");
	fprintf(psfile,"%1.3f w\n", 0.0);
	/*fprintf(psfile,"%1.3f w\n", status->linewidth);*/

	color (red, green, blue, 2);
}

/*------------------------------------------------------------*/
/*                processing of GL_POINT_TOKEN                */
/*------------------------------------------------------------*/
static GLfloat * gsgl2ps_Point (int type, GLfloat *loc, Psstatus *status)
{
	Feedback3Dcolor *vertex;
	int item;

	vertex = (Feedback3Dcolor *) loc;
	switch (type) {
	case GSGL2PS_HEADER:
		break;
	case GSGL2PS_3DPRIM_NUM:
		if (status->now3d) (status->nprimitives)++;
		break;
	case GSGL2PS_3DPRIM_DEPTH:
		if (status->now3d) {
			item = status->item;
			status->prims[item].ptr = loc-1;
			status->prims[item].depth = vertex[0].z;
			status->prims[item].linewidth = status->linewidth;
			status->prims[item].OnlyGouraud = status->graudonly;
			(status->item)++;
		}
		break;
	case GSGL2PS_2D:
		break;
	case GSGL2PS_3D:
		break;
	default:
		break;
	}
	loc += 7;
	return loc;
}

/*------------------------------------------------------------*/
/*        与えられた点の深さを Primitive の深さにする         */
/*------------------------------------------------------------*/
static GLfloat * gsgl2ps_DepthByPoint (GLfloat * loc, int type, Psstatus *status)
{
	int i;
	float p[4], out[4];
	
	status->depthtype = GSGL_DEPTH_BY_POINT;
	for (i=0; i<3; i++) {
		loc ++;
		p[i] = (float) *loc;
		loc ++;
	}
	p[3] = 1.0;

	if (type == GSGL2PS_3DPRIM_DEPTH) {
		/*
		  fprintf (stderr, "ps\n");
		  fprintf (stderr, "%f %f %f %f\n",Modelview[0],Modelview[4],Modelview[8],Modelview[12]);
		  fprintf (stderr, "%f %f %f %f\n",Modelview[1],Modelview[5],Modelview[9],Modelview[13]);
		  fprintf (stderr, "%f %f %f %f\n",Modelview[2],Modelview[6],Modelview[10],Modelview[14]);
		  fprintf (stderr, "%f %f %f %f\n",Modelview[3],Modelview[7],Modelview[11],Modelview[15]);
		*/

		/*fprintf (stderr, "pre %f %f %f %f\n", p[0], p[1], p[2], p[3]);*/
		gsgl_mat16_mult_vec4 (status->modelview, p, out);
		gsgl_mat16_mult_vec4 (status->projection, out, p);
		g_depth_of_prim = (p[2]/p[3]+1.0)*0.5;
		/*fprintf (stderr, "after %f %f %f %f\n", p[0], p[1], p[2], p[3]);*/
	}
	return loc;
}

/*-----------------------------------------------------------------------*/
/* 与えられた2つのベクトルの外積の符号によって depth by hoge を変える    */
/*-----------------------------------------------------------------------*/
static GLfloat * gsgl2ps_DepthByVectProd (GLfloat * loc, Psstatus *status)
{
	float p1[4], p2[4], p3[4];
	float pw1[4], pw2[4], pw3[4];
	float pality;
	int i;

	for (i=0; i<3; i++) {
		loc ++;
		p1[i] = (float) *loc;
		loc ++;
	}
	p1[3] = 1.0;
	for (i=0; i<3; i++) {
		loc ++;
		p2[i] = (float) *loc;
		loc ++;
	}
	p2[3] = 1.0;
	for (i=0; i<3; i++) {
		loc ++;
		p3[i] = (float) *loc;
		loc ++;
	}
	p3[3] = 1.0;

	gsgl_mat16_mult_vec4 (status->modelview, p1, pw1);
	gsgl_mat16_mult_vec4 (status->modelview, p2, pw2);
	gsgl_mat16_mult_vec4 (status->modelview, p3, pw3);

	pality = (pw2[0]-pw1[0])*(pw3[1]-pw2[1])-(pw2[1]-pw1[1])*(pw3[0]-pw2[0]);
	if (pality > 0.0)
		status->depthtype = GSGL_DEPTH_BY_DEEP;
	else if (pality < 0.0)
		status->depthtype = GSGL_DEPTH_BY_SHALLOW;
	else if (pality == 0.0)
		status->depthtype = GSGL_DEPTH_BY_MEAN;
	else
		status->depthtype = GSGL_DEPTH_BY_DEEP;
	
	return loc;
}

/*-----------------------------------------------------------------------*/
/* 与えられた2つのベクトルの外積の符号によって depth by hoge を変える    */
/*-----------------------------------------------------------------------*/
static GLfloat * gsgl2ps_DepthByVectProd2 (GLfloat * loc, Psstatus *status)
{
	float p1[4], p2[4], p3[4];
	float pw1[4], pw2[4], pw3[4];
	float pality;
	int i, j;
	unsigned int npoints;
	float p[4], out[4], *dep;

	for (i=0; i<3; i++) {
		loc ++;
		p1[i] = (float) *loc;
		loc ++;
	}
	p1[3] = 1.0;
	for (i=0; i<3; i++) {
		loc ++;
		p2[i] = (float) *loc;
		loc ++;
	}
	p2[3] = 1.0;
	for (i=0; i<3; i++) {
		loc ++;
		p3[i] = (float) *loc;
		loc ++;
	}
	p3[3] = 1.0;

	gsgl_mat16_mult_vec4 (status->modelview, p1, pw1);
	gsgl_mat16_mult_vec4 (status->modelview, p2, pw2);
	gsgl_mat16_mult_vec4 (status->modelview, p3, pw3);

	pality = (pw2[0]-pw1[0])*(pw3[1]-pw2[1])-(pw2[1]-pw1[1])*(pw3[0]-pw2[0]);

	if (pality > 0.0)
		status->depthtype = GSGL_DEPTH_BY_DEEP;
	else if (pality < 0.0)
		status->depthtype = GSGL_DEPTH_BY_SHALLOW;
	else if (pality == 0.0)
		status->depthtype = GSGL_DEPTH_BY_MEAN;
	else
		status->depthtype = GSGL_DEPTH_BY_DEEP;

	loc ++;
	npoints = (unsigned int) *loc;
	loc ++;

	dep = (float *) gsgl_malloc_atomic (sizeof(float)*npoints);

	for (j=0; j<npoints; j++) {
		for (i=0; i<3; i++) {
			loc ++;
			p[i] = (float) *loc;
			loc ++;
		}
		p[3] = 1.0;
		gsgl_mat16_mult_vec4 (status->modelview, p, out);
		gsgl_mat16_mult_vec4 (status->projection, out, p);
		dep[j] = (p[2]/p[3]+1.0)*0.5;
	}

	qsort (dep, npoints, sizeof(float), comparef);
	
	if (status->depthtype == GSGL_DEPTH_BY_DEEP) {
		g_depth_of_prim = dep[npoints-1];
	} else if (status->depthtype == GSGL_DEPTH_BY_SHALLOW) {
		g_depth_of_prim = dep[0];
	} else if (status->depthtype == GSGL_DEPTH_BY_MEAN) {
		g_depth_of_prim = 0.5*(dep[0] + dep[npoints-1]);
	} else {
		g_depth_of_prim = dep[npoints-1];
	}
	status->depthtype = GSGL_DEPTH_BY_POINT;
	return loc;
}

static void mult_matrix (float m1[2][2], float m2[2][2], float m3[2][2]);
/*------------------------------------------------------------*/
/*            processing of GL_PASS_THROUGH_TOKEN             */
/*------------------------------------------------------------*/
static GLfloat * gsgl2ps_PassThrough (int type, GLfloat *loc, GLint *iexit, Psstatus *status)
{
	int pass;
	float line_width;
	int i;

	pass = (int) *loc;
	loc ++;
	switch (pass) {
	case GSGLINTERNAL_BEGIN_HEAD:
		if (type == GSGL2PS_HEADER) {
			return loc;
		} else {
			Unexpected_token ("GSGLINTERNAL_BEGIN_HEAD", "gsgl2ps_PassThrough");
		}
	case GSGLINTERNAL_END_HEAD:
		if (type == GSGL2PS_HEADER) {
			if(g_imv) {
				fprintf(psfile,"S\n");
				g_imv = 0;
			}
			*iexit = 1;
			return loc;
		} else {
			Unexpected_token ("GSGLINTERNAL_END_HEAD", "gsgl2ps_PassThrough");
		}
	case GSGLINTERNAL_BEGIN_PAGE:
		return loc;
	case GSGLINTERNAL_END_PAGE:
		if (type == GSGL2PS_HEADER) {
			Unexpected_token ("GSGLINTERNAL_END_PAGE", "gsgl2ps_PassThrough");
		} else if (type == GSGL2PS_3DPRIM_NUM) {
			*iexit = 1;
			return loc;
		} else if (type == GSGL2PS_3DPRIM_DEPTH) {
		} else if (type == GSGL2PS_2D || type == GSGL2PS_3D) {
			if(g_imv) {
				fprintf(psfile,"S\n");
				g_imv = 0;
			}
			if (g_ipoly) gsgl2ps_drawpoly_buffer (status);
			return NULL;
		}
		return loc;
	case GSGLINTERNAL_LINEWIDTH:
		if (type == GSGL2PS_HEADER || type == GSGL2PS_2D || type == GSGL2PS_3D){ 
			loc ++; line_width = *loc; loc ++;
			if (line_width < 0.0) 
				status->linewidth = 0.0;
			else
				status->linewidth = line_width * 72.0/2.54;
			if (g_imv) {
				fprintf(psfile,"S\n");
				g_imv = 0;
			}
		} else if (type == GSGL2PS_3DPRIM_NUM) {
			loc += 2;
		} else if (type == GSGL2PS_3DPRIM_DEPTH) {
			loc ++; line_width = *loc; loc ++;
			if (line_width < 0.0) 
				status->linewidth = 0.0;
			else
				status->linewidth = line_width * 72.0/2.54;
		} else {
			loc ++; line_width = *loc; loc ++;
		}
		return loc;
	case GSGLINTERNAL_LINEWIDTH2:
		if (type == GSGL2PS_HEADER || type == GSGL2PS_2D || type == GSGL2PS_3D){ 
			loc ++; line_width = *loc; loc ++;
			if (line_width < 0.0) 
				status->linewidth = 0.0;
			else
				status->linewidth = line_width * 0.5;
			if (g_imv) {
				fprintf(psfile,"S\n");
				g_imv = 0;
			}
		} else if (type == GSGL2PS_3DPRIM_NUM) {
			loc += 2;
		} else if (type == GSGL2PS_3DPRIM_DEPTH) {
			loc ++;
			line_width = *loc;
			loc ++;
			if (line_width< 0.0) 
				status->linewidth = 0.0;
			else
				status->linewidth = line_width * 0.5;
		} else {
			loc ++; line_width = *loc; loc ++; 
		}
		return loc;
	case GSGLINTERNAL_MODELVIEW_MATRIX:
		for (i=0; i<16; i++) {
			loc ++; 
			status->modelview[i] = (float) *loc;
			loc ++;
		}
		return loc;
	case GSGLINTERNAL_PROJECTION_MATRIX:
		for (i=0; i<16; i++) {
			loc ++; 
			status->projection[i] = (float) *loc;
			loc ++;
		}
		return loc;
	case GSGLINTERNAL_NORMAL_MODE:
		if (type == GSGL2PS_3DPRIM_DEPTH) status->offset = 0.0;
		status->now3d = GSGL_FALSE;
		return loc;
	case GSGLINTERNAL_2D_MODE:
		if (type == GSGL2PS_3DPRIM_DEPTH) status->offset = 0.0;
		status->now3d = GSGL_FALSE;
		return loc;
	case GSGLINTERNAL_3D_MODE:
		if (type == GSGL2PS_HEADER) {
			Unexpected_token ("GSGLINTERNAL_3D_MODE", "gsgl2ps_PassThrough");
		} else if (type==GSGL2PS_3DPRIM_NUM) {
			status->now3d = GSGL_TRUE;
		} else if (type==GSGL2PS_3DPRIM_DEPTH){
			status->now3d = GSGL_TRUE;
			status->offset = POLYGON_OFFSET;
		} else if (type == GSGL2PS_2D || type == GSGL2PS_3D) {
			status->now3d = GSGL_TRUE;
			if (g_imv) {
				fprintf(psfile,"S\n");
				g_imv = 0;
			}
			if (g_ipoly) gsgl2ps_drawpoly_buffer (status);
		}
		return loc;
	case GSGLINTERNALPS_DEPTH_BY_DEEP:
		if (type == GSGL2PS_HEADER) {
			Unexpected_token ("GSGLINTERNALPS_DEPTH_BY_DEEP", "gsgl2ps_PassThrough");
		} else if (type == GSGL2PS_3DPRIM_DEPTH) {
			status->depthtype = GSGL_DEPTH_BY_DEEP;
		}
		return loc;
	case GSGLINTERNALPS_DEPTH_BY_SHALLOW:
		if (type == GSGL2PS_3DPRIM_DEPTH) {
			status->depthtype = GSGL_DEPTH_BY_SHALLOW;
		}
		return loc;
	case GSGLINTERNALPS_DEPTH_BY_MEAN:
		if (type == GSGL2PS_3DPRIM_DEPTH) {
			status->depthtype = GSGL_DEPTH_BY_MEAN;
		}
		return loc;
	case GSGLINTERNALPS_BEGIN_GOURAUD_ONLY:
		if (type == GSGL2PS_HEADER) {
			Unexpected_token ("GSGLPS_BEGIN_GOURAUD_ONLY", "gsgl2ps_page_header");
		} else if (type == GSGL2PS_3DPRIM_NUM) {
		} else {
			status->graudonly = GSGL_TRUE;
		}
		return loc;
	case GSGLINTERNALPS_END_GOURAUD_ONLY:
		if (type ==  GSGL2PS_HEADER) {
			Unexpected_token ("GSGLINTERNALPS_END_GOURAUD_ONLY", "gsgl2ps_page_header");
		} else if (type == GSGL2PS_3DPRIM_NUM) {
		} else {
			status->graudonly = GSGL_FALSE;
		}
		return loc;
	case GSGLINTERNALPS_DEPTH_BY_VECT_PROD:
		if (type == GSGL2PS_HEADER) {
			Unexpected_token ("GSGLINTERNALPS_DEPTH_BY_VECT_PROD", "gsgl2ps_page_header");
		} else {
			loc = gsgl2ps_DepthByVectProd (loc, status);
		}     
		return loc;
	case GSGLINTERNALPS_DEPTH_BY_VECT_PROD2:
		if (type == GSGL2PS_HEADER) {
			Unexpected_token ("GSGLINTERNALPS_DEPTH_BY_VECT_PROD", "gsgl2ps_page_header");
		} else {
			loc = gsgl2ps_DepthByVectProd2 (loc, status);
		}     
		return loc;
	case GSGLINTERNALPS_DEPTH_BY_POINT:
		if (type ==  GSGL2PS_HEADER) {
			Unexpected_token ("GSGLINTERNALPS_DEPTH_BY_POINT", "gsgl2ps_page_header");
		} else {
			loc = gsgl2ps_DepthByPoint (loc, type, status);
		}  
		return loc;
	case GSGLINTERNAL_PSTEXT:		
		return gsgl2ps_pstext (type, loc, status);
	default:
		gsgl_error ("gsgl2ps_PassThrough", "Pass = %d", pass);
	}
	return loc;
}

static void mult_matrix (float m1[2][2], float m2[2][2], float m3[2][2])
{
	int i, j, k;

	for (i=0; i<2; i++) {
		for (j=0; j<2; j++) {
			m3[i][j] = 0.0;
			for (k=0; k<2; k++)
				m3[i][j] += m1[i][k]*m2[k][j];
		}
	}
}

static GLfloat * gsgl2ps_pstext (int type, GLfloat *loc, Psstatus *status)
{
	float chh, chw, chsp, angle, tilt, x, y, depth;
	int ifnts, ifntr, nmax, *iascs;
	int ifont;
	float dchs,dchss,dchsp;
	float vec0[4], vec1[4], vec2[4], vec3[4];
	float mat1[2][2], mat2[2][2], mat3[2][2], mat4[2][2];
	float mat5[16], rgb[3];
	int item;
	int i, j, k;

	/* get the depth of 3d-objects */
	if (type == GSGL2PS_3DPRIM_DEPTH) {
		if (status->now3d) {
			item = status->item;
			status->prims[item].ptr = loc-2;
		}
	}

	/* get info about fonts */
	loc++;
	ifnts = *loc; loc+=2;
	ifntr = *loc; loc+=2;
	chh = *loc; loc+=2;
	chw = *loc; loc+=2;
	chsp = *loc; loc+=2;
	angle = *loc; loc+=2;
	tilt = *loc; loc+=2;
	x = *loc; loc+=2;
	y = *loc; loc+=2;
	rgb[0] = *loc; loc+=2;
	rgb[1] = *loc; loc+=2;
	rgb[2] = *loc; loc+=2;
	nmax = *loc;
	if (nmax > 0) {
		iascs = (int *) gsgl_malloc_atomic (sizeof(int) * nmax);
		for (i=0; i<nmax; i++) {
			loc+=2;
			iascs[i] = *loc;
		}
		loc++;
	} else {
		loc++;
		return loc;
	}
	/* TimesRoman is #0 here, #32 in GSAF. */
	ifont = ifnts - 32;

	vec1[0] = x; 
	vec1[1] = y;
	vec1[2] = 0.0;
	vec1[3] = 1.0;
	gsgl_mat16_mult_vec4 (status->modelview, vec1, vec2);
	gsgl_mat16_mult_vec4 (status->projection, vec2, vec1);

	/* get x, y in window-coordinates */
	x = vec1[0]/vec1[3];
	y = vec1[1]/vec1[3];
	x = (x + 1.0) * 0.5 * GSGLBASIC_WINDOWWIDTH;
	y = (y + 1.0) * 0.5 * GSGLBASIC_WINDOWHEIGHT;

	if (x < -0.3 * GSGLBASIC_WINDOWWIDTH || x > 1.3 * GSGLBASIC_WINDOWWIDTH) return loc; /* Fix me */
	if (y < -0.3 * GSGLBASIC_WINDOWHEIGHT || y > 1.3 * GSGLBASIC_WINDOWHEIGHT) return loc; /* Fix me */

	/* get depth of objects */
	depth = (vec1[2]/vec1[3]+1.0)*0.5;

	if (type == GSGL2PS_3DPRIM_NUM) {
		if (status->now3d) (status->nprimitives)++;
	} else if (type == GSGL2PS_3DPRIM_DEPTH) {
		if (status->now3d) {
			item = status->item;
			status->prims[item].linewidth = status->linewidth;
			status->prims[item].OnlyGouraud = status->graudonly;
			status->prims[item].depth = depth;
			for (i=0; i<16; i++) {
				status->prims[item].modelview[i] = status->modelview[i];
				status->prims[item].projection[i] = status->projection[i];
			}
			(status->item)++;
		}
	} else if (
		((type == GSGL2PS_HEADER || type == GSGL2PS_2D) && !status->now3d) || 
		(type == GSGL2PS_3D && status->now3d)) {
		
		/* character size */
		if (ifnts >= 40 && ifnts <= 43) {
			dchs = chh * CM * 1.666667;
		} else {
			dchs = chh * CM * 1.466667;
		}
		/* character width ? */
		dchss = dchs * 0.8;
		/* character space */
		dchsp = (chsp - chh) * CM;

		if (g_imv) {
			fprintf(psfile, "S\n");
			g_imv = 0;
		}
		if (g_ipoly) gsgl2ps_drawpoly_buffer (status);
		
		fprintf(psfile,"u\n"); /* start grouping */

		color (rgb[0], rgb[1], rgb[2], 3);

		g_ifused [ifont] = GSGL_TRUE;

		switch(ifont) {
		case 0: fprintf(psfile,"/Times-Roman %f %f %f 0 z\n",dchs,dchss,dchsp); break;
		case 1: fprintf(psfile,"/Times-Italic %f %f %f 0 z\n",dchs,dchss,dchsp); break;
		case 2: fprintf(psfile,"/Times-Bold %f %f %f 0 z\n",dchs,dchss,dchsp); break;
		case 3: fprintf(psfile,"/Times-BoldItalic %f %f %f 0 z\n",dchs,dchss,dchsp);break;
		case 4: fprintf(psfile,"/Helvetica %f %f %f 0 z\n",dchs,dchss,dchsp); break;
		case 5: fprintf(psfile,"/Helvetica-Oblique %f %f %f 0 z\n",dchs,dchss,dchsp);
			break;
		case 6: fprintf(psfile,"/Helvetica-Bold %f %f %f 0 z\n",dchs,dchss,dchsp); break;
		case 7: fprintf(psfile,"/Helvetica-BoldOblique %f %f %f 0 z\n",dchs,dchss,dchsp);
			break;
		case 8: fprintf(psfile,"/Courier %f %f %f 0 z\n",dchs,dchss,dchsp); break;
		case 9: fprintf(psfile,"/Courier-Oblique %f %f %f 0 z\n",dchs,dchss,dchsp); break;
		case 10: fprintf(psfile,"/Courier-Bold %f %f %f 0 z\n",dchs,dchss,dchsp); break;
		case 11: fprintf(psfile,"/Courier-BoldOblique %f %f %f 0 z\n",dchs,dchss,dchsp);
			break;
		case 12: fprintf(psfile,"/Symbol %f %f %f 0 z\n",dchs,dchss,dchsp); break;
		}
		
		/* mat1 = distortion matrix */
		mat1[0][0] = 1.0;
		mat1[0][1] = -tan(tilt * DEG);
		mat1[1][0] = 0.0;
		mat1[1][1] = 1.0;
		
		/* mat2 = rotation matrix */
		mat2[0][0] = cos(angle * DEG);
		mat2[0][1] = -sin(angle * DEG);
		mat2[1][0] = sin(angle * DEG);
		mat2[1][1] = cos(angle * DEG);
		
		/* mat5 = projection * modelview */
		for (i=0; i<4; i++) {
		    for (j=0; j<4; j++) {
			mat5[i+4*j] = 0.0;
			for (k=0; k<4; k++) {
				mat5[i+4*j]+=status->projection[i+4*k]*status->modelview[k+4*j];
			}
		    }
		}
		
		vec0[0]=0.0;
		vec0[1]=0.0;
		vec0[2]=0.0;
		vec0[3]=1.0;
		gsgl_mat16_mult_vec4 (mat5, vec0, vec1);
		vec0[0]=1.0;
		vec0[1]=0.0;
		vec0[2]=0.0;
		vec0[3]=1.0;
		gsgl_mat16_mult_vec4 (mat5, vec0, vec2);
		vec0[0]=0.0;
		vec0[1]=1.0;
		vec0[2]=0.0;
		vec0[3]=1.0;
		gsgl_mat16_mult_vec4 (mat5, vec0, vec3);

		/* mat3 = making small effect by projection matrix (perspective) */
		mat3[0][0] = (vec2[0]/vec2[3]-vec1[0]/vec1[3])*0.5*GSGL_PAPER_WIDTH;
		mat3[1][0] = (vec2[1]/vec2[3]-vec1[1]/vec1[3])*0.5*GSGL_PAPER_HEIGHT;
		mat3[0][1] = (vec3[0]/vec3[3]-vec1[0]/vec1[3])*0.5*GSGL_PAPER_WIDTH;
		mat3[1][1] = (vec3[1]/vec3[3]-vec1[1]/vec1[3])*0.5*GSGL_PAPER_HEIGHT;
		
		/* mat1 = mat3 * mat2 * mat1 */
		if (g_irotate) {
			mult_matrix (mat2, mat1, mat4);
			mult_matrix (mat3, mat4, mat1);
		} else {
			mult_matrix (mat2, mat1, mat4);
			mult_matrix (mat3, mat4, mat2);
			mat3[0][0] = cos(90.0 * DEG);
			mat3[0][1] = -sin(90.0 * DEG);
			mat3[1][0] = sin(90.0 * DEG);
			mat3[1][1] = cos(90.0 * DEG);
			mult_matrix (mat3, mat2, mat1);
		}
		fprintf(psfile,"[%1.3f %1.3f %1.3f %1.3f %1.3f %1.3f ] e\n(",
			mat1[0][0],mat1[1][0],mat1[0][1],mat1[1][1],
			ps_x(x,y,status),ps_y(x,y,status));
		
		for(i=0; i<nmax; i++) {
			switch(iascs[i]) {
			case '(': fprintf(psfile,"\\050"); break;
			case ')': fprintf(psfile,"\\051"); break;
			case '<': fprintf(psfile,"\\074"); break;
			case '>': fprintf(psfile,"\\076"); break;
			case '[': fprintf(psfile,"\\133"); break;
			case ']': fprintf(psfile,"\\135"); break;
			case '{': fprintf(psfile,"\\173"); break;
			case '}': fprintf(psfile,"\\175"); break;
			case 92: fprintf(psfile,"\\"); break;
			case '%': fprintf(psfile,"\\045"); break;
			default: 
				if (isprint (iascs[i])) {
					fprintf(psfile,"%c",iascs[i]);
				} else {
					fprintf(psfile,"\\%o",iascs[i]);
				}
			}
		}
		fprintf(psfile,") t T\nU\n"); /* end grouping */
	}
	return loc;
}
