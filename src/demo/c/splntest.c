#include "gsgl.h"
#include <stdio.h>
#include <math.h>

static void tdgrc (float *z, float *xd, float *yd, float xmin, float xmax, float ymin, float ymax, float zmin, float zmax, int nxa, int nx, int ny);
static void tdgr (float *z, float *xd, float *yd, float xmin, float xmax, float ymin, float ymax, float zmin, float zmax, int nxa, int nx, int ny);
static void graph1 (float pxmin, float pxmax, float pymin, float pymax, float *gx, float *gy, int nxm, int nxmax, int ngmax);

int main ()
{
	const float pi = 3.141596;
	float x[21], y[21], f[21][21];
	float p[21][21], q[21][21], s[21][21], a[21][4];
	float c[3][21];
	float xg[101], yg[101], fg[101][101], ff[101];
	int i, j;
	int id, error;

	for (i=0; i<21; i++) {
		x[i] = (i-10) * 0.1 * pi;
	}

	for (j=0; j<21; j++) {
		y[j] = (j-10) * 0.1 * pi;
	}

	for (i=0; i<21; i++) {
		for (j=0; j<21; j++) {
			f[j][i] = sin(2.0*x[i]) * cos(2.0*y[j]) + 1.0;
		}
	}

	for (i=0; i<101; i++) {
		xg[i] = (i-50) * 0.02 * pi;
	}

	for (j=0; j<101; j++) {
		yg[j] = (j-50) * 0.02 * pi;
	}

	gsaf_gsopen ();
	for (;;) {
		do {
			error = gsgl_stdin (" # INPUT TYPE : 1,2,3 9/END\n", 1, 
					    " %d", &id);
			if (error == EOF) goto EXIT;
		} while (error == 1 || error == 2);
		
		if (id == 1) {
			tdgrc (&f[0][0], y, x, -pi, pi, -pi, pi, 0.0, 2.0, 21, 21, 21);
			gsaf_spln2d (x, y, &f[0][0], &p[0][0], &q[0][0], &s[0][0], &a[0][0],
				     21, 21, 21, xg, yg, &fg[0][0], 101, 101, 101, 0);
			tdgrc (&fg[0][0], yg, xg, -pi, pi, -pi, pi, 0.0, 2.0, 101, 101, 101);
		} else if (id == 2) {
			tdgr (&f[0][0], y, x, -pi, pi, -pi, pi, 0.0, 2.0, 21, 21, 21);
			gsaf_spln2d (x, y, &f[0][0], &p[0][0], &q[0][0], &s[0][0], &a[0][0],
				     21, 21, 21, xg, yg, &fg[0][0], 101, 101, 101, 0);
			tdgr (&fg[0][0], yg, xg, -pi, pi, -pi, pi, 0.0, 2.0, 101, 101, 101);
		} else if (id == 3) {
			gsaf_pages ();
			graph1 (3.0, 23.0, 2.0, 17.0, x, &f[10][0], 1, 21, 1);
			gsaf_spln1d (x, &f[10][0], &c[0][0], 21, xg, ff, 101, 0);
			graph1 (3.0, 23.0, 2.0, 17.0, xg, ff, 1, 101, 1);
			gsaf_pagee ();
		} else if (id == 9) {
			break;
		}
	}
 EXIT:
	gsaf_gsclos ();
	return 0;
}

static void tdgrc (float *z, float *xd, float *yd, float xmin, float xmax, float ymin, float ymax, float zmin, float zmax, int nxa, int nx, int ny)
{
	gsaf_pages ();
	gsaf_gdefin (3.0, 23.0, 2.0, 17.0, ymin, ymax, xmin, xmax);
	gsaf_gframe ();
	gsaf_contq2 (z, yd, xd, nxa, ny, nx, zmin, zmax/11, 12, 0, 0);
	gsaf_pagee ();
}

#define INDEX(nxa,nx,ny) ((nx)+(nxa)*(ny))
#define MAX(a,b) (a >= b ? a : b)
#define MIN(a,b) (a < b ? a : b)

static void tdgr (float *z, float *xd, float *yd, float xmin, float xmax, float ymin, float ymax, float zmin, float zmax, int nxa, int nx, int ny)
{
	float cr[20000], cp[20000];
	float xc, yc, zc;
	float xs, ys, zs;
	int i;
	int ix, iy;
	int jx, jy, jz;
	int ibc;
	float x, y;
	int ioy, ioym, ioyb;
	float rx, ry, rz;

	gsaf_pages ();
	yc=5.0/(ymax-ymin);
	xc=10.0/(xmax-xmin);
	zc=10.0/(zmax-zmin);
	ys=(ymax-ymin)/10.0;
	xs=(xmax-xmin)/10.0;
	zs=(zmax-zmin)/10.0;

	for (i=0; i<20000; i++) {
		cr[i]=0.0;
		cp[i]=0.0;
	}

	gsaf_move(15.0, 2.0);

	for (iy=0; iy<ny; iy++) {

		x=15.0+(yd[iy]-ymin)*yc-(xd[0]-xmin)*xc;
		y= 2.0+(yd[iy]-ymin)*yc+(z[INDEX(nxa,iy,0)]-zmin)*zc;
		ibc=(x-15.0)*nx/(xmax-xmin)/xc;
		gsaf_move(x,y);

		for (ix=1; ix<nx; ix++) {
			x=15.0+(yd[iy]-ymin)*yc-(xd[ix]-xmin)*xc;
			y= 2.0+(yd[iy]-ymin)*yc+(z[INDEX(nxa,iy,ix)]-zmin)*zc;

			if (y > cr[9999-ibc+ix]) {
				cr[9999-ibc+ix] = y;
				if (iy/1*1 == iy) gsaf_draw (x, y);
			} else {
				gsaf_move (x, y);
			}
		}
	}

	gsaf_move(15.0,2.0);

	for (ix=0; ix<nx; ix++) {

		x=15.0+(yd[0]-ymin)*yc-(xd[ix]-xmin)*xc;
		y= 2.0+(yd[0]-ymin)*yc+(z[INDEX(nxa,0,ix)]-zmin)*zc;
		ibc=(15.0-x)*ny/(ymax-ymin)/yc;
		gsaf_move(x,y);

		for (iy=1; iy<ny; iy++) {
			x=15.0+(yd[iy]-ymin)*yc-(xd[ix]-xmin)*xc;
			y= 2.0+(yd[iy]-ymin)*yc+(z[INDEX(nxa,iy,ix)]-zmin)*zc;
			
			if (y > cp[9999+ibc-iy]) {
				if (ix/1*1 == ix) gsaf_draw(x,y);
				cp[9999+ibc-iy]=y;
			} else {
				gsaf_move(x,y);
			}
		}
	}

	gsaf_move(15.0,2.0);
	gsaf_draw(21.0,8.0);
	gsaf_move(15.0,2.0);
	gsaf_draw(5.0,2.0);
	gsaf_draw(5.0,12.0);

	for (jy=0; jy<=10; jy++) {
		x=15.0+jy*0.5;
		y= 2.0+jy*0.5;
		gsaf_move(x,y);
		if (jy/5*5 == jy) {
			gsaf_draw(x+0.3,y);
		} else {
			gsaf_draw(x+0.15,y);
		}
	}

	for (jx=0; jx<=10; jx++) {
		x=15.0-jx;
		y= 2.0;
		gsaf_move(x,y);
		if (jx/5*5 == jx) {
			gsaf_draw(x-0.2,y-0.2);
		} else {
			gsaf_draw(x-0.1,y-0.1);
		}
	}

	for (jz=0; jz<=10; jz++) {
		x=5.0;
		y=2.0+jz;
		gsaf_move(x,y);
		if (jz/5*5 == jz) {
			gsaf_draw(x-0.2,y);
		} else {
			gsaf_draw(x-0.1,y);
		}
	}

	if (fabs(ymax) > 1.e-20) {
		ioym=log10(fabs(ymax));
	} else {
		ioym=1;
	}
	if (fabs(ymin) > 1.e-20) {
		ioyb=log10(fabs(ymin));
	} else {
		ioyb=1;
	}
	ioy=MAX(ioym,ioyb);

	for (jy=0; jy<=10; jy+=5) {
		ry = ymin+jy*ys;
		x = 15.0+jy*0.5;
		y =  2.0+jy*0.5;
		gsaf_move(x-2.5+0.4*ioy,y);
		gsaf_numbr(ry, "%13.2f", 13);
	}

	for (jx=0; jx<=10; jx+=5) {
		rx=xmin+jx*xs;
		x=15.0-jx;
		y= 1.0;
		gsaf_move(x-3.0,y);
		gsaf_numbr(rx, "%13.2f", 13);
	}

	for (jz=0; jz<=10; jz+=5) {
		rz=zmin+jz*zs;
		x=0.0;
		y=2.0+jz;
		gsaf_move(x,y);
		gsaf_numbr(rz, "%13.2f", 13);
	}
	gsaf_pagee ();
	return;
}

static void graph1 (float pxmin, float pxmax, float pymin, float pymax,
		    float *gx, float *gy, int nxm, int nxmax, int ngmax)
{
	float xmin, xmax;
	float ymin, ymax;
	float ymin1, ymax1;
	int ng;
	float gxmin,gxmax,gxscal;
	float gymin,gymax,gyscal;
	float gxorg, gyorg;

	gsaf_gmnmx1 (gx, 0, nxmax-1, 1, &xmin, &xmax);
	gsaf_gmnmx1 (&gy[INDEX(nxm,0,0)], 0, nxmax-1, 1, &ymin, &ymax);
	
	for (ng=1; ng<ngmax; ng++) {
		gsaf_gmnmx1 (&gy[INDEX(nxm,0,ng)], 0, nxmax-1, 1, &ymin1, &ymax1);
		ymin = MIN(ymin,ymin1);
		ymax = MAX(ymax,ymax1);
	}
	gsaf_gqscal (xmin, xmax, &gxmin, &gxmax, &gxscal);
	gsaf_gqscal (ymin, ymax, &gymin, &gymax, &gyscal);

	if (gxmax*gxmax > 0.0) {
		gxorg=gxmin;
	} else {
		gxorg=0.0;
	}
	if (gymax*gymax > 0.0) {
		gyorg=gymin;
	} else {
		gyorg=0.0;
	}

	gsaf_setchs (0.35,0.0);
	gsaf_gdefin (pxmin,pxmax,pymin,pymax,gxmin,gxmax,gymin,gymax);
	gsaf_gframe ();
	gsaf_gscale(gxorg,gxscal,gyorg,gyscal,0.3,9);
	gsaf_gvalue(gxorg,2*gxscal,gyorg,2*gyscal,-2);

	for (ng=0; ng<ngmax; ng++) {
		gsaf_setlin (-1,-1,7-ng%5);
		gsaf_gplotp (gx, &gy[INDEX(nxm,0,ng)], 0, nxmax-1, 1, 0, 0, ng%8);
	}
	gsaf_setlin(-1,-1,7);
}

