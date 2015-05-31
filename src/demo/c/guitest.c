#include "gsgl.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

/*
  C
  C     ******************************
  C     ****** BSC TEST PROGRAM ******
  C     ******************************
  C
*/

static void buttons ();
static void texts ();
void xtest ();
void xclip ();
void xmark ();
void xline ();
void xtext ();
void xchar (char kid);

float rgb[3];
int   fnt;
char *text = NULL;
float chs = 0.35;
int   bb = 0;

static void clicked ()
{
	gsaf_setrgb (rgb[0], rgb[1], rgb[2]);
	gsaf_gtextx (10.0,10.0,"@Clicked!!@",0);
}

static void redraw ()
{
	gsaf_setrgb (rgb[0], rgb[1], rgb[2]);
	xmark ();	
}

static void clear_redraw ()
{
	gsaf_eras ();	
	gsaf_setrgb (rgb[0], rgb[1], rgb[2]);
	xmark ();	
}

static void drawtext (const char *text1, int *i)
{
	float size, height, depth;

	gsaf_eras ();	
	if (text1 && *i) {
		gsaf_setfnt (fnt);
		gsaf_setchr (chs, chs * 0.16 / 0.35, chs, 0.0, 0.0);
		
		if (text) free(text);
		text = strdup(text1);
		if (bb) {
			gsaf_inqtextbbox (text, &size, &height, &depth);
			gsaf_move (15.0, 10.0+depth);
			gsaf_draw (15.0+size, 10.0+depth);
			gsaf_draw (15.0+size, 10.0+height);
			gsaf_draw (15.0, 10.0+height);
			gsaf_draw (15.0, 10.0+depth);
		}
		gsaf_gtext (15.0, 10.0, text, 0);
	}
}

static void boundingbox (int *i)
{
	char *text2;
	int l;

	if (*i)
		bb = 1;
	else
		bb = 0;
	
	if (text) {
		text2 = strdup(text);
		l = strlen(text2);
		drawtext (text2, &l);
		free (text2);
	}
}

static void clear ()
{
	gsaf_eras ();
}

static void setrgb (int *i)
{
	if (*i == 0) {
		rgb[0] = 0.0;
		rgb[1] = 0.0;
		rgb[2] = 0.0;
	} else if (*i == 1) {
		rgb[0] = 1.0;
		rgb[1] = 0.0;
		rgb[2] = 0.0;
	} else if (*i == 2) {
		rgb[0] = 0.0;
		rgb[1] = 0.0;
		rgb[2] = 1.0;
	} else if (*i == 3) {
		rgb[0] = 1.0;
		rgb[1] = 1.0;
		rgb[2] = 0.0;
	} else if (*i == 4) {
		rgb[0] = 0.0;
		rgb[1] = 1.0;
		rgb[2] = 0.0;
	}
}

static void setfnt (float *i)
{
	char *text2;
	int l;
	fnt = (int) (*i + 0.5);

	if (text) {
		text2 = strdup(text);
		l = strlen(text2);
		drawtext (text2, &l);
		free (text2);
	}
}

static void setchs (float *c)
{
	char *text2;
	int l;

	chs = *c;

	if (text) {
		text2 = strdup(text);
		l = strlen(text2);
		drawtext (text2, &l);
		free (text2);
	}
}

int main ()
{
	char mode;
	int error;

	gsaf_gsopen ();

	for (;;) {
		do {
			error = gsgl_stdin (" # INPUT B/BUTTONS T/TEXT\n", 1, "%c", &mode);
			if (error == EOF) goto EXIT;
		} while (error == 1 || error == 2 || (mode != 'b' && mode != 't'));
		
		if (mode == 'b')
			buttons ();
		else if (mode == 't')
			texts ();
	}

 EXIT:
	gsaf_gsclos ();
	return 0;
}

static void buttons ()
{
	char *menu[] = {"black", "red", "blue", "yellow", "green"};
	float w, h;

	gsaf_pages ();

	rgb[0] = 0.0;
	rgb[1] = 0.0;
	rgb[2] = 0.0;
	
	redraw ();
	
	fnt = 2;
	gsaf_setfnt (2);
	gsaf_setchr (0.35, 0.16, 0.35, 0.0, 0.0);

	gsaf_guibutton (1.0, 17.0, 2.0, 1.0, "click", clicked);
	gsaf_guibutton (1.0, 16.0, 2.0, 1.0, "redraw", redraw);
	gsaf_guibutton (1.0, 15.0, 2.0, 1.0, "clear", clear);
	gsaf_inqlabelsize ("clear and redraw", &w, &h);
	gsaf_guibutton (1.0, 14.0, w+0.5, 1.0, "clear and redraw", clear_redraw);
	gsaf_guilabel (1.0, 13.0, 2.0, 1.0, "color = ", 0);
	gsaf_guicombobox (3.0, 13.0, 2.0, 1.0, 5, menu, 0, setrgb, 0);

	/*gsaf_guiexternal (1);

	gsaf_guibutton (1.0, 7.0, 2.0, 1.0, "click", clicked);
	gsaf_guibutton (1.0, 6.0, 2.0, 1.0, "redraw", redraw);
	gsaf_guibutton (1.0, 5.0, 2.0, 1.0, "clear", clear);
	gsaf_inqlabelsize ("clear and redraw", &w, &h);
	gsaf_guibutton (1.0, 4.0, w+0.5, 1.0, "clear and redraw", clear_redraw);
	gsaf_guilabel (1.0, 3.0, 2.0, 1.0, "color = ", 0);
	gsaf_guicombobox (3.0, 3.0, 2.0, 1.0, 5, menu, 0, setrgb, 0);*/

	gsaf_pagee ();
}

static void texts ()
{
#define TEXT0 "$frac{A}{B}1234"
	
	float w, h;
	int l;
	fnt = 0;

	gsaf_pages ();
	fnt = 2;
	gsaf_setchr (0.35, 0.1, 0.35, 0.0, 0.0);

	l = strlen(TEXT0);
	drawtext (TEXT0, &l);

	gsaf_inqlabelsize ("draw bounding box", &w, &h);
	gsaf_guicheckbutton (1.0, 13.0, w+1.0, 1.0, "draw bounding box", 0,
			     boundingbox);
	/*NULL);*/
	gsaf_guilineedit (1.0, 17.0, 6.0, 1.0, 40, TEXT0, drawtext);

	gsaf_inqlabelsize ("Font Number = #", &w, &h);
	gsaf_guilabel (1.0, 15.0, w, 1.0, "Font Number = #", 0);
	gsaf_guispinbuttonf (1.1+w, 15.0, 2.0, 1.0, 0.0, 0.0, 19.0, 0, setfnt);
	gsaf_inqlabelsize ("Font Size = ", &w, &h);
	gsaf_guilabel (1.0, 14.0, w, 1.0, "Font Size = ", 0);
	gsaf_guispinbuttonf (1.1+w, 14.0, 2.0, 1.0, 0.35, 0.0, 10.0, 2, setchs);
	gsaf_guilabel (3.2+w, 14.0, 1.0, 1.0, "cm", 0);

	gsaf_pagee ();
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
