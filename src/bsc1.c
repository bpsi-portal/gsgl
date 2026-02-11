#include "gsaf.h"
#include "gsgl.h"
#include "utils.h"
#include "utils2.h"
#include "base.h"
#include "matrix.h"
#include <fenv.h>
#include "text.h"
#include "bsc1.h"
#include "bsc2.h"
#include "fileio_base.h"
#include "text_hersey.h"
#include "unix.h"
#include "device.h"
#include "xwin.h"
#include "ogl.h"
#include "iof.h"
#include "displaylist.h"
#include "gui_common.h"

static void sffile (void);
static void sfhead (void);
static void sfhelp (void);
static void sfoptn (void);
static void gs_sfmark (float x, float y, gsglPage *);
static void gs_sfstmk (int imrk, float hmrk, float wmrk, float angl, float tilt, gsglPage *);
static void gs_sfchar (float *x, float *y, int ins, int ind, int idim, gsglPage *);

static gsglboolean OpenGSAF = GSGL_FALSE;
static gsglLibrary *Library = NULL;
static gsglWindow *Window0 = NULL;
static gsglPage *Page = NULL;

static const int Is[530] = {
	200000, 128024, 123022, 200000, 118016, 138036, 200000, 106046, 
	144004, 118012, 132038, 200000, 103033,  44035,  15006,  17047,
	128022, 200000,  48118,   7016, 27018, 132043,  34023,  32200,
	143022,  12003,   4027,  18007, 42200, 128026, 200000, 128016,
	14022, 200000, 128036,  34022,	200000, 103046, 143006, 127025, 
	124022, 200000, 105045, 127023,	200000, 123011, 200000, 105045, 
	200000, 123012, 113022, 200000,  48200, 103012,  32043,  47038, 
	18007,   3200,      0, 117028,	 22200, 107018,  38047,  46003, 
	2042, 200000, 107018,  38047,	 46035,  15135,  44043,  32012, 
	3200, 132038,   4044, 200000,	103012,  32043,  44035,   5008, 
	48200, 104015,  35044,  43032,	 12003,   7018,  38047, 200000, 
	108048,  12200, 115004,   3012,	 32043,  44035,  15006,   7018, 
	38047,  46035, 200000, 103012,	 32043,  47038,  18007,   5014, 
	34045, 200000, 116025, 126015,	113022, 123012, 200000, 116025, 
	126015, 123011, 200000, 148038,	  5032,  42200, 106046, 104044, 
	200000,  12045,  18008, 200000,	107018,  38047,  46025,  24123, 
	22200, 134036,  26015,  14044,	 47038,  18007,   3012,  42200, 
	5028,  45042, 104044, 200000,	  8038,  47046,  35005, 135044, 
	43032,   2200, 147038,  18007,	  3012,  32043, 200000,   8028, 
	46044,  22002, 200000, 148008,	  2042, 105035, 200000,   8048, 
	105035, 200000, 147038,  18007,	  3012,  32043,  45025, 200000, 
	8105,  45148,  42200, 118038,	128022, 112032, 200000, 103012, 
	32043,  48200,   8148,   4115,	 42200, 108002,  42200,   8025, 
	48042, 200000,   8042,  48200,	103012,  32043,  47038,  18007, 
	3200,   8038,  47046,  35005,	200000, 103012,  32043,  47038, 
	18007,   3124,  42200,   8038,	 47046,  35005, 125042, 200000, 
	103012,  32043,  44035,  15006,	  7018,  38047, 200000, 108048, 
	128022, 200000, 108003,  12032,	 43048, 200000, 108022,  48200, 
	108006,  12026,  32046,  48200,	 48108,  42200, 108025, 148025, 
	22200, 108048,   2042, 200000,	138018,  12032, 200000, 108042, 
	200000, 118038,  32012, 200000,	106028,  46200, 100040, 200000, 
	128036, 200000, 135026,  16005,	  3012,  22033, 136032,  42200, 
	108018,  12115,  26036,  45043,	 32022,  13200, 136016,   5003, 
	12032,  43200, 135026,  16005,	  3012,  22033, 128038,  32042, 
	200000, 104034,  45036,  16005,	  3012,  32043, 200000, 105025, 
	137028,  17012, 200000, 101010,	 20031,  36046, 135026,  16005, 
	3012,  22033, 200000, 108018,	 12115,  26036,  45042, 200000, 
	128027, 116026,  22112,  32200,	101010,  20031,  36026, 138037, 
	200000, 108018,  12146,  14042,	200000, 118028,  22112,  32200, 
	6105,  16025,  22125,  36045,	 42200, 112016,   6115,  26036, 
	45042, 200000, 103012,  32043,	 45036,  16005,   3200, 100010, 
	16006, 115026,  36045,  43032,	 22013, 200000, 133022,  12003, 
	5016,  26035, 146036,  30040,	200000, 112016,   6115,  26036, 
	45200, 103012,  32043,  34014,	  5016,  36045, 200000, 106036, 
	118013,  22032,  43200, 106003,	 12022,  33136,  32042, 200000, 
	106022,  46200, 106012,  24032,	 46200,  46106,  42200, 101010, 
	20031,  36046, 106003,  12022,	 33200, 106046,   2042, 200000, 
	138027,  26015,  24023,  32200,	128025, 124021, 200000, 118027, 
	26035,  24023,  12200, 105016,	 25034,  45200,  42048,   8002, 
	200000,      0,      0,      0,      0,       0,      0,     0, 
	125025, 200000, 114034,  36016,  14200, 105045, 127023, 200000, 
	103047, 107043, 200000, 104046, 144006, 127023, 200000, 103047, 
	107043, 145005, 127023, 200000, 103043,  27003, 200000, 103043, 
	47007,   3200, 105023,  45027,   5200, 104013,  33044,  46037, 
	17006,   4200, 103043,  27003, 125025, 200000, 103043,  47007, 
	3125,  25200, 105023,  45027,   5125,  25200, 104013,  33044, 
	46037,  17006,   4125,  25200, 103043,  47007,   3047, 107043, 
	200000, 105023,  45027,   5045, 127023, 200000,      0,      0, 
	0,      0,      0,      0,      0,      0, 
	125027,  47043,   3007,  27200, 125027,  37046,  44033,  13004, 
	6017,  27200, 125027,  43003,  27200, 105045, 127023, 200000, 
	147003, 107043, 200000, 125027,  45023,   5027, 200000, 123027, 
	45005,  27200, 103047,   7043, 200000, 107047,   3043, 115035, 
	200000, 107025,  23147,  25200, 125047, 107016,  14034,  36016, 
	114003, 134043, 200000, 103047, 107043, 123027, 105045, 200000, 
	103047,   7043,   3200, 127023, 200000, 125027,   4044,  27123, 
	46006,  23200, 125045, 200000};

static const short Ns[128] = {
	401,403,406,409,412,416,421,424,
	427,430,435,439,443,447,453,401,
	471,475,481,484,487,490,494,497,
	500,504,507,514,519,522,524,529,
	1,  2,  5,  8, 13, 19, 25, 30,
	32, 35, 38, 43, 46, 48, 50, 53,
	54, 60, 62, 67, 74, 77, 82, 89,
	91,100,107,112,116,119,122,125,
	130,137,141,147,152,156,160,163,
	169,172,176,179,182,184,187,189,
	194,198,204,209,216,219,223,225,
	229,231,234,237,240,242,245,247,
	249,251,257,263,267,274,280,284,
	292,297,301,306,310,313,318,323,
	328,335,342,346,352,356,361,363,
	366,368,374,377,381,384,388,391};

#define BoolToChar(bool) (bool ? 'T': 'F')


/* initilalize library */
void gsaf_gsopen ()
{
	gsaf_gsopen_rank (0);
}

void gsaf_gsopen_rank (int rank)
{
	int year, month, day, hour, minute, second;
	int4 ich;
	int id;
	char kid;
	int error;

	if (OpenGSAF) return;
	OpenGSAF = GSGL_FALSE;

	GC_init ();
	Library = gsgl_malloc (sizeof(gsglLibrary));
	Library->OpenPage = GSGL_FALSE;
	Library->Rank = rank;
	Window0 = NULL;
	Page = NULL;

	if (rank != 0) return;

	gsaf_gudate (&year, &month, &day, &hour, &minute, &second);
	gsaf_gu_srand (hour*3600+minute*60+second);
	gsaf_gutime (&(Library->TimeOpen));

	OpenGSAF = GSGL_TRUE;
	Library->OpenPage = GSGL_FALSE;
	Library->KeepHeader = GSGL_FALSE;
	Library->Pagenum = 0;
	Library->OpenSaveFile = GSGL_FALSE;
	Library->SaveToFile = GSGL_FALSE;

	Library->AskInSave = GSGL_TRUE;
	Library->Bell = GSGL_TRUE;

	gsaf_reset_quit_callback ();
	gsgl_servercallback_reset ();

	gsaf_gssize (GSGL_PAPER_WIDTH, GSGL_PAPER_HEIGHT - GSGL_HEADER_HEIGHT);
	gsaf_gstitl ("/ /");

	gsgl_device_judge (Library);
	gsgl_device_type (&ich, Library);
/*
 *      ich = 1: Resolution must be given
 */
	if (ich == 1) {
		do {
			fprintf (stdout,
#ifdef USE_INLINE
    " # INPUT DISPLAY TYPE : 1)512x380 2)640x475 3)768x570 4)896x665 5)1024x760\n"
    "                        6)1280x950 f)ull screen u)ser defined 0)quiet\n"
    "                        i)nline\n");
#else
    " # INPUT DISPLAY TYPE : 1)512x380 2)640x475 3)768x570 4)896x665 5)1024x760\n"
    "                        6)1280x950 f)ull screen u)ser defined 0)quiet\n");
#endif
			READ_1_CHAR_F77 (&kid, &error);
			if (error == 2) exit (0);
			kid = tolower (kid);

		} while (error == 1 ||
			 (kid != '1' &&
			  kid != '2' && 
			  kid != '3' &&
			  kid != '4' &&
			  kid != '5' && 
			  kid != '6' &&
			  kid != 'u' && 
			  kid != 'f' &&
#ifdef USE_INLINE
			  kid != 'i' &&
#endif
			  kid != '0'));
		ich = kid;
	}
/*
 *    ich = 0: Non Interactive device
 *    ich < 0: Interactive device but don't ask in saving
 *    ich > 0: Interactive device
 *    abs(ich): Key input
 */
	gsgl_device_open (&ich, Library);
	if (ich <= 0) Library->AskInSave = GSGL_FALSE;

	if (ich != 0) {
		for (;;) {
		id = abs (ich);
		if (id == 1) {
			do {
				fprintf (stdout, 
				" # INPUT : (C)ONTINUE,(O)PTION,(F)ILE,(H)ELP,(Q)UIT\n");
				fflush (stdout);

				READ_1_CHAR_F77 (&kid, &error);
				if (error == 2) exit (0);
				kid = tolower (kid);

			} while (error == 1 ||
				 (kid != 'c' &&
				  kid != 'o' &&
				  kid != 'f' &&
				  kid != 'h' &&
				  kid != 'q'));
		} else {
			kid = id;
			kid = tolower (kid);
		}
		if (kid == 'c') {
			break;
		} else if (kid == 'o') {
			sfoptn ();
		} else if (kid == 'f') {
			sffile ();
			gsfon ();
		} else if (kid == 'h') {
			sfhelp ();
		} else if (kid == 'q') {
			gsaf_gsclos ();
			exit (0);
		}
		}
	}
}

/* closing message */
void gsgl_close_message (gsglLibrary *lib)
{
	float time;

	gsaf_gutime (&time);
	gsgl_usedtime (time - lib->TimeOpen, lib);
}

/* system terminate */
void gsaf_gsclos ()
{
	int4 ich;

	if (!OpenGSAF) return;
	if (gsgl_inq_page_is_open ()) return;

	if (Library->OpenSaveFile) gsfcls (Library);
	gsgl_device_close (&ich, Library);
    /* Clear floating point exception flags to avoid noisy Fortran warnings on exit. */
#ifdef FE_ALL_EXCEPT
    feclearexcept(FE_ALL_EXCEPT);
#endif
	
	OpenGSAF = GSGL_FALSE;
	if (ich != 0) gsgl_close_message (Library);
}

/* set page size */
void gsaf_gssize (float x, float y)
{
	if (!OpenGSAF) return;
	if (gsgl_inq_page_is_open ()) return;
	
	if (x <= 0.0 || y <= 0.0) gsgl_error ("gspagesize", "x and y must be positive value");
	
	Library->Size_x = x;
	Library->Size_y = y;
}

/* set page title */
void gsaf_gstitl (char *title)
{
	if (!OpenGSAF) return;
	if (gsgl_inq_page_is_open ()) return;
	
	gsaf_gudstr (title, Library->Header);
}

/* set option */
/*void  gsoptn (KOPTDL,IOPT)
  {
  CHARACTER KOPTDL*4,KOPT*2;
  
  if (!OpenGSAF) return;
  if (gsgl_inq_page_is_open ()) return;
  
  CALL GUDSTR(KOPTDL,4,KOPT,IKOPT);
  IF (IKOPT == 0) {
  CALL SFOPTN
  }
  }*/

void gsgl_keep_header ()
{
	Library->KeepHeader = GSGL_TRUE;
}


static void gsgl_init_internal_variables (gsglPage *p)
{
	p->base->Setvew_physical.xmin = 0.0;
	p->base->Setvew_physical.xmax = GSGL_PAPER_WIDTH;
	p->base->Setvew_physical.ymin = 0.0;
	p->base->Setvew_physical.ymax = GSGL_PAPER_HEIGHT;
	p->base->Setvew_logical.xmin = 0.0;
	p->base->Setvew_logical.xmax = GSGL_PAPER_WIDTH;
	p->base->Setvew_logical.ymin = 0.0;
	p->base->Setvew_logical.ymax = GSGL_PAPER_HEIGHT;
	p->base->Clip.xmin = 0.0;
	p->base->Clip.xmax = GSGL_PAPER_WIDTH;
	p->base->Clip.ymin = 0.0;
	p->base->Clip.ymax = GSGL_PAPER_HEIGHT;
	p->base->Fntreal = GSGL_DEFAULT_FNT;
	p->base->LineType = 0;
	p->base->LineWidth = 2;
	p->base->LineColor = 7;
	gsaf_move (GSGL_ORIGIN, GSGL_ORIGIN);
	gsaf_move2d (GSGL_ORIGIN, GSGL_ORIGIN);
	gsaf_move3d (GSGL_ORIGIN, GSGL_ORIGIN, GSGL_ORIGIN);
	gsaf_setmks (0, 0.2);
}

gsglLibrary * gsgl_inq_lib ()
{
	return Library;
}

gsglPage * gsgl_inq_page ()
{
	return Page;
}

/* page initialize */
void gsaf_pages ()
{
	if (!OpenGSAF) return;
	if (gsgl_inq_page_is_open ()) return;

	Page = gsgl_page_new (Library, Window0);

	Library->OpenPage = GSGL_TRUE;
	Library->Pagenum++;
	
	Window0 = gsgl_device_pages (Library->Pagenum,Library->Size_x,Library->Size_y,Library->KeepHeader,Page);
	gsgl_init_internal_variables (Page);

	if (!Library->KeepHeader) {
		gsgl_begin_header (Page);
		gsaf_offvew ();
		/*gsaf_setclp (0.0, GSGL_PAPER_WIDTH, 0.0, GSGL_PAPER_HEIGHT);*/
		gsaf_offclp ();
		gsaf_setlin (0, 2, 7);
		gsaf_setfnt (GSGL_HEADER_FNT);
		gsaf_setchs (GSGL_DEFAULT_CHS, 0.0);
		sfhead ();
		gsgl_end_header (Page);
		gsaf_setlin (0, 2, 7);
		gsaf_offvew (); /* erase でこの先の displaylist は削除される */
	}
	gsaf_setfnt (GSGL_DEFAULT_FNT);
	gsaf_setchs (GSGL_DEFAULT_CHS, 0.0);
}

/* page terminate */
void gsaf_pagee ()
{
	char kid;
	int4 ich;
	gsglboolean save;
	int error;
	char c;

	if (!OpenGSAF) return;
	if (!gsgl_inq_page_is_open ()) return;
	
	if (Library->Bell) gsgl_bell (Library);

	ich = 1;
	gsgl_device_pagee (&ich, gsgl_inq_page ());
    /* Clear floating point exception flags to avoid noisy Fortran warnings on exit. */
#ifdef FE_ALL_EXCEPT
    feclearexcept(FE_ALL_EXCEPT);
#endif

/*
 * ich == 0: non interactive device
 * abs(ich): key input
 */
	Library->OpenPage = GSGL_FALSE;
	Library->KeepHeader = GSGL_FALSE;
	save = Library->SaveToFile;

	if (ich != 0) {
		if (ich > 32) {
			switch (ich) {
			case 's':
				if (!Library->OpenSaveFile) {
					sffile ();
				}
				save = GSGL_TRUE;
				goto EXIT;
			case 'q':
				do {
					fprintf (stdout, " # ARE YOU SURE TO QUIT ? (Y/N)\n");
					READ_1_CHAR_F77 (&c, &error);
					if (error == 2) break;
					c = tolower (c);
				} while (c != 'y' && c != 'n');
			
				if (c == 'y') {
					gsgl_finish_every_page (gsgl_inq_page ());
					gsaf_gsclos ();
					exit (0);
				}
				break;
			case 'h':
				sfhelp ();
				break;
			default:
				break;
			}
		}
		if (Library->SaveToFile) {
			do {
				fprintf(stdout, " # SAVE GRAPHIC DATA IN FILE (Y)ES OR (N)O ?\n");
				fflush (stdout);

				READ_1_CHAR_F77 (&kid, &error);
				if (error == 2) break;

			} while (error == 1 || (kid != 'y' && kid != 'n'));
			
			if (error != 2) {
				if (tolower(kid) == 'y') {
					save = GSGL_TRUE;
				} else {
					save = GSGL_FALSE;
				}
			}
		}
	}
 EXIT:
	/* save the page into a file (interactive case) */
	if (ich != 0 && save) {
		flsave (Library->Pagenum, Page);
	}
	gsgl_finish_every_page (gsgl_inq_page ());
}

/* page quickly terminate with file save */
void gsaf_pagey ()
{
	int4 ich;
	gsglPage *p;
		
	if (!OpenGSAF) return;
	if (!gsgl_inq_page_is_open ()) return;

	p = gsgl_inq_page ();
	ich = 0;
	gsgl_device_pagee (&ich, p);

	Library->OpenPage = GSGL_FALSE;

	if (Library->SaveToFile) flsave (Library->Pagenum, p);
	gsgl_finish_every_page (p);
}

/* page quickly terminate without file save */
void gsaf_pagen ()
{
	int4 ich;
	gsglPage *p;

	if (!OpenGSAF) return;
	if (!gsgl_inq_page_is_open ()) return;

	p = gsgl_inq_page ();
	ich= 0;
	gsgl_device_pagee (&ich, p);
	gsgl_finish_every_page (p);

	Library->OpenPage = GSGL_FALSE;
}

/* set view port */
void gsaf_setvew (float pxmin, float pxmax, float pymin, float pymax, 
		  float xmin, float xmax, float ymin, float ymax)
{
	gsglPage *p;

	if (!gsgl_inq_page_is_open ()) return;
	p = gsgl_inq_page ();

	if (fabs(xmax - xmin) < 1.e-32 || fabs(ymax - ymin) < 1.e-32) {
		gsgl_error ("setvew", "xmin,xmax,ymin,ymax = %f,%f,%f,%f", xmin,xmax,ymin,ymax);
	} else if (pxmax < pxmin || pymax < pymin) {
		gsgl_error ("setvew", "pxmin,pxmax,pymin,pymax = %f,%f,%f,%f",
			    pxmin,pxmax,pymin,pymax);
	}

	p->base->Setvew_physical.xmin = pxmin;
	p->base->Setvew_physical.xmax = pxmax;
	p->base->Setvew_physical.ymin = pymin;
	p->base->Setvew_physical.ymax = pymax;
	p->base->Setvew_logical.xmin = xmin;
	p->base->Setvew_logical.xmax = xmax;
	p->base->Setvew_logical.ymin = ymin;
	p->base->Setvew_logical.ymax = ymax;

	gsaf_offclp ();
	gsgl_matrix_setvew (pxmin,pxmax,pymin,pymax,xmin,xmax,ymin,ymax,p);
}

/* cancel view port */
void gsaf_offvew ()
{
	float x1, x2, y1, y2;

	if (!gsgl_inq_page_is_open ()) return;

	x1 = Library->Size_x;
	y1 = Library->Size_y;
	x2 = Library->Size_x;
	y2 = Library->Size_y;
	
	gsaf_setvew (0.0,x1,0.0,y1,0.0,x2,0.0,y2);
}

/* inquire view port */
void gsaf_inqvew (float *pxmin, float *pxmax, float *pymin, float *pymax, 
		  float *xmin, float *xmax, float *ymin, float *ymax)
{
	gsglPage *p;

	if (!gsgl_inq_page_is_open ()) return;
	p = gsgl_inq_page ();

	*pxmin = p->base->Setvew_physical.xmin;
	*pxmax = p->base->Setvew_physical.xmax;
	*pymin = p->base->Setvew_physical.ymin;
	*pymax = p->base->Setvew_physical.ymax;
	*xmin  = p->base->Setvew_logical.xmin;
	*xmax  = p->base->Setvew_logical.xmax;
	*ymin  = p->base->Setvew_logical.ymin;
	*ymax  = p->base->Setvew_logical.ymax;
}

/* translate projected image */
void gsgl_projimgtranslate (float dx, float dy)
{
	if (!gsgl_inq_page_is_open ()) return;
	gsgl_projimg_translate (dx, dy);
}

/* scale projected image */
void gsgl_projimgscale (float xratio, float yratio)
{
	if (!gsgl_inq_page_is_open ()) return;
	gsgl_projimg_scale (xratio, yratio);
}

/* rotate projected image */
void gsgl_projimgrotate (float degree) /* angle in degree */
{
       if (!gsgl_inq_page_is_open ()) return;
       gsgl_projimg_rotate (degree);
}

/* clear projected image transform matrix */
void gsgl_projimgidentity ()
{
	if (!gsgl_inq_page_is_open ()) return;
	gsgl_projimg_identity ();
}

/* set clip region */
void gsaf_setclp (float xmin, float xmax, float ymin, float ymax)
{
	float eqn[4];
	gsglPage *p;

	if (!gsgl_inq_page_is_open ()) return;

	p = gsgl_inq_page ();

	p->base->Clip.xmin = xmin;
	p->base->Clip.xmax = xmax;
	p->base->Clip.ymin = ymin;
	p->base->Clip.ymax = ymax;

	eqn[0] = 1.0;
	eqn[1] = 0.0;
	eqn[2] = 0.0;
	eqn[3] = -xmin;
	gsgl_clip_disable (0, p);
	gsgl_clip_enable_normal (0, eqn, p);

	eqn[0] = -1.0;
	eqn[1] = 0.0;
	eqn[2] = 0.0;
	eqn[3] = xmax;
	gsgl_clip_disable (1, p);
	gsgl_clip_enable_normal (1, eqn, p);

	eqn[0] = 0.0;
	eqn[1] = 1.0;
	eqn[2] = 0.0;
	eqn[3] = -ymin;
	gsgl_clip_disable (2, p);
	gsgl_clip_enable_normal (2, eqn, p);

	eqn[0] = 0.0;
	eqn[1] = -1.0;
	eqn[2] = 0.0;
	eqn[3] = ymax;
	gsgl_clip_disable (3, p);
	gsgl_clip_enable_normal (3, eqn, p);
}

/* cancel clip region */
void gsaf_offclp ()
{
	if (!gsgl_inq_page_is_open ()) return;
	gsgl_clip_delete (gsgl_inq_page ());
}

/* inquire clip region */
void gsaf_inqclp (float *xmin, float *xmax, float *ymin, float *ymax)
{
	gsglPage *p;
	if (!gsgl_inq_page_is_open ()) return;

	p = gsgl_inq_page ();
	*xmin = p->base->Clip.xmin;
	*xmax = p->base->Clip.xmax;
	*ymin = p->base->Clip.ymin;
	*ymax = p->base->Clip.ymax;
}

/* move to */
void gsaf_move (float x, float y)
{
	if (!gsgl_inq_page_is_open ()) return;
	
	gsgl_move (x, y, gsgl_inq_page ());
}

/* draw line */
void gsaf_draw (float x, float y)
{
	if (!gsgl_inq_page_is_open ()) return;
	
	gsgl_draw (x, y, gsgl_inq_page ());
}

/* draw mark */
void gsaf_mark (float x, float y)
{
	float x1, y1;
	gsglPage *p;

	if (!gsgl_inq_page_is_open ()) return;
	p = gsgl_inq_page ();

	if (p->base->Imk < 0) gsaf_draw (x, y);
	
	gsgl_separate_objects (p);
	gsglprojectview (x, y, &x1, &y1);
	gs_sfmark (x1, y1, p);
	gsgl_separate_objects (p);
	gsaf_move (x, y);
}

/* inquire position */
void gsaf_inqpos (float *x, float *y)
{
	if (!gsgl_inq_page_is_open ()) return;
	
	gsgl_inqpos (x, y, gsgl_inq_page ());
}

void setlnw_text (gsglPage *p)
{
	float scale;

	if (!gsgl_inq_page_is_open ()) return;
	gsgl_inq_projection_scale (&scale, p);
	gsaf_setlnw (0.08*MIN(p->base->Tflx,p->base->Tfly));
}

/* draw text */
void gsaf_text (char *text)
{
	int fnt;
	float ws;
	float x1, y1;
	float x2, y2;
	gsglboolean k;
	gsglPage *p;

	if (!gsgl_inq_page_is_open ()) return;
	p = gsgl_inq_page ();

	k = gsgl_inq_view_matrix_is_unity (p);
	if (!k) gsgl_clean_view_matrix (p);
	gsgl_separate_objects (p);

	gsaf_inqfnt (&fnt);
	if (fnt >=2 && fnt < 19) {
		gsaf_inqlnw (&ws);
		setlnw_text (p);
	}

	gsaf_inqpos (&x1, &y1);
	gsglprojectview (x1, y1, &x2, &y2);
	gsgltext (text, strlen(text), &x2, &y2, 1, p);
	gsglunprojectview (x2, y2, &x1, &y1);
	gsaf_move (x1, y1);

	if (fnt >= 2 && fnt < 19) {
		gsaf_setlnw (ws);
	}
	gsgl_separate_objects (p);
	if (!k) gsgl_load_projection_matrix (p);
}

/* inquire text bounding box */
void gsaf_inqtextbbox (char *text, float *size, float *height, float *depth)
{
	if (!gsgl_inq_page_is_open ()) return;

	gsglinqtextbbox (text,strlen(text),size,height,depth,gsgl_inq_page ());
}

/* inquire text length (cm) */
void gsaf_inqtsz (char *text, float *size)
{
	if (!gsgl_inq_page_is_open ()) return;

	gsglinqtextsize (text,strlen(text),size,gsgl_inq_page ());
}

/* set line attribute */
void gsaf_setlin (int iln, int ibl, int icl)
{
/*    Input
 *    iln [i] line type  0 : solid line 
 *                       1 : dotted line
 *                       2 : broken line
 *                       3 : 長破線  
 *                       4 : 一点鎖線 
 *                       5 : 長一点鎖線
 *                       6 : 二点鎖線
 *                       7 : 長二点鎖線  
 *    ibl [i] line width 0 : 最も細線 
 *                       1 : 1 dot
 *                       2 : 2 dots
 *                       ...
 *    icl [i] line color  
 *                       Background black case.
 *                       0 : black
 *                       1 : blue
 *                       2 : red
 *                       3 : 紫
 *                       4:green
 *                       5:水色 
 *                       6 : yellow
 *                       7 : white
 *                       Background white case.
 *                       0:white 
 *                       1:yellow 2:水色 3:紫 4:green 5:red 6:blue 7:black
 *    This setting make affects for lines, marks, stroke texts.
 *    線の種類は出力装置の固有機能に依存し，各直線毎にパターンが描かれるため，
 *     短い直線を連ねた場合には実線にみえる．
 *    長い直線にのみこの機能を使用した方がよい．
 *    iln,ibl,icl として -1 を指定した場合には，現在の設定値のまま変更されない．
 */
	gsglPage *p;

	if (!gsgl_inq_page_is_open ()) return;
	p = gsgl_inq_page ();

	if (iln >= 0) p->base->LineType = iln;
	if (ibl >= 0) {
		gsgl_set_ws (-1.0, p);
		gsgl_linewidth_index (ibl, p);
	}
	if (icl >= 0) {
		gsgl_set_rs (-1.0, p);
		gsgl_color_index (icl, p);
	}
}

/* inquire line atribute */
void gsaf_inqlin (int *iln, int *ibl, int *icl)
{
	gsglPage *p;

	if (!gsgl_inq_page_is_open ()) return;
	p = gsgl_inq_page ();

	*iln = p->base->LineType;
	*ibl = p->base->LineWidth;
	*icl = p->base->LineColor;
}

/* set mark attribute (full) */
/* mark type, height, width, rotation angle, tilt angle */
void gsaf_setmrk (int imrk, float hmrk, float wmrk, float angl, float tilt)
{
	gsglPage *p;

	if (!gsgl_inq_page_is_open ()) return;
	p = gsgl_inq_page ();

	p->base->Imk = imrk;
	p->base->Smrks[0]=hmrk;
	p->base->Smrks[1]=wmrk;
	p->base->Smrks[2]=angl;
	p->base->Smrks[3]=tilt;
	gs_sfstmk (imrk,hmrk,wmrk,angl,tilt,p);
}

/* set mark attribute (partial) */
void gsaf_setmks (int imrk, float smrk)
{
	if (!gsgl_inq_page_is_open ()) return;
	gsaf_setmrk (imrk,smrk,smrk,0.0,0.0);
}

/* inquire mark attribute */
void gsaf_inqmrk (int *imrk, float *hmrk, float *wmrk, float *angl, float *tilt)
{
	gsglPage *p;

	if (!gsgl_inq_page_is_open ()) return;
	p = gsgl_inq_page ();

	*imrk = p->base->Imk;
	*hmrk = p->base->Smrks[0];
	*wmrk = p->base->Smrks[1];
	*angl = p->base->Smrks[2];
	*tilt = p->base->Smrks[3];
}

/* set character attribute (full) */
void gsaf_setchr (float chh, float chw, float chsp, float angl, float tilt)
{
	if (!gsgl_inq_page_is_open ()) return;
	gsglsetcharsize (chh,chw,chsp,angl,tilt,gsgl_inq_page ());
}

/* set charcter attribute (partial) */
void gsaf_setchs (float chh, float angl)
{
	float chars[5];
	int fnt;

	if (!gsgl_inq_page_is_open ()) return;

	gsaf_inqfnt (&fnt);

	if (fnt >= 32) {
		chars[0]=chh;
		chars[1]=chh;
		chars[2]=chh;
		chars[3]=angl;
		chars[4]=0.0;
	} else {
		chars[0]=chh;
		chars[1]=chh*0.6666667;
		chars[2]=chh;
		chars[3]=angl;
		
		if (fnt == 37 || fnt == 39 || fnt == 41 || fnt == 43) {
			chars[4]=-12.0;
		} else {
			chars[4]=0.0;
		}
	}
	gsaf_setchr (chars[0],chars[1],chars[2],chars[3],chars[4]);
}

/* set font type */
void gsaf_setfnt (int fnt)
{
	if (!gsgl_inq_page_is_open ()) return;
	gsglsetfnt (fnt, gsgl_inq_page ());
}

/* inquire character attribute */
void gsaf_inqchr (float *chh, float *chw, float *chsp, float *angl, float *tilt)
{
	if (!gsgl_inq_page_is_open ()) return;
	gsglinqchr (chh, chw, chsp, angl, tilt, gsgl_inq_page ());
}

/* inquire font type */
void gsaf_inqfnt (int *fnt)
{
	if (!gsgl_inq_page_is_open ()) return;
	gsglinqfnt (fnt, gsgl_inq_page ());
}

/* erase screen */
void gsaf_eras ()
{
	if (!gsgl_inq_page_is_open ()) return;
	/*gsgl_erase ();*/
	gsgl_clear (gsgl_inq_page ());
}

/* beep sound */
void gsaf_bell ()
{
	if (!gsgl_inq_page_is_open ()) return;
	gsgl_bell (Library);
}

/* convert string with delim. to string */
void gsaf_gudstr (char *kin, char *kout)
{
	char kdl;
	int i, j;
	int nin;

	nin = strlen (kin);
	kdl = kin[0];
	i=1;

	while (kin[i] != kdl && i != nin)
		i++;
	

	if (kin[i] == kdl) {
		if (i == 1) {
			kout[0] = '\0';
		} else {
			for (j=0; j<i-1; j++)
				kout[j] = kin[j+1];
			kout[i-1] = '\0';
		}
	} else {
		kout[0] = '\0';
	}
}

/* convert string with delim. to string */
void gsaf_f77_gudstr (char *kin, int nin, char *kout, int *nout)
{
	char kdl;
	int i, j;

	kdl = kin[0];
	i=1;

	while (kin[i] != kdl && i != nin)
		i++;
	
	if (kin[i] == kdl) {
		if (i == 1) {
			*nout = 0;
		} else {
			for (j=0; j<i-1; j++)
				kout[j] = kin[j+1];
			*nout = i-1;
		}
	} else {
		*nout = 0;
	}
}

/* capitalize char */
void gsaf_gucptl (char *kid)
{
	char in = *kid;
	*kid = toupper (in);
}

/* avoid real*4 underflow */
float gsaf_guclip (double d)
{
	if (fabs(d) < 1.e-30) {
		return 0.0;
	} else if (d > 1.e+30) {
		return 1.e+30;
	} else if (d < -1.e+30) {
		return -1.e+30;
	} else {
		return (float) d;
	}
}

/* optimum num length for gvalue */
int gsaf_ngulen (float gstep)
{
	float gxl, gx;
	int ngx;

	gxl = (float) log10(gstep*0.11);
	if (gxl < 0.0) {
		ngx = -((int)-gxl);
	} else {
		ngx =  (int) gxl;
	}
	if (ngx < -4 || ngx > 4){
		gx = gxl-ngx;
		if (gx < 0.0) gx=gx+1.0;
		if (gx <= 0.15) {
			ngx=100;
		} else {
			ngx=101;
		}
	} else if (ngx >= 0) {
		ngx=0;
	}
	return -ngx;
}

/* error in the case pages is not called */
/*void gserror_pages (int *ierror)
  {
  if (!gsgl_inq_page_is_open ()) {
  gsgl_error ("gserror_pages", "pages is not called yet");
  } else {
  ierror=0;
  }
  }*/

/*void gsglsyncwithc (int i)
  {
  if (i == 0) OpenSaveFile = GSGL_FALSE;
  if (i == 1) OpenSaveFile = GSGL_TRUE;
  }*/

gsglboolean gsgl_inq_savefile_opened ()
{
	return Library->OpenSaveFile;
}

void gsgl_set_savefile_is_opened ()
{
	Library->OpenSaveFile = GSGL_TRUE;
}

void gsgl_set_savefile_is_closed ()
{
	Library->OpenSaveFile = GSGL_FALSE;
}

gsglboolean gsgl_inq_save_to_file ()
{
	return Library->SaveToFile;
}

void gsgl_set_savetofile (gsglboolean bool)
{
	Library->SaveToFile = bool;
}

gsglboolean gsgl_inq_page_is_open ()
{
	return Library->OpenPage;
}

gsglboolean gsgl_inq_gsopen_is_called ()
{
	return OpenGSAF;
}

gsglboolean gsgl_inq_ask_in_save ()
{
	return Library->AskInSave;
}

/* file name input */
static void sffile (void)
{
	char *fcsnam = NULL;
	char kid;
	int ind;
	int error;

	if (gsgl_inq_savefile_opened ()) {
		do {
			fprintf (stdout, " # CURRENT FILE IS GOING TO BE CLOSED.  "
				 "ARE YOU SURE (Y) OR (N) ?\n");
			fflush (stdout);

			READ_1_CHAR_F77 (&kid, &error);
			if (error == 2) break;
			kid = tolower (kid);

		} while (error == 1 || (kid != 'y' && kid != 'n'));

		if (error != 2) {
			if (kid == 'y') {
				gsfoff ();
				gsfcls (Library);
			}
		}
	}

	if (!gsgl_inq_savefile_opened ()) {
		do {
			fcsnam = flname ();
			if (fcsnam == NULL) break;
			gsfopn (fcsnam, &ind, Library);
		} while (ind != 0);
	}
}

/* draw page header */
static void sfhead (void)
{
	int ndy,ndm,ndd,nth,ntm,nts;
	char kh1[19], kh2[5], kp[5];
	char k1[4], k2[4], k3[4], k4[4], k5[4], k6[4];

	if (strlen(Library->Header) == 0) return;
	
	gsaf_gudate (&ndy, &ndm, &ndd, &nth, &ntm, &nts);
	sprintf (k1, "%3d", 100+ndy);
	sprintf (k2, "%3d", 100+ndm);
	sprintf (k3, "%3d", 100+ndd);
	sprintf (k4, "%3d", 100+nth);
	sprintf (k5, "%3d", 100+ntm);
	sprintf (k6, "%3d", 100+nts);
	
	sprintf (kh1, "%s-%s-%s  %s:%s:%s", &k1[1], &k2[1], &k3[1], &k4[1], &k5[1], &k6[1]);

	sprintf (kp, "%4d", 1000+Library->Pagenum);
	sprintf (kh2, "#%.3s", &kp[1]);
	
	if (Library->Size_x > 25.7) gsaf_setchs (0.30, 0.0);
	gsaf_move (0.1, Library->Size_y+0.25);
	gsaf_text (kh1);
	gsaf_move (8.15, Library->Size_y+0.25);
	gsaf_text (Library->Header);
	gsaf_move (Library->Size_x - 1.5, Library->Size_y + 0.25);
	gsaf_text (kh2);
	if (Library->Size_x > 25.7) gsaf_setchs (0.35, 0.0);
}

/* show help informations */
static void sfhelp (void)
{
	fprintf (stdout, " ### GSAF INFORMATION ###\n");
	fprintf (stdout, " # YOU CAN INPUT ONE OF THE FOLLOWING CHARACTERS"
		 " AT THE END OF EACH PAGE.\n");
	fprintf (stdout, " #    C OR CR : CONTINUE.\n");
	fprintf (stdout, " #    F       : OPEN FILE AND START TO SAVE PAGES\n");
	fprintf (stdout, " #    S       : SAVE THIS PAGE, OPEN FILE IF NOT YET.\n");
	fprintf (stdout, " #    Y       : SAVE THIS PAGE IN FILE. \n");
	fprintf (stdout, " #    N       : DO NOT SAVE THIS PAGE IN FILE. \n");
	fprintf (stdout, " #    X       : ON/OFF SAVING PAGES. \n");
	fprintf (stdout, " #    B       : ON/OFF BELL SOUND.\n");
	fprintf (stdout, " #    D       : DUMP DOT IMAGE. (X)\n");
	fprintf (stdout, " #    O       : SELECT OPTIONS.\n");
	fprintf (stdout, " #    H       : INFORMATIONS.\n");
	fprintf (stdout, " #    Q       : QUIT RUN.\n");
}

/* interactive option setting */
static void sfoptn (void)
{
	float sizex, sizey;
	int error;
	char buffer[256];
	int l;
	
	do {
		fprintf (stdout, " # PAGE SIZE (CM) : [%5.2f, %5.2f]\n", Library->Size_x, Library->Size_y);
		fflush (stdout);
		
		sizex = Library->Size_x;
		sizey = Library->Size_y;
		READ_2_FLOAT_F77 (&sizex, &sizey, &error);

	} while (error == 1);
	if (error == 0) gsaf_gssize (sizex, sizey);

	do {
		fprintf (stdout, " # TITLE (40CHARS) : [/%s/]\n", Library->Header);
		fprintf (stdout, " # INPUT A STRING ENCLOSED WITH DELIMITERS\n");
		fflush (stdout);

		READ_256_CHAR_F77 (buffer, &error);

	} while (error == 1);
	if (error == 0) gsaf_gstitl (buffer);
	
	do {
		fprintf (stdout, " # FILE SAVE (LOGICAL) : [%c]\n", BoolToChar(Library->SaveToFile));
		fflush (stdout);
		
		l = Library->SaveToFile;
		READ_LOGICAL_F77 (&l, &error);

	} while (error == 1);
	if (error == 0) {
		Library->SaveToFile = l;
		if (Library->SaveToFile) {
			sffile ();
			gsfon ();
		} else {
			gsfoff ();
		}
	}
}

/* draw mark */
static void gs_sfmark (float x, float y, gsglPage *p)
{
	float xp, yp;

	xp = x - p->base->Xmkx*2.0 - p->base->Xmky*3.0;
	yp = y - p->base->Ymkx*2.0 - p->base->Ymky*3.0;
	gs_sfchar (&xp, &yp, abs(p->base->Imk), 1, 1, p);
}

/* set mark attribute */
static void gs_sfstmk (int imrk, float hmrk, float wmrk, float angl, float tilt, gsglPage *p)
{
	float deg = 0.0174533;
	float a, t, b, cost, ca, sa;
	float chx, chy;

	p->base->Imk = imrk;
	a = deg * angl;
	t = deg * tilt;
	b = a+t;
	cost = cos(t);
	
	if (cost>=0.0 && cost< 0.1) cost= 0.1;
	if (cost<0.0 && cost>-0.1) cost=-0.1;
	
	chx = 0.25 * wmrk;
	chy = 0.25 * hmrk / cost;
	ca = cos(a);
	sa = sin(a);
	p->base->Xmkx = chx * ca;
	p->base->Xmky =-chy * sin(b);
	p->base->Ymkx = chx * sa;
	p->base->Ymky = chy * cos(b);
}

/* draw one character */
static void gs_sfchar (float *x, float *y, int ins, int ind, int idim, gsglPage *p)
{
	int j;
	int il, il1, il2, il1p, il2p, il1x, il1y, il2x, il2y;
	float xx, xy, yx, yy;
	gsglboolean lmv;
	float xt, yt;

	j = Ns[ins+1-1]; /*!!!*/
	lmv = GSGL_TRUE;
	if (ind==0) {
		xx=p->base->Xchx;
		xy=p->base->Xchy;
		yx=p->base->Ychx;
		yy=p->base->Ychy;
	} else {
		xx=p->base->Xmkx;
		xy=p->base->Xmky;
		yx=p->base->Ymkx;
		yy=p->base->Ymky;
	}

	for (;;) {
		il = Is[j-1]; /*!!!*/
		il1 = il/1000;
		il2 = il-il1*1000;
		il1p = il1/100;
		if (il1p == 2) break;
		il1 = il1-il1p*100;
		il1x = il1/10;
		il1y = il1-il1x*10-2;
		if (lmv && il1p==0) {
			xt = *x;
			yt = *y;
			gsgl_move (xt,yt,p);
		}
		lmv = GSGL_FALSE;
		xt = *x + xx * il1x + xy * il1y;
		yt = *y + yx * il1x + yy * il1y;
		if (il1p==0) {
			if (idim==1) {
				gsgl_draw (xt,yt,p);
			} else if (idim==2) {
				gsgl_draw_text2d (xt,yt,p);
			} else if (idim==3) { 
				gsgl_draw_text3d (xt,yt,p);
			}
		}
		if (il1p==1) gsgl_move (xt,yt,p);
		il2p=il2/100;
		if (il2p == 2) break;
		il2=il2-il2p*100;
		il2x=il2/10;
		il2y=il2-il2x*10-2;
		xt = *x + xx * il2x + xy * il2y;
		yt = *y + yx * il2x + yy * il2y;
		if (il2p==0) {
			if (idim==1) {
				gsgl_draw (xt,yt,p);
			} else if (idim==2) { 
				gsgl_draw_text2d (xt,yt,p);
			} else if (idim==3) { 
				gsgl_draw_text3d (xt,yt,p);
			}
		}
		if (il2p==1) gsgl_move (xt,yt,p);
		j=j+1;
	}
	*x = *x + p->base->Xchsp;
	*y = *y + p->base->Ychsp;
}

/* set character attribute */
void gs_sfstch (float chh, float chw, float chsp, float angl, float tilt, gsglPage *p)
{
	float deg = 0.0174533;
	float a, t, b;
	float cost;
	float chx, chy;
	float ca, sa;
	float fact;

	a = deg*angl;
	t = deg*tilt;
	b = a+t;
	chx = 0.25 * chw;
	cost = cos(t);
	if (cost>=0.0 && cost< 0.1) cost= 0.1;
	if (cost<0.0 && cost>-0.1) cost=-0.1;
	chy = 0.166667 * chh / cost;
	
	ca = cos(a);
	sa = sin(a);
	p->base->Xchx = chx*ca;
	p->base->Ychx = chx*sa;
	p->base->Xchy = -chy*sin(b);
	p->base->Ychy = chy*cos(b);
	p->base->Xchsp = chsp*ca;
	p->base->Ychsp = chsp*sa;

	chy = 1.0 / cost;
	if (fabs(chh) < FLOAT_EPS) {
		fact = 1.0;
	} else {
		fact = 1.5 * chw / chh;
	}
	p->base->Tfly = chh;
	p->base->Tflx = p->base->Tfly  *fact;
	p->base->Tfxx = ca    *fact;
	p->base->Tfyx = sa    *fact;
	p->base->Tfxy = -sin(b)*chy;
	p->base->Tfyy = cos(b)*chy;
	p->base->Tfsp = (chsp-chh);
	p->base->Tfspx = ca*p->base->Tfsp;
	p->base->Tfspy = sa*p->base->Tfsp;
}

/* draw text */
void gs_sftext (float x, float y, unsigned int *iasc, int nchar, int idim, gsglPage *p)
{
	int fnt;
	int ins;
	int n;

	gsaf_inqfnt (&fnt);
	if (fnt >= 32) {
		if (fnt >= 32 && fnt <= 35) {
			if (fnt==32) p->base->Fntreal=2;
			if (fnt==33) p->base->Fntreal=3;
			if (fnt==34) p->base->Fntreal=4;
			if (fnt==35) p->base->Fntreal=5;
		} else if (fnt>=36 && fnt <= 39) {
			p->base->Fntreal=6;
		} else if (fnt>=40 && fnt <= 43) {
			p->base->Fntreal=0;
		} else if (fnt==44) {
			p->base->Fntreal=11;
		} else {
			p->base->Fntreal=0;
		}
	} else {
		p->base->Fntreal = fnt;
	}
	if (nchar != 0) {
		for (n=0; n < abs(nchar); n++) {
			ins = iasc[n];
			if (p->base->Fntreal == 0) {
				gs_sfchar (&x, &y, ins, 0, idim, p);
			} else {
				gs_grchar (&x, &y, ins, 0, idim, p);
			}
		}
	}
}

void gsgl_inq_fntreal (int* fntr, gsglPage *p)
{
	*fntr = p->base->Fntreal;
}

void gsgl_inq_font_scale_info (float *tfxx, float *tfxy, float *tfyx, float *tfyy,
			       float *tflx, float *tfly,
			       float *tfsp, float *tfspx, float *tfspy,
			       gsglPage *p)
{
	*tfxx = p->base->Tfxx;
	*tfxy = p->base->Tfxy; 
	*tfyx = p->base->Tfyx;
	*tfyy = p->base->Tfyy;
	*tflx = p->base->Tflx;
	*tfly = p->base->Tfly;
	*tfsp = p->base->Tfsp;
	*tfspx = p->base->Tfspx;
	*tfspy = p->base->Tfspy;
}

void gsaf_set_quit_callback (void (*callback)(void))
{
	Library->Callback = callback;
}

void gsaf_reset_quit_callback (void)
{
	Library->Callback = NULL;       
}

void (*gsaf_inq_quit_callback(void))(void)
{
	return Library->Callback;
}
