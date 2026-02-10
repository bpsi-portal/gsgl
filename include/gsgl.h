
#ifndef __gsgl_h_
#define __gsgl_h_

#define GSGL_PAPER_WIDTH 25.6f
#define GSGL_PAPER_HEIGHT 19.0f
#define GSGL_HEADER_HEIGHT 0.9f
#define GSGL_VIEW_DEPTH 100.0f

#define GSGL_ORTHOGONAL  0
#define GSGL_FRUSTUM     1

#ifdef __cplusplus
extern "C"{
#endif

/* anime.c */
void gsgl_animes (void);
void gsgl_animee (void);

/* apl1.c */
void gsaf_textx (char *ktextx);
void gsaf_textx2d (char *ktextx);
void gsaf_textx3d (char *ktextx);
void gsaf_gtext (float x, float y, char *ktext, int ijust);
void gsaf_gtext2d (float x, float y, char *ktext, int ijust);
void gsaf_gtext3d (float x, float y, float z, char *ktext, int ijust);
void gsaf_gtextx (float x, float y, char *ktextx, int ijust);
void gsaf_gtextx2d (float x, float y, char *ktextx, int ijust);
void gsaf_gtextx3d (float x, float y, float z, char *ktextx, int ijust);
void gsaf_numbi (int i, char *kform, int nchar);
void gsaf_numbi2d (int i, char *kform, int nchar);
void gsaf_numbi3d (int i, char *kform, int nchar);
void gsaf_numbd (double d, char *kform, int nchar);
void gsaf_numbd2d (double d, char *kform, int nchar);
void gsaf_numbd3d (double d, char *kform, int nchar);
void gsaf_numbr (float r, char *kform, int nchar);
void gsaf_numbr2d (float r, char *kform, int nchar);
void gsaf_numbr3d (float r, char *kform, int nchar);
void gsaf_gnumbi (float x, float y, int i, int ijust);
void gsaf_gnumbi2d (float x, float y, int i, int ijust);
void gsaf_gnumbi3d (float x, float y, float z, int i, int ijust);
void gsaf_gnumbd (float x, float y, double d, int ind, int ijust);
void gsaf_gnumbd2d (float x, float y, double d, int ind, int ijust);
void gsaf_gnumbd3d (float x, float y, float z, double d, int ind, int ijust);
void gsaf_gnumbr (float x, float y, float r, int ind, int ijust);
void gsaf_gnumbr2d (float x, float y, float r, int ind, int ijust);
void gsaf_gnumbr3d (float x, float y, float z, float r, int ind, int ijust);
void gsaf_gnumbp (float x, float y, int i, int j, int ijust);
void gsaf_gnumbp2d (float x, float y, int i, int j, int ijust);
void gsaf_gnumbp3d (float x, float y, float z, int i, int j, int ijust);
void gsaf_drawpt (float x, float y);
void gsaf_drawpt2d (float x, float y);
void gsaf_drawpt3d (float x, float y, float z);
void gsaf_movept (float x, float y, int ipat);
void gsaf_movept2d (float x, float y, int ipat);
void gsaf_movept3d (float x, float y, float z, int ipat);
void gsaf_setlpt (int ipat, int npat, float pat[10]);
void gsaf_gmnmx1 (float *a, int ns, int ne, int nstep, float *amin, float *amax);
void gsaf_gmnmx2 (float *a, int nxa, int nxs, int nxe, int nxstep,
		  int nys, int nye, int nystep, float *amin, float *amax);
void gsaf_gqscal (float min, float max, float *gmin1, float *gmax1, float *scale1);

/* apl2.c */
void gsaf_gdefin (float pxmin, float pxmax, float pymin, float pymax,
		  float gxmin, float gxmax, float gymin, float gymax);
void gsaf_inqgdefin (float *pxs,float *pxe, float *pys, float *pye,
		     float *gxs,float *gxe, float *gys, float *gye);
void gsaf_gframe (void);
void gsaf_gscale (float xorg, float xstep, float yorg, float ystep, float sleng, int ind);
void gsaf_gvalue (float xorg, float xstep, float yorg, float ystep, int ind);
void gsaf_gscall (float xorg, int nxstep, float yorg, int nystep, float sleng, int ind);
void gsaf_gvalul (float xorg, int nxstep, float yorg, int nystep, int ind);
void gsaf_gplotp (float *gx,float *gy,int nsrt,int nend,int nstep,int imark,int istep,int ipat);
void gsaf_gplotpg (float *gx, float *gy, int nsrt, int nend, int nstep,
		   int imark, int istep, int ipat, int id);
void gsaf_gplotpe (float *gx, float *gy1, float *gy2, int nsrt, int nend, int nstep, float scal);
void gsaf_gvectr (float *vx,float *vy,float *x,float *y,int nxa,int nxs,int nxe,int nxstep,
		  int nys, int nye, int nystep, float xn, float yn, float al);
void gsaf_gvectr5 (float *vx, float *vy, float *x, float *y,
		   int nxa, int nxs, int nxe, int nxstep, int nys, int nye, int nystep,
		   float xn, float yn, float al);

/* apl3.c */
void gsaf_gaxis2 (float x0, float y0, float x1, float y1,
		  float fmin, float fmax, float forg, float fstep, float sleng, int ind);
void gsaf_gaxiv2 (float x0, float y0, float x1, float y1,
		  float fmin, float fmax, float forg, float fstep, int indc, int indp);

/* apl3d.c */
void gsaf_gaxis3d (int iaxis);
void gsaf_gdrawback3d (float r, float g, float b);
void gsaf_gplotp3d (float *gx, float *gy, float *gz, int nsrt, int nend, int nstep, 
		    int imark, int istep, int ipat);
void gsaf_gscale3dx (float xorg, float xstep, float scl, int ipos);
void gsaf_gscale3dy (float yorg, float ystep, float scl, int ipos);
void gsaf_gscale3dz (float zorg, float zstep, float scl, int ipos);
void gsaf_gvalue3dx (float xorg, float dpx, int ipos, int ind);
void gsaf_gvalue3dy (float yorg, float dpy, int ipos, int ind);
void gsaf_gvalue3dz (float zorg, float dpz, int ipos, int ind);

/* base.c */
void gsgl_enablelighting (void);
void gsaf_guilabel (float x, float y, float w, float h, char *text, int ijust);
void gsaf_guibutton (float x, float y, float w, float h, char *text, void (*callback)(void));
void gsaf_guicheckbutton (float x, float y, float w, float h, char *text, int init, void (*callback)(int *));
void gsaf_guicombobox (float x, float y, float w, float h, int items, char *text[], int current, void (*callback)(int *), int fortran);
void gsaf_guilineedit (float x, float y, float w, float h, int max, char *text, void (*callback)(const char *, int *));
void gsaf_guispinbuttonf (float x, float y, float w, float h, float init, float lower, float upper, int digits, void (*callback)(float *));
void gsaf_guispinbuttoni (float x, float y, float w, float h, int init, int lower, int upper, void (*callback)(int *));
void gsaf_guiexternal (int b);

/* bsc1.c */
void gsaf_gsopen (void);
void gsaf_gsopen_rank (int i);
void gsaf_gsclos (void);
void gsaf_gssize (float x, float y);
void gsaf_gstitl (char *title);
void gsaf_pages (void);
void gsaf_pagee (void);
void gsaf_pagey (void);
void gsaf_pagen (void);
void gsaf_setvew (float pxmin, float pxmax, float pymin, float pymax, 
		  float xmin, float xmax, float ymin, float ymax);
void gsaf_offvew (void);
void gsaf_inqvew (float *pxmin, float *pxmax, float *pymin, float *pymax, 
		  float *xmin, float *xmax, float *ymin, float *ymax);
void gsgl_projimgtranslate (float dx, float dy);
void gsgl_projimgscale (float xratio, float yratio);
void gsgl_projimgrotate (float degree); /* angle in degree */
void gsgl_projimgidentity (void);
void gsaf_setclp (float xmin, float xmax, float ymin, float ymax);
void gsaf_offclp (void);
void gsaf_inqclp (float *xmin, float *xmax, float *ymin, float *ymax);
void gsaf_move (float x, float y);
void gsaf_draw (float x, float y);
void gsaf_mark (float x, float y);
void gsaf_inqpos (float *x, float *y);
void gsaf_text (char *text);
void gsaf_inqtextbbox (char *text, float *size, float *height, float *depth);
void gsaf_inqtsz (char *text, float *size);
void gsaf_setlin (int iln, int ibl, int icl);
void gsaf_inqlin (int *iln, int *ibl, int *icl);
void gsaf_setmrk (int imrk, float hmrk, float wmrk, float angl, float tilt);
void gsaf_setmks (int imrk, float smrk);
void gsaf_inqmrk (int *imrk, float *hmrk, float *wmrk, float *angl, float *tilt);
void gsaf_setchr (float chh, float chw, float chsp, float angl, float tilt);
void gsaf_setchs (float chh, float angl);
void gsaf_setfnt (int fnt);
void gsaf_inqchr (float *chh, float *chw, float *chsp, float *angl, float *tilt);
void gsaf_inqfnt (int *fnt);
void gsaf_eras (void);
void gsaf_bell (void);
void gsaf_gudstr (char *kin, char *kout);
void gsaf_gucptl (char *kid);
float gsaf_guclip (double d);
int gsaf_ngulen (float gstep);
void gsaf_set_quit_callback (void (*callback)(void));
void gsaf_reset_quit_callback (void);
int gsaf_inq_rank (void);

/* bsc2.c */
void gsaf_line1 (float xs, float ys, float xe, float ye);
void gsaf_lines (float *x, float *y, int n);
void gsaf_linesc (float *x, float *y, int n);
void gsaf_poly (float *x, float *y, int n);
void gsaf_rgbtrg (float xn[3], float yn[3], float r[3], float g[3], float b[3]);
void gsaf_setlnw (float w);
void gsaf_inqlnw (float *w);
void gsaf_setrgb (float r, float g, float b);
void gsaf_inqrgb (float *r, float *g, float *b);

/* bsc2d.c */
void gsaf_move2d (float x1, float y1);
void gsaf_inqpos2d (float *x1, float *y1);
void gsaf_draw2d (float x1, float y1);
void gsaf_poly2d (float *xn, float *yn, int n);
void gsaf_rgbtrg2d (float *xn, float *yn, float *r, float *g, float *b);
void gsaf_line2d (float x1, float y1, float x2, float y2);
void gsaf_lines2d (float *x, float *y, int n);
void gsaf_linesc2d (float *x, float *y, int n);
void gsaf_mark2d (float x, float y);
void gsaf_text2d (char *ktext);

/* bsc3d.c */
void gsaf_gdefin3d (float xl1, float yl1, float zl1, float gxmin1, float gxmax1, 
		    float gymin1, float gymax1, float gzmin1, float gzmax1);
void gsaf_setprojectiontype (int iproj);
void gsaf_set3dcoord_caut1 (float ox1, float oy1, float oz1, float depth, 
			    float *vx, float *vy, float *vz);
void gsaf_set3dcoord_caut2 (float wx, float wy, float depth, float x, float y, float z,
			    float *vx, float *vy, float *vz);
void gsaf_set3dcoord_polar1 (float ox1, float oy1, float oz1, 
			     float radius1, float theta1, float phi1);
void gsaf_set3dcoord_polar2 (float wx, float wy, float depth, float x, float y, float z,
			     float theta, float phi);
void gsgl_seteyepos (float eye);
void gsaf_move3d (float x1, float y1, float z1);
void gsaf_inqpos3d (float *x1, float *y1, float *z1);
void gsaf_draw3d (float x1, float  y1, float  z1);
void gsaf_draw3d_with_normal (float x1, float  y1, float z1, float  vx, float  vy, float  vz);
void gsaf_draw3d_with_normals (float x1, float y1, float z1,float vx[],float vy[],float vz[]);
void gsaf_poly3d (float *xn, float *yn, float *zn, int n);
void gsaf_poly3d_with_normal (float *xn, float *yn, float *zn, 
			      float vx1, float vy1, float vz1, int n);
void gsaf_poly3d_with_normals (float *xn, float *yn, float *zn, float *vx, float *vy, float *vz,
			       int n);
void gsaf_rgbtrg3d (float *xn, float *yn, float *zn, float *r, float *g, float *b);
void gsaf_rgbtrg3d_with_normal (float *xn, float *yn, float *zn, float *r, float *g, float *b, 
				float vx, float vy, float vz);
void gsaf_rgbtrg3d_with_normals (float *xn, float *yn, float *zn, float *r, float *g, float *b,
				 float *vx, float *vy, float *vz);
void gsaf_line3d (float x1, float y1, float z1, float x2, float y2, float z2);
void gsaf_line3d_with_normal (float x1, float y1, float z1, float x2, float y2, float z2,
			      float vx, float vy, float vz);
void gsaf_line3d_with_normals (float x1, float y1, float z1, float x2, float y2, float z2, 
			       float vx[], float vy[], float vz[]);
void gsaf_lines3d (float *x, float *y, float *z, int n);
void gsaf_lines3d_with_normal (float *x,float *y,float *z,float vx, float vy, float vz, int n);
void gsaf_lines3d_with_normals (float *x, float *y, float *z, 
				float vx[], float vy[], float vz[], int n);
void gsaf_text3d (char *ktext);
void gsaf_set3dtextbaseline (float x1, float y1, float z1, float x2, float y2, float z2);
	
/* cont1.c */
void gsaf_contr1 (float *z, int nxa, int nxmax, int nymax, 
		  float zorg, float zstep, int nstep, int iprd);
void gsaf_contr2 (float *z, float *x, float *y, int nxa, int nxmax, int nymax,
		  float zorg, float zstep, int nstep, int iprd);
void gsaf_contr3 (float *z, float *r, int nxa, int nxmax, int nymax, 
		  float zorg, float zstep, int nstep);
void gsaf_contr4 (float *z, float *r, float *t, int nxa, int nxmax, int nymax,
		  float zorg, float zstep, int nstep);
void gsaf_contp1 (float *z, int nxa, int nxmax, int nymax, 
		  float zorg, float zstep, int nstep, int iprd, int ipat);
void gsaf_contp2 (float *z, float *x, float *y, int nxa, int nxmax, int nymax,
		  float zorg, float zstep, int nstep, int iprd, int ipat);
void gsaf_contp3 (float *z, float *r, int nxa, int nxmax, int nymax,
		  float zorg, float zstep, int nstep, int ipat);
void gsaf_contp4 (float *z, float *r, float *t, int nxa, int nxmax, int nymax,
		  float zorg, float zstep, int nstep, int ipat);
void gsaf_contq1 (float *z, int nxa, int nxmax, int nymax, 
		  float zorg, float zstep, int nstep, int iprd, int ipat);
void gsaf_contq2 (float *z, float *x, float *y, int nxa, int nxmax, int nymax,
		  float zorg, float zstep, int nstep, int iprd, int ipat);
void gsaf_contq3 (float *z, float *r, int nxa, int nxmax, int nymax,
		  float zorg, float zstep, int nstep, int ipat);
void gsaf_contq4 (float *z, float *r, float *t, int nxa, int nxmax, int nymax,
		  float zorg, float zstep, int nstep, int ipat);
void gsaf_contpf (float *z, float *x, float *y, int nnod, int *ielm, int nelm,
		  float zorg, float zstep, int nstep, int ipat);
/*#define SUB void (*sub) (int,int,int,int,float,float,float,float *,float *,int,int)*/
#define SUB void (*sub) (int*,int*,int*,int*,float*,float*,float*,float*,float*,int*,int*)
void gsaf_contr0 (float *z, float *x, float *y, int nxa, int nx, int ny,
		  float zorg, float zstep, int nstep, int iprd, SUB);
void gsaf_contp0 (float *z, float *x, float *y, int nxa, int nxmax, int nymax,
		  float zorg, float zstep, int nstep, int iprd, int ipat, int *ka, SUB);
void gsaf_contq0 (float *z, float *x, float *y, int nxa, int nxmax, int nymax,
		  float zorg, float zstep, int nstep, int iprd, int ipat, int *ka, SUB);

/* cont2.h */
void gsaf_conte1 (float *z, int nxa, int nxmax, int nymax, float *zl, int nstep, 
		  int iprd, int ipat);
void gsaf_conte2 (float *z, float *x, float *y, int nxa, int nxmax, int nymax,
		  float *zl, int nstep, int ipat, int iprd);
void gsaf_conte3 (float *z, float *r, int nxa, int nxmax, int nymax,
		  float *zl, int nstep, int ipat);
void gsaf_conte4 (float *z, float *r, float *t, int nxa, int nxmax, int nymax,
		  float *zl, int nstep, int ipat);
void gsaf_contg1 (float *z, int nxa, int nxmax, int nymax, float *zl, float *rgb,
		  int *iln, float *wln, int nstep, int ispl, int iprd);
void gsaf_contg2 (float *z, float *x, float *y, int nxa, int nxmax, int nymax, float *zl,
		  float *rgb, int *iln, float *wln, int nstep, int ispl, int iprd);
void gsaf_contg3 (float *z, float *r, int nxa, int nxmax, int nymax, float *zl, float *rgb,
		  int *iln, float *wln, int nstep, int ispl);
void gsaf_contg4 (float *z, float *r, float *t, int nxa, int nxmax, int nymax,
		  float *zl, float *rgb, int *iln, float *wln, int nstep, int ispl);
void gsaf_contf1 (float *z, int nxa, int nxmax, int nymax, 
		  float *zl, float *rgb, int nstep, int iprd);
void gsaf_contf2 (float *z, float *x, float *y, int nxa, int nxmax, int nymax,
		  float *zl, float *rgb, int nstep, int iprd);
void gsaf_contf3 (float *z, float *r, int nxa, int nxmax, int nymax,
		  float *zl, float *rgb, int nstep);
void gsaf_contf4 (float *z, float *r, float *t, int nxa, int nxmax, int nymax,
		  float *zl, float *rgb, int nstep);
void gsaf_linept (float *xg, float *yg, int n, int ipat);
void gsaf_gusp2d (float *xh, float *yh, int n, 
		  float *xp, float *yp, int npm, int *np, int ispl);
void gsaf_gucspl (int n, float *x, float *y, int *ikn, int ioc, int np, float *xp, float *yp);
void gsaf_gubspl (float tp, int *itm, int n, int *ikn, int m, float *b);
void gsaf_rgbbar (float x1, float x2, float y1, float y2, float *rgb, int n, int ind);
#define SUBV void (*subv)(float *, float *, int, float *, float *, int, int *)
void gsaf_contf0 (float *z, float *x, float *y, int nxa, int nx, int ny, float *zl,
		  float *rgb, int nstep, int iprd, int indx, SUBV);
void gsaf_contg0 (float *z, float *x, float *y, int nxa, int nxmax, int nymax,
		  float *zl, float *rgb, int *iln, float *wln, int nstep, int ispl, int iprd,
		  int indx, SUBV, int *ka);

/* pers3d.c */
#define RGBFUNC void (*rgbfunc)(const float *, float *)
void gsaf_perse3d1 (float *z, int nmx, int nx, int ny, int ixy, int ind);
void gsaf_pers3d1 (float *z, int nmx, int nx, int ny, int ind, RGBFUNC);
void gsaf_pers3d2 (float *z, float *x, float *y, int nmx, int nx, int ny, int ind, RGBFUNC);
void gsaf_pers3d3 (float *z, float *r, int nmx, int nx, int ny, int ind, RGBFUNC);
void gsaf_pers3d4 (float *z, float *r, float *t, int nmx, int nx, int ny, int ind, RGBFUNC);
void gsaf_pers3d5 (float *z, float *x, float *y, int nmx, int nx, int ny, int ind, RGBFUNC);
void gsaf_pers3df (float *z, float *x, float *y, int nnod, int *ielm, int nelm, int ind, RGBFUNC);

/* quadrics.c */
void gsgl_solidsphere (float x, float y, float z, float vx, float vy, float vz,
		       float radius, int slices, int stacks);
void gsgl_wiresphere (float x, float y, float z, float vx, float vy, float vz,
		      float radius, int slices, int stacks);
void gsgl_solidtorus (float x, float y, float z, float vx, float vy, float vz,
		      float innerRadius, float outerRadius, int nsides, int rings);
void gsgl_wiretorus (float x, float y, float z, float vx, float vy, float vz,
		     float innerRadius, float outerRadius, int nsides, int rings);

/* rgbfunc.c */
void gsaf_r2g2b_17 (const float *z, float *rgb);
void gsaf_r2g2p (const float *r, float *rgb);
void gsaf_white (const float *r, float *rgb);
void gsaf_black (const float *r, float *rgb);
void gsaf_r2g2b (const float *r, float *rgb);
void gsaf_r2w2b (const float *r, float *rgb);
void gsaf_r2g2bw (const float *r, float *rgb);
void gsaf_r2y2w(const float *r, float *rgb);
void gsaf_w2g2b(const float *r, float *rgb);
void gsaf_w2r2k(const float *r, float *rgb);
void gsaf_k2b2w(const float *r, float *rgb);
void gsaf_wrkbw(const float *r, float *rgb);
void gsaf_wrgbw(const float *r, float *rgb);
void gsaf_r2g2b_gat0(const float *r, float *rgb);
void gsaf_r2w2b_wat0(const float *r, float *rgb);

/* text.c */ 
void gsgl_polygontext (void);
void gsgl_vectortext (void);
void gsgl_setfreetypenode (int node);
void gsgl_usetype1 (void);

/* unix.c */
void gsaf_gudate (int *ndy, int *ndm, int *ndd, int *nth, int *ntm, int *nts);
void gsaf_gutime (float *time);
void gsaf_gu_sleep (float t);
float gsaf_gurand (float x);
void gsaf_gu_srand (int i);
void gsaf_guflsh (void);
	
/* utils.c */
int gsgl_stdin (const char *question, int args, const char *format, ...);
/*
  return 0 : success
         EOF : Ctrl^D
         1 : error
         2 : "/"
*/

/* spln.c */
void gsaf_spln1d (float *x,float *f,float *c,int nx,float *xn,float *fn,int nxn,int ip);
void gsaf_gusplc (float *x,int n,float *y,float *df,int *iopt,float *c,int nc,int *ier);
void gsaf_gusplf (float *x,int n,float *y,float *c,int nc,float *v,int m,float *f,int *ier);
void gsaf_spln2d (float *x,float *y,float *f,float *p,float *q,float *s,float *a,
		  int im,int ni,int nj,float *xg,float *yg,float *fg,
		  int in,int nni,int nnj,int ip);
void gsaf_guspln (float *x,float *y,float *f,int ni,int nj,
		  float *p,float *q,float *s,float *a,int im,int ip );
void gsaf_guencn (float *p,float *q,float *s,float *x,float *y,float *f,int im,int ni,int nj);
void gsaf_gubnda (float *a,int *l1);
void gsaf_gucalg (float *x,float *y,float *f,float *p,float *q,float *s,float *gam,
		  int im,int i,int j);
void gsaf_gucala (float h,float *a);
void gsaf_gucalk (float *p,float *q,float *s,float *f,float *fk,int im,int i,int j);

/* matrix.c */
void gsaf_setpage (float xoffset, float yoffset, float xscale, float yscale);
void gsaf_inqpage (float *xoffset, float *yoffset, float *xscale, float *yscale);

/* displaylist.c */
/*void gsaf_clear (void);*/

/* guiitems.c */
void gsaf_inqlabelsize (const char *text, float *w, float *h);
void gsaf_guiexitloop (void);

#ifdef __cplusplus
}
#endif

#endif
