#include "gsgl.h"
#include <stdio.h>

int main ()
{
	float chh;
	int n;
	int ifnt;
	int i;
	char str[2];

	gsaf_gsopen ();

	gsaf_pages ();
	chh = 0.55;
	gsaf_setchs (chh, 0.0);

	for (n=1; n<=4; n++) {
		ifnt = 501 + n;
		gsaf_setfnt(ifnt);
		if (ifnt == 502) {
			gsaf_move (0.5, 20.2-3.7*1);
			gsaf_textx ("@Times Roman (502)@");
		} else if (ifnt == 503) {
			gsaf_move (0.5, 20.2-3.7*2);
			gsaf_textx ("@Times Italic (503)@");
		} else if (ifnt == 504) {
			gsaf_move (0.5, 20.2-3.7*3);
			gsaf_textx ("@Times Bold (504)@");
		} else if (ifnt == 505) {
			gsaf_move (0.5, 20.2-3.7*4);
			gsaf_textx ("@Times Italic-Bold (505)@");
		}
		for (i=32; i<=63; i++) {
			gsaf_move(1.0+chh*1.05*(i-32),19.2-n*3.7);
			sprintf (str, "%c", i);
			gsaf_text (str);
		}
		for (i=64; i<=95; i++) {
			gsaf_move(1.0+chh*1.05*(i-64),18.5-n*3.7);
			sprintf (str, "%c", i);
			gsaf_text (str);
		}
		for (i=96; i<=127; i++) {
			gsaf_move(1.0+chh*1.05*(i-96),17.8-n*3.7);
			sprintf (str, "%c", i);
			gsaf_text (str);
		}
	}
	gsaf_pagee ();

	gsaf_pages ();
	chh = 0.55;
	gsaf_setchs (chh, 0.0);

	for (n=1; n<=4; n++) {
		ifnt = 505 + n;
		gsaf_setfnt(ifnt);
		if (ifnt == 506) {
			gsaf_move (0.5, 20.2-3.7*1);
			gsaf_textx ("@helvetica (506)@");
		} else if (ifnt == 507) {
			gsaf_move (0.5, 20.2-3.7*2);
			gsaf_textx ("@helvetica Oblique (507)@");
		} else if (ifnt == 508) {
			gsaf_move (0.5, 20.2-3.7*3);
			gsaf_textx ("@helvetica Bold (508)@");
		} else if (ifnt == 509) {
			gsaf_move (0.5, 20.2-3.7*4);
			gsaf_textx ("@helvetica BoldOblique (509)@");
		}
		for (i=32; i<=63; i++) {
			gsaf_move(1.0+chh*1.05*(i-32),19.2-n*3.7);
			sprintf (str, "%c", i);
			gsaf_text (str);
		}
		for (i=64; i<=95; i++) {
			gsaf_move(1.0+chh*1.05*(i-64),18.5-n*3.7);
			sprintf (str, "%c", i);
			gsaf_text (str);
		}
		for (i=96; i<=127; i++) {
			gsaf_move(1.0+chh*1.05*(i-96),17.8-n*3.7);
			sprintf (str, "%c", i);
			gsaf_text (str);
		}
	}
	gsaf_pagee ();

	gsaf_pages ();
	chh = 0.55;
	gsaf_setchs (chh, 0.0);

	for (n=1; n<=4; n++) {
		ifnt = 509 + n;
		gsaf_setfnt(ifnt);
		if (ifnt == 510) {
			gsaf_move (0.5, 20.2-3.7*1);
			gsaf_textx ("@Courier (510)@");
		} else if (ifnt == 511) {
			gsaf_move (0.5, 20.2-3.7*2);
			gsaf_textx ("@Courier Oblique (511)@");
		} else if (ifnt == 512) {
			gsaf_move (0.5, 20.2-3.7*3);
			gsaf_textx ("@Courier Bold (512)@");
		} else if (ifnt == 513) {
			gsaf_move (0.5, 20.2-3.7*4);
			gsaf_textx ("@Courier BoldOblique (513)@");
		}
		for (i=32; i<=63; i++) {
			gsaf_move(1.0+chh*1.05*(i-32),19.2-n*3.7);
			sprintf (str, "%c", i);
			gsaf_text (str);
		}
		for (i=64; i<=95; i++) {
			gsaf_move(1.0+chh*1.05*(i-64),18.5-n*3.7);
			sprintf (str, "%c", i);
			gsaf_text (str);
		}
		for (i=96; i<=127; i++) {
			gsaf_move(1.0+chh*1.05*(i-96),17.8-n*3.7);
			sprintf (str, "%c", i);
			gsaf_text (str);
		}
	}
	gsaf_pagee ();

	gsaf_pages ();
	chh = 0.55;
	gsaf_setchs (chh, 0.0);

	for (n=1; n<=1; n++) {
		ifnt = 513 + n;
		gsaf_setfnt(ifnt);
		if (ifnt == 514) {
			gsaf_move (0.5, 20.2-3.7*1);
			gsaf_textx ("@Symbol (514)@");
		}
		for (i=32; i<=63; i++) {
			gsaf_move(1.0+chh*1.05*(i-32),19.2-n*3.7);
			sprintf (str, "%c", i);
			gsaf_text (str);
		}
		for (i=64; i<=95; i++) {
			gsaf_move(1.0+chh*1.05*(i-64),18.5-n*3.7);
			sprintf (str, "%c", i);
			gsaf_text (str);
		}
		for (i=96; i<=127; i++) {
			gsaf_move(1.0+chh*1.05*(i-96),17.8-n*3.7);
			sprintf (str, "%c", i);
			gsaf_text (str);
		}
	}
	gsaf_pagee ();

	gsaf_pages ();
	chh = 0.55;
	gsaf_setchs (chh, 0.0);

	for (n = 1; n<=4; n++) {
		ifnt = 1001 + n;
		gsaf_setfnt(ifnt);
		if (ifnt == 1002) {
			gsaf_move (0.5, 20.2-3.7*1);
			gsaf_textx ("@Times Roman (1002)@");
		} else if (ifnt == 1003) {
			gsaf_move (0.5, 20.2-3.7*2);
			gsaf_textx ("@Times Italic (1003)@");
		} else if (ifnt == 1004) {
			gsaf_move (0.5, 20.2-3.7*3);
			gsaf_textx ("@Times Bold (1004)@");
		} else if (ifnt == 1005) {
			gsaf_move (0.5, 20.2-3.7*4);
			gsaf_textx ("@Times Italic-Bold (1005)@");
		}
		for (i=32; i<=63; i++) {
			gsaf_move(1.0+chh*1.05*(i-32),19.2-n*3.7);
			sprintf (str, "%c", i);
			gsaf_text (str);
		}
		for (i=64; i<=95; i++) {
			gsaf_move(1.0+chh*1.05*(i-64),18.5-n*3.7);
			sprintf (str, "%c", i);
			gsaf_text (str);
		}
		for (i=96; i<=127; i++) {
			gsaf_move(1.0+chh*1.05*(i-96),17.8-n*3.7);
			sprintf (str, "%c", i);
			gsaf_text (str);
		}
	}
	gsaf_pagee ();

	gsaf_pages ();
	chh = 0.55;
	gsaf_setchs (chh, 0.0);

	for (n=1; n<=4; n++) {
		ifnt = 1005 + n;
		gsaf_setfnt(ifnt);
		if (ifnt == 1006) {
			gsaf_move (0.5, 20.2-3.7*1);
			gsaf_textx ("@Arial Roman (1006)@");
		} else if (ifnt == 1007) {
			gsaf_move (0.5, 20.2-3.7*2);
			gsaf_textx ("@Arial Italic (1007)@");
		} else if (ifnt == 1008) {
			gsaf_move (0.5, 20.2-3.7*3);
			gsaf_textx ("@Arial Bold (1008)@");
		} else if (ifnt == 1009) {
			gsaf_move (0.5, 20.2-3.7*4);
			gsaf_textx ("@Arial Italic-Bold (1009)@");
		}
		for (i=32; i<=63; i++) {
			gsaf_move(1.0+chh*1.05*(i-32),19.2-n*3.7);
			sprintf (str, "%c", i);
			gsaf_text (str);
		}
		for (i=64; i<=95; i++) {
			gsaf_move(1.0+chh*1.05*(i-64),18.5-n*3.7);
			sprintf (str, "%c", i);
			gsaf_text (str);
		}
		for (i=96; i<=127; i++) {
			gsaf_move(1.0+chh*1.05*(i-96),17.8-n*3.7);
			sprintf (str, "%c", i);
			gsaf_text (str);
		}
	}
	gsaf_pagee ();

	gsaf_pages ();
	chh = 0.55;
	gsaf_setchs (chh, 0.0);

	for (n=1; n<=4; n++) {
		ifnt = 1009 + n;
		gsaf_setfnt(ifnt);
		if (ifnt == 1010) {
			gsaf_move (0.5, 20.2-3.7*1);
			gsaf_textx ("@Courier Roman (1010)@");
		} else if (ifnt == 1011) {
			gsaf_move (0.5, 20.2-3.7*2);
			gsaf_textx ("@Courier Italic (1011)@");
		} else if (ifnt == 1012) {
			gsaf_move (0.5, 20.2-3.7*3);
			gsaf_textx ("@Courier Bold (1012)@");
		} else if (ifnt == 1013) {
			gsaf_move (0.5, 20.2-3.7*4);
			gsaf_textx ("@Courier Italic-Bold (1013)@");
		}
		for (i=32; i<=63; i++) {
			gsaf_move(1.0+chh*1.05*(i-32),19.2-n*3.7);
			sprintf (str, "%c", i);
			gsaf_text (str);
		}
		for (i=64; i<=95; i++) {
			gsaf_move(1.0+chh*1.05*(i-64),18.5-n*3.7);
			sprintf (str, "%c", i);
			gsaf_text (str);
		}
		for (i=96; i<=127; i++) {
			gsaf_move(1.0+chh*1.05*(i-96),17.8-n*3.7);
			sprintf (str, "%c", i);
			gsaf_text (str);
		}
	}
	gsaf_pagee ();

	gsaf_pages ();
	chh = 0.55;
	gsaf_setchs (chh, 0.0);

	for (n=1; n<=4; n++) {
		ifnt = 1014 + n;
		gsaf_setfnt(ifnt);
		if (ifnt == 1015) {
			gsaf_move (0.5, 20.2-3.7*1);
			gsaf_textx ("@Greek Roman (1015)@");
		} else if (ifnt == 1016) {
			gsaf_move (0.5, 20.2-3.7*2);
			gsaf_textx ("@Greek Italic (1016)@");
		} else if (ifnt == 1017) {
			gsaf_move (0.5, 20.2-3.7*3);
			gsaf_textx ("@Greek Bold (1017)@");
		} else if (ifnt == 1018) {
			gsaf_move (0.5, 20.2-3.7*4);
			gsaf_textx ("@Greek Italic-Bold (1018)@");
		}
		for (i=32; i<=63; i++) {
			gsaf_move(1.0+chh*1.05*(i-32),19.2-n*3.7);
			sprintf (str, "%c", i);
			gsaf_text (str);
		}
		for (i=64; i<=95; i++) {
			gsaf_move(1.0+chh*1.05*(i-64),18.5-n*3.7);
			sprintf (str, "%c", i);
			gsaf_text (str);
		}
		for (i=96; i<=127; i++) {
			gsaf_move(1.0+chh*1.05*(i-96),17.8-n*3.7);
			sprintf (str, "%c", i);
			gsaf_text (str);
		}
	}
	gsaf_pagee ();

	gsaf_pages ();
	chh = 0.55;
	gsaf_setchs (chh, 0.0);

	gsaf_setfnt (1020);
	gsaf_move (0.5, 20.2-3.7*1);
	gsaf_text ("日本語明朝体 (1020)");
	gsaf_move (1.0, 19.2-3.7*1);
	gsaf_text ("あいうえおかきくけこさしすせそたちつてとなにぬねの");
	gsaf_move (1.0, 18.2-3.7*1);
	gsaf_text ("はひふへほまみむめもやいゆえよらりるれろわゐゑをん");
	gsaf_move (1.0, 17.2-3.7*1);
	gsaf_text ("アイウエオカキクケコサシスセソタチツテトナニヌネノ");
	gsaf_move (1.0, 16.2-3.7*1);
	gsaf_text ("ハヒフヘホマミムメモヤイユエヨラリルレロワヰヱヲン");
	gsaf_move (1.0, 15.2-3.7*1);
	gsaf_text ("微分積分線形代数古典力学電磁気解析力学確率統計量子");
	gsaf_move (1.0, 14.2-3.7*1);
	gsaf_text ("日本語の文字数は１文字２バイトであることを考慮して");

	gsaf_setfnt (1021);
	gsaf_move (0.5, 20.2-3.7*3);
	gsaf_text ("日本語ゴシック体 (1021)");
	gsaf_move (1.0, 19.2-3.7*3);
	gsaf_text ("あいうえおかきくけこさしすせそたちつてとなにぬねの");
	gsaf_move (1.0, 18.2-3.7*3);
	gsaf_text ("はひふへほまみむめもやいゆえよらりるれろわゐゑをん");
	gsaf_move (1.0, 17.2-3.7*3);
	gsaf_text ("アイウエオカキクケコサシスセソタチツテトナニヌネノ");
	gsaf_move (1.0, 16.2-3.7*3);
	gsaf_text ("ハヒフヘホマミムメモヤイユエヨラリルレロワヰヱヲン");
	gsaf_move (1.0, 15.2-3.7*3);
	gsaf_text ("微分積分線形代数古典力学電磁気解析力学確率統計量子");
	gsaf_move (1.0, 14.2-3.7*3);
	gsaf_text ("日本語の文字数は１文字２バイトであることを考慮して");
	gsaf_pagee ();

	gsaf_pages ();
	chh = 0.55;
	gsaf_setchs (chh, 0.0);

	for (n=1; n<=4; n++) {
		ifnt = 1021 + n;
		gsaf_setfnt(ifnt);
		if (ifnt == 1022) {
			gsaf_move (0.5, 20.2-3.7*1);
			gsaf_textx ("@Computer Modern Roman (1022)@");
		} else if (ifnt == 1023) {
			gsaf_move (0.5, 20.2-3.7*2);
			gsaf_textx ("@Computer Modern Text Italic (1023)@");
		} else if (ifnt == 1024) {
			gsaf_move (0.5, 20.2-3.7*3);
			gsaf_textx ("@Computer Modern Typewriter Type (1024)@");
		} else if (ifnt == 1025) {
			gsaf_move (0.5, 20.2-3.7*4);
			gsaf_textx ("@Computer Modern Math Italic (1025)@");
		}
		for (i=32; i<=63; i++) {
			gsaf_move(1.0+chh*1.05*(i-32),19.2-n*3.7);
			sprintf (str, "%c", i);
			gsaf_text (str);
		}
		for (i=64; i<=95; i++) {
			gsaf_move(1.0+chh*1.05*(i-64),18.5-n*3.7);
			sprintf (str, "%c", i);
			gsaf_text (str);
		}
		for (i=96; i<=127; i++) {
			gsaf_move(1.0+chh*1.05*(i-96),17.8-n*3.7);
			sprintf (str, "%c", i);
			gsaf_text (str);
		}
	}
	gsaf_pagee ();

	gsaf_pages ();
	chh = 0.55;
	gsaf_setchs (chh, 0.0);

	n = 1;
	ifnt = 1025 + n;
	gsaf_setfnt(ifnt);
	gsaf_move (0.5, 20.2-4.0*1);
	gsaf_textx ("@$0Computer Modern Math Symbols (1026)@");
	for (i=32; i<=63; i++) {
		gsaf_move(1.0+chh*1.05*(i-32),19.2-n*4.0);
		sprintf (str, "%c", i);
		gsaf_text (str);
	}
	for (i=64; i<=95; i++) {
		gsaf_move(1.0+chh*1.05*(i-64),18.5-n*4.0);
		sprintf (str, "%c", i);
		gsaf_text (str);
	}
	for (i=96; i<=127; i++) {
		gsaf_move(1.0+chh*1.05*(i-96),17.8-n*4.0);
		sprintf (str, "%c", i);
		gsaf_text (str);
	}
	n = 2;
	ifnt = 1025 + n;
	gsaf_setfnt(ifnt);
	gsaf_move (0.5, 20.2-4.0*2);
	gsaf_textx ("@$0Computer Modern Math Extension (1027)@");
	for (i=32; i<=63; i++) {
		gsaf_move(1.0+chh*1.05*(i-32),19.2-n*4.0);
		sprintf (str, "%c", i);
		gsaf_text (str);
	}
	for (i=64; i<=95; i++) {
		gsaf_move(1.0+chh*1.05*(i-64),16.5-n*4.0);
		sprintf (str, "%c", i);
		gsaf_text (str);
	}
	for (i=96; i<=127; i++) {
		gsaf_move(1.0+chh*1.05*(i-96),13.8-n*4.0);
		sprintf (str, "%c", i);
		gsaf_text (str);
	}
	gsaf_pagee ();

	gsaf_pages ();
	chh = 0.55;
	gsaf_setchs (chh, 0.0);

	gsaf_setfnt (1000);
	gsaf_move (1.0, 19.2-2.7*1);
	gsaf_text ("$$sqrt{abc}=$sqrt{abc}");
	gsaf_move (1.0, 18.2-2.7*1);
	gsaf_text ("$$frac{a}{b}=$frac{a}{b}");
	gsaf_move (1.0, 17.2-2.7*1);
	gsaf_setchr (chh, chh, chh, 0.0, -20.0);
	gsaf_text ("tilt=-20.0 abcdefg");
	gsaf_move (1.0, 16.2-2.7*1);
	gsaf_setchr (chh, chh, chh, -45.0, 0.0);
	gsaf_text ("angl=-45.0 abcdefg");
	gsaf_move (1.0, 15.2-2.7*1);
	gsaf_setchr (chh, chh, chh, 90.0, 0.0);
	gsaf_move (4.0, 4.0);
	gsaf_text ("y(x) = $frac{$del}{$del x}$left($frac{$del e}{$del x}$right)");
	gsaf_pagee ();

	gsaf_pages ();
	chh = 0.55;
	gsaf_setchs (chh, 0.0);

	gsaf_setfnt (1020);
	gsaf_move (1.0, 19.2-3.7*1);
	gsaf_text ("日本語用やギリシャ語用のフォントが設定されている状態で");
	gsaf_move (1.0, 18.2-3.7*1);
	gsaf_text ("英数字が使われた場合，1000番のフォントが使われる．ま");
	gsaf_move (1.0, 17.2-3.7*1);
	gsaf_text ("た，英語やギリシャ語用のフォントが設定されていて日本語が");
	gsaf_move (1.0, 16.2-3.7*1);
	gsaf_text ("使われた場合1019番のフォントが使われる．ギリシャ文字");
	gsaf_move (1.0, 15.2-3.7*1);
	gsaf_text ("を表示するには$$#で囲えばよい．例えば$$#abc$$#と入力す");
	gsaf_move (1.0, 14.2-3.7*1);
	gsaf_text ("ると$#abc$#と表示される．$${14}abcと入力しても同じ結果が");
	gsaf_move (1.0, 13.2-3.7*1);
	gsaf_text ("得られる．添え字を表示するには$$+または$$-で始め，$$=で");
	gsaf_move (1.0, 12.2-3.7*1);
	gsaf_text ("終わる．例えばx$$+2$$=と入力すればx$+2$=となり，x$$-1$$=と");
	gsaf_move (1.0, 11.2-3.7*1);
	gsaf_text ("入力するとx$-1$=となる．x$$+2$$=$$-1$$=と入力すればx$+2$=$-1$= となる．");
	gsaf_move (1.0, 10.2-3.7*1);
	gsaf_text ("添え字は何重にも表示できて，x$-a$-b$-c$=$=$= ということもできる．");
	gsaf_pagee ();

	gsaf_gsclos ();
	return 0;
}
