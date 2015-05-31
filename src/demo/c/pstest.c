#include "gsgl.h"

int main ()
{
	int nd3[4] = {32,33,36,40};
	int n;
	float y, d;
	int ifnt;

	gsaf_gsopen ();
	gsaf_pages ();

	for (n=0; n<=12; n++) {
		gsaf_move(0.5,16.2-n*1.2);
		gsaf_setfnt(0);
		gsaf_setchs(0.5,0.0);
		gsaf_numbi(n, "%2d",2);
		gsaf_setfnt(n);
		gsaf_setchs(0.8,0.0);
		gsaf_move(2.0,16.2-n*1.2);
		gsaf_text("ABCDEFGH");
		gsaf_move(12.5,16.2-n*1.2);
		gsaf_text("abcdefgh");
		gsaf_move(21.0,16.2-n*1.2);
		gsaf_text("1234");
	}
	gsaf_pagee ();

	gsaf_pages ();
	for (n=0; n<=12; n++) {
		gsaf_move(0.5,16.2-n*1.2);
		gsaf_setfnt(0);
		gsaf_setchs(0.5,0.0);
		gsaf_numbi(n+32,"%2d",2);
		gsaf_setfnt(n+32);
		gsaf_setchs(0.8,0.0);
		gsaf_move(2.0,16.2-n*1.2);
		gsaf_text("ABCDEFGH");
		gsaf_move(12.5,16.2-n*1.2);
		gsaf_text("abcdefgh");
		gsaf_move(21.0,16.2-n*1.2);
		gsaf_text("1234");
	}
	gsaf_pagee ();

	gsaf_pages ();
	y=18.0;
	for (n=1; n<=4; n++) {
		y=y-4.0;
		gsaf_setfnt(nd3[n-1]);
		gsaf_setchs(0.3,0.0);
		gsaf_move( 3.0,y+2.4);
		gsaf_text("NORMAL$$+SUPER$$=N.");
		gsaf_move(13.0,y+2.4);
		gsaf_text("NORMAL$+SUPER$=N.");
		gsaf_move( 3.0,y+1.8);
		gsaf_text("NORMAL$$-SUB$$=N.");
		gsaf_move(13.0,y+1.8);
		gsaf_text("NORMAL$-SUB$=N.");
		gsaf_move( 3.0,y+1.2);
		gsaf_text("NORMAL $$#abc$$# N.");
		gsaf_move(13.0,y+1.2);
		gsaf_text("NORMAL $#abc$# N.");
		gsaf_move( 3.0,y+0.6);
		gsaf_text("NORMAL $$0abc$$# N.");
		gsaf_move(13.0,y+0.6);
		gsaf_text("NORMAL $0abc$# N.");
		gsaf_move( 3.0,y);
		gsaf_text("NORMAL$$,A$$;B$$!C$$?DEF$$*BACK.");
		gsaf_move(13.0,y);
		gsaf_text("NORMAL$,A$;B$!C$?DEF$*BACK.");
	}
	gsaf_move( 3.0,1.0);
	gsaf_text("$$# $$/264123123 $$#");
	gsaf_move(13.0,1.0);
	gsaf_text("$# $/264123123 $#");
	gsaf_pagee ();

	d=12345.0;
	ifnt=32;
	gsaf_pages ();
	gsaf_setvew(1.0,2.0,1.0,2.0,1.0,3.0,1.0,3.0);
	gsaf_setfnt(ifnt);
         for (n=0; n<=3; n++) {
		 gsaf_gnumbr( 1.0,17.0-n,d,n,0);
		 gsaf_gnumbr(10.0,17.0-n,d,n,1);
		 gsaf_gnumbr(15.0,17.0-n,d,n,2);
		 gsaf_gnumbr(20.0,17.0-n,d,n,3);
         }
         for (n=0; n<=3; n++) {
		 gsaf_gnumbr( 1.0,12.0-n,d,-n,0);
		 gsaf_gnumbr(10.0,12.0-n,d,-n,1);
		 gsaf_gnumbr(15.0,12.0-n,d,-n,2);
		 gsaf_gnumbr(20.0,12.0-n,d,-n,3);
         }
         for (n=0; n<=3; n++) {
		 gsaf_gnumbr( 1.0, 7.0-n,d,-n-100,0);
		 gsaf_gnumbr(10.0, 7.0-n,d,-n-100,1);
		 gsaf_gnumbr(15.0, 7.0-n,d,-n-100,2);
		 gsaf_gnumbr(20.0, 7.0-n,d,-n-100,3);
         }
	 gsaf_pagee ();

	 ifnt=36;
	 gsaf_pages ();
         gsaf_setfnt(ifnt);
         for (n=0; n<=3; n++) {
		 gsaf_gnumbr( 1.0,17.0-n,d,n,0);
		 gsaf_gnumbr(10.0,17.0-n,d,n,1);
		 gsaf_gnumbr(15.0,17.0-n,d,n,2);
		 gsaf_gnumbr(20.0,17.0-n,d,n,3);
         }
         for (n=0; n<=3; n++) {
		 gsaf_gnumbr( 1.0,12.0-n,d,-n,0);
		 gsaf_gnumbr(10.0,12.0-n,d,-n,1);
		 gsaf_gnumbr(15.0,12.0-n,d,-n,2);
		 gsaf_gnumbr(20.0,12.0-n,d,-n,3);
         }
         for (n=0; n<=3; n++) {
		 gsaf_gnumbr( 1.0, 7.0-n,d,-n-100,0);
		 gsaf_gnumbr(10.0, 7.0-n,d,-n-100,1);
		 gsaf_gnumbr(15.0, 7.0-n,d,-n-100,2);
		 gsaf_gnumbr(20.0, 7.0-n,d,-n-100,3);
         }
	 gsaf_pagee ();

	 gsaf_gsclos ();
	 return 0;
}
