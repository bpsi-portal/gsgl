#ifndef __internal_h__
#define __internal_h__

#define   GSGLINTERNAL_BEGIN_HEAD     2
#define   GSGLINTERNAL_END_HEAD      -2
#define   GSGLINTERNAL_BEGIN_PAGE     3
#define   GSGLINTERNAL_END_PAGE      -3
#define   GSGLINTERNAL_BEGIN_ANIME    5
#define   GSGLINTERNAL_END_ANIME     -5

#define   GSGLINTERNAL_LINEWIDTH     19 /* width (cm) */
#define   GSGLINTERNAL_LINEWIDTH2    20 /* pixel in X, 0.5 point in ps */

#define   GSGLINTERNAL_MODELVIEW_MATRIX   100
#define   GSGLINTERNAL_NORMAL_MODE   101
#define   GSGLINTERNAL_2D_MODE       102
#define   GSGLINTERNAL_3D_MODE       103
#define   GSGLINTERNAL_PROJECTION_MATRIX  104
#define   GSGLINTERNAL_PSTEXT             105 /* added int 0.3.68 */

#define   GSGLINTERNALPS_BEGIN_GOURAUD_ONLY   201
#define   GSGLINTERNALPS_END_GOURAUD_ONLY    -201
#define   GSGLINTERNALPS_DEPTH_BY_DEEP        202
#define   GSGLINTERNALPS_DEPTH_BY_SHALLOW     203
#define   GSGLINTERNALPS_DEPTH_BY_MEAN        204
#define   GSGLINTERNALPS_DEPTH_BY_VECT_PROD   205 /*3つの点 A,B,C に対して
						    (AB x BC)z > 0 なら DEEP,
						    < 0 なら SHALLOW,
						    = 0 なら MEAN. */
#define   GSGLINTERNALPS_DEPTH_BY_VECT_PROD2  206 /*3つの点 A,B,C に対して
						    (AB x BC)z > 0 なら 1つ目,
						    < 0 なら 2つ目,
						    = 0 なら 3つ目の点の深さ. */
#define   GSGLINTERNALPS_DEPTH_BY_POINT       207

typedef struct _gsglGLFeedbackBuffer {
	GLfloat *fbuffer;
	unsigned int size;
} gsglGLFeedbackBuffer;

gsglGLFeedbackBuffer * gsgl_internal_format_make (gsglPage *p);

#endif

