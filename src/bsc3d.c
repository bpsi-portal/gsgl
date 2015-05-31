#include "gsaf.h"
#include "gsgl.h"
#include "base.h"
#include "bsc1.h"
#include "bsc2.h"
#include "bsc3d.h"
#include "utils.h"
#include "matrix.h"
#include "apl1.h"
#include "text.h"

#define AMOD(x,y) (x - y * (int) (x/y))

void gsgl_inq_region_3d (float *xmin, float *xmax, float *ymin, float *ymax,
			 float *zmin, float *zmax, gsglPage *p)
{
	gsgl3DInfo *info;
	info = p->ThreeDInfo;

	*xmin = info->Gxmin;
	*xmax = info->Gxmax;
	*ymin = info->Gymin;
	*ymax = info->Gymax;
	*zmin = info->Gzmin;
	*zmax = info->Gzmax;
}

void gsgl_inq_axis_3d (float *xa, float *ya, float *za, gsglPage *p)
{
	int i;
	gsgl3DInfo *info;
	info = p->ThreeDInfo;

	for (i=0; i<3; i++) {
		xa[i] = info->Xa[i];
		ya[i] = info->Ya[i];
		za[i] = info->Za[i];
	}
}

void gsgl_inq_length_3d (float *xl, float *yl, float *zl, gsglPage *p)
{
	gsgl3DInfo *info;
	info = p->ThreeDInfo;

	*xl = info->Xl;
	*yl = info->Yl;
	*zl = info->Zl;
}

void gsgl_init_text3d_baseline (gsglPage *p)
{
	gsgl3DInfo *info;
	info = p->ThreeDInfo;

	info->V1x = 0.0;
	info->V1y = 0.0;
	info->V1z = 0.0;
	info->V2x = 0.0;
	info->V2y = 0.0;
	info->V2z = 0.0;
}

gsglboolean gsgl_inq_text3d_normal (gsglPage *p)
{
	gsglboolean normal;
	const float eps = 1.e-5f;
	gsgl3DInfo *info;
	info = p->ThreeDInfo;

	normal = GSGL_FALSE;
	/* --- baselline か normal vector の長さが 0 ならば視線に垂直に配置する --- */
	if ((fabs(info->V1x) < eps && fabs(info->V1y) < eps && fabs(info->V1z) < eps) ||
	    (fabs(info->V2x) < eps && fabs(info->V2y) < eps && fabs(info->V2z) < eps)) {
		normal = GSGL_TRUE;
	/* --- baselline と normal vector が直行していないならば視線に垂直に配置する --- */
	} else if (fabs(info->V1x*info->V2x+info->V1y*info->V2y+info->V1z*info->V2z)
		   /sqrt(info->V1x*info->V1x+info->V1y*info->V1y+info->V1z*info->V1z)/sqrt(info->V2x*info->V2x+info->V2y*info->V2y+info->V2z*info->V2z) > eps) {
		normal = GSGL_TRUE;
	}
	return normal;
}

/* 
 * world座標系をユーザ指定の座標系に 
 * 回転するための角度，回転軸を返す  
 * 
 *  --- 入力 ---
 *  x軸に対応するベクトル v1x,v1y,v1z
 *  y軸に対応するベクトル v2x,v2y,v2z
 *  --- 出力 ---
 *  一つめの回転の回転軸 vn1[3]
 *  一つめの回転の回転角 angle1 (degree)
 *  二つめの回転の回転軸 vn2[3]
 *  二つめの回転の回転角 angle2 (degree)
 */
void gsgl_rotatoworldtouserdefined (float v1x_in, float v1y_in, float v1z_in,
				    float v2x_in, float v2y_in, float v2z_in,
				    float *vn1, float *angle1, float *vn2, float *angle2)
{
	const float eps = 1.e-5f;
	float pi = atan(1.0)*4.0;
	float v1l, v1x, v1y, v1z;
	float v2l, v2x, v2y, v2z;
	float v5l, v5x, v5y, v5z;
	float vn1l;
	float vn2l;
	float ar1;
	float s1, s2, s3;
	float prod;

        /* --- 正規化 --- */
	v1l=sqrt(v1x_in*v1x_in + v1y_in*v1y_in + v1z_in*v1z_in);
	v1x = v1x_in/v1l;
	v1y = v1y_in/v1l;
	v1z = v1z_in/v1l;
	v2l=sqrt(v2x_in*v2x_in + v2y_in*v2y_in + v2z_in*v2z_in);
	v2x = v2x_in/v2l;
	v2y = v2y_in/v2l;
	v2z = v2z_in/v2l;
	/* --- 1つめの回転 --- */
	/* --- v1 と x軸とを一致させるため，v1とx軸に垂直なベクトルvn1を軸として回転する --- */
	/* --- vn1 = \hat{x}\times v1 --- */
	vn1[0] = 0.0;
	vn1[1] = -v1z;
	vn1[2] = v1y;
	vn1l = sqrt(vn1[0]*vn1[0]+vn1[1]*vn1[1]+vn1[2]*vn1[2]);
	if (fabs(vn1l) > eps) {
		vn1[0] = vn1[0]/vn1l;
		vn1[1] = vn1[1]/vn1l;
		vn1[2] = vn1[2]/vn1l;
	} else {
		/* --- 法線ベクトルの長さが0 => v1 は x軸に平行 => z軸周りで回転 --- */
		vn1[0] = 0.0;
		vn1[1] = 0.0;
		vn1[2] = v1x;
	}

	ar1 = acos(v1x);             /* radian */
	*angle1 = acos(v1x)/pi*180.0; /* degree */
	/* --- 2つめの回転 --- */
	/* --- 1つめの回転で y 軸がどう射影されたかを求める --- */
	/* --- redbook (gl1.1) 2nd edition (日本語訳) p596 --- */
	s1 = -vn1[2];
	s2 = 0.0;
	s3 = vn1[0];
	v5x = vn1[0]*vn1[1] + cos(ar1)*(   -vn1[0]*vn1[1]) + sin(ar1) * s1;
	v5y = vn1[1]*vn1[1] + cos(ar1)*(1.0-vn1[1]*vn1[1]) + sin(ar1) * s2;
	v5z = vn1[2]*vn1[1] + cos(ar1)*(   -vn1[2]*vn1[1]) + sin(ar1) * s3;
	v5l = sqrt(v5x*v5x+v5y*v5y+v5z*v5z);
	if (fabs(v5l) > eps) {
		v5x = v5x/v5l;
		v5y = v5y/v5l;
		v5z = v5z/v5l;
	} else {
		gsgl_warning ("", "回転後のy方向単位ベクトルの長さが0");
	}
	
	prod = v5x*v2x+v5y*v2y+v5z*v2z;
	if (prod > 1.0) prod = 1.0;
	if (prod < -1.0) prod = -1.0;
	/* --- vn2 = v5 x v2 --- */
	vn2[0] = v5y*v2z - v5z*v2y;
	vn2[1] = v5z*v2x - v5x*v2z;
	vn2[2] = v5x*v2y - v5y*v2x;
	vn2l = sqrt(vn2[0]*vn2[0]+vn2[1]*vn2[1]+vn2[2]*vn2[2]);
	if (vn2l > eps) {
		vn2[0] = vn2[0]/vn2l;
		vn2[1] = vn2[1]/vn2l;
		vn2[2] = vn2[2]/vn2l;
	} else {
		/* --- 法線ベクトルとy軸の内積が0 --- */
		vn2[0] = v1x;
		vn2[1] = v1y;
		vn2[2] = v1z;
	}
	*angle2 = acos(prod)/pi*180.0;    /* degree */	
}

/* gdefin for 3d plot */
/* --- ここでは common ブロックにいれているだけ --- */
void gsaf_gdefin3d (float xl1, float yl1, float zl1, float gxmin1, float gxmax1,
		    float gymin1, float gymax1, float gzmin1, float gzmax1)
{
	float vx[3],vy[3],vz[3];
	float ox, oy, oz;
	gsglPage *p;
	gsgl3DInfo *info;

	if (!gsgl_inq_page_is_open ()) return;
	p = gsgl_inq_page ();
	info = p->ThreeDInfo;

	info->Xl = xl1;
	info->Yl = yl1;
	info->Zl = zl1;
	info->Gxmin = gxmin1;
	info->Gxmax = gxmax1;
	info->Gymin = gymin1;
	info->Gymax = gymax1;
	info->Gzmin = gzmin1;
	info->Gzmax = gzmax1;
	/* --- パラメータの初期化 --- */
	gsgl_init_text3d_baseline (p);
	gsgl_init_hold_text_position (p);
	/* --- 3d座標系の初期化 --- */
	ox = 0.5*(info->Gxmax+info->Gxmin);
	oy = 0.5*(info->Gymax+info->Gymin);
	oz = 0.5*(info->Gzmax+info->Gzmin);
	vx[0] = 1.0;
	vx[1] = 0.0;
	vx[2] = 0.0;
	vy[0] = 0.0;
	vy[1] = 1.0;
	vy[2] = 0.0;
	vz[0] = 0.0;
	vz[1] = 0.0;
	vz[2] = 1.0;
	gsaf_setprojectiontype (1); /* 1 = frustum */
	gsaf_set3dcoord_caut1 (ox, oy, oz, 25.0, vx, vy, vz);
}

void gsaf_setprojectiontype (int iproj)
{
	if (!gsgl_inq_page_is_open ()) return;

	gsgl_projectiontype (iproj, gsgl_inq_page());
}

void gsaf_set3dcoord_caut1 (float ox1, float oy1, float oz1, float depth, 
			    float *vx, float *vy, float *vz)
{
	if (!gsgl_inq_page_is_open ()) return;

	gsaf_set3dcoord_caut2 (GSGL_PAPER_WIDTH*0.5, GSGL_PAPER_HEIGHT*0.5,
			       depth,ox1,oy1,oz1,vx,vy,vz);
}

void gsaf_set3dcoord_caut2 (float wx, float wy, float depth, float x, float y, float z,
			    float *vx, float *vy, float *vz)
{
	float vn1[3], vn2[3];
	int error;
	float vl, vlength;
	float dot;
	int i;
	float angle1, angle2;
	const float eps = 1.e-5f;
	gsglPage *p;
	gsgl3DInfo *info;

	if (!gsgl_inq_page_is_open ()) return;
	p = gsgl_inq_page ();
	info = p->ThreeDInfo;
	
	/* --- depth is needed when rotate or scale --- */
	gsgl_depth3d (depth, p);
	/* --- 各ベクトルが直行しているか --- */
	error = 0;
	dot = vx[0]*vy[0]+vx[1]*vy[1]+vx[2]*vy[2];
	if (fabs(dot) > eps) {
		error = 1;
	}
	dot = vz[0]*vy[0]+vz[1]*vy[1]+vz[2]*vy[2];
	if (fabs(dot) > eps) {
		error = 1;
	}
	dot = vz[0]*vx[0]+vz[1]*vx[1]+vz[2]*vx[2];
	if (fabs(dot) > eps) {
		error = 1;
	}
	if (error) {
		gsgl_warning ("set3dcoord_caut2", "vx, vy and yz  are not perpendicular");
		vx[0] = 1.0;
		vx[1] = 0.0;
		vx[2] = 0.0;
		vy[0] = 0.0;
		vy[1] = 1.0;
		vy[2] = 0.0;
		vz[0] = 0.0;
		vz[1] = 0.0;
		vz[2] = 1.0;
	}
	/* --- 長さ0のベクトルを補完する --- */
	error = 0;
	vlength = vx[0]*vx[0]+vx[1]*vx[1]+vx[2]*vx[2];
	if (vlength < eps) error = 1;
	vlength = vy[0]*vy[0]+vy[1]*vy[1]+vy[2]*vy[2];
	if (vlength < eps) error += 1;
	vlength = vz[0]*vz[0]+vz[1]*vz[1]+vz[2]*vz[2];
	if (vlength < eps) error += 1;
	if (error >=2) {
		gsgl_warning ("set3dcoord_caut2", "too many vector's length are zero");
		vx[0] = 1.0;
		vx[1] = 0.0;
		vx[2] = 0.0;
		vy[0] = 0.0;
		vy[1] = 1.0;
		vy[2] = 0.0;
		vz[0] = 0.0;
		vz[1] = 0.0;
		vz[2] = 1.0;
	}

	if (error == 1) {
		vlength = vx[0]*vx[0]+vx[1]*vx[1]+vx[2]*vx[2];
		if (vlength < eps) {
			vx[0]=vy[1]*vz[2]-vy[2]*vz[1];
			vx[1]=vy[2]*vz[0]-vy[0]*vz[2];
			vx[2]=vy[0]*vz[1]-vy[1]*vz[0];
		}
		vlength = vy[0]*vy[0]+vy[1]*vy[1]+vy[2]*vy[2];
		if (vlength < eps) {
			vy[0]=vz[1]*vy[2]-vz[2]*vy[1];
			vy[1]=vz[2]*vy[0]-vz[0]*vy[2];
			vy[2]=vz[0]*vy[1]-vz[1]*vy[0];
		}
		vlength = vz[0]*vz[0]+vz[1]*vz[1]+vz[2]*vz[2];
		if (vlength < eps) {
			vz[0]=vx[1]*vy[2]-vx[2]*vy[1];
			vz[1]=vx[2]*vy[0]-vx[0]*vy[2];
			vz[2]=vx[0]*vy[1]-vx[1]*vy[0];
		}
	}
        /* --- 正規化 --- */
	vl = sqrt(vx[0]*vx[0]+vx[1]*vx[1]+vx[2]*vx[2]);
	vx[0] = vx[0]/vl;
	vx[1] = vx[1]/vl;
	vx[2] = vx[2]/vl;
	vl = sqrt(vy[0]*vy[0]+vy[1]*vy[1]+vy[2]*vy[2]);
	vy[0] = vy[0]/vl;
	vy[1] = vy[1]/vl;
	vy[2] = vy[2]/vl;
	vl = sqrt(vz[0]*vz[0]+vz[1]*vz[1]+vz[2]*vz[2]);
	vz[0] = vz[0]/vl;
	vz[1] = vz[1]/vl;
	vz[2] = vz[2]/vl;
	/* --- submit to common block --- */
	for (i=0; i<3; i++) {
		info->Xa[i] = vx[i];
		info->Ya[i] = vy[i];
		info->Za[i] = vz[i];
	}
	/* ---------------------------------------- */
	gsgl_rotatoworldtouserdefined (vx[0],vx[1],vx[2],vy[0],vy[1],vy[2],
				       vn1, &angle1, vn2, &angle2);
	/* ---------------------------------------- */
	gsgl_set_3d_coord (wx, wy, depth, info->Xl, info->Yl, info->Zl, 
			   info->Gxmax, info->Gxmin, info->Gymax, info->Gymin, 
			   info->Gzmax, info->Gzmin, x, y, z, angle1, vn1, angle2, vn2, p);
}

void gsaf_set3dcoord_polar1 (float ox1, float oy1, float oz1, 
			float radius1, float theta1, float phi1)
{
	if (!gsgl_inq_page_is_open ()) return;

	gsaf_set3dcoord_polar2 (GSGL_PAPER_WIDTH*0.5, GSGL_PAPER_HEIGHT*0.5,
				radius1, ox1, oy1, oz1, theta1, phi1);
}

void gsaf_set3dcoord_polar2 (float wx, float wy, float depth, float x, float y, float z,
			     float theta, float phi)
{
	float vx[3],vy[3],vz[3];
	float pi = atan (1.0) * 4.0;
	float degree;

	if (!gsgl_inq_page_is_open ()) return;

	gsgl_convert_angle (theta, phi);      

	degree = pi / 180.0;
	/* --- z軸ベクトル --- */
	vz[0] = 0.0;
	vz[1] = sin(theta * degree);
	vz[2] = cos(theta * degree);
	/* --- x軸ベクトル --- */
	vx[0] = -sin(phi * degree);
	vx[1] = -cos(theta * degree)*cos(phi * degree);
	vx[2] = sin(theta * degree)*cos(phi * degree);
	/* --- y軸ベクトル --- */
	vy[0] = cos(phi * degree);
	vy[1] = -cos(theta * degree)*sin(phi * degree);
	vy[2] = sin(theta * degree)*sin(phi * degree);

	gsaf_set3dcoord_caut2 (wx,wy,depth,x,y,z,vx,vy,vz);
}

void gsgl_convert_angle (float theta, float phi)
{
	/* phi   should be 0.0 <= phi < 360.0   */
        /* theta should be 0.0 <= theta < 180.0 */
	float ct1, ct2;

	if (theta >=180.0) {
		ct1 = 180.0 - AMOD(theta,180.0);
		ct2=AMOD(theta,360.0);
		theta=ct1;
		if (ct2 >=180.0) phi=phi+180.0;
	} else if (theta < 0.0) {
		theta=-theta;
		ct1=180.0-AMOD(theta,180.0);
		ct2=AMOD(theta,360.0);
		theta=ct1;
		if (ct2 >=180.0) phi=phi+180.0;
	}
	
	if (phi >=360.0) {
		phi=AMOD(phi,360.0);
	} else if (phi < 0.0) {
		phi=-phi;
		phi=360.0-AMOD(phi,360.0);
	}
}

void gsgl_seteyepos (float eye)
{
	if (!gsgl_inq_page_is_open ()) return;

	gsgl_eyeposition (eye, gsgl_inq_page());
}

/* set view-point for 3d plot (obsolute) */
void gsaf_gview3d (float phi1, float theta1, float radius1, float ox1, float oy1, float oz1)
{
	int iproj;
	float radius;

	if (!gsgl_inq_page_is_open ()) return;

	if (radius1 > 500.0) {
		iproj = 0;       /* 正射影 */
		radius = radius1 - 1000.0;
	} else {
		iproj = 1;       /* 透視法 */
		radius = radius1;
	}

	gsaf_setprojectiontype (iproj);
	gsaf_set3dcoord_polar1 (ox1,oy1,oz1,radius,theta1,phi1);
}

/* move in 3d space */
void gsaf_move3d (float x1, float y1, float z1)
{
	gsglPage *p;

	if (!gsgl_inq_page_is_open ()) return;
	p = gsgl_inq_page ();

	gsgl_move3d (x1, y1, z1, p);
}

/* inqpos in 3d space */
void gsaf_inqpos3d (float *x1, float *y1, float *z1)
{
	gsglPage *p;

	if (!gsgl_inq_page_is_open ()) return;
	p = gsgl_inq_page ();

	gsgl_inqpos3d (x1, y1, z1, p);
}

/* draw in 3d space */
void gsaf_draw3d (float x1, float  y1, float  z1)
{
	gsglPage *p;

	if (!gsgl_inq_page_is_open ()) return;
	p = gsgl_inq_page ();

	gsgl_draw3d (x1, y1, z1, p);
}

/* draw in 3d space */
void gsaf_draw3d_with_normal (float x1, float  y1, float  z1, float  vx, float  vy, float  vz)
{
	gsglPage *p;

	if (!gsgl_inq_page_is_open ()) return;
	p = gsgl_inq_page ();

	gsgl_draw3d_with_normal (x1, y1, z1, vx, vy, vz, p);
}

/* draw in 3d space */
void gsaf_draw3d_with_normals (float x1, float y1, float z1, float *vx, float *vy, float *vz)
{
	gsglPage *p;

	if (!gsgl_inq_page_is_open ()) return;
	p = gsgl_inq_page ();

	gsgl_draw3d_with_normals (x1, y1, z1, vx, vy, vz, p);
}

/* poly in 3d space */
void gsaf_poly3d (float *xn, float *yn, float *zn, int n)
{
	gsglPage *p;

	if (!gsgl_inq_page_is_open ()) return;
	p = gsgl_inq_page ();

	gsgl_polygon3d (xn, yn, zn, n, p);
}

/* poly in 3d space */
void gsaf_poly3d_with_normal (float *xn, float *yn, float *zn, 
			      float vx1, float vy1, float vz1, int n)
{
	gsglPage *p;

	if (!gsgl_inq_page_is_open ()) return;
	p = gsgl_inq_page ();

	gsgl_polygon3d_with_normal (xn, yn, zn, vx1, vy1, vz1, n, p);
}

/* poly in 3d space */
void gsaf_poly3d_with_normals (float *xn, float *yn, float *zn, float *vx, float *vy, float *vz,
			       int n)
{
	gsglPage *p;

	if (!gsgl_inq_page_is_open ()) return;
	p = gsgl_inq_page ();

	gsgl_polygon3d_with_normals (xn, yn, zn, vx, vy, vz, n, p);
}
 

/* rgbtrg in 3d space */
void gsaf_rgbtrg3d (float *xn, float *yn, float *zn, float *r, float *g, float *b)
{
	gsglPage *p;

	if (!gsgl_inq_page_is_open ()) return;
	p = gsgl_inq_page ();

	gsgl_rgbtriangle3d (xn, yn, zn, r, g, b, p);
	gsaf_setrgb (r[2],g[2],b[2]);
}

/* rgbtrg in 3d space */
void gsaf_rgbtrg3d_with_normal (float *xn, float *yn, float *zn, float *r, float *g, float *b, 
				float vx, float vy, float vz)
{
	gsglPage *p;

	if (!gsgl_inq_page_is_open ()) return;
	p = gsgl_inq_page ();

	gsgl_rgbtriangle3d_with_normal (xn, yn, zn, r, g, b, vx, vy, vz, p);
	gsaf_setrgb (r[2],g[2],b[2]);
}
 
/* rgbtrg in 3d space */
void gsaf_rgbtrg3d_with_normals (float *xn, float *yn, float *zn, float *r, float *g, float *b,
				 float *vx, float *vy, float *vz)
{
	gsglPage *p;

	if (!gsgl_inq_page_is_open ()) return;
	p = gsgl_inq_page ();

	gsgl_rgbtriangle3d_with_normals (xn, yn, zn, r, g, b, vx, vy, vz, p);
	gsaf_setrgb (r[2],g[2],b[2]);
}

/* line in 3d space */
void gsaf_line3d (float x1, float y1, float z1, float x2, float y2, float z2)
{
	if (!gsgl_inq_page_is_open ()) return;

	gsaf_move3d (x1, y1, z1);
	gsaf_draw3d (x2, y2, z2);
}

/* line in 3d space */
void gsaf_line3d_with_normal (float x1, float y1, float z1, float x2, float y2, float z2,
			      float vx, float vy, float vz)
{
	if (!gsgl_inq_page_is_open ()) return;

	gsaf_move3d (x1, y1, z1);
	gsaf_draw3d_with_normal (x2, y2, z2, vx, vy, vz);
}

/* line in 3d space */
void gsaf_line3d_with_normals (float x1, float y1, float z1, float x2, float y2, float z2, 
			       float vx[], float vy[], float vz[])
{
	if (!gsgl_inq_page_is_open ()) return;

	gsaf_move3d (x1, y1, z1);
	gsaf_draw3d_with_normals (x2, y2, z2, vx, vy, vz);
}

/* lines in 3d space */
void gsaf_lines3d (float *x, float *y, float *z, int n)
{
	gsglPage *p;

	if (!gsgl_inq_page_is_open ()) return;
	p = gsgl_inq_page ();

	gsgl_lines3d (x, y, z, n, p);
}

/* lines in 3d space */
void gsaf_lines3d_with_normal (float *x, float *y, float *z, float vx, float vy, float vz, int n)
{
	gsglPage *p;

	if (!gsgl_inq_page_is_open ()) return;
	p = gsgl_inq_page ();

	gsgl_lines3d_with_normal (x, y, z, vx, vy, vz, n, p);
}

/* lines in 3d space */
void gsaf_lines3d_with_normals (float *x, float *y, float *z, 
				float vx[], float vy[], float vz[], int n)
{
	gsglPage *p;

	if (!gsgl_inq_page_is_open ()) return;
	p = gsgl_inq_page ();

	gsgl_lines3d_with_normals (x, y, z, vx, vy, vz, n, p);
}

/* inquire 8 corners of the region defined by gdefine3d */
void inqcorner3d (float *x, float *y, float *z, gsglPage *p)
{
	gsgl3DInfo *info;
	info = p->ThreeDInfo;

	x[0] = info->Gxmin;
	y[0] = info->Gymin;
	z[0] = info->Gzmin;
	x[1] = info->Gxmax;
	y[1] = info->Gymin;
	z[1] = info->Gzmin;
	x[2] = info->Gxmin;
	y[2] = info->Gymax;
	z[2] = info->Gzmin;
	x[3] = info->Gxmax;
	y[3] = info->Gymax;
	z[3] = info->Gzmin;    
	x[4] = info->Gxmin;
	y[4] = info->Gymin;
	z[4] = info->Gzmax;
	x[5] = info->Gxmax;
	y[5] = info->Gymin;
	z[5] = info->Gzmax;
	x[6] = info->Gxmin;
	y[6] = info->Gymax;
	z[6] = info->Gzmax;
	x[7] = info->Gxmax;
	y[7] = info->Gymax;
	z[7] = info->Gzmax;
}

/* transform 3d data to world coordinate */
void gsaf_gttta (float x, float y, float z, float *w1, float *w2)
{
	float w3;
	gsglPage *p;

	if (!gsgl_inq_page_is_open ()) return;
	p = gsgl_inq_page ();

	gsglproject3d (x, y, z, w1, w2, &w3, p);
}

/* transform 3d data to 2d */
void gsaf_gtttb (float x, float y, float z, float *w1, float *w2)
{
	if (!gsgl_inq_page_is_open ()) return;

	gsaf_gttta (x, y, z, w1, w2);
}

/* draw text in 3d space */
void gsaf_text3d (char *ktext)
{
	float vn1[3], vn2[3], vn3[3], vn4[3];
	gsglboolean normal;
	float x3d, y3d, z3d;
	float xworld, yworld, zworld;
	float angle1, angle2, angle3, angle4;
	int ifnt;
	float ws;
	float x1, y1;
	gsglPage *p;
	gsgl3DInfo *info;

	if (!gsgl_inq_page_is_open ()) return;
	p = gsgl_inq_page();
	info = p->ThreeDInfo;

	normal = gsgl_inq_text3d_normal (p);
	gsaf_inqpos3d (&x3d, &y3d, &z3d);
	gsglproject3d (x3d, y3d, z3d, &xworld, &yworld, &zworld, p);
	
	if (!gsgl_inq_hold_text_position (gsgl_inq_page())) {
		/* gtext3d で設定した x1, y1 の値を使う */
		gsaf_move (0.0, 0.0);
	}
	
	gsgl_separate_objects (p);
	
	if (normal) {
		gsgl_set_text3d_matrix_normal (xworld, yworld, zworld, p);
	} else {
		gsgl_rotatoworldtouserdefined (info->Xa[0], info->Xa[1], info->Xa[2],
					       info->Ya[0], info->Ya[1], info->Ya[2],
					       vn1, &angle1, vn2, &angle2);
		gsgl_rotatoworldtouserdefined (info->V1x, info->V1y, info->V1z, 
					       info->V2x, info->V2y, info->V2z,
					       vn3, &angle3, vn4, &angle4);
		gsgl_set_text3d_matrix (xworld, yworld, zworld, 
					angle1, vn1, angle2, vn2, angle3, vn3, angle4, vn4, p);
	}
	
	gsaf_inqfnt (&ifnt);
	if(ifnt>=2 && ifnt <= 19) {
		gsaf_inqlnw (&ws);
		setlnw_text (p);
	}
	
	gsaf_inqpos (&x1, &y1);
	gsgltext (ktext, strlen(ktext), &x1, &y1, 3, p);
	
	if(ifnt>=2 && ifnt<=19) {
		gsaf_setlnw (ws);
	}
	gsglprojecttext3d (x1, y1, &xworld, &yworld, &zworld, p);
	gsglunproject3d (xworld, yworld, zworld, &x3d, &y3d, &z3d, p);
	gsaf_move3d (x3d, y3d, z3d);
	
	gsgl_separate_objects (p);
}

/*  3次元的に配置するテキストのbase lineとその法線を定義する  */
void gsaf_set3dtextbaseline (float x1, float y1, float z1, float x2, float y2, float z2)
{
	gsglPage *p;
	gsgl3DInfo *info;

	if (!gsgl_inq_page_is_open ()) return;
	p = gsgl_inq_page ();
	info = p->ThreeDInfo;

	info->V1x = x1;
	info->V1y = y1;
	info->V1z = z1;
	info->V2x = x2;
	info->V2y = y2;
	info->V2z = z2;
}
