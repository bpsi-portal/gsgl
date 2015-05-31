#include "gsaf.h"
#include "gsgl.h"
#include "base.h"
#include "apl1.h"
#include "apl2.h"
#include "bsc2.h"
#include "utils.h"
#include "cont1.h"
#include "cont2.h"
#include "bsc1.h"

/* contour plot : xy, fixed step, pattern */
void gsaf_conte1_internal (float *z, int nxa, int nxmax, int nymax, float *zl, int nstep, 
			   int iprd, int ipat, int *ka)
{
	float pxs,pys,pxe,pye,gxs,gys,gxe,gye;
	int nxm, nym;
	float x[2], y[2];
	int iln[1];
	float wln[1], rgb[3];
	int i;

	gsaf_inqgdefin (&pxs, &pxe, &pys, &pye, &gxs, &gxe, &gys, &gye);

	nxm=nxmax-1;
	nym=nymax-1;
	if (iprd == 1 || iprd == 3) nxm=nxmax;
	if (iprd == 2 || iprd == 3) nym=nymax;
	x[0]=(gxe-gxs)/(float)(nxm);
	y[0]=(gye-gys)/(float)(nym);
	x[1]=gxs;
	y[1]=gys;
	iln[0]=ipat;

	for (i=0; i<3; i++) {
		rgb[i] = 0.0;
	}
	wln[0]=0.0;

	if (ka == NULL) ka = (int *) gsgl_malloc_atomic (2*nxmax*nymax*sizeof(int));

	gsaf_contg0 (z,x,y,nxa,nxmax,nymax,zl,rgb,iln,wln,nstep,-1,iprd,0,gsaf_contv1,ka);
}

void gsaf_conte1 (float *z, int nxa, int nxmax, int nymax, float *zl, int nstep, 
		  int iprd, int ipat)
{
	if (!gsgl_inq_page_is_open ()) return;
	gsaf_conte1_internal (z, nxa, nxmax, nymax, zl, nstep, iprd, ipat, NULL);
}

/* contour plot : xy, variable step, pattern */
void gsaf_conte2_internal (float *z, float *x, float *y, int nxa, int nxmax, int nymax,
			   float *zl, int nstep, int ipat, int iprd, int *ka)
{
	int i;
	float rgb[3];
	int iln[1];
	float wln[1];

	iln[0] = ipat;
	for (i=0; i<3; i++) {
		rgb[i] = 0.0;
	}
	wln[0]=0.0;

	if (ka == NULL) ka = (int *) gsgl_malloc_atomic (2*nxmax*nymax*sizeof(int));

	gsaf_contg0 (z,x,y,nxa,nxmax,nymax,zl,rgb,iln,wln,nstep,-1,iprd,3,gsaf_contv1,ka);
}

void gsaf_conte2 (float *z, float *x, float *y, int nxa, int nxmax, int nymax,
		  float *zl, int nstep, int ipat, int iprd)
{
	if (!gsgl_inq_page_is_open ()) return;
	gsaf_conte2_internal (z, x, y, nxa, nxmax, nymax, zl, nstep, ipat, iprd, NULL);
}


/* contour plot : r-theta, variable r, pattern */
void gsaf_conte3_internal (float *z, float *r, int nxa, int nxmax, int nymax,
			   float *zl, int nstep, int ipat, int *ka)
{
	float rmax,rt,tt,xt,yt;
	float t[2], rgb[3];
	int iln[1];
	float wln[1];
	int i;

	gsgl_pop_cont_info (&rmax, &rt, &tt, &xt, &yt);
	gsgl_push_cont_info (r[nxmax-1], rt, tt, xt, yt);

	t[0] = 2*3.1415926/nymax;
	t[1] = 0.0;
	iln[0]=ipat;

	for (i=0; i<3; i++) {
		rgb[i] = 0.0;
	}
	wln[0]=0.0;

	if (ka == NULL) ka = (int *) gsgl_malloc_atomic (2*nxmax*nymax*sizeof(int));

	gsaf_contg0 (z,r,t,nxa,nxmax,nymax,zl,rgb,iln,wln,nstep,-1,2,1,gsaf_contv2,ka);
}

void gsaf_conte3 (float *z, float *r, int nxa, int nxmax, int nymax,
		  float *zl, int nstep, int ipat)
{
	if (!gsgl_inq_page_is_open ()) return;
	gsaf_conte3_internal (z, r, nxa, nxmax, nymax, zl, nstep, ipat, NULL);
}


/* contour plot : r-theta, variable step, pattern */
void gsaf_conte4_internal (float *z, float *r, float *t, int nxa, int nxmax, int nymax,
			   float *zl, int nstep, int ipat, int *ka)
{
	float rmax,rt,tt,xt,yt;
	float rgb[3], wln[1];
	int iln[1];
	int i;

	gsgl_pop_cont_info (&rmax, &rt, &tt, &xt, &yt);
	gsgl_push_cont_info (r[nxmax-1], rt, tt, xt, yt);
	iln[0]=ipat;

	for (i=0; i<3; i++) {
		rgb[i] = 0.0;
	}
	wln[0]=0.0;

	if (ka == NULL) ka = (int *) gsgl_malloc_atomic (2*nxmax*nymax*sizeof(int));

	gsaf_contg0 (z,r,t,nxa,nxmax,nymax,zl,rgb,iln,wln,nstep,-1,0,3,gsaf_contv2,ka);
}

void gsaf_conte4 (float *z, float *r, float *t, int nxa, int nxmax, int nymax,
		  float *zl, int nstep, int ipat)
{
	if (!gsgl_inq_page_is_open ()) return;
	gsaf_conte4_internal (z, r, t, nxa, nxmax, nymax, zl, nstep, ipat, NULL);
}

/* contour plot : xy, fixed step, pattern */
void gsaf_contg1_internal (float *z, int nxa, int nxmax, int nymax, float *zl, float *rgb,
			   int *iln, float *wln, int nstep, int ispl, int iprd, int *ka)
{
	float pxs,pys,pxe,pye,gxs,gys,gxe,gye;
	int nxm, nym;
	float x[2], y[2];

	gsaf_inqgdefin (&pxs, &pxe, &pys, &pye, &gxs, &gxe, &gys, &gye);

	nxm=nxmax-1;
	nym=nymax-1;
	if (iprd == 1 || iprd == 3) nxm=nxmax;
	if (iprd == 2 || iprd == 3) nym=nymax;
	x[0]=(gxe-gxs)/(float)(nxm);
	y[0]=(gye-gys)/(float)(nym);
	x[1]=gxs;
	y[1]=gys;

	if (ka == NULL) ka = (int *) gsgl_malloc_atomic (2*nxmax*nymax*sizeof(int));

	gsaf_contg0 (z,x,y,nxa,nxmax,nymax,zl,rgb,iln,wln,nstep,ispl,iprd,0,gsaf_contv1,ka);
}

void gsaf_contg1 (float *z, int nxa, int nxmax, int nymax, float *zl, float *rgb,
		  int *iln, float *wln, int nstep, int ispl, int iprd)
{
	if (!gsgl_inq_page_is_open ()) return;
	gsaf_contg1_internal (z, nxa, nxmax, nymax, zl, rgb, iln, wln, nstep, ispl, iprd, NULL);
}

/* contour plot : xy, variable step, pattern */
void gsaf_contg2_internal (float *z, float *x, float *y, int nxa, int nxmax, int nymax,
			   float *zl, float *rgb, int *iln, float *wln, int nstep, int ispl,
			   int iprd, int *ka)
{
	if (ka == NULL) ka = (int *) gsgl_malloc_atomic (2*nxmax*nymax*sizeof(int));

	gsaf_contg0(z,x,y,nxa,nxmax,nymax,zl,rgb,iln,wln,nstep,ispl,iprd,3,gsaf_contv1,ka);
}

void gsaf_contg2 (float *z, float *x, float *y, int nxa, int nxmax, int nymax, float *zl,
		  float *rgb, int *iln, float *wln, int nstep, int ispl, int iprd)
{
	if (!gsgl_inq_page_is_open ()) return;
	gsaf_contg2_internal (z, x, y, nxa, nxmax, nymax, zl, rgb, iln, wln, nstep, ispl, iprd, NULL);
}

/* contour plot : r-theta, variable r, pattern */
void gsaf_contg3_internal (float *z, float *r, int nxa, int nxmax, int nymax, float *zl, 
			   float *rgb, int *iln, float *wln, int nstep, int ispl, int *ka)
{
	float t[2];
	float rmax,rt,tt,xt,yt;

	gsgl_pop_cont_info (&rmax, &rt, &tt, &xt, &yt);
	gsgl_push_cont_info (r[nxmax-1], rt, tt, xt, yt);

	t[0]=2*3.1415926/nymax;
	t[1]=0.0;

	if (ka == NULL) ka = (int *) gsgl_malloc_atomic (2*nxmax*nymax*sizeof(int));

	gsaf_contg0 (z,r,t,nxa,nxmax,nymax,zl,rgb,iln,wln,nstep,ispl,2,1,gsaf_contv2,ka);
}

void gsaf_contg3 (float *z, float *r, int nxa, int nxmax, int nymax, float *zl, 
		  float *rgb, int *iln, float *wln, int nstep, int ispl)
{
	if (!gsgl_inq_page_is_open ()) return;
	gsaf_contg3_internal (z, r, nxa, nxmax, nymax, zl, rgb, iln, wln, nstep, ispl, NULL);
}

/* contour plot : r-theta, variable step, pattern */
void gsaf_contg4_internal (float *z, float *r, float *t, int nxa, int nxmax, int nymax,
			   float *zl, float *rgb, int *iln, float *wln, int nstep, int ispl,
			   int *ka)
{
	float rmax,rt,tt,xt,yt;

	gsgl_pop_cont_info (&rmax, &rt, &tt, &xt, &yt);
	gsgl_push_cont_info (r[nxmax-1], rt, tt, xt, yt);

	if (ka == NULL) ka = (int *) gsgl_malloc_atomic (2*nxmax*nymax*sizeof(int));

	gsaf_contg0 (z,r,t,nxa,nxmax,nymax,zl,rgb,iln,wln,nstep,ispl,0,3,gsaf_contv2,ka);
}

void gsaf_contg4 (float *z, float *r, float *t, int nxa, int nxmax, int nymax,
		  float *zl, float *rgb, int *iln, float *wln, int nstep, int ispl)
{
	if (!gsgl_inq_page_is_open ()) return;
	gsaf_contg4_internal (z, r, t, nxa, nxmax, nymax, zl, rgb, iln, wln, nstep, ispl, NULL);
}

/* contour plot : common sub */
void gsaf_contg0 (float *z, float *x, float *y, int nxa, int nxmax, int nymax,
		  float *zl, float *rgb, int *iln, float *wln, int nstep, int ispl, int iprd,
		  int indx, SUBV, int *ka)
{
	/*float  z(nxa,nymax),x(nxmax),y(nymax),ka(2,nxmax*nymax)
	  float  zl(nstep),rgb(3,nstep),iln(nstep),wln(nstep)*/
#define nfmax 2000
#define ngmax 4000
	float  xf[nfmax],yf[nfmax];
	float  xp[nfmax],yp[nfmax];
	float  xg[ngmax],yg[ngmax];
	float rs, gs, bs;
	float ws;
	int kmax;
	float *zls, *rgbs, *wlns;
	int *ilns;
	int i;
	int nxm, nym, ny, nx, ie, nxp, nyp;
	float u1, u2, u3, u4, umax, umin, u0;
	int k;
	int i4x, i4y, nax, nay, nbx, nby, mode, nsax, nsay;
	float ua, ub, usa;
	int n1x, n1y, n2x, n2y, n3x, n3y, n4x, n4y;
	int i2x, i2y, i3x, i3y;
	int nsbx, nsby, modes, j, iel, nxl, nyl, model, nxl1, nyl1, ie1;
	float usb, xa, xb, ya, yb;
	gsglboolean linv, lend;
	int nf, np, nn;
	float zorg;
	float rmax,rt,tt,xt,yt;

	if (!gsgl_inq_page_is_open ()) return;
	gsgl_pop_cont_info (&rmax, &rt, &tt, &xt, &yt);

	if (ispl >= 0) {
		gsaf_inqrgb (&rs, &gs, &bs);
		gsaf_inqlnw (&ws);
	}

	kmax = nstep;
	zls = (float *) gsgl_malloc_atomic (sizeof(float)*kmax);
	rgbs = (float *) gsgl_malloc_atomic (sizeof(float)*kmax*3);
	wlns = (float *) gsgl_malloc_atomic (sizeof(float)*kmax);
	ilns = (int *) gsgl_malloc_atomic (sizeof(int)*kmax);

	if (zl[0] <= zl[kmax-1]) {
		zorg = zl[0];
		for (i=1; i<=kmax; i++) {
			zls[i-1] = zl[i-1] - zorg;
		}
	} else {
		zorg=zl[kmax-1];
		for (i=1; i<=kmax; i++) {
			zls[i-1]=zl[kmax-i+1-1]-zorg;
		}
	}
	
	if (ispl >= 0) {
		if (zl[0] <= zl[kmax-1]) {
			for (i=1; i<=kmax; i++) {
				rgbs[findex2(1-1,i-1,3)] = rgb[findex2(1-1,i-1,3)];
				rgbs[findex2(2-1,i-1,3)] = rgb[findex2(2-1,i-1,3)];
				rgbs[findex2(3-1,i-1,3)] = rgb[findex2(3-1,i-1,3)];
				ilns[i-1]=iln[i-1];
				wlns[i-1]=wln[i-1];
			}
		} else {
			for (i=1; i<=kmax; i++) {
				rgbs[findex2(1-1,i-1,3)]=rgb[findex2(1-1,kmax-i+1-1,3)];
				rgbs[findex2(2-1,i-1,3)]=rgb[findex2(2-1,kmax-i+1-1,3)];
				rgbs[findex2(3-1,i-1,3)]=rgb[findex2(3-1,kmax-i+1-1,3)];
				ilns[i-1]=iln[kmax-i+1-1];
				wlns[i-1]=wln[kmax-i+1-1];
			}
		}
	} else {
		for (i=1; i<=kmax; i++) {
			ilns[i-1]=iln[0];
		}
	}

	nxm=nxmax-1;
	nym=nymax-1;
	if (iprd == 1 || iprd == 3) nxm=nxmax;
	if (iprd == 2 || iprd == 3) nym=nymax;

	for (ny=1; ny<=nym; ny++) {
		for (nx=1; nx<=nxm; nx++) {
			ie=nxm*(ny-1)+nx;
			nxp=nx+1;
			nyp=ny+1;
			if (nxp > nxmax) nxp=1;
			if (nyp > nymax) nyp=1;
			u1=z[findex2(nx-1,ny-1,nxa)] - zorg;
			u2=z[findex2(nxp-1,ny-1,nxa)] - zorg;
			u3=z[findex2(nxp-1,nyp-1,nxa)] - zorg;
			u4=z[findex2(nx-1,nyp-1,nxa)] - zorg;
			umax=MAX(u1,MAX(u2,MAX(u3,u4)));
			umin=MIN(u1,MIN(u2,MIN(u3,u4)));
			ka[findex2(1-1,ie-1,2)]=1;
			for (k=1; k<=kmax; k++) {
				if (zls[k-1] < umin) ka[findex2(1-1,ie-1,2)]=k+1;
				if (zls[k-1] <= umax) ka[findex2(2-1,ie-1,2)]=k;
			}
		}
	}

	for (k=1; k<=kmax; k++) {
	    u0=zls[k-1];
	    if (ispl >= 0) {
		    gsaf_setrgb(rgbs[findex2(1-1,k-1,3)],
			   rgbs[findex2(2-1,k-1,3)],
			   rgbs[findex2(3-1,k-1,3)]);
		    gsaf_setlnw(wlns[k-1]);
	    }
	    for (ny=1; ny<=nym; ny++) {
		for (nx=1; nx<=nxm; nx++) {
			ie=nxm*(ny-1)+nx;
			if (ka[findex2(1-1,ie-1,2)] <= k && ka[findex2(2-1,ie-1,2)] >= k) {
			n1x=nx;
			n1y=ny;
			n2x=nx+1;
			n2y=ny;
			n3x=nx+1;
			n3y=ny+1;
			n4x=nx;
			n4y=ny+1;
			
			u1=z[findex2(n1x-1,n1y-1,nxa)]-zorg;
			i2x=n2x;
			i2y=n2y;
			if (i2x > nxmax) i2x=1;
			if (i2y > nymax) i2y=1;
			u2=z[findex2(i2x-1,i2y-1,nxa)]-zorg;
			i3x=n3x;
			i3y=n3y;
			if (i3x > nxmax) i3x=1;
			if (i3y > nymax) i3y=1;
			u3=z[findex2(i3x-1,i3y-1,nxa)]-zorg;
			i4x=n4x;
			i4y=n4y;
			if (i4x > nxmax) i4x=1;
			if (i4y > nymax) i4y=1;
			u4=z[findex2(i4x-1,i4y-1,nxa)]-zorg;
			
			if ((u1 > u0 && u2 <= u0) || 
			    (u1 <= u0 && u2 > u0)) {
				nax=n1x;;
				nay=n1y;
				ua=u1;
				nbx=n2x;
				nby=n2y;
				ub=u2;
				mode=1;
				if ((u2 > u0 && u3 <= u0) || 
				    (u2 <= u0 && u3 > u0)) {
					nsax=n2x;
					nsay=n2y;
					usa=u2;
					nsbx=n3x;
					nsby=n3y;
					usb=u3;
					modes=2;
				} else if ((u3 > u0 && u4 <= u0) || 
					    (u3 <= u0 && u4 > u0)) {
					nsax=n3x;
					nsay=n3y;
					usa=u3;
					nsbx=n4x;
					nsby=n4y;
					usb=u4;
					modes=3;
				} else if ((u4 > u0 && u1 <= u0) || 
					    (u4 <= u0 && u1 > u0)) {
					nsax=n4x;
					nsay=n4y;
					usa=u4;
					nsbx=n1x;
					nsby=n1y;
					usb=u1;
					modes=4;
				} else {
					/*goto THOUSAND;*/
					continue;
				}
			} else if ((u2 > u0 && u3 <= u0) || 
				   (u2 <= u0 && u3 > u0)) {
				nax=n2x;
				nay=n2y;
				ua=u2;
				nbx=n3x;
				nby=n3y;
				ub=u3;
				mode=2;
				if ((u3 > u0 && u4 <= u0) || 
				    (u3 <= u0 && u4 > u0)) {
					nsax=n3x;
					nsay=n3y;
					usa=u3;
					nsbx=n4x;
					nsby=n4y;
					usb=u4;
					modes=3;
				} else if ((u4 > u0 && u1 <= u0) || 
					   (u4 <= u0 && u1 > u0)) {
					nsax=n4x;
					nsay=n4y;
					usa=u4;
					nsbx=n1x;
					nsby=n1y;
					usb=u1;
					modes=4;
				} else {
					/*goto THOUSAND;*/
					continue;
				}
			} else if ((u3 > u0 && u4 <= u0) || 
				   (u3 <= u0 && u4 > u0)) {
				nax=n3x;
				nay=n3y;
				ua=u3;
				nbx=n4x;
				nby=n4y;
				ub=u4;
				mode=3;
				if ((u4 > u0 && u1 <= u0) || 
				    (u4 <= u0 && u1 > u0)) {
					nsax=n4x;
					nsay=n4y;
					usa=u4;
					nsbx=n1x;
					nsby=n1y;
					usb=u1;
					modes=4;
				} else {
					/*goto THOUSAND;*/
					continue;
				}
			} else {
				/*goto THOUSAND;*/
				continue;
			}

			if (indx == 0 || indx == 2) {
				xa=x[0]*(nax-1)+x[1];
				xb=x[0]*(nbx-1)+x[1];
			} else {
				xa=x[nax-1];
				xb=x[nbx-1];
			}
			if (indx == 0 || indx == 1) {
				ya=y[0]*(nay-1)+y[1];
				yb=y[0]*(nby-1)+y[1];
			} else {
				ya=y[nay-1];
				yb=y[nby-1];
			}
			j=1;
			rt=(u0-ua)/(ub-ua);
			gsgl_push_cont_info (rmax, rt, tt, xt, yt);

			xf[j-1]=(xb-xa)*rt+xa;
			yf[j-1]=(yb-ya)*rt+ya;

			iel=ie;
			nxl=nx;
			nyl=ny;
			model=mode;
			linv=GSGL_FALSE;
			lend=GSGL_FALSE;

			nxl1=nx;
			nyl1=ny;
			if (model == 1) nyl1=nyl-1;
			if (model == 2) nxl1=nxl+1;
			if (model == 3) nyl1=nyl+1;
			if (model == 4) nxl1=nxl-1;

			if (nxl1 >= 1 && nxl1 <= nxm && nyl1 >= 1 && nyl1 <= nym) {
				ie1=nxm*(nyl1-1)+nxl1;
			} else {
				ie1=ie;
			}
		    TWO:

			if (model == 1) nyl=nyl-1;
			if (model == 2) nxl=nxl+1;
			if (model == 3) nyl=nyl+1;
			if (model == 4) nxl=nxl-1;

			if (nxl >= 1 && nxl <= nxm && nyl >= 1 && nyl <= nym) {
				iel=nxm*(nyl-1)+nxl;
				n1x=nxl;
				n1y=nyl;
				n2x=nxl+1;
				n2y=nyl;
				n3x=nxl+1;
				n3y=nyl+1;
				n4x=nxl;
				n4y=nyl+1;
				u1=z[findex2(n1x-1,n1y-1,nxa)]-zorg;
				i2x=n2x;
				i2y=n2y;
				if (i2x > nxmax) i2x=1;
				if (i2y > nymax) i2y=1;
				u2=z[findex2(i2x-1,i2y-1,nxa)]-zorg;
				i3x=n3x;
				i3y=n3y;
				if (i3x > nxmax) i3x=1;
				if (i3y > nymax) i3y=1;
				u3=z[findex2(i3x-1,i3y-1,nxa)]-zorg;
				i4x=n4x;
				i4y=n4y;
				if (i4x > nxmax) i4x=1;
				if (i4y > nymax) i4y=1;
				u4=z[findex2(i4x-1,i4y-1,nxa)]-zorg;

				if (model == 1) {
				    if ((u4 > u0 && u1 <= u0) || (u4 <= u0 && u1 > u0)) {
					    nax=n4x;
					    nay=n4y;
					    ua=u4;
					    nbx=n1x;
					    nby=n1y;
					    ub=u1;
					    model=4;
				    } else if ((u1 > u0 && u2 <= u0) || (u1 <= u0 && u2 > u0)) {
					    nax=n1x;
					    nay=n1y;
					    ua=u1;
					    nbx=n2x;
					    nby=n2y;
					    ub=u2;
					    model=1;
				    } else if ((u2 > u0 && u3 <= u0) || (u2 <= u0 && u3 > u0)) {
					    nax=n2x;
					    nay=n2y;
					    ua=u2;
					    nbx=n3x;
					    nby=n3y;
					    ub=u3;
					    model=2;
				    } else {
					    lend=GSGL_TRUE;
				    }
				} else if (model == 2) {
				    if ((u1 > u0 && u2 <= u0) || (u1 <= u0 && u2 > u0)) {
					    nax=n1x;
					    nay=n1y;
					    ua=u1;
					    nbx=n2x;
					    nby=n2y;
					    ub=u2;
					    model=1;
				    } else if ((u2 > u0 && u3 <= u0) || (u2 <= u0 && u3 > u0)) {
					    nax=n2x;
					    nay=n2y;
					    ua=u2;
					    nbx=n3x;
					    nby=n3y;
					    ub=u3;
					    model=2;
				    } else if ((u3 > u0 && u4 <= u0) || (u3 <= u0 && u4 > u0)) {
					    nax=n3x;
					    nay=n3y;
					    ua=u3;
					    nbx=n4x;
					    nby=n4y;
					    ub=u4;
					    model=3;
				    } else {
					    lend=GSGL_TRUE;
				    }
				} else if (model == 3) {
				    if ((u2 > u0 && u3 <= u0) || (u2 <= u0 && u3 > u0)) {
					    nax=n2x;
					    nay=n2y;
					    ua=u2;
					    nbx=n3x;
					    nby=n3y;
					    ub=u3;
					    model=2;
				    } else if ((u3 > u0 && u4 <= u0) || (u3 <= u0 && u4 > u0)) {
					    nax=n3x;
					    nay=n3y;
					    ua=u3;
					    nbx=n4x;
					    nby=n4y;
					    ub=u4;
					    model=3;
				    } else if ((u4 > u0 && u1 <= u0) || (u4 <= u0 && u1 > u0)) {
					    nax=n4x;
					    nay=n4y;
					    ua=u4;
					    nbx=n1x;
					    nby=n1y;
					    ub=u1;
					    model=4;
				    } else {
					    lend=GSGL_TRUE;
				    }
				} else if (model == 4) {
				    if ((u3 > u0 && u4 <= u0) || (u3 <= u0 && u4 > u0)) {
					    nax=n3x;
					    nay=n3y;
					    ua=u3;
					    nbx=n4x;
					    nby=n4y;
					    ub=u4;
					    model=3;
				    } else if ((u4 > u0 && u1 <= u0) || (u4 <= u0 && u1 > u0)) {
					    nax=n4x;
					    nay=n4y;
					    ua=u4;
					    nbx=n1x;
					    nby=n1y;
					    ub=u1;
					    model=4;
				    } else if ((u1 > u0 && u2 <= u0) || (u1 <= u0 && u2 > u0)) {
					    nax=n1x;
					    nay=n1y;
					    ua=u1;
					    nbx=n2x;
					    nby=n2y;
					    ub=u2;
					    model=1;
				    } else {
					    lend=GSGL_TRUE;
				    }
				}
			} else {
				if (linv) {
					lend=GSGL_TRUE;
				} else {

					for (nf=nfmax; nf>=nfmax-j+1; nf--) {
						xf[nf-1]=xf[nf-nfmax+j-1];
						yf[nf-1]=yf[nf-nfmax+j-1];
					}
					j=nfmax-j+1;

					iel=ie;
					nax=nsax;
					nay=nsay;
					ua=usa;
					nbx=nsbx;
					nby=nsby;
					ub=usb;
					nxl=nx;
					nyl=ny;
					model=modes;
					linv=GSGL_TRUE;
				}
			}
			
			if (!lend) {
			    if (indx == 0 || indx == 2) {
				    xa=x[0]*(nax-1)+x[1];
				    xb=x[0]*(nbx-1)+x[1];
			    } else {
				    xa=x[nax-1];
				    xb=x[nbx-1];
			    }
			    if (indx == 0 || indx == 1) {
				    ya=y[0]*(nay-1)+y[1];
				    yb=y[0]*(nby-1)+y[1];
			    } else {
				    if (nay > nym) nay = nym;
				    if (nby > nym) nby = nym;
				    ya=y[nay-1];
				    yb=y[nby-1];
			    }
			    if (!linv) {
				    if (j == nfmax) {
					    gsaf_gusp2d (&(xf[0]), &(yf[0]), j,xp,yp,nfmax,&np,ispl);
					    subv (xp, yp, np, xg, yg, ngmax, &nn);
					    gsaf_linept (xg, yg, nn, ilns[k-1]);
					    j=1;
					    xf[0] = xf[nfmax-1];
					    yf[0] = yf[nfmax-1];
				    }
				    j=j+1;
			    } else {
			       if (j == 1) {
				   gsaf_gusp2d(&(xf[j-1]),&(yf[j-1]),nfmax-j+1,xp,yp,nfmax,&np,ispl);
				   subv (xp, yp, np, xg, yg, ngmax, &nn);
				   gsaf_linept (xg, yg, nn, ilns[k-1]);
				   j=nfmax;
				   xf[nfmax-1] = xf[0];
				   yf[nfmax-1] = yf[0];
			       }
			       j=j-1;
			    }
			    rt=(u0-ua)/(ub-ua);
			    gsgl_push_cont_info (rmax, rt, tt, xt, yt);

			    xf[j-1]=(xb-xa)*rt+xa;
			    yf[j-1]=(yb-ya)*rt+ya;

			    ka[findex2(1-1,iel-1,2)] = ka[findex2(1-1,iel-1,2)]+1;
			    if (iel == ie) lend=GSGL_TRUE;
			    /*if (iel == ie && !linv) lend=GSGL_TRUE;*/
			    /*if (iel == ie1 && !linv) lend=GSGL_TRUE;*/
			}

/*      write(6,'(a,6i5,2i7)') 'step 3: k,ny,nx,nyl,nxl,model,ie,iel',
c     &                                k,ny,nx,nyl,nxl,model,ie,iel
c      write(6,'(a,1p5e12.4)') 'u0..4: ',u0,u1,u2,u3,u4
c      guflsh*/

			if (!lend) goto TWO;

			if (!linv) {
				gsaf_gusp2d (&(xf[0]), &(yf[0]), j, xp, yp, nfmax, &np, ispl);
			} else {
				gsaf_gusp2d (&(xf[j-1]), &(yf[j-1]), nfmax-j+1,xp,yp,nfmax,&np,ispl);
			}
			subv (xp, yp, np, xg, yg, ngmax, &nn);
			gsaf_linept (xg, yg, nn, ilns[k-1]);
/*
c            if (!linv) {
c               write(6,*) j,np,nn
c            } else {
c               write(6,*) nfmax-j+1,np,nn
c            }
c            guflsh
c*/
		    }
			/*THOUSAND:;*/
		}
	    }
	}
	if (ispl >= 0) {
		gsaf_setlnw (ws);
		gsaf_setrgb (rs,gs,bs);
	}
	gsgl_push_cont_info (rmax, rt, tt, xt, yt);
}

/* contour paint : xy, fixed step */
void gsaf_contf1 (float *z, int nxa, int nxmax, int nymax, 
		  float *zl, float *rgb, int nstep, int iprd)
{
	float pxs,pys,pxe,pye,gxs,gys,gxe,gye;
	float x[2], y[2];
	int nxm, nym;

	if (!gsgl_inq_page_is_open ()) return;
	gsaf_inqgdefin (&pxs, &pxe, &pys, &pye, &gxs, &gxe, &gys, &gye);

	nxm=nxmax-1;
	nym=nymax-1;
	if (iprd == 1 || iprd == 3) nxm=nxmax;
	if (iprd == 2 || iprd == 3) nym=nymax;
	x[0]=(gxe-gxs)/(float)(nxm);
	y[0]=(gye-gys)/(float)(nym);
	x[1]=gxs;
	y[1]=gys;

	gsaf_contf0 (z, x, y, nxa, nxmax, nymax, zl, rgb, nstep, iprd, 0, gsaf_contv1);
}

/* contour paint : xy, variable step */
void gsaf_contf2 (float *z, float *x, float *y, int nxa, int nxmax, int nymax,
		  float *zl, float *rgb, int nstep, int iprd)
{
	if (!gsgl_inq_page_is_open ()) return;
	gsaf_contf0(z,x,y,nxa,nxmax,nymax,zl,rgb,nstep,iprd,3,gsaf_contv1);
}

/* contour paint : r-theta, variable r step */
void gsaf_contf3 (float *z, float *r, int nxa, int nxmax, int nymax,
		  float *zl, float *rgb, int nstep)
{
	float t[2];
	float rmax,rt,tt,xt,yt;

	if (!gsgl_inq_page_is_open ()) return;
	gsgl_pop_cont_info (&rmax, &rt, &tt, &xt, &yt);
	gsgl_push_cont_info (r[nxmax-1], rt, tt, xt, yt);

	t[0]=2*3.1415926/nymax;
	t[1]=0.0;

	gsaf_contf0(z,r,t,nxa,nxmax,nymax,zl,rgb,nstep,2,1,gsaf_contv2);
}

/* contour paint : r-theta, variable step */
void gsaf_contf4 (float *z, float *r, float *t, int nxa, int nxmax, int nymax,
		  float *zl, float *rgb, int nstep)
{
	float rmax,rt,tt,xt,yt;

	if (!gsgl_inq_page_is_open ()) return;
	gsgl_pop_cont_info (&rmax, &rt, &tt, &xt, &yt);
	gsgl_push_cont_info (r[nxmax-1], rt, tt, xt, yt);

	gsaf_contf0(z,r,t,nxa,nxmax,nymax,zl,rgb,nstep,0,3,gsaf_contv2);
}

#define RGBS(i,j) rgbs[findex2(i-1,j,3)]
#define RGB(i,j) rgb[findex2(i-1,j,3)]
#define Z(i,j) z[findex2(i-1,j-1,nxa)]
#define XYT_SIZE 8
#define XT(i,j) xt[findex2(i-1,j,XYT_SIZE)]
#define YT(i,j) yt[findex2(i-1,j,XYT_SIZE)]

/* contour paint : common sub */
void gsaf_contf0 (float *z, float *x, float *y, int nxa, int nx, int ny, float *zl,
		  float *rgb, int nstep, int iprd, int indx, SUBV)
{
	float xa[4], ya[4], za[4];
	float *xt;
	float *yt;
	int *jh;
	float *ht;
	float *rgbs;
	float xg[101], yg[101];
	float rs, gs, bs;
	int kmax;
	int j, nxmax, nymax, ih2;
	int i, jj, ii, ih, nn;

	if (!gsgl_inq_page_is_open ()) return;
	gsaf_inqrgb (&rs,&gs,&bs);

	kmax=nstep;
	/*
	  NH = 101
	  float xt[6*(NH+1)];
	  float yt[6*(NH+1)];
	  int jh[NH+1];
	  float ht[NH+2];
	  float rgbs[3*(NH+1)];
	*/
	xt = (float *) gsgl_malloc_atomic (XYT_SIZE*(kmax+1)*sizeof(float));
	yt = (float *) gsgl_malloc_atomic (XYT_SIZE*(kmax+1)*sizeof(float));
	jh = (int *) gsgl_malloc_atomic ((kmax+1)*sizeof(int));
	ht = (float *) gsgl_malloc_atomic ((kmax+2)*sizeof(float));
	rgbs = (float *) gsgl_malloc_atomic (3*(kmax+1)*sizeof(float));

	if (zl[0] <= zl[kmax-1]) {
		RGBS(1,0) = RGB(1,0);
		RGBS(2,0) = RGB(2,0);
		RGBS(3,0) = RGB(3,0);
		for (j=1; j<=kmax; j++) {
			ht[j]=zl[j-1];
			RGBS(1,j)=RGB(1,j);
			RGBS(2,j)=RGB(2,j);
			RGBS(3,j)=RGB(3,j);
		}
	} else {
		RGBS(1,0)=RGB(1,kmax);
		RGBS(2,0)=RGB(2,kmax);
		RGBS(3,0)=RGB(3,kmax);
		for (j=1; j<=kmax; j++) {
			ht[j]=zl[kmax-j+1-1];
			RGBS(1,j)=RGB(1,kmax-j+1);
			RGBS(2,j)=RGB(2,kmax-j+1);
			RGBS(3,j)=RGB(3,kmax-j+1);
		}
	}

	ht[0]=-1.e35;
	ht[kmax+1]=1.e35;

	nxmax=nx-1;
	nymax=ny-1;
	if (iprd == 1 || iprd == 3) nxmax=nx;
	if (iprd == 2 || iprd == 3) nymax=ny;

	/* made an easy change to reduce the number of gsgl displaylists */

	for (ih2=0; ih2<=kmax; ih2++) {
		gsaf_setrgb (RGBS(1,ih2), RGBS(2,ih2), RGBS(3,ih2));
		for (j=1; j<=nymax; j++) {
			for (i=1; i<=nxmax; i++) {
				if (indx == 0 || indx == 2) {
					xa[0]=x[0]*(i-1)+x[1];
					xa[1]=x[0]* i   +x[1];
				} else {
					xa[0]=x[i-1];
					xa[1]=x[i+1-1];
				}
				xa[2]=xa[1];
				xa[3]=xa[0];
				if (indx == 0 || indx == 1) {
					ya[0]=y[0]*(j-1)+y[1];
					ya[2]=y[0]* j   +y[1];
				} else {
					ya[0]=y[j-1];
					ya[2]=y[j];
				}
				ya[1]=ya[0];
				ya[3]=ya[2];

				jj=j+1;
				if (j == ny) jj=1;
				ii=i+1;
				if (i == nx) ii=1;

				za[0] = Z(i,j);
				za[1] = Z(ii,j);
				za[2] = Z(ii,jj);
				za[3] = Z(i,jj);

				gsaf_contfx (xa,ya,za,4,xt,yt,jh,ht,kmax);

				for (ih=0; ih<=kmax; ih++) {
				    if (jh[ih] >= 3 && ih == ih2) {
					subv(&(XT(1,ih)),&(YT(1,ih)),jh[ih],xg,yg,100,&nn);
					xg[nn+1-1] = xg[0];
					yg[nn+1-1] = yg[0];
					gsaf_poly (xg, yg, nn+1);
				    }
				}
			}
		}
	}
	gsaf_setrgb (rs, gs, bs);
}

/* contour paint : slave routine */
void gsaf_contfx (float *x, float *y, float *z, int n, float *xt, float *yt, int *jh, float *ht, int kmax)
{
	/*float  x(n),y(n),z(n)
	  float  xt(6,0:kmax),yt(6,0:kmax),jh(0:kmax),ht(0:kmax+1)*/
	int ih, j, jl, ihs, ihl, ihv;
	float xp, yp;
	float rt;

	for (ih=0; ih<=kmax; ih++) {
		jh[ih] = 0;
	}

	for (j=1; j<=n; j++) {

		if (j != n) {
			jl=j+1;
		} else {
			jl=1;
		}

		if (z[j-1] < z[jl-1]) {
			for (ih=0; ih<=kmax; ih++) {
				if ((z[j-1] >= ht[ih]) && (z[j-1] < ht[ih+1])) {
					ihs=ih;
					jh[ihs]=jh[ihs]+1;
					XT(jh[ihs],ihs) = x[j-1];
					YT(jh[ihs],ihs) = y[j-1];
				} else if (z[j-1] == ht[ih]) {
					ihs=ih-1;
					jh[ihs]=jh[ihs]+1;
					XT(jh[ihs],ihs) = x[j-1];
					YT(jh[ihs],ihs) = y[j-1];
					ihs=ih;
					ihl=ih;
					jh[ihl]=jh[ihl]+1;
					XT(jh[ihl],ihl) = x[j-1];
					YT(jh[ihl],ihl) = y[j-1];
				}
				if ((z[jl-1] >= ht[ih]) && (z[jl-1] < ht[ih+1])) {
					ihl=ih;
				} else if (z[jl-1] == ht[ih]) {
					ihl=ih-1;
				}
			}

			for (ih=ihs+1; ih<=ihl; ih++) {
				rt=(ht[ih]-z[j-1])/(z[jl-1]-z[j-1]);

				xp=x[j-1]+(x[jl-1]-x[j-1])*rt;
				yp=y[j-1]+(y[jl-1]-y[j-1])*rt;
				for (ihv=ih-1; ihv<=ih; ihv++) {
					jh[ihv] = jh[ihv]+1;
					XT(jh[ihv],ihv) = xp;
					YT(jh[ihv],ihv) = yp;
				}
			}

		} else if (z[j-1] == z[jl-1]) {

			for (ih=0; ih<=kmax; ih++) {
				if ((z[j-1] >= ht[ih]) && (z[j-1] < ht[ih+1])) {
					jh[ih]=jh[ih]+1;
					XT(jh[ih],ih) = x[j-1];
					YT(jh[ih],ih) = y[j-1];
				}
			}

		} else {

			for (ih=kmax; ih>=0; ih--) {
				if ((z[j-1] >= ht[ih]) && (z[j-1] < ht[ih+1])) {
					ihl=ih;
					jh[ihl]=jh[ihl]+1;
					XT(jh[ihl],ihl) = x[j-1];
					YT(jh[ihl],ihl) = y[j-1];
				} else if (z[j-1] == ht[ih]) {
					ihl=ih;
					jh[ihl]=jh[ihl]+1;
					XT(jh[ihl],ihl) = x[j-1];
					YT(jh[ihl],ihl) = y[j-1];
					ihl=ih-1;
					ihs=ih-1;
					jh[ihs]=jh[ihs]+1;
					XT(jh[ihs],ihs) = x[j-1];
					YT(jh[ihs],ihs) = y[j-1];
				}
				if ((z[jl-1] >= ht[ih]) && (z[jl-1] < ht[ih+1])) {
					ihs=ih;
				} else if (z[jl-1] == ht[ih]) {
					ihs=ih;
				}
			}

			for (ih=ihl; ih>=ihs+1; ih--) {
				rt = (ht[ih]-z[jl-1])/(z[j-1]-z[jl-1]);
				xp = x[jl-1]+(x[j-1]-x[jl-1])*rt;
				yp = y[jl-1]+(y[j-1]-y[jl-1])*rt;
				for (ihv=ih; ihv>=ih-1; ihv--) {
					jh[ihv]=jh[ihv]+1;
					XT(jh[ihv],ihv) = xp;
					YT(jh[ihv],ihv) = yp;
				}
			}
		}
	}
}

/* contour plot slave routine : conv xy */
void gsaf_contv1 (float *xa, float *ya, int n, float *xb, float *yb, int m, int *nn)
{
	float dx,dy,pxs,pys,pxe,pye,gxs,gys,gxe,gye;
	int i;

	gsaf_inqgdefin (&pxs, &pxe, &pys, &pye, &gxs, &gxe, &gys, &gye);
	dx=(pxe-pxs)/(gxe-gxs);
        dy=(pye-pys)/(gye-gys);

	for (i=0; i<n; i++) {
		xb[i] = dx*(xa[i]-gxs)+pxs;
		yb[i] = dy*(ya[i]-gys)+pys;
	}
	*nn=n;
}

/* contour plot slave routine : conv rt */
void gsaf_contv2 (float *ra, float *ta, int n, float *xb, float *yb, int m, int *nn)
{
	float rmax,rt,tt,xt,yt;
	/*float  ra(n),ta(n),xb(m),yb(m)*/
	float dx,dy,pxs,pys,pxe,pye,gxs,gys,gxe,gye;
	int j, i, imax, k;
	float rs, ts, xs, ys, delr, delt, ri, ti, xi, yi;
	float x0, y0, s;

	gsaf_inqgdefin (&pxs, &pxe, &pys, &pye, &gxs, &gxe, &gys, &gye);
	dx=(pxe-pxs)/(gxe-gxs);
        dy=(pye-pys)/(gye-gys);

	gsgl_pop_cont_info (&rmax, &rt, &tt, &xt, &yt);
	rt=ra[0];
	tt=ta[0];
	xt=dx*(rt*cos(tt)-gxs)+pxs;
	yt=dy*(rt*sin(tt)-gys)+pys;

	j=1;
	xb[j-1]=xt;
	yb[j-1]=yt;

	for (i=2; i<=n; i++) {
		rs=ra[i-1];
		ts=ta[i-1];
		xs=dx*(rs*cos(ts)-gxs)+pxs;
		ys=dy*(rs*sin(ts)-gys)+pys;

		imax=(int)(sqrt((xs-xt)*(xs-xt)+(ys-yt)*(ys-yt))*8/rmax+1);
		delr=(rs-rt)/imax;
		delt=ts-tt;
		if (delt >  4.0) delt=delt-2*3.1415926;
		if (delt < -4.0) delt=delt+2*3.1415926;
		delt=delt/imax;
		for (k=1; k<=imax; k++) {
			ri=delr*k+rt;
			ti=delt*k+tt;
			xi=dx*(ri*cos(ti)-gxs)+pxs;
			yi=dy*(ri*sin(ti)-gys)+pys;
			if (j >= m) {
				/*for (n=1; n<=m/2; n++) {
				  xb[n-1]=xb[2*n-1-1];
				  yb[n-1]=yb[2*n-1-1];
				  }
				  j=m/2;*/
				gsgl_warning("contv2", "buffer over");
				*nn=j;
				return;
			}
			j=j+1;
			xb[j-1]=xi;
			yb[j-1]=yi;
		}
		rt=rs;
		tt=ts;
		xt=xs;
		yt=ys;
	}
	*nn=j;

        /***** slightly expand the region *****/

	x0=0.0;
	y0=0.0;
	for (j=1; j<=(*nn); j++) {
		x0 += xb[j-1];
		y0 += yb[j-1];
	}
	x0 /= (*nn);
	y0 /= (*nn);
	for (j=1; j<=(*nn); j++) {
		s = sqrt(pow(xb[j-1]-x0, 2.0) + pow(yb[j-1]-y0, 2.0));
		if (s == 0.0) s = 1.0;
		xb[j-1] = xb[j-1] + 0.02 * (xb[j-1]-x0)/s;
		yb[j-1] = yb[j-1] + 0.02 * (yb[j-1]-y0)/s;
	}

	gsgl_push_cont_info (rmax, rt, tt, xt, yt);
}

/* draw lines with pattern */
void gsaf_linept (float *xg, float *yg, int n, int ipat)
{
	int i;

	if (!gsgl_inq_page_is_open ()) return;

	gsaf_movept (xg[0],yg[0],ipat);
	for (i=1; i<n; i++) {
		gsaf_drawpt (xg[i], yg[i]);
	}
}

/* spline interpolation of 2d lines */
void gsaf_gusp2d (float *xh, float *yh, int n, 
		  float *xp, float *yp, int npm, int *np, int ispl)
{
/*#define npa 2001*/
#define M 3
	/*float  xh(n),yh(n),xp(npm),yp(npm)*/
	int *ikn;
	int nq, ioc, i;

	if (!gsgl_inq_page_is_open ()) return;

	ikn = (int*) gsgl_malloc_atomic((npm+2*M+1)*sizeof(int));

	nq=npm;
	/*if (nq > npa) nq=npa;*/

	if ((fabs(xh[n-1]-xh[0]) < 1.e-30) && (fabs(yh[n-1]-yh[0]) < 1.e-30)) {
		ioc=1;
	} else {
		ioc=0;
	}

	if ((ispl <= 0) || ((n == 2) && (ioc == 0))) {
		for (i=1; i<=n; i++) {
			xp[i-1]=xh[i-1];
			yp[i-1]=yh[i-1];
		}
		*np=n;
	} else if (n > 2) {
		*np = ispl * n;
		if (*np > nq) *np=nq;

		if (ioc == 0) {
			for (i=0-M; i<=0; i++) {
				ikn[i+M]=0;
			}

			for (i=1; i<=n-1; i++) {
				ikn[i+M]=i;
			}

			for (i=n; i<=n+M-1; i++) {
				ikn[i+M]=n-1;
			}
		} else if (ioc == 1) {
			for (i=0-M; i<=n+M-1; i++) {
				ikn[i+M]=i;
			}
		}
		gsaf_gucspl(n-1,xh,yh,ikn,ioc,*np,xp,yp);
	}
}

#define M 3
#define B(i,j) b[findex2(i+M,j,2*M+1)]

/* spline */
void gsaf_gucspl (int n, float *x, float *y, int *ikn, int ioc, int np, float *xp, float *yp)
{
	int j, itm, iv, i;
	float tp, xv, yv;
	/*float  ikn(0-m:n+m),
	  float  x(0:n),y(0:n),xp(np),yp(np)*/
	float b[(2*M+1)*(M+1)];
	float h;

	if (!gsgl_inq_page_is_open ()) return;

	h=(ikn[n+M]-ikn[0+M])/(float)(np-1);

	for (j=1; j<=np; j++) {
		tp=ikn[0+M]+h*(j-1);
		gsaf_gubspl(tp,&itm,n,ikn,M,b);
		xv=0;
		yv=0;
		for (i=0; i>=0-M; i--) {
			iv=itm+i+2;
			if (ioc == 1) {
				if (iv < 0) {
					iv=iv+n;
				} else if (iv > n) {
					iv=iv-n;
				}
			}
			xv+=x[ikn[iv+M]] * B(i,M);
			yv+=y[ikn[iv+M]] * B(i,M);
		}
		xp[j-1]=xv;
		yp[j-1]=yv;
	}
}

/* b spline */
void gsaf_gubspl (float tp, int *itm, int n, int *ikn, int m, float *b)
{
	/*float  ikn(0-m:n+m),b(0-m:m,0:m)*/
	int jt, k, i, iv;
	float b0, b1;

	if (!gsgl_inq_page_is_open ()) return;

	for (jt=n-1; jt>=0; jt--) {
		if (tp >= (float)(ikn[jt+m])) {
			*itm=jt;
			break;
		}
	}
	
	for (k=0; k<=m-1; k++) {
		for (i=-1-k; i<=m-k; i++) {
			B(i,k) = 0.0;
		}
	}

	B(0,0) = 1.0;
	for (k=1; k<=m; k++) {
		for (i=0-k; i<=0; i++) {
			iv=(*itm)+i;
			if (ikn[iv+k+m] > ikn[iv+m]) {
				b0=(tp-ikn[iv+m])/(ikn[iv+k+m]-ikn[iv+m]+1.e-30)
					* B(i,k-1);
			} else {
				b0=0;
			}

			if (ikn[iv+k+1+m] > ikn[iv+1+m]) {
				b1=(ikn[iv+k+1+m]-tp)/(ikn[iv+k+1+m]-ikn[iv+1+m]+1.e-30)
					* B(i+1,k-1);
			} else {
				b1=0;
			}
			B(i,k) = b0+b1;
		}
	}
}

#undef RGB
#define RGB(i,j) rgb[findex2(i-1,j-1,3)]

/* rgb color patern */
void gsaf_rgbbar (float x1, float x2, float y1, float y2, float *rgb, int n, int ind)
{
	/*float rgb(3,n)*/
	float x[5],y[5];
	float dx, dy, dxl, dyl;
	int i;
	gsglPage *p = gsgl_inq_page ();

	if (!gsgl_inq_page_is_open ()) return;
	gsgl_separate_objects (p);
     
	if (ind == 0) {
		dx=(x2-x1)/n;
		dy=0.0;
		dxl=0.0;
		dyl=y2-y1;
	} else {
		dx=0.0;
		dy=(y2-y1)/n;
		dxl=x2-x1;
		dyl=0.0;
	}
	for (i=1; i<=n; i++) {
		x[0]=x1+dx*(i-1);
		y[0]=y1+dy*(i-1);
		x[1]=x1+dx*i+dxl;
		y[1]=y[0];
		x[2]=x[1];
		y[2]=y1+dy*i+dyl;
		x[3]=x[0];
		y[3]=y[2];
		x[4]=x[0];
		y[4]=y[0];
		gsaf_setrgb (RGB(1,i), RGB(2,i), RGB(3,i));
		gsaf_poly (x,y,5);
	}

	x[0]=x1;
	y[0]=y1;
	x[1]=x2;
	y[1]=y1;
	x[2]=x2;
	y[2]=y2;
	x[3]=x1;
	y[3]=y2;
	x[4]=x1;
	y[4]=y1;
	gsaf_setrgb(-1.0,0.0,0.0);
	gsaf_lines (x,y,5);
	
	gsgl_separate_objects (p);
}
