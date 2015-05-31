#include "gsgl.h"
#include <stdio.h>

int main ()
{
	float zmin = -1.0;
	float zmax =  1.0;
	float xmin = -2.0;
	float xmax =  2.0;
	float ymin = -1.0;
	float ymax =  1.0;
	float xl=15.0;
	float yl=15.0;
	float zl=6.0;
	float theta=60.0;
	float radius=50.0;
	float gxmin,gxmax,xscal;
	float gymin,gymax,yscal;
	float gzmin,gzmax,zscal;
	int error;
	int i;
	float size;

	gsaf_gqscal (xmin,xmax,&gxmin,&gxmax,&xscal);
	gsaf_gqscal (ymin,ymax,&gymin,&gymax,&yscal);
	gsaf_gqscal (zmin,zmax,&gzmin,&gzmax,&zscal);

	gsaf_gsopen ();

	do {
		error = gsgl_stdin (" # INPUT THETA\n", 1, " %d", &theta);
		if (error == EOF) goto EXIT;
	} while (error == 1);
	
	for (i=30; i<=360; i+=45) {
		gsaf_pages ();
		gsaf_setchs (0.35, 0.0);
		gsaf_setfnt (2);

		gsaf_gtextx (22.0, 17.0, "@$#f$#=@", 0);
		gsaf_inqtsz ("$#f$#=", &size);
		gsaf_gnumbi (22.0+size, 17.0, i, 0);

		gsaf_setfnt (0);
		gsaf_gdefin3d (xl, yl, zl, xmin, xmax, ymin, ymax, zmin, zmax);
		gsaf_set3dcoord_polar1 (0.5*(xmin+xmax), 0.5*(ymin+ymax), 0.5*(zmin+zmax),
					radius, theta, i);
		gsaf_setprojectiontype (GSGL_ORTHOGONAL);
		
		gsaf_gscale3dx (xmin, xscal, 0.3, 1);
		gsaf_gscale3dy (ymin, yscal, 0.3, 1);
		gsaf_gscale3dz (0.0,  zscal, 0.3, 1);
		gsaf_gvalue3dx (xmin, xscal, 1, 1);
		gsaf_gvalue3dy (ymin, yscal, 1, 1);
		gsaf_gvalue3dz (0.0,  zscal, 1, 1);
		
		gsaf_gscale3dx (xmin, xscal, 0.3, 11);
		gsaf_gscale3dy (ymin, yscal, 0.3, 11);
		gsaf_gscale3dz (0.0,  zscal, 0.3, 11);
		gsaf_gvalue3dx (xmin, xscal, 11, 1);
		gsaf_gvalue3dy (ymin, yscal, 11, 1);
		gsaf_gvalue3dz (0.0,  zscal, 11, 1);
		
		gsaf_gscale3dx (xmin, xscal, 0.3, 21);
		gsaf_gscale3dy (ymin, yscal, 0.3, 21);
		gsaf_gscale3dz (0.0,  zscal, 0.3, 21);
		gsaf_gvalue3dx (xmin, xscal, 21, 1);
		gsaf_gvalue3dy (ymin, yscal, 21, 1);
		gsaf_gvalue3dz (0.0,  zscal, 21, 1);
		
		gsaf_gscale3dx (xmin, xscal, 0.3, 31);
		gsaf_gscale3dy (ymin, yscal, 0.3, 31);
		gsaf_gscale3dz (0.0,  zscal, 0.3, 31);
		gsaf_gvalue3dx (xmin, xscal, 31, 1);
		gsaf_gvalue3dy (ymin, yscal, 31, 1);
		gsaf_gvalue3dz (0.0,  zscal, 31, 1) ;     
		
		gsaf_gaxis3d (2);
		gsaf_pagee ();
	}
	
	for (i=30; i<=360; i+=45) {
		gsaf_pages ();
		gsaf_setchs (0.35,0.0);
		gsaf_setfnt (2);

		gsaf_gtextx (22.0, 17.0, "@$#f$#=@", 0);
		gsaf_inqtsz ("$#f$#=", &size);
		gsaf_gnumbi (22.0+size, 17.0, i, 0);

		gsaf_setfnt (0);
		gsaf_gdefin3d (xl, yl, zl, xmin, xmax, ymin, ymax, zmin, zmax);
		gsaf_set3dcoord_polar1 (0.5*(xmin+xmax), 0.5*(ymin+ymax), 0.5*(zmin+zmax),
					radius, theta, i);
		gsaf_setprojectiontype (GSGL_ORTHOGONAL);
		
		gsaf_gscale3dx (xmin, xscal, 0.3, 4);
		gsaf_gscale3dy (ymin, yscal, 0.3, 4);
		gsaf_gscale3dz (0.0,  zscal, 0.3, 4);
		gsaf_gvalue3dx (xmin, xscal, 4, 1);
		gsaf_gvalue3dy (ymin, yscal, 4, 1);
		gsaf_gvalue3dz (0.0,  zscal, 4, 1);
		
		gsaf_gscale3dx (xmin, xscal, 0.3, 14);
		gsaf_gscale3dy (ymin, yscal, 0.3, 14);
		gsaf_gscale3dz (0.0,  zscal, 0.3, 14);
		gsaf_gvalue3dx (xmin, xscal, 14, 1);
		gsaf_gvalue3dy (ymin, yscal, 14, 1);
		gsaf_gvalue3dz (0.0,  zscal, 14, 1);

		gsaf_gscale3dx (xmin, xscal, 0.3, 24);
		gsaf_gscale3dy (ymin, yscal, 0.3, 24);
		gsaf_gscale3dz (0.0,  zscal, 0.3, 24);
		gsaf_gvalue3dx (xmin, xscal, 24, 1);
		gsaf_gvalue3dy (ymin, yscal, 24, 1);
		gsaf_gvalue3dz (0.0,  zscal, 24, 1);

		gsaf_gscale3dx (xmin, xscal, 0.3, 34);
		gsaf_gscale3dy (ymin, yscal, 0.3, 34);
		gsaf_gscale3dz (0.0,  zscal, 0.3, 34);
		gsaf_gvalue3dx (xmin, xscal, 34, 1);
		gsaf_gvalue3dy (ymin, yscal, 34, 1);
		gsaf_gvalue3dz (0.0,  zscal, 34, 1);

		gsaf_gaxis3d (2);
		gsaf_pagee ();
	}

	for (i=30; i<=360; i+=45) {
		gsaf_pages ();
		gsaf_setchs (0.35,0.0);
		gsaf_setfnt (2);

		gsaf_gtextx (22.0, 17.0, "@$#f$#=@", 0);
		gsaf_inqtsz ("$#f$#=", &size);
		gsaf_gnumbi (22.0+size, 17.0, i, 0);

		gsaf_setfnt (0);
		gsaf_gdefin3d (xl, yl, zl, xmin, xmax, ymin, ymax, zmin, zmax);
		gsaf_set3dcoord_polar1 (0.5*(xmin+xmax), 0.5*(ymin+ymax), 0.5*(zmin+zmax),
					radius, theta, i);
		gsaf_setprojectiontype (GSGL_ORTHOGONAL);

		gsaf_gscale3dx (xmin, xscal, 0.3, 5);
		gsaf_gscale3dy (ymin, yscal, 0.3, 5);
		gsaf_gscale3dz (0.0,  zscal, 0.3, 5);
		gsaf_gvalue3dx (xmin, xscal, 5, 1);
		gsaf_gvalue3dy (ymin, yscal, 5, 1);
		gsaf_gvalue3dz (0.0,  zscal, 5, 1);

		gsaf_gscale3dx (xmin, xscal, 0.3, 15);
		gsaf_gscale3dy (ymin, yscal, 0.3, 15);
		gsaf_gscale3dz (0.0,  zscal, 0.3, 15);
		gsaf_gvalue3dx (xmin, xscal, 15, 1);
		gsaf_gvalue3dy (ymin, yscal, 15, 1);
		gsaf_gvalue3dz (0.0,  zscal, 15, 1);

		gsaf_gscale3dx (xmin, xscal, 0.3, 25);
		gsaf_gscale3dy (ymin, yscal, 0.3, 25);
		gsaf_gscale3dz (0.0,  zscal, 0.3, 25);
		gsaf_gvalue3dx (xmin, xscal, 25, 1);
		gsaf_gvalue3dy (ymin, yscal, 25, 1);
		gsaf_gvalue3dz (0.0,  zscal, 25, 1);

		gsaf_gscale3dx (xmin, xscal, 0.3, 35);
		gsaf_gscale3dy (ymin, yscal, 0.3, 35);
		gsaf_gscale3dz (0.0,  zscal, 0.3, 35);
		gsaf_gvalue3dx (xmin, xscal, 35, 1);
		gsaf_gvalue3dy (ymin, yscal, 35, 1);
		gsaf_gvalue3dz (0.0,  zscal, 35, 1);
     
		gsaf_gaxis3d (2);
		gsaf_pagee ();
	}
 EXIT:
	gsaf_gsclos ();
	return 0;
}
