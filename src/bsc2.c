#include "gsaf.h"
#include "gsgl.h"
#include "bsc1.h"
#include "base.h"
#include "bsc2.h"

void gsgl_set_rs (float rs, gsglPage *page)
{
	page->rgb.r = rs;
}

void gsgl_set_ws (float ws, gsglPage *page)
{
	page->Ws = ws;
}

/* draw a line */
void gsaf_line1 (float xs, float ys, float xe, float ye)
{
	if (!gsgl_inq_page_is_open ()) return;
	gsaf_move (xs, ys);
	gsaf_draw (xe, ye);
}

/* draw lines */
void gsaf_lines (float *x, float *y, int n)
{
	if (!gsgl_inq_page_is_open ()) return;
	
	gsgl_lines (x, y, n, gsgl_inq_page ());
	gsaf_move (x[n-1], y[n-1]);
}

/* draw lines */
void gsaf_linesc (float *x, float *y, int n)
{
	int i;
	float *x1;
 	float *y1;

	if (!gsgl_inq_page_is_open ()) return;
	
	x1 = (float *) gsgl_malloc(sizeof(float)*(n+1));
 	y1 = (float *) gsgl_malloc(sizeof(float)*(n+1)); 

	for (i=0; i<n; i++) {
	  x1[i] = x[i];
	  y1[i] = y[i];
	}
	x1[n] = x[0];
	y1[n] = y[0];

	gsgl_lines (x1, y1, n+1, gsgl_inq_page ());
	gsaf_move (x[0], y[0]);
}

/* fill polygon */
void gsaf_poly (float *x, float *y, int n)
{
	if (!gsgl_inq_page_is_open ()) return;
	
	gsgl_polygon (x, y, n, gsgl_inq_page ());
	gsaf_move (x[n-1], y[n-1]);
}

/* rgb triangle */
void gsaf_rgbtrg (float xn[3], float yn[3], float r[3], float g[3], float b[3])
{
	if (!gsgl_inq_page_is_open ()) return;
	
	gsgl_rgbtriangle (xn, yn, r, g, b, gsgl_inq_page ());
	gsaf_setrgb (r[2], g[2], b[2]);

	gsaf_move (xn[2], yn[2]);
}

/* set line width */
void gsaf_setlnw (float w)
{
	int ilns,ibls,icls;
	gsglPage *p;

	if (!gsgl_inq_page_is_open ()) return;
	p = gsgl_inq_page ();

	if (w < 0.0) {
		gsaf_inqlin (&ilns, &ibls, &icls);
		gsaf_setlin (-1, ibls, -1);
	} else {
		gsgl_linewidth (w, p);
	}
}

/* inquire line width */
void gsaf_inqlnw (float *w)
{
	gsglPage *p;

	if (!gsgl_inq_page_is_open ()) return;
	p = gsgl_inq_page ();
	*w = p->Ws;
}

/* set rgb color */
/*    In the case setlin's setting is valid, inqrgb returns r < 0.
 *    And, if you calll setrgb with r < 0, former setlin's setting becomes valid.
 *    setlnw/inqlnw's w < 0 is likewise.
 */
void gsaf_setrgb (float r, float g, float b)
{
	int ilns,ibls,icls;
	gsglPage *p;

	if (!gsgl_inq_page_is_open ()) return;
	p = gsgl_inq_page ();

	if (r < 0.0) {
		gsaf_inqlin (&ilns, &ibls, &icls);
		gsaf_setlin (-1, -1, icls);
		return;
	}
	
	gsgl_rgb (r, g, b, p);
}

/* inquire rgb color */
void gsaf_inqrgb (float *r, float *g, float *b)
{
	gsglPage *p;

	if (!gsgl_inq_page_is_open ()) return;
	p = gsgl_inq_page ();

	*r = p->rgb.r;
	*g = p->rgb.g;
	*b = p->rgb.b;
}
