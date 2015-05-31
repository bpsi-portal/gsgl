#include "gsaf.h"
#include "gsgl.h"
#include "base.h"
#include "bsc1.h"
#include "bsc2.h"
#include "text.h"
#include "matrix.h"
#include "apl1.h"

/** move on gdefin coordinates */
void gsaf_move2d (float x1, float y1)
{
	if (!gsgl_inq_page_is_open ()) return;
	gsgl_move2d (x1, y1, gsgl_inq_page ());
}

/* inqpos in 2D space */
void gsaf_inqpos2d (float *x1, float *y1)
{
	if (!gsgl_inq_page_is_open ()) return;
	gsgl_inqpos2d (x1, y1, gsgl_inq_page ());
}

/* draw line on gdefin coordinates */
void gsaf_draw2d (float x, float y)
{
	/* introduced software clipping, which is copyed from bsc1.f of gsaf */
	float Pxs, Pxe, Pys, Pye, Gxs, Gxe, Gys, Gye;
	float x1, y1, x2, y2, xn, yn;
	float dx_clip, dy_clip;
	int jx1, jx2, jy1, jy2, jx, jy, ind, jxn, jyn;
	gsglboolean keep_xy1;

	if (!gsgl_inq_page_is_open ()) return;
	
	gsaf_inqgdefin (&Pxs, &Pxe, &Pys, &Pye, &Gxs, &Gxe, &Gys, &Gye);
	/*fprintf(stderr, "Gxs=%f, Gxe=%f, Gys=%f, Gye=%f\n", Gxs, Gxe, Gys, Gye);*/
	gsaf_inqpos2d (&x1, &y1);

	dx_clip = Gxe - Gxs;
	dy_clip = Gye - Gys;

	/* checking whether the line (x,y)-(x1,y1) crosses the clipping plane or not. */
	jx1=0;
	if((x1-Gxs)*dx_clip < 0.0) jx1=-1;
	if((Gxe-x1)*dx_clip < 0.0) jx1=+1;

	jy1=0;
	if((y1-Gys)*dy_clip < 0.0) jy1=-1;
	if((Gye-y1)*dy_clip < 0.0) jy1=+1;

	x2=x;
	y2=y;

	jx2=0;
	if((x2-Gxs)*dx_clip < 0.0) jx2=-1;
	if((Gxe-x2)*dx_clip < 0.0) jx2=+1;

	jy2=0;
	if((y2-Gys)*dy_clip < 0.0) jy2=-1;
	if((Gye-y2)*dy_clip < 0.0) jy2=+1;

	keep_xy1 = GSGL_TRUE;
	
/*#if 0*/
loop:
	if (jx1 != 0 || jy1 != 0 || jx2 != 0 || jy2 != 0) {
		if(jx1*jx2 == 1 || jy1*jy2 == 1) {
			/* the line (x,y)-(x1,y1) is out of the clipping planes */
			gsaf_move2d (x, y);
			return;
		} else {
			if(jx1 == 0 && jy1 == 0) {
				jx=jx2;
				jy=jy2;
				ind=2;
			} else {
				jx=jx1;
				jy=jy1;
				ind=1;
			}

			/*fprintf(stderr, "jx = %d, jy = %d, x2=%f, y2=%f\n", jx, jy, x2, y2);*/
			if(jy == 1) {
				xn=x1+(x2-x1)*(Gye-y1)/(y2-y1);
				yn=Gye;
                        } else if (jy == -1) {
				xn=x1+(x2-x1)*(Gys-y1)/(y2-y1);
				yn=Gys;
			} else if (jx == 1) {
				yn=y1+(y2-y1)*(Gxe-x1)/(x2-x1);
				xn=Gxe;
			} else if(jx == -1) {
				yn=y1+(y2-y1)*(Gxs-x1)/(x2-x1);
				xn=Gxs;
			} else {
				gsgl_warning ("gsaf_draw2d",
					      "Logical error in clip procedure.");
				return;
			}
			/*fprintf(stderr, "xn=%f, yn=%f\n", xn, yn);*/

			jxn=0;
			if((xn-Gxs)*dx_clip < 0.0) jxn=-1;
			if((Gxe-xn)*dx_clip < 0.0) jxn=+1;
			jyn=0;
			if((yn-Gys)*dy_clip < 0.0) jyn=-1;
			if((Gye-yn)*dy_clip < 0.0) jyn=+1;
			if(ind == 1) {
				x1=xn;
				y1=yn;
				jx1=jxn;
				jy1=jyn;
				keep_xy1 = GSGL_FALSE;
			} else {
				x2=xn;
				y2=yn;
				jx2=jxn;
				jy2=jyn;
			}
			goto loop;
		}
	}

/*if(lmv || !lkp1) {*/
	if(!keep_xy1) {
		gsaf_move2d(x1, y1);
	}
/*#endif*/
	gsgl_draw2d(x2, y2, gsgl_inq_page ());
	gsaf_move2d(x, y);
}

/* poly in 2D space */
void gsaf_poly2d (float *xn, float *yn, int n)
{
	if (!gsgl_inq_page_is_open ()) return;
	gsgl_polygon2d (xn, yn, n, gsgl_inq_page ());
}

/* rgbtrg in 2D space */
void gsaf_rgbtrg2d (float *xn, float *yn, float *r, float *g, float *b)
{
	if (!gsgl_inq_page_is_open ()) return;

	gsgl_rgbtriangle2d (xn, yn, r, g, b, gsgl_inq_page ());
	gsaf_setrgb (r[2], g[2], b[2]);
}

/* line in 2D space */
void gsaf_line2d (float x1, float y1, float x2, float y2)
{
	if (!gsgl_inq_page_is_open ()) return;
	gsaf_move2d (x1, y1);
	gsaf_draw2d (x2, y2);
}

/* lines in 2D space */
void gsaf_lines2d (float *x, float *y, int n)
{
	if (!gsgl_inq_page_is_open ()) return;
	gsgl_lines2d (x, y, n, gsgl_inq_page ());
}

/* linesc in 2D space */
void gsaf_linesc2d (float *x, float *y, int n)
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

	gsgl_lines2d (x1, y1, n+1, gsgl_inq_page ());
}


/* mark in 2D space */
void gsaf_mark2d (float x, float y)
{
	float x2, y2;

	if (!gsgl_inq_page_is_open ()) return;
	gsglproject2d (x, y, &x2, &y2, gsgl_inq_page ());
	gsaf_mark (x2, y2);
}

/* draw text in 2d space */
void gsaf_text2d (char *ktext)
{
	float x2d, y2d;
	float x1, y1;
	float xworld, yworld;
	int ifnt;
	float ws;
	gsglPage *p;

	if (!gsgl_inq_page_is_open ()) return;
	p = gsgl_inq_page ();

	gsgl_separate_objects (p);
	gsaf_inqpos2d (&x2d, &y2d);
	gsglproject2d (x2d, y2d, &xworld, &yworld, p);

	if (!gsgl_inq_hold_text_position (p))
		gsaf_move (0.0, 0.0);
	
	gsgl_set_text2d_matrix (xworld, yworld, p);
     
	gsaf_inqfnt (&ifnt);
	if (ifnt >= 2 && ifnt <= 19) {
		gsaf_inqlnw (&ws);
		setlnw_text (p);
	}
	gsaf_inqpos (&x1, &y1);
	gsgltext (ktext, strlen(ktext), &x1, &y1, 2, p);

	if (ifnt >= 2 && ifnt < 19) {
		gsaf_setlnw (ws);
	}
	gsglprojecttext2d (x1, y1, &xworld, &yworld, p);
	gsglunproject2d (xworld, yworld, &x2d, &y2d, p);
	gsaf_move2d (x2d, y2d);

	gsgl_separate_objects (p);
}
