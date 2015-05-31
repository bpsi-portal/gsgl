#include "gsgl.h"

int main ()
{
	float xl=10.0;     /* x軸方向の長さ */
	float yl=10.0;     /* y軸方向の長さ */
	float zl=10.0;     /* z軸方向の長さ */
	float phi=50.0;    /* 極座標の φ */
	float theta=60.0;  /* 極座標の θ */
	float radius=10.0; /* 極座標の r */
	float xmin=-1.0;
	float ymin=-1.0;
	float zmin=-1.0;
	float xmax= 1.0;
	float ymax= 1.0;
	float zmax= 1.0;

	gsaf_gsopen (); /* gsgl の初期化 */
	
	gsaf_pages ();  /* ページの開始 */
	gsaf_gdefin3d (xl ,yl ,zl, xmin, xmax, ymin, ymax, zmin, zmax);
	gsaf_set3dcoord_polar1 (0.5*(xmin+xmax), 0.5*(ymin+ymax), 0.5*(zmin+zmax),
				radius, theta, phi);
	
	gsaf_setrgb (1.0, 0.0, 0.0); /* 赤色に設定 */
	gsgl_wiretorus (0.0, 0.0, 0.0, /* トーラスの中心の座標 */
			0.0, 0.0, 1.0,       /* トーラスの軸を決めるベクトル */
			0.25, 1.0, 20, 20);   /* 小半径，大半径 */
	gsaf_pagee ();  /* ページの終了 */
	gsaf_gsclos (); /* gsgl の終了 */

	return 0;
}
