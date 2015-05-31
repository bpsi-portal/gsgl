#include "gsgl.h"
#include <stdio.h>
#include <math.h>

#define NXMAX 101
#define NYMAX 101

int main ()
{
	float z[NYMAX][NXMAX], x[NXMAX], y[NYMAX], x2[NYMAX][NXMAX], y2[NYMAX][NXMAX];
	int ielm[NXMAX][3];
	float z3[NXMAX], x3[NXMAX], y3[NXMAX];
	int ixmax=31;
	int iymax=31;
	float f = 1.e+0;
	int i, j;
	float zmin, zmax;
	float xmin, xmax;
	float ymin, ymax;
	float gxmin,gxmax,xscal;
	float gymin,gymax,yscal;
	float gzmin,gzmax,zscal;
	float xl=15.0; 
	float yl=15.0;
	float zl=6.0;
	float phi=45.0;
	float theta=45.0;
	float radius=15.0;
	int error;
	float ox, oy, oz;
	float dx, dy;
	float dt;
	int nnode, nelm;

	gsaf_gsopen ();

	for (i=0; i<ixmax; i++) {
		for (j=0; j<iymax; j++) {
			x[i] = 0.2 * i - 5.0;
			y[j] = 0.1 * j - 5.0;
			z[j][i] = (1.5*cos(x[i])*sin(y[j])-0.6)*f;
		}
	}
	zmin= -1.0*f;
	zmax= 1.0*f;

	do {
		error = gsgl_stdin (" # INPUT PHI,THETA\n", 2, "%f%*c%f", &phi, &theta);
		if (error == EOF) goto EXIT;
	} while (error == 1);

	gsaf_gmnmx1 (x,0,ixmax-1,1,&xmin,&xmax);
	gsaf_gmnmx1 (y,0,iymax-1,1,&ymin,&ymax);
	gsaf_gqscal (xmin,xmax,&gxmin,&gxmax,&xscal);
	gsaf_gqscal (ymin,ymax,&gymin,&gymax,&yscal);
	gsaf_gqscal (zmin,zmax,&gzmin,&gzmax,&zscal);
	ox = 0.5*(xmin+xmax);
	oy = 0.5*(ymin+ymax);
	oz = 0.5*(zmin+zmax);

	/*********************************************************************/
	gsaf_pages ();
	gsaf_setchs(0.3, 0.0);

	gsgl_projimgscale (1.4, 1.4);
	gsgl_projimgtranslate (0.0, 1.0);
	gsaf_gdefin3d (xl ,yl ,zl, xmin, xmax, ymin, ymax, zmin, zmax);
	gsaf_set3dcoord_polar1 (ox, oy, oz, radius, theta, phi);

	gsaf_gscale3dx (xmin, xscal, 0.3, 0);
	gsaf_gscale3dy (ymin, yscal, 0.3, 0);
	gsaf_gscale3dz (0.0,  zscal, 0.3, 0);
	gsaf_gvalue3dx (xmin, xscal, 1, 1);
	gsaf_gvalue3dy (ymin, yscal, 1, 1);
	gsaf_gvalue3dz (0.0,  zscal, 3, -1);
	gsaf_pers3d1 (&z[0][0],NXMAX,ixmax,iymax,3,gsaf_r2g2b);
	gsaf_gaxis3d (2);
	gsaf_gdrawback3d (0.3, 0.3, 0.3);
	gsaf_pagee ();
	/********************************************************************/
	gsaf_pages ();
	gsaf_setchs(0.35,0.0);

	gsgl_projimgscale (1.4, 1.4);
	gsgl_projimgtranslate (0.0, 1.0);
	gsaf_gdefin3d (xl ,yl ,zl, xmin, xmax, ymin, ymax, zmin, zmax);
	gsaf_set3dcoord_polar1 (ox, oy, oz, radius, theta, phi);

	gsaf_gscale3dx (xmin, xscal, 0.3, 0);
	gsaf_gscale3dy (ymin, yscal, 0.3, 0);
	gsaf_gscale3dz (0.0,  zscal, 0.3, 0);
	gsaf_gvalue3dx (xmin, xscal, 1, 1);
	gsaf_gvalue3dy (ymin, yscal, 1, 1);
	gsaf_gvalue3dz (0.0,  zscal, 1, -1);
	gsaf_pers3d1 (&z[0][0], NXMAX, ixmax, iymax, 8, gsaf_r2g2b_17);
	gsaf_gaxis3d (2);
	gsaf_gdrawback3d (0.3, 0.3, 0.3);
	gsaf_pagee ();
	/******************************************************************/
	ixmax=33;
	iymax=33;
	dx = 6.0/ixmax;
	dy = 4.0/iymax;

	for (i=0; i<ixmax; i++) {
		for (j=0; j<iymax; j++) {
			if (i <= 2*ixmax/3-2) {
				x[i] = i*(3.0/ixmax);
			} else {
				x[i] = (i-2*ixmax/3)*(12.0/ixmax)+2.0;
			}
			if (j <= iymax/3+1-1) {
				y[j] = j*(6.0/iymax);
			} else {
				y[j] = (j-iymax/3)*(3.0/iymax)+2.0;
			}
			z[j][i] = (exp(-pow(x[i]-1.0,2.0)-pow(y[j]-3.0,2.0)))*f;
		}
	}
	gsaf_gmnmx1 (x,0,ixmax-1,1,&xmin,&xmax);
	gsaf_gmnmx1 (y,0,iymax-1,1,&ymin,&ymax);
	gsaf_gqscal (xmin,xmax,&gxmin,&gxmax,&xscal);
	gsaf_gqscal (ymin,ymax,&gymin,&gymax,&yscal);
	gsaf_gqscal (zmin,zmax,&gzmin,&gzmax,&zscal);
	ox = 0.5*(xmin+xmax);
	oy = 0.5*(ymin+ymax);
	oz = 0.5*(zmin+zmax);
	/******************************************************************/
	gsaf_pages ();
	gsaf_setchs (0.35,0.0);

	gsgl_projimgscale (1.4, 1.4);
	gsgl_projimgtranslate (0.0, 1.0);
	gsaf_gdefin3d (xl ,yl ,zl, xmin, xmax, ymin, ymax, zmin, zmax);
	gsaf_set3dcoord_polar1 (ox, oy, oz, radius, theta, phi);

	gsaf_gscale3dx (xmin, xscal, 0.3, 0);
	gsaf_gscale3dy (ymin, yscal, 0.3, 0);
	gsaf_gscale3dz (0.0,  zscal, 0.3, 0);
	gsaf_gvalue3dx (xmin, xscal, 1, 1);
	gsaf_gvalue3dy (ymin, yscal, 1, 1);
	gsaf_gvalue3dz (0.0,  zscal, 1, -1);
	gsaf_pers3d2 (&z[0][0], x, y, NXMAX, ixmax, iymax, 3, gsaf_r2g2b);
	gsaf_gaxis3d (0);
	gsaf_gdrawback3d (0.3, 0.3, 0.3);
	gsaf_pagee ();
	/******************************************************************/
	ixmax=33;
	iymax=33;
	dx = 6.0/ixmax;
	dy = 4.0/iymax;

	for (i=0; i<ixmax; i++) {
		for (j=0; j<iymax; j++) {
			x[i] = 0.2*i;
			z[j][i] = 0.7 * exp(-0.5*pow(x[i]-2.0,2.0)) + 0.1;
		}
	}
	gsaf_gmnmx1 (x,0,ixmax-1,1,&xmin,&xmax);
	xmin = -xmax;
	ymin = xmin;
	ymax = xmax;
	zmin = 0.0;
	zmax = 1.0;
	gsaf_gqscal(xmin,xmax,&gxmin,&gxmax,&xscal);
	gsaf_gqscal(ymin,ymax,&gymin,&gymax,&yscal);
	gsaf_gqscal(zmin,zmax,&gzmin,&gzmax,&zscal);
	ox = 0.5*(xmin+xmax);
	oy = 0.5*(ymin+ymax);
	oz = 0.5*(zmin+zmax);
	/*****************************************************************/
	gsaf_pages ();
	gsaf_setchs (0.50,0.0);

	gsgl_projimgscale (1.4, 1.4);
	gsgl_projimgtranslate (0.0, 1.0);
	gsaf_gdefin3d (xl ,yl ,zl, xmin, xmax, ymin, ymax, zmin, zmax);
	gsaf_set3dcoord_polar1 (ox, oy, oz, radius, theta, phi);

	gsaf_gscale3dx (xmin, xscal, 0.3, 0);
	gsaf_gscale3dy (ymin, yscal, 0.3, 0);
	gsaf_gscale3dz (0.0,  zscal, 0.3, 0);
	gsaf_gvalue3dx (xmin, xscal, 1, 1);
	gsaf_gvalue3dy (ymin, yscal, 1, 1);
	gsaf_gvalue3dz (0.0,  zscal, 1, 1);
	gsaf_pers3d3 (&z[0][0], x, NXMAX, ixmax, iymax, 3, gsaf_r2g2b);
	gsaf_gaxis3d (0);
	gsaf_gdrawback3d (0.3, 0.3, 0.3);
	gsaf_pagee ();
	/****************************************************************/
	ixmax=33;
	iymax=33;
	dt = 120.0/(iymax-1);

	for (i=0; i<ixmax; i++) {
		for (j=0; j<iymax; j++) {
			x[i] = 0.2*i;
			y[j] = 180.0 * asin(2.0/(iymax-1)*j-1.0)/3.1415927;
			z[j][i] = 0.7 * exp(-0.5*pow(x[i]-2.0,2.0)) + 0.1;
		}
	}
	gsaf_gmnmx1(x,0,ixmax-1,1,&xmin,&xmax);
	xmin = 0.0;
	ymin = -xmax;
	ymax = xmax;
	zmin = 0.0;
	zmax = 1.0;
	gsaf_gqscal(xmin,xmax,&gxmin,&gxmax,&xscal);
	gsaf_gqscal(ymin,ymax,&gymin,&gymax,&yscal);
	gsaf_gqscal(zmin,zmax,&gzmin,&gzmax,&zscal);
	ox = 0.5*(xmin+xmax);
	oy = 0.5*(ymin+ymax);
	oz = 0.5*(zmin+zmax);
	/****************************************************************/
	gsaf_pages ();
	gsaf_setchs (0.50,0.0);

	gsgl_projimgscale (1.4, 1.4);
	gsgl_projimgtranslate (0.0, 1.0);
	gsaf_gdefin3d (xl ,yl ,zl, xmin, xmax, ymin, ymax, zmin, zmax);
	gsaf_set3dcoord_polar1 (ox, oy, oz, radius, theta, phi);

	gsaf_gscale3dx (xmin, xscal, 0.3, 0);
	gsaf_gscale3dy (ymin, yscal, 0.3, 0);
	gsaf_gscale3dz (0.0,  zscal, 0.3, 0);
	gsaf_gvalue3dx (xmin, xscal, 1, 1);
	gsaf_gvalue3dy (ymin, yscal, 1, 1);
	gsaf_gvalue3dz (0.0,  zscal, 1, 1);
	gsaf_pers3d4 (&z[0][0], x, y, NXMAX, ixmax, iymax, 3, gsaf_r2w2b);
	gsaf_gaxis3d (0);
	gsaf_gdrawback3d (0.3, 0.3, 0.3);
	gsaf_pagee ();
	/****************************************************************/
	ixmax=31;
	iymax=31;
	dy = 2.0 * 3.1415927 / (iymax-1);

	for (i=0; i<ixmax; i++) {
		for (j=0; j<iymax; j++) {
			x2[j][i] = 0.3 * i * (2.0  + cos(j*dy));
			y2[j][i] = i * (2.0  + sin(j*dy));
			z [j][i] = 0.7 * sin(0.1*x2[j][i]*y2[j][i]);
		}
	}
	gsaf_gmnmx2 (&x2[0][0],NXMAX,0,ixmax-1,1,0,iymax-1,1,&xmin,&xmax);
	gsaf_gmnmx2 (&y2[0][0],NXMAX,0,ixmax-1,1,0,iymax-1,1,&ymin,&ymax);
	gsaf_gmnmx2 (&z[0][0], NXMAX,0,ixmax-1,1,0,iymax-1,1,&zmin,&zmax);
	gsaf_gqscal(xmin,xmax,&gxmin,&gxmax,&xscal);
	gsaf_gqscal(ymin,ymax,&gymin,&gymax,&yscal);
	gsaf_gqscal(zmin,zmax,&gzmin,&gzmax,&zscal);
	ox = 0.5*(xmin+xmax);
	oy = 0.5*(ymin+ymax);
	oz = 0.5*(zmin+zmax);
	/****************************************************************/
	gsaf_pages ();
	gsaf_setchs (0.35,0.0);

	gsgl_projimgscale (1.4, 1.4);
	gsgl_projimgtranslate (0.0, 1.0);
	gsaf_gdefin3d (xl ,yl ,zl, xmin, xmax, ymin, ymax, zmin, zmax);
	gsaf_set3dcoord_polar1 (ox, oy, oz, radius, theta, phi);

	gsaf_gscale3dx (xmin, xscal, 0.3, 0);
	gsaf_gscale3dy (ymin, yscal, 0.3, 0);
	gsaf_gscale3dz (0.0,  zscal, 0.3, 0);
	gsaf_gvalue3dx (xmin, xscal, 1, 1);
	gsaf_gvalue3dy (ymin, yscal, 1, 1);
	gsaf_gvalue3dz (0.0,  zscal, 1, 1);
	gsaf_pers3d5 (&z[0][0], &x2[0][0], &y2[0][0], NXMAX, ixmax, iymax, -7, gsaf_r2w2b);
	gsaf_gaxis3d (0);
	gsaf_gdrawback3d (0.3, 0.3, 0.3);
	gsaf_pagee ();
	/****************************************************************/
	nnode = 11 * 5 + 5 * 4;
	nelm = 10 * 4 * 2 + 4 * 4 * 2;
	for (i=0; i<11; i++) {
		x3[i] = i;
		x3[i+11] = i;
		x3[i+22] = i;
		x3[i+33] = i;
		x3[i+44] = i;
	}
	for (i=0; i<5; i++) {
		x3[i+55] = i + 4;
		x3[i+60] = i + 4;
		x3[i+65] = i + 4;
		x3[i+70] = i + 4;
	}
	for (i=0; i<11; i++) {
		y3[i] = 0.0;
		y3[i+11] = 1.0;
		y3[i+22] = 2.0;
		y3[i+33] = 3.0;
		y3[i+44] = 4.0;
	}
	for (i=0; i<5; i++) {
		y3[i+55] = 5.0;
		y3[i+60] = 6.0;
		y3[i+65] = 7.0;
		y3[i+70] = 8.0;
	}
	for (i=0; i<nnode; i++) 
		z3 [i] = 0.7 * sin(0.1*x3[i]*y3[i]);

	for (i=0; i<20; i++) {
		if (i%2 == 1) {
			ielm[i][0] = (i+1)/2;
			ielm[i][1] = 12+(i+1)/2;
			ielm[i][2] = 11+(i+1)/2;
			ielm[i+20][0] = 11+(i+1)/2;
			ielm[i+20][1] = 23+(i+1)/2;
			ielm[i+20][2] = 22+(i+1)/2;
			ielm[i+40][0] = 22+(i+1)/2;
			ielm[i+40][1] = 34+(i+1)/2;
			ielm[i+40][2] = 33+(i+1)/2;
			ielm[i+60][0] = 33+(i+1)/2;
			ielm[i+60][1] = 45+(i+1)/2;
			ielm[i+60][2] = 44+(i+1)/2;
		} else {
			ielm[i][0] = 1+i/2;
			ielm[i][1] = 2+i/2;
			ielm[i][2] = 13+i/2;
			ielm[i+20][0] = 12+i/2;
			ielm[i+20][1] = 13+i/2;
			ielm[i+20][2] = 24+i/2;
			ielm[i+40][0] = 23+i/2;
			ielm[i+40][1] = 24+i/2;
			ielm[i+40][2] = 35+i/2;
			ielm[i+60][0] = 34+i/2;
			ielm[i+60][1] = 35+i/2;
			ielm[i+60][2] = 46+i/2;
		}
	}
	for (i=0; i<8; i++) {
		if (i%2 == 1) {
			ielm[i+80][0] = 48+(i+1)/2;
			ielm[i+80][1] = 56+(i+1)/2;
			ielm[i+80][2] = 55+(i+1)/2;
			ielm[i+88][0] = 60-5+(i+1)/2;
			ielm[i+88][1] = 66-5+(i+1)/2;
			ielm[i+88][2] = 65-5+(i+1)/2;
			ielm[i+96][0] = 65-5+(i+1)/2;
			ielm[i+96][1] = 71-5+(i+1)/2;
			ielm[i+96][2] = 70-5+(i+1)/2;
			ielm[i+104][0] = 70-5+(i+1)/2;
			ielm[i+104][1] = 76-5+(i+1)/2;
			ielm[i+104][2] = 75-5+(i+1)/2;
		} else {
			ielm[i+80][0] = 49+i/2;
			ielm[i+80][1] = 50+i/2;
			ielm[i+80][2] = 57+i/2;
			ielm[i+88][0] = 61-5+i/2;
			ielm[i+88][1] = 62-5+i/2;
			ielm[i+88][2] = 67-5+i/2;
			ielm[i+96][0] = 66-5+i/2;
			ielm[i+96][1] = 67-5+i/2;
			ielm[i+96][2] = 72-5+i/2;
			ielm[i+104][0] = 71-5+i/2;
			ielm[i+104][1] = 72-5+i/2;
			ielm[i+104][2] = 77-5+i/2;
		}
	}
	gsaf_gmnmx1 (x3,0,nnode-1,1,&xmin,&xmax);
	gsaf_gmnmx1 (y3,0,nnode-1,1,&ymin,&ymax);
	gsaf_gmnmx1 (z3,0,nnode-1,1,&zmin,&zmax);
	gsaf_gqscal (xmin,xmax,&gxmin,&gxmax,&xscal);
	gsaf_gqscal (ymin,ymax,&gymin,&gymax,&yscal);
	gsaf_gqscal (zmin,zmax,&gzmin,&gzmax,&zscal);
	ox = 0.5*(xmin+xmax);
	oy = 0.5*(ymin+ymax);
	oz = 0.5*(zmin+zmax);
	/*****************************************************************/
	gsaf_pages ();
	gsaf_setchs (0.35,0.0);

	gsgl_projimgscale (1.4, 1.4);
	gsgl_projimgtranslate (0.0, 1.0);
	gsaf_gdefin3d (xl, yl, zl, xmin, xmax, ymin, ymax, zmin, zmax);
	gsaf_set3dcoord_polar1 (ox, oy, oz, radius, theta, phi);

	gsaf_gscale3dx (xmin, xscal, 0.3, 0);
	gsaf_gscale3dy (ymin, yscal, 0.3, 0);
	gsaf_gscale3dz (0.0,  zscal, 0.3, 0);
	gsaf_gvalue3dx (xmin, xscal, 1, 1);
	gsaf_gvalue3dy (ymin, yscal, 1, 1);
	gsaf_gvalue3dz (0.0,  zscal, 1, 1);
	gsaf_pers3df (z3, x3, y3, nnode, &ielm[0][0], nelm, 3, gsaf_r2g2b);
	gsaf_gaxis3d (0);
	gsaf_gdrawback3d (0.3, 0.3, 0.3);
	gsaf_pagee ();
	/****************************************************************/
 EXIT:
	gsaf_gsclos ();
	return 0;
}
