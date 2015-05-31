#ifndef __bsc3d_h_
#define __bsc3d_h_

struct _gsgl3DInfo {
	gsglboolean   Now3D;
	gsglboolean   rotated;
	float depth;
	int   projection; /* 0 : Orthogonal *//* 1 : Frustum */
	gsglboolean  Lighting;
	float eyeposition;
	float Xl, Yl, Zl;
	float Xa[3], Ya[3], Za[3];
	float V1x, V1y, V1z, V2x, V2y, V2z;
	float Gxmax, Gxmin, Gymax, Gymin, Gzmax, Gzmin;
};

void gsgl_rotatoworldtouserdefined (float v1x_in, float v1y_in, float v1z_in,
				    float v2x_in, float v2y_in, float v2z_in,
				    float *vn1, float *angle1, float *vn2, float *angle2);
void gsgl_convert_angle (float theta, float phi);
void gsaf_gview3d (float phi1, float theta1, float radius1, float ox1, float oy1, float oz1);
void inqcorner3d (float *x, float *y, float *z, gsglPage *);
void gsaf_gttta (float x, float y, float z, float *w1, float *w2);
void gsaf_gtttb (float x, float y, float z, float *w1, float *w2);
void gsgl_init_text3d_baseline (gsglPage *);
gsglboolean gsgl_inq_text3d_normal (gsglPage *);
void gsgl_inq_region_3d (float *xmin, float *xmax, float *ymin, float *ymax, float *zmin, float *zmax, gsglPage *p);
void gsgl_inq_length_3d (float *xl, float *yl, float *zl, gsglPage *);
void gsgl_inq_axis_3d (float *xa, float *ya, float *za, gsglPage *);

#endif
