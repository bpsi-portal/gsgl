#include "gsgl.h"
#include <stdio.h>
#include <math.h>

int main ()
{
	float x[51], y[51], dx, y0, ya, gxorg, gyorg, scl;
	int mode, nxs, nxv, nys, nyv;
	int nxsl, nxvl, nysl, nyvl;
	int nm, np, n;
	float xmin, xmax, gxmin, gxmax, gxscal;
	float ymin, ymax, gymin, gymax, gyscal;
	float s;
	int error;

	gsaf_gsopen ();

	mode=0;
	dx=2*3.1415927/50;
	y0=0.0;
	ya=1.0;
	gxorg=0.0;
	gyorg=0.0;
	nxs=9;
	nxv=2;
	nys=9;
	nyv=2;
	nxsl=7;
	nxvl=1;
	nysl=9;
	nyvl=9;
	nm=0;
	np=0;
	scl=0.3;

	for (;;) {
		do {
			error = gsgl_stdin (" ## INPUT TYPE (0..7,8,9) ?\n", 1, "%d", &mode);
			if (error == EOF) goto EXIT;
		} while (error == 1 || error == 2);

		if (mode <= 3) {
			for (;;) {
			do {
				error = gsgl_stdin (" # INPUT : Y0,YA,GXORG,GYORG\n",
						    4, "%f%*c%f%*c%f%*c%f", 
						    &y0, &ya, &gxorg, &gyorg);
				if (error == EOF) goto EXIT;
			} while (error == 1);
			do {
				error = gsgl_stdin (" # INPUT : NXS,NYS,NXV,NYV,NM,NP,SCL\n",
						    7, "%d%*c%d%*c%d%*c%d%*c%d%*c%d%*c%f",
						    &nxs,&nys,&nxv,&nyv,&nm,&np,&scl);
				if (error == EOF) goto EXIT;
			} while (error == 1);

			for (n=0; n<51; n++) {
				x[n] = n * dx;
				y[n] = y0+ya*sin(x[n]);
			}
			gsaf_gmnmx1 (x,0,50,1,&xmin,&xmax);
			gsaf_gqscal (xmin,xmax,&gxmin,&gxmax,&gxscal);
			gsaf_gmnmx1 (y,0,50,1,&ymin,&ymax);
			gsaf_gqscal (ymin,ymax,&gymin,&gymax,&gyscal);

			gsaf_pages ();
			gsaf_setchs (0.35,0.0);
			if (mode == 0) {
				gsaf_gdefin(4.0,24.0,2.0,17.0,gxmin,gxmax,gymin,gymax);
			} else if (mode == 1) {
				gsaf_gdefin(4.0,24.0,2.0,17.0,gxmax,gxmin,gymin,gymax);
			} else if (mode == 2) {
				gsaf_gdefin(4.0,24.0,2.0,17.0,gxmin,gxmax,gymax,gymin);
			} else {
				gsaf_gdefin(4.0,24.0,2.0,17.0,gxmax,gxmin,gymax,gymin);
			}
			gsaf_gframe ();
			gsaf_gscale (gxorg,gxscal,0.0,0.0,scl,nxs);
			gsaf_gscale (0.0,0.0,gyorg,gyscal,scl,nys);
			gsaf_gvalue (gxorg,2*gxscal,0.0,0.0,nxv);
			gsaf_gvalue (0.0,0.0,gyorg,2*gyscal,nyv);
			gsaf_gplotp (x,y,0,50,1,nm,10,np);
			gsaf_pagee ();
			}

		} else if (mode <= 7) {
			for (;;) {
			ONE:
			do {
				error = gsgl_stdin (" # INPUT : Y0,YA,GXORG,GYORG\n",
						    4, "%f%*c%f%*c%f%*c%f", 
						    &y0, &ya, &gxorg, &gyorg);
				if (error == EOF) goto EXIT;
			} while (error == 1);
			TWO:
			do {
				error = gsgl_stdin (" # INPUT : NXS,NYS,NXV,NYV,NM,NP,SCL\n",
						    7, "%d%*c%d%*c%d%*c%d%*c%d%*c%d%*c%f",
						    &nxs,&nys,&nxv,&nyv,&nm,&np,&scl);
				if (error == EOF) goto ONE;
			} while (error == 1);
			do {
				error = gsgl_stdin (" # INPUT : NXSL,NYSL,NXVL,NYVL\n",
						    4, "%d%*c%d%*c%d%*c%d",
						    &nxsl,&nysl,&nxvl,&nyvl);
				if (error == EOF) goto TWO;
			} while (error == 1);

			for (n=0; n<51; n++) {
				x[n] = n * dx;
				y[n] = y0+ya*sin(x[n]);
			}
			gsaf_gmnmx1 (x,0,50,1,&xmin,&xmax);
			gsaf_gqscal (xmin,xmax,&gxmin,&gxmax,&gxscal);
			gsaf_gmnmx1 (y,0,50,1,&ymin,&ymax);
			gsaf_gqscal (ymin,ymax,&gymin,&gymax,&gyscal);

			gsaf_pages ();
			gsaf_setchs (0.35,0.0);
			if (mode == 4) {
				gsaf_gdefin (4.0,24.0,2.0,17.0,gxmin,gxmax,gymin,gymax);
			} else if (mode == 5) {
				gsaf_gdefin (4.0,24.0,2.0,17.0,gxmax,gxmin,gymin,gymax);
			} else if (mode == 6) {
				gsaf_gdefin (4.0,24.0,2.0,17.0,gxmin,gxmax,gymax,gymin);
			} else {
				gsaf_gdefin (4.0,24.0,2.0,17.0,gxmax,gxmin,gymax,gymin);
			}
			gsaf_gframe ();
			gsaf_gscall (gxorg,nxsl,0.0,0,scl,nxs);
			gsaf_gscall (0.0,0,gyorg,nysl,scl,nys);
			gsaf_gvalul (gxorg,nxvl,0.0,0,nxv);
			gsaf_gvalul (0.0,0,gyorg,nyvl,nyv);
			gsaf_gplotp (x,y,0,50,1,nm,10,np);
			gsaf_pagee ();
			}

		} else if (mode == 8) {
			gsaf_pages ();
			gsaf_setfnt (2);
			gsaf_gnumbr (5.0,15.0, 30.0 ,0,1);
			gsaf_gnumbr (5.0,14.0, 30.0, 1,1);
			gsaf_gnumbr (5.0,13.0, 30.0,-1,1);
			gsaf_gnumbr (5.0,12.0,  3.0 ,0,1);
			gsaf_gnumbr (5.0,11.0,  3.0, 1,1);
			gsaf_gnumbr (5.0,10.0,  3.0,-1,1);
			gsaf_gnumbr (5.0, 9.0,  0.0 ,0,1);
			gsaf_gnumbr (5.0, 8.0,  0.0, 1,1);
			gsaf_gnumbr (5.0, 7.0,  0.0,-1,1);
			gsaf_gnumbr (5.0, 6.0, -3.0 ,0,1);
			gsaf_gnumbr (5.0, 5.0, -3.0, 1,1);
			gsaf_gnumbr (5.0, 4.0, -3.0,-1,1);
			gsaf_gnumbr (5.0, 3.0,-30.0 ,0,1);
			gsaf_gnumbr (5.0, 2.0,-30.0, 1,1);
			gsaf_gnumbr (5.0, 1.0,-30.0,-1,1);
			gsaf_gnumbr (8.0,15.0, 30.0 ,0,0);
			gsaf_gnumbr (8.0,14.0, 30.0, 1,0);
			gsaf_gnumbr (8.0,13.0, 30.0,-1,0);
			gsaf_gnumbr (8.0,12.0,  3.0 ,0,0);
			gsaf_gnumbr (8.0,11.0,  3.0, 1,0);
			gsaf_gnumbr (8.0,10.0,  3.0,-1,0);
			gsaf_gnumbr (8.0, 9.0,  0.0 ,0,0);
			gsaf_gnumbr (8.0, 8.0,  0.0, 1,0);
			gsaf_gnumbr (8.0, 7.0,  0.0,-1,0);
			gsaf_gnumbr (8.0, 6.0, -3.0 ,0,0);
			gsaf_gnumbr (8.0, 5.0, -3.0, 1,0);
			gsaf_gnumbr (8.0, 4.0, -3.0,-1,0);
			gsaf_gnumbr (8.0, 3.0,-30.0 ,0,0);
			gsaf_gnumbr (8.0, 2.0,-30.0, 1,0);
			gsaf_gnumbr (8.0, 1.0,-30.0,-1,0);
			gsaf_gtext (15.0,15.0,"123",0);
			gsaf_gtext (15.0,14.0,"123",1);
			gsaf_gtext (15.0,13.0,"12",0);
			gsaf_gtext (15.0,12.0,"12",1);
			gsaf_gtext (15.0,11.0,"1",0);
			gsaf_gtext (15.0,10.0,"1",1);
			gsaf_inqtsz ("123",&s);
			gsaf_gnumbr (15.0, 8.0,s,-3,0);
			gsaf_inqtsz ("12",&s);
			gsaf_gnumbr (15.0, 7.0,s,-3,0);
			gsaf_inqtsz ("1",&s);
			gsaf_gnumbr (15.0, 6.0,s,-3,0);  
			gsaf_pagee ();

		} else if (mode == 9) {
			gsaf_pages ();
			gsaf_setchs (0.3,0.0);
			gsaf_gaxis2 (2.0,2.0,14.0, 2.0,0.0,10.0,0.0,1.0,0.2,0);
			gsaf_gaxis2 (2.0,2.0,12.0, 8.0,0.0,10.0,0.0,1.0,0.2,1);
			gsaf_gaxis2 (2.0,2.0, 8.0,12.0,0.0,10.0,0.0,1.0,0.2,2);
			gsaf_gaxis2 (2.0,2.0, 2.0,14.0,0.0,10.0,0.0,1.0,0.2,3);
			gsaf_gaxiv2 (2.0,2.0,14.0, 2.0,0.0,10.0,0.0,2.0,1, 0);
			gsaf_gaxiv2 (2.0,2.0,12.0, 8.0,0.0,10.0,0.0,2.0,1, 4);
			gsaf_gaxiv2 (2.0,2.0, 8.0,12.0,0.0,10.0,0.0,2.0,1, 8);
			gsaf_gaxiv2 (2.0,2.0, 2.0,14.0,0.0,10.0,0.0,2.0,1,12);
			gsaf_pagee ();
		}
	}
 EXIT:
	gsaf_gsclos ();
	return 0;
}

