#include "gsgl.h"
#include <stdio.h>

int main ()
{
	int nd1[] = {0, 2, 3, 4, 5, 6, 7, 8, 9,10};
	int nd2[] = {0,11,12,13,14,15,16,17,18,19};
	int nd3[] = {0,2,4,6};
	int i, n;
	int ifnt;
	float chh, chw, chsp, angl, tilt;
	float x, y;
	int nx, ny;
	float d, dl;
	int ic;
	char str[2];
	int error;

	gsaf_gsopen ();
	gsaf_pages ();

	for (n=0; n<=11; n++) {
		gsaf_move(0.5,16.2-n*1.4);
		gsaf_setfnt(0);
		gsaf_setchs(0.5,0.0);
		gsaf_numbi(n, "%2d", 2);
		gsaf_setfnt(n);
		gsaf_setchs(1.0,0.0);
		gsaf_move (2.0,16.1-n*1.4);
		gsaf_text ("abcdefgh");
		gsaf_move (12.5,16.1-n*1.4);
		gsaf_text ("abcdefgh");
		gsaf_move (21.0,16.1-n*1.4);
		gsaf_text ("1234");
	}
	gsaf_pagee ();

	gsaf_pages ();
	gsaf_setchs(0.3,0.0);

	for (n=1; n<=10; n++) {
		ifnt = nd1[n-1];
		gsaf_move(0.5,19.2-n*1.8);
		gsaf_setfnt(0);
		gsaf_numbi(ifnt, "%2d", 2);
		gsaf_setfnt(ifnt);
		for (i=32; i<=63; i++) {
			gsaf_move(2.0+0.5*(i-32),19.2-n*1.8);
			sprintf (str, "%c", i);
			gsaf_text (str);
		}
		for (i=64; i<=95; i++) {
			gsaf_move(2.0+0.5*(i-64),18.7-n*1.8);
			sprintf (str, "%c", i);
			gsaf_text (str);
		}
		for (i=96; i<=127; i++) {
			gsaf_move(2.0+0.5*(i-96),18.2-n*1.8);
			sprintf (str, "%c", i);
			gsaf_text (str);
		}
	}
	gsaf_pagee ();

	gsaf_pages ();
	gsaf_setchs(0.3,0.0);

	for (n=1; n<=10; n++) {
		ifnt = nd2[n-1];
		gsaf_move (0.5,19.2-n*1.8);
		gsaf_setfnt (0);
		gsaf_numbi (ifnt, "%2d", 2);
		gsaf_setfnt (ifnt);
		for (i=32; i<=63; i++) {
			gsaf_move(2.0+0.5*(i-32),19.2-n*1.8);
			sprintf (str, "%c", i);
			gsaf_text (str);
		}
		for (i=64; i<=95; i++) {
			gsaf_move(2.0+0.5*(i-64),18.7-n*1.8);
			sprintf (str, "%c", i);
			gsaf_text (str);
		}
		for (i=96; i<=127; i++) {
			gsaf_move(2.0+0.5*(i-96),18.2-n*1.8);
			sprintf (str, "%c", i);
			gsaf_text (str);
		}
	}
	gsaf_pagee ();

	gsaf_pages ();
	y=18.0;

	for (n=1; n<=4; n++) {
		y=y-4.0;
		gsaf_setfnt (nd3[n-1]);
		gsaf_setchs (0.3,0.0);
		gsaf_move ( 3.0,y+2.4);
		gsaf_text ("NORMAL$$+SUPER$$=N.");
		gsaf_move (13.0,y+2.4);
		gsaf_text ("NORMAL$+SUPER$=N.");
		gsaf_move ( 3.0,y+1.8);
		gsaf_text ("NORMAL$$-SUB$$=N.");
		gsaf_move (13.0,y+1.8);
		gsaf_text ("NORMAL$-SUB$=N.");
		gsaf_move ( 3.0,y+1.2);
		gsaf_text ("NORMAL $$#ABC$$# N.");
		gsaf_move (13.0,y+1.2);
		gsaf_text ("NORMAL $#ABC$# N.");
		gsaf_move ( 3.0,y+0.6);
		gsaf_text ("NORMAL $$0ABC$$# N.");
		gsaf_move (13.0,y+0.6);
		gsaf_text ("NORMAL $0ABC$# N.");
		gsaf_move ( 3.0,y);
		gsaf_text ("NORMAL$$,A$$;B$$!C$$?DEF$$*BACK.");
		gsaf_move (13.0,y);
		gsaf_text ("NORMAL$,A$;B$!C$?DEF$*BACK.");
	}
	gsaf_pagee ();

	d=12345.0;
	ifnt=0;
	gsaf_pages ();
	gsaf_inqchr (&chh,&chw,&chsp,&angl,&tilt);
	gsaf_setchr (chh,chw,chsp,20.0,tilt);
	gsaf_setvew (1.0,2.0,1.0,2.0,1.0,3.0,1.0,3.0);
	gsaf_move( 1.0, 1.0);
	gsaf_draw( 1.0,17.0);
	gsaf_move(10.0, 1.0);
	gsaf_draw(10.0,17.0);
	gsaf_move(15.0, 1.0);
	gsaf_draw(15.0,17.0);
	gsaf_move(20.0, 1.0);
	gsaf_draw(20.0,17.0);
	gsaf_setfnt (ifnt);

	for (n=0; n<=3; n++) {
		gsaf_gnumbr( 1.0,16.0-n,d,n,0);
		gsaf_gnumbr(10.0,16.0-n,d,n,1);
		gsaf_gnumbr(15.0,16.0-n,d,n,2);
		gsaf_gnumbr(20.0,16.0-n,d,n,3);
	}
	for (n=0; n<=3; n++) {
		gsaf_gnumbr( 1.0,11.0-n,d,-n,0);
		gsaf_gnumbr(10.0,11.0-n,d,-n,1);
		gsaf_gnumbr(15.0,11.0-n,d,-n,2);
		gsaf_gnumbr(20.0,11.0-n,d,-n,3);
	}
	for (n=0; n<=3; n++) {
		gsaf_gnumbr( 1.0, 6.0-n,d,-n-100,0);
		gsaf_gnumbr(10.0, 6.0-n,d,-n-100,1);
		gsaf_gnumbr(15.0, 6.0-n,d,-n-100,2);
		gsaf_gnumbr(20.0, 67.0-n,d,-n-100,3);
	}
	gsaf_pagee ();

	ifnt=2;
	gsaf_pages ();
	gsaf_setvew (1.0,2.0,1.0,2.0,1.0,3.0,1.0,3.0);
	gsaf_inqchr (&chh,&chw,&chsp,&angl,&tilt);
	gsaf_setchr (chh,chw,chsp,20.0,-20.0);
	gsaf_move ( 1.0, 1.0);
	gsaf_draw ( 1.0,17.0);
	gsaf_move (10.0, 1.0);
	gsaf_draw (10.0,17.0);
	gsaf_move (15.0, 1.0);
	gsaf_draw (15.0,17.0);
	gsaf_move (20.0, 1.0);
	gsaf_draw (20.0,17.0);
	gsaf_setfnt (ifnt);
	for (n=0; n<=3; n++) {
		gsaf_gnumbr ( 1.0,16.0-n,d,n,0);
		gsaf_gnumbr (10.0,16.0-n,d,n,1);
		gsaf_gnumbr (15.0,16.0-n,d,n,2);
		gsaf_gnumbr (20.0,16.0-n,d,n,3);
	}
	for (n=0; n<=3; n++) {
		gsaf_gnumbr ( 1.0,11.0-n,d,-n,0);
		gsaf_gnumbr (10.0,11.0-n,d,-n,1);
		gsaf_gnumbr (15.0,11.0-n,d,-n,2);
		gsaf_gnumbr (20.0,11.0-n,d,-n,3);
	}
	for (n=0; n<=3; n++) {
		gsaf_gnumbr ( 1.0, 6.0-n,d,-n-100,0);
		gsaf_gnumbr (10.0, 6.0-n,d,-n-100,1);
		gsaf_gnumbr (15.0, 6.0-n,d,-n-100,2);
		gsaf_gnumbr (20.0, 6.0-n,d,-n-100,3);
	}
	gsaf_pagee ();

	x=4.0;
	y=1.0;
	ifnt=2;
	ic=0;
	dl=1.0;
	angl=0.0;
	tilt=0.0;

	for (;;) {
		do {
			error = gsgl_stdin (" # INPUT : IFNT,IC,DL,ANGL,TILT ?\n", 5, 
					    "%d%*c%d%*c%f%*c%f%*c%f",&ifnt,&ic,&dl,&angl,&tilt);
			if (error == EOF) goto EXIT;
		} while (error == 1);
		
		if (ic == 0) {
			gsaf_pages ();
			gsaf_move (0.0,17.5);
			gsaf_text ("grfont: ifnt =");
			gsaf_numbi (ifnt,"%5d", 5);
			gsaf_setchr (dl,dl/1.5,dl,angl,tilt);
			gsaf_setfnt (ifnt);
			for (ny=0; ny<=7; ny++) {
				for (nx=0; nx<=15; nx++) {
					gsaf_move(x+nx*1.3,y+15.0-ny*1.5);
					sprintf (str, "%c", nx+ny*16);
					gsaf_text (str);
				}
			}
			gsaf_pagee ();
		} else {
			gsaf_pages ();
			gsaf_move (0.0,17.5);
			gsaf_text ("grfont: ifnt =");
			gsaf_numbi (ifnt, "%5d", 5);
			gsaf_setchr (dl,dl/1.5,dl,angl,tilt);
			gsaf_setfnt (ifnt);
			gsaf_move (5.0,5.0);
			sprintf (str, "%c", ic);
			gsaf_text (str);
			gsaf_pagee ();
		}
	}
 EXIT:
	gsaf_gsclos ();
	return 0;
}
