#include "gsgl.h"
#include <math.h>

static void draw_back (float xl, float yl, float zl);

#define nmx 101

int main ()
{
	float x[nmx], y[nmx], z[nmx][nmx];
	float vx[3], vy[3], vz[3];
	float xmin, xmax, ymin, ymax, zmin, zmax;
	int i, j;
	int ixmax, iymax;
	float xl, yl, zl;
	float phi, theta, radius;
	float gxmin,gxmax,xscal;
	float gymin,gymax,yscal;
	float gzmin,gzmax,zscal;
	float ox, oy, oz;
	float r, dt;
	float v1, v2, v3;

	gsaf_gsopen ();

	gsaf_pages ();
	gsaf_setchs (0.35,0.0);

	xmin = 2.0;
	xmax = 24.0;
	x[0] = xmin;
	x[1] = xmin;
	x[2] = xmax;
	x[3] = xmax;

	ymin = 2.0;
	ymax = 17.0;
	y[0] = ymin;
	y[1] = ymax;
	y[2] = ymax;
	y[3] = ymin   ;   

	gsaf_gdefin (xmin, xmax, ymin, ymax, 0.0, 1.0, 0.0, 1.0);

	for (i=1; i<=91; i++) {
		gsgl_animes ();

		x[0] = xmin;
		x[1] = xmin;
		x[2] = xmax;
		x[3] = xmax;
		y[0] = ymin;
		y[1] = ymax;
		y[2] = ymax;
		y[3] = ymin;
		gsaf_setrgb (1.0, 1.0, 0.5);
		gsaf_poly (x, y, 4);

		x[0] = (i-1) * 0.01 ;
		x[1] = (i-1) * 0.01 ;
		x[2] = (i-1) * 0.01 + 0.1;
		x[3] = (i-1) * 0.01 + 0.1;
		y[0] = (i-1) * 0.01;
		y[1] = (i-1) * 0.01 + 0.1;
		y[2] = (i-1) * 0.01 + 0.1;
		y[3] = (i-1) * 0.01;
		gsaf_setrgb (1.0, 0.0, 0.0);
		gsaf_poly2d (x, y, 4);

		gsaf_setrgb(-1.0, 0.0, 0.0);
		gsaf_gframe ();

		gsgl_animee ();
	}
	gsaf_pagee ();

	ixmax = 31;
	iymax = 31;

	for (i=0; i<ixmax; i++) {
		for (j=0; j<iymax; j++) {
			x[i] = 0.2 * i - 5.0;
			y[j] = 0.1 * j - 5.0;
			z[j][i] = (1.5*cos(x[i])*sin(y[j])-0.6);
		}
	}

	zmin= -1.0;
	zmax= 1.0;

	xl=15.0;
	yl=15.0;
	zl=6.0;

	phi=45.0;
	theta=45.0;
	radius=15.0;
	
	gsaf_gmnmx1 (x,0,ixmax-1,1,&xmin,&xmax);
	gsaf_gmnmx1 (y,0,iymax-1,1,&ymin,&ymax);
	gsaf_gqscal (xmin,xmax,&gxmin,&gxmax,&xscal);
	gsaf_gqscal (ymin,ymax,&gymin,&gymax,&yscal);
	gsaf_gqscal (zmin,zmax,&gzmin,&gzmax,&zscal);
	ox = 0.5*(xmin+xmax);
	oy = 0.5*(ymin+ymax);
	oz = 0.5*(zmin+zmax);

	gsaf_pages ();
	gsaf_setchs(0.3, 0.0);

	gsgl_projimgscale (1.4, 1.4);
	gsgl_projimgtranslate (0.0, 1.0);
	gsaf_gdefin3d (xl, yl, zl, xmin, xmax, ymin, ymax, zmin, zmax);

	for (i=0; i<=10; i++) {
		phi = phi + 10.0;
		gsgl_animes ();
		gsaf_set3dcoord_polar1 (ox, oy, oz, radius, theta, phi);

		gsaf_gscale3dx (xmin, xscal, 0.3, 0);
		gsaf_gscale3dy (ymin, yscal, 0.3, 0);
		gsaf_gscale3dz (0.0,  zscal, 0.3, 0);
		gsaf_gvalue3dx (xmin, xscal, 1, 1);
		gsaf_gvalue3dy (ymin, yscal, 1, 1);
		gsaf_gvalue3dz (0.0,  zscal, 3, -1);
		gsaf_pers3d1 (&z[0][0], nmx, ixmax, iymax, 3, gsaf_r2g2b) ;
		gsaf_gaxis3d (2);
		gsgl_animee ();
	}
	gsaf_pagee ();

	gsaf_pages ();

	r = 2.0;
	xl = GSGL_PAPER_WIDTH;
	yl = GSGL_PAPER_HEIGHT;
	zl = GSGL_VIEW_DEPTH;
	vx[0] = 1.0;
	vx[1] = 0.0;
	vx[2] = 0.0;
	vy[0] = 0.0;
	vy[1] = 1.0;
	vy[2] = 0.0;
	vz[0] = 0.0;
	vz[1] = 0.0;
	vz[2] = 1.0;
	gsaf_gdefin3d (xl, yl, zl, 0.0, xl, 0.0, yl, 0.0, zl);
	ox = 0.5 * xl;
	oy = 0.5 * yl;
	oz = 0.5 * zl;
	gsaf_set3dcoord_caut1 (ox, oy, oz, oz, vx, vy, vz);
	gsaf_setrgb (1.0, 0.0, 0.0);

	phi = gsaf_gurand (8.0 * atan (1.0));
	theta = gsaf_gurand (4.0 * atan (1.0));
	v1 = 1.0 * sin(theta) * cos (phi);
	v2 = 1.0 * sin(theta) * sin (phi);
	v3 = fabs(1.0 * cos(theta));

	dt = 1.0;
	gsgl_enablelighting ();
	for (i=0; i<=100; i++) {
		gsgl_animes ();
		gsaf_setrgb (1.0, 1.0, 0.5);
		draw_back (xl, yl, zl);
		ox = ox + v1 * dt;
		oy = oy + v2 * dt;
		oz = oz + v3 * dt;
		if (ox < r) {
			ox = r + (r - ox);
			v1 = -v1;
		} else if (ox > xl-r) {
			ox = xl - r - (ox - xl + r);
			v1 = -v1;
		}
		if (oy < r) {
			oy = r + (r - oy);
			v2 = -v2;
		} else if (oy > yl-r) {
			oy = yl - r - (oy - yl + r);
			v2 = -v2;
		}
		if (oz < r) {
			oz = r + (r - oz);
			v3 = -v3;
		} else if (oz > zl-r) {
			oz = zl - r - (oz - zl + r);
			v3 = -v3;
		}
		gsaf_setrgb (1.0, 0.0, 0.0);
		gsgl_solidsphere (ox, oy, oz, vx[0], vy[0], vz[0], r, 20, 20);
		gsgl_animee ();
	}
	gsaf_pagee ();
	
	gsaf_gsclos ();
	return 0;
}


static void draw_back (float xl, float yl, float zl)
{
	float x[4],y[4],z[4];
	const float eps = 0.01;

	x[0] = 0.0 + eps;
	x[1] = 0.0 + eps;
	x[2] = 0.0 + eps;
	x[3] = 0.0 + eps;
	y[0] = 0.0 + eps;
	y[1] = yl - eps;
	y[2] = yl - eps;
	y[3] = 0.0 + eps;
	z[0] = 0.0 + eps;
	z[1] = 0.0 + eps;
	z[2] = zl - eps;
	z[3] = zl - eps;
	gsaf_poly3d (x, y, z, 4);

	x[0] = xl - eps;
	x[1] = xl - eps;
	x[2] = xl - eps;
	x[3] = xl - eps;
	y[0] = 0.0 + eps;
	y[1] = yl - eps;
	y[2] = yl - eps;
	y[3] = 0.0 + eps;
	z[0] = 0.0 + eps;
	z[1] = 0.0 + eps;
	z[2] = zl - eps;
	z[3] = zl - eps;  
	gsaf_poly3d (x, y, z, 4);

	x[0] = 0.0 + eps;
	x[1] = xl - eps;
	x[2] = xl - eps;
	x[3] = 0.0 + eps;
	y[0] = yl - eps;
	y[1] = yl - eps;
	y[2] = yl - eps;
	y[3] = yl - eps;
	z[0] = 0.0 + eps;
	z[1] = 0.0 + eps;
	z[2] = zl - eps;
	z[3] = zl - eps   ;  
	gsaf_poly3d (x, y, z, 4);

	x[0] = 0.0 + eps;
	x[1] = xl - eps;
	x[2] = xl - eps;
	x[3] = 0.0 + eps;
	y[0] = 0.0 + eps;
	y[1] = 0.0 + eps;
	y[2] = 0.0 + eps;
	y[3] = 0.0 + eps;
	z[0] = 0.0 + eps;
	z[1] = 0.0 + eps;
	z[2] = zl - eps;
	z[3] = zl - eps;  
	gsaf_poly3d (x, y, z, 4);

	x[0] = 0.0 + eps;
	x[1] = xl - eps;
	x[2] = xl - eps;
	x[3] = 0.0 + eps;
	y[0] = 0.0 + eps;
	y[1] = 0.0 + eps;
	y[2] = yl - eps;
	y[3] = yl - eps;
	z[0] = 2.0 + eps;
	z[1] = 2.0 + eps;
	z[2] = 2.0 + eps;
	z[3] = 2.0 + eps;  
	gsaf_poly3d (x, y, z, 4);
}
