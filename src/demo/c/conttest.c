#include "gsgl.h"
#include <stdio.h>
#include <math.h>
/*
  c
  c     ****** cont/cont2/cont3 test program ******
  c
*/

void ctest0 ();
void ctest1 ();
void ctest2 ();
void ctest3 ();
void ctest4 ();
float a[41][41];

int main ()
{
	int id = 0;
	int error;

	gsaf_gsopen ();

	for (;;) {
		do {
			error = gsgl_stdin (" ## INPUT : TYPE (0,1,2,3,4) ?\n",
					    1, " %d", &id);
			if (error == EOF) goto EXIT;
		} while (error == 1 || error == 2);

		if (id == 0) {
			ctest0 ();
		} else if (id == 1) {
			ctest1 ();
		} else if (id == 2) {
			ctest2 ();
		} else if (id == 3) {
			ctest3 ();
		} else if (id == 4) {
			ctest4 ();
		}
	}
 EXIT:
	gsaf_gsclos ();
	return 0;
}

/*
  c
  c     ****** test of linesp ******
  c
*/
void ctest0 ()
{
	float xp[20], yp[20];
	float x[] = {10.0,15.0,10.0, 5.0,10.0};
	float y[] = {15.0,17.0, 5.0,12.0,15.0};
	int np;

	gsaf_pages ();

	gsaf_setlin (0, 0, 5);
	gsaf_linept (x, y, 5, 0);
	gsaf_setlin (0, 0, 6);
	gsaf_gusp2d (x, y, 5, xp, yp, 20, &np, 4);
	gsaf_linept (xp, yp, np, 0);

	gsaf_pagee ();
}

/*
  c
  c     ****** test of cont?1 ******
  c
*/
void ctest1 ()
{
	float zl[10], rgbl[10][3], wln[10];
	int iln[10];
	float rgb[11][3] = {{0.00,0.00,0.00},
			    {0.20,0.10,0.00},
			    {0.40,0.20,0.00},
			    {0.60,0.30,0.00},
			    {0.80,0.40,0.00},
			    {1.00,0.50,0.00},
			    {1.00,0.60,0.20},
			    {1.00,0.70,0.40},
			    {1.00,0.80,0.60},
			    {1.00,0.90,0.80},
			    {1.00,1.00,1.00}};
	int i, j;
	float fx, fy;
	static int model=2;
	float zorg=0.0;
	float zstep=0.2;
	int nstep=10;
	int ipat=0;
	int ispl=0;
	int error;

	for (i=0; i<41; i++) {
		for (j=0; j<41; j++) {
			fx = 0.1*(i-20)+0.01;
			fy = 0.1*(j-20);
			a[j][i] = pow(fx, 4.0)+pow(fy, 4.0)-2.0*(fx*fx+fy*fy)+2.0;
		}
	}
 
	for (;;) {
	do {
		error = gsgl_stdin (" ## INPUT : MODEL(1-5)\n", 1, " %d", &model);
		if (error == EOF) return;
	} while (error == 1);

	for (i=0; i<nstep; i++) {
		zl[i] = zstep*i+zorg;
		rgbl[i][0] = 0.0;
		rgbl[i][1] = 0.0;
		rgbl[i][2] = 0.0;
		iln[i] = ipat;
		wln[i] = 0.0;
	}

	gsaf_pages ();
	gsaf_gdefin (3.0, 23.0, 2.0, 17.0, 1.0, 41.0, 1.0, 41.0);
	gsaf_gframe ();
	if (model == 1) {
		gsaf_contr1 (&a[0][0], 41, 41, 41, zorg, zstep, nstep, 0);
	} else if (model == 2) {
		/*gsaf_conte1(a, 41, 41, 41, zl, nstep, 0, ipat)*/
		gsaf_contp1 (&a[0][0], 41, 41, 41, zorg, zstep, nstep, 0, ipat);
	} else if (model == 3) {
		gsaf_contq1 (&a[0][0], 41, 41, 41, zorg, zstep, nstep, 0, ipat);
	} else if (model == 4) {
		gsaf_contg1 (&a[0][0], 41, 41, 41, zl, &rgbl[0][0], iln, wln, nstep, ispl, 0);
	} else if (model == 5) {
		gsaf_contf1 (&a[0][0], 41, 41, 41, zl, &rgb[0][0], nstep, 0);
		gsaf_setlin (0, 0, 7);
		gsaf_gframe ();
		gsaf_rgbbar (3.0, 23.0, 0.5, 1.5, &rgb[0][0], nstep+1, 0);
		gsaf_rgbbar (23.5, 24.5, 2.0, 17.0, &rgb[0][0], nstep+1, 1);
	}
	gsaf_pagee ();
	}
}

/*
  c
  c     ****** test of cont?2 ******
  c
*/
void ctest2 ()
{
	float x[41], y[41];
	float zl[10], rgbl[10][3], wln[10];
	int iln[10];
	float rgb[11][3] = {{0.00,0.00,0.00},
			    {0.00,0.10,0.20},
			    {0.00,0.20,0.40},
			    {0.00,0.30,0.60},
			    {0.00,0.40,0.80},
			    {0.00,0.50,1.00},
			    {0.20,0.60,1.00},
			    {0.40,0.70,1.00},
			    {0.60,0.80,1.00},
			    {0.80,0.90,1.00},
			    {1.00,1.00,1.00}};
	int i, j;
	int model=2;
	float zorg=2.0;
	float zstep=-0.2;
	int nstep=10;
	int ipat=0;
	int ispl=0;
	int error;

	for (i=0; i<41; i++) {
		x[i] = 2.0*pow(0.025*i, 2.0);
	}
	for (j=0; j<41; j++) {
		y[j] = 2.0*pow(0.025*j, 2.0);
	}

	for (i=0; i<41; i++) {
		for (j=0; j<41; j++) {
			a[j][i] = pow(x[i], 4.0)+pow(y[j], 4.0)-2.0*(x[i]*x[i]+y[j]*y[j])+2.0;
		}
	}

	for (;;) {
	do {
		error = gsgl_stdin (" ## INPUT : MODEL(1-5)\n", 1, " %d", &model);
		if (error == EOF) return;
	} while (error == 1);

	for (i=0; i<nstep; i++) {
		zl[i] = zstep*i+zorg;
		rgbl[i][0] = 0.0;
		rgbl[i][1] = 0.0;
		rgbl[i][2] = 0.0;
		iln[i] = ipat;
		wln[i] = 0.0;
	}

	gsaf_pages ();
	gsaf_gdefin(3.0, 23.0, 2.0, 17.0, 0.0, 2.0, 0.0, 2.0);
	gsaf_gframe ();
	
	if (model == 1) {
		gsaf_contr2 (&a[0][0], x, y, 41, 41, 41, zorg, zstep, nstep, 0);
	} else if (model == 2) {
		gsaf_contp2 (&a[0][0], x, y, 41, 41, 41, zorg, zstep, nstep, 0, ipat);
	} else if (model == 3) {
		gsaf_contq2 (&a[0][0], x, y, 41, 41, 41, zorg, zstep, nstep, 0, ipat);
	} else if (model == 4) {
		gsaf_contg2 (&a[0][0], x, y, 41, 41, 41, zl, &rgbl[0][0], iln,wln,nstep,ispl,0);
	} else if (model == 5) {
		gsaf_contf2 (&a[0][0], x, y, 41, 41, 41, zl, &rgb[0][0], nstep, 0);
		gsaf_setlin (0, 0, 7);
		gsaf_gframe ();
		gsaf_rgbbar (3.0, 23.0, 0.5, 1.5, &rgb[0][0], nstep+1, 0);
		gsaf_rgbbar (23.5, 24.5, 2.0, 17.0, &rgb[0][0], nstep+1, 1);
	}
	gsaf_pagee ();
	}
}

/*
  c
  c     ****** test of cont?3 ******
  c
*/
void ctest3 ()
{
	float zl[10], rgbl[10][3], wln[10], r[41];
	int iln[10];
	float rgb[11][3] = {{0.00,0.00,0.00},
			    {0.10,0.20,0.00},
			    {0.20,0.40,0.00},
			    {0.30,0.60,0.00},
			    {0.40,0.80,0.00},
			    {0.50,1.00,0.00},
			    {0.60,1.00,0.20},
			    {0.70,1.00,0.40},
			    {0.80,1.00,0.60},
			    {0.90,1.00,0.80},
			    {1.00,1.00,1.00}};
	int n=41;
	float pi;
	int i, j;
	int model=2;
	float zorg=0.0;
	float zstep=0.1;
	int nstep=10;
	int ipat=0;
	int ispl=0;
	float rl, th;
	int error;

	pi=2.0*asin(1.0);

	for (i=0; i<n; i++) {
		r[i] = i/(float)(n-1);
	}

	for (i=0; i<n; i++) {
		for (j=0; j<n; j++) {
			rl = r[i];
			th = 2.0*pi*j/(float) n;
			a[j][i] = rl*rl/(1.0+rl*rl)+0.2*sin(3.0*th);
		}
	}

	for (;;) {
	do {
		error = gsgl_stdin (" ## INPUT : MODEL(1-5)\n", 1, " %d", &model);
		if (error == EOF) return;
	} while (error == 1);

	for (i=0; i<nstep; i++) {
		zl[i] = zstep*i+zorg;
		rgbl[i][0] = 0.0;
		rgbl[i][1] = 0.0;
		rgbl[i][2] = 0.0;
		iln[i] = ipat;
		wln[i] = 0.0;
	}

	gsaf_pages ();
	gsaf_gdefin (5.0, 20.0, 2.0, 17.0, -1.0, 1.0, -1.0, 1.0);
	gsaf_gframe ();

	if (model == 1) {
		gsaf_contr3 (&a[0][0], r, 41, n, n, zorg, zstep, nstep);
	} else if (model == 2) {
		gsaf_contp3 (&a[0][0], r, 41, n, n, zorg, zstep, nstep, ipat);
	} else if (model == 3) {
		gsaf_contq3 (&a[0][0], r, 41, n, n, zorg, zstep, nstep, ipat);
	} else if (model == 4) {
		gsaf_contg3 (&a[0][0], r, 41, n, n, zl, &rgbl[0][0], iln, wln, nstep, ispl);
		for (i=0; i<nstep; i++) {
			rgbl[i][0] = 1.0;
		}
		gsaf_contg3 (&a[0][0], r, 41, n, n, zl, &rgbl[0][0], iln, wln, nstep, 2);
	} else if (model == 5) {
		gsaf_contf3 (&a[0][0], r, 41, n, n, zl, &rgb[0][0], nstep);
		gsaf_setlin (0, 0, 7);
		gsaf_gframe ();
		gsaf_rgbbar (5.0, 20.0, 0.5, 1.5, &rgb[0][0], nstep+1, 0);
		gsaf_rgbbar (20.5, 21.5, 2.0, 17.0, &rgb[0][0], nstep+1, 1);
	}
	gsaf_pagee ();
	}
}

/*
  c
  c     ****** test of cont?4 ******
  c
*/
void ctest4 ()
{
	float r[41], t[41];
	float zl[10], rgbl[10][3], wln[10];
	int iln[10];
	float rgb[11][3] = {{0.00,0.00,0.00},
			    {0.10,0.00,0.20},
			    {0.20,0.00,0.40},
			    {0.30,0.00,0.60},
			    {0.40,0.00,0.80},
			    {0.50,0.00,1.00},
			    {0.60,0.20,1.00},
			    {0.70,0.40,1.00},
			    {0.80,0.60,1.00},
			    {0.90,0.80,1.00},
			    {1.00,1.00,1.00}};
	int i, j;
	int model = 2;
	float zorg = 1.0;
	float zstep = -0.1;
	int nstep = 10;
	int ipat = 0;
	int ispl = 0;
	int error;

	for (i=0; i<41; i++) {
		r[i] = pow (0.025*i, 2.0);
	}
	for (j=0; j<41; j++) {
		t[j] = 3.1415926 * 0.025 * j;
	}
	for (i=0; i<41; i++) {
		for (j=0; j<41; j++) {
			a[j][i] = exp(-3.0*r[i]*r[i]) + 0.2*cos(4.0*t[j]) * r[i];
		}
	}

	for (;;) {
	do {
		error = gsgl_stdin (" ## INPUT : MODEL(1-5)\n", 1, " %d", &model);
		if (error == EOF) return;
	} while (error == 1);

	for (i=0; i<nstep; i++) {
		zl[i] = zstep*i+zorg;
		rgbl[i][0] = 0.0;
		rgbl[i][1] = 0.0;
		rgbl[i][2] = 0.0;
		iln[i] = ipat;
		wln[i] = 0.0;
	}

	gsaf_pages ();
	gsaf_gdefin (3.0, 23.0, 3.0, 13.0, -1.0, 1.0, 0.0, 1.0);
	gsaf_gframe ();

	if (model == 1) {
		gsaf_contr4 (&a[0][0], r, t, 41, 41, 41, zorg, zstep, nstep);
	} else if (model == 2) {
		gsaf_contp4 (&a[0][0], r, t, 41, 41, 41, zorg, zstep, nstep, ipat);
	} else if (model == 3) {
		gsaf_contq4 (&a[0][0], r, t, 41, 41, 41, zorg, zstep, nstep, ipat);
	} else if (model == 4) {
		gsaf_contg4 (&a[0][0], r, t, 41, 41, 41, zl, &rgbl[0][0], iln, wln,nstep,ispl);
	} else if (model == 5) {
		gsaf_contf4 (&a[0][0], r, t, 41, 41, 41, zl, &rgb[0][0], nstep);
		gsaf_setlin (0, 0, 7);
		gsaf_gframe ();
		gsaf_rgbbar (3.0, 23.0, 1.5, 2.5, &rgb[0][0], nstep+1, 0);
		gsaf_rgbbar (23.5, 24.5, 3.0, 13.0, &rgb[0][0], nstep+1, 1);
	}
	gsaf_pagee ();
	}
}
