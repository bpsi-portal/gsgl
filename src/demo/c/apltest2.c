#include "gsgl.h"
#include <math.h>
#include <stdio.h>

void pattern ();
void pattern2d ();
void text_in_2d ();
void pattern3d ();
void text_in_3d ();
void text_in_3d_2 ();
void text_in_3d_3 ();
void vector1 ();
void vector5 ();

int main ()
{
	int mode;
	int error;

	gsaf_gsopen ();
	
	for (;;) {
		do {
			error = gsgl_stdin (" ## INPUT TYPE (1,2,3,4,5,6,7,8,9) ?\n",
					    1, " %d", &mode);
			if (error == EOF) {
				gsaf_gsclos ();
				return 0;
			}
		} while (error == 1 || error == 2);

		if (mode == 1) {
			gsaf_pages ();
			pattern ();
			gsaf_pagee ();
		} else if (mode == 2) {
			gsaf_pages ();
			pattern2d ();
			gsaf_pagee ();
		} else if (mode == 3) {
			gsaf_pages ();
			text_in_2d ();
			gsaf_pagee ();
		} else if (mode == 4) {
			gsaf_pages ();
			pattern3d ();
			gsaf_pagee ();
		} else if (mode == 5) {
			gsaf_pages ();
			text_in_3d ();
			gsaf_pagee ();
		} else if (mode == 6) {
			gsaf_pages ();
			text_in_3d_2 ();
			gsaf_pagee ();
		} else if (mode == 7) {
			gsaf_pages ();
			text_in_3d_3 ();
			gsaf_pagee ();
		} else if (mode == 8) {
			gsaf_pages ();
			vector1 ();
			gsaf_pagee ();
		} else if (mode == 9) {
			gsaf_pages ();
			vector5 ();
			gsaf_pagee ();
		}
	}
	gsaf_gsclos ();
	return 0;
}

void pattern ()
{
	int ipat;

	for (ipat=0; ipat<=7; ipat++) {
		gsaf_movept (1.0, 16.0 - ipat * 2, ipat);
		gsaf_drawpt (24.0, 16.0 - ipat * 2);
	}
}

void pattern2d ()
{
	int ipat;

	gsaf_gdefin (0.0, 25.6, 0.0, 18.1, 0.0, 1.0, 0.0, 1.0);
	
	for (ipat=0; ipat<=7; ipat++) {
		gsaf_movept2d (0.1, 0.9 - ipat * 0.12, ipat);
		gsaf_drawpt2d (0.9, 0.9 - ipat * 0.12);
	}
}
 
void text_in_2d ()
{
	gsaf_gdefin (0.0, 25.6, 0.0, 18.1, 0.0, 1.0, 0.0, 1.0);
	gsaf_move2d (0.1, 0.1);
	gsaf_draw2d (0.2, 0.2);
	gsaf_text2d ("text2d");

	gsaf_gtext2d (0.2, 0.4, "gtext2d 1.0", 0);
	gsaf_gtext2d (0.2, 0.43, "gtext2d 1.0", 1);
	gsaf_gtext2d (0.2, 0.46, "gtext2d 1.0", 2);
	gsaf_gtext2d (0.2, 0.49, "gtext2d 1.0", 3);

	gsaf_gtextx2d (0.2, 0.6, "@$frac{dy}{dx} = f(x)@", 0);

	gsaf_gnumbi2d (0.6, 0.10, 12345, 0);
	gsaf_gnumbi2d (0.6, 0.13, 12345, 1);
	gsaf_gnumbi2d (0.6, 0.16, 12345, 2);
	gsaf_gnumbr2d (0.6, 0.20, 12345.678, 3, 0);
	gsaf_gnumbr2d (0.6, 0.23, 12345.678, 3, 1);
	gsaf_gnumbr2d (0.6, 0.26, 12345.678, 3, 2);
	gsaf_gnumbr2d (0.6, 0.29, 12345.678, 3, 3);
	gsaf_gnumbr2d (0.6, 0.40, 123.456, -3, 0);
	gsaf_gnumbr2d (0.6, 0.43, 123.456, -3, 1);
	gsaf_gnumbr2d (0.6, 0.46, 123.456, -3, 2);
	gsaf_gnumbr2d (0.6, 0.49, 123.456, -3, 3);
	gsaf_gnumbr2d (0.6, 0.60, 123.456, -103, 0);
	gsaf_gnumbr2d (0.6, 0.63, 123.456, -103, 1);
	gsaf_gnumbr2d (0.6, 0.66, 123.456, -103, 2);
	gsaf_gnumbr2d (0.6, 0.69, 123.456, -103, 3);
	gsaf_gnumbp2d (0.6, 0.80, 10, 16, 0);
	gsaf_gnumbp2d (0.6, 0.83, 10, 16, 1);
	gsaf_gnumbp2d (0.6, 0.86, 10, 16, 2);
	gsaf_gnumbp2d (0.6, 0.89, 10, 16, 3);
}

void pattern3d ()
{
	int ipat;
	float xl, yl, zl, xmin, xmax, ymin, ymax, zmin, zmax;
	float ox, oy, oz, radius, theta, phi;

	xl = 15.0;
	yl = 15.0;
	zl = 15.0;
	xmin = 0.0;
	xmax = 1.0;
	ymin = 2.0;
	ymax = 4.0;
	zmin = 100.0;
	zmax = 200.0;

	ox = 0.5 * (xmin + xmax);
	oy = 0.5 * (ymin + ymax);
	oz = 0.5 * (zmin + zmax);
	radius = 15.0;
	theta = 30.0;
	phi = 30.0;

	gsaf_gdefin3d (xl, yl, zl, xmin, xmax, ymin, ymax, zmin, zmax);
	gsaf_set3dcoord_polar1 (ox, oy, oz, radius, theta, phi);

	for (ipat=0; ipat<=7; ipat++) {
		gsaf_movept3d (0.1, 3.9 - ipat * 0.24, 150.0, ipat);
		gsaf_drawpt3d (0.9, 3.9 - ipat * 0.24, 150.0);
	}
}

void text_in_3d ()
{
	float xl, yl, zl, xmin, xmax, ymin, ymax, zmin, zmax;
	float ox, oy, oz, radius, theta, phi;

	xl = 15.0;
	yl = 15.0;
	zl = 15.0;
	xmin = 0.0;
	xmax = 1.0;
	ymin = 0.0;
	ymax = 1.0;
	zmin = 0.0;
	zmax = 1.0;

	ox = 0.5 * (xmin + xmax);
	oy = 0.5 * (ymin + ymax);
	oz = 0.5 * (zmin + zmax);
	radius = 15.0;
	theta = 10.0;
	phi = -60.0;

	gsaf_gdefin3d (xl, yl, zl, xmin, xmax, ymin, ymax, zmin, zmax);
	gsaf_set3dcoord_polar1 (ox, oy, oz, radius, theta, phi);
	gsaf_setprojectiontype (0); // orthogonal
	gsaf_set3dtextbaseline (1.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	gsaf_move3d (0.1, 0.1, 0.1);
	gsaf_draw3d (0.2, 0.2, 0.2);
	gsaf_text3d ("text3d");

	gsaf_gtext3d (0.2, 0.40, 0.1, "gtext3d 1.0", 0);
	gsaf_gtext3d (0.2, 0.43, 0.1, "gtext3d 1.0", 1);
	gsaf_gtext3d (0.2, 0.46, 0.1, "gtext3d 1.0", 2);
	gsaf_gtext3d (0.2, 0.49, 0.1, "gtext3d 1.0", 3);

	gsaf_gtextx3d (0.2, 0.6, 0.1, "@$frac{dy}{dx} = f(x)@", 0);

	gsaf_gnumbi3d (0.6, 0.10, 0.1, 12345, 0);
	gsaf_gnumbi3d (0.6, 0.13, 0.1, 12345, 1);
	gsaf_gnumbi3d (0.6, 0.16, 0.1, 12345, 2);
	gsaf_gnumbr3d (0.6, 0.20, 0.1, 12345.678, 3, 0);
	gsaf_gnumbr3d (0.6, 0.23, 0.1, 12345.678, 3, 1);
	gsaf_gnumbr3d (0.6, 0.26, 0.1, 12345.678, 3, 2);
	gsaf_gnumbr3d (0.6, 0.29, 0.1, 12345.678, 3, 3);
	gsaf_gnumbr3d (0.6, 0.40, 0.1, 123.456, -3, 0);
	gsaf_gnumbr3d (0.6, 0.43, 0.1, 123.456, -3, 1);
	gsaf_gnumbr3d (0.6, 0.46, 0.1, 123.456, -3, 2);
	gsaf_gnumbr3d (0.6, 0.49, 0.1, 123.456, -3, 3);
	gsaf_gnumbr3d (0.6, 0.60, 0.1, 123.456, -103, 0);
	gsaf_gnumbr3d (0.6, 0.63, 0.1, 123.456, -103, 1);
	gsaf_gnumbr3d (0.6, 0.66, 0.1, 123.456, -103, 2);
	gsaf_gnumbr3d (0.6, 0.69, 0.1, 123.456, -103, 3);
	gsaf_gnumbp3d (0.6, 0.80, 0.1, 10, 16, 0);
	gsaf_gnumbp3d (0.6, 0.83, 0.1, 10, 16, 1);
	gsaf_gnumbp3d (0.6, 0.86, 0.1, 10, 16, 2);
	gsaf_gnumbp3d (0.6, 0.89, 0.1, 10, 16, 3);
}

void text_in_3d_2 ()
{
	float zmin, zmax, xmin, xmax, ymin, ymax;
	float xl, yl, zl;
	float phi, theta, radius;

	zmin = -1.0;
	zmax =  1.0;
	xmin = -1.0;
	xmax =  1.0;
	ymin = -1.0;
	ymax =  1.0;

	xl=10.0;
	yl=10.0;
	zl=10.0;

	phi=50.0;
	theta=60.0;
	radius=10.0;

	gsaf_setchs(0.50,0.0);

	gsaf_gdefin3d (xl, yl, zl, xmin, xmax, ymin, ymax, zmin, zmax);
	gsaf_set3dcoord_polar1 (0.5*(xmin+xmax),0.5*(ymin+ymax),0.5*(zmin+zmax),
				radius, theta, phi);

	gsaf_set3dtextbaseline (1.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	gsaf_gtext3d (0.0, 0.0, 1.0, "x-y plane (z=1.0)", 2);
	gsaf_set3dtextbaseline (-1.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	gsaf_gtext3d (0.0, 0.0,-1.0, "x-y plane (z=-1.0)", 2);

	gsaf_set3dtextbaseline (0.0, 1.0, 0.0, 0.0, 0.0, 1.0);
	gsaf_gtext3d (1.0, 0.0, 0.0, "y-z plane (x=1.0)", 2);
	gsaf_set3dtextbaseline (0.0, -1.0, 0.0, 0.0, 0.0, 1.0);
	gsaf_gtext3d (-1.0, 0.0, 0.0, "y-z plane (x=-1.0)", 2);

	gsaf_set3dtextbaseline (0.0, 1.0, 0.0, 0.0, 0.0, 1.0);
	gsaf_gtext3d (1.0, 0.0, 0.0, "y-z plane (x=1.0)", 2);
	gsaf_set3dtextbaseline (0.0, -1.0, 0.0, 0.0, 0.0, 1.0);
	gsaf_gtext3d (-1.0, 0.0, 0.0, "y-z plane (x=-1.0)", 2);

	gsaf_set3dtextbaseline (0.0, 0.0, 1.0, 1.0, 0.0, 0.0);
	gsaf_gtext3d (0.0, 1.0, 0.0, "z-x plane (y=1.0)", 2);
	gsaf_set3dtextbaseline (0.0, 0.0,-1.0, 1.0, 0.0, 0.0);
	gsaf_gtext3d (0.0,-1.0, 0.0, "z-x plane (y=-1.0)", 2);

	gsaf_set3dtextbaseline (0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
	gsaf_setrgb (0.0, 0.0, 1.0);
	gsaf_move3d (-1.5, 0.0, 0.0);
	gsaf_draw3d (1.5, 0.0, 0.0);
	gsaf_gtext3d (1.6, 0.0, 0.0, "x-axis", 2);
	gsaf_move3d (0.0,-1.5, 0.0);
	gsaf_draw3d (0.0, 1.5, 0.0);
	gsaf_gtext3d (0.0, 1.6, 0.0, "y-axis", 2);
	gsaf_move3d (0.0, 0.0,-1.5);
	gsaf_draw3d (0.0, 0.0, 1.5);
	gsaf_gtext3d (0.0, 0.0, 1.6, "z-axis", 2);

	gsaf_gaxis3d (2);
}

void text_in_3d_3 ()
{
	float zmin, zmax, xmin, xmax, ymin, ymax;
	float xl, yl, zl;
	float phi, theta, radius;
	float pi;
	int i;

	zmin = -1.0;
	zmax =  1.0;
	xmin = -1.0;
	xmax =  1.0;
	ymin = -1.0;
	ymax =  1.0;

	xl=10.0;
	yl=10.0;
	zl=10.0;

	phi=50.0;
	theta=60.0;
	radius=10.0;

	gsaf_setchs(1.0,0.0);

	gsaf_gdefin3d(xl ,yl ,zl, xmin, xmax, ymin, ymax, zmin, zmax);
	gsaf_set3dcoord_polar1 (0.5*(xmin+xmax),0.5*(ymin+ymax),0.5*(zmin+zmax),
				radius, theta, phi);
	pi = atan(1.0)*4.0;

	for (i=0; i<=360; i+=60) {
		gsaf_setrgb (i/360.0, 0.0, 1.0-i/360.0);
		gsaf_set3dtextbaseline (cos(2.0*pi*i/360.0), sin(2.0*pi*i/360.0),
					0.0, 0.0, 0.0, 1.0);
		gsaf_move3d (0.0, 0.0, 0.0);
		gsaf_text3d ("  text in 3d space");
	}
}

void vector1 ()
{
#define NXMAX 41
#define NYMAX 31
	float vx[NYMAX][NXMAX], vy[NYMAX][NXMAX], x[NXMAX], y[NYMAX];
	int i, j;
	float dx, dy;

	dx = 1.0 / (NXMAX-1);
	dy = 1.0 / (NYMAX-1);

	for (i=0; i<NXMAX; i++)
		x[i] = dx * i;

	for (j=0; j<NYMAX; j++)
		y[j] = dy * j;

	for (i=0; i<NXMAX; i++) {		
		for (j=0; j<NYMAX; j++) {
			vx[j][i] = cos(x[i] * y[j] * 0.1);
			vy[j][i] = cos(i * j * 0.01);
		}
	}
	
	gsaf_gdefin (2.0, 23.5, 2.0, 16.0, 0.0, 1.0, 0.0, 1.0);
	gsaf_gframe ();
	gsaf_gvectr (&vx[0][0], &vy[0][0], x, y, NXMAX, 0, NXMAX-1, 1, 
		     0, NYMAX-1, 1, 2.0, 2.0, 0.2);
}

void vector5 ()
{
#define NXMAX 41
#define NYMAX 31
	float vx[NYMAX][NXMAX], vy[NYMAX][NXMAX], x[NYMAX][NXMAX], y[NYMAX][NXMAX];
	int i, j;
	float dr, dt;

	dr = 1.0 / (NXMAX-1);
	dt = 2.0 * 3.141592 / (NYMAX-1);

	for (i=0; i<NXMAX; i++)
		for (j=0; j<NYMAX; j++)
			x[j][i] = dr * i * cos(dt * j);
	
	for (i=0; i<NXMAX; i++)
		for (j=0; j<NYMAX; j++)
			y[j][i] = dr * i * sin(dt * j);

	for (i=0; i<NXMAX; i++) {		
		for (j=0; j<NYMAX; j++) {
			vx[j][i] = cos(x[j][i] * y[j][j] * 0.1);
			vy[j][i] = cos(i * j * 0.01);
		}
	}
	
	gsaf_gdefin (2.0, 23.5, 2.0, 16.0, -1.0, 1.0, -1.0, 1.0);
	gsaf_gframe ();
	gsaf_gvectr5 (&vx[0][0], &vy[0][0], &x[0][0], &y[0][0],
		      NXMAX, 0, NXMAX-1, 1, 0, NYMAX-1, 1, 2.0, 2.0, 0.2);
}
