#include "gsgl.h"
#include <math.h>

int main ()
{
	float gx[101], gy[101], gz[101];
	float xl=10.0;     /* x軸方向の長さ */
	float yl=10.0;    /* y軸方向の長さ */
	float zl=10.0;     /* z軸方向の長さ */
	float phi=50.0;    /* 極座標の φ */
	float theta=60.0;  /* 極座標の θ */
	float radius=10.0; /* 極座標の r */
	int n = 101;
	int i;
	float xmin, xmax, ymin, ymax, zmin, zmax;
	float gxmin,gxmax,xscal;
	float gymin,gymax,yscal;
	float gzmin,gzmax,zscal;

	gsaf_gsopen (); /* gsgl の初期化 */

	for (i=0; i<n; i++) {
		gx[i] = sin(0.1 * i);
		gy[i] = cos(0.1 * i);
		gz[i] = 0.1 * i;
	}
	gsaf_gmnmx1(gx,0,n-1,1,&xmin,&xmax); /* gx の最大値，最小値を求める */
	gsaf_gmnmx1(gy,0,n-1,1,&ymin,&ymax); /* gy の最大値，最小値を求める */
	gsaf_gmnmx1(gz,0,n-1,1,&zmin,&zmax); /* gz の最大値，最小値を求める */
     
	gsaf_gqscal(xmin,xmax,&gxmin,&gxmax,&xscal); /* 目盛りの間隔，開始値 */
	gsaf_gqscal(ymin,ymax,&gymin,&gymax,&yscal); /* 目盛りの間隔，開始値 */
	gsaf_gqscal(zmin,zmax,&gzmin,&gzmax,&zscal); /* 目盛りの間隔，開始値 */
    
	gsaf_pages ();              /* ページの開始 */
	gsaf_setchs (0.50,0.0);     /* フォントの大きさ 0.50 cm */
	gsaf_setfnt (2);            /* フォント番号 */
     
	gsaf_gdefin3d(xl ,yl ,zl, xmin, xmax, ymin, ymax, zmin, zmax);
	gsaf_set3dcoord_polar1 (0.5*(xmin+xmax), 0.5*(ymin+ymax), 0.5*(zmin+zmax),
				radius, theta, phi);
     
	gsaf_gscale3dx (xmin, xscal, 0.3, 0); /* x軸の目盛り */
	gsaf_gscale3dy (ymin, yscal, 0.3, 0); /* y軸の目盛り */
	gsaf_gscale3dz (0.0,  zscal, 0.3, 0); /* z軸の目盛り */
	gsaf_gvalue3dx (xmin, xscal, 2, 1); /* x軸の目盛りの数値 */
	gsaf_gvalue3dy (ymin, yscal, 2, 1); /* y軸の目盛りの数値 */
	gsaf_gvalue3dz (0.0,  zscal, 2, 1); /* z軸の目盛りの数値 */
	gsaf_setrgb (0.0, 1.0, 1.0); /* 水色に設定 */
	gsaf_gplotp3d (gx, gy, gz, 0, 100, 1, 0, 0, 0);
	gsaf_setrgb (0.0, 0.0, 0.0); /* 黒色に設定 */
	gsaf_gaxis3d (2);          /* 座標軸の描画 */
	gsaf_gdrawback3d (0.3, 0.3, 0.3); /* 背景を灰色に */
	
	gsaf_pagee ();                /* ページの終了 */
	gsaf_gsclos ();               /* gsgl の終了 */
	return 0;
}
