#include "gsgl.h"
#include <math.h>

int main ()
{
	float z[101][101], x[101], y[101];	
	int ixmax=21;
	int iymax=21;
	int i, j;
	float xmin, xmax, ymin, ymax, zmin, zmax;
	float xl=15.0;
	float yl=15.0;
	float zl=6.0;
	float phi=50.0;
	float theta=60.0;
	float radius=10.0;
	float gxmin,gxmax,xscal;
	float gymin,gymax,yscal;
	float gzmin,gzmax,zscal;

	gsaf_gsopen ();
	for (i=0; i<ixmax; i++) {
		for (j=0; j<iymax; j++) {
			x[i] = 0.1 * i;
			y[j] = 0.1 * j;
			z[j][i] = cos(x[i])*sin(y[j]);
		}
	}
	zmin= -1.0;
	zmax= 1.0;

	gsaf_pages ();
	gsaf_setchs(0.50,0.0);
	gsaf_setfnt(2);

	gsaf_gmnmx1(x,0,ixmax-1,1,&xmin,&xmax);
	gsaf_gmnmx1(y,0,iymax-1,1,&ymin,&ymax);
	gsaf_gqscal(xmin,xmax,&gxmin,&gxmax,&xscal);
	gsaf_gqscal(ymin,ymax,&gymin,&gymax,&yscal);
	gsaf_gqscal(zmin,zmax,&gzmin,&gzmax,&zscal);

	gsaf_gdefin3d(xl ,yl ,zl, xmin, xmax, ymin, ymax, zmin, zmax);
	gsaf_set3dcoord_polar1 (0.5*(xmin+xmax), 0.5*(ymin+ymax), 0.5*(zmin+zmax),
				radius, theta, phi);

	gsaf_gscale3dx( xmin, xscal, 0.3, 0);
	gsaf_gscale3dy( ymin, yscal, 0.3, 0);
	gsaf_gscale3dz( 0.0,  zscal, 0.3, 0);
	gsaf_gvalue3dx( xmin, xscal, 2, 1);
	gsaf_gvalue3dy( ymin, yscal, 2, 1);
	gsaf_gvalue3dz( 0.0,  zscal, 2, 1);

	gsaf_pers3d1 (&z[0][0],101,ixmax,iymax,8,gsaf_r2g2b_17);
	gsaf_gaxis3d(2);
	gsaf_gdrawback3d(0.3,0.3,0.3);
	gsaf_pagee ();
	gsaf_gsclos ();
	return 0;
}
