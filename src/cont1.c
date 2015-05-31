#include "gsaf.h"
#include "gsgl.h"
#include "apl1.h"
#include "apl2.h"
#include "utils.h"
#include "bsc1.h"

/*#define SUB void (*sub) (int,int,int,int,float,float,float,float *,float *,int,int)
  
static void conts1 (int nax, int nay, int nbx, int nby, float u0, float ua, float ub,
float *x, float *y, int ipat, int ind);
static void conts2 (int nax, int nay, int nbx, int nby, float u0, float ua, float ub,
float *x, float *y, int ipat, int ind);
static void conts3 (int nax, int nay, int nbx, int nby, float u0, float ua, float ub,
float *r, float *t, int ipat, int ind);
static void conts4 (int nax, int nay, int nbx, int nby, float u0, float ua, float ub,
float *r, float *t, int ipat, int ind);*/

/*#define SUB void (*sub) (int *,int *,int *,int *,float *,float *,float *,float *,float *,int *,int *)*/

static void conts1 (int *nax, int *nay, int *nbx, int *nby, float *u0, float *ua, float *ub,
		    float *x, float *y, int *ipat, int *ind);
static void conts2 (int *nax, int *nay, int *nbx, int *nby, float *u0, float *ua, float *ub,
		    float *x, float *y, int *ipat, int *ind);
static void conts3 (int *nax, int *nay, int *nbx, int *nby, float *u0, float *ua, float *ub,
		    float *r, float *t, int *ipat, int *ind);
static void conts4 (int *nax, int *nay, int *nbx, int *nby, float *u0, float *ua, float *ub,
		    float *r, float *t, int *ipat, int *ind);

static float Rmax,Rt,Tt,Xt,Yt;

void gsgl_push_cont_info (float rmax, float rt, float tt, float xt, float yt)
{
	Rmax = rmax;
	Rt = rt;
	Tt = tt;
	Xt = xt;
	Yt = yt;
}

void gsgl_pop_cont_info (float *rmax, float *rt, float *tt, float *xt, float *yt)
{
	*rmax = Rmax;
	*rt = Rt;
	*tt = Tt;
	*xt = Xt;
	*yt = Yt;
}


/* contour plot : xy, fixed step, pattern */
void gsaf_contr1 (float *z, int nxa, int nxmax, int nymax, 
		  float zorg, float zstep, int nstep, int iprd)
{
	float pxs,pys,pxe,pye,gxs,gys,gxe,gye;
	int nxm, nym;
	float delx, dely;
	const float eps = 1.e-32;
	gsglPage *p = gsgl_inq_page ();

	if (!gsgl_inq_page_is_open ()) return;
	if (!gsgl_inq_gdefin_is_called (p) || fabs(zstep) < eps) return;
	nxm = nxmax-1;
	nym = nymax-1;
	if (iprd == 1 || iprd == 3) nxm = nxmax;
	if (iprd == 2 || iprd == 3) nym = nymax;

	gsaf_inqgdefin (&pxs, &pxe, &pys, &pye, &gxs, &gxe, &gys, &gye);

	delx = (pxe-pxs)/(float)(nxm);
	dely = (pye-pys)/(float)(nym);

	gsaf_contr0 (z,&delx,&dely,nxa,nxmax,nymax,zorg,zstep,nstep,iprd,conts1);
}

/* contour plot : xy, variable step, pattern */
void gsaf_contr2 (float *z, float *x, float *y, int nxa, int nxmax, int nymax,
		  float zorg, float zstep, int nstep, int iprd)
{
	const float eps = 1.e-32;
	gsglPage *p = gsgl_inq_page ();

	if (!gsgl_inq_page_is_open ()) return;
	if (!gsgl_inq_gdefin_is_called (p) || fabs(zstep) < eps) return;

	gsaf_contr0 (z,x,y,nxa,nxmax,nymax,zorg,zstep,nstep,iprd,conts2);
}

/* contour plot : r-theta, variable step, pattern */
void gsaf_contr3 (float *z, float *r, int nxa, int nxmax, int nymax, 
		  float zorg, float zstep, int nstep)
{
	const float eps = 1.e-32;
	float delt;
	gsglPage *p = gsgl_inq_page ();
	
	if (!gsgl_inq_page_is_open ()) return;
	if (!gsgl_inq_gdefin_is_called (p) || fabs(zstep) < eps) return;

	Rmax = r[nxmax-1];
	delt = 2 * 3.1415926 / nymax;

	gsaf_contr0 (z,r,&delt,nxa,nxmax,nymax,zorg,zstep,nstep,2,conts3);
}

/* contour plot : r-theta, variable step, pattern */
void gsaf_contr4 (float *z, float *r, float *t, int nxa, int nxmax, int nymax,
		  float zorg, float zstep, int nstep)
{
	const float eps = 1.e-32;
	gsglPage *p = gsgl_inq_page ();

	if (!gsgl_inq_page_is_open ()) return;
	if (!gsgl_inq_gdefin_is_called (p) || fabs(zstep) < eps) return;

	Rmax = r[nxmax-1];

	gsaf_contr0 (z,r,t,nxa,nxmax,nymax,zorg,zstep,nstep,0,conts4);
}

/* contour plot : xy, fixed step, pattern */
void gsaf_contp1_internal (float *z, int nxa, int nxmax, int nymax, 
			   float zorg, float zstep, int nstep, int iprd, int ipat, int *ka)
{
	float pxs,pys,pxe,pye,gxs,gys,gxe,gye;
	int nxm, nym;
	float delx, dely;
	const float eps = 1.e-32;
	gsglPage *p = gsgl_inq_page ();

	if (!gsgl_inq_gdefin_is_called (p) || fabs(zstep) < eps) return;
	
	nxm = nxmax-1;
	nym = nymax-1;
	
	if (iprd == 1 || iprd == 3) nxm = nxmax;
	if (iprd == 2 || iprd == 3) nym = nymax;

	gsaf_inqgdefin (&pxs, &pxe, &pys, &pye, &gxs, &gxe, &gys, &gye);
	delx = (pxe-pxs)/(float)(nxm);
	dely = (pye-pys)/(float)(nym);

	if (ka == NULL) ka = (int *) gsgl_malloc_atomic (nxmax*nymax*4*sizeof(int));

	gsaf_contp0 (z,&delx,&dely,nxa,nxmax,nymax,zorg,zstep,nstep,iprd,ipat,ka,conts1);
}

void gsaf_contp1 (float *z, int nxa, int nxmax, int nymax, 
		  float zorg, float zstep, int nstep, int iprd, int ipat)
{
	if (!gsgl_inq_page_is_open ()) return;
	gsaf_contp1_internal (z, nxa, nxmax, nymax, zorg, zstep, nstep, iprd, ipat, NULL);
}

/* contour plot : xy, variable step, pattern */
void gsaf_contp2_internal (float *z, float *x, float *y, int nxa, int nxmax, int nymax,
			   float zorg, float zstep, int nstep, int iprd, int ipat, int *ka)
{
	const float eps = 1.e-32;
	gsglPage *p = gsgl_inq_page ();

	if (!gsgl_inq_gdefin_is_called (p) || fabs(zstep) < eps) return;

	if (ka == NULL) ka = (int *) gsgl_malloc_atomic (nxmax*nymax*4*sizeof(int));

	gsaf_contp0 (z,x,y,nxa,nxmax,nymax,zorg,zstep,nstep,iprd,ipat,ka,conts2);
}

void gsaf_contp2 (float *z, float *x, float *y, int nxa, int nxmax, int nymax,
		  float zorg, float zstep, int nstep, int iprd, int ipat)
{
	if (!gsgl_inq_page_is_open ()) return;
	gsaf_contp2_internal (z, x, y, nxa, nxmax, nymax, zorg, zstep, nstep, iprd, ipat, NULL);
}

/* contour plot : r-theta, variable step, pattern */
void gsaf_contp3_internal (float *z, float *r, int nxa, int nxmax, int nymax,
			   float zorg, float zstep, int nstep, int ipat, int *ka)
{
	float delt;
	const float eps = 1.e-32;
	gsglPage *p = gsgl_inq_page ();

	if (!gsgl_inq_gdefin_is_called (p) || fabs(zstep) < eps) return;

	Rmax = r[nxmax-1];
	delt=2*3.1415926/nymax;

	if (ka == NULL) ka = (int *) gsgl_malloc_atomic (nxmax*nymax*4*sizeof(int));

	gsaf_contp0 (z,r,&delt,nxa,nxmax,nymax,zorg,zstep,nstep,2,ipat,ka,conts3);
}

void gsaf_contp3 (float *z, float *r, int nxa, int nxmax, int nymax,
		  float zorg, float zstep, int nstep, int ipat)
{	
	if (!gsgl_inq_page_is_open ()) return;
	gsaf_contp3_internal (z, r, nxa, nxmax, nymax, zorg, zstep, nstep, ipat, NULL);
}


/* contour plot : r-theta, variable step, pattern */
void gsaf_contp4_internal (float *z, float *r, float *t, int nxa, int nxmax, int nymax,
			   float zorg, float zstep, int nstep, int ipat, int *ka)
{
	const float eps = 1.e-32;
	gsglPage *p = gsgl_inq_page ();

	if (!gsgl_inq_gdefin_is_called (p) || fabs(zstep) < eps) return;

	Rmax = r[nxmax-1];

	if (ka == NULL) ka = (int *) gsgl_malloc_atomic (nxmax*nymax*4*sizeof(int));

	gsaf_contp0 (z,r,t,nxa,nxmax,nymax,zorg,zstep,nstep,0,ipat,ka,conts4);
}

void gsaf_contp4 (float *z, float *r, float *t, int nxa, int nxmax, int nymax,
		  float zorg, float zstep, int nstep, int ipat)
{
	if (!gsgl_inq_page_is_open ()) return;
	gsaf_contp4_internal (z, r, t, nxa, nxmax, nymax, zorg, zstep, nstep, ipat, NULL);
}

/* contour plot : xy, fixed step, pattern */
void gsaf_contq1_internal (float *z, int nxa, int nxmax, int nymax, 
			   float zorg, float zstep, int nstep, int iprd, int ipat, int *ka)
{
	float pxs,pys,pxe,pye,gxs,gys,gxe,gye;
	float delx, dely;
	int nxm, nym;
	const float eps = 1.e-32;
	gsglPage *p = gsgl_inq_page ();

	if (!gsgl_inq_gdefin_is_called (p) || fabs(zstep) < eps) return;
	nxm=nxmax-1;
	nym=nymax-1;
	if (iprd == 1 || iprd == 3) nxm=nxmax;
	if (iprd == 2 || iprd == 3) nym=nymax;

	gsaf_inqgdefin (&pxs, &pxe, &pys, &pye, &gxs, &gxe, &gys, &gye);
	delx=(pxe-pxs)/(float) (nxm);
	dely=(pye-pys)/(float) (nym);

	if (ka == NULL) ka = (int *) gsgl_malloc_atomic (nxmax*nymax*8*sizeof(int));

	gsaf_contq0(z,&delx,&dely,nxa,nxmax,nymax,zorg,zstep,nstep,iprd,ipat,ka,conts1);
}

void gsaf_contq1 (float *z, int nxa, int nxmax, int nymax, 
		  float zorg, float zstep, int nstep, int iprd, int ipat)
{
	if (!gsgl_inq_page_is_open ()) return;
	gsaf_contq1_internal (z, nxa, nxmax, nymax, zorg, zstep, nstep, iprd, ipat, NULL);
}


/* contour plot : xy, variable step, pattern */
void gsaf_contq2_internal (float *z, float *x, float *y, int nxa, int nxmax, int nymax,
			   float zorg, float zstep, int nstep, int iprd, int ipat, int *ka)
{
	const float eps = 1.e-32;
	gsglPage *p = gsgl_inq_page ();

	if (!gsgl_inq_page_is_open ()) return;
	if (!gsgl_inq_gdefin_is_called (p) || fabs(zstep) < eps) return;

	if (ka == NULL) ka = (int *) gsgl_malloc_atomic (nxmax*nymax*8*sizeof(int));

	gsaf_contq0 (z,x,y,nxa,nxmax,nymax,zorg,zstep,nstep,iprd,ipat,ka,conts2);
}

void gsaf_contq2 (float *z, float *x, float *y, int nxa, int nxmax, int nymax,
		  float zorg, float zstep, int nstep, int iprd, int ipat)
{
	if (!gsgl_inq_page_is_open ()) return;
	gsaf_contq2_internal (z, x, y, nxa, nxmax, nymax, zorg, zstep, nstep, iprd, ipat, NULL);
}

/* contour plot : r-theta, variable step, pattern */
void gsaf_contq3_internal (float *z, float *r, int nxa, int nxmax, int nymax,
			   float zorg, float zstep, int nstep, int ipat, int *ka)
{
	float delt;
	const float eps = 1.e-32;
	gsglPage *p = gsgl_inq_page ();

	if (!gsgl_inq_gdefin_is_called (p) || fabs(zstep) < eps) return;

	Rmax = r[nxmax-1];
	delt = 2*3.1415926/nymax;

	if (ka == NULL) ka = (int *) gsgl_malloc_atomic (nxmax*nymax*8*sizeof(int));

	gsaf_contq0(z,r,&delt,nxa,nxmax,nymax,zorg,zstep,nstep,2,ipat,ka,conts3);
}

void gsaf_contq3 (float *z, float *r, int nxa, int nxmax, int nymax,
		  float zorg, float zstep, int nstep, int ipat)
{
	if (!gsgl_inq_page_is_open ()) return;
	gsaf_contq3_internal (z, r, nxa, nxmax, nymax, zorg, zstep, nstep, ipat, NULL);
}

/* contour plot : r-theta, variable step, pattern */
void gsaf_contq4_internal (float *z, float *r, float *t, int nxa, int nxmax, int nymax,
			   float zorg, float zstep, int nstep, int ipat, int *ka)
{
	const float eps = 1.e-32;
	gsglPage *p = gsgl_inq_page ();

	if (!gsgl_inq_gdefin_is_called (p) || fabs(zstep) < eps) return;

	Rmax=r[nxmax-1];

	if (ka == NULL) ka = (int *) gsgl_malloc_atomic (nxmax*nymax*8*sizeof(int));

	gsaf_contq0(z,r,t,nxa,nxmax,nymax,zorg,zstep,nstep,0,ipat,ka,conts4);
}

void gsaf_contq4 (float *z, float *r, float *t, int nxa, int nxmax, int nymax,
		  float zorg, float zstep, int nstep, int ipat)
{
	if (!gsgl_inq_page_is_open ()) return;
	gsaf_contq4_internal (z, r, t, nxa, nxmax, nymax, zorg, zstep, nstep, ipat, NULL);
}


/* contour plot slave routine : xy fixed */
/*static void conts1 (int nax, int nay, int nbx, int nby, float u0, float ua, float ub,
  float *x, float *y, int ipat, int ind)*/
static void conts1 (int *nax1, int *nay1, int *nbx1, int *nby1, float *u01, float *ua1,
		    float *ub1, float *x, float *y, int *ipat1, int *ind1)
{
	float pxs,pys,pxe,pye,gxs,gys,gxe,gye;
	float xa, ya, xb, yb, xs, ys;
	int nax, nay, nbx, nby, ipat, ind;
	float u0, ua, ub;

	nax = *nax1;
	nay = *nay1;
	nbx = *nbx1;
	nby = *nby1;
	ipat = *ipat1;
	ind = *ind1;
	u0 = *u01;
	ua = *ua1;
	ub = *ub1;

	gsaf_inqgdefin (&pxs, &pxe, &pys, &pye, &gxs, &gxe, &gys, &gye);
	
	if (nax > 0) {
		xa=x[0]*(nax-1)+pxs;
		ya=y[0]*(nay-1)+pys;
	} else {
		xa=x[0]*(-nax-0.5)+pxs;
		ya=y[0]*(-nay-0.5)+pys;
	}
	if (nbx > 0) {
		xb=x[0]*(nbx-1)+pxs;
		yb=y[0]*(nby-1)+pys;
	} else {
		xb=x[0]*(-nbx-0.5)+pxs;
		yb=y[0]*(-nby-0.5)+pys;
	}
	xs=(xb-xa)*(u0-ua)/(ub-ua)+xa;
	ys=(yb-ya)*(u0-ua)/(ub-ua)+ya;

	if (ind == 1) {
		gsaf_movept(xs,ys,ipat);
	} else if (ind == -1) {
		gsaf_movept(xs,ys,-ipat);
	} else if (ind == 0) {
		gsaf_drawpt(xs,ys);
	}
}

/* contour plot slave routine : xy variable */
/*static void conts2 (int nax, int nay, int nbx, int nby, float u0, float ua, float ub,
  float *x, float *y, int ipat, int ind)*/
static void conts2 (int *nax1, int *nay1, int *nbx1, int *nby1, float *u01, float *ua1,
		    float *ub1, float *x, float *y, int *ipat1, int *ind1)
{
	float dx,dy,pxs,pys,pxe,pye,gxs,gys,gxe,gye;
	float xa, ya, xb, yb, xs, ys;
	int nax, nay, nbx, nby, ipat, ind;
	float u0, ua, ub;

	nax = *nax1;
	nay = *nay1;
	nbx = *nbx1;
	nby = *nby1;
	ipat = *ipat1;
	ind = *ind1;
	u0 = *u01;
	ua = *ua1;
	ub = *ub1;

	gsaf_inqgdefin (&pxs, &pxe, &pys, &pye, &gxs, &gxe, &gys, &gye);
	dx=(pxe-pxs)/(gxe-gxs);
        dy=(pye-pys)/(gye-gys);

	if (nax > 0) {
		xa=dx*(x[nax-1]-gxs)+pxs;
		ya=dy*(y[nay-1]-gys)+pys;
	} else {
		xa=dx*(0.5*(x[-nax-1]+x[-nax+1-1])-gxs)+pxs;
		ya=dy*(0.5*(y[-nay-1]+y[-nay+1-1])-gys)+pys;
	}
	if (nbx > 0) {
		xb=dx*(x[nbx-1]-gxs)+pxs;
		yb=dy*(y[nby-1]-gys)+pys;
	} else {
		xb=dx*(0.5*(x[-nbx-1]+x[-nbx+1-1])-gxs)+pxs;
		yb=dy*(0.5*(y[-nby-1]+y[-nby+1-1])-gys)+pys;
	}
	xs=(xb-xa)*(u0-ua)/(ub-ua)+xa;
	ys=(yb-ya)*(u0-ua)/(ub-ua)+ya;

	if (ind == 1) {
		gsaf_movept(xs,ys,ipat);
	} else if (ind == -1) {
		gsaf_movept(xs,ys,-ipat);
	} else if (ind == 0) {
		gsaf_drawpt(xs,ys);
	}
}

/* contour plot slave routine : r-theta fixed */
/*static void conts3 (int nax, int nay, int nbx, int nby, float u0, float ua, float ub,
  float *r, float *t, int ipat, int ind)*/
static void conts3 (int *nax1, int *nay1, int *nbx1, int *nby1, float *u01, float *ua1,
		    float *ub1, float *r, float *t, int *ipat1, int *ind1)
{
	float dx,dy,pxs,pys,pxe,pye,gxs,gys,gxe,gye;
	float ri, ti, xi, yi;
	float delr, delt;
	float ra, ta, rb, tb, rs, ts, xs, ys;
	int i, imax;
	int nax, nay, nbx, nby, ipat, ind;
	float u0, ua, ub;

	nax = *nax1;
	nay = *nay1;
	nbx = *nbx1;
	nby = *nby1;
	ipat = *ipat1;
	ind = *ind1;
	u0 = *u01;
	ua = *ua1;
	ub = *ub1;

	gsaf_inqgdefin (&pxs, &pxe, &pys, &pye, &gxs, &gxe, &gys, &gye);
	dx=(pxe-pxs)/(gxe-gxs);
        dy=(pye-pys)/(gye-gys);

	if (nax > 0) {
		ra=r[nax-1];
		ta=t[0]*(nay-1);
	} else {
		ra=0.5*(r[-nax-1]+r[-nax+1-1]);
		ta=t[0]*(-nay-0.5);
	}
	if (nbx > 0) {
		rb=r[nbx-1];
		tb=t[0]*(nby-1);
	} else {
		rb=0.5*(r[-nbx-1]+r[-nbx+1-1]);
		tb=t[0]*(-nby-0.5);
	}
	rs=(rb-ra)*(u0-ua)/(ub-ua)+ra;
	ts=(tb-ta)*(u0-ua)/(ub-ua)+ta;
	xs=dx*(rs*cos(ts)-gxs)+pxs;
	ys=dy*(rs*sin(ts)-gys)+pys;

	if (ind == 1) {
		gsaf_movept(xs,ys,ipat);
	} else if (ind == -1) {
		gsaf_movept(xs,ys,-ipat);
	} else if (ind == 0) {
		imax=(int)(sqrt((xs-Xt)*(xs-Xt)+(ys-Yt)*(ys-Yt))*16/Rmax)+1;
		delr=(rs-Rt)/imax;
		delt=(ts-Tt)/imax;
		for (i=1; i<=imax; i++) {
			ri=delr*i+Rt;
			ti=delt*i+Tt;
			xi=dx*(ri*cos(ti)-gxs)+pxs;
			yi=dy*(ri*sin(ti)-gys)+pys;
			gsaf_drawpt(xi,yi);
		}
	}
	Rt=rs;
	Tt=ts;
	Xt=xs;
	Yt=ys;
}

/* contour plot slave routine : r-theta variable */
/*static void conts4 (int nax, int nay, int nbx, int nby, float u0, float ua, float ub,
  float *r, float *t, int ipat, int ind)*/
static void conts4 (int *nax1, int *nay1, int *nbx1, int *nby1, float *u01, float *ua1,
		    float *ub1, float *r, float *t, int *ipat1, int *ind1)
{
	float dx,dy,pxs,pys,pxe,pye,gxs,gys,gxe,gye;
	float ra, ta, rb, tb, rs, ts, xs, ys;
	int imax;
	float delr, delt;
	float ri, ti, xi, yi;
	int i;
	int nax, nay, nbx, nby, ipat, ind;
	float u0, ua, ub;

	nax = *nax1;
	nay = *nay1;
	nbx = *nbx1;
	nby = *nby1;
	ipat = *ipat1;
	ind = *ind1;
	u0 = *u01;
	ua = *ua1;
	ub = *ub1;

	gsaf_inqgdefin (&pxs, &pxe, &pys, &pye, &gxs, &gxe, &gys, &gye);
	dx=(pxe-pxs)/(gxe-gxs);
        dy=(pye-pys)/(gye-gys);

	if (nax > 0) {
		ra=r[nax-1];
		ta=t[nay-1];
	} else {
		ra=0.5*(r[-nax-1]+r[-nax+1-1]);
		ta=0.5*(t[-nay-1]+t[-nay+1-1]);
	}
	if (nbx > 0) {
		rb=r[nbx-1];
		tb=t[nby-1];
	} else {
		rb=0.5*(r[-nbx-1]+r[-nbx+1-1]);
		tb=0.5*(t[-nby-1]+t[-nby+1-1]);
	}

	rs=(rb-ra)*(u0-ua)/(ub-ua)+ra;
	ts=(tb-ta)*(u0-ua)/(ub-ua)+ta;
	xs=dx*(rs*cos(ts)-gxs)+pxs;
	ys=dy*(rs*sin(ts)-gys)+pys;

	if (ind == 1) {
		gsaf_movept(xs,ys,ipat);
	} else if (ind == -1) {
		gsaf_movept(xs,ys,-ipat);
	} else if (ind == 0) {
		imax=(int)(sqrt((xs-Xt)*(xs-Xt)+(ys-Yt)*(ys-Yt))*16/Rmax)+1;
		delr=(rs-Rt)/imax;
		delt=(ts-Tt)/imax;
		for (i=1; i<=imax; i++) {
			ri=delr*i+Rt;
			ti=delt*i+Tt;
			xi=dx*(ri*cos(ti)-gxs)+pxs;
			yi=dy*(ri*sin(ti)-gys)+pys;
			gsaf_drawpt(xi,yi);
		}
	}
	Rt=rs;
	Tt=ts;
	Xt=xs;
	Yt=ys;
}

/* contour plot : common sub (rectangular) */
void gsaf_contr0(float *z, float *x, float *y, int nxa, int nx, int ny,
		 float zorg, float zstep, int nstep, int iprd, SUB)
{
	const float eps = 1.e-32;
	int kmax, nxmax, nymax;
	float zbias;
	int jdo, j, jj;
	int ido, i, ii;
	float u0, u1, u2, u3, u4;
	int k1, k2, k3, k4;
	gsglboolean lj1, lj2, lj3, lj4;
	int ks, ke, k;
	int itmp1, itmp2, itmp3, itmp4, itmp5, itmp6;
	float ftmp1, ftmp2, ftmp3;

	if (!gsgl_inq_page_is_open ()) return;

	kmax=nstep;
	nxmax=nx-1;
	nymax=ny-1;
	if (iprd == 1 || iprd == 3) nxmax=nx;
	if (iprd == 2 || iprd == 3) nymax=ny;
	zbias=0.9999999;

	for (jdo=1; jdo<=nymax; jdo++) {
		j=jdo;
		jj=j+1;
		if (j == ny) jj=1;
		u1=(z[findex2(1-1,j-1,nxa)]-zorg)/zstep+zbias;
		u4=(z[findex2(1-1,jj-1,nxa)]-zorg)/zstep+zbias;
		k1=(int)(MAX(u1,0.0));
		k4=(int)(MAX(u4,0.0));

		for (ido=1; ido<=nxmax; ido++) {
			i=ido;
			ii=i+1;
			if (i == nx) ii=1;
			u2=(z[findex2(ii-1,j-1,nxa)]-zorg)/zstep+zbias;
			u3=(z[findex2(ii-1,jj-1,nxa)]-zorg)/zstep+zbias;
			k2=(int)(MAX(u2,0.0));
			k3=(int)(MAX(u3,0.0));

			lj1=fabs(u2-u1) >= eps;
			lj2=fabs(u3-u2) >= eps;
			lj3=fabs(u4-u3) >= eps;
			lj4=fabs(u1-u4) >= eps;

			if (lj1 && (k1 != k2) && (k1 > 0 || k2 > 0)
			    && (k1 < kmax || k2 < kmax)) {
				ks=MAX(MIN(k1,k2),0)+1;
				ke=MIN(MAX(k1,k2),kmax);

				for (k=ks; k<=ke; k++) {
					u0=(float) (k);
					if (lj2 && (u0 <= u3 || u0 <= u2)
					    && (u3 <= u0 || u2 <= u0)) {
						itmp1 = i;
						itmp2 = j;
						itmp3 = i+1;
						itmp4 = j;
						itmp5 = 0;
						itmp6 = 1;
						ftmp1 = u0;
						ftmp2 = u1;
						ftmp3 = u2;
						/*sub(i  ,j  ,i+1,j  ,u0,u1,u2,x,y,0,1);*/
						sub(&itmp1,&itmp2,&itmp3,&itmp4,&ftmp1,&ftmp2,
						    &ftmp3,x,y,&itmp5,&itmp6);
						itmp1 = i+1;
						itmp2 = j;
						itmp3 = i+1;
						itmp4 = j+1;
						itmp5 = 0;
						itmp6 = 0;
						ftmp1 = u0;
						ftmp2 = u2;
						ftmp3 = u3;
						/*sub(i+1,j  ,i+1,j+1,u0,u2,u3,x,y,0,0);*/
						sub(&itmp1,&itmp2,&itmp3,&itmp4,&ftmp1,&ftmp2,
						    &ftmp3,x,y,&itmp5,&itmp6);

					}

					if ((u0 <= u1 || u0 <= u3) && 
					    (u1 <= u0 || u3 <= u0)) {

						if (lj3 && (u0 <= u4 || u0 <= u3)
						    && (u4 <= u0 || u3 <= u0)
						    && (u0 <= u2 || u0 <= u4)
						    && (u2 <= u0 || u4 <= u0)) {
							itmp1 = i;
							itmp2 = j;
							itmp3 = i+1;
							itmp4 = j;
							itmp5 = 0;
							itmp6 = 1;
							ftmp1 = u0;
							ftmp2 = u1;
							ftmp3 = u2;
							/*sub(i  ,j  ,i+1,j  ,u0,u1,u2,x,y,0,1);*/
							sub(&itmp1,&itmp2,&itmp3,&itmp4,&ftmp1,&ftmp2,
							    &ftmp3,x,y,&itmp5,&itmp6);
							itmp1 = i;
							itmp2 = j+1;
							itmp3 = i+1;
							itmp4 = j+1;
							itmp5 = 0;
							itmp6 = 0;
							ftmp1 = u0;
							ftmp2 = u4;
							ftmp3 = u3;
							/*sub(i  ,j+1,i+1,j+1,u0,u4,u3,x,y,0,0);*/
							sub(&itmp1,&itmp2,&itmp3,&itmp4,&ftmp1,&ftmp2,
							    &ftmp3,x,y,&itmp5,&itmp6);
						}

						if (lj4 && (u0 <= u1 || u0 <= u4)
						    && (u1 <= u0 || u4 <= u0)) {
							itmp1 = i;
							itmp2 = j;
							itmp3 = i+1;
							itmp4 = j;
							itmp5 = 0;
							itmp6 = 1;
							ftmp1 = u0;
							ftmp2 = u1;
							ftmp3 = u2;
							/*sub(i  ,j  ,i+1,j  ,u0,u1,u2,x,y,0,1);*/
							sub(&itmp1,&itmp2,&itmp3,&itmp4,&ftmp1,&ftmp2,
							    &ftmp3,x,y,&itmp5,&itmp6);
							itmp1 = i;
							itmp2 = j;
							itmp3 = i;
							itmp4 = j+1;
							itmp5 = 0;
							itmp6 = 0;
							ftmp1 = u0;
							ftmp2 = u1;
							ftmp3 = u4;
							/*sub(i  ,j  ,i  ,j+1,u0,u1,u4,x,y,0,0);*/
							sub(&itmp1,&itmp2,&itmp3,&itmp4,&ftmp1,&ftmp2,
							    &ftmp3,x,y,&itmp5,&itmp6);
						}
					}
				}
			}
     
			if (lj2 && (k2 != k3)
			    && (k2 > 0 || k3 > 0)
			    && (k2 < kmax || k3 < kmax)) {
				ks=MAX(MIN(k2,k3),0)+1;
				ke=MIN(MAX(k2,k3),kmax);

				for (k=ks; k<=ke; k++) {
					u0=(float) (k);
					if ((u0 <= u1 || u0 <= u3) && 
					    (u1 <= u0 || u3 <= u0)) {

						if (lj3 && (u0 <= u3 || u0 <= u4)
						    && (u3 <= u0 || u4 <= u0)) {
							itmp1 = i+1;
							itmp2 = j;
							itmp3 = i+1;
							itmp4 = j+1;
							itmp5 = 0;
							itmp6 = 1;
							ftmp1 = u0;
							ftmp2 = u2;
							ftmp3 = u3;
							/*sub(i+1,j  ,i+1,j+1,u0,u2,u3,x,y,0,1);*/
							sub(&itmp1,&itmp2,&itmp3,&itmp4,&ftmp1,&ftmp2,
							    &ftmp3,x,y,&itmp5,&itmp6);
							itmp1 = i;
							itmp2 = j+1;
							itmp3 = i+1;
							itmp4 = j+1;
							itmp5 = 0;
							itmp6 = 0;
							ftmp1 = u0;
							ftmp2 = u4;
							ftmp3 = u3;
							/*sub(i  ,j+1,i+1,j+1,u0,u4,u3,x,y,0,0);*/
							sub(&itmp1,&itmp2,&itmp3,&itmp4,&ftmp1,&ftmp2,
							    &ftmp3,x,y,&itmp5,&itmp6);
						}
     
						if (lj4 && (u0 <= u1 || u0 <= u4)
						    && (u1 <= u0 || u4 <= u0)
						    && (u0 <= u2 || u0 <= u4)
						    && (u2 <= u0 || u4 <= u0)) {
							itmp1 = i+1;
							itmp2 = j;
							itmp3 = i+1;
							itmp4 = j+1;
							itmp5 = 0;
							itmp6 = 1;
							ftmp1 = u0;
							ftmp2 = u2;
							ftmp3 = u3;
							/*sub(i+1,j  ,i+1,j+1,u0,u2,u3,x,y,0,1);*/
							sub(&itmp1,&itmp2,&itmp3,&itmp4,&ftmp1,&ftmp2,
							    &ftmp3,x,y,&itmp5,&itmp6);
							itmp1 = i;
							itmp2 = j;
							itmp3 = i;
							itmp4 = j+1;
							itmp5 = 0;
							itmp6 = 0;
							ftmp1 = u0;
							ftmp2 = u1;
							ftmp3 = u4;
							/*sub(i  ,j  ,i  ,j+1,u0,u1,u4,x,y,0,0);*/
							sub(&itmp1,&itmp2,&itmp3,&itmp4,&ftmp1,&ftmp2,
							    &ftmp3,x,y,&itmp5,&itmp6);
						}
					}
				}
			}
     
			if (lj3 && (k3 != k4)
			    && (k3 > 0 || k4 > 0)
			    && (k3 < kmax || k4 < kmax)) {
				ks=MAX(MIN(k3,k4),0)+1;
				ke=MIN(MAX(k3,k4),kmax);
     
				for (k=ks; k<=ke; k++) {
					u0=(float) (k);
					if (lj4 && (u1 <= u0 || u4 <= u0)
					    && (u0 <= u1 || u0 <= u4)) {
						itmp1 = i;
						itmp2 = j+1;
						itmp3 = i+1;
						itmp4 = j+1;
						itmp5 = 0;
						itmp6 = 1;
						ftmp1 = u0;
						ftmp2 = u4;
						ftmp3 = u3;
						/*sub(i  ,j+1,i+1,j+1,u0,u4,u3,x,y,0,1);*/
						sub(&itmp1,&itmp2,&itmp3,&itmp4,&ftmp1,&ftmp2,
						    &ftmp3,x,y,&itmp5,&itmp6);
						itmp1 = i;
						itmp2 = j;
						itmp3 = i;
						itmp4 = j+1;
						itmp5 = 0;
						itmp6 = 0;
						ftmp1 = u0;
						ftmp2 = u1;
						ftmp3 = u4;
						/*sub(i  ,j  ,i  ,j+1,u0,u1,u4,x,y,0,0);*/
						sub(&itmp1,&itmp2,&itmp3,&itmp4,&ftmp1,&ftmp2,
						    &ftmp3,x,y,&itmp5,&itmp6);
					}
				}
			}
     
			u1=u2;
			u4=u3;
			k1=k2;
			k4=k3;
		}
	}
}

/* contour plot : common sub (2 triangle) */
void gsaf_contp0 (float *z, float *x, float *y, int nxa, int nxmax, int nymax,
		  float zorg, float zstep, int nstep, int iprd, int ipat, int *ka, SUB)
{
	const float eps = 1.e-7;
	int kmax, nxm, nym, ie, nx, ny, nxp, nyp;
	float zbias;
	float u0, u1, u2, u3;
	int k1, k2, k3;
	gsglboolean linv, lend;
	int k;	
	int ne, n1x, n1y, n2x, n2y, n3x, n3y, i2x, i2y, i3x, i3y, nax;
	int nay, nbx, nby, nsax, nsay, mode, nsbx, nsby;
	float ua, ub, usa, usb;
	int modes, naxq, nayq, nbxq, nbyq, iel, nel, nxl, nyl, model;
	float uaq, ubq;
	int itmp1, itmp2, itmp3, itmp4, itmp5, itmp6;
	float ftmp1, ftmp2, ftmp3;

	if (!gsgl_inq_page_is_open ()) return;

	nxm=nxmax-1;
	nym=nymax-1;
	if (iprd == 1 || iprd == 3) nxm=nxmax;
	if (iprd == 2 || iprd == 3) nym=nymax;

	zbias=2.0-10.0*eps;
	kmax=nstep;

	ie=0;
	for (ny=1;ny<=nym;ny++) {
		for (nx=1;nx<=nxm;nx++) {
			ie=ie+1;
			nxp=nx+1;
			nyp=ny+1;
			if (nxp > nxmax) nxp=1;
			if (nyp > nymax) nyp=1;
			k1=(int)((z[findex2(nx-1,ny-1,nxa)]-zorg)/zstep+zbias);
			k2=(int)((z[findex2(nxp-1,nyp-1,nxa)]-zorg)/zstep+zbias);
			k3=(int)((z[findex2(nx-1,nyp-1,nxa)]-zorg)/zstep+zbias);
			ka[findex2(1-1,ie-1,2)]=MAX(MIN(k1,MIN(k2,k3)),1);
			ka[findex2(2-1,ie-1,2)]=MAX(k1,MAX(k2,MAX(k3,1)));
			ie=ie+1;
			k3=(int)((z[findex2(nxp-1,ny-1,nxa)]-zorg)/zstep+zbias);
			ka[findex2(1-1,ie-1,2)]=MAX(MIN(k1,MIN(k2,k3)),1);
			ka[findex2(2-1,ie-1,2)]=MAX(k1,MAX(k2,MAX(k3,1)));
		}
	}
	/* iemax=ie */

	for (k=1;k<=kmax;k++) {
	    u0=(float) (k+1);
	    for (ny=1;ny<=nym;ny++) {
		for (nx=1;nx<=nxm;nx++) {
		    for (ne=1;ne<=2;ne++) {
			ie=(nxm*(ny-1)+nx-1)*2+ne;
			if (ka[findex2(1-1,ie-1,2)] <= k && ka[findex2(2-1,ie-1,2)] > k) {
				n1x=nx;
				n1y=ny;
				n2x=nx+1;
				n2y=ny+1;
				if (ne == 1) {
					n3x=nx;
					n3y=ny+1;
				} else {
					n3x=nx+1;
					n3y=ny;
				}
				u1=(z[findex2(n1x-1,n1y-1,nxa)]-zorg)/zstep+zbias;
				i2x=n2x;
				i2y=n2y;
				if (i2x > nxmax) i2x=1;
				if (i2y > nymax) i2y=1;
				u2=(z[findex2(i2x-1,i2y-1,nxa)]-zorg)/zstep+zbias;
				i3x=n3x;
				i3y=n3y;
				if (i3x > nxmax) i3x=1;
				if (i3y > nymax) i3y=1;
				u3=(z[findex2(i3x-1,i3y-1,nxa)]-zorg)/zstep+zbias;
				
				if ((u1 > u0 && u2 <= u0) || 
				    (u1 <= u0 && u2 > u0)) {
					nax=n1x;
					nay=n1y;
					ua=u1;
					nbx=n2x;
					nby=n2y;
					ub=u2;
					nsax=n3x;
					nsay=n3y;
					usa=u3;
					mode=3;
					if ((u2 > u0 && u3 <= u0) || 
					    (u2 <= u0 && u3 > u0)) {
						nsbx=n2x;
						nsby=n2y;
						usb=u2;
						modes=1;
					} else if ((u1 > u0 && u3 <= u0) || 
						   (u1 <= u0 && u3 > u0)) {
						nsbx=n1x;
						nsby=n1y;
						usb=u1;
						modes=2;
					} else {
						continue;
					}
				} else if ((u1 > u0 && u3 <= u0) || 
					   (u1 <= u0 && u3 > u0)) {
					nax=n1x;
					nay=n1y;
					ua=u1;
					nbx=n3x;
					nby=n3y;
					ub=u3;
					nsax=n2x;
					nsay=n2y;
					usa=u2;
					nsbx=n3x;
					nsby=n3y;
					usb=u3;
					mode=2;
					modes=1;
				} else {
					continue;
				}

				itmp1 = nax;
				itmp2 = nay;
				itmp3 = nbx;
				itmp4 = nby;
				itmp5 = ipat;
				itmp6 = 1;
				ftmp1 = u0;
				ftmp2 = ua;
				ftmp3 = ub;
				/*sub(nax,nay,nbx,nby,u0,ua,ub,x,y,ipat,1);*/
				sub(&itmp1,&itmp2,&itmp3,&itmp4,&ftmp1,&ftmp2,
				    &ftmp3,x,y,&itmp5,&itmp6);
				naxq=nax;
				nayq=nay;
				nbxq=nbx;
				nbyq=nby;
				uaq=ua;
				ubq=ub;
				
				iel=ie;
				nel=ne;
				nxl=nx;
				nyl=ny;
				model=mode;
				linv=GSGL_FALSE;
				lend=GSGL_FALSE;

			TWO:  
				if (nel == 1) {
					nel=2;
					if (model == 1) nyl=nyl+1;
					if (model == 2) nxl=nxl-1;
				} else {
					nel=1;
					if (model == 1) nxl=nxl+1;
					if (model == 2) nyl=nyl-1;
				}

				if (nxl >= 1 && nxl <= nxm && 
				    nyl >= 1 && nyl <= nym) {
					iel=(nxm*(nyl-1)+nxl-1)*2+nel;
					n1x=nxl;
					n1y=nyl;
					n2x=nxl+1;
					n2y=nyl+1;
					if (nel == 1) {
						n3x=nxl;
						n3y=nyl+1;
					} else {
						n3x=nxl+1;
						n3y=nyl;
					}
					u1=(z[findex2(n1x-1,n1y-1,nxa)]-zorg)/zstep+zbias;
					i2x=n2x;
					i2y=n2y;
					if (i2x > nxmax) i2x=1;
					if (i2y > nymax) i2y=1;
					u2=(z[findex2(i2x-1,i2y-1,nxa)]-zorg)/zstep+zbias;
					i3x=n3x;
					i3y=n3y;
					if (i3x > nxmax) i3x=1;
					if (i3y > nymax) i3y=1;
					u3=(z[findex2(i3x-1,i3y-1,nxa)]-zorg)/zstep+zbias;

					if (model == 3) {
						nax=n3x;
						nay=n3y;
						ua=u3;
						if ((u2 > u0 && u3 <= u0) || 
						    (u2 <= u0 && u3 > u0)) {
							nbx=n2x;
							nby=n2y;
							ub=u2;
							model=1;
						} else if ((u1 > u0 && u3 <= u0) || 
							   (u1 <= u0 && u3 > u0)) {
							nbx=n1x;
							nby=n1y;
							ub=u1;
							model=2;
						} else {
							lend=GSGL_TRUE;
						}
					} else if (model == 2) {
						nax=n1x;
						nay=n1y;
						ua=u1;
						if ((u1 > u0 && u3 <= u0) || 
						    (u1 <= u0 && u3 > u0)) {
							nbx=n3x;
							nby=n3y;
							ub=u3;
							model=2;
						} else if ((u1 > u0 && u2 <= u0) || 
							   (u1 <= u0 && u2 > u0)) {
							nbx=n2x;
							nby=n2y;
							ub=u2;
							model=3;
						} else {
							lend=GSGL_TRUE;
						}
					} else if (model == 1) {
						nax=n2x;
						nay=n2y;
						ua=u2;
						if ((u2 > u0 && u1 <= u0) || 
						    (u2 <= u0 && u1 > u0)) {
							nbx=n1x;
							nby=n1y;
							ub=u1;
							model=3;
						} else if ((u2 > u0 && u3 <= u0) || 
							   (u2 <= u0 && u3 > u0)) {
							nbx=n3x;
							nby=n3y;
							ub=u3;
							model=1;
						} else {
							lend=GSGL_TRUE;
						}
					}
				} else {
					if (linv) {
						lend=GSGL_TRUE;
					} else {
						
						itmp1 = naxq;
						itmp2 = nayq;
						itmp3 = nbxq;
						itmp4 = nbyq;
						itmp5 = ipat;
						itmp6 = -1;
						ftmp1 = u0;
						ftmp2 = uaq;
						ftmp3 = ubq;
						/*sub(naxq,nayq,nbxq,nbyq,u0,uaq,ubq,x,y,ipat,-1);*/
						sub(&itmp1,&itmp2,&itmp3,&itmp4,&ftmp1,&ftmp2,
						    &ftmp3,x,y,&itmp5,&itmp6);

						iel=ie;
						nax=nsax;
						nay=nsay;
						ua=usa;
						nbx=nsbx;
						nby=nsby;
						ub=usb;
						nxl=nx;
						nyl=ny;
						nel=ne;
						model=modes;
						linv=GSGL_TRUE;
					}
				}
				if (!lend) {

					itmp1 = nax;
					itmp2 = nay;
					itmp3 = nbx;
					itmp4 = nby;
					itmp5 = ipat;
					itmp6 = 0;
					ftmp1 = u0;
					ftmp2 = ua;
					ftmp3 = ub;
					/*sub(nax,nay,nbx,nby,u0,ua,ub,x,y,ipat,0);*/
					sub(&itmp1,&itmp2,&itmp3,&itmp4,&ftmp1,&ftmp2,
						    &ftmp3,x,y,&itmp5,&itmp6);

					ka[findex2(1-1,iel-1,2)]=ka[findex2(1-1,iel-1,2)]+1;
					if (iel == ie && !linv) lend=GSGL_TRUE;
				}

				if (!lend) goto TWO;
				
			}
		    }
		}
	    }
	}
}

/* contour plot : common sub (4 triangle) */
void gsaf_contq0 (float *z, float *x, float *y, int nxa, int nxmax, int nymax,
		  float zorg, float zstep, int nstep, int iprd, int ipat, int *ka, SUB)
{
	const float eps = 1.e-7;
	int nxm, nym, kmax, ie, ny, nx, nxp, nyp, k1, k2, k3, k4, kc;
	float zbias, zc;
	int ne, n1x, n1y, n2x, n2y, i2x, i2y, nax;
	int nay, nbx, nby, nsax, nsay, mode, nsbx, nsby;
	float ua, ub, usa, usb;
	int modes, naxq, nayq, nbxq, nbyq, iel, nel, nxl, nyl, model;
	float uaq, ubq;
	int k, i1x, i1y;
	float u0;
	float uc, u1, u2, ucl;
	gsglboolean linv, lend;
	int itmp1, itmp2, itmp3, itmp4, itmp5, itmp6;
	float ftmp1, ftmp2, ftmp3;

	if (!gsgl_inq_page_is_open ()) return;

	nxm=nxmax-1;
	nym=nymax-1;
	if (iprd == 1 || iprd == 3) nxm=nxmax;
	if (iprd == 2 || iprd == 3) nym=nymax;

	zbias=2.0-10.0*eps;
	kmax=nstep;

	ie=0;
	for (ny=1; ny<=nym; ny++) {
		for (nx=1; nx<=nxm; nx++) {
			nxp=nx+1;
			nyp=ny+1;
			if (nxp > nxmax) nxp=1;
			if (nyp > nymax) nyp=1;
			k1=(int)((z[findex2(nx-1,ny-1,nxa)]-zorg)/zstep+zbias);
			k2=(int)((z[findex2(nxp-1,ny-1,nxa)]-zorg)/zstep+zbias);
			k3=(int)((z[findex2(nxp-1,nyp-1,nxa)]-zorg)/zstep+zbias);
			k4=(int)((z[findex2(nx-1,nyp-1,nxa)]-zorg)/zstep+zbias);
			zc=0.25*(+z[findex2(nx-1,ny-1,nxa)]
				 +z[findex2(nxp-1,ny-1,nxa)]
				 +z[findex2(nxp-1,nyp-1,nxa)]
				 +z[findex2(nx-1,nyp-1,nxa)]);
			kc=(int)((zc        -zorg)/zstep+zbias);
			ie=ie+1;
			ka[findex2(1-1,ie-1,2)]=MAX(MIN(k1,MIN(k2,kc)),1);
			ka[findex2(2-1,ie-1,2)]=MAX(k1,MAX(k2,MAX(kc,1)));
			ie=ie+1;
			ka[findex2(1-1,ie-1,2)]=MAX(MIN(k2,MIN(k3,kc)),1);
			ka[findex2(2-1,ie-1,2)]=MAX(k2,MAX(k3,MAX(kc,1)));
			ie=ie+1;
			ka[findex2(1-1,ie-1,2)]=MAX(MIN(k3,MIN(k4,kc)),1);
			ka[findex2(2-1,ie-1,2)]=MAX(k3,MAX(k4,MAX(kc,1)));
			ie=ie+1;
			ka[findex2(1-1,ie-1,2)]=MAX(MIN(k4,MIN(k1,kc)),1);
			ka[findex2(2-1,ie-1,2)]=MAX(k4,MAX(k1,MAX(kc,1)));
		}
	}
/* iemax=ie */

	for (k=1; k<=kmax; k++) {
	    u0=(float) (k+1);
	    for (ny=1; ny<=nym; ny++) {
		for (nx=1; nx<=nxm; nx++) {
		    i1x=nx;
		    i1y=ny;
		    i2x=nx+1;
		    i2y=ny+1;
		    if (i2x > nxmax) i2x=1;
		    if (i2y > nymax) i2y=1;
		    zc=0.25*(+z[findex2(i1x-1,i1y-1,nxa)]
			     +z[findex2(i2x-1,i1y-1,nxa)]
			     +z[findex2(i2x-1,i2y-1,nxa)]
			     +z[findex2(i1x-1,i2y-1,nxa)]);
		    uc=(zc-zorg)/zstep+zbias;
		    for (ne=1; ne<=4; ne++) {
			ie=(nxm*(ny-1)+nx-1)*4+ne;
			if (ka[findex2(1-1,ie-1,2)] <= k &&
			    ka[findex2(2-1,ie-1,2)] > k) {
			    if (ne == 1) {
				    n1x=nx;
				    n1y=ny;
				    n2x=nx+1;
				    n2y=ny;
			    } else if (ne == 2) {
				    n1x=nx+1;
				    n1y=ny;
				    n2x=nx+1;
				    n2y=ny+1;
			    } else if (ne == 3) {
				    n1x=nx+1;
				    n1y=ny+1;
				    n2x=nx;
				    n2y=ny+1;
			    } else {
				    n1x=nx;
				    n1y=ny+1;
				    n2x=nx;
				    n2y=ny;
			    }
			    i1x=n1x;
			    i1y=n1y;
			    if (i1x > nxmax) i1x=1;
			    if (i1y > nymax) i1y=1;
			    u1=(z[findex2(i1x-1,i1y-1,nxa)]-zorg)/zstep+zbias;
			    i2x=n2x;
			    i2y=n2y;
			    if (i2x > nxmax) i2x=1;
			    if (i2y > nymax) i2y=1;
			    u2=(z[findex2(i2x-1,i2y-1,nxa)]-zorg)/zstep+zbias;
			    
			    if ((u1 > u0 && u2 <= u0) || 
				(u1 <= u0 && u2 > u0)) {
				    nax=n1x;
				    nay=n1y;
				    ua=u1;
				    nbx=n2x;
				    nby=n2y;
				    ub=u2;
				    nsax=-nx;
				    nsay=-ny;
				    usa=uc;
				    mode=3;
				    if ((u2 > u0 && uc <= u0) || 
					(u2 <= u0 && uc > u0)) {
					    nsbx=n2x;
					    nsby=n2y;
					    usb=u2;
					    modes=1;
				    } else if ((u1 > u0 && uc <= u0) || 
					       (u1 <= u0 && uc > u0)) {
					    nsbx=n1x;
					    nsby=n1y;
					    usb=u1;
					    modes=2;
				    } else {
					    continue;
				    }
			    } else if ((u1 > u0 && uc <= u0) || 
				       (u1 <= u0 && uc > u0)) {
				    nax=n1x;
				    nay=n1y;
				    ua=u1;
				    nbx=-nx;
				    nby=-ny;
				    ub=uc;
				    nsax=n2x;
				    nsay=n2y;
				    usa=u2;
				    nsbx=-nx;
				    nsby=-ny;
				    usb=uc;
				    mode=2;
				    modes=1;
			    } else {
				    continue;
			    }
			    
			    itmp1 = nax;
			    itmp2 = nay;
			    itmp3 = nbx;
			    itmp4 = nby;
			    itmp5 = ipat;
			    itmp6 = 1;
			    ftmp1 = u0;
			    ftmp2 = ua;
			    ftmp3 = ub;
			    /*sub(nax,nay,nbx,nby,u0,ua,ub,x,y,ipat,1);*/
			    sub(&itmp1,&itmp2,&itmp3,&itmp4,&ftmp1,&ftmp2,
				&ftmp3,x,y,&itmp5,&itmp6);
			    naxq=nax;
			    nayq=nay;
			    nbxq=nbx;
			    nbyq=nby;
			    uaq=ua;
			    ubq=ub;
			    
			    iel=ie;
			    nel=ne;
			    nxl=nx;
			    nyl=ny;
			    ucl=uc;
			    model=mode;
			    linv=GSGL_FALSE;
			    lend=GSGL_FALSE;
			    
			TWO:
			    if (model == 1) {
				    nel=nel+1;
				    if (nel == 5) nel=1;
			    } else if (model == 2) {
				    nel=nel-1;
				    if (nel == 0) nel=4;
			    } else {
				if (nel == 1) {
					nyl=nyl-1;
				} else if (nel == 2) {
					nxl=nxl+1;
				} else if (nel == 3) {
					nyl=nyl+1;
				} else {
					nxl=nxl-1;
				}
				if (nxl >= 1 && nxl <= nxm && 
				    nyl >= 1 && nyl <= nym) {
				    nel=nel+2;
				    if (nel > 4) nel=nel-4;
				    i1x=nxl;
				    i1y=nyl;
				    if (i1x > nxmax) i1x=1;
				    if (i1y > nymax) i1y=1;
				    i2x=nxl+1;
				    i2y=nyl+1;
				    if (i2x > nxmax) i2x=1;
				    if (i2y > nymax) i2y=1;
				    zc=0.25*(+z[findex2(i1x-1,i1y-1,nxa)]
					     +z[findex2(i2x-1,i1y-1,nxa)]
					     +z[findex2(i2x-1,i2y-1,nxa)]
					     +z[findex2(i1x-1,i2y-1,nxa)]);
				    ucl=(zc-zorg)/zstep+zbias;
				}
			    }
			    
			    if (nxl >= 1 && nxl <= nxm && 
				nyl >= 1 && nyl <= nym) {
				    iel=(nxm*(nyl-1)+nxl-1)*4+nel;
				    if (nel == 1) {
					    n1x=nxl;
					    n1y=nyl;
					    n2x=nxl+1;
					    n2y=nyl;
				    } else if (nel == 2) {
					    n1x=nxl+1;
					    n1y=nyl;
					    n2x=nxl+1;
					    n2y=nyl+1;
				    } else if (nel == 3) {
					    n1x=nxl+1;
					    n1y=nyl+1;
					    n2x=nxl;
					    n2y=nyl+1;
				    } else {
					    n1x=nxl;
					    n1y=nyl+1;
					    n2x=nxl;
					    n2y=nyl;
				    }
				    i1x=n1x;
				    i1y=n1y;
				    if (i1x > nxmax) i1x=1;
				    if (i1y > nymax) i1y=1;
				    u1=(z[findex2(i1x-1,i1y-1,nxa)]-zorg)/zstep+zbias;
				    i2x=n2x;
				    i2y=n2y;
				    if (i2x > nxmax) i2x=1;
				    if (i2y > nymax) i2y=1;
				    u2=(z[findex2(i2x-1,i2y-1,nxa)]-zorg)/zstep+zbias;
				    
				    if (model == 3) {
					    nax=-nxl;
					    nay=-nyl;
					    ua=ucl;
					    if ((u2 > u0 && ucl <= u0) || 
						(u2 <= u0 && ucl > u0)) {
						    nbx=n2x;
						    nby=n2y;
						    ub=u2;
						    model=1;
					    } else if ((u1 > u0 && ucl <= u0) || 
						       (u1 <= u0 && ucl > u0)) {
						    nbx=n1x;
						    nby=n1y;
						    ub=u1;
						    model=2;
					    } else {
						    lend=GSGL_TRUE;
					    }
				    } else if (model == 2) {
					    nax=n1x;
					    nay=n1y;
					    ua=u1;
					    if ((u1 > u0 && ucl <= u0) || 
						(u1 <= u0 && ucl > u0)) {
						    nbx=-nxl;
						    nby=-nyl;
						    ub=ucl;
						    model=2;
					    } else if ((u1 > u0 && u2 <= u0) || 
						       (u1 <= u0 && u2 > u0)) {
						    nbx=n2x;
						    nby=n2y;
						    ub=u2;
						    model=3;
					    } else {
						    lend=GSGL_TRUE;
					    }
				    } else if (model == 1) {
					    nax=n2x;
					    nay=n2y;
					    ua=u2;
					    if ((u2 > u0 && u1 <= u0) || 
						(u2 <= u0 && u1 > u0)) {
						    nbx=n1x;
						    nby=n1y;
						    ub=u1;
						    model=3;
					    } else if ((u2 > u0 && ucl <= u0) || 
						       (u2 <= u0 && ucl > u0)) {
						    nbx=-nxl;
						    nby=-nyl;
						    ub=ucl;
						    model=1;
					    } else {
						    lend=GSGL_TRUE;
					    }
				    }
			    } else {
				    if (linv) {
					    lend=GSGL_TRUE;
				    } else {
					    
					    itmp1 = naxq;
					    itmp2 = nayq;
					    itmp3 = nbxq;
					    itmp4 = nbyq;
					    itmp5 = ipat;
					    itmp6 = -1;
					    ftmp1 = u0;
					    ftmp2 = uaq;
					    ftmp3 = ubq;
					    /*sub(naxq,nayq,nbxq,nbyq,u0,uaq,ubq,x,y,ipat,-1);*/
					    sub(&itmp1,&itmp2,&itmp3,&itmp4,&ftmp1,&ftmp2,
						&ftmp3,x,y,&itmp5,&itmp6);

					    nax=nsax;
					    nay=nsay;
					    ua=usa;
					    nbx=nsbx;
					    nby=nsby;
					    ub=usb;
					    iel=ie;
					    nxl=nx;
					    nyl=ny;
					    nel=ne;
					    ucl=uc;
					    model=modes;
					    linv=GSGL_TRUE;
				    }
			    }
			    if (!lend) {

				    itmp1 = nax;
				    itmp2 = nay;
				    itmp3 = nbx;
				    itmp4 = nby;
				    itmp5 = ipat;
				    itmp6 = 0;
				    ftmp1 = u0;
				    ftmp2 = ua;
				    ftmp3 = ub;
				    /*sub(nax,nay,nbx,nby,u0,ua,ub,x,y,ipat,0);*/
				    sub(&itmp1,&itmp2,&itmp3,&itmp4,&ftmp1,&ftmp2,
					&ftmp3,x,y,&itmp5,&itmp6);

				    ka[findex2(1-1,iel-1,2)]=ka[findex2(1-1,iel-1,2)]+1;
				    if (iel == ie && !linv) lend=GSGL_TRUE;
			    }

			    if (!lend) goto TWO;

			}
		    }
		}
	    }
	}
}

/* contour plot : xy, fem, pattern */
void gsaf_contpf_internal(float *z, float *x, float *y, int nnod, int *ielm, int nelm,
			  float zorg, float zstep, int nstep, int ipat, int *ka)
{
	float dx,dy,pxs,pys,pxe,pye,gxs,gys,gxe,gye;
	const float eps = 1.e-7;
	int kmax, ie, k1, k2, k3;
	float zbias;
	float ua, ub;
	int iel;
	int k, na, nb, ma, mb;
	float u0, u1, u2, u3;
	float xa, xb, ya, yb;
	float xs, ys;
	float xx, yy;
	int ies, iep, iem;
	gsglboolean linv;
	int n1, n2, n3;
	gsglboolean l12, l23, l31;
	gsglPage *p = gsgl_inq_page ();

	if (!gsgl_inq_gdefin_is_called (p) || fabs(zstep) < 1.e-32) return;

	if (ka == NULL) ka = (int *) gsgl_malloc_atomic (nelm*2*sizeof(int));

	gsaf_inqgdefin (&pxs, &pxe, &pys, &pye, &gxs, &gxe, &gys, &gye);
	dx=(pxe-pxs)/(gxe-gxs);
        dy=(pye-pys)/(gye-gys);

	zbias=2.-10.*eps;
	kmax=nstep;

	for (ie=1; ie<=nelm; ie++) {
		k1=(int)((z[abs(ielm[findex2(1-1,ie-1,3)])-1]-zorg)/zstep+zbias);
		k2=(int)((z[abs(ielm[findex2(2-1,ie-1,3)])-1]-zorg)/zstep+zbias);
		k3=(int)((z[abs(ielm[findex2(3-1,ie-1,3)])-1]-zorg)/zstep+zbias);
		ka[findex2(1-1,ie-1,2)]=MAX(MIN(k1,MIN(k2,k3)),1);
		ka[findex2(2-1,ie-1,2)]=MAX(k1,MAX(k2,MAX(k3,1)));
	}

	for (k=1; k<=kmax; k++) {
		u0=(float) (k+1);
		for (ie=1; ie<=nelm; ie++) {
			if (ka[findex2(1-1,ie-1,2)] <= k && ka[findex2(2-1,ie-1,2)] > k) {
				u1=(z[abs(ielm[findex2(1-1,ie-1,3)])-1]-zorg)/zstep+zbias;
				u2=(z[abs(ielm[findex2(2-1,ie-1,3)])-1]-zorg)/zstep+zbias;
				u3=(z[abs(ielm[findex2(3-1,ie-1,3)])-1]-zorg)/zstep+zbias;

				if (fabs(u1-u2) > eps 
				    && (u1 > u0 || u2 > u0)
				    && (u1 <= u0 || u2 <= u0)) {
					na=abs(ielm[findex2(1-1,ie-1,3)]);
					nb=abs(ielm[findex2(2-1,ie-1,3)]);
					ma=abs(ielm[findex2(3-1,ie-1,3)]);
					if (fabs(u2-u3) > eps
					    && (u2 > u0 || u3 > u0)
					    && (u2 <= u0 || u3 <= u0)) {
						mb=abs(ielm[findex2(2-1,ie-1,3)]);
					} else {
						mb=abs(ielm[findex2(1-1,ie-1,3)]);
					}
				} else {
					na=abs(ielm[findex2(1-1,ie-1,3)]);
					nb=abs(ielm[findex2(3-1,ie-1,3)]);
					ma=abs(ielm[findex2(2-1,ie-1,3)]);
					mb=abs(ielm[findex2(3-1,ie-1,3)]);
				}
				ua=(z[na-1]-zorg)/zstep+zbias;
				ub=(z[nb-1]-zorg)/zstep+zbias;
				xa=dx*(x[na-1]-gxs)+pxs;
				ya=dy*(y[na-1]-gys)+pys;
				xb=dx*(x[nb-1]-gxs)+pxs;
				yb=dy*(y[nb-1]-gys)+pys;
				if (ub != ua) {
					xs=(xb-xa)*(u0-ua)/(ub-ua)+xa;
					ys=(yb-ya)*(u0-ua)/(ub-ua)+ya;
				} else {
					xs=xa;
					ys=ya;
				}
				gsaf_movept(xs,ys,ipat);

				iel=ie;
				linv=GSGL_FALSE;
				ies=iel;
				iep=iel;
				iem=iel;

			TWO:
			THREE:
				if (iem != ie || iep != nelm) goto FOUR;
				if (linv) {
					/*goto FIVE;*/
					continue;
				}
				gsaf_movept(xs,ys,-ipat);
				iel=ie;
				linv=GSGL_TRUE;
				na=ma;
				nb=mb;
				ua=(z[na-1]-zorg)/zstep+zbias;
				ub=(z[nb-1]-zorg)/zstep+zbias;
				xa=dx*(x[na-1]-gxs)+pxs;
				ya=dy*(y[na-1]-gys)+pys;
				xb=dx*(x[nb-1]-gxs)+pxs;
				yb=dy*(y[nb-1]-gys)+pys;
				if (ub != ua) {
					xx=(xb-xa)*(u0-ua)/(ub-ua)+xa;
					yy=(yb-ya)*(u0-ua)/(ub-ua)+ya;
					gsaf_drawpt(xx,yy);
				}
				ka[findex2(1-1,iel-1,2)]=ka[findex2(1-1,iel-1,2)]+1;
				ies=iel;
				iep=iel;
				iem=iel;
				goto THREE;
				
			FOUR:
				if ((iel == iem && iep < nelm) || iem == ie) {
					iep=iep+1;
					iel=iep;
				} else {
					iem=iem-1;
					iel=iem;
				}

				if (ka[findex2(1-1,iel-1,2)] <= k && 
				    ka[findex2(2-1,iel-1,2)] > k
				    && iel != ies) {
				    n1=abs(ielm[findex2(1-1,iel-1,3)]);
				    n2=abs(ielm[findex2(2-1,iel-1,3)]);
				    n3=abs(ielm[findex2(3-1,iel-1,3)]);
				    l12=(n1 == na && n2 == nb) || (n1 == nb && n2 == na);
				    l23=(n2 == na && n3 == nb) || (n2 == nb && n3 == na);
				    l31=(n3 == na && n1 == nb) || (n3 == nb && n1 == na);
				    
				    if (l12 || l23 || l31) {
				       u1=(z[abs(ielm[findex2(1-1,iel-1,3)])-1]-zorg)/zstep+zbias;
				       u2=(z[abs(ielm[findex2(2-1,iel-1,3)])-1]-zorg)/zstep+zbias;
				       u3=(z[abs(ielm[findex2(3-1,iel-1,3)])-1]-zorg)/zstep+zbias;

				       if (l12) {
					       na=abs(ielm[findex2(3-1,iel-1,3)]);
					       if (fabs(u2-u3) > eps
						   && (u2 > u0 || u3 > u0)
						   && (u2 <= u0 || u3 <= u0)) {
						       nb=abs(ielm[findex2(2-1,iel-1,3)]);
					       } else {
						       nb=abs(ielm[findex2(1-1,iel-1,3)]);
					       }
				       } else if (l23) {
					       na=abs(ielm[findex2(1-1,iel-1,3)]);
					       if (fabs(u3-u1) > eps
						   && (u3 > u0 || u1 > u0)
						   && (u3 <= u0 || u1 <= u0)) {
						       nb=abs(ielm[findex2(3-1,iel-1,3)]);
					       } else {
						       nb=abs(ielm[findex2(2-1,iel-1,3)]);
					       }
				       } else {
					       na=abs(ielm[findex2(2-1,iel-1,3)]);
					       if (fabs(u1-u2) > eps
						   && (u1 > u0 || u2 > u0)
						   && (u1 <= u0 || u2 <= u0)) {
						       nb=abs(ielm[findex2(1-1,iel-1,3)]);
					       } else {
						       nb=abs(ielm[findex2(3-1,iel-1,3)]);
					       }
				       }
				       ua=(z[na-1]-zorg)/zstep+zbias;
				       ub=(z[nb-1]-zorg)/zstep+zbias;
				       xa=dx*(x[na-1]-gxs)+pxs;
				       ya=dy*(y[na-1]-gys)+pys;
				       xb=dx*(x[nb-1]-gxs)+pxs;
				       yb=dy*(y[nb-1]-gys)+pys;
				       if (ub != ua) {
					       xx=(xb-xa)*(u0-ua)/(ub-ua)+xa;
					       yy=(yb-ya)*(u0-ua)/(ub-ua)+ya;
					       gsaf_drawpt(xx,yy);
				       }
				       ka[findex2(1-1,iel-1,2)]=ka[findex2(1-1,iel-1,2)]+1;
				       if (iel == ie) {
					       /*goto FIVE;*/
					       continue;
				       }
				       ies=iel;
				       iep=iel;
				       iem=iel;
				    }
				}				
				goto TWO;
				/*FIVE:;*/
			}
		}
	}
}

void gsaf_contpf (float *z, float *x, float *y, int nnod, int *ielm, int nelm,
		  float zorg, float zstep, int nstep, int ipat)
{	
	if (!gsgl_inq_page_is_open ()) return;

	gsaf_contpf_internal (z, x, y, nnod, ielm, nelm, zorg, zstep, nstep, ipat, NULL);
}

