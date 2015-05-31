#include "gsaf.h"
#include "gsgl.h"
#include "base.h"
#include "utils.h"
#include "utils2.h"
#include "matrix.h"
#include "bsc1.h"
#include "bsc3d.h"

/* find nearest point to eye from given n-points */
void gsglinqnearestpoint (float *x, float *y, float *z, int n, int *nnearest, gsglPage *p)
{
	int i;
	float *x_dev, *y_dev, *z_dev;
	double distance, distance1;
	float x_eye, y_eye, z_eye;
	float x1, y1, z1;
	float x2, y2, z2;
	
	if (n <= 1) return;

	gsgl_inq_eyepoint (&x_eye, &y_eye, &z_eye, p);
      
	x_dev = (float *) gsgl_malloc_atomic (sizeof(float)*n);
	y_dev = (float *) gsgl_malloc_atomic (sizeof(float)*n);
	z_dev = (float *) gsgl_malloc_atomic (sizeof(float)*n);
	for (i=0; i<n; i++) {
		gsglproject3d (x[i], y[i], z[i], &x_dev[i], &y_dev[i], &z_dev[i], p);
	}
      
	distance = sqrt((x_eye-x_dev[0])*(x_eye-x_dev[0])+
			(y_eye-y_dev[0])*(y_eye-y_dev[0])+
			(z_eye-z_dev[0])*(z_eye-z_dev[0]));
	*nnearest = 0;
	
	for (i=1; i<n; i++) {
		distance1 = sqrt((x_eye-x_dev[i])*(x_eye-x_dev[i])+
				 (y_eye-y_dev[i])*(y_eye-y_dev[i])+
				 (z_eye-z_dev[i])*(z_eye-z_dev[i]));
		if  (fabs(distance1-distance) < 1.e-5) {
			if (fabs(z[*nnearest]-z[i]) < 1.e-5) {
				gsglproject3d (x[*nnearest],y[*nnearest],z[*nnearest],
					       &x1, &y1, &z1, p);
				gsglproject3d (x[i],y[i],z[i], &x2, &y2, &z2, p);
				if (fabs(x1-x2) < 1.e-5) {
					if (y1 > y2) {
						distance = distance1;
						*nnearest = i;
					}
				} else if (x1 < x2) {
					distance = distance1;
					*nnearest = i;
				}
			} else if (z[*nnearest] < z[i]) {
				distance = distance1;
				*nnearest = i;
			}
		} else if (distance1 < distance ) {
			distance = distance1;
			*nnearest = i;
		}
	}
}

/* find farest point to eye from given n-points */
void gsglinqmostfarpoint (float *x, float *y, float *z, int n, int *nfar, gsglPage *p)
{
	int i;
	float *x_dev, *y_dev, *z_dev;
	float distance, distance1;
	float x_eye, y_eye, z_eye;

	if (n <= 1) return;

	gsgl_inq_eyepoint (&x_eye, &y_eye, &z_eye, p);
      
	x_dev = (float *) gsgl_malloc_atomic (sizeof(float)*n);
	y_dev = (float *) gsgl_malloc_atomic (sizeof(float)*n);
	z_dev = (float *) gsgl_malloc_atomic (sizeof(float)*n);
	for (i=0; i<n; i++) {
		gsglproject3d (x[i], y[i], z[i], &x_dev[i], &y_dev[i], &z_dev[i], p);
	}
      
	distance = sqrt((x_eye-x_dev[0])*(x_eye-x_dev[0])+
			(y_eye-y_dev[0])*(y_eye-y_dev[0])+
			(z_eye-z_dev[0])*(z_eye-z_dev[0]));
	*nfar = 0;

	for (i=1; i<n; i++) {
		distance1 = sqrt((x_eye-x_dev[i])*(x_eye-x_dev[i])+
				 (y_eye-y_dev[i])*(y_eye-y_dev[i])+
				 (z_eye-z_dev[i])*(z_eye-z_dev[i]));
		if (distance1 > distance) {
			distance = distance1;
			*nfar = i;
		}
	}
}

void gsgl2psdepthbydeepest (gsglPage *p)
{
	float x[8], y[8], z[8];
	int n;

	inqcorner3d(x, y, z, p);
	gsglinqmostfarpoint(x, y, z, 8, &n, p);
	gsgl_ps_depth_by_point (x[n], y[n], z[n], p);
}

void gsglprojectview (float x1, float y1, float *x2, float *y2)
{
	float pxmin, pxmax, pymin, pymax, xmin, xmax, ymin, ymax;

	gsaf_inqvew (&pxmin, &pxmax, &pymin, &pymax, &xmin, &xmax, &ymin, &ymax);

	*x2 = (x1 - xmin) * (pxmax - pxmin)/(xmax - xmin) + pxmin;
	*y2 = (y1 - ymin) * (pymax - pymin)/(ymax - ymin) + pymin;
}

void gsglunprojectview (float x1, float y1, float *x2, float *y2)
{
	float pxmin, pxmax, pymin, pymax, xmin, xmax, ymin, ymax;
	
	gsaf_inqvew (&pxmin, &pxmax, &pymin, &pymax, &xmin, &xmax, &ymin, &ymax);

	*x2 = (x1 - pxmin) * (xmax - xmin)/(pxmax - pxmin) + xmin;
	*y2 = (y1 - pymin) * (ymax - ymin)/(pymax - pymin) + ymin;
}
