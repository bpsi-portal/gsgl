#include "gsaf.h"
#include "base.h"
#include "utils.h"
#include "gsgl.h"
#include "bsc1.h"
#include "bsc3d.h"
#include "cont1.h"
#include "cont2.h"
#include "guiitems.h"

#define GSOPEN_F77 F77_FUNC(gsopen,GSOPEN)
void GSOPEN_F77 ()
{
	gsaf_gsopen ();
}

#define GSOPENRANK_F77 F77_FUNC(gsopenrank,GSOPENRANK)
void GSOPENRANK_F77 (int *i)
{
	gsaf_gsopen_rank (*i);
}

#define GSCLOS_F77 F77_FUNC(gsclos,GSCLOS)
void GSCLOS_F77 ()
{
	gsaf_gsclos ();
}

#define GSSIZE_F77 F77_FUNC(gssize,GSSIZE)
void GSSIZE_F77 (float *x, float *y)
{
	gsaf_gssize (*x, *y);
}

#define GSTITL_F77 F77_FUNC(gstitl,GSTITL)
void GSTITL_F77 (char *title)
{
	gsaf_gstitl (title);
}

/*void  gsoptn (KOPTDL,IOPT);*/

#define PAGES_F77 F77_FUNC(pages,PAGES)
void PAGES_F77 ()
{
	gsaf_pages ();
}

#define PAGEE_F77 F77_FUNC(pagee,PAGEE)
void PAGEE_F77 ()
{
	gsaf_pagee ();
}

#define PAGEY_F77 F77_FUNC(pagey,PAGEY)
void PAGEY_F77 ()
{
	gsaf_pagey ();
}

#define PAGEN_F77 F77_FUNC(pagen,PAGEN)
void PAGEN_F77 ()
{
	gsaf_pagen ();
}

#define SETVEW_F77 F77_FUNC(setvew,SETVEW)
void SETVEW_F77 (float *pxmin, float *pxmax, float *pymin, float *pymax, 
		 float *xmin, float *xmax, float *ymin, float *ymax)
{
	gsaf_setvew (*pxmin, *pxmax, *pymin, *pymax, 
		     *xmin, *xmax, *ymin, *ymax);
}

#define OFFVEW_F77 F77_FUNC(offvew,OFFVEW)
void OFFVEW_F77 ()
{
	gsaf_offvew ();
}

#define INQVEW_F77 F77_FUNC(inqvew,INQVEW)
void INQVEW_F77 (float *pxmin, float *pxmax, float *pymin, float *pymax, 
		 float *xmin, float *xmax, float *ymin, float *ymax)
{
	gsaf_inqvew (pxmin, pxmax, pymin, pymax, 
		     xmin, xmax, ymin, ymax);
}

#define GSGLPROJIMGTRANSLATE_F77 F77_FUNC(gsglprojimgtranslate,GSGLPROJIMGTRANSLATE)
void GSGLPROJIMGTRANSLATE_F77 (float *dx, float *dy)
{
	gsgl_projimgtranslate (*dx, *dy);
}

#define GSGLPROJIMGSCALE_F77 F77_FUNC(gsglprojimgscale,GSGLPROJIMGSCALE)
void GSGLPROJIMGSCALE_F77 (float *xratio, float *yratio)
{
	gsgl_projimgscale (*xratio, *yratio);
}

#define GSGLPROJIMGROTATE_F77 F77_FUNC(gsglprojimgrotate,GSGLPROJIMGROTATE)
void GSGLPROJIMGROTATE_F77 (float *degree) /* angle in degree */
{
	gsgl_projimgrotate (*degree); /* angle in degree */
}

#define GSGLPROJIMGIDENTITY_F77 F77_FUNC(gsglprojimgidentity,GSGLPROJIMGIDENTITY)
void GSGLPROJIMGIDENTITY_F77 ()
{
	gsgl_projimgidentity ();
}

#define SETCLP_F77 F77_FUNC(setclp,SETCLP)
void SETCLP_F77 (float *xmin, float *xmax, float *ymin, float *ymax)
{
	gsaf_setclp (*xmin, *xmax, *ymin, *ymax);
}

#define OFFCLP_F77 F77_FUNC(offclp,OFFCLP)
void OFFCLP_F77 ()
{
	gsaf_offclp ();
}

#define INQCLP_F77 F77_FUNC(inqclp,INQCLP)
void INQCLP_F77 (float *xmin, float *xmax, float *ymin, float *ymax)
{
	gsaf_inqclp (xmin, xmax, ymin, ymax);
}

#define MOVE_F77 F77_FUNC(move,MOVE)
void MOVE_F77 (float *x, float *y)
{
	gsaf_move (*x, *y);
}

#define DRAW_F77 F77_FUNC(draw,DRAW)
void DRAW_F77 (float *x, float *y)
{
	gsaf_draw (*x, *y);
}

#define MARK_F77 F77_FUNC(mark,MARK)
void MARK_F77 (float *x, float *y)
{
	gsaf_mark (*x, *y);
}

#define INQPOS_F77 F77_FUNC(inqpos,INQPOS)
void INQPOS_F77 (float *x, float *y)
{
	gsaf_inqpos (x, y);
}

#define TEXT_F77 F77_FUNC(text,TEXT)
void TEXT_F77 (char *text1, int *nchar)
{
	char *str;
	int i;

	str = (char *) gsgl_malloc_atomic (sizeof(char)*((*nchar)+1));
	for (i=0; i<*nchar; i++)
		str[i] = text1[i];
	str[*nchar] = '\0';

	gsaf_text (str);
}

#define INQTEXTBBOX_F77 F77_FUNC(inqtextbbox,INQTEXTBBOX)
void INQTEXTBBOX_F77 (char *text, int *nchar, float *size, float *height, float *depth)
{
	char *str;
	int i;

	str = (char *) gsgl_malloc_atomic (sizeof(char)*((*nchar)+1));
	for (i=0; i<*nchar; i++)
		str[i] = text[i];
	str[*nchar] = '\0';

	gsaf_inqtextbbox (str, size, height, depth);
}

#define INQTSZ_F77 F77_FUNC(inqtsz,INQTSZ)
void INQTSZ_F77 (char *text, int *nchar, float *size)
{
	char *str;
	int i;

	str = (char *) gsgl_malloc_atomic (sizeof(char)*((*nchar)+1));
	for (i=0; i<*nchar; i++)
		str[i] = text[i];
	str[*nchar] = '\0';

	gsaf_inqtsz (str, size);
}

#define SETLIN_F77 F77_FUNC(setlin,SETLIN)
void SETLIN_F77 (int *iln, int *ibl, int *icl)
{
	gsaf_setlin (*iln, *ibl, *icl);
}

#define INQLIN_F77 F77_FUNC(inqlin,INQLIN)
void INQLIN_F77 (int *iln, int *ibl, int *icl)
{
	gsaf_inqlin (iln, ibl, icl);
}

#define SETMRK_F77 F77_FUNC(setmrk,SETMRK)
void SETMRK_F77 (int *imrk, float *hmrk, float *wmrk, float *angl, float *tilt)
{
	gsaf_setmrk (*imrk, *hmrk, *wmrk, *angl, *tilt);
}

#define SETMKS_F77 F77_FUNC(setmks,SETMKS)
void SETMKS_F77 (int *imrk, float *smrk)
{
	gsaf_setmks (*imrk, *smrk);
}

#define INQMRK_F77 F77_FUNC(inqmrk,INQMRK)
void INQMRK_F77 (int *imrk, float *hmrk, float *wmrk, float *angl, float *tilt)
{
	gsaf_inqmrk (imrk, hmrk, wmrk, angl, tilt);
}

#define SETCHR_F77 F77_FUNC(setchr,SETCHR)
void SETCHR_F77 (float *chh, float *chw, float *chsp, float *angl, float *tilt)
{
	gsaf_setchr (*chh, *chw, *chsp, *angl, *tilt);
}

#define SETCHS_F77 F77_FUNC(setchs,SETCHS)
void SETCHS_F77 (float *chh, float *angl)
{
	gsaf_setchs (*chh, *angl);
}

#define SETFNT_F77 F77_FUNC(setfnt,SETFNT)
void SETFNT_F77 (int *fnt)
{
	gsaf_setfnt (*fnt);
}

#define INQCHR_F77 F77_FUNC(inqchr,INQCHR)
void INQCHR_F77 (float *chh, float *chw, float *chsp, float *angl, float *tilt)
{
	gsaf_inqchr (chh, chw, chsp, angl, tilt);
}

#define INQFNT_F77 F77_FUNC(inqfnt,INQFNT)
void INQFNT_F77 (int *fnt)
{
	gsaf_inqfnt (fnt);
}

#define ERAS_F77 F77_FUNC(eras,ERAS)
void ERAS_F77 ()
{
	gsaf_eras ();
}

#define BELL_F77 F77_FUNC(bell,BELL)
void BELL_F77 ()
{
	gsaf_bell ();
}

#define GUDSTR_F77 F77_FUNC(gudstr,GUDSTR)
void GUDSTR_F77 (char *kin, int *nin, char *kout, int *nout)
{
	gsaf_f77_gudstr (kin, *nin, kout, nout);
}

#define GUCPTL_F77 F77_FUNC(gucptl,GUCPTL)
void GUCPTL_F77 (char *kid)
{
	gsaf_gucptl (kid);
}

#define GUCLIP_F77 F77_FUNC(guclip,GUCLIP)
float GUCLIP_F77 (double *d)
{
	return gsaf_guclip (*d);
}

#define NGULEN_F77 F77_FUNC(ngulen,NGULEN)
int NGULEN_F77 (float *gstep)
{
	return gsaf_ngulen (*gstep);
}

/*#define GSERROR_PAGES_F77 F77_FUNC_(gserror_pages,GSERROR_PAGES)
  void GSERROR_PAGES_F77 (int *error)
  {
  gsaf_gserror_pages (error);
  }*/

#define LINE1_F77 F77_FUNC(line1,LINE1)
void LINE1_F77 (float *xs, float *ys, float *xe, float *ye)
{
	gsaf_line1 (*xs, *ys, *xe, *ye);
}

#define LINES_F77 F77_FUNC(lines,LINES)
void LINES_F77 (float *x, float *y, int *n)
{
	gsaf_lines (x, y, *n);
}

#define LINESC_F77 F77_FUNC(linesc,LINESC)
void LINESC_F77 (float *x, float *y, int *n)
{
	gsaf_linesc (x, y, *n);
}

#define POLY_F77 F77_FUNC(poly,POLY)
void POLY_F77 (float *x, float *y, int *n)
{
	gsaf_poly (x, y, *n);
}

#define RGBTRG_F77 F77_FUNC(rgbtrg,RGBTRG)
void RGBTRG_F77 (float *xn, float *yn, float *r, float *g, float *b)
{
	gsaf_rgbtrg (xn, yn, r, g, b);
}

#define SETLNW_F77 F77_FUNC(setlnw,SETLNW)
void SETLNW_F77 (float *w)
{
	gsaf_setlnw (*w);
}

#define INQLNW_F77 F77_FUNC(inqlnw,INQLNW)
void INQLNW_F77 (float *w)
{
	gsaf_inqlnw (w);
}

#define SETRGB_F77 F77_FUNC(setrgb,SETRGB)
void SETRGB_F77 (float *r, float *g, float *b)
{
	gsaf_setrgb (*r, *g, *b);
}

#define INQRGB_F77 F77_FUNC(inqrgb,INQRGB)
void INQRGB_F77 (float *r, float *g, float *b)
{
	gsaf_inqrgb (r, g, b);
}

#define TEXTX_F77 F77_FUNC(textx,TEXTX)
void TEXTX_F77 (char *ktextx)
{
	gsaf_textx (ktextx);
}

#define GNUMBI_F77 F77_FUNC(gnumbi,GNUMBI)
void GNUMBI_F77 (float *x, float *y, int *i, int *ijust)
{
	gsaf_gnumbi (*x, *y, *i, *ijust);
}

#define GNUMBD_F77 F77_FUNC(gnumbd,GNUMBD)
void GNUMBD_F77 (float *x, float *y, double *d, int *ind, int *ijust)
{
	gsaf_gnumbd (*x, *y, *d, *ind, *ijust);
}

#define GNUMBR_F77 F77_FUNC(gnumbr,GNUMBR)
void GNUMBR_F77 (float *x, float *y, float *r, int *ind, int *ijust)
{
	gsaf_gnumbr (*x, *y, *r, *ind, *ijust);
}

#define GNUMBP_F77 F77_FUNC(gnumbp,GNUMBP)
void GNUMBP_F77 (float *x, float *y, int *i, int *j, int *ijust)
{
	gsaf_gnumbp (*x, *y, *i, *j, *ijust);
}

#define DRAWPT_F77 F77_FUNC(drawpt,DRAWPT)
void DRAWPT_F77 (float *x, float *y)
{
	gsaf_drawpt (*x, *y);
}

#define MOVEPT_F77 F77_FUNC(movept,MOVEPT)
void MOVEPT_F77 (float *x, float *y, int *ipat)
{
	gsaf_movept (*x, *y, *ipat);
}

#define SETLPT_F77 F77_FUNC(setlpt,SETLPT)
void SETLPT_F77 (int *ipat, int *npat, float *pat)
{
	gsaf_setlpt (*ipat, *npat, pat);
}

#define GMNMX1_F77 F77_FUNC(gmnmx1,GMNMX1)
void GMNMX1_F77 (float *a, int *ns, int *ne, int *nstep, float *amin, float *amax)
{
	int ns1 = (*ns)-1, ne1 = (*ne)-1;
	
	gsaf_gmnmx1 (a, ns1, ne1, *nstep, amin, amax);
}

#define GMNMX2_F77 F77_FUNC(gmnmx2,GMNMX2)
void GMNMX2_F77 (float *a, int *nxa, 
		 int *nxs, int *nxe, int *nxstep,
		 int *nys, int *nye, int *nystep,
		 float *amin, float *amax)
{
	int nxs1 = (*nxs) - 1;
	int nxe1 = (*nxe) - 1;
	int nys1 = (*nys) - 1;
	int nye1 = (*nye) - 1;
	gsaf_gmnmx2 (a,*nxa,nxs1,nxe1,*nxstep,nys1,nye1,*nystep,amin,amax);
}

#define GQSCAL_F77 F77_FUNC(gqscal,GQSCAL)
void GQSCAL_F77 (float *amin, float *amax, float *gmin, float *gmax, float *scale)
{
	gsaf_gqscal (*amin, *amax, gmin, gmax, scale);
}

#define MOVE2D_F77 F77_FUNC(move2d,MOVE2D)
void MOVE2D_F77 (float *x1, float *y1)
{
	gsaf_move2d (*x1, *y1);
}

#define INQPOS2D_F77 F77_FUNC(inqpos2d,INQPOS2D)
void INQPOS2D_F77 (float *x1, float *y1)
{
	gsaf_inqpos2d (x1, y1);
}

#define DRAW2D_F77 F77_FUNC(draw2d,DRAW2D)
void DRAW2D_F77 (float *x1, float *y1)
{
	gsaf_draw2d (*x1, *y1);
}

#define POLY2D_F77 F77_FUNC(poly2d,POLY2D)
void POLY2D_F77 (float *xn, float *yn, int *n)
{
	gsaf_poly2d (xn, yn, *n);
}

#define RGBTRG2D_F77 F77_FUNC(rgbtrg2d,RGBTRG2D)
void RGBTRG2D_F77 (float *xn, float *yn, float *r, float *g, float *b)
{
	gsaf_rgbtrg2d (xn, yn, r, g, b);
}

#define LINE2D_F77 F77_FUNC(line2d,LINE2D)
void LINE2D_F77 (float *x1, float *y1, float *x2, float *y2)
{
	gsaf_line2d (*x1, *y1, *x2, *y2);
}

#define LINES2D_F77 F77_FUNC(lines2d,LINES2D)
void LINES2D_F77 (float *x, float *y, int *n)
{
	gsaf_lines2d (x, y, *n);
}

#define LINESC2D_F77 F77_FUNC(linesc2d,LINESC2D)
void LINESC2D_F77 (float *x, float *y, int *n)
{
	gsaf_linesc2d (x, y, *n);
}

#define MARK2D_F77 F77_FUNC(mark2d,MARK2D)
void MARK2D_F77 (float *x, float *y)
{
	gsaf_mark2d (*x, *y);
}

#define GDEFIN3D_F77 F77_FUNC(gdefin3d,GDEFIN3D)
void GDEFIN3D_F77 (float *xl1, float *yl1, float *zl1,
		   float *gxmin1, float *gxmax1, 
		   float *gymin1, float *gymax1, 
		   float *gzmin1, float *gzmax1)
{
	gsaf_gdefin3d (*xl1, *yl1, *zl1, *gxmin1, *gxmax1, *gymin1, *gymax1, *gzmin1, *gzmax1);
}

#define SETPROJECTIONTYPE_F77 F77_FUNC(setprojectiontype,SETPROJECTIONTYPE)
void SETPROJECTIONTYPE_F77 (int *iproj)
{
	gsaf_setprojectiontype (*iproj);
}

#define SET3DCOORD_CAUT1_F77 F77_FUNC_(set3dcoord_caut1,SET3DCOORD_CAUT1)
void SET3DCOORD_CAUT1_F77 (float *ox1, float *oy1, float *oz1, float *depth, 
			   float *vx, float *vy, float *vz)
{
	gsaf_set3dcoord_caut1 (*ox1, *oy1, *oz1, *depth, vx, vy, vz);
}

#define SET3DCOORD_CAUT2_F77 F77_FUNC_(set3dcoord_caut2,SET3DCOORD_CAUT2)
void SET3DCOORD_CAUT2_F77 (float *wx, float *wy, float *depth, float *x, float *y, float *z,
			   float *vx, float *vy, float *vz)
{
	gsaf_set3dcoord_caut2 (*wx, *wy, *depth, *x, *y, *z, vx, vy, vz);
}

#define SET3DCOORD_POLAR1_F77 F77_FUNC_(set3dcoord_polar1,SET3DCOORD_POLAR1)
void SET3DCOORD_POLAR1_F77 (float *ox1, float *oy1, float *oz1, 
			    float *radius1, float *theta1, float *phi1)
{
	gsaf_set3dcoord_polar1 (*ox1, *oy1, *oz1, *radius1, *theta1, *phi1);
}

#define SET3DCOORD_POLAR2_F77 F77_FUNC_(set3dcoord_polar2,SET3DCOORD_POLAR2)
void SET3DCOORD_POLAR2_F77 (float *wx, float *wy, float *depth, float *x, float *y, float *z,
			    float *theta, float *phi)
{
	gsaf_set3dcoord_polar2 (*wx, *wy, *depth, *x, *y, *z, *theta, *phi);
}

#define GSGLSETEYEPOS_F77 F77_FUNC(gsglseteyepos,GSGLSETEYEPOS)
void GSGLSETEYEPOS_F77 (float *eye)
{
	gsgl_seteyepos (*eye);
}

#define GVIEW3D_F77 F77_FUNC(gview3d,GVIEW3D)
void GVIEW3D_F77 (float *phi1, float *theta1, float *radius1,
		  float *ox1, float *oy1, float *oz1)
{
	gsaf_gview3d (*phi1, *theta1, *radius1, *ox1, *oy1, *oz1);
}

#define MOVE3D_F77 F77_FUNC(move3d,MOVE3D)
void MOVE3D_F77 (float *x1, float *y1, float *z1)
{
	gsaf_move3d (*x1, *y1, *z1);
}

#define INQPOS3D_F77 F77_FUNC(inqpos3d,INQPOS3D)
void INQPOS3D_F77 (float *x1, float *y1, float *z1)
{
	gsaf_inqpos3d (x1, y1, z1);
}

#define DRAW3D_F77 F77_FUNC(draw3d,DRAW3D)
void DRAW3D_F77 (float *x1, float *y1, float *z1)
{
	gsaf_draw3d (*x1, *y1, *z1);
}

#define DRAW3D_WITH_NORMAL_F77 F77_FUNC_(draw3d_with_normal,DRAW3D_WITH_NORMAL)
void DRAW3D_WITH_NORMAL_F77 (float *x1, float *y1, float *z1, float *vx, float *vy, float *vz)
{
	gsaf_draw3d_with_normal (*x1, * y1, *z1, *vx, *vy, *vz);
}

#define DRAW3D_WITH_NORMALS_F77 F77_FUNC_(draw3d_with_normals,DRAW3D_WITH_NORMALS)
void DRAW3D_WITH_NORMALS_F77 (float *x1, float *y1, float *z1, float *vx, float *vy, float *vz)
{
	gsaf_draw3d_with_normals (*x1, *y1, *z1, vx, vy, vz);
}

#define POLY3D_F77 F77_FUNC(poly3d,POLY3D)
void POLY3D_F77 (float *xn, float *yn, float *zn, int *n)
{
	gsaf_poly3d (xn, yn, zn, *n);
}

#define POLY3D_WITH_NORMAL_F77 F77_FUNC_(poly3d_with_normal,POLY3D_WITH_NORMAL)
void POLY3D_WITH_NORMAL_F77 (float *xn, float *yn, float *zn, 
			     float *vx1, float *vy1, float *vz1, int *n)
{
	gsaf_poly3d_with_normal (xn, yn, zn, *vx1, *vy1, *vz1, *n);
}

#define POLY3D_WITH_NORMALS_F77 F77_FUNC_(poly3d_with_normals,POLY3D_WITH_NORMALS)
void POLY3D_WITH_NORMALS_F77 (float *xn, float *yn, float *zn, float *vx, float *vy, float *vz,
			      int *n)
{
	gsaf_poly3d_with_normals (xn, yn, zn, vx, vy, vz, *n);
}

#define RGBTRG3D_F77 F77_FUNC(rgbtrg3d,RGBTRG3D)
void RGBTRG3D_F77 (float *xn, float *yn, float *zn, float *r, float *g, float *b)
{
	gsaf_rgbtrg3d (xn, yn, zn, r, g, b);
}

#define RGBTRG3D_WITH_NORMAL_F77 F77_FUNC_(rgbtrg3d_with_normal,RGBTRG3D_WITH_NORMAL)
void RGBTRG3D_WITH_NORMAL_F77 (float *xn, float *yn, float *zn, float *r, float *g, float *b, 
			       float *vx, float *vy, float *vz)
{
	gsaf_rgbtrg3d_with_normal (xn, yn, zn, r, g, b, *vx, *vy, *vz);
}

#define RGBTRG_WITH_NORMALS_F77 F77_FUNC_(rgbtrg_with_normals,RGBTRG_WITH_NORMALS)
void RGBTRG_WITH_NORMALS_F77 (float *xn, float *yn, float *zn, float *r, float *g, float *b,
			      float *vx, float *vy, float *vz)
{
	gsaf_rgbtrg3d_with_normals (xn, yn, zn, r, g, b, vx, vy, vz);
}

#define LINE3D_F77 F77_FUNC(line3d,LINE3D)
void LINE3D_F77 (float *x1, float *y1, float *z1, float *x2, float *y2, float *z2)
{
	gsaf_line3d (*x1, *y1, *z1, *x2, *y2, *z2);
}

#define LINE3D_WITH_NORMAL_F77 F77_FUNC_(line3d_with_normal,LINE3D_WITH_NORMAL)
void LINE3D_WITH_NORMAL_F77 (float *x1, float *y1, float *z1, float *x2, float *y2, float *z2,
			     float *vx, float *vy, float *vz)
{
	gsaf_line3d_with_normal (*x1, *y1, *z1, *x2, *y2, *z2, *vx, *vy, *vz);
}

#define LINE3D_WITH_NORMALS_F77 F77_FUNC_(line3d_with_normals,LINE3D_WITH_NORMALS)
void LINE3D_WITH_NORMALS_F77 (float *x1, float *y1, float *z1, float *x2, float *y2, float *z2, 
			      float *vx, float *vy, float *vz)
{
	gsaf_line3d_with_normals (*x1, *y1, *z1, *x2, *y2, *z2, vx, vy, vz);
}

#define LINES3D_F77 F77_FUNC(lines3d,LINES3D)
void LINES3D_F77 (float *x, float *y, float *z, int *n)
{
	gsaf_lines3d (x, y, z, *n);
}

#define LINES3D_WITH_NORMAL_F77 F77_FUNC_(lines3d_with_normal,LINES3D_WITH_NORMAL)
void LINES3D_WITH_NORMAL_F77 (float *x, float *y, float *z, 
			      float *vx, float *vy, float *vz, int *n)
{
	gsaf_lines3d_with_normal (x, y, z, *vx, *vy, *vz, *n);
}

#define LINES3D_WITH_NORMALS_F77 F77_FUNC_(lines3d_with_normals,LINES3D_WITH_NORMALS)
void LINES3D_WITH_NORMALS_F77 (float *x, float *y, float *z, 
			       float *vx, float *vy, float *vz, int *n)
{
	gsaf_lines3d_with_normals (x, y, z, vx, vy, vz, *n);
}

#define GTTTA_F77 F77_FUNC(gttta,GTTTA)
void GTTTA_F77 (float *x, float *y, float *z, float *w1, float *w2)
{
	gsaf_gttta (*x, *y, *z, w1, w2);
}

#define GTTTB_F77 F77_FUNC(gtttb,GTTTB)
void GTTTB_F77 (float *x, float *y, float *z, float *w1, float *w2)
{
	gsaf_gtttb (*x, *y, *z, w1, w2);
}

#define TEXTX2D_F77 F77_FUNC(textx2d,TEXTX2D)
void TEXTX2D_F77 (char *ktextx)
{
	gsaf_textx2d (ktextx);
}

#define TEXTX3D_F77 F77_FUNC(textx3d,TEXTX3D)
void TEXTX3D_F77 (char *ktextx)
{
	gsaf_textx3d (ktextx);
}

#define GTEXT_F77 F77_FUNC(gtext,GTEXTD)
void GTEXT_F77 (float *x, float *y, char *ktext, int *nchar, int *ijust)
{
	char *str;
	int i;

	str = (char *) gsgl_malloc_atomic (sizeof(char)*((*nchar)+1));
	for (i=0; i<*nchar; i++)
		str[i] = ktext[i];
	str[*nchar] = '\0';

	gsaf_gtext (*x, *y, str, *ijust);
}

#define GTEXT2D_F77 F77_FUNC(gtext2d,GTEXT2D)
void GTEXT2D_F77 (float *x, float *y, char *ktext, int *nchar, int *ijust)
{
	char *str;
	int i;

	str = (char *) gsgl_malloc_atomic (sizeof(char)*((*nchar)+1));
	for (i=0; i<*nchar; i++)
		str[i] = ktext[i];
	str[*nchar] = '\0';

	gsaf_gtext2d (*x, *y, str, *ijust);
}

#define GTEXT3D_F77 F77_FUNC(gtext3d,GTEXT3D)
void GTEXT3D_F77 (float *x, float *y, float *z, char *ktext, int *nchar, int *ijust)
{
	char *str;
	int i;

	str = (char *) gsgl_malloc_atomic (sizeof(char)*((*nchar)+1));
	for (i=0; i<*nchar; i++)
		str[i] = ktext[i];
	str[*nchar] = '\0';

	gsaf_gtext3d (*x, *y, *z, str, *ijust);
}

#define GTEXTX_F77 F77_FUNC(gtextx,GTEXTX)
void GTEXTX_F77 (float *x, float *y, char *ktext, int *ijust)
{
	gsaf_gtextx (*x, *y, ktext, *ijust);
}

#define GTEXTX2D_F77 F77_FUNC(gtextx2d,GTEXTX2D)
void GTEXTX2D_F77 (float *x, float *y, char *ktextx, int *ijust)
{
	gsaf_gtextx2d (*x, *y, ktextx, *ijust);
}

#define GTEXTX3D_F77 F77_FUNC(gtextx3d,GTEXTX3D)
void GTEXTX3D_F77 (float *x, float *y, float *z, char *ktextx, int *ijust)
{
	gsaf_gtextx3d (*x, *y, *z, ktextx, *ijust);
}

#define GNUMBI2D_F77 F77_FUNC(gnumbi2d,GNUMBI2D)
void GNUMBI2D_F77 (float *x, float *y, int *i, int *ijust)
{
	gsaf_gnumbi2d (*x, *y, *i, *ijust);
}

#define GNUMBI3D_F77 F77_FUNC(gnumbi3d,GNUMBI3D)
void GNUMBI3D_F77 (float *x, float *y, float *z, int *i, int *ijust)
{
	gsaf_gnumbi3d (*x, *y, *z, *i, *ijust);
}

#define GNUMBD2D_F77 F77_FUNC(gnumbd2d,GNUMBD2D)
void GNUMBD2D_F77 (float *x, float *y, double *d, int *ind, int *ijust)
{
	gsaf_gnumbd2d (*x, *y, *d, *ind, *ijust);
}

#define GNUMBD3D_F77 F77_FUNC(gnumbd3d,GNUMBD3D)
void GNUMBD3D_F77 (float *x, float *y, float *z, double *d, int *ind, int *ijust)
{
	gsaf_gnumbd3d (*x, *y, *z, *d, *ind, *ijust);
}

#define GNUMBR2D_F77 F77_FUNC(gnumbr2d,GNUMBR2D)
void GNUMBR2D_F77 (float *x, float *y, float *r, int *ind, int *ijust)
{
	gsaf_gnumbr2d (*x, *y, *r, *ind, *ijust);
}

#define GNUMBR3D_F77 F77_FUNC(gnumbr3d,GNUMBR3D)
void GNUMBR3D_F77 (float *x, float *y, float *z, float *r, int *ind, int *ijust)
{
	gsaf_gnumbr3d (*x, *y, *z, *r, *ind, *ijust);
}

#define GNUMBP2D_F77 F77_FUNC(gnumbp2d,GNUMBP2D)
void GNUMBP2D_F77 (float *x, float *y, int *i, int *j, int *ijust)
{
	gsaf_gnumbp2d (*x, *y, *i, *j, *ijust);
}

#define GNUMBP3D_F77 F77_FUNC(gnumbp3d,GNUMBP3D)
void GNUMBP3D_F77 (float *x, float *y, float *z, int *i, int *j, int *ijust)
{
	gsaf_gnumbp3d (*x, *y, *z, *i, *j, *ijust);
}

#define DRAWPT2D_F77 F77_FUNC(drawpt2d,DRAWPT2D)
void DRAWPT2D_F77 (float *x, float *y)
{
	gsaf_drawpt2d (*x, *y);
}

#define DRAWPT3D_F77 F77_FUNC(drawpt3d,DRAWPT3D)
void DRAWPT3D_F77 (float *x, float *y, float *z)
{
	gsaf_drawpt3d (*x, *y, *z);
}

#define MOVEPT2D_F77 F77_FUNC(movept2d,MOVEPT2D)
void MOVEPT2D_F77 (float *x, float *y, int *ipat)
{
	gsaf_movept2d (*x, *y, *ipat);
}

#define MOVEPT3D_F77 F77_FUNC(movept3d,MOVEPT3D)
void MOVEPT3D_F77 (float *x, float *y, float *z, int *ipat)
{
	gsaf_movept3d (*x, *y, *z, *ipat);
}

#define GAXIS3D_F77 F77_FUNC(gaxis3d,GAXIS3D)
void GAXIS3D_F77 (int *iaxis)
{
	gsaf_gaxis3d (*iaxis);
}

#define GDRAWBACK3D_F77 F77_FUNC(gdrawback3d,GDRAWBACK3D)
void GDRAWBACK3D_F77 (float *r, float *g, float *b)
{
	gsaf_gdrawback3d (*r, *g, *b);
}

#define GPLOTP3D_F77 F77_FUNC(gplotp3d,GPLOTP3D)
void GPLOTP3D_F77 (float *gx, float *gy, float *gz, int *nsrt, int *nend, int *nstep, 
		   int *imark, int *istep, int *ipat)
{
	int ns = (*nsrt) - 1;
	int ne = (*nend) - 1;
	gsaf_gplotp3d (gx, gy, gz, ns, ne, *nstep, *imark, *istep, *ipat);
}

#define GSCALE3DX_F77 F77_FUNC(gscale3dx,GSCALE3DX)
void GSCALE3DX_F77 (float *xorg, float *xstep, float *scl, int *ipos)
{
	gsaf_gscale3dx (*xorg, *xstep, *scl, *ipos);
}

#define GSCALE3DY_F77 F77_FUNC(gscale3dy,GSCALE3DY)
void GSCALE3DY_F77 (float *yorg, float *ystep, float *scl, int *ipos)
{
	gsaf_gscale3dy (*yorg, *ystep, *scl, *ipos);
}

#define GSCALE3DZ_F77 F77_FUNC(gscale3dz,GSCALE3DZ)
void GSCALE3DZ_F77 (float *zorg, float *zstep, float *scl, int *ipos)
{
	gsaf_gscale3dz (*zorg, *zstep, *scl, *ipos);
}

#define GVALUE3DX_F77 F77_FUNC(gvalue3dx,GVALUE3DX)
void GVALUE3DX_F77 (float *xorg, float *dpx, int *ipos, int *ind)
{
	gsaf_gvalue3dx (*xorg, *dpx, *ipos, *ind);
}

#define GVALUE3DY_F77 F77_FUNC(gvalue3dy,GVALUE3DY)
void GVALUE3DY_F77 (float *yorg, float *dpy, int *ipos, int *ind)
{
	gsaf_gvalue3dy (*yorg, *dpy, *ipos, *ind);
}

#define GVALUE3DZ_F77 F77_FUNC(gvalue3dz,GVALUE3DZ)
void GVALUE3DZ_F77 (float *zorg, float *dpz, int *ipos, int *ind)
{
	gsaf_gvalue3dz (*zorg, *dpz, *ipos, *ind);
}

#define TEXT3D_F77 F77_FUNC(text3d,TEXT3D)
void TEXT3D_F77 (char *ktext, int *nchar)
{
	char *str;
	int i;

	str = (char *) gsgl_malloc_atomic (sizeof(char)*((*nchar)+1));
	for (i=0; i<*nchar; i++)
		str[i] = ktext[i];
	str[*nchar] = '\0';

	gsaf_text3d (str);
}

#define SET3DTEXTBASELINE_F77 F77_FUNC(set3dtextbaseline,SET3DTEXTBASELINE)
void SET3DTEXTBASELINE_F77 (float *x1, float *y1, float *z1, float *x2, float *y2, float *z2)
{
	gsaf_set3dtextbaseline (*x1, *y1, *z1, *x2, *y2, *z2);
}

#define TEXT2D_F77 F77_FUNC(text2d,TEXT2D)
void TEXT2D_F77 (char *ktext, int *nchar)
{
	char *str;
	int i;

	str = (char *) gsgl_malloc_atomic (sizeof(char)*((*nchar)+1));
	for (i=0; i<*nchar; i++)
		str[i] = ktext[i];
	str[*nchar] = '\0';

	gsaf_text2d (str);
}

#define GSGLENABLELIGHTING_F77 F77_FUNC(gsglenablelighting,GSGLENABLELIGHTING)
void GSGLENABLELIGHTING_F77 ()
{
	gsgl_lighting_enable_by_user (gsgl_inq_page());
}

#define PERSE3D1_F77 F77_FUNC(perse3d1,PERSE3D1)
void PERSE3D1_F77 (float *z, int *nmx, int *nx, int *ny, int *ixy, int *ind)
{
	gsaf_perse3d1 (z, *nmx, *nx, *ny, *ixy, *ind);
}

#define PERS3D1_F77 F77_FUNC(pers3d1,PERS3D1)
void PERS3D1_F77 (float *z, int *nmx, int *nx, int *ny, int *ind, RGBFUNC)
{
	gsaf_pers3d1 (z, *nmx, *nx, *ny, *ind, rgbfunc);
}

#define PERS3D2_F77 F77_FUNC(pers3d2,PERS3D2)
void PERS3D2_F77 (float *z, float *x, float *y, int *nmx, int *nx, int *ny, int *ind, RGBFUNC)
{
	gsaf_pers3d2 (z, x, y, *nmx, *nx, *ny, *ind, rgbfunc);
}

#define PERS3D3_F77 F77_FUNC(pers3d3,PERS3D3)
void PERS3D3_F77 (float *z, float *r, int *nmx, int *nx, int *ny, int *ind, RGBFUNC)
{
	gsaf_pers3d3 (z, r, *nmx, *nx, *ny, *ind, rgbfunc);
}

#define PERS3D4_F77 F77_FUNC(pers3d4,PERS3D4)
void PERS3D4_F77 (float *z, float *r, float *t, int *nmx, int *nx, int *ny, int *ind, RGBFUNC)
{
	gsaf_pers3d4 (z, r, t, *nmx, *nx, *ny, *ind, rgbfunc);
}

#define PERS3D5_F77 F77_FUNC(pers3d5,PERS3D5)
void PERS3D5_F77 (float *z, float *x, float *y, int *nmx, int *nx, int *ny, int *ind, RGBFUNC)
{
	gsaf_pers3d5 (z, x, y, *nmx, *nx, *ny, *ind, rgbfunc);
}

#define PERS3DF_F77 F77_FUNC(pers3df,PERS3DF)
void PERS3DF_F77 (float *z, float *x, float *y, int *nnod, int *ielm, int *nelm, int *ind, RGBFUNC)
{
	gsaf_pers3df (z, x, y, *nnod, ielm, *nelm, *ind, rgbfunc);
}

#define GDEFIN_F77 F77_FUNC(gdefin,GDEFIN)
void GDEFIN_F77 (float *pxmin, float *pxmax, float *pymin, float *pymax,
		 float *gxmin, float *gxmax, float *gymin, float *gymax)
{
	gsaf_gdefin (*pxmin, *pxmax, *pymin, *pymax,
		     *gxmin, *gxmax, *gymin, *gymax);
}

#define INQGDEFIN_F77 F77_FUNC(inqgdefin,INQGDEFIN)
void INQGDEFIN_F77 (float *pxmin, float *pxmax, float *pymin, float *pymax,
		    float *gxmin, float *gxmax, float *gymin, float *gymax)
{
	gsaf_inqgdefin (pxmin, pxmax, pymin, pymax, gxmin, gxmax, gymin, gymax);
}

#define GFRAME_F77 F77_FUNC(gframe,GFRAME)
void GFRAME_F77 ()
{
	gsaf_gframe ();
}

#define GSCALE_F77 F77_FUNC(gscale,GSCALE)
void GSCALE_F77 (float *xorg, float *xstep, float *yorg, float *ystep, float *sleng, int *ind)
{
	gsaf_gscale (*xorg, *xstep, *yorg, *ystep, *sleng, *ind);
}

#define GVALUE_F77 F77_FUNC(gvalue,GVALUE)
void GVALUE_F77 (float *xorg, float *xstep, float *yorg, float *ystep, int *ind)
{
	gsaf_gvalue (*xorg, *xstep, *yorg, *ystep, *ind);
}

#define GSCALL_F77 F77_FUNC(gscall,GSCALL)
void GSCALL_F77 (float *xorg, int *nxstep, float *yorg, int *nystep, float *sleng, int *ind)
{
	gsaf_gscall (*xorg, *nxstep, *yorg, *nystep, *sleng, *ind);
}

#define GVALUL_F77 F77_FUNC(gvalul,GVALUL)
void GVALUL_F77 (float *xorg, int *nxstep, float *yorg, int *nystep, int *ind)
{
	gsaf_gvalul (*xorg, *nxstep, *yorg, *nystep, *ind);
}

#define GPLOTP_F77 F77_FUNC(gplotp,GPLOTP)
void GPLOTP_F77 (float *gx, float *gy, int *nsrt, int *nend, int *nstep,
		 int *imark, int *istep, int *ipat)
{
	int ns = (*nsrt) - 1;
	int ne = (*nend) - 1;
	gsaf_gplotp (gx, gy, ns, ne, *nstep, *imark, *istep, *ipat);
}

#define GPLOTPG_F77 F77_FUNC(gplotpg,GPLOTPG)
void GPLOTPG_F77 (float *gx, float *gy, int *nsrt, int *nend, int *nstep,
		  int *imark, int *istep, int *ipat, int *id)
{
	int ns = (*nsrt) - 1;
	int ne = (*nend) - 1;
	gsaf_gplotpg (gx, gy, ns, ne, *nstep, *imark, *istep, *ipat, *id);
}

#define GPLOTPE_F77 F77_FUNC(gplotpe,GPLOTPE)
void GPLOTPE_F77 (float *gx, float *gy1, float *gy2, int *nsrt, int *nend, int *nstep,
		  float *scal)
{
	int ns;
	int ne;

	ns = (*nsrt) - 1;
	ne = (*nend) - 1;

	gsaf_gplotpe (gx, gy1, gy2, ns, ne, *nstep, *scal);
}

#define GVECTR_F77 F77_FUNC(gvectr,GVECTR)
void GVECTR_F77 (float *vx, float *vy, float *x, float *y, 
		 int *nxa, int *nxs, int *nxe, int *nxstep,
		 int *nys, int *nye, int *nystep, float *xn, float *yn, float *al)
{
	int ns1 = (*nxs) - 1;
	int ne1 = (*nxe) - 1;
	int ns2 = (*nys) - 1;
	int ne2 = (*nye) - 1;

	gsaf_gvectr (vx, vy, x, y, *nxa, ns1, ne1, *nxstep, ns2, ne2, *nystep, *xn, *yn, *al);
}

#define GVECTR5_F77 F77_FUNC(gvectr5,GVECTR5)
void GVECTR5_F77 (float *vx, float *vy, float *x, float *y,
		  int *nxa, int *nxs, int *nxe, int *nxstep, int *nys, int *nye, int *nystep,
		  float *xn, float *yn, float *al)
{
	int ns1 = (*nxs) - 1;
	int ne1 = (*nxe) - 1;
	int ns2 = (*nys) - 1;
	int ne2 = (*nye) - 1;

	gsaf_gvectr5 (vx, vy, x, y, *nxa, ns1, ne1, *nxstep, ns2, ne2, *nystep, *xn, *yn, *al);
}

#define CONTR1_F77 F77_FUNC(contr1,CONTR1)
void CONTR1_F77 (float *z, int *nxa, int *nxmax, int *nymax, 
		 float *zorg, float *zstep, int *nstep, int *iprd)
{
	gsaf_contr1 (z, *nxa, *nxmax, *nymax, *zorg, *zstep, *nstep, *iprd);
}

#define CONTR2_F77 F77_FUNC(contr2,CONTR2)
void CONTR2_F77 (float *z, float *x, float *y, int *nxa, int *nxmax, int *nymax,
		 float *zorg, float *zstep, int *nstep, int *iprd)
{
	gsaf_contr2 (z, x, y, *nxa, *nxmax, *nymax, *zorg, *zstep, *nstep, *iprd);
}

#define CONTR3_F77 F77_FUNC(contr3,CONTR3)
void CONTR3_F77 (float *z, float *r, int *nxa, int *nxmax, int *nymax, 
		float *zorg, float *zstep, int *nstep)
{
	gsaf_contr3 (z, r, *nxa, *nxmax, *nymax, *zorg, *zstep, *nstep);
}

#define CONTR4_F77 F77_FUNC(contr4,CONTR4)
void CONTR4_F77 (float *z, float *r, float *t, int *nxa, int *nxmax, int *nymax,
		 float *zorg, float *zstep, int *nstep)
{
	gsaf_contr4 (z, r, t, *nxa, *nxmax, *nymax, *zorg, *zstep, *nstep);
}

#define CONTP1_F77 F77_FUNC(contp1,CONTP1)
void CONTP1_F77 (float *z, int *nxa, int *nxmax, int *nymax, 
		float *zorg, float *zstep, int *nstep, int *iprd, int *ipat, int *ka)
{
	gsaf_contp1_internal (z, *nxa, *nxmax, *nymax, *zorg, *zstep, *nstep, *iprd, *ipat, ka);
}

#define CONTP2_F77 F77_FUNC(contp2,CONTP2)
void CONTP2_F77 (float *z, float *x, float *y, int *nxa, int *nxmax, int *nymax,
		float *zorg, float *zstep, int *nstep, int *iprd, int *ipat, int *ka)
{
	gsaf_contp2_internal (z, x, y, *nxa, *nxmax, *nymax, *zorg, *zstep, *nstep, *iprd, *ipat, ka);
}

#define CONTP3_F77 F77_FUNC(contp3,CONTP3)
void CONTP3_F77 (float *z, float *r, int *nxa, int *nxmax, int *nymax,
		float *zorg, float *zstep, int *nstep, int *ipat, int *ka)
{
	gsaf_contp3_internal (z, r, *nxa, *nxmax, *nymax, *zorg, *zstep, *nstep, *ipat, ka);
}

#define CONTP4_F77 F77_FUNC(contp4,CONTP4)
void CONTP4_F77 (float *z, float *r, float *t, int *nxa, int *nxmax, int *nymax,
		float *zorg, float *zstep, int *nstep, int *ipat, int *ka)
{
	gsaf_contp4_internal (z, r, t, *nxa, *nxmax, *nymax, *zorg, *zstep, *nstep, *ipat, ka);
}

#define CONTQ1_F77 F77_FUNC(contq1,CONTQ1)
void CONTQ1_F77 (float *z, int *nxa, int *nxmax, int *nymax, 
		float *zorg, float *zstep, int *nstep, int *iprd, int *ipat, int *ka)
{
	gsaf_contq1_internal (z, *nxa, *nxmax, *nymax, *zorg, *zstep, *nstep, *iprd, *ipat, ka);
}

#define CONTQ2_F77 F77_FUNC(contq2,CONTQ2)
void CONTQ2_F77 (float *z, float *x, float *y, int *nxa, int *nxmax, int *nymax,
		float *zorg, float *zstep, int *nstep, int *iprd, int *ipat, int *ka)
{
	gsaf_contq2_internal (z, x, y, *nxa, *nxmax, *nymax, *zorg, *zstep, *nstep, *iprd, *ipat, ka);
}

#define CONTQ3_F77 F77_FUNC(contq3,CONTQ3)
void CONTQ3_F77 (float *z, float *r, int *nxa, int *nxmax, int *nymax,
		float *zorg, float *zstep, int *nstep, int *ipat, int *ka)
{
	gsaf_contq3_internal (z, r, *nxa, *nxmax, *nymax, *zorg, *zstep, *nstep, *ipat, ka);
}

#define CONTQ4_F77 F77_FUNC(contq4,CONTQ4)
void CONTQ4_F77 (float *z, float *r, float *t, int *nxa, int *nxmax, int *nymax,
		float *zorg, float *zstep, int *nstep, int *ipat, int *ka)
{
	gsaf_contq4_internal (z, r, t, *nxa, *nxmax, *nymax, *zorg, *zstep, *nstep, *ipat, ka);
}

#define CONTPF_F77 F77_FUNC(contpf,CONTPF)
void CONTPF_F77 (float *z, float *x, float *y, int *nnod, int *ielm, int *nelm,
		float *zorg, float *zstep, int *nstep, int *ipat, int *ka)
{
	gsaf_contpf_internal (z, x, y, *nnod, ielm, *nelm, *zorg, *zstep, *nstep, *ipat, ka);
}

#define CONTR0_F77 F77_FUNC(contr0,CONTR0)
void CONTR0_F77 (float *z, float *x, float *y, int *nxa, int *nx, int *ny,
		 float *zorg, float *zstep, int *nstep, int *iprd, SUB)
{
	gsaf_contr0 (z, x, y, *nxa, *nx, *ny, *zorg, *zstep, *nstep, *iprd, sub);
}

#define CONTP0_F77 F77_FUNC(contp0,CONTP0)
void CONTP0_F77 (float *z, float *x, float *y, int *nxa, int *nxmax, int *nymax,
		 float *zorg, float *zstep, int *nstep, int *iprd, int *ipat, int *ka, SUB)
{
	gsaf_contp0 (z,x,y, *nxa, *nxmax, *nymax, *zorg, *zstep, *nstep, *iprd, *ipat,ka,sub);
}

#define CONTQ0_F77 F77_FUNC(contq0,CONTQ0)
void CONTQ0_F77 (float *z, float *x, float *y, int *nxa, int *nxmax, int *nymax,
		 float *zorg, float *zstep, int *nstep, int *iprd, int *ipat, int *ka, SUB)
{
	gsaf_contq0 (z, x, y, *nxa, *nxmax, *nymax, *zorg, *zstep, *nstep, *iprd, *ipat,ka,sub);
}

#define CONTE1_F77 F77_FUNC(conte1,CONTE1)
void CONTE1_F77 (float *z, int *nxa, int *nxmax, int *nymax, float *zl, int *nstep, 
		int *iprd, int *ipat, int *ka)
{
	gsaf_conte1_internal (z, *nxa, *nxmax, *nymax, zl, *nstep, *iprd, *ipat, ka);
}

#define CONTE2_F77 F77_FUNC(conte2,CONTE2)
void CONTE2_F77 (float *z, float *x, float *y, int *nxa, int *nxmax, int *nymax,
		float *zl, int *nstep, int *ipat, int *iprd, int *ka)
{
	gsaf_conte2_internal (z, x, y, *nxa, *nxmax, *nymax, zl, *nstep, *ipat, *iprd, ka);
}

#define CONTE3_F77 F77_FUNC(conte3,CONTE3)
void CONTE3_F77 (float *z, float *r, int *nxa, int *nxmax, int *nymax,
		float *zl, int *nstep, int *ipat, int *ka)
{
	gsaf_conte3_internal (z, r, *nxa, *nxmax, *nymax, zl, *nstep, *ipat, ka);
}

#define CONTE4_F77 F77_FUNC(conte4,CONTE4)
void CONTE4_F77 (float *z, float *r, float *t, int *nxa, int *nxmax, int *nymax,
		float *zl, int *nstep, int *ipat, int *ka)
{
	gsaf_conte4_internal (z, r, t, *nxa, *nxmax, *nymax, zl, *nstep, *ipat, ka);
}

#define CONTG1_F77 F77_FUNC(contg1,CONTG1)
void CONTG1_F77 (float *z, int *nxa, int *nxmax, int *nymax, float *zl, float *rgb,
		int *iln, float *wln, int *nstep, int *ispl, int *iprd, int *ka)
{
	gsaf_contg1_internal (z, *nxa, *nxmax, *nymax, zl, rgb, iln, wln, *nstep, *ispl, *iprd, ka);
}

#define CONTG2_F77 F77_FUNC(contg2,CONTG2)
void CONTG2_F77 (float *z, float *x, float *y, int *nxa, int *nxmax, int *nymax, float *zl,
		float *rgb, int *iln, float *wln, int *nstep, int *ispl, int *iprd, int *ka)
{
	gsaf_contg2_internal (z, x, y, *nxa, *nxmax, *nymax, zl,rgb, iln, wln, *nstep, *ispl, *iprd, ka);
}

#define CONTG3_F77 F77_FUNC(contg3,CONTG3)
void CONTG3_F77 (float *z, float *r, int *nxa, int *nxmax, int *nymax, float *zl, float *rgb,
		int *iln, float *wln, int *nstep, int *ispl, int *ka)
{
	gsaf_contg3_internal (z, r, *nxa, *nxmax, *nymax, zl, rgb, iln, wln, *nstep, *ispl, ka);
}

#define CONTG4_F77 F77_FUNC(contg4,CONTG4)
void CONTG4_F77 (float *z, float *r, float *t, int *nxa, int *nxmax, int *nymax,
		float *zl, float *rgb, int *iln, float *wln, int *nstep, int *ispl, int *ka)
{
	gsaf_contg4_internal (z, r, t, *nxa, *nxmax, *nymax, zl, rgb, iln, wln, *nstep, *ispl, ka);
}

#define CONTF1_F77 F77_FUNC(contf1,CONTF1)
void CONTF1_F77 (float *z, int *nxa, int *nxmax, int *nymax, 
		float *zl, float *rgb, int *nstep, int *iprd)
{
	gsaf_contf1 (z, *nxa, *nxmax, *nymax, zl, rgb, *nstep, *iprd);
}

#define CONTF2_F77 F77_FUNC(contf2,CONTF2)
void CONTF2_F77 (float *z, float *x, float *y, int *nxa, int *nxmax, int *nymax,
		float *zl, float *rgb, int *nstep, int *iprd)
{
	gsaf_contf2 (z, x, y, *nxa, *nxmax, *nymax, zl, rgb, *nstep, *iprd);
}

#define CONTF3_F77 F77_FUNC(contf3,CONTF3)
void CONTF3_F77 (float *z, float *r, int *nxa, int *nxmax, int *nymax,
		float *zl, float *rgb, int *nstep)
{
	gsaf_contf3 (z, r, *nxa, *nxmax, *nymax, zl, rgb, *nstep);
}

#define CONTF4_F77 F77_FUNC(contf4,CONTF4)
void CONTF4_F77 (float *z, float *r, float *t, int *nxa, int *nxmax, int *nymax,
		float *zl, float *rgb, int *nstep)
{
	gsaf_contf4 (z, r, t, *nxa, *nxmax, *nymax, zl, rgb, *nstep);
}

/* internal function */
#define CONTV1_F77 F77_FUNC(contv1,CONTV1)
void CONTV1_F77 (float *xa, float *ya, int *n, float *xb, float *yb, int *m, int *nn)
{
	gsaf_contv1 (xa, ya, *n, xb, yb, *m, nn);
}

/* internal function */
#define CONTV2_F77 F77_FUNC(contv2,CONTV2)
void CONTV2_F77 (float *ra, float *ta, int *n, float *xb, float *yb, int *m, int *nn)
{
	gsaf_contv2 (ra, ta, *n, xb, yb, *m, nn);
}

/* internal function */
#define CONTFX_F77 F77_FUNC(contfx,CONTFX)
void CONTFX_F77 (float *x, float *y, float *z, int *n, float *xt, float *yt, int *jh, float *ht, int *kmax)
{
	gsaf_contfx (x, y, z, *n, xt, yt, jh, ht, *kmax);
}

#define LINEPT_F77 F77_FUNC(linept,LINEPT)
void LINEPT_F77 (float *xg, float *yg, int *n, int *ipat)
{
	gsaf_linept (xg, yg, *n, *ipat);
}

#define GUSP2D_F77 F77_FUNC(gusp2d,GUSP2D)
void GUSP2D_F77 (float *xh, float *yh, int *n, 
		float *xp, float *yp, int *npm, int *np, int *ispl)
{
	gsaf_gusp2d (xh, yh, *n, xp, yp, *npm, np, *ispl);
}

#define GUCSPL_F77 F77_FUNC(gucspl,GUCSPL)
void GUCSPL_F77 (int *n, float *x, float *y, int *ikn, int *ioc, int *np, float *xp, float *yp)
{
	gsaf_gucspl (*n, x, y, ikn, *ioc, *np, xp, yp);
}

#define GUBSPL_F77 F77_FUNC(gubspl,GUBSPL)
void GUBSPL_F77 (float *tp, int *itm, int *n, int *ikn, int *m, float *b)
{
	gsaf_gubspl (*tp, itm, *n, ikn, *m, b);
}

#define RGBBAR_F77 F77_FUNC(rgbbar,RGBBAR)
void RGBBAR_F77 (float *x1, float *x2, float *y1, float *y2, float *rgb, int *n, int *ind)
{
	gsaf_rgbbar (*x1, *x2, *y1, *y2, rgb, *n, *ind);
}

#define CONTF0_F77 F77_FUNC(contf0,CONTF0)
void CONTF0_F77 (float *z, float *x, float *y, int *nxa, int *nx, int *ny, float *zl,
		 float *rgb, int *nstep, int *iprd, int *indx, SUBV)
{
	 gsaf_contf0 (z, x, y, *nxa, *nx, *ny, zl, rgb, *nstep, *iprd, *indx, subv);
}

#define CONTG0_F77 F77_FUNC(contg0,CONTG0)
void CONTG0_F77 (float *z, float *x, float *y, int *nxa, int *nxmax, int *nymax,
		 float *zl, float *rgb, int *iln, float *wln, int *nstep, int *ispl, int *iprd,
		 int *indx, SUBV, int *ka)
{
	gsaf_contg0 (z, x, y, *nxa, *nxmax, *nymax, zl, rgb, iln, wln, *nstep, *ispl, *iprd,
		     *indx, subv, ka);
}

#define SPLN1D_F77 F77_FUNC(spln1d,SPLN1D)
void SPLN1D_F77 (float *x,float *f,float *c,int *nx,float *xn,float *fn,int *nxn,int *ip)
{
	gsaf_spln1d(x,f,c,*nx,xn,fn,*nxn,*ip);
}

#define GUSPLC_F77 F77_FUNC(gusplc,GUSPLC)
void GUSPLC_F77 (float *x,int *n,float *y,float *df,int *iopt,float *c,int *nc,int *ier)
{
	gsaf_gusplc(x, *n, y, df, iopt, c, *nc, ier);
}

#define GUSPLF_F77 F77_FUNC(gusplf,GUSPLF)
void GUSPLF_F77 (float *x,int *n,float *y,float *c,int *nc,float *v,int *m,float *f,int *ier)
{
	gsaf_gusplf(x, *n, y, c, *nc, v, *m, f, ier);
}

#define SPLN2D_F77 F77_FUNC(spln2d,SPLN2D)
void SPLN2D_F77 (float *x,float *y,float *f,float *p,float *q,float *s,float *a,
	    int *im,int *ni,int *nj,float *xg,float *yg,float *fg,
	    int *in,int *nni,int *nnj,int *ip)
{
	gsaf_spln2d(x,y,f,p,q,s,a,*im,*ni,*nj,xg,yg,fg,*in,*nni,*nnj,*ip);
}

#define GUSPLN_F77 F77_FUNC(guspln,GUSPLN)
void GUSPLN_F77 (float *x,float *y,float *f,int *ni,int *nj,
		 float *p,float *q,float *s,float *a,int *im,int *ip )
{
	gsaf_guspln (x,y,f,*ni,*nj,p,q,s,a,*im,*ip);
}

#define GUENCN_F77 F77_FUNC(guencn,GUENCN)
void GUENCN_F77 (float *p,float *q,float *s,float *x,float *y,float *f,int *im,int *ni,int *nj)
{
	gsaf_guencn (p,q,s,x,y,f,*im,*ni,*nj);
}

#define GUBNDA_F77 F77_FUNC(gubnda,GUENCN)
void GUBNDA_F77 (float *a,int *l1)
{
	gsaf_gubnda (a,l1);
}

#define GUCALG_F77 F77_FUNC(gucalg,GUCALG)
void GUCALG_F77 (float *x,float *y,float *f,float *p,float *q,float *s,float *gam,
		 int *im,int *i,int *j)
{
	gsaf_gucalg(x,y,f,p,q,s,gam,*im,*i,*j);
}

#define GUCALA_F77 F77_FUNC(gucala,GUCALA)
void GUCALA_F77 (float *h,float *a)
{
	gsaf_gucala (*h,a);
}

#define GUCALK_F77 F77_FUNC(gucalk,GUCALK)
void GUCALK_F77 (float *p,float *q,float *s,float *f,float *fk,int *im,int *i,int *j)
{
	gsaf_gucalk(p,q,s,f,fk,*im,*i,*j);
}

#define R2G2B_17_F77 F77_FUNC_(r2g2b_17,R2G2B_17)
void R2G2B_17_F77 (float *r, float *rgb)
{
	gsaf_r2g2b_17 (r, rgb);
}

#define R2G2P_F77 F77_FUNC(r2g2p,R2G2P)
void R2G2P_F77 (float *r, float *rgb)
{
	gsaf_r2g2p (r, rgb);
}

#define WHITE_F77 F77_FUNC(white,WHITE)
void WHITE_F77 (float *r, float *rgb)
{
	gsaf_white (r, rgb);
}

#define BLACK_F77 F77_FUNC(black,BLACK)
void BLACK_F77 (float *r, float *rgb)
{
	gsaf_black (r, rgb);
}

#define R2G2B_F77 F77_FUNC(r2g2b,R2G2B)
void R2G2B_F77 (float *r, float *rgb)
{
	gsaf_r2g2b (r, rgb);
}

#define R2W2B_F77 F77_FUNC(r2w2b,R2W2B)
void R2W2B_F77 (float *r, float *rgb)
{
	gsaf_r2w2b (r, rgb);
}

#define R2G2BW_F77 F77_FUNC(r2g2bw,R2G2BW)
void R2G2BW_F77 (float *r, float *rgb)
{
	gsaf_r2g2bw (r, rgb);
}

#define R2Y2W_F77 F77_FUNC(r2y2w,R2Y2W)
void R2Y2W_F77 (float *r, float *rgb)
{
	gsaf_r2y2w(r, rgb);
}

#define W2G2B_F77 F77_FUNC(w2g2b,W2G2B)
void W2G2B_F77 (float *r, float *rgb)
{
	gsaf_w2g2b(r, rgb);
}  

#define W2R2K_F77 F77_FUNC(w2r2k,W2R2K)
void W2R2K_F77 (float *r, float *rgb)
{
	gsaf_w2r2k(r, rgb);
}  

#define K2B2W_F77 F77_FUNC(k2b2w,K2B2W)
void K2B2W_F77 (float *r, float *rgb)
{
	gsaf_k2b2w(r, rgb);
}  

#define WRKBW_F77 F77_FUNC(wrkbw,WRKBW)
void WRKBW_F77 (float *r, float *rgb)
{
	gsaf_wrkbw(r, rgb);
}  

#define WRGBW_F77 F77_FUNC(wrgbw,WRGBW)
void WRGBW_F77 (float *r, float *rgb)
{
	gsaf_wrgbw(r, rgb);
}  

#define R2G2B_GAT0_F77 F77_FUNC_(r2g2b_gat0,R2G2B_GAT0)
void R2G2B_GAT0_F77 (float *r, float *rgb)
{
	gsaf_r2g2b_gat0(r, rgb);
}  

#define R2W2B_WAT0_F77 F77_FUNC_(r2w2b_wat0,R2W2B_WAT0)
void R2W2B_WAT0_F77 (float *r, float *rgb)
{
	gsaf_r2w2b_wat0(r, rgb);
}  

#define GUDATE_F77 F77_FUNC(gudate,GUDATE)
void GUDATE_F77 (int *ndy, int *ndm, int *ndd, int *nth, int *ntm, int *nts)
{
	gsaf_gudate (ndy, ndm, ndd, nth, ntm, nts);
}

#define GUTIME_F77 F77_FUNC(gutime,GUTIME)
void GUTIME_F77 (float *time)
{
	gsaf_gutime (time);
}

#define GU_SLEEP_F77 F77_FUNC_(gu_sleep,GU_SLEEP)
void GU_SLEEP_F77 (float *t)
{
	gsaf_gu_sleep (*t);
}

#define GURAND_F77 F77_FUNC(gurand,GURAND)
float GURAND_F77 (float *x)
{
	return gsaf_gurand (*x);
}

#define GU_SRAND_F77 F77_FUNC_(gu_srand,GU_SRAND)
void GU_SRAND_F77 (int *i)
{
	gsaf_gu_srand (*i);
}

#define GUFLSH_F77 F77_FUNC(guflsh,GUFLSH)
void GUFLSH_F77 ()
{
	gsaf_guflsh ();
}

#define ANIMES_F77 F77_FUNC(animes,ANIMES)
void ANIMES_F77 ()
{
	gsgl_animes ();
}
  
#define ANIMEE_F77 F77_FUNC(animee,ANIMEE)
void ANIMEE_F77 ()
{
	gsgl_animee ();
}

#define GSGLSOLIDSPHERE_F77 F77_FUNC(gsglsolidsphere, GSGLSOLIDSPHERE)
void GSGLSOLIDSPHERE_F77 (float *x, float *y, float *z, float *vx, float *vy, float *vz,
			  float *radius, int *slices, int *stacks)
{
	gsgl_solidsphere (*x, *y, *z, *vx, *vy, *vz,
			  *radius, *slices, *stacks);
}
	
#define GSGLWIRESPHERE_F77 F77_FUNC(gsglwiresphere, GSGLWIRESPHERE)
void GSGLWIRESPHERE_F77 (float *x, float *y, float *z, float *vx, float *vy, float *vz,
			 float *radius, int *slices, int *stacks)
{
	gsgl_wiresphere (*x, *y, *z, *vx, *vy, *vz,
			 *radius, *slices, *stacks);
}

#define GSGLSOLIDTORUS_F77 F77_FUNC(gsglsolidtorus, GSGLSOLIDTORUS)
void GSGLSOLIDTORUS_F77 (float *x, float *y, float *z, float *vx, float *vy, float *vz,
			 float *innerRadius, float *outerRadius, int *nsides, int *rings)
{
	gsgl_solidtorus (*x, *y, *z, *vx, *vy, *vz,
			 *innerRadius, *outerRadius, *nsides, *rings);	
}

#define GSGLWIRETORUS_F77 F77_FUNC(gsglwiretorus, GSGLWIRETORUS)
void GSGLWIRETORUS_F77 (float *x, float *y, float *z, float *vx, float *vy, float *vz,
			float *innerRadius, float *outerRadius, int *nsides, int *rings)
{
	gsgl_wiretorus (*x, *y, *z, *vx, *vy, *vz,
			*innerRadius, *outerRadius, *nsides, *rings);
}

#define GSGLPOLYGONTEXT_F77 F77_FUNC(gsglpolygontext,GSGLPOLYGONTEXT)
void GSGLPOLYGONTEXT_F77 ()
{
	gsgl_polygontext ();
}

#define GSGLVECTORTEXT_F77 F77_FUNC(gsglvectortext,GSGLVECTORTEXT)
void GSGLVECTORTEXT_F77 ()
{
	gsgl_vectortext ();
}

#define GSGLSETFREETYPENODE_F77 F77_FUNC(gsglsetfreetypenode,GSGLSETFREETYPENODE)
void GSGLSETFREETYPENODE_F77 (int *node)
{
	gsgl_setfreetypenode (*node);
}

#define GSGLUSETYPE1_F77 F77_FUNC(gsglusetype1,GSGLUSETYPE1)
void GSGLUSETYPE1_F77 ()
{
	gsgl_usetype1 ();
}

#define GAXIS2_F77 F77_FUNC(gaxis2,GAXIS2)
void GAXIS2_F77 (float *x0, float *y0, float *x1, float *y1,
		float *fmin, float *fmax, float *forg, float *fstep, float *sleng, int *ind)
{
	gsaf_gaxis2 (*x0, *y0, *x1, *y1, *fmin, *fmax, *forg, *fstep, *sleng, *ind);
}

#define GAXIV2_F77 F77_FUNC(gaxiv2,GAXIV2)
void GAXIV2_F77 (float *x0, float *y0, float *x1, float *y1,
		 float *fmin, float *fmax, float *forg, float *fstep, int *indc, int *indp)
{
	gsaf_gaxiv2 (*x0, *y0, *x1, *y1, *fmin, *fmax, *forg, *fstep, *indc, *indp);
}

#define SETPAGE_F77 F77_FUNC(setpage,SETPAGE)
void SETPAGE_F77 (float *xoffset, float *yoffset, float *xscale, float *yscale)
{
	gsaf_setpage (*xoffset, *yoffset, *xscale, *yscale);
}

#define INQPAGE_F77 F77_FUNC(inqpage,INQPAGE)
void INQPAGE_F77 (float *xoffset, float *yoffset, float *xscale, float *yscale)
{
	gsaf_inqpage (xoffset, yoffset, xscale, yscale);
}

#define SETQUITCALLBACK_F77 F77_FUNC(setquitcallback,SETQUITCALLBACK)
void SETQUITCALLBACK_F77 (void (*callback)())
{
	gsaf_set_quit_callback (callback);
}

#define RESETQUITCALLBACK_F77 F77_FUNC(resetquitcallback,RESETQUITCALLBACK)
void RESETQUITCALLBACK_F77 ()
{
	gsaf_reset_quit_callback ();
}

#define GUIBUTTON_F77 F77_FUNC(guibutton,GUIBUTTON)
void GUIBUTTON_F77 (float *x, float *y, float *w, float *h, char *text, void (*callback)(void))
{
	char *ktext;
        int ntext;

        ntext = 256;
	ktext = (char *) gsgl_malloc_atomic ((ntext+1)*sizeof(char));
	gsaf_gudstr (text, ktext);

	gsaf_guibutton (*x, *y, *w, *h, ktext, callback);
}

#define GUICOMBOBOX_F77 F77_FUNC(guicombobox,GUICOMBOBOX)
void GUICOMBOBOX_F77 (float *x, float *y, float *w, float *h, int *nitem, char *text1,
		      int *current1, void (*callback)(int *))
{
	int i;
	char **text;
	int ntext;
	int current;

	current = (*current1) - 1;

	text = (char **) gsgl_malloc (sizeof(char *) * (*nitem));
	for (i=0; i<*nitem; i++) {
		ntext = 256;
		text[i] = (char *) gsgl_malloc_atomic ((ntext+1)*sizeof(char));
		gsaf_gudstr (&(text1[i*256]), text[i]);
	}

	gsaf_guicombobox (*x, *y, *w, *h, *nitem, text,
			  current, callback, 1);
}

#define GUISPINBUTTONF_F77 F77_FUNC(guispinbuttonf,GUISPINBUTTONF)
void GUISPINBUTTONF_F77 (float *x, float *y, float *w, float *h, float *init, float *lower, float *upper, int *digits, void (*callback)(float *))
{
	gsaf_guispinbuttonf (*x, *y, *w, *h, *init, *lower, *upper, *digits, callback);
}

#define GUISPINBUTTONI_F77 F77_FUNC(guispinbuttoni,GUISPINBUTTONI)
void GUISPINBUTTONI_F77 (float *x, float *y, float *w, float *h, int *init, int *lower, int *upper, void (*callback)(int *))
{
	gsaf_guispinbuttoni (*x, *y, *w, *h, *init, *lower, *upper, callback);
}

#define GUICHECKBUTTON_F77 F77_FUNC(guicheckbutton,GUICHECKBUTTON)
void GUICHECKBUTTON_F77 (float *x, float *y, float *w, float *h, char *label1, 
			 int *init, void (*callback)(int *))
{
	char *label;
        int ntext;

        ntext = 256;
	label = (char *) gsgl_malloc_atomic ((ntext+1)*sizeof(char));
	gsaf_gudstr (label1, label);

	gsaf_guicheckbutton (*x, *y, *w, *h, label, *init, callback);
}

#define GUILINEEDIT_F77 F77_FUNC(guilineedit,GUILINEEDIT)
void GUILINEEDIT_F77 (float *x, float *y, float *w, float *h, int *max, char *text, 
		      int *ntext, void (*callback)(const char *, int *))
{
	char *ktext;
	int i;

	ktext = (char *) gsgl_malloc_atomic ((*ntext+1)*sizeof(char));
	for (i=0; i<*ntext; i++)
		ktext[i] = text[i];
	ktext[*ntext] = '\0';

	gsaf_guilineedit (*x, *y, *w, *h, *max, ktext, callback);
}

/*#define GSCLEAR_F77 F77_FUNC(gsclear,GSCLEAR)
  void GSCLEAR_F77 ()
  {
  gsaf_clear ();
  }*/

#define INQLABELSIZE_F77 F77_FUNC(inqlabelsize,INQLABELSIZE)
void INQLABELSIZE_F77 (char *text, float *w, float *h)
{
	char *ktext;
        int ntext;

        ntext = 256;
	ktext = (char *) gsgl_malloc_atomic ((ntext+1)*sizeof(char));
	gsaf_gudstr (text, ktext);

	gsaf_inqlabelsize (ktext, w, h);
}

#define GUIEXTERNAL_F77 F77_FUNC(guiexternal,GUIEXTERNAL)
void GUIEXTERNAL_F77 (int *i)
{
	gsaf_guiexternal (*i);
}

#define GUILABEL_F77 F77_FUNC(guilabel,GUILABEL)
void GUILABEL_F77 (float *x, float *y, float *w, float *h, char *text, int *ijust)
{
	char *ktext;
        int ntext;

        ntext = 256;
	ktext = (char *) gsgl_malloc_atomic ((ntext+1)*sizeof(char));
	gsaf_gudstr (text, ktext);

	gsaf_guilabel (*x, *y, *w, *h, ktext, *ijust);
}

#define GUIEXITLOOP_F77 F77_FUNC(guiexitloop,GUIEXITLOOP)
void GUIEXITLOOP_F77 ()
{
	gsaf_guiexitloop ();
}

/*#define AA_F77 F77_FUNC(aa,AA)
  void AA_F77 (
  {*/
