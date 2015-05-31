#include "gsaf.h"
#include "gsgl.h"
#include "base.h"
#include "bsc1.h"
#include "bsc2.h"
#include "bsc3d.h"
#include "apl1.h"
#include "apl3d.h"
#include "utils.h"
#include "matrix.h"
#include "utils2.h"

static void gsgl_back_normal (float corner, float amax, float amin, float *a, float *v);
static void gsgl_checkscalprod (float *x, float *y, float *z, float *vec);
static gsglboolean logical_gsgl_checkscalprod (float *x, float *y, float *z, float *v);
static void gsgl_exchangevec (int nrank, float *vec);
static void gtxval (float org, float del, float pos2, float pos3, float scal, int ind, int ipos, int iz, gsglPage *);
static void gtyval (float yorg, float dpy, float xpos, float zpos, float scal, int ind, int ipos, int iz, gsglPage *);
static void gtzval (float zorg, float dpz, float xpos, float ypos, int nec, float scal, int ind, int ipos, gsglPage *);
static void gsgl_set_ijust_for_z (int *ijust, float xw1, float xw2, int ipos2);
static void gsgl_set_dxyz_for_z (float *p1, float *p2, float *p3, int ipos2, float *dx1, float *dy1, float *dz1, gsglPage *);
static void gsgl_inqxaxispos (int ipos, float *axis_y, float *axis_z, gsglPage *);
static void gsgl_inqyaxispos (int ipos, float *axis_x, float *axis_z, gsglPage *);
static void gsgl_inqzaxispos (int ipos, float *axis_x, float *axis_y, int *nec, gsglPage *);
static void gsgl_set4points (float zpos, float *pnearz, float *p1, float *p2, float *p3, float *p4, gsglPage *);
static void gsgl_viewfact (float zpos, float *p1, float *p2, float *p3, float *p4, float *viewfact, gsglPage *);
static void gscale3d (int iaxis, float org, float step, float scl, int ipos, gsglPage *);
static void gsgl_scale3d_longaxis (int iaxis, float *a1, float *a2, float amax, float amin, float *scale, int *nec, gsglPage *);
static void gsgl_scale (float org, float del, float pos2, float pos3, float scal, int id, int iaxis, float amin, float amax);
static float * gsgl_set_tics (int iaxis, float org, float del, float scal, int iz, int ipos_s1, float pos2, float pos3, int *ns, int *ne, gsglPage *);

/* draw axises of gdefine3d-defined region
 *    座標軸を描く．
 *    入力    iaxis   [i]     表示する軸を指定．
 *                            iaxis=0 奥側の軸を表示．
 *                            iaxis=1 手前の軸を表示．
 *                            iaxis=2 手前と奥の軸を表示．
 */
void gsaf_gaxis3d (int iaxis)
{
	float x[8], y[8], z[8];
	int i, j;
	int n;
	gsglPage *p;

	if (!gsgl_inq_page_is_open ()) return;
	p = gsgl_inq_page ();

	inqcorner3d (x, y, z, p);
/* --- 8つの角のうち一番近い点を求める --- */
	gsglinqnearestpoint (x, y, z, 8, &n, p);

	for (i=0; i<8; i++) {
		for (j=0; j<8; j++) {
			if ((i != j) && 
			    ((x[i] == x[j] && y[i] == y[j] && z[i] != z[j]) || 
			     (x[i] == x[j] && y[i] != y[j] && z[i] == z[j]) || 
			     (x[i] != x[j] && y[i] == y[j] && z[i] == z[j]))) { 
				/* 隣り合った異なる2点について */
				if (iaxis == 0 || iaxis == 2) { 
/* 後ろ側は共に一番近い点でなければ線を引き */
					if (i != n && j != n){
						gsaf_line3d(x[i], y[i], z[i], x[j], y[j], z[j]);
					}
				}
				if (iaxis == 1  ||  iaxis == 2) {
/* 表側はどちらかが一番近い点ならば引く */
					if (i == n  ||  j == n) {
/* ps に変換するとき，その線の深さをその線の一番浅い点(手前の点)から求める */
						gsgl_ps_depth_by_shallow (p);
						gsaf_line3d (x[i], y[i], z[i], x[j], y[j], z[j]);
/* ps に変換するとき，線の一番深い点から求める (default) */
						gsgl_ps_depth_by_deep (p);
					}                   
				}
			}
		}
	}
}

/* gdefine3d で定義された領域の背面を塗り潰す
 *
 *     入力    r       [r]     塗り潰す色 (rgb値のr)
 *             g       [r]     塗り潰す色 (rgb値のg)
 *             b       [r]     塗り潰す色 (rgb値のb)
 */
void gsaf_gdrawback3d (float r, float g, float b)
{
	float x[4], y[4], z[4];
	float vec[3];
	float p1[3], p2[3], p3[3], p4[3];
	float cr, cg, cb;
	float gxs, gxe, gys, gye, zmin, zmax;
	float pnearz;
	gsglPage *p;

	if (!gsgl_inq_page_is_open ()) return;

	p = gsgl_inq_page ();
	gsgl_inq_region_3d (&gxs, &gxe, &gys, &gye, &zmin, &zmax, p);
	gsaf_inqrgb (&cr,&cg,&cb);
	gsaf_setrgb (r,g,b);
/* --- このサブルーチンと3dオブジェクトの間に他のlistが入ってはいけない --- */
	/*gsgl2psdepthbydeepest (p);*/
/* --- z=constの平面を描く --- */
	x[0] = gxs;
	y[0] = gys;
	x[1] = gxs;
	y[1] = gye;
	x[2] = gxe;
	y[2] = gye;
	x[3] = gxe;
	y[3] = gys;
/* --- 視点から遠い方の z = const 面を選ぶ --- */
	gsgl_set4points(zmin, &pnearz, p1, p2, p3, p4, p);
	vec[0] = 0.0;
	vec[1] = 0.0;
	vec[2] = 0.0 ;
	gsgl_back_normal (pnearz, zmax, zmin, z, &vec[2]);
	gsgl_checkscalprod (x, y, z, vec);
	gsgl_ps_depth_by_vectprod2 (x, y, z, 4, x, y, z, p);
	gsaf_poly3d_with_normal (x, y, z, vec[0], vec[1], vec[2], 4);
/* --- x=constの平面を描く --- */
	z[0] = zmin;
	y[0] = gys;
	z[1] = zmin;
	y[1] = gye;
	z[2] = zmax;
	y[2] = gye;
	z[3] = zmax;
	y[3] = gys;
/* --- 視点から遠い方の x = const 面を選ぶ --- */
	gsgl_set4points(zmin, &pnearz, p1, p2, p3, p4, p);
	vec[0] = 0.0;
	vec[1] = 0.0;
	vec[2] = 0.0 ;
	gsgl_back_normal (p1[0], gxe, gxs, x, &vec[0]);
	gsgl_checkscalprod (x, y, z, vec);
	gsgl_ps_depth_by_vectprod2 (x, y, z, 4, x, y, z, p);
	gsaf_poly3d_with_normal (x, y, z, vec[0], vec[1], vec[2], 4);
/* --- y=constの平面を描く --- */
	z[0] = zmin;
	x[0] = gxs;
	z[1] = zmin;
	x[1] = gxe;
	z[2] = zmax;
	x[2] = gxe;
	z[3] = zmax;
	x[3] = gxs;
/* --- 視点から遠い方の y = const 面を選ぶ --- */
	gsgl_set4points(zmin, &pnearz, p1, p2, p3, p4, p);
	vec[0] = 0.0;
	vec[1] = 0.0;
	vec[2] = 0.0 ;
	gsgl_back_normal (p1[1], gye, gys, y, &vec[1]);
	gsgl_checkscalprod (x, y, z, vec);
	gsgl_ps_depth_by_vectprod2 (x, y, z, 4, x, y, z, p);
	gsaf_poly3d_with_normal (x, y, z, vec[0], vec[1], vec[2], 4);

	gsgl_ps_depth_by_deep (p);
	gsaf_setrgb (cr,cg,cb);
}

/*     入力    corner
               amax
               amin
       出力    a
               v
*/
static void gsgl_back_normal (float corner, float amax, float amin, float *a, float *v)
{
	int i;

	if ((corner-amin) < (amax-amin)*0.1) {
		for (i=0; i<4; i++) {
			a[i] = amax;
		}
		*v = -0.1*(amax-amin);
	} else {
		for (i=0; i<4; i++) {
			a[i] = amin;
		}
		*v = 0.1*(amax-amin);
	}
}

/*    input      vec[2]

      input & output 
                 x
                 y
                 z
*/
static void gsgl_checkscalprod (float *x, float *y, float *z, float *vec)
{
      if (!logical_gsgl_checkscalprod (x, y, z, vec)) {
	      gsgl_exchangevec (4, x);
	      gsgl_exchangevec (4, y);
	      gsgl_exchangevec (4, z);
      }
}

/* ------------------------------------------
 * --- 与えられたポリゴンの法線ベクトルと ---
 * --- 与えられたベクトル v の内積が正 or 0 なら true, 負なら false を返す ---
 * ------------------------------------------
 */
static gsglboolean logical_gsgl_checkscalprod (float *x, float *y, float *z, float *v)
{
	float vecx[2],vecy[2],vecz[2];
	float vnormal[3];
	float parity;
	int i;

	vecx[0] = x[1] - x[0];
	vecy[0] = y[1] - y[0];
	vecz[0] = z[1] - z[0];
	vecx[1] = x[2] - x[1];
	vecy[1] = y[2] - y[1];
	vecz[1] = z[2] - z[1];
	vnormal[0] = vecy[0]*vecz[1]-vecz[0]*vecy[1];
	vnormal[1] = vecz[0]*vecx[1]-vecx[0]*vecz[1];
	vnormal[2] = vecx[0]*vecy[1]-vecy[0]*vecx[1];
	parity=0.0;

	for (i=0; i<3; i++) {
		parity = parity + vnormal[i]*v[i];
	}
	if (parity > 0.0) {
		return GSGL_TRUE;
	} else {
		return GSGL_FALSE;
	}
}

/* -----------------------------------------
 * --- 配列 vec の要素の順番を入れ替える ---
 * -----------------------------------------
 */
static void gsgl_exchangevec (int nrank, float *vec)
{
	float *tmp;
	int i;

	tmp = (float *) gsgl_malloc_atomic (sizeof(float)*nrank);
	for (i=0; i<nrank; i++) {
		tmp[i] = vec[i];
	}
	for (i=0; i<nrank; i++) {
		vec[i] = tmp[nrank-1-i];
	}
}

/* draw xyz line graph */
void gsaf_gplotp3d (float *gx, float *gy, float *gz, int nsrt, int nend, int nstep, 
		    int imark, int istep, int ipat)
{
	int n;
	/*int imarks;
	  float hmrk, wmrk, angl, tilt;*/
	
	if (!gsgl_inq_page_is_open ()) return;

	if (imark == 0) {
		gsaf_movept3d (gx[nsrt], gy[nsrt], gz[nsrt], ipat);
		for (n=nsrt+nstep; n<=nend; n+=nstep)
			gsaf_drawpt3d (gx[n], gy[n], gz[n]);

	} else if (imark > 0) {
		/*gsaf_inqmrk (&imarks, &hmrk, &wmrk, &angl, &tilt);
		  gsaf_setmrk (imark, hmrk, wmrk, angl, tilt);
		  for (n=nsrt; n<=nend; n+=nstep) {
		  mark(px,py);
		  }
		  gsaf_setmrk (imarks,hmrk,wmrk,angl,tilt);*/
	} else {
		gsaf_movept3d (gx[nsrt], gy[nsrt], gz[nsrt], ipat);
		for (n=nsrt; n<=nend; n+=nstep)
			gsaf_drawpt3d (gx[n], gy[n], gz[n]);

		/*gsaf_inqmrk (&imarks, &hmrk, &wmrk, &angl, &tilt);
		  gsaf_setmrk (abs(imark), hmrk, wmrk, angl, tilt);
		  for (n=nsrt; n<=nend; n+=nstep) {
		  mark(px,py);
		  }
		  gsaf_setmrk (imarks,hmrk,wmrk,angl,tilt);*/
	}
}

/* draw ticks on x-axis
 *     引数    xorg,xstep は gscale と同じ．
 *             step が 0.0 の時には目盛は描かれない．
 *             scl     [r]     目盛りの長さ (world 座標系)
 *             ind     [i]     目盛の付け方
 *             ind=0 xy平面内に格子を描く．この時線種はint(scl)．
 *             ind=1 position1のx軸にxy平面内に内側の目盛を描く．
 *             ind=2 position1のx軸にxy平面内に外側の目盛を描く．
 *             ind=3 position1のx軸にxy平面内に内外両側の目盛を描く．
 *             +3    x,y軸の目盛をz軸に平行に．
 *             +6    見る角度によって+0か+3を自動的に選択
 *             +10   position2
 *             +20   position3
 *             +30   position4
 *
 *             position1 とはグラフを上側から眺める時は下側手前の軸．
 *                           グラフを下側から眺める時は上側手前の軸．
 *             position2 とはグラフを上側から眺める時は下側奥側の軸．
 *                           グラフを下側から眺める時は上側奥側の軸．
 *             position3 とはグラフを上側から眺める時は上側手前の軸．
 *                           グラフを下側から眺める時は下側手前の軸．
 *             position4 とはグラフを上側から眺める時は上側奥側の軸．
 *                           グラフを下側から眺める時は下側奥側の軸．
 *
 *     目盛は開始値から指定された間隔で正負両方向につけられる．
 *     開始値がその軸の座標の最小値と最大値の間になければ，目
 *     盛は表示されない．
 */
void gsaf_gscale3dx (float xorg, float xstep, float scl, int ipos)
{
	if (!gsgl_inq_page_is_open ()) return;
	gscale3d (1, xorg, xstep, scl, ipos, gsgl_inq_page());
}

void gsaf_gscale3dy (float yorg, float ystep, float scl, int ipos)
{
	if (!gsgl_inq_page_is_open ()) return;
	gscale3d (2, yorg, ystep, scl, ipos, gsgl_inq_page());
}

/*
 *   position1
 *   if ipos=0, 
 *   if ipos=1, internal .
 *   if ipos=2, outer .
 *   if ipos=3, internal and outer .
 *   +10 position2
 *   +20 position3
 *   +30 position4
 */
void gsaf_gscale3dz (float zorg, float zstep, float scl, int ipos)
{
	if (!gsgl_inq_page_is_open ()) return;
	gscale3d (3, zorg, zstep, scl, ipos, gsgl_inq_page());
}

/* draw ticks on axis */
static void gscale3d (int iaxis, float org, float step, float scl, int ipos, gsglPage *p)
{
	float xa[3], ya[3], za[3];
	float xl,yl,zl;
	int ipos1, ipos2;
	float a1, a2;
	float gxs, gxe, gys, gye, zmin, zmax;
	float amax, amin;
	float rad, theta;
	int ir, iz;
	int ipos_com;
	int nec;
	float scal_com;
	float scal, scale;
	const float eps = 0.01f;
	gsglScale3D *info;

	gsgl_inq_length_3d (&xl, &yl, &zl, p);
	gsgl_inq_region_3d (&gxs, &gxe, &gys, &gye, &zmin, &zmax, p);
	gsgl_inq_axis_3d (xa, ya, za, p);

	ipos1 = ipos%10;
	ipos2 = (int) (0.1*ipos+eps);
	if (ipos2 >= 4) {
		gsgl_warning ("gscale3d", "ipos should be less than 40");
		return;
	}
	ipos_com = ipos;
	
	gsgl2psdepthbydeepest (p);

/* --- このサブルーチンと3dオブジェクトの間に他のlistが ---
   ---  入ってはいけない --- */

	scal = 0.0;
	scal_com = 0.0;
	if (iaxis == 1) {
		gsgl_inqxaxispos (ipos2+1,&a1,&a2,p);
		amax = gye;
		amin = gys;
		scal_com = scl/yl*(gye-gys); /* cm を y軸の値に */
	} else if (iaxis == 2) {
		gsgl_inqyaxispos (ipos2+1, &a1, &a2, p);
		amax = gxe;
		amin = gxs;
		scal_com = scl/xl*(gxe-gxs); /* cm を x軸の値に */
	} else if (iaxis == 3) {
		amin = 0.0;
		amax = 0.0;
		gsgl_inqzaxispos (ipos2+1, &a1, &a2, &nec, p);
		if (nec == 0) {
			scal_com = scl/xl*(gxe-gxs);
		} else {
			scal_com = scl/yl*(gye-gys);
		}
	} else {
		return;
	}

/* --- 目盛りを格子状に書くときは別に扱う --- */
	if (ipos1 == 0) {
		gsgl_scale3d_longaxis (iaxis, &a1, &a2, amax, amin, &scale, &nec, p);
		iz=0;
		ir=1;
		goto THREE;
	}
/* --- 目盛りが内側 ind=1,4,7 --- */
	if (ipos1%3 == 1) {
		scal_com = -scal_com;
	}
	scal = scal_com;
	ir=1;
	rad = atan(1.0)*4.0/180.0;
	theta = acos (za[2]); /* (za[0],za[1],za[2]) dot (0, 0, 1) */
	if (ipos1 >= 7) {/* 見る角度によって+0か+3か (7以上) */
		if (cos(2.*rad*theta) >= 0.0) {
			iz=0;
		} else {
			iz=1;
		}
	} else if (ipos1 >= 4) {/* z軸に平行に (4以上) */
		iz=1;
	} else {
		iz=0;
	}
	if (ipos1%3 == 0) ir=0; /* 内外 */
/* --- 見る角度によって目盛りの方向を変化 --- */
 ONE:
	if (iaxis == 1) {
		gsgl_inqxaxispos (ipos2+1, &a1, &a2, p);
	} else if (iaxis == 2) {
		gsgl_inqyaxispos (ipos2+1, &a1, &a2, p);
	}
	if (iaxis == 1) {
		if (iz == 0) {
			if (fabs((a1-gys)/(gye-gys)) < eps) {
				scale=-scal;
			} else {
				scale=scal;
			}
		} else if (iz == 1) {
			if (fabs((a2-zmin)/(zmax-zmin)) < eps) {
				scale=-scal*yl/(gye-gys)/zl*(zmax-zmin);
			} else {
				scale=scal*yl/(gye-gys)/zl*(zmax-zmin);
			}
		}
	} else if (iaxis == 2) {
		if (iz == 0) {
			if (fabs((a1-gxs)/(gxe-gxs)) < eps) {
				scale=-scal;
			} else {
				scale=scal;
			}
		} else if (iz == 1) {
			if (fabs((a2-zmin)/(zmax-zmin)) < eps) {
				scale=-scal*xl/(gxe-gxs)/zl*(zmax-zmin);
			} else {
				scale=scal*xl/(gxe-gxs)/zl*(zmax-zmin);
			}
		}
	} else if (iaxis == 3) {
		if (nec == 0) {
			if (fabs((a1-gxs)/(gxe-gxs)) < eps) {
				scale=-scal;
			} else {
				scale=scal;
			}
		} else {
			if (fabs((a2-gys)/(gye-gys)) < eps) {
				scale=-scal;
			} else {
				scale=scal;
			}
		}
	}
	/* --- 実際の描画 --- */
 THREE:
	if (iaxis == 1) {
		gsgl_scale (org,step,a1,a2,scale,iz,iaxis,gxs,gxe);
	} else if (iaxis == 2) {
		gsgl_scale (org,step,a1,a2,scale,iz,iaxis,gys,gye);
	} else if (iaxis == 3) {
		gsgl_scale (org,step,a1,a2,scale,nec,iaxis,zmin,zmax);
		if (ipos1 == 0) {
			gsgl_scale3d_longaxis (3+1,&a1,&a2,amax,amin,&scale,&nec,p);
			gsgl_scale (org,step,a1,a2,scale,nec,iaxis,zmin,zmax);
		}
	}
	/* --- 3,6,9 ならば目盛りの方向を変えてもう1度描画 --- */
	if (ipos1%3 == 0 && ir == 0) {
		scal=-scal;
		ir=1;
		goto ONE;
	}

	gsgl_ps_depth_by_deep (p);

	info = p->scale3d;

	if (iaxis == 1) {
		info->Iposx = ipos_com;
		info->Sclx = scal_com;
	} else if (iaxis == 2) {
		info->Iposy = ipos_com;
		info->Scly = scal_com;
	} else if (iaxis == 3) {
		info->Iposz = ipos_com;
		info->Sclz = scal_com;
	}
}

static void gsgl_scale3d_longaxis (int iaxis, float *a1, float *a2, float amax, float amin, float *scale, int *nec, gsglPage *p)
{
	const float eps = 0.01f;
	float gxs, gxe, gys, gye, zmin, zmax;
	float x[8], y[8], z[8];
	float pnearz;
	float p1[3], p2[3], p3[3], p4[3];
	float vec[3];
	int n;

	gsgl_inq_region_3d (&gxs, &gxe, &gys, &gye, &zmin, &zmax, p);

	if (iaxis == 1 || iaxis == 2) {
		if (fabs((*a1-amin)/(amax-amin)) < eps) {
			*scale=amax-amin;
		} else {
			*scale=-(amax-amin);
		}
		x[0] = gxs;
		y[0] = gys;
		x[1] = gxs;
		y[1] = gye;
		x[2] = gxe;
		y[2] = gye;
		x[3] = gxe;
		y[3] = gys;
		gsgl_set4points(zmin, &pnearz, p1, p2, p3, p4, p);
		vec[0] = 0.0;
		vec[1] = 0.0;
		vec[2] = 0.0 ;
		gsgl_back_normal (pnearz, zmax, zmin, z, &vec[2]);
		gsgl_checkscalprod (x, y, z, vec);
		gsgl_ps_depth_by_vectprod2 (x, y, z, 4, x, y, z, p);
		return;
	} else if (iaxis == 3) {
		gsgl_inqzaxispos (1, a1, a2, nec, p);
		*nec = 1-(*nec);

		inqcorner3d (x, y, z, p);
		gsglinqmostfarpoint (x, y, z, 8, &n, p);

		if ((*a1-x[n])/(gxe-gxs) < 0.1) {
			/* --- x=constの平面 --- */
			z[0] = zmin;
			y[0] = gys;
			z[1] = zmin;
			y[1] = gye;
			z[2] = zmax;
			y[2] = gye;
			z[3] = zmax;
			y[3] = gys;
			gsgl_set4points(zmin, &pnearz, p1, p2, p3, p4, p);
			vec[0] = 0.0;
			vec[1] = 0.0;
			vec[2] = 0.0 ;
			gsgl_back_normal (p1[0], gxe, gxs, x, &vec[0]);
			gsgl_checkscalprod (x, y, z, vec);
			gsgl_ps_depth_by_vectprod2 (x, y, z, 4, x, y, z, p);
		} else {
                        /* --- y=constの平面 --- */
			z[0] = zmin;
			x[0] = gxs;
			z[1] = zmin;
			x[1] = gxe;
			z[2] = zmax;
			x[2] = gxe;
			z[3] = zmax;
			x[3] = gxs;
			gsgl_set4points(zmin, &pnearz, p1, p2, p3, p4, p);
			vec[0] = 0.0;
			vec[1] = 0.0;
			vec[2] = 0.0 ;
			gsgl_back_normal (p1[1], gye, gys, y, &vec[1]);
			gsgl_checkscalprod (x, y, z, vec);
			gsgl_ps_depth_by_vectprod2 (x, y, z, 4, x, y, z, p);
		}
	} else if (iaxis == 3+1) {
		gsgl_inqzaxispos (2, a1, a2, nec, p);

		inqcorner3d (x, y, z, p);
		gsglinqmostfarpoint (x, y, z, 8, &n, p);

		if ((*a1-x[n])/(gxe-gxs) < 0.1) {
			/* --- x=constの平面 --- */
			z[0] = zmin;
			y[0] = gys;
			z[1] = zmin;
			y[1] = gye;
			z[2] = zmax;
			y[2] = gye;
			z[3] = zmax;
			y[3] = gys;
			gsgl_set4points(zmin, &pnearz, p1, p2, p3, p4, p);
			vec[0] = 0.0;
			vec[1] = 0.0;
			vec[2] = 0.0 ;
			gsgl_back_normal (p1[0], gxe, gxs, x, &vec[0]);
			gsgl_checkscalprod (x, y, z, vec);
			gsgl_ps_depth_by_vectprod2 (x, y, z, 4, x, y, z, p);
		} else {
                        /* --- y=constの平面 --- */
			z[0] = zmin;
			x[0] = gxs;
			z[1] = zmin;
			x[1] = gxe;
			z[2] = zmax;
			x[2] = gxe;
			z[3] = zmax;
			x[3] = gxs;
			gsgl_set4points(zmin, &pnearz, p1, p2, p3, p4, p);
			vec[0] = 0.0;
			vec[1] = 0.0;
			vec[2] = 0.0 ;
			gsgl_back_normal (p1[1], gye, gys, y, &vec[1]);
			gsgl_checkscalprod (x, y, z, vec);
			gsgl_ps_depth_by_vectprod2 (x, y, z, 4, x, y, z, p);
		}
	}
	if (*nec == 0) {
		if (fabs((*a1-gxs)/(gxe-gxs)) < eps) {
			*scale=gxe-gxs;
		} else {
			*scale=-(gxe-gxs);
		}
	} else {
		if (fabs((*a2-gys)/(gye-gys)) < eps) {
			*scale=gye-gys;
		} else {
			*scale=-(gye-gys);
		}
	}
}

/*     draw ticks
 *     if i1=0, tick's length is scl(cm)
 *     if i1=1, scl is pre-transformed tick's length
 *     if i1=2, latice
 */
static void gsgl_scale (float org, float del, float pos2, float pos3, float scal,
			int id /* = iz */, int iaxis, float amin, float amax)
{
	const float eps = 0.01f;
	float p0, p1, p2, p3;
	int ns, ne;
	float vn1, vn2, vn3;
	float px, py, pz;
	float d2, d3;
	float dx, dy, dz;
	int n;

	if (org < amin || org > amax) return;
	if (fabs(del)/(amax-amin) < eps) return;

	p0 = amin-org;
	p1 = amax-org;
	ns = (int) (p0/del-eps);
	ne = (int) (p1/del+eps);
	p2 = pos2;
	p3 = pos3;
	p1 = org;

	if (id == 0) {
		d2 = scal;
		d3 = 0.0;
/* --- fix me --- */
		if (iaxis != 3) {
			vn1 = 0.0;
			vn2 = 0.0;
			vn3 = 1.0;
		} else {
			vn1 = 0.0;
			vn2 = 1.0;
			vn3 = 0.0;
		}
	} else {
		d2 = 0.0;
		d3 = scal;
/* --- fix me --- */
		vn3 = 0.0;
		if (iaxis == 1) {
			vn1 = 0.0;
			vn2 = 1.0;
		} else if (iaxis == 2) {
			vn1 = 1.0;
			vn2 = 0.0;
		} else {
			vn1 = 1.0;
			vn2 = 0.0;
		}
	}

	for (n=ns; n<=ne; n++) {
		if (iaxis == 1) {
			px=p1+del*n;
			py=p2;
			pz=p3;
			dx=0.0;
			dy=d2;
			dz=d3;
		} else if (iaxis == 2) {
			py=p1+del*n;
			px=p2;
			pz=p3;
			dx=d2;
			dy=0.0;
			dz=d3;
		} else if (iaxis == 3) {
			pz=p1+del*n;
			px=p2;
			py=p3;
			dx=d2;
			dy=d3;
			dz=0.0;
		} else {
			return;
		}
		gsaf_line3d_with_normal (px, py, pz, px+dx, py+dy, pz+dz, vn1, vn2, vn3);
	}
}

/* draw numbers */
static void gvalue3d (int iaxis, float org, float del, int ipos, int ind, gsglPage *p)
{
	float xa[3], ya[3], za[3];
	float gxs, gxe, gys, gye, zmin, zmax;
	float rad, theta;
	int iz;
	float amin, amax, a1, a2, scale = 0.0;
	int nec;
	int ipos1, ipos2;
	float scal;
	const float eps = 0.01f;
	gsglScale3D *info = p->scale3d;

	gsgl_inq_axis_3d (xa, ya, za, p);
	gsgl_inq_region_3d (&gxs, &gxe, &gys, &gye, &zmin, &zmax, p);

	if (iaxis == 1) {
		scal = info->Sclx;
	} else if (iaxis == 2) {
		scal = info->Scly;
	} else if (iaxis == 3) {
		scal = info->Sclz;
	} else {
		return;
	}
	ipos1 = ipos % 10;
	ipos2 = (int) (ipos*0.1+eps);
	if (ipos2 >= 4) {
		gsgl_warning ("gvalue3d", "ipos should be less than 40");
		return;
	}
	rad = 3.1415927/180.0;
/* --- iz = 0 baseline が x-y 平面に含まれる ---
   --- iz = 1 baseline が x,or,y-z 平面に含まれる --- */
	theta = acos (za[2]);      /* (za[0],za[1],za[2]) dot (0, 0, 1) */
	if (ipos1 == 6 || ipos1 == 7 || ipos1 == 8) {
		if (ipos1 == 6) {
			iz=0;
			if (cos(2.0*rad*theta) >= 0.0) {
				ipos=1;
			} else {
				ipos=3;
			}
		} else if (cos(2.0*rad*theta) >= 0.0) {
			iz=0;
		} else {
			iz=1;
		}
	} else if ((int) (ipos1/4) == 0) { /* 1, 2, 3 */
		iz=0;
	} else {                      /* 4, 5 */
		iz=1;
	}
	if (iaxis == 1) {
		amin = gys;
		amax = gye;
		gsgl_inqxaxispos(ipos2+1,&a1,&a2,p);
	} else if (iaxis == 2) {
		amin = gxs;
		amax = gxe;
		gsgl_inqyaxispos(ipos2+1,&a1,&a2,p);
	} else if (iaxis == 3) {
		amin = zmin; /* ??? */
		amax = zmax; /* ??? */
		gsgl_inqzaxispos(ipos2+1,&a1,&a2,&nec,p);
	} else {
		return;
	}
	if (iaxis != 3) {
		if (iz == 0) {
			if (fabs((a1-amin)/(amax-amin)) < eps) {
				scale=-scal;
			} else {
				scale=scal;
			}
		} else if (iz == 1) {
			if (fabs((a2-zmin)/(zmax-zmin)) < eps) {
				scale=-scal;
			} else {
				scale=scal;
			}
		}
	} else {
		if (nec == 0) {
			if (fabs((a1-gxs)/(gxe-gxs)) < eps) {
				scale=-scal;
			} else {
				scale=scal;
			}
		} else {
			if (fabs((a2-gys)/(gye-gys)) < eps) {
				scale=-scal;
			} else {
				scale=scal;
			}
		}
	}
	if (iaxis == 1) {
		gtxval (org,del,a1,a2,scale,ind,ipos,iz,p);
	} else if (iaxis == 2) {
		gtyval (org,del,a1,a2,scale,ind,ipos,iz,p);
	} else if (iaxis == 3) {
		gtzval (org,del,a1,a2,nec,scale,ind,ipos,p);
	}
}

/* draw numbers on x-axis
 *       引数  xorg,xstep は gscale3dx と同じ．
 *             xstep が 0.0 の時には数値は描かれない．
 *
 *             ipos  [i]   数値の書き方を指定．
 *             1     position1のxy平面内にx,y軸に平行に数字を描く．
 *             2     position1のxy平面上に視線に垂直に数字を描く．   
 *             3     position1のxy平面上にz軸に平行に描く．
 *             4     視点から最も近いxz平面又はyz平面に常に平行に
 *                   z軸に平行に描く．
 *             5     position1のyz又はzx平面内に視線に垂直に描く．
 *             6,7,8 見る角度によって数字の角度を自動的に選択．
 *                   xy平面内に描かれる場合，6,7は1，8は2に対応する．
 *                   z軸に平行に描かれる場合，6は3,7は4,8は5と同じ
 *                   になる． 
 *             +10   position2
 *             +20   position3
 *             +30   position4
 *
 *             indx は gnumbr の ind と同じ．
 *
 *    目盛り数値は開始値から指定された間隔で正負両方向につけられる．
 *    開始値がその軸の座標の最小値と最大値の間になければ，目盛り数値は
 *    表示されない．
 */
void gsaf_gvalue3dx (float xorg, float dpx, int ipos, int ind)
{
	if (!gsgl_inq_page_is_open ()) return;
	gvalue3d (1, xorg, dpx, ipos, ind, gsgl_inq_page());
}

void gsaf_gvalue3dy (float yorg, float dpy, int ipos, int ind)
{
	if (!gsgl_inq_page_is_open ()) return;
	gvalue3d (2, yorg, dpy, ipos, ind, gsgl_inq_page());
}

void gsaf_gvalue3dz (float zorg, float dpz, int ipos, int ind)
{
	if (!gsgl_inq_page_is_open ()) return;
	gvalue3d (3, zorg, dpz, ipos, ind, gsgl_inq_page());
}

static float * gsgl_set_tics (int iaxis, float org, float del, float scal, int iz, 
			      int ipos_s1, float pos2, float pos3, int *ns, int *ne,
			      gsglPage *p)
{
	float xl,yl,zl;
	float gxs, gxe, gys, gye, zmin, zmax;
	float amin, amax, bmin, bmax;
	float bl;
	float a0, a1;
	float px, py, pz;
	float del2, del3;
	int n;
	int ntics;
	float *tic;
	int nmin;
	const float eps = 0.01f;	

	gsgl_inq_length_3d (&xl, &yl, &zl, p);
	gsgl_inq_region_3d (&gxs, &gxe, &gys, &gye, &zmin, &zmax, p);

	if (iaxis == 1) {
		amin = gxs;
		amax = gxe;
		bmin = gys;
		bmax = gye;
		bl = yl;
	} else if (iaxis == 2) {
		amin = gys;
		amax = gye;
		bmin = gxs;
		bmax = gxe;
		bl = xl;
	} else if (iaxis == 3) {
		amin = zmin;
		amax = zmax;
		bmin = zmin;
		bmax = zmax;
		bl = zl;
	} else {
		return NULL;
	}
	a0 = amin-org;
	a1 = amax-org;
	*ns = (int) (a0/del-eps);
	*ne = (int) (a1/del+eps);
	nmin = *ns;
	ntics = (*ne) - (*ns) + 1;
	tic = (float *) gsgl_malloc_atomic (sizeof(float)*(ntics*4+1));

	if (iz == 0) { /* baseline が x-y 平面に含まれる */
		del2 = scal;
		del3 = 0.0;
	} else { /* baseline が x || y-z 平面に含まれる */
		del2 = 0.0;
		del3 = scal/(bmax-bmin)*bl/zl*(zmax-zmin);
	}
	if (ipos_s1%3 != 1 && ipos_s1 != 0) { 
		/* ---  2, 3, 5, 6, 8, 9 <= 目盛りが外側 --- */
		for (n=*ns; n<=*ne; n++) {
			if (iaxis == 1) {
				px = org + del * n;
				tic[findex2(n-nmin,0,ntics)]=px;
				tic[findex2(n-nmin,1,ntics)]=pos2+del2;
				tic[findex2(n-nmin,2,ntics)]=pos3+del3;
				tic[findex2(n-nmin,3,ntics)]=px;
			} else if (iaxis == 2) {
				py = org + del * n;
				tic[findex2(n-nmin,0,ntics)]=pos2+del2;
				tic[findex2(n-nmin,1,ntics)]=py;
				tic[findex2(n-nmin,2,ntics)]=pos3+del3;
				tic[findex2(n-nmin,3,ntics)]=py;
			} else if (iaxis == 3) {
				pz = org + del * n;
				tic[findex2(n-nmin,0,ntics)]=pos2+del2;
				tic[findex2(n-nmin,1,ntics)]=pos3+del3;
				tic[findex2(n-nmin,2,ntics)]=pz;
				tic[findex2(n-nmin,3,ntics)]=pz;
			}
		}
	} else {                      /* 0, 1, 4, 7 <= 目盛りが内側 */
		for (n=(*ns); n<=(*ne); n++) {
			if (iaxis == 1) {
				px=org+del*n;
				tic[findex2(n-nmin,0,ntics)]=px;
				tic[findex2(n-nmin,1,ntics)]=pos2;
				tic[findex2(n-nmin,2,ntics)]=pos3;
				tic[findex2(n-nmin,3,ntics)]=px;
			} else if (iaxis == 2) {
				py=org+del*n;
				tic[findex2(n-nmin,0,ntics)]=pos2;
				tic[findex2(n-nmin,1,ntics)]=py;
				tic[findex2(n-nmin,2,ntics)]=pos3;
				tic[findex2(n-nmin,3,ntics)]=py;
			} else if (iaxis == 3) {
				pz=org+del*n;
				tic[findex2(n-nmin,0,ntics)]=pos2;
				tic[findex2(n-nmin,1,ntics)]=pos3;
				tic[findex2(n-nmin,2,ntics)]=pz;
				tic[findex2(n-nmin,3,ntics)]=pz;
			}
		}
	}
	return tic;
}

static void gsgl_viewfact (float zpos, float *p1, float *p2, float *p3, float *p4, float *viewfact, gsglPage *p)
{
	float pnearz;
	float xa[3], ya[3], za[3];	

	gsgl_inq_axis_3d (xa, ya, za, p);

	gsgl_set4points (zpos, &pnearz, p1, p2, p3, p4, p);

	if (za[2] >= 0.0) {
		*viewfact = 1.0;
	} else {
		*viewfact = -1.0;
	}
}

static void gsgl_p1_p2_is_axis (int iaxis, int ipos2, float viewfact, float *p1,
				float *d1, float *d2, float *d3, gsglPage *p)
{
	float xl,yl,zl;
	float chh,chw,chsp,angl,tilt;
	float d201, d202;
	float gxs, gxe, gys, gye, zmin, zmax;
	float bmin, bmax, bl, point;
	const float eps = 0.01f;
	
	gsgl_inq_length_3d (&xl, &yl, &zl, p);
	gsgl_inq_region_3d (&gxs, &gxe, &gys, &gye, &zmin, &zmax, p);
	gsaf_inqchr (&chh, &chw, &chsp, &angl, &tilt);

	if (iaxis == 1) {
		point = p1[1];
		bl = yl;
		bmax = gye;
		bmin = gys;
	} else if (iaxis == 2) {
		point = p1[0];
		bl = xl;
		bmax = gxe;
		bmin = gxs;
	} else {
		return;
	}

	*d1 = 0.0;
	*d3 = 0.0;
	d201 = (1.5+0.5*viewfact)*chh/bl*(bmax-bmin);
	d202 = (1.5-0.5*viewfact)*chh/bl*(bmax-bmin);
	if (ipos2%2 == 0) { /* position 1, 3 */
		if (fabs(point-bmax)/(bmax-bmin) < eps) {
			*d2 =  d201;
		} else {
			*d2 = -d201;
		}
	} else {            /* position 2, 4 */
		if (fabs(point-bmax)/(bmax-bmin) < eps) {
			*d2 = -d202;
		} else {
			*d2 =  d202;
		}
	}
}

static void gsgl_p1_p3_is_axis (int iaxis, int ipos2, float *p1, float *v2, 
				float xw1, float xw2,
				int *ijust, float *d1, float *d2, float *d3, gsglPage *p)
{
	float xl,yl,zl;
	float chh,chw,chsp,angl,tilt;
	float gxs, gxe, gys, gye, zmin, zmax;
	float amin, amax, bmin, bmax;
	float al, bl;
	float d20;
	float point;
	const float eps = 1.e-6f;

	gsgl_inq_length_3d (&xl, &yl, &zl, p);
	gsgl_inq_region_3d (&gxs, &gxe, &gys, &gye, &zmin, &zmax, p);
	gsaf_inqchr (&chh, &chw, &chsp, &angl, &tilt);

	if (iaxis == 1) {
		point = p1[1];
		amin = gxs;
		amax = gxe;
		bmin = gys;
		bmax = gye;
		al = xl;
		bl = yl;
	} else if (iaxis == 2) {
		point = p1[0];
		amin = gys;
		amax = gye;
		bmin = gxs;
		bmax = gxe;
		al = yl;
		bl = xl;
	} else {
		return;
	}
	*d3 = 0.0;
	d20 = 1.5*chw/bl*(bmax-bmin);

	if (fabs(v2[iaxis-1]) < eps) { /*!!!*/
		*d1 = 0.5*chh/al*(amax-amin) * (-1.0);
	} else {
		*d1 = 0.5*chh/al*(amax-amin) * (-v2[iaxis-1])/fabs(v2[iaxis-1]); /*!!!*/
	}

	if (ipos2%2 == 0) { /* position 1, 3 */
		if (xw1 > xw2) {
			*ijust = 0;
		} else {
			*ijust = 1;
		}
		if (fabs(point-bmax)/(bmax-bmin) < eps) {
			*d2 =  d20;
		} else {
			*d2 = -d20;
		}
	} else {            /* position 2, 4 */
		if (xw1 > xw2) {
			*ijust = 1;
		} else {
			*ijust = 0;
		}
		if (fabs(point-bmax)/(bmax-bmin) < eps) {
			*d2 = -d20;
		} else {
			*d2 =  d20;
		}                   
	}
}

static void gtxval (float org, float del, float pos2, float pos3, float scal,
		    int ind, int ipos, int iz, gsglPage *p)
{
	float xl,yl,zl;
	float *tic;
	float p1[3], p2[3], p3[3], p4[3];
	float v1[3], v2[3];
	float xw1, yw1, zw1;
	float xw2, yw2, zw2;
	float xw3, yw3, zw3;
	float xw4, yw4, zw4;
	float chh,chw,chsp,angl,tilt;
	float gxs, gxe, gys, gye, zmin, zmax;
	float dx1, dy1, dz1;
	int ipos1, ipos2;
	int ijust, i;
	float dz01, dz02;
	int ns, ne;
	float viewfact;
	float vlength;
	int idrc;
	int ntics;	
	const float eps = 0.01f;
	gsglScale3D *info = p->scale3d;

	gsgl_inq_region_3d (&gxs, &gxe, &gys, &gye, &zmin, &zmax, p);

	if (org < gxs || org > gxe) return;
	if (fabs(del/(gxe-gxs)) < eps) return;

	gsgl_inq_length_3d (&xl, &yl, &zl, p);

	ipos1 = ipos % 10;
	ipos2 = (int) (0.1*ipos+eps);

	tic = gsgl_set_tics (1, org, del, scal, iz, info->Iposx%10, pos2, pos3, &ns, &ne, p);
	if (tic == NULL) return;
	ntics = ne - ns + 1;

	gsgl_viewfact (pos3, p1, p2, p3, p4, &viewfact, p);
	gsglproject3d (p1[0], p1[1], p1[2], &xw1, &yw1, &zw1, p);
	gsglproject3d (p2[0], p2[1], p2[2], &xw2, &yw2, &zw2, p);
	gsglproject3d (p3[0], p3[1], p3[2], &xw3, &yw3, &zw3, p);
	gsglproject3d (p4[0], p4[1], p4[2], &xw4, &yw4, &zw4, p);

	gsaf_inqchr (&chh,&chw,&chsp,&angl,&tilt);

	if (xw2 >= xw1) {
		v1[0] = p2[0] - p1[0];
		v1[1] = p2[1] - p1[1];
		v1[2] = p2[2] - p1[2];
	} else {
		v1[0] = -p2[0] + p1[0];
		v1[1] = -p2[1] + p1[1];
		v1[2] = -p2[2] + p1[2];       
	}
	vlength = sqrt(v1[0]*v1[0]+v1[1]*v1[1]+v1[2]*v1[2]);
	v1[0] = v1[0]/vlength;
	v1[1] = v1[1]/vlength;
	v1[2] = v1[2]/vlength;

	if (yw3 >= yw1) {
		v2[0] = p3[0] - p1[0];
		v2[1] = p3[1] - p1[1];
		v2[2] = p3[2] - p1[2];
	} else {
		v2[0] = -p3[0] + p1[0];
		v2[1] = -p3[1] + p1[1];
		v2[2] = -p3[2] + p1[2];
	}
	vlength = sqrt(v2[0]*v2[0]+v2[1]*v2[1]+v2[2]*v2[2]);
	v2[0] = v2[0]/vlength;
	v2[1] = v2[1]/vlength;
	v2[2] = v2[2]/vlength;

	if (iz == 0) {
		if ((ipos1-2)%3 == 0) { /* 2, 5, 8 <= 数字を視線に垂直に */
			idrc=0;
		} else if (ipos1%3 == 0) { /* 3, 6 <= 法線が z 軸に平行 */
			idrc=7;
		} else {                /* 1, 7(=1) <= 法線が y 軸に平行 */
			idrc=3;
		}
		if (p1[0] != p2[0]) { /* p1-p2 が x軸 */
			gsgl_p1_p2_is_axis (1,ipos2,viewfact,p1,&dx1,&dy1,&dz1,p);
			ijust = 3;
		} else {              /* p1-p3 が x軸 */
			gsgl_p1_p3_is_axis (1, ipos2, p1, v2, xw1, xw2,
					    &ijust, &dx1, &dy1, &dz1, p);
		}
		if (idrc == 3) {
			gsaf_set3dtextbaseline (v1[0],v1[1],v1[2],v2[0],v2[1],v2[2]);
		} else if (idrc == 0) {
			gsaf_set3dtextbaseline (0.0,0.0,0.0,0.0,0.0,0.0);
		} else if (idrc == 7) {
			gsaf_set3dtextbaseline (v1[0],v1[1],v1[2],0.0,0.0,1.0);
			dx1 = 0.0;
			dz1 = -0.5 * chh/zl*(zmax-zmin);
		}
		for (i=ns; i<=ne; i++) {
			gsaf_gnumbr3d (tic[findex2(i-ns,0,ntics)]+dx1,
				       tic[findex2(i-ns,1,ntics)]+dy1,
				       tic[findex2(i-ns,2,ntics)]+dz1, 
				       tic[findex2(i-ns,3,ntics)], ind, ijust);
		}
	} else if (iz == 1) {
		if ((ipos1-2)%3 == 0) {
			idrc=0;
		} else {
			idrc=7;
		}
		ijust = 3;
		dx1 = 0.0;
		dy1 = 0.0;
		dz1 = 0.0;
		dz01 = -2.0*chh/zl*(zmax-zmin);
		dz02 = 1.0*chh/zl*(zmax-zmin);
		if (ipos2 <= 1) {  /* position 1, 2 */
			if (viewfact == 1.0) {
				dz1 = dz01;
			} else {
				dz1 = dz02;
			}
		} else {           /* position 3, 4 */
			if (viewfact == 1.0) {
				dz1 = dz02;
			} else {
				dz1 = dz01;
			}               
		}
		if (idrc == 7) {
			gsaf_set3dtextbaseline(v1[0],v1[1],v1[2],0.0,0.0,1.0);
		} else if (idrc == 0) {
			gsaf_set3dtextbaseline(0.0,0.0,0.0,0.0,0.0,0.0);
		}
		for (i=ns; i<=ne; i++) {
			gsaf_gnumbr3d (tic[findex2(i-ns,0,ntics)]+dx1,
				       tic[findex2(i-ns,1,ntics)]+dy1, 
				       tic[findex2(i-ns,2,ntics)]+dz1, 
				       tic[findex2(i-ns,3,ntics)], ind, ijust);
		}
	}
}

static void gtyval (float yorg, float dpy, float xpos, float zpos, float scal,
		    int ind, int ipos, int iz, gsglPage *p)
{
	float xl,yl,zl;
	float *tic;
	float p1[3], p2[3], p3[3], p4[3];
	float v1[3], v2[3];
	float xw1, yw1, zw1;
	float xw2, yw2, zw2;
	float xw3, yw3, zw3;
	float xw4, yw4, zw4;
	float chh,chw,chsp,angl,tilt;
	float gxs, gxe, gys, gye, zmin, zmax;
	float dx1, dy1, dz1;
	int ipos1, ipos2;
	int ns, ne;
	float viewfact;
	float vlength;
	float dz01, dz02;
	int i;
	int ijust;
	int idrc;
	int ntics;
	const float eps = 0.01f;
	gsglScale3D *info = p->scale3d;

	gsgl_inq_region_3d (&gxs, &gxe, &gys, &gye, &zmin, &zmax, p);

	if (yorg < gys || yorg > gye) return;
	if (fabs(dpy/(gye-gys)) < eps) return;

	gsgl_inq_length_3d (&xl, &yl, &zl, p);

	ipos1 = ipos % 10;
	ipos2 = (int) (0.1*ipos+eps);

	tic = gsgl_set_tics (2, yorg, dpy, scal, iz, info->Iposy%10, xpos, zpos, &ns, &ne, p);
	if (tic == NULL) return;
	ntics = ne - ns + 1;

	gsgl_viewfact (zpos, p1, p2, p3, p4, &viewfact, p);
	gsglproject3d (p1[0], p1[1], p1[2], &xw1, &yw1, &zw1, p);
	gsglproject3d (p2[0], p2[1], p2[2], &xw2, &yw2, &zw2, p);
	gsglproject3d (p3[0], p3[1], p3[2], &xw3, &yw3, &zw3, p);
	gsglproject3d (p4[0], p4[1], p4[2], &xw4, &yw4, &zw4, p);

	gsaf_inqchr(&chh,&chw,&chsp,&angl,&tilt);

	if (xw2 >= xw1) {
		v1[0] = p2[0] - p1[0];
		v1[1] = p2[1] - p1[1];
		v1[2] = p2[2] - p1[2];
	} else {
		v1[0] = -p2[0] + p1[0];
		v1[1] = -p2[1] + p1[1];
		v1[2] = -p2[2] + p1[2];       
	}
	vlength = sqrt(v1[0]*v1[0]+v1[1]*v1[1]+v1[2]*v1[2]);
	v1[0] = v1[0]/vlength;
	v1[1] = v1[1]/vlength;
	v1[2] = v1[2]/vlength;

	if (yw3 >= yw1) {
		v2[0] = p3[0] - p1[0];
		v2[1] = p3[1] - p1[1];
		v2[2] = p3[2] - p1[2];
	} else {
		v2[0] = -p3[0] + p1[0];
		v2[1] = -p3[1] + p1[1];
		v2[2] = -p3[2] + p1[2];
	}
	vlength = sqrt(v2[0]*v2[0]+v2[1]*v2[1]+v2[2]*v2[2]);
	v2[0] = v2[0]/vlength;
	v2[1] = v2[1]/vlength;
	v2[2] = v2[2]/vlength;

	if (iz == 0) {
		if (ipos1%3 == 2) {
			idrc=0;
		} else if (ipos1%3 == 0) {
			idrc=7;
		} else {
			idrc=3;
		}
		if (p1[1] != p2[1]) { /* p1-p2 が y軸 */
			gsgl_p1_p2_is_axis (2,ipos2,viewfact,p1,&dy1,&dx1,&dz1,p);
			ijust = 3;
		} else {              /* p1-p3 が y軸 */
			gsgl_p1_p3_is_axis (2,ipos2,p1,v2,xw1,xw2,&ijust,&dy1,&dx1,&dz1,p);
		}
		if (idrc == 3) {
			gsaf_set3dtextbaseline(v1[0],v1[1],v1[2],v2[0],v2[1],v2[2]);
		} else if (idrc == 0) {
			gsaf_set3dtextbaseline(0.0,0.0,0.0,0.0,0.0,0.0);
		} else if (idrc == 7) {
			gsaf_set3dtextbaseline(v1[0],v1[1],v1[2],0.0,0.0,1.0);
			dx1 = 0.0;
			dz1 = -0.5 * chh/zl*(zmax-zmin);
		}
		for (i=ns; i<=ne; i++) {
			gsaf_gnumbr3d (tic[findex2(i-ns,0,ntics)]+dx1, 
				       tic[findex2(i-ns,1,ntics)]+dy1,
				       tic[findex2(i-ns,2,ntics)]+dz1,
				       tic[findex2(i-ns,3,ntics)], ind, ijust);
		}
	} else if (iz == 1) {
		if (ipos1-2%3 == 0) {
			idrc=0;
		} else {
			idrc=7;
		}
		ijust = 3;
		dx1 = 0.0;
		dy1 = 0.0;
		dz1 = 0.0;
		dz01 = -2.0*chh/zl*(zmax-zmin);
		dz02 = 1.0*chh/zl*(zmax-zmin);
		if (ipos2 <= 1) {  /* position 1, 2 */
			if (viewfact == 1.0) {
				dz1 = dz01;
			} else {
				dz1 = dz02;
			}
		} else {           /* position 3, 4 */
			if (viewfact == 1.0) {
				dz1 = dz02;
			} else {
				dz1 = dz01;
			}               
		}
		if (idrc == 7) {
			gsaf_set3dtextbaseline(v1[0],v1[1],v1[2],0.0,0.0,1.0);
		} else if (idrc == 0) {
			gsaf_set3dtextbaseline(0.0,0.0,0.0,0.0,0.0,0.0);
		}
		for (i=ns; i<=ne; i++) {
			gsaf_gnumbr3d (tic[findex2(i-ns,0,ntics)]+dx1,
				       tic[findex2(i-ns,1,ntics)]+dy1,
				       tic[findex2(i-ns,2,ntics)]+dz1, 
				       tic[findex2(i-ns,3,ntics)], ind, ijust);
		}
	}
}

static void gsgl_set_ijust_for_z (int *ijust, float xw1, float xw2, int ipos2)
{
	if (ipos2 == 0 || ipos2 == 3) {     /* position 1,4 */
		if (xw2 >= xw1) {
			*ijust = 0;
		} else {
			*ijust = 1;
		}
	} else if (ipos2 == 1 || ipos2 == 2) { /* position 2,3 */
		if (xw2 >= xw1) {
			*ijust = 1;
		} else {
			*ijust = 0;
		}
	}
}

static void gsgl_set_dxyz_for_z (float *p1, float *p2, float *p3,
				 int ipos2, float *dx1, float *dy1, float *dz1, gsglPage *p)
{
	float xl,yl,zl;
	float chh,chw,chsp,angl,tilt;
	float px, py;
	float gxs, gxe, gys, gye, zmin, zmax;
	float f1, f2;
	const float eps = 0.01f;
	
	gsgl_inq_length_3d (&xl, &yl, &zl, p);
	gsgl_inq_region_3d (&gxs, &gxe, &gys, &gye, &zmin, &zmax, p);
	gsaf_inqchr (&chh, &chw, &chsp, &angl, &tilt);

	*dz1 = -0.5 * chh/zl*(zmax-zmin);
	if (ipos2 == 0 || ipos2 == 2) {
		px = p2[0];
		py = p2[1];
	} else if (ipos2 == 1 || ipos2 == 3) {
		px = p3[0];
		py = p3[1];
	} else {
		return;
	}
	if (ipos2 == 0 || ipos2 == 1) {
		f1 = -1.0;
		f2 = 1.0;
	} else if (ipos2 == 2 || ipos2 == 3) {
		f1 = 1.0;
		f2 = -1.0;
	} else {
		return;
	}
	if (fabs(p1[0]-p2[0])/(gxe-gxs) > eps) {  /* p1-p2 is x-axis */
		*dy1 = 0.0;
		if (fabs(px-gxs)/(gxe-gxs) < eps) {
			*dx1 = f1 * chw /xl * (gxe-gxs);
		} else {
			*dx1 = f2 * chw /xl * (gxe-gxs);
		}
	} else {                      /* p1-p2 is y-axis */
		*dx1 = 0.0;
		if (fabs(py-gys)/(gye-gys) < eps) {
			*dy1 = f1 * chw /yl * (gye-gys);
		} else {
			*dy1 = f2 * chw /yl * (gxe-gys);
		}
	}
}

static void gtzval (float zorg, float dpz, float xpos, float ypos, int nec, float scal,
		    int ind, int ipos, gsglPage *p)
{
	float *tic;
	float p1[3], p2[3], p3[3], p4[3];
	float v1[3];
	const float eps = 0.01f;
	float xw1, yw1, zw1;
	float xw2, yw2, zw2;
	float xw3, yw3, zw3;
	float xw4, yw4, zw4;
	float gxs, gxe, gys, gye, zmin, zmax;
	float dx1, dy1, dz1;
	int ipos1, ipos2;
	int ns, ne;
	int idrc;
	float vlength;
	int ijust;
	int i;
	float viewfact;
	int ntics;
	gsglScale3D *info = p->scale3d;

	gsgl_inq_region_3d (&gxs, &gxe, &gys, &gye, &zmin, &zmax, p);

	if (zorg < zmin || zorg > zmax) return;
	if (fabs(dpz/(zmax-zmin)) < eps) return;

	ipos1 = ipos % 10;
	ipos2 = (int) (0.1*ipos+eps);

	tic = gsgl_set_tics (3, zorg, dpz, scal, nec, info->Iposz%10, xpos, ypos, &ns, &ne, p);
	if (tic == NULL) return;
	ntics = ne - ns + 1;

	gsgl_viewfact (zmin, p1, p2, p3, p4, &viewfact, p);
	gsglproject3d (p1[0], p1[1], p1[2], &xw1, &yw1, &zw1, p);
	gsglproject3d (p2[0], p2[1], p2[2], &xw2, &yw2, &zw2, p);
	gsglproject3d (p3[0], p3[1], p3[2], &xw3, &yw3, &zw3, p);
	gsglproject3d (p4[0], p4[1], p4[2], &xw4, &yw4, &zw4, p);

	if (ipos1%3 == 2) { /* 2 */
		idrc=0;
	} else {            /* 1 */
		idrc=7;
	}
	if (xw2 >= xw1) {
		v1[0] = p2[0] - p1[0];
		v1[1] = p2[1] - p1[1];
		v1[2] = p2[2] - p1[2];
	} else {
		v1[0] = -p2[0] + p1[0];
		v1[1] = -p2[1] + p1[1];
		v1[2] = -p2[2] + p1[2];       
	}
	vlength = sqrt(v1[0]*v1[0]+v1[1]*v1[1]+v1[2]*v1[2]);
	v1[0] = v1[0]/vlength;
	v1[1] = v1[1]/vlength;
	v1[2] = v1[2]/vlength;

	gsgl_set_ijust_for_z (&ijust, xw1, xw2, ipos2);
	gsgl_set_dxyz_for_z (p1, p2, p3, ipos2, &dx1, &dy1, &dz1, p);

	if (idrc == 7) {
		gsaf_set3dtextbaseline(v1[0],v1[1],v1[2],0.0,0.0,1.0);
	} else if (idrc == 0) {
		gsaf_set3dtextbaseline(0.0,0.0,0.0,0.0,0.0,0.0);
	}
	for (i=ns; i<=ne; i++) {
		gsaf_gnumbr3d (tic[findex2(i-ns,0,ntics)]+dx1,
			       tic[findex2(i-ns,1,ntics)]+dy1,
			       tic[findex2(i-ns,2,ntics)]+dz1, 
			       tic[findex2(i-ns,3,ntics)], ind, ijust);
	}
}

/*     pn (n=1,..4) に n 番目に近い点を代入する．
 *     ただし，真上から見た場合は2次元表示の慣例に従って
 *    画面左下をp1とする．
 */
static void gsgl_set4points (float zpos, float *pnearz, float *p1, float *p2, float *p3, float *p4, gsglPage *p)
{
	float xa[3],ya[3],za[3];
	float xw1, yw1, zw1;
	float xw2, yw2, zw2;
	float xw3, yw3, zw3;
	float xc[8], yc[8], zc[8];
	float t[4][3], tl[4], xw[4], yw[4], zw[4];
	int n;
	float gxs, gxe, gys, gye, zmin, zmax;
	float angle12, angle13;
	float pi, deg;
	int imin;
	float tmin;
	int i, isec;
	const float eps =1.e-5f;

	gsgl_inq_axis_3d (xa, ya, za, p);
	gsgl_inq_region_3d (&gxs, &gxe, &gys, &gye, &zmin, &zmax, p);

	p1[2] = zpos;
	p2[2] = zpos;
	p3[2] = zpos;
	p4[2] = zpos;
	inqcorner3d (xc, yc, zc, p); /* get 8 corners */
	gsglinqnearestpoint (xc, yc, zc, 8, &n, p); /* nearest point */
	*pnearz = zc[n];
	p1[0] = xc[n];
	p1[1] = yc[n];

	pi = atan(1.0)*4.0;
	deg = 180.0/pi;
	if (acos(za[2])*deg < 5.0) {
		t[0][0] = gxs;
		t[0][1] = gys;
		t[1][0] = gxe;
		t[1][1] = gys;
		t[2][0] = gxe;
		t[2][1] = gye;
		t[3][0] = gxs;
		t[3][1] = gye;
		gsglproject3d (t[0][0], t[0][1], zpos, &xw[0], &yw[0], &zw[0], p);
		gsglproject3d (t[1][0], t[1][1], zpos, &xw[1], &yw[1], &zw[1], p);  
		gsglproject3d (t[2][0], t[2][1], zpos, &xw[2], &yw[2], &zw[2], p); 
		gsglproject3d (t[3][0], t[3][1], zpos, &xw[3], &yw[3], &zw[3], p);
		tl[0] = sqrt(xw[0]*xw[0]+yw[0]*yw[0]);
		tl[1] = sqrt(xw[1]*xw[1]+yw[1]*yw[1]);
		tl[2] = sqrt(xw[2]*xw[2]+yw[2]*yw[2]);
		tl[3] = sqrt(xw[3]*xw[3]+yw[3]*yw[3]);
		tmin = tl[0];
		imin = 0;
		for (i=1; i<4; i++) {
			if (tl[i] < tmin) {
				tmin = tl[i];
				imin = i;
			}
		}
		if (imin == 0) {
			tmin = tl[1];
			isec = 1;
		} else {
			tmin = tl[0];
			isec = 0;
		}
		for (i=1; i<4; i++) {
			if (i != imin && tl[i] < tmin) {
				tmin = tl[i];
				isec = i;
			}
		}
		if (fabs(tl[imin]-tl[isec]) < eps) {
			if (yw[isec] < yw[imin]) {
				imin = isec;
			}
		}
		p1[0] = t[imin][0];
		p1[1] = t[imin][1];
	}

	if (acos(za[2])*deg < 5.0) {
		p4[0] = gxe+gxs-p1[0];
		p4[1] = gye+gys-p1[1];
		
		p2[0] = gxe+gxs-p1[0];
		p2[1] = p1[1];
		
		p3[0] = p1[0];
		p3[1] = gye+gys-p1[1];
		
		gsglproject3d (p1[0], p1[1], p1[2], &xw1, &yw1, &zw1, p);
		gsglproject3d (p2[0], p2[1], p2[2], &xw2, &yw2, &zw2, p);
		gsglproject3d (p3[0], p3[1], p3[2], &xw3, &yw3, &zw3, p);
		
		if (fabs(xw1-xw2) < eps) {
			p2[0] = p1[0];
			p2[1] = gye+gys-p1[1];
			
			p3[0] = gxe+gxs-p1[0];
			p3[1] = p1[1];
			
			return;
		} else if (fabs(xw1-xw3) < eps) {
			return;
		} else {
			angle12 = fabs(atan ((yw2-yw1)/(xw2-xw1)));
			angle13 = fabs(atan ((yw3-yw1)/(xw3-xw1)));
			if (angle12 <= angle13) {
				return;
			} else {
				p2[0] = p1[0];
				p2[1] = gye+gys-p1[1];
				
				p3[0] = gxe+gxs-p1[0];
				p3[1] = p1[1];
				
				return;
			}
		}
	} else {
		if (fabs(xc[n]-gxs)/(gxe-gxs) < eps) {
			xc[0] = gxs;
			xc[1] = gxe;
			p4[0] = gxe;
			if (fabs(yc[n]-gys)/(gye-gys) < eps) {
				yc[0] = gye;
				yc[1] = gys;
				p4[1] = gye;
			} else {
				yc[1] = gye;
				yc[0] = gys;
				p4[1] = gys;
			}
		} else {
			xc[0] = gxe;
			xc[1] = gxs;
			p4[0] = gxs;
			if (fabs(yc[n]-gys)/(gye-gys) < eps) {
				yc[0] = gye;
				yc[1] = gys;
				p4[1] = gye;         
			} else {
				yc[1] = gye;
				yc[0] = gys;
				p4[1] = gys;
			}
		}
		gsglinqnearestpoint (xc, yc, zc, 2, &n, p);
		p2[0] = xc[n];
		p2[1] = yc[n];
		if (n == 0) {
			p3[0] = xc[1];
			p3[1] = yc[1];
		} else {
			p3[0] = xc[0];
			p3[1] = yc[0];
		}
	}
}

/* inquire x-axis's position
 * ipos  : position 1 or 2 or 3 or 4
 */
static void gsgl_inqxaxispos (int ipos, float *axis_y, float *axis_z, gsglPage *p)
{
	float xmin,xmax,ymin,ymax,zmin,zmax;
	float x[8], y[8], z[8];
	float p1[3],p2[3],p3[3],p4[3];
	int n;
	float pnearz;

	gsgl_inq_region_3d (&xmin, &xmax, &ymin, &ymax, &zmin, &zmax, p);
	inqcorner3d (x, y, z, p);
        /* --- 8つの角のうち一番近い点を求める --- */
	gsglinqnearestpoint (x, y, z, 8, &n, p);
        /* z-axis */
	if (ipos == 3 || ipos == 4) *axis_z = z[n];
	if (ipos == 1 || ipos == 2) {
		*axis_z = zmax + zmin - z[n];
	}
        /* y-axis */
	gsgl_set4points (*axis_z, &pnearz, p1, p2, p3, p4, p);
	if (ipos == 1 || ipos == 3) *axis_y = p1[1];
	if (ipos == 2 || ipos == 4) {
		*axis_y = ymax + ymin - p1[1];
	}
}

/*     inquire y-axis's position
 *     ipos  : position 1 or 2 or 3 or 4
 */
static void gsgl_inqyaxispos (int ipos, float *axis_x, float *axis_z, gsglPage *p)
{
	float xmin,xmax,ymin,ymax,zmin,zmax;
	float x[8], y[8], z[8];
	float p1[3], p2[3], p3[3], p4[3];
	int n;
	float pnearz;

	gsgl_inq_region_3d (&xmin, &xmax, &ymin, &ymax, &zmin, &zmax, p);
	inqcorner3d (x, y, z, p);
/* --- 8つの角のうち一番近い点を求める --- */
	gsglinqnearestpoint (x, y, z, 8, &n, p);
/* --- axis_z --- */
	if (ipos == 3 || ipos == 4) *axis_z = z[n];
	if (ipos == 1 || ipos == 2) {
		*axis_z = zmax + zmin - z[n];
	}
/* --- axis_x --- */
	gsgl_set4points (*axis_z, &pnearz, p1, p2, p3, p4, p);
	if (ipos == 1 || ipos == 3) *axis_x = p1[0];
	if (ipos == 2 || ipos == 4) {
		*axis_x = xmax + xmin - p1[0];
	}
}

/*     inquire z-axis's position
 *
 *     ipos  : position 1 or 2 or 3 or 4
 *     nec   : 目盛りを普通に(iposが0でない)描く場合，
 *             その目盛りがx軸と平行ならば0，y軸と平行ならば1となる．
 */
static void gsgl_inqzaxispos (int ipos, float *axis_x, float *axis_y, int *nec, gsglPage *p)
{
	float xmin,xmax,ymin,ymax,zmin,zmax;
	float p1[3], p2[3], p3[3], p4[3];
	float dx, dy;
	float pnearz;

	gsgl_inq_region_3d (&xmin, &xmax, &ymin, &ymax, &zmin, &zmax, p);
	gsgl_set4points (zmin, &pnearz, p1, p2, p3, p4, p);
	
	if (ipos == 1) {
		*axis_x = p2[0];
		*axis_y = p2[1];
	} else if (ipos == 2) {
		*axis_x = p3[0];
		*axis_y = p3[1];
	} else if (ipos == 3) {
		*axis_x = p1[0];
		*axis_y = p1[1];
	} else if (ipos == 4) {
		*axis_x = p4[0];
		*axis_y = p4[1];
	}
	dx = p2[0] - p1[0];
	dy = p2[1] - p1[1];
	if (fabs(dx) < 0.1*(xmax-xmin)) *nec=1;
	if (fabs(dy) < 0.1*(ymax-ymin)) *nec=0;
	/*else gsgl_warning ("gsgl_inqzaxispos", "");*/
}
