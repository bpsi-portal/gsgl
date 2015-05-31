#include "gsaf.h"
#include "gsgl.h"
#include "base.h"
#include "bsc1.h"
#include "bsc2.h"
#include "bsc3d.h"
#include "apl1.h"
#include "utils.h"
#include "clientstate.h"
#include "pers3d.h"

typedef struct _zinfo {
	float *zth;
	float *rgbflat;
} zinfo;

static void persflat (int itype, float *z, float *x, float *y, int nmx, int nx, int ny, int ind, RGBFUNC, int *ielm, gsglPage *);
static void persgraud (int itype, float *z, float *x, float *y, int nmx, int nx, int ny, int ind1, RGBFUNC, int *ielm, gsglPage *);
static void gsglcontour3d (float *xt, float *yt, float *zt, float zline);
static void persinqcolorthreshold (int *iz, zinfo *zinfo1, RGBFUNC);
static void gsglpers1frame (float *z, int nmx, int nx, int ny, gsglPage *);
static void gsglpers2frame (float *z, float *x, float *y, int nmx, int nx, int ny);
static void gsglpers4frame (float *z, float *r, float *t, int nmx, int nx, int ny);
static void gsglpers5frame (float *z, float *x, float *y, int nmx, int nx, int ny);
static void gsglupperclipplane (float threshould, gsglPage *);
static void gsgllowerclipplane (float threshould, gsglPage *);
static void gsgl_setsquare1 (float *z, int nmx, float dx, float dy, int ix, int iy, float *xb, float *yb, float *zb, gsglPage *);
static void gsglanothertriangle (float *x2, float *y2, float *z2, float *x1, float *y1, float *z1);
static void gsgl_setsquare2 (float *z, float *x, float *y, int nmx, int nx, int ny, int ix, int iy, float *xb, float *yb, float *zb);
static void gsgl_setsquare3 (float *z, float *r, int nmx, int nx, int ny, int ix, int iy,
			     float *xb, float *yb, float *zb);
static void gsgl_setsquare4 (float *z, float *r, float *t, int nmx, int nx, int ny, int ix,int iy, float *xb, float *yb, float *zb);
static void gsgl_setsquare5 (float *z, float *x, float *y, int nmx, int nx, int ny, int ix, int iy,
		      float *xb, float *yb, float *zb);
static void gsgl_set_data (int itype, int nmx, int nx, int ny, float *z, float *x, float *y, int *ielm, gsglPage *);
static void gsgl_array_poly (int itype, int nmx, int nx, int ny, int iymax, float *z, int *ielm, float z1, float z2, gsglPage *p);
static void gsgl_array_contour (int itype, int nmx, int nx, int ny, int iymax, float *z, float *x, float *y, int *ielm, float z1, float dx, float dy, gsglPage *);
static void gsgl_array_lattice (int itype, int nx, int ny, int iymax, int *ielm, int ind, gsglPage *);
static void gsgl_array_frame (int itype, int nmx, int nx, int ny, float *z, float *x, float *y, gsglPage *);
static void gsgl_array_color (int itype, float *z, int nmx, int nx, int ny, RGBFUNC, gsglPage *);

/* obsolute */
void gsaf_perse3d1 (float *z, int nmx, int nx, int ny, int ixy, int ind)
{
	float gxmin,gxmax,gymin,gymax,gzmin,gzmax;
	/*float z(nmx,ny)*/
	float x1[4],y1[4],z1[4];
	float x2[4],y2[4],z2[4];
	int i, ix, iy;
	float dx, dy, dz;
	float zline;
	gsglPage *p = gsgl_inq_page ();

	if (!gsgl_inq_page_is_open ()) return;
	gsgl_inq_region_3d (&gxmin, &gxmax, &gymin, &gymax, &gzmin, &gzmax, p);

	dx = (gxmax-gxmin) / (float) (nx-1);
	dy = (gymax-gymin) / (float) (ny-1);
	dz = (gzmax-gzmin) / (float) (ind+1);

	zline = gzmin + 0.0 * ixy;
	for (i=0; i<ind; i++) {
		zline = zline + dz;
		for (ix=0; ix<nx-1; ix++) {
			for (iy=0; iy<ny-1; iy++) {
				gsgl_setsquare1 (z, nmx, dx, dy, ix, iy, x1, y1, z1, p);
				gsglcontour3d (x1, y1, z1, zline);
				gsglanothertriangle (x2, y2, z2, x1, y1, z1);
				gsglcontour3d (x2, y2, z2, zline);
			}
		}
	}
}

/* gsaf 3d routines : pers 3d1
 *    pers3d1(z, nmx, nx, ny, ind, rgbfunc)
 *    等間隔格子2次元データの透視図を描く．
 *    src/demo/pers3dtest.f 参照．
 *
 *    引数    z       [r(nmx,*)]   z座標データの2次元配列
 *            nmx     [i]          2次元データの第1添字の上限
 *            nx      [i]          x方向のデータ数
 *                                 (gdefin3dで与えられたpxminから
 *                                 pxmax までを nx 等分する．)
 *            ny      [i]          y方向のデータ数
 *                                 (gdefin3dで与えられたpyminから
 *                                 pymax までを ny 等分する．)
 *            ind     [i]          表示内容を決めるパラメータ
 *
 *            ind<0 グローシェーディング．(-ind+2)本の等高線を描く
 *            ind=0 グローシェーディング．ポリゴンのみを描く
 *            ind=1 グローシェーディング．ポリゴンとx方向の線を描く
 *            ind=2 グローシェーディング．ポリゴンとy方向の線を描く
 *            ind=3 グローシェーディング．ポリゴンと格子を描く
 *            ind=4 フラットシェーディング．ポリゴンのみを描く
 *            ind=5 フラットシェーディング．ポリゴンとx方向の線を描く
 *            ind=6 フラットシェーディング．ポリゴンとy方向の線を描く
 *            ind=7 フラットシェーディング．ポリゴンと格子を描く
 *            ind=8 フラットシェーディング．ポリゴンと等高線を描く
 *
 *            rgbfunc [void] 配色を決めるサブルーチン
 */
void gsaf_pers3d1 (float *z, int nmx, int nx, int ny, int ind, RGBFUNC)
{
	float x[1], y[1];
	int ielm[3][1];
	gsglPage *p = gsgl_inq_page ();

	if (!gsgl_inq_page_is_open ()) return;

	if (ind < 4) {
		persgraud (GSGL_DATA_ORTHO_EQUIV_INTERVAL, 
			   z, x, y, nmx, nx, ny, ind, rgbfunc, &(ielm[0][0]), p);
	} else {
		persflat (GSGL_DATA_ORTHO_EQUIV_INTERVAL,
			  z, x, y, nmx, nx, ny, ind-4, rgbfunc, &(ielm[0][0]), p);
	}
}

/* gsaf 3d routines : pers 3d2
 *    *************************************************
 *    pers3d2(z, x, y, nmx, nx, ny, ind, rgbfunc)
 *    非等間隔格子2次元データの透視図を描く．
 *    src/demo/pers3dtest.f 参照．
 *
 *    引数    z       [r(nmx,*)]   z座標データの2次元配列
 *            x       [r(nx)]      x座標データの1次元配列
 *            y       [r(ny)]      y座標データの1次元配列
 *            nmx     [i]          2次元データの第1添字の上限
 *            nx      [i]          x方向のデータ数
 *                                 (gdefin3dで与えられたpxminから
 *                                 pxmax までを nx 等分する．)
 *            ny      [i]          y方向のデータ数
 *                                 (gdefin3dで与えられたpyminから
 *                                 pymax までを ny 等分する．)
 *            ind     [i]          表示内容を決めるパラメータ
 *            rgbfunc [void] 配色を決めるサブルーチン
 */
void gsaf_pers3d2 (float *z, float *x, float *y, int nmx, int nx, int ny, int ind, RGBFUNC)
{
	int ielm[3][1];
	gsglPage *p = gsgl_inq_page ();
      
	if (!gsgl_inq_page_is_open ()) return;

	if (ind < 4) {
		persgraud(GSGL_DATA_ORTHO_UNEQU_INTERVAL,
			  z, x, y, nmx, nx, ny, ind, rgbfunc, &(ielm[0][0]), p);
	} else {
		persflat (GSGL_DATA_ORTHO_UNEQU_INTERVAL, 
			  z, x, y, nmx, nx, ny, ind-4, rgbfunc, &(ielm[0][0]), p);
	}
}

/*  gsaf 3d routines : pers 3d3
 *
 *    等角度間隔円形2次元データの透視図を描く．
 *    src/demo/pers3dtest.f 参照．
 *
 *    引数    z       [r(nmx,*)]   z座標データの2次元配列
 *            r       [r(*)]  半径方向座標データの1次元配列
 *            nmx     [i]     2次元データの第1添字の上限
 *            nx      [i]     半径方向のデータ数
 *            ny      [i]     角度方向のデータ数
 *            ind     [i]     表示内容を決めるパラメータ
 *            rgbfun *[void] 配色を決めるサブルーチン
 *
 *    角度方向に関しては，最初のデータが x 軸上にとられ，反時計回りに
 *    360/nymax 度間隔で表示される．周期 360 度の周期性が仮定される．
 */    
void gsaf_pers3d3 (float *z, float *r, int nmx, int nx, int ny, int ind, RGBFUNC)
{
	float y[1];
	int ielm[3][1];
	gsglPage *p = gsgl_inq_page ();

	if (!gsgl_inq_page_is_open ()) return;

	if (ind < 4) {
		persgraud (GSGL_DATA_CYLIN_EQUIV_INTERVAL,
			   z, r, y, nmx, nx, ny, ind, rgbfunc, &(ielm[0][0]), p);
	} else {
		persflat (GSGL_DATA_CYLIN_EQUIV_INTERVAL, 
			  z, r, y, nmx, nx, ny, ind-4, rgbfunc, &(ielm[0][0]), p);
	}
}

/*    *  gsaf 3d routines : pers 3d4                 
 *
 *    等角度間隔円形2次元データの透視図を描く．
 *    src/demo/pers3dtest.f 参照．
 *
 *    引数    z       [r(nmx,*)]   z座標データの2次元配列
 *            r       [r(*)]  半径方向座標データの1次元配列
 *            t       [r(*)]  角度方向座標データの1次元配列 (度)
 *            nmx     [i]     2次元データの第1添字の上限
 *            nx      [i]     半径方向のデータ数
 *            ny      [i]     角度方向のデータ数
 *            ind     [i]          表示内容を決めるパラメータ
 *            rgbfun *[void] 配色を決めるサブルーチン
 *
 *    角度方向に関しては，最初のデータが x 軸上にとられ，反時計回りに
 *    360/nymax 度間隔で表示される．周期 360 度の周期性が仮定される．
 */    
void gsaf_pers3d4 (float *z, float *r, float *t, int nmx, int nx, int ny, int ind, RGBFUNC)
{
	int ielm[3][1];
	gsglPage *p = gsgl_inq_page ();

	if (!gsgl_inq_page_is_open ()) return;

	if (ind < 4) {
		persgraud (GSGL_DATA_CYLIN_UNEQU_INTERVAL,
			   z, r, t, nmx, nx, ny, ind, rgbfunc, &(ielm[0][0]), p);
	} else {
		persflat (GSGL_DATA_CYLIN_UNEQU_INTERVAL,
			  z, r, t, nmx, nx, ny, ind-4, rgbfunc, &(ielm[0][0]), p);
	}
}

/*    *  gsaf 3d routines : pers 3d5                  
 *
 *    z, x, y ともに 2次元配列である場合の透視図
 *    src/demo/pers3dtest.f 参照．
 *
 *    引数    z       [r(nmx,*)]  z座標データの2次元配列
 *            x       [r(nmx,*)]  x座標データの2次元配列
 *            y       [r(nmx,*)]  y座標データの2次元配列
 *            nmx     [i]     2次元データの第1添字の上限
 *            nx      [i]     x方向のデータ数
 *            ny      [i]     y方向のデータ数
 *            ind     [i]          表示内容を決めるパラメータ
 *            rgbfunc [void] 配色を決めるサブルーチン
 */
void gsaf_pers3d5 (float *z, float *x, float *y, int nmx, int nx, int ny, int ind, RGBFUNC)
{
	int ielm[3][1];
	gsglPage *p = gsgl_inq_page ();

	if (!gsgl_inq_page_is_open ()) return;

	if (ind < 4) {
		persgraud (GSGL_DATA_X_Y_Z_2D,
			   z, x, y, nmx, nx, ny, ind, rgbfunc, &(ielm[0][0]), p);
	} else {
		persflat (GSGL_DATA_X_Y_Z_2D, 
			  z, x, y, nmx, nx, ny, ind-4, rgbfunc, &(ielm[0][0]), p);
	}
}

/*    *  gsaf 3d routines : pers3df                  
 *
 *    三角形有限要素上の2次元データの透視図
 *    src/demo/pers3dtest.f 参照．
 *
 *    引数    z       [r(*)]       節点上のデータからなる1次元配列 (引数は節点番号)
 *            x       [r(*)]       節点の x 座標データからなる1次元配列 (引数は節点番号)
 *            y       [r(*)]       節点の y 座標データからなる1次元配列 (引数は節点番号)
 *            nnod    [i]          全節点数
 *            ielm    [i(3,*)]     要素に属する節点番号（引数は要素番号）
 *            nelm    [i]          全要素数
 *            ind     [i]          表示内容を決めるパラメータ
 *            rgbfunc [void] 配色を決めるサブルーチン
 */
void gsaf_pers3df (float *z, float *x, float *y, int nnod, int *ielm, int nelm, int ind, RGBFUNC)
{
	gsglPage *p = gsgl_inq_page ();      

	if (!gsgl_inq_page_is_open ()) return;

	if (ind < 4) {
		persgraud (GSGL_DATA_FEM_TRIANGLE,
			   z, x, y, nnod, nnod, nelm, ind, rgbfunc, ielm, p);
	} else {
		persflat (GSGL_DATA_FEM_TRIANGLE,
			  z, x, y, nnod, nnod, nelm, ind-4, rgbfunc, ielm, p);
	}
}

/*    -   gsaf 3d routines : pers with flat shading   -
 *    グラフをカラー表示する
 *    ind=0, draw poly only
 *    ind=1, draw poly and x-lines only
 *    ind=2, draw poly and y-lines only
 *    ind=3, draw poly and lines
 *    ind=4, draw contour.
 */
static void persflat (int itype, float *z, float *x, float *y, int nmx, int nx, int ny, int ind, RGBFUNC, int *ielm, gsglPage *p)
{
	float gxmin,gxmax,gymin,gymax,gzmin,gzmax;
	int iz;
	zinfo zinfo1;
	float *zth, *rgb;
	float w;
	float cr, cg, cb;
	int iymax, i;
	float z1, z2, dx, dy;

	persinqcolorthreshold (&iz, &zinfo1, rgbfunc);
	zth = zinfo1.zth;
	rgb = zinfo1.rgbflat;

	gsgl_inq_region_3d (&gxmin, &gxmax, &gymin, &gymax, &gzmin, &gzmax, p);

	gsaf_inqlnw (&w);
	gsaf_setlnw (0.0);
	gsaf_inqrgb (&cr,&cg,&cb);
	/*    --- データをopenglが使える形でメモリに保存する ---*/
	gsgl_set_data (itype, nmx, nx, ny, z, x, y, ielm, p);
	/*    --- データの色をopenglが使える形でメモリに保存する ---*/
	gsglsetcolorpointer (0, 0, 0, 0, NULL, p);
	/*    -------*/
	if (itype != GSGL_DATA_CYLIN_EQUIV_INTERVAL) {
		iymax = ny-1;
	} else {
		iymax = ny;
	}
	/*    --- ポリゴンの描画 --- */
	for (i=0; i<iz; i++) {
		z1 = zth[i+1] * (gzmax-gzmin) + gzmin;
		gsaf_setrgb (rgb[findex2(0,i,3)],rgb[findex2(1,i,3)],rgb[findex2(2,i,3)]);
		gsglupperclipplane (z1, p);
		z2 = zth[i] * (gzmax-gzmin) + gzmin;
		gsgllowerclipplane (z2, p);
		gsgl_array_poly (itype, nmx, nx, ny, iymax, z, ielm, z1, z2, p);
		gsgl_clip_disable (4, p);
		gsgl_clip_disable (5, p);
	}

	gsaf_setlnw (w);
	gsaf_setrgb (cr, cb, cg);

	dx = (gxmax-gxmin) / (float) (nx-1);
	dy = (gymax-gymin) / (float) (ny-1);
	/*    --- 等高線を描く ---*/
	if (ind == 4) {
		for (i=0; i<iz; i++) {
			z1 = zth[i] * (gzmax-gzmin) + gzmin;
			gsgl_array_contour (itype,nmx,nx,ny,iymax,z,x,y,ielm,z1,dx,dy,p);
		}
	} else if (ind == 1 || ind == 2 || ind == 3) {
		gsgl_array_lattice (itype, nx, ny, iymax, ielm, ind, p);
	}
	/*    --- 外枠を描く ---*/
	gsgl_array_frame (itype, nmx, nx, ny, z, x, y, p);
}

/*    -     gsaf 3d routines : pers with gouraud shading.     -*/
static void persgraud (int itype, float *z, float *x, float *y, int nmx, int nx, int ny, int ind1, RGBFUNC, int *ielm, gsglPage *p)
{
/*    --- グラフをカラー表示する ---
 *    ind=0, draw poly only
 *    ind=1, draw poly and x-lines only
 *    ind=2, draw poly and y-lines only
 *    ind=3, draw poly and lines
 *    ind<0, draw (-ind) lines contour.
 */
	float gxmin,gxmax,gymin,gymax,gzmin,gzmax;
	float *rgb, *zthv;
	float zth;
	zinfo zinfo1;
	float w;
	float cr, cg, cb;
	float dx, dy;
	int iymax, ind, iz, i;

	gsgl_inq_region_3d (&gxmin, &gxmax, &gymin, &gymax, &gzmin, &gzmax, p);

	/*    --- データをopenglが使える形でメモリに保存する ---*/
	gsgl_set_data (itype, nmx, nx, ny, z, x, y, ielm, p);

	/*    --- 格子を描く ---*/
	if (itype != 3) {
		iymax = ny-1;
	} else {
		iymax = ny;
	}      
	if (ind1 < 0) {
		ind=0;
	} else {
		ind=ind1;
	}

	if (ind == 1 || ind == 2 || ind == 3) {
		gsgl_array_lattice (itype, nx, ny, iymax, ielm, ind, p);
	}
	/*    --- 色の設定 ---*/
	persinqcolorthreshold (&iz, &zinfo1, rgbfunc);
	zthv = zinfo1.zth;
	rgb = zinfo1.rgbflat;

	if (iz == 0) {
		gsglsetcolorpointer (0, 0, 0, 0, NULL, p);
		gsaf_setrgb (rgb[findex2(0,0,3)],rgb[findex2(1,0,3)],rgb[findex2(2,0,3)]);
	} else {
		gsgl_array_color (itype, z, nmx, nx, ny, rgbfunc, p);
	}

	gsaf_inqlnw (&w);
	gsaf_setlnw (0.0);
	gsgl_ps_gouraudonly_begin (p);
	gsaf_inqrgb (&cr,&cg,&cb);

/*    --- ポリゴンを描く ---*/
	gsglupperclipplane (gzmax, p);
	gsgllowerclipplane (gzmin, p);
	gsgl_array_poly (itype, nmx, nx, ny, iymax, z, ielm, gzmax, gzmin, p);
	gsgl_clip_disable (5, p);
	gsgl_clip_disable (4, p);

	gsaf_setrgb (cr,cg,cb);
	gsaf_setlnw (w);

	dx = (gxmax-gxmin)/(float) (nx-1);
	dy = (gymax-gymin)/(float) (ny-1);
	/*    --- 等高線を引く ---*/
	if (ind1 < 0) {
		gsglupperclipplane (gzmax, p);
		gsgllowerclipplane (gzmin, p);
		ind=-ind1+2;
		for (i=0; i<ind; i++) {
			zth = gzmin+(gzmax-gzmin)*(float)i/(float)(ind-1);
			if (zth == gzmin) gsgl_clip_disable (5, p);
			if (zth == gzmax) gsgl_clip_disable (4, p);
			gsgl_array_contour (itype, nmx, nx, ny, iymax, z, x, y, ielm, zth, dx, dy, p);
			if (zth == gzmin) gsglupperclipplane (gzmax, p);
			if (zth == gzmax) gsgllowerclipplane (gzmin, p);
		}
		gsgl_clip_disable (5, p);
		gsgl_clip_disable (4, p);
	}
	/*    --- 外枠を描く ---*/
	gsgl_array_frame (itype, nmx, nx, ny, z, x, y, p);

	gsgl_ps_gouraudonly_end (p);
}

/*    等高線を引く */
static void gsglcontour3d (float *xt, float *yt, float *zt, float zline)
{
	float fact1, fact2, fact0;

	if (zt[0] > zline && zt[1] <= zline && zt[2] <= zline) {
		fact1 = (zt[0] - zline)/(zt[0] - zt[1]);
		fact2 = (zt[0] - zline)/(zt[0] - zt[2]);
		gsaf_move3d(fact1*xt[1] + (1.0-fact1)*xt[0], 
		       fact1*yt[1] + (1.0-fact1)*yt[0],
		       zline);
		gsaf_draw3d(fact2*xt[2] + (1.0-fact2)*xt[0],
		       fact2*yt[2] + (1.0-fact2)*yt[0],
		       zline);
	} else if (zt[1] > zline && zt[2] <= zline && zt[0] <= zline) {
		fact0 = (zt[1] - zline )/(zt[1] - zt[0]);
		fact2 = (zt[1] - zline )/(zt[1] - zt[2]);
		gsaf_move3d(fact0*xt[0] + (1.0-fact0)*xt[1],
		       fact0*yt[0] + (1.0-fact0)*yt[1],
		       zline);
		gsaf_draw3d(fact2*xt[2] + (1.0-fact2)*xt[1],
		       fact2*yt[2] + (1.0-fact2)*yt[1],
		       zline);
	} else if (zt[2] > zline && zt[0] <= zline && zt[1] <= zline) {
		fact0 = (zt[2] - zline )/(zt[2] - zt[0]);
		fact1 = (zt[2] - zline )/(zt[2] - zt[1]);
		gsaf_move3d(fact0*xt[0] + (1.0-fact0)*xt[2],
		       fact0*yt[0] + (1.0-fact0)*yt[2],
		       zline);
		gsaf_draw3d(fact1*xt[1] + (1.0-fact1)*xt[2],
		       fact1*yt[1] + (1.0-fact1)*yt[2],
		       zline);
	} else if (zt[0] >= zline && zt[1] >= zline && zt[2] < zline) {
		fact0 = (zt[2] - zline )/(zt[2] - zt[0]);
		fact1 = (zt[2] - zline )/(zt[2] - zt[1]);
		gsaf_move3d(fact0*xt[0] + (1.0-fact0)*xt[2],
		       fact0*yt[0] + (1.0-fact0)*yt[2],
		       zline);
		gsaf_draw3d(fact1*xt[1] + (1.0-fact1)*xt[2],
		       fact1*yt[1] + (1.0-fact1)*yt[2],
		       zline);
	} else if (zt[1] >= zline && zt[2] >= zline && zt[0] < zline) {
		fact1 = (zt[0] - zline )/(zt[0] - zt[1]);
		fact2 = (zt[0] - zline )/(zt[0] - zt[2]);
		gsaf_move3d(fact1*xt[1] + (1.0-fact1)*xt[0],
		       fact1*yt[1] + (1.0-fact1)*yt[0],
		       zline);
		gsaf_draw3d(fact2*xt[2] + (1.0-fact2)*xt[0],
		       fact2*yt[2] + (1.0-fact2)*yt[0],
		       zline);
	} else if (zt[2] >= zline && zt[0] >= zline && zt[1] < zline) {
		fact0 = (zt[1] - zline )/(zt[1] - zt[0]);
		fact2 = (zt[1] - zline )/(zt[1] - zt[2]);
		gsaf_move3d(fact0*xt[0] + (1.0-fact0)*xt[1],
		       fact0*yt[0] + (1.0-fact0)*yt[1],
		       zline);
		gsaf_draw3d(fact2*xt[2] + (1.0-fact2)*xt[1],
		       fact2*yt[2] + (1.0-fact2)*yt[1],
		       zline);
	}
}

static void persinqcolorthreshold (int *iz, zinfo *zinfo1, RGBFUNC)
{
	/*float rgb_flat(3,*),zth(*),*/
	int imax=100001;
	float z, dz;
	float r0, g0, b0;
	float rgb[3];
	int iz1;
	int i;

	dz = 1.0 / (float) (imax-1);
	r0 = -1.0;
	g0 = -1.0;
	b0 = -1.0;

	iz1 = 0;
	for (i=0; i<imax; i++) {
		z = i * dz;
		rgbfunc (&z, rgb);

		if (fabs(rgb[0]-r0)>FLOAT_EPS || fabs(rgb[1]-g0)>FLOAT_EPS || fabs(rgb[2]-b0)>FLOAT_EPS) {
			iz1++;
			r0 = rgb[0];
			g0 = rgb[1];
			b0 = rgb[2];
		}
	}
	iz1++;

	if (iz1 == 0) {
		*iz = 0;
		return;
	}
	zinfo1->zth = (float *) gsgl_malloc_atomic (sizeof(float)*iz1);
	zinfo1->rgbflat = (float *) gsgl_malloc_atomic (sizeof(float)*iz1*3);

	r0 = -1.0;
	g0 = -1.0;
	b0 = -1.0;
	iz1 = 0;
	for (i=0; i<imax; i++) {
		z = i * dz;
		rgbfunc (&z, rgb);

		if (fabs(rgb[0]-r0)>FLOAT_EPS || fabs(rgb[1]-g0)>FLOAT_EPS || fabs(rgb[2]-b0)>FLOAT_EPS) {
			zinfo1->zth [iz1] = z;
			zinfo1->rgbflat[findex2(0,iz1,3)] = rgb[0];
			zinfo1->rgbflat[findex2(1,iz1,3)] = rgb[1];
			zinfo1->rgbflat[findex2(2,iz1,3)] = rgb[2];
			iz1++;
			r0 = rgb[0];
			g0 = rgb[1];
			b0 = rgb[2];
		}
	}
	zinfo1->zth [iz1] = 1.0;
	*iz = iz1;
}

/*    -               pers3d の外枠を描く                      -*/
static void gsglpers1frame (float *z, int nmx, int nx, int ny, gsglPage *p)
{
	float gxmin,gxmax,gymin,gymax,gzmin,gzmax;
	/*float z(nmx, ny)*/
	float xn[2],yn[2],zn[2];
	float dx, dy;
	int ix, iy;

	gsgl_inq_region_3d (&gxmin, &gxmax, &gymin, &gymax, &gzmin, &gzmax, p);

	dx = (gxmax-gxmin)/(float) (nx-1);
	dy = (gymax-gymin)/(float) (ny-1);
	for (iy=0; iy<ny; iy+=ny-1) {
		for (ix=0; ix<nx-1; ix++) {
			xn[0]=(float)ix*dx+gxmin;
			yn[0]=(float)iy*dy+gymin;
			zn[0]=z[findex2(ix,iy,nmx)];
			xn[1]=(float)(ix+1)*dx+gxmin;
			yn[1]=(float)(iy)*dy+gymin;
			zn[1]=z[findex2(ix+1,iy,nmx)];
			gsaf_move3d(xn[0],yn[0],zn[0]);
			gsaf_draw3d(xn[1],yn[1],zn[1]);
		}
	}
	for (ix=0; ix<nx; ix+=nx-1) {
		for (iy=0; iy<ny-1; iy++) {
			xn[0]=(float)ix*dx+gxmin;
			yn[0]=(float)iy*dy+gymin;
			zn[0]=z[findex2(ix,iy,nmx)];
			xn[1]=(float)ix*dx+gxmin;
			yn[1]=(float)(iy+1)*dy+gymin;
			zn[1]=z[findex2(ix,iy+1,nmx)];
			gsaf_move3d (xn[0],yn[0],zn[0]);
			gsaf_draw3d (xn[1],yn[1],zn[1]);
		}
	}
}

/*    -               pers3d の外枠を描く                      -*/
static void gsglpers2frame (float *z, float *x, float *y, int nmx, int nx, int ny)
{
	/*float z(nmx, ny), x(nx), y(ny)*/
	float xn[2],yn[2],zn[2];
	int ix, iy;

	for (iy=0; iy<ny; iy+=ny-1) {
		for (ix=0; ix<nx-1; ix++) {
			xn[0]=x[ix];
			yn[0]=y[iy];
			zn[0]=z[findex2(ix,iy,nmx)];
			xn[1]=x[ix+1];
			yn[1]=y[iy];
			zn[1]=z[findex2(ix+1,iy,nmx)];
			gsaf_move3d (xn[0],yn[0],zn[0]);
			gsaf_draw3d (xn[1],yn[1],zn[1]);
		}
	}
	for (ix=0; ix<nx; ix+=nx-1) {
		for (iy=0; iy<ny-1; iy++) {
			xn[0]=x[ix];
			yn[0]=y[iy];
			zn[0]=z[findex2(ix,iy,nmx)];
			xn[1]=x[ix];
			yn[1]=y[iy+1];
			zn[1]=z[findex2(ix,iy+1,nmx)];
			gsaf_move3d (xn[0],yn[0],zn[0]);
			gsaf_draw3d (xn[1],yn[1],zn[1]);
		}
	}
}

/*    -               pers3d の外枠を描く                      -*/
static void gsglpers3frame (float *z, float *r, int nmx, int nx, int ny)
{
	/*float z(nmx, ny), r(nx)*/
	float xn[2],yn[2],zn[2];
	float pi = 3.1415927;
	float dt;
	int iy;

	dt = 2.0*pi/ny;

	if (fabs(r[0]) > FLOAT_EPS) {
		for (iy=0; iy<ny-1; iy++) {
			xn[0]=r[0]*cos(dt*iy);
			yn[0]=r[0]*sin(dt*iy);
			zn[0]=z[findex2(0,iy,nmx)];
			xn[1]=r[0]*cos(dt*(iy+1));
			yn[1]=r[0]*sin(dt*(iy+1));
			zn[1]=z[findex2(0,iy+1,nmx)];
			gsaf_move3d (xn[0],yn[0],zn[0]);
			gsaf_draw3d (xn[1],yn[1],zn[1]);
		}
	}
	for (iy=0; iy<ny; iy++) {
		xn[0]=r[nx-1]*cos(dt*(iy));
		yn[0]=r[nx-1]*sin(dt*(iy));
		zn[0]=z[findex2(nx-1,iy,nmx)];
		xn[1]=r[nx-1]*cos(dt*(iy+1));
		yn[1]=r[nx-1]*sin(dt*(iy+1));
		zn[1]=z[findex2(nx-1,iy%ny+1,nmx)];
		gsaf_move3d (xn[0],yn[0],zn[0]);
		gsaf_draw3d (xn[1],yn[1],zn[1]);
	}
}
 
/*    -               pers3d の外枠を描く                      -*/
static void gsglpers4frame (float *z, float *r, float *t, int nmx, int nx, int ny)
{
	/*float z(nmx, ny), r(nx), t(ny)*/
	float xn[2],yn[2],zn[2];
	float pi = 3.1415927;
	float rad;
	int ix, iy;

	rad = pi/180.0;
	
	for (iy=0; iy<ny; iy+=ny-1) {
		for (ix=0; ix<nx-1; ix++) {
			xn[0]=r[ix]*cos(rad*t[iy]);
			yn[0]=r[ix]*sin(rad*t[iy]);
			zn[0]=z[findex2(ix,iy,nmx)];
			xn[1]=r[ix+1]*cos(rad*t[iy]);
			yn[1]=r[ix+1]*sin(rad*t[iy]);
			zn[1]=z[findex2(ix+1,iy,nmx)];
			gsaf_move3d (xn[0],yn[0],zn[0]);
			gsaf_draw3d (xn[1],yn[1],zn[1]);
		}
	}
	for (ix=0; ix<nx; ix+=nx-1) {
		for (iy=0; iy<ny-1; iy++) {
			xn[0]=r[ix]*cos(rad*t[iy]);
			yn[0]=r[ix]*sin(rad*t[iy]);
			zn[0]=z[findex2(ix,iy,nmx)];
			xn[1]=r[ix]*cos(rad*t[iy+1]);
			yn[1]=r[ix]*sin(rad*t[iy+1]);
			zn[1]=z[findex2(ix,iy+1,nmx)];
			gsaf_move3d (xn[0],yn[0],zn[0]);
			gsaf_draw3d (xn[1],yn[1],zn[1]);
		}
	}
}

/*    -               pers3d の外枠を描く                      -*/
static void gsglpers5frame (float *z, float *x, float *y, int nmx, int nx, int ny)
{
	/*float z(nmx, ny), x(nmx, ny), y(nmx, ny)*/
	float xn[2],yn[2],zn[2];
	int ix, iy;

	for (iy=0; iy<ny; iy+=ny-1) {
		for (ix=0; ix<nx-1; ix++) {
			xn[0]=x[findex2(ix,iy,nmx)];
			yn[0]=y[findex2(ix,iy,nmx)];
			zn[0]=z[findex2(ix,iy,nmx)];
			xn[1]=x[findex2(ix+1,iy,nmx)];
			yn[1]=y[findex2(ix+1,iy,nmx)];
			zn[1]=z[findex2(ix+1,iy,nmx)];
			gsaf_move3d (xn[0],yn[0],zn[0]);
			gsaf_draw3d (xn[1],yn[1],zn[1]);
		}
	}
	for (ix=0; ix<nx; ix+=nx-1) {
		for (iy=0; iy<ny-1; iy++) {
			xn[0]=x[findex2(ix,iy,nmx)];
			yn[0]=y[findex2(ix,iy,nmx)];
			zn[0]=z[findex2(ix,iy,nmx)];
			xn[1]=x[findex2(ix,iy+1,nmx)];
			yn[1]=y[findex2(ix,iy+1,nmx)];
			zn[1]=z[findex2(ix,iy+1,nmx)];
			gsaf_move3d (xn[0],yn[0],zn[0]);
			gsaf_draw3d (xn[1],yn[1],zn[1]);
		}
	}
}

static void gsglupperclipplane (float threshould, gsglPage *p)
{
	float eqn[4];

	eqn[0]=0.0;
	eqn[1]=0.0;
	eqn[2]=-1.0;
	eqn[3]=threshould;

	gsgl_clip_enable_3d_fileonly (4, eqn, p);
}

static void gsgllowerclipplane (float threshould, gsglPage *p)
{
	float eqn[4];

	eqn[0]=0.0;
	eqn[1]=0.0;
	eqn[2]=1.0;
	eqn[3]=-threshould;
	gsgl_clip_enable_3d_fileonly (5, eqn, p);
}

static void gsgl_setsquare1 (float *z, int nmx, float dx, float dy, int ix, int iy, 
			     float *xb, float *yb, float *zb, gsglPage *p)
{
	float gxmin,gxmax,gymin,gymax,gzmin,gzmax;
	/*float xb(4), yb(4), zb(4), z(nmx, *)*/

	gsgl_inq_region_3d (&gxmin, &gxmax, &gymin, &gymax, &gzmin, &gzmax, p);

	xb[0] = (ix)*dx+gxmin;
	yb[0] = (iy)*dy+gymin;
	zb[0] = z[findex2(ix,iy,nmx)];
	xb[1] = (ix+1)*dx+gxmin;
	yb[1] = (iy)*dy+gymin;
	zb[1] = z[findex2(ix+1,iy,nmx)];
	xb[2] = (ix+1)*dx+gxmin;
	yb[2] = (iy+1)*dy+gymin;
	zb[2] = z[findex2(ix+1,iy+1,nmx)];
	xb[3] = (ix)*dx+gxmin;
	yb[3] = (iy+1)*dy+gymin;
	zb[3] = z[findex2(ix,iy+1,nmx)];
}

static void gsglanothertriangle (float *x2, float *y2, float *z2, float *x1, float *y1, float *z1)
{
	x2[0] = x1[0];
	y2[0] = y1[0];
	z2[0] = z1[0];
	x2[1] = x1[2];
	y2[1] = y1[2];
	z2[1] = z1[2];
	x2[2] = x1[3];
	y2[2] = y1[3];
	z2[2] = z1[3];
}

static void gsgl_setsquare2 (float *z, float *x, float *y, int nmx, int nx, int ny, 
			     int ix, int iy, float *xb, float *yb, float *zb)
{
	/*     float xb[3], yb[3], zb[3]
	       float z(nmx, *), x(nx), y(ny)*/

	xb[0] = x[ix];
	yb[0] = y[iy];
	zb[0] = z[findex2(ix,iy,nmx)];
	xb[1] = x[ix+1];
	yb[1] = y[iy];
	zb[1] = z[findex2(ix+1,iy,nmx)];
	xb[2] = x[ix+1];
	yb[2] = y[iy+1];
	zb[2] = z[findex2(ix+1,iy+1,nmx)];
	xb[3] = x[ix];
	yb[3] = y[iy+1];
	zb[3] = z[findex2(ix,iy+1,nmx)];
}

static void gsgl_setsquare3 (float *z, float *r, int nmx, int nx, int ny, int ix, int iy,
			     float *xb, float *yb, float *zb)
{
	/*      float xb(4), yb(4), zb(4)
		float z(nmx, *), r(nx)*/
	float pi = 3.1415927;
	float dt;

	dt = 2.0*pi/ny;
	xb[0] = r[ix]*cos(dt*(iy));
	yb[0] = r[ix]*sin(dt*(iy));
	zb[0] = z[findex2(ix,iy,nmx)];
	xb[1] = r[ix+1]*cos(dt*(iy));
	yb[1] = r[ix+1]*sin(dt*(iy));
	zb[1] = z[findex2(ix+1,iy,nmx)];
	xb[2] = r[ix+1]*cos(dt*(iy+1));
	yb[2] = r[ix+1]*sin(dt*(iy+1));
	zb[2] = z[findex2(ix+1, (iy+1)%ny, nmx)];
	xb[3] = r[ix]*cos(dt*(iy+1));;
	yb[3] = r[ix]*sin(dt*(iy+1));
	zb[3] = z[findex2(ix, (iy+1)%ny, nmx)];
}

static void gsgl_setsquare4 (float *z, float *r, float *t, int nmx, int nx, int ny, 
			     int ix, int iy,
			     float *xb, float *yb, float *zb)
{
	/*float xb(4), yb(4), zb(4)
	  float z(nmx, *), r(nx), t(ny)*/
	float pi = 3.1415927;
	float rad;

	rad = pi/180.0;
	xb[0] = r[ix]*cos(rad*t[iy]);
	yb[0] = r[ix]*sin(rad*t[iy]);
	zb[0] = z[findex2(ix,iy,nmx)];
	xb[1] = r[ix+1]*cos(rad*t[iy]);
	yb[1] = r[ix+1]*sin(rad*t[iy]);
	zb[1] = z[findex2(ix+1,iy,nmx)];
	xb[2] = r[ix+1]*cos(rad*t[iy+1]);
	yb[2] = r[ix+1]*sin(rad*t[iy+1]);
	zb[2] = z[findex2(ix+1,iy+1,nmx)];
	xb[3] = r[ix]*cos(rad*t[iy+1]);
	yb[3] = r[ix]*sin(rad*t[iy+1]);
	zb[3] = z[findex2(ix,iy+1,nmx)];
}

static void gsgl_setsquare5 (float *z, float *x, float *y, int nmx, int nx, int ny, 
			     int ix, int iy,
			     float *xb, float *yb, float *zb)
{
	/*      float xb(4), yb(4), zb(4)
		float z(nmx,ny), x(nmx,ny), y(nmx,ny)*/

	if (ix >= nx-1) return;

	xb[0] = x[findex2(ix,iy,nmx)];
	yb[0] = y[findex2(ix,iy,nmx)];
	zb[0] = z[findex2(ix,iy,nmx)];
	xb[1] = x[findex2(ix+1,iy,nmx)];
	yb[1] = y[findex2(ix+1,iy,nmx)];
	zb[1] = z[findex2(ix+1,iy,nmx)];
	xb[2] = x[findex2(ix+1,iy+1,nmx)];
	yb[2] = y[findex2(ix+1,iy+1,nmx)];
	zb[2] = z[findex2(ix+1,iy+1,nmx)];
	xb[3] = x[findex2(ix,iy+1,nmx)];
	yb[3] = y[findex2(ix,iy+1,nmx)];
	zb[3] = z[findex2(ix,iy+1,nmx)];
}

static void gsgl_set_data (int itype, int nmx, int nx, int ny, float *z, 
			   float *x, float *y, int *ielm, gsglPage *p)
{
	float gxmin,gxmax,gymin,gymax,gzmin,gzmax;

	gsgl_inq_region_3d (&gxmin, &gxmax, &gymin, &gymax, &gzmin, &gzmax, p);

	/*    --- データをopenglが使える形でメモリに保存する ---*/
	if (itype == GSGL_DATA_ORTHO_EQUIV_INTERVAL) {
		gsglsetdatapointer1 (nmx, nx, ny, z, gxmin, gxmax, gymin, gymax, p);
	} else if (itype == GSGL_DATA_ORTHO_UNEQU_INTERVAL) {
		gsglsetdatapointer2 (nmx, nx, ny, z, x, y, p);
	} else if (itype == GSGL_DATA_CYLIN_EQUIV_INTERVAL) {
		gsglsetdatapointer3 (nmx, nx, ny, z, x, p);
	} else if (itype == GSGL_DATA_CYLIN_UNEQU_INTERVAL) {
		gsglsetdatapointer4 (nmx, nx, ny, z, x, y, p);
	} else if (itype == GSGL_DATA_X_Y_Z_2D) {
		gsglsetdatapointer5 (nmx, nx, ny, z, x, y, p);
	} else if (itype == GSGL_DATA_FEM_TRIANGLE) {
		gsglsetdatapointerf (nx, ny, ielm, z, x, y, p);
	}
}

static void gsgl_array_poly (int itype, int nmx, int nx, int ny, int iymax,
			     float *z, int *ielm, float z1, float z2, gsglPage *p)
{
	int ix, iy, ie;
	int ix1, ix2, ix3;

	if (itype != GSGL_DATA_FEM_TRIANGLE) {
        for (ix=0; ix<nx-1; ix++) {
		for (iy=0; iy<iymax; iy++) {
			if ((z[findex2(ix,iy,nmx)] > z1 && z[findex2(ix,iy+1,nmx)] > z1 && 
			     z[findex2(ix+1,iy%ny+1,nmx)]>z1 && z[findex2(ix+1,iy,nmx)]>z1) || 
			    (z[findex2(ix,iy,nmx)] < z2 && z[findex2(ix,iy%ny+1,nmx)] < z2 && 
			     z[findex2(ix+1,iy%ny+1,nmx)]<z2 && z[findex2(ix+1,iy,nmx)]<z2)) {
			} else {
				gsgl_glarray3d_begin_polygon (p);
				gsgl_glarray_element (ny*(ix)+iy, p);
				gsgl_glarray_element (ny*(ix+1)+iy, p);
				gsgl_glarray_element (ny*(ix+1)+(iy+1)%ny, p);
				gsgl_glarray_element (ny*(ix)+(iy+1)%ny, p);
				gsgl_glarray3d_end_polygon (p);
			}
		}
	}
	} else {
		for (ie=0; ie<ny; ie++) {
			ix1 = ielm[findex2(0,ie,3)]-1;
			ix2 = ielm[findex2(1,ie,3)]-1;
			ix3 = ielm[findex2(2,ie,3)]-1;
			if ((z[ix1] > z1 && z[ix2] > z1 && z[ix3] > z1) || 
			    (z[ix1] < z2 && z[ix2] < z2 && z[ix3] < z2)) {
			} else {
				gsgl_glarray3d_begin_polygon (p);
				gsgl_glarray_element (ix1, p);
				gsgl_glarray_element (ix2, p);
				gsgl_glarray_element (ix3, p);
				gsgl_glarray3d_end_polygon (p);
			}
		}
	}
}

static void gsgl_array_contour (int itype, int nmx, int nx, int ny, int iymax,
				float *z, float *x, float *y, int *ielm, float z1, float dx, float dy, gsglPage *p)
{
	float xn[4], yn[4], zn[4];
	float xt[4], yt[4], zt[4];
	gsglboolean l;
	int ix, iy, ie;
	int in1, in2, in3;

	if (itype != GSGL_DATA_FEM_TRIANGLE) {
		for (ix=0; ix<nx-1; ix++) {
			for (iy=0; iy<iymax; iy++) {
				if (itype == GSGL_DATA_ORTHO_EQUIV_INTERVAL) {
					gsgl_setsquare1 (z, nmx, dx, dy, ix, iy, xn, yn, zn, p);
				} else if (itype == GSGL_DATA_ORTHO_UNEQU_INTERVAL) {
					gsgl_setsquare2 (z, x, y,nmx,nx,ny,ix,iy,xn,yn,zn);
				} else if (itype == GSGL_DATA_CYLIN_EQUIV_INTERVAL) {
					gsgl_setsquare3 (z, x, nmx, nx, ny,ix,iy,xn,yn,zn);
				} else if (itype == GSGL_DATA_CYLIN_UNEQU_INTERVAL) {
					gsgl_setsquare4 (z, x, y,nmx,nx,ny,ix,iy,xn,yn,zn);
				} else if (itype == GSGL_DATA_X_Y_Z_2D) {
					gsgl_setsquare5 (z, x, y,nmx,nx,ny,ix,iy,xn,yn,zn);
				}
				l = (zn[0] > z1 && zn[1] > z1 && zn[2] > z1 && zn[3] > z1) || 
					(zn[0] < z1 && zn[1] < z1 && zn[2] < z1 && zn[3] < z1);
				if (!l) {
					gsglcontour3d (xn, yn, zn, z1);
					gsglanothertriangle (xt, yt, zt, xn, yn, zn);
					gsglcontour3d (xt, yt, zt, z1);
				}
			}
		}
	} else { /* FEM */
		for (ie=0; ie<ny; ie++) {
			in1 = ielm[findex2(0,ie,3)]-1;
			in2 = ielm[findex2(1,ie,3)]-1;
			in3 = ielm[findex2(2,ie,3)]-1;
			xn[0] = x[in1];
			xn[1] = x[in2];
			xn[2] = x[in3];
			yn[0] = y[in1];
			yn[1] = y[in2];
			yn[2] = y[in3];
			zn[0] = z[in1];
			zn[1] = z[in2];
			zn[2] = z[in3];
			l = (zn[0] > z1 && zn[1] > z1 && zn[2] > z1) || 
				(zn[0] < z1 && zn[1] < z1 && zn[2] < z1);
			if (!l) {
				gsglcontour3d (xn, yn, zn, z1);
			}
		}
	}
}
 
static void gsgl_array_lattice (int itype, int nx, int ny, int iymax, int *ielm, int ind, gsglPage *p)
{
	float gxmin,gxmax,gymin,gymax,gzmin,gzmax;
	int ie, in1, in2, in3, ix, iy;

	gsgl_inq_region_3d (&gxmin, &gxmax, &gymin, &gymax, &gzmin, &gzmax, p);

	/*    --- 格子を描く ---*/
	if (itype == GSGL_DATA_FEM_TRIANGLE) {
		if (ind == 3) {
			gsglupperclipplane (gzmax, p);
			gsgllowerclipplane (gzmin, p);
			for (ie=0; ie<ny; ie++) {
				in1 = ielm[findex2(0,ie,3)]-1;
				in2 = ielm[findex2(1,ie,3)]-1;
				in3 = ielm[findex2(2,ie,3)]-1;
				gsgl_glarray3d_begin_lines (p);
				gsgl_glarray_element (in1, p);
				gsgl_glarray_element (in2, p);
				gsgl_glarray_element (in3, p);
				gsgl_glarray3d_end_lines (p);
			}
			gsgl_clip_disable (4, p);
			gsgl_clip_disable (5, p);
		}
		return;
	}
	if (ind == 1 || ind == 3) {
		/*    --- x軸に沿った線のみ ---*/
		gsglupperclipplane (gzmax, p);
		gsgllowerclipplane (gzmin, p);
		for (ix=0; ix<nx; ix++) {
			gsgl_glarray3d_begin_lines (p);
			for (iy=0; iy<iymax; iy++) {
				gsgl_glarray_element (ny*(ix)+iy, p);
				gsgl_glarray_element (ny*(ix)+(iy+1)%ny, p);
			}
			gsgl_glarray3d_end_lines (p);
		}
		gsgl_clip_disable (4, p);
		gsgl_clip_disable (5, p);
	}
	if (ind == 2 || ind == 3) {
		/*    --- y軸に沿った線のみ ---*/
		gsglupperclipplane (gzmax, p);
		gsgllowerclipplane (gzmin, p);
		for (iy=0; iy<ny; iy++) {
			gsgl_glarray3d_begin_lines (p);
			for (ix=0; ix<nx-1; ix++) {
				gsgl_glarray_element (ny*(ix)+iy, p);
				gsgl_glarray_element (ny*(ix+1)+iy, p);
			}
			gsgl_glarray3d_end_lines (p);
		}
		gsgl_clip_disable (4, p);
		gsgl_clip_disable (5, p);
	}
}

static void gsgl_array_frame (int itype, int nmx, int nx, int ny, float *z, float *x, float *y, gsglPage *p)
{
	float gxmin,gxmax,gymin,gymax,gzmin,gzmax;

	gsgl_inq_region_3d (&gxmin, &gxmax, &gymin, &gymax, &gzmin, &gzmax, p);

	/*    --- 外枠を描く ---*/
	if (itype == GSGL_DATA_ORTHO_EQUIV_INTERVAL || 
	    itype == GSGL_DATA_ORTHO_UNEQU_INTERVAL || 
	    itype == GSGL_DATA_CYLIN_EQUIV_INTERVAL || 
	    itype == GSGL_DATA_CYLIN_UNEQU_INTERVAL) {
		gsglupperclipplane (gzmax, p);
		gsgllowerclipplane (gzmin, p);
		if (itype == GSGL_DATA_ORTHO_EQUIV_INTERVAL) {
			gsglpers1frame (z, nmx, nx, ny, p);
		} else if (itype == GSGL_DATA_ORTHO_UNEQU_INTERVAL) { 
			gsglpers2frame (z, x, y, nmx, nx, ny);
		} else if (itype == GSGL_DATA_CYLIN_EQUIV_INTERVAL) {
			gsglpers3frame (z, x, nmx, nx, ny);
		} else if (itype == GSGL_DATA_CYLIN_UNEQU_INTERVAL) {
			gsglpers4frame (z, x, y, nmx, nx, ny);
		}
		gsgl_clip_disable (4, p);
		gsgl_clip_disable (5, p);
	}
}

static void gsgl_array_color (int itype, float *z, int nmx, int nx, int ny, RGBFUNC, gsglPage *p)
{
	float gxmin,gxmax,gymin,gymax,gzmin,gzmax;
	float rgb[3];
	int i, j;
	float tmp;

	gsgl_inq_region_3d (&gxmin, &gxmax, &gymin, &gymax, &gzmin, &gzmax, p);

	if (itype == GSGL_DATA_FEM_TRIANGLE) {
		for (i=0; i<nx; i++) {
			tmp = (z[i]-gzmin)/(gzmax-gzmin);
			rgbfunc (&tmp, rgb);
			gsglsetcolorpointer (1, nx, 0, i, rgb, p);
		}
	} else {
		for (i=0; i<nx; i++) {
			for (j=0; j<ny; j++) {
				tmp = (z[findex2(i,j,nmx)]-gzmin)/(gzmax-gzmin);
				rgbfunc (&tmp, rgb);
				gsglsetcolorpointer (nx, ny, i, j, rgb, p);
			}
		}
	}
}
