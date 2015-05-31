#include "gsgl.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

/*
  C
  C     ******************************
  C     ****** BSC TEST PROGRAM ******
  C     ******************************
  C
*/

void xtest ();
void xclip ();
void xmark ();
void xline ();
void xtext ();
void xchar (char kid);

int main ()
{
	float xmin, xmax, ymin, ymax, xcmin, xcmax, ycmin, ycmax;
	char kmenu;
	int error;

	gsaf_gsopen ();

	xmin=0.0;
	xmax=25.6;
	ymin=0.0;
	ymax=18.1;
	xcmin=0.0;
	xcmax=25.6;
	ycmin=0.0;
	ycmax=18.1;

	for (;;) {
		do {
			error = gsgl_stdin (" # INPUT : M/MARK  T/TEXT  L/LINE  S,H,F/CHAR\n"
					    "           V/VIEW  C/CLIP  E/END\n",
					    1, " %c", &kmenu);
		} while (error == 1 || error == 2);
		if (error == EOF) break;
		
		kmenu = tolower(kmenu);

		if (kmenu == 'm') {
			gsaf_pages ();
			gsaf_setvew (0.0,25.6,0.0,18.1,xmin,xmax,ymin,ymax);
			gsaf_setclp (xcmin,xcmax,ycmin,ycmax);
			xmark ();
			gsaf_pagee ();
		} else if (kmenu == 't') {
			gsaf_pages ();
			gsaf_setvew (0.0,25.6,0.0,18.1,xmin,xmax,ymin,ymax);
			gsaf_setclp (xcmin,xcmax,ycmin,ycmax);
			xtext ();
			gsaf_pagee ();
		} else if (kmenu == 'l') {
			gsaf_pages ();
			gsaf_setvew (0.0,25.6,0.0,18.1,xmin,xmax,ymin,ymax);
			gsaf_setclp (xcmin,xcmax,ycmin,ycmax);
			xline ();
			gsaf_pagee ();
		} else if (kmenu == 'h' || kmenu == 's' || kmenu == 'f') {
			gsaf_pages ();
			gsaf_setvew (0.0,25.6,0.0,18.1,xmin,xmax,ymin,ymax);
			gsaf_setclp (xcmin,xcmax,ycmin,ycmax);
			xchar(kmenu);
			gsaf_pagee ();
		} else if (kmenu == 'v') {
			do {
			    error = gsgl_stdin (" # INPUT : XMIN,XMAX,YMIN,YMAX\n",
						4, "%f%*c%f%*c%f%*c%f",
						&xmin,&xmax,&ymin,&ymax);
			    if (error == EOF) goto EXIT;
			} while (error == 1);

			gsaf_pages ();
			gsaf_setvew (0.0,25.6,0.0,18.1,xmin,xmax,ymin,ymax);
			gsaf_setclp (xcmin,xcmax,ycmin,ycmax);
			xtest ();
			gsaf_pagee ();
		} else if (kmenu == 'c') {
			do {
			    error = gsgl_stdin (" # INPUT : XMIN,XMAX,YMIN,YMAX\n",
						4, "%f%*c%f%*c%f%*c%f",
						&xcmin,&xcmax,&ycmin,&ycmax);
			    if (error == EOF) goto EXIT;
			} while (error == 1);

			gsaf_pages ();
			gsaf_setvew (0.0,25.6,0.0,18.1,xmin,xmax,ymin,ymax);
			gsaf_setclp (xcmin,xcmax,ycmin,ycmax);
			xclip ();
			gsaf_pagee ();
		} else if (kmenu == 'e') {
			break;
		}
	}
 EXIT:
	gsaf_gsclos ();
	return 0;
}

void xtest ()
{
	gsaf_move (1.0,17.1);
	gsaf_draw (1.0,1.0);
	gsaf_draw (24.6,1.0);
	gsaf_move (5.,5.);
	gsaf_draw (10.,10.);
	gsaf_setchr (1.,1.,2.,0.,0.);
	gsaf_text ("GRAPHIC");
	gsaf_setchs(0.30,-90.);
	gsaf_text ("GRAPHIC");
	gsaf_setchs(0.30,-180.);
	gsaf_text ("GRAPHIC");
}

void xclip ()
{
	int n = 100;
	int i;
	float x1, y1, x2, y2;

	for (i=0; i<n; i++) {
		x1=gsaf_gurand(25.0);
		y1=gsaf_gurand(20.0);
		gsaf_move (x1,y1);
		x2=gsaf_gurand(25.0);
		y2=gsaf_gurand(20.0);
		gsaf_draw (x2,y2);
	}
}

void xmark ()
{
	float deg = deg=3.141593/180.0;
	float r1=1.0;
	float r2=8.0;
	int i;
	float angl, ca, sa;

	for (i=-31; i<=31; i++) {
		angl=180.0*i/32.0;
		gsaf_setmrk (i,0.6,0.6,angl-90.0,0.0);
		ca=cos(angl*deg);
		sa=sin(angl*deg);
		gsaf_move (r1*ca+12.8,r1*sa+9.0);
		gsaf_mark (r2*ca+12.8,r2*sa+9.0);
	}
}

void xtext ()
{
	int i, j;
	float x, y;
	char c;
	char str[2] = {'\0','\0'};

	gsaf_setchs(0.4,0.0);//

	for (i=2; i<=7; i++) {
		for (j=0; j<=15; j++) {
			x=j+5.0;
			y=18.0-i;
			gsaf_move (x,y);
			c = 16*i + j;
			str[0] = c;
			gsaf_text (str);
		}
	}
	
	gsaf_setchs(0.4,0.0);

	for (i=0; i<=7; i++) {
		for (j=0; j<=15; j++) {
			x=j+5.0;
			y=9.0-i;
			gsaf_move (x,y);
			c = 16*i + j;
			str[0] = c;
			gsaf_text (str);
		}
	}
}

void xline ()
{
	int iln, ibl, icl;
	float x, y;
	
	for (iln=0; iln<=7; iln++) {
		for (ibl=0; ibl<=4; ibl++) {
			for (icl=0; icl<=7; icl++) {
				x=2*icl+5;
				y=2*iln+0.3*ibl+1;
				gsaf_setlin(iln,ibl,icl);
				gsaf_move (x,y);
				gsaf_draw (x+1.5,y);
			}
		}
	}
}

void xchar (char kid)
{
	int i;
	float x, y, chh, chw, angl, tilt;

	if (kid == 's') {
		gsaf_setfnt(0);
	} else if (kid == 'h') {
		gsaf_setfnt(0);
	} else {
		gsaf_setfnt(2);
	}
	
	for (i=1; i<=13; i++) {
		angl=30*(i-1);
		gsaf_setchr (0.6,0.4,0.6,angl,0.0);
		x=i+6.0;
		y=15.0;
		gsaf_move (x,y);
		gsaf_text ("A");
	}

	for (i=1; i<=13; i++) {
		tilt=8*(i-7);
		gsaf_setchr (0.6,0.4,0.6,0.0,tilt);
		x=i+6.0;
		y=13.0;
		gsaf_move (x,y);
		gsaf_text ("A");
	}

	for (i=1; i<=20; i++) {
		chh=0.05*(i-1);
		gsaf_setchr (chh,0.6666667,0.6,0.0,0.0);
		x=i+3.0;
		y=11.0;
		gsaf_move (x,y);
		gsaf_text ("A");
	}

	for (i=1; i<=20; i++) {
		chw=0.05*(i-1);
		gsaf_setchr (1.0,chw*0.6666667,0.6,0.0,0.0);
		x=i+3.0;
		y=9.0;
		gsaf_move (x,y);
		gsaf_text ("A");
	}

	for (i=1; i<=20; i++) {
		chh=0.05*(i-1);
		gsaf_setchr (chh,chh*0.6666667,0.6,0.0,0.0);
		x=i+3.0;
		y=7.0;
		gsaf_move (x,y);
		gsaf_text ("A");
	}
	
	gsaf_setchr (0.6,0.4,0.6,0.0,0.0);
	gsaf_move (3.0,5.0);
	gsaf_text ("NORMAL$$+SUPER$$=N.");
	gsaf_move (14.0,5.0);
	gsaf_text ("NORMAL$+SUPER$=N.");
	gsaf_move (3.0,3.5);
	gsaf_text ("NORMAL$$-SUB$$=N.");
	gsaf_move (14.0,3.5);
	gsaf_text ("NORMAL$-SUB$=N.");
	gsaf_move (3.0,2.0);
	gsaf_text ("NORMAL$$#ABC$$#N.");
	gsaf_move (14.0,2.0);
	gsaf_text ("NORMAL$#ABC$#N.");
	gsaf_move (3.0,0.5);
	gsaf_text ("NORMAL$$*BACK.");
	gsaf_move (14.0,0.5);
	gsaf_text ("NORMAL$*BACK.");
}
