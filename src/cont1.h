#ifndef __cont1_h_
#define __cont1_h_

void gsgl_push_cont_info (float rmax, float rt, float tt, float xt, float yt);
void gsgl_pop_cont_info (float *rmax, float *rt, float *tt, float *xt, float *yt);

void gsaf_contp1_internal (float *z, int nxa, int nxmax, int nymax, 
			   float zorg, float zstep, int nstep, int iprd, int ipat, int *ka);
void gsaf_contp2_internal (float *z, float *x, float *y, int nxa, int nxmax, int nymax,
			   float zorg, float zstep, int nstep, int iprd, int ipat, int *ka);
void gsaf_contp3_internal (float *z, float *r, int nxa, int nxmax, int nymax,
			   float zorg, float zstep, int nstep, int ipat, int *ka);
void gsaf_contp4_internal (float *z, float *r, float *t, int nxa, int nxmax, int nymax,
			   float zorg, float zstep, int nstep, int ipat, int *ka);
void gsaf_contq1_internal (float *z, int nxa, int nxmax, int nymax, 
			   float zorg, float zstep, int nstep, int iprd, int ipat, int *ka);
void gsaf_contq2_internal (float *z, float *x, float *y, int nxa, int nxmax, int nymax,
			   float zorg, float zstep, int nstep, int iprd, int ipat, int *ka);
void gsaf_contq3_internal (float *z, float *r, int nxa, int nxmax, int nymax,
			   float zorg, float zstep, int nstep, int ipat, int *ka);
void gsaf_contq4_internal (float *z, float *r, float *t, int nxa, int nxmax, int nymax,
			   float zorg, float zstep, int nstep, int ipat, int *ka);
void gsaf_contpf_internal (float *z, float *x, float *y, int nnod, int *ielm, int nelm,
			   float zorg, float zstep, int nstep, int ipat, int *ka);

#endif
