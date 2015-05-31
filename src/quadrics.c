#include "gsaf.h"
#include "matrix.h"
#include "base.h"
#include "displaylist.h"
#include "drv_wrapper.h"
#include "utils.h"
#include "bsc1.h"

/*slices : The number of subdivisions around the Z axis (similar to lines of longitude)*/
/*stacks : The number of subdivisions along the Z axis  (similar to lines of latitude) */

static void set_matrix_for_quadrics (float x, float y, float z, float vx, float vy, float vz, gsglPage *p)
{
	gsglvec3f vector, axis;
	float mat1[16], mat2[16], mat3[16];
	float length;
	float angle;

	gsgl_3dmode (p);

	length = sqrt (vx*vx+vy*vy+vz*vz);

	if (length < 1.e-5) {
		gsgl_warning ("set_matrix_for_quadrics", "axis vector's length is zero");
		return;
	}

	vector.x = vx / length;
	vector.y = vy / length;
	vector.z = vz / length;
	
	gsgl_matrix_inq_3dmatrix (mat1, p);

	/*glTranslatef (x, y, z);*/
	gsgl_inq_matrix_translate (x, y, z, mat2);
	gsgl_mat16_mult_mat16 (mat1, mat2, mat3);

	/*        ^          */
	/* axis = z x vector */
	axis.x = -vector.y;
	axis.y = vector.x;
	axis.z = 0.0;
	
	length = sqrt(axis.x*axis.x+axis.y*axis.y+axis.z*axis.z);
	if (length < 1.e-5) { /* vector is parallel to z-axis */
		axis.x = 1.0;
		axis.y = 0.0;
		axis.z = 0.0;
	} else {
		axis.x = axis.x / length;
		axis.y = axis.y / length;
		axis.z = axis.z / length;
	}

	angle = acos(vector.z);
	
	gsgl_inq_matrix_rotate (angle * GSGL_RAD, axis.x, axis.y, axis.z, mat1);
	gsgl_mat16_mult_mat16 (mat3, mat1, mat2);

	gsgl_modelview_matrix (mat2, p);
}

static void gsglSolidSphere (float radius, int slices, int stacks, gsglPage *p)
{
	int i, j;
	float x[4], y[4], z[4];
	float theta1, theta2, phi1, phi2;

	for (i = 0; i < slices; i++) {
		for (j = 0; j < stacks; j++) {
			theta1 = j / (float) stacks * M_PI;
			phi1 = 2.0 * i / (float) slices * M_PI;
			theta2 = (j + 1) / (float) stacks * M_PI;
			phi2 = 2.0 * (i + 1) / (float) slices * M_PI;

			if (j == 0) {
				x[0] = radius * sin (theta1) * cos (phi1);
				y[0] = radius * sin (theta1) * sin (phi1);
				z[0] = radius * cos (theta1);
				
				x[1] = radius * sin (theta2) * cos (phi1);
				y[1] = radius * sin (theta2) * sin (phi1);
				z[1] = radius * cos (theta2);
				
				x[2] = radius * sin (theta2) * cos (phi2);
				y[2] = radius * sin (theta2) * sin (phi2);
				z[2] = radius * cos (theta2);
				
				gsgl_polygon3d_with_normals (x, y, z, x, y, z, 3, p);
			} else if (j == stacks-1) {
				x[0] = radius * sin (theta1) * cos (phi1);
				y[0] = radius * sin (theta1) * sin (phi1);
				z[0] = radius * cos (theta1);
				
				x[1] = radius * sin (theta2) * cos (phi1);
				y[1] = radius * sin (theta2) * sin (phi1);
				z[1] = radius * cos (theta2);
				
				x[2] = radius * sin (theta1) * cos (phi2);
				y[2] = radius * sin (theta1) * sin (phi2);
				z[2] = radius * cos (theta1);
				
				gsgl_polygon3d_with_normals (x, y, z, x, y, z, 3, p);
			} else {
				x[0] = radius * sin (theta1) * cos (phi1);
				y[0] = radius * sin (theta1) * sin (phi1);
				z[0] = radius * cos (theta1);
				
				x[1] = radius * sin (theta2) * cos (phi1);
				y[1] = radius * sin (theta2) * sin (phi1);
				z[1] = radius * cos (theta2);
				
				x[2] = radius * sin (theta2) * cos (phi2);
				y[2] = radius * sin (theta2) * sin (phi2);
				z[2] = radius * cos (theta2);
				
				x[3] = radius * sin (theta1) * cos (phi2);
				y[3] = radius * sin (theta1) * sin (phi2);
				z[3] = radius * cos (theta1);
				
				gsgl_polygon3d_with_normals (x, y, z, x, y, z, 4, p);
			}
		}
	}
}

/* Draw a complex object */
void gsgl_solidsphere (float x, float y, float z, float vx, float vy, float vz,
		       float radius, int slices, int stacks)
{
	gsglPage *p = gsgl_inq_page ();

	if (!gsgl_inq_page_is_open ()) return;

	set_matrix_for_quadrics (x, y, z, vx, vy, vz, p);

	gsglSolidSphere (radius, slices, stacks, p);

	gsgl_3dmode (p);
}

static void gsglWireSphere (float radius, int slices, int stacks, gsglPage *p)
{
	int i, j;
	float x[4], y[4], z[4];
	float theta1, theta2, phi1, phi2;

	for (i = 0; i < slices; i++) {
		for (j = 0; j < stacks; j++) {
			theta1 = j / (float) stacks * M_PI;
			phi1 = 2.0 * i / (float) slices * M_PI;
			theta2 = (j + 1) / (float) stacks * M_PI;
			phi2 = 2.0 * (i + 1) / (float) slices * M_PI;

			if (j == 0) {
				x[0] = radius * sin (theta1) * cos (phi1);
				y[0] = radius * sin (theta1) * sin (phi1);
				z[0] = radius * cos (theta1);
				
				x[1] = radius * sin (theta2) * cos (phi1);
				y[1] = radius * sin (theta2) * sin (phi1);
				z[1] = radius * cos (theta2);
				
				x[2] = radius * sin (theta2) * cos (phi2);
				y[2] = radius * sin (theta2) * sin (phi2);
				z[2] = radius * cos (theta2);
				
				gsgl_lines3d_with_normals (x, y, z, x, y, z, 3, p);
			} else if (j == stacks-1) {
				x[0] = radius * sin (theta1) * cos (phi1);
				y[0] = radius * sin (theta1) * sin (phi1);
				z[0] = radius * cos (theta1);
				
				x[1] = radius * sin (theta2) * cos (phi1);
				y[1] = radius * sin (theta2) * sin (phi1);
				z[1] = radius * cos (theta2);
				
				x[2] = radius * sin (theta1) * cos (phi2);
				y[2] = radius * sin (theta1) * sin (phi2);
				z[2] = radius * cos (theta1);
				
				gsgl_lines3d_with_normals (x, y, z, x, y, z, 3, p);
			} else {
				x[0] = radius * sin (theta1) * cos (phi1);
				y[0] = radius * sin (theta1) * sin (phi1);
				z[0] = radius * cos (theta1);
				
				x[1] = radius * sin (theta2) * cos (phi1);
				y[1] = radius * sin (theta2) * sin (phi1);
				z[1] = radius * cos (theta2);
				
				x[2] = radius * sin (theta2) * cos (phi2);
				y[2] = radius * sin (theta2) * sin (phi2);
				z[2] = radius * cos (theta2);
				
				x[3] = radius * sin (theta1) * cos (phi2);
				y[3] = radius * sin (theta1) * sin (phi2);
				z[3] = radius * cos (theta1);
				
				gsgl_lines3d_with_normals (x, y, z, x, y, z, 4, p);
			}
		}
	}
}


void gsgl_wiresphere (float x, float y, float z, float vx, float vy, float vz,
		      float radius, int slices, int stacks)
{
	gsglPage *p = gsgl_inq_page ();

	if (!gsgl_inq_page_is_open ()) return;

	set_matrix_for_quadrics (x, y, z, vx, vy, vz, p);
	
	gsglWireSphere (radius, slices, stacks, p);
	
	gsgl_3dmode (p);
}

static void gsglSolidTorus (float innerRadius, float outerRadius, int nsides, int rings, gsglPage *p)
{
	int i, j;
	float x[4], y[4], z[4];
	float nx[4], ny[4], nz[4];
	float theta1, theta2, phi1, phi2;

	for (i = 0; i < rings; i++) {
		for (j = 0; j < nsides; j++) {
			theta1 = 2.0 * j / (float) nsides * M_PI;
			phi1 = 2.0 * i / (float) rings * M_PI;
			theta2 = 2.0 * (j + 1) / (float) nsides * M_PI;
			phi2 = 2.0 * (i + 1) / (float) rings * M_PI;

			x[0] = (outerRadius + innerRadius * cos (theta1)) * cos (phi1);
			y[0] = (outerRadius + innerRadius * cos (theta1)) * sin (phi1);
			z[0] = innerRadius * sin (theta1);

			nx[0] = innerRadius * cos (theta1) * cos (phi1);
			ny[0] = innerRadius * cos (theta1) * sin (phi1);
			nz[0] = innerRadius * sin (theta1);

			x[1] = (outerRadius + innerRadius * cos (theta1)) * cos (phi2);
			y[1] = (outerRadius + innerRadius * cos (theta1)) * sin (phi2);
			z[1] = innerRadius * sin (theta1);

			nx[1] = innerRadius * cos (theta1) * cos (phi2);
			ny[1] = innerRadius * cos (theta1) * sin (phi2);
			nz[1] = innerRadius * sin (theta1);

			x[2] = (outerRadius + innerRadius * cos (theta2)) * cos (phi2);
			y[2] = (outerRadius + innerRadius * cos (theta2)) * sin (phi2);
			z[2] = innerRadius * sin (theta2);

			nx[2] = innerRadius * cos (theta2) * cos (phi2);
			ny[2] = innerRadius * cos (theta2) * sin (phi2);
			nz[2] = innerRadius * sin (theta2);

			x[3] = (outerRadius + innerRadius * cos (theta2)) * cos (phi1);
			y[3] = (outerRadius + innerRadius * cos (theta2)) * sin (phi1);
			z[3] = innerRadius * sin (theta2);

			nx[3] = innerRadius * cos (theta2) * cos (phi1);
			ny[3] = innerRadius * cos (theta2) * sin (phi1);
			nz[3] = innerRadius * sin (theta2);

			gsgl_polygon3d_with_normals (x, y, z, nx, ny, nz, 4, p);
		}
	}
}

void gsgl_solidtorus (float x, float y, float z, float vx, float vy, float vz,
		      float innerRadius, float outerRadius, int nsides, int rings)
{
	gsglPage *p = gsgl_inq_page ();

	if (!gsgl_inq_page_is_open ()) return;

	set_matrix_for_quadrics (x, y, z, vx, vy, vz, p);

	gsglSolidTorus (innerRadius, outerRadius, nsides, rings, p);

	gsgl_3dmode (p);
}

static void gsglWireTorus (float innerRadius, float outerRadius, int nsides, int rings, gsglPage *p)
{
	int i, j;
	float x[4],y[4], z[4];
	float nx[4], ny[4], nz[4];
	float theta1, theta2, phi1, phi2;

	for (i = 0; i < rings; i++) {
		for (j = 0; j < nsides; j++) {
			theta1 = 2.0 * j / (float) nsides * M_PI;
			phi1 = 2.0 * i / (float) rings * M_PI;
			theta2 = 2.0 * (j + 1) / (float) nsides * M_PI;
			phi2 = 2.0 * (i + 1) / (float) rings * M_PI;
			
			x[0] = (outerRadius + innerRadius * cos (theta1)) * cos (phi1);
			y[0] = (outerRadius + innerRadius * cos (theta1)) * sin (phi1);
			z[0] = innerRadius * sin (theta1);

			nx[0] = innerRadius * cos (theta1) * cos (phi1);
			ny[0] = innerRadius * cos (theta1) * sin (phi1);
			nz[0] = innerRadius * sin (theta1);

			x[1] = (outerRadius + innerRadius * cos (theta1)) * cos (phi2);
			y[1] = (outerRadius + innerRadius * cos (theta1)) * sin (phi2);
			z[1] = innerRadius * sin (theta1);

			nx[1] = innerRadius * cos (theta1) * cos (phi2);
			ny[1] = innerRadius * cos (theta1) * sin (phi2);
			nz[1] = innerRadius * sin (theta1);

			x[2] = (outerRadius + innerRadius * cos (theta2)) * cos (phi2);
			y[2] = (outerRadius + innerRadius * cos (theta2)) * sin (phi2);
			z[2] = innerRadius * sin (theta2);

			nx[2] = innerRadius * cos (theta2) * cos (phi2);
			ny[2] = innerRadius * cos (theta2) * sin (phi2);
			nz[2] = innerRadius * sin (theta2);

			x[3] = (outerRadius + innerRadius * cos (theta2)) * cos (phi1);
			y[3] = (outerRadius + innerRadius * cos (theta2)) * sin (phi1);
			z[3] = innerRadius * sin (theta2);

			nx[3] = innerRadius * cos (theta2) * cos (phi1);
			ny[3] = innerRadius * cos (theta2) * sin (phi1);
			nz[3] = innerRadius * sin (theta2);

			gsgl_lines3d_with_normals (x, y, z, nx, ny, nz, 4, p);
		}
	}
}

void gsgl_wiretorus (float x, float y, float z, float vx, float vy, float vz,
		     float innerRadius, float outerRadius, int nsides, int rings)
{
	gsglPage *p = gsgl_inq_page ();

	if (!gsgl_inq_page_is_open ()) return;

	set_matrix_for_quadrics (x, y, z, vx, vy, vz, p);

	gsglWireTorus (innerRadius, outerRadius, nsides, rings, p);

	gsgl_3dmode (p);
}
