#include "gsgl.h"

int main ()
{
	float xl = 10.0;
	float yl = 10.0;
	float zl = 10.0;
	float phi = 50.0;
	float theta = 60.0;
	float radius = 50.0;
	float xmin = 0.0;
	float ymin = 0.0;
	float zmin = 0.0;
	float xmax = 2.0;
	float ymax = 2.0;
	float zmax = 2.0;
	float vx[3], vy[3], vz[3];

	gsaf_gsopen ();

	/*2    write (6,*) 'input phi,theta'
	  read (5,*,end = 9999,err = 2) phi, theta*/

	gsaf_pages ();

	gsgl_enablelighting ();

	gsaf_gdefin3d (xl, yl, zl, xmin, xmax, ymin, ymax, zmin, zmax);
	gsaf_set3dcoord_polar1 (0.5*(xmin+xmax),0.5*(ymin+ymax),0.5*(zmin+zmax),
				radius, theta, phi);
	gsaf_setprojectiontype (GSGL_ORTHOGONAL);

	gsgl_projimgscale (0.75, 0.75);
	gsgl_projimgtranslate (-0.125*GSGL_PAPER_WIDTH, -0.125*GSGL_PAPER_HEIGHT);

	gsaf_move (0.0, 0.0);
	gsaf_draw (GSGL_PAPER_WIDTH, 0.0);
	gsaf_draw (GSGL_PAPER_WIDTH, GSGL_PAPER_HEIGHT);
	gsaf_draw (0.0, GSGL_PAPER_HEIGHT);
	gsaf_draw (0.0, 0.0);

	gsaf_setclp (0.0, GSGL_PAPER_WIDTH, 0.0, GSGL_PAPER_HEIGHT);

	gsaf_setrgb(1.0, 0.0, 0.0);

	gsgl_solidsphere (1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 0.5, 20, 20);
	gsaf_setrgb (0.2, 0.2, 1.0);
	gsgl_solidtorus (1.0, 1.0, 1.0, 1.0, 0.0, 1.0, 0.5, 2.0, 20, 20);
	gsgl_projimgidentity ();

	gsaf_offclp ();

	gsgl_projimgscale (0.4, 0.4);
	gsgl_projimgtranslate (0.3*GSGL_PAPER_WIDTH, 0.3*GSGL_PAPER_HEIGHT);
	gsaf_setrgb(-1.0, 0.0, 0.0);
	gsaf_move (0.0, 0.0);
	gsaf_draw (GSGL_PAPER_WIDTH, 0.0);
	gsaf_draw (GSGL_PAPER_WIDTH, GSGL_PAPER_HEIGHT);
	gsaf_draw (0.0, GSGL_PAPER_HEIGHT);
	gsaf_draw (0.0, 0.0);
	gsaf_setclp (0.0, GSGL_PAPER_WIDTH, 0.0, GSGL_PAPER_HEIGHT);
	gsaf_setrgb(1.0, 0.0, 0.0);
	gsgl_wiresphere(1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 0.5, 20, 20);
	gsaf_setrgb(0.2, 0.2, 1.0);
	gsgl_wiretorus(1.0, 1.0, 1.0, 1.0, 0.0, 1.0, 0.5, 2.0, 20, 20);
	gsaf_pagee ();

	/*****************************************************************/

	gsaf_pages ();

	gsgl_enablelighting ();
	gsaf_gdefin3d (xl, yl, zl, xmin, xmax, ymin, ymax, zmin, zmax);
	vx[0] = 1.0;
	vx[1] = 0.0;
	vx[2] = 0.0;
	vy[0] = 0.0;
	vy[1] = 1.0;
	vy[2] = 0.0;
	vz[0] = 0.0;
	vz[1] = 0.0;
	vz[2] = 1.0;
	gsaf_set3dcoord_caut1 (0.5*(xmin+xmax),0.5*(ymin+ymax),0.5*(zmin+zmax),
			       0.0, vx, vy, vz);

	gsaf_setrgb (0.6, 0.6, 1.0);
	gsgl_solidtorus (1.0, 1.0, 1.0, 0.0, 1.0, 0.0, 0.5, 2.0, 40, 40);
	gsaf_setrgb (0.4, 0.4, 1.0);
	gsgl_solidtorus (1.0, 1.0, 1.0, 0.0, 1.0, 0.0, 0.3, 2.0, 40, 40);
	gsaf_setrgb (0.2, 0.2, 1.0);
	gsgl_solidtorus (1.0, 1.0, 1.0, 0.0, 1.0, 0.0, 0.1, 2.0, 40, 40);

	gsaf_pagee ();

	gsaf_gsclos ();
	return 0;
}
