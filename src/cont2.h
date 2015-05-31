#ifndef __cont2_h_
#define __cont2_h_

void gsaf_conte1_internal (float *z, int nxa, int nxmax, int nymax, float *zl, int nstep, 
			   int iprd, int ipat, int *ka);
void gsaf_conte2_internal (float *z, float *x, float *y, int nxa, int nxmax, int nymax,
			   float *zl, int nstep, int ipat, int iprd, int *ka);
void gsaf_conte3_internal (float *z, float *r, int nxa, int nxmax, int nymax,
			   float *zl, int nstep, int ipat, int *ka);
void gsaf_conte4_internal (float *z, float *r, float *t, int nxa, int nxmax, int nymax,
			   float *zl, int nstep, int ipat, int *ka);
void gsaf_contg1_internal (float *z, int nxa, int nxmax, int nymax, float *zl, float *rgb,
			   int *iln, float *wln, int nstep, int ispl, int iprd, int *ka);
void gsaf_contg2_internal (float *z, float *x, float *y, int nxa, int nxmax, int nymax, 
			   float *zl, float *rgb, int *iln, float *wln, int nstep, int ispl,
			   int iprd, int *ka);
void gsaf_contg3_internal (float *z, float *r, int nxa, int nxmax, int nymax, float *zl,
			   float *rgb, int *iln, float *wln, int nstep, int ispl, int *ka);
void gsaf_contg4_internal (float *z, float *r, float *t, int nxa, int nxmax, int nymax,
			   float *zl, float *rgb, int *iln, float *wln, int nstep, int ispl,
			   int *ka);
void gsaf_contfx (float *x, float *y, float *z, int n, float *xt, float *yt, int *jh, float *ht, int kmax);
void gsaf_contv1 (float *xa, float *ya, int n, float *xb, float *yb, int m, int *nn);
void gsaf_contv2 (float *ra, float *ta, int n, float *xb, float *yb, int m, int *nn);

#endif
