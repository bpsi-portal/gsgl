#include "gsgl.h"

int main ()
{
	float x[3], y[3], z[3];
	float xl, yl, zl;
	float phi, theta, radius;
	float xmin, xmax, ymin, ymax, zmin, zmax;

	gsaf_gsopen (); /* gsgl の初期化 */
	xl=10.0;     /* x軸の長さ */
	yl=10.0;     /* y軸の長さ */
	zl=10.0;     /* z軸の長さ */
	phi=50.0;    /* 極座標の φ */
	theta=60.0;  /* 極座標の θ */
	radius=10.0; /* 極座標の r */
	xmin=-1.0;
	ymin=-1.0;
	zmin=-1.0;
	xmax= 1.0;
	ymax= 1.0;
	zmax= 1.0;

	gsaf_pages (); /* ページの開始 */
	gsaf_gdefin3d (xl, yl, zl, xmin, xmax, ymin, ymax, zmin, zmax);
	gsaf_set3dcoord_polar1 (0.5*(xmin+xmax), 0.5*(ymin+ymax), 0.5*(zmin+zmax),
				radius, theta, phi);

	gsaf_move3d (0.0, 0.0, 0.0);
	gsaf_draw3d (1.0, 0.0, 0.0);
	gsaf_draw3d (1.0, 1.0, 0.0);
	gsaf_draw3d (0.0, 1.0, 0.0);
	gsaf_draw3d (0.0, 0.0, 0.0); /* 正方形 */

	x[0] = 0.0;
	x[1] = 1.0;
	x[2] = 1.0;
	y[0] = 0.0;
	y[1] = 1.0;
	y[2] = 0.0;
	z[0] = 1.0;
	z[1] = 1.0;
	z[2] = 1.0;
	gsaf_setrgb (0.0, 1.0, 1.0);
	gsaf_poly3d (x, y, z, 3); /* 3角形のポリゴン */

	gsaf_pagee ();  /* ページの終了 */
	gsaf_gsclos (); /* gsgl の終了 */
	return 0;
}
