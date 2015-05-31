#include "gsaf.h"
#include "gsgl.h"
#include "bsc1.h"
#include "text_hersey.h"
#include "text_hersey_data.h"
#include "text_type1_size.h"
#include "base.h"

void gs_grchar (float *x, float *y, int ic, int ind, int idim, gsglPage *p)
{
	const int nc1 = 0, nc2 = 2000;
	float tfxx,tfxy,tfyx,tfyy,tflx,tfly,tfsp,tfspx,tfspy;
	int xygrid[401];
	gsglboolean lmv;
	int iloc, idat, ilen;
	float dl;
	int iybas, iymax, ixmin;
	int ix0, iy0;
	int i;
	int ifntr;
	int nc;
	float xt, yt, dlx;
	float xf, yf;
	float xn, yn;

	gsgl_inq_fntreal (&ifntr, p);
	gsgl_inq_font_scale_info (&tfxx, &tfxy, &tfyx, &tfyy, &tflx, &tfly,
				  &tfsp, &tfspx, &tfspy, p);
	gs_grconv (ic,ifntr,&nc);
	if (nc < nc1 || nc > nc2) return;

	gsgl_hersey_index (nc, &iloc);
	if (iloc == 0) return;

	gsgl_hersey_buffer (iloc, &xygrid[1]);
	i=0;
	
	do {
		i=i+2;
		iloc = iloc + 1;
		gsgl_hersey_buffer (iloc, &idat);
		xygrid[i  ]=idat/128-64;
		xygrid[i+1]=idat%128-64;
	} while (xygrid[i+1] != -64);

	ilen=i+1;

	iybas=xygrid[2];
	iymax=xygrid[3];
	ixmin=xygrid[4];

	dl=tfly/(iymax-iybas);
	if (ind == 0) {
		ix0 = ixmin;
		iy0 = iybas;
	} else {
		ix0 = 0;
		iy0 = 0;
	}

	lmv = GSGL_TRUE;
	for (i=6; i<=ilen; i+=2) {
		if (xygrid[i] == -64) {
			lmv=GSGL_TRUE;
		} else {
			xf=(xygrid[i  ]-ix0)*dl;
			yf=(xygrid[i+1]-iy0)*dl;
			xn=tfxx*xf+tfxy*yf;
			yn=tfyx*xf+tfyy*yf;
			xt = *x+xn;
			yt = *y+yn;
			if (lmv) {
				gsgl_move (xt,yt,p);
			} else {
				if (idim == 1) {
					gsgl_draw (xt,yt,p);
				} else if (idim == 2) {
					gsgl_draw_text2d (xt,yt,p);
				} else if (idim == 3) { 
					gsgl_draw_text3d (xt,yt,p);
				}
			}
			lmv=GSGL_FALSE;
		}
	}
	gs_grsize(ic, &dlx, p);
	xn=tfxx*dlx+tfspx;
	yn=tfyx*dlx+tfspy;
	*x = *x + xn;
	*y = *y + yn;
}

void gs_grsize(int ic, float *dlx, gsglPage *p)
{
	const int nc1 = 0,nc2 = 2000;
	float tfxx,tfxy,tfyx,tfyy,tflx,tfly,tfsp,tfspx,tfspy;
	int width;
	int xygrid[5];
	int iybas, iymax, ixmin, ixmax;
	int ifnt;
	int nc;
	int i;
	int idat;
	int iloc;
	float dl;
	int id;

	gsaf_inqfnt (&ifnt);
	gsgl_inq_font_scale_info (&tfxx, &tfxy, &tfyx, &tfyy, &tflx, &tfly,
				  &tfsp, &tfspx, &tfspy, p);
	
	if (ifnt <= 1) {
		*dlx=tfly;
	} else if (ifnt < 32) {
		gs_grconv(ic,ifnt,&nc);
		if (nc < nc1 || nc >nc2) {
			*dlx=0.0;
		} else {
			gsgl_hersey_index (nc, &iloc);
			if (iloc == 0) {
				*dlx=0.0;
			} else {
				gsgl_hersey_buffer (iloc, &xygrid[0]);
				i=0;
				i=i+1;
				iloc = iloc + 1;
				gsgl_hersey_buffer (iloc, &idat);
				xygrid[i  ]=idat/128-64;
				xygrid[i+1]=idat%128-64;
				i=i+2;
				iloc = iloc + 1;
				gsgl_hersey_buffer (iloc, &idat);
				xygrid[i  ]=idat/128-64;
				xygrid[i+1]=idat%128-64;

				iybas=xygrid[1];
				iymax=xygrid[2];
				ixmin=xygrid[3];
				ixmax=xygrid[4];

				dl=tfly/(iymax-iybas);
				*dlx=(ixmax-ixmin)*dl;
			}
		}
	} else if (ifnt <= 44) {
		if (ic < 32) {
			*dlx=0.0;
		} else {
			if (ifnt < 40) {
				id=ifnt-31;
				width = type1_width (ic,id);
				*dlx=width/600.0*tfly*(8.0/9.0);
			} else if (ifnt < 44) {
				*dlx=tfly;
			} else {
				id=ifnt-35;
				width = type1_width (ic,id);
				*dlx=width/600.0*tfly*(8.0/9.0);
			}
		}
	} else {
		*dlx=0.0;
	}
}

void gs_grconv(int ic1, int id, int *nc)
{
	const static short ia1[128] = {
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		400,214,228,375,219,371,218,217,221,222,223,225,211,224,210,220,
		200,201,202,203,204,205,206,207,208,209,212,213,325,226,326,215,
		373,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
		16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26,323,229,324,347,350,
		216,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,
		116,117,118,119,120,121,122,123,124,125,126,341,329,342,346,400};
	const static short ia2[128] = {
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		400,214,217,375,274,371,272,251,221,222,219,232,211,231,210,220,
		200,201,202,203,204,205,206,207,208,209,212,213,325,238,326,215,
		373,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
		16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26,323,218,324,347,350,
		252,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,
		116,117,118,119,120,121,122,123,124,125,126,341,329,342,346,400};
	const static short ia3[128] = {
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		400,214,217,375,274,371,272,251,221,222,219,232,211,231,210,220,
		200,201,202,203,204,205,206,207,208,209,212,213,325,238,326,215,
		373,  1,  2, 22,  4,  5, 21,  3,  7,  9,108, 10, 11, 12, 13, 15,
		16,  8, 17, 18, 19, 20,161, 24, 14, 23,  6,151,152,153,154,155,
		252,101,102,122,104,105,160,103,107,109,121,110,111,112,113,115,
		116,159,117,118,119,120,158,124,114,123,106,165,166,167,168,169};
	int ic, nd, md;

	if (ic1 >= 128) {
		ic = 0;
	} else {
		ic = ic1;
	}
	if (id == 2) {
		nd=ia2[ic];
		md=nd/100;
		nd=nd%100;
		if (md == 0) {
			*nc=     nd;
		} else if (md == 1) {
			*nc= 100+nd;
		} else if (md == 2) {
			*nc= 200+nd;
		} else if (md == 3) {
			*nc= 200+nd;
		} else {
			*nc= 199;
		}
	} else if (id == 3) {
		nd=ia2[ic];
		md=nd/100;
		nd=nd%100;
		if (md == 0 && nd != 0) {
			*nc=  50+nd;
		} else if (md == 1) {
			*nc= 150+nd;
		} else if (md == 2) {
			*nc= 200+nd;
		} else if (md == 3) {
			*nc= 200+nd;
		} else if (md == 4) {
			*nc= 199;
		} else {
			*nc= 0;
		}
	} else if (id == 4) {
		nd=ia1[ic];
		md=nd/100;
		nd=  nd % 100;
		if (md == 0) {
			*nc=1000+nd;
		} else if (md == 1) {
			*nc=1100+nd;
		} else if (md == 2) {
			*nc=1200+nd;
		} else if (md == 3) {
			*nc= 200+nd;
		} else {
			*nc= 199;
		}
	} else if (id == 5) {
		nd=ia1[ic];
		md=nd/100;
		nd= nd % 100;
		if (md == 0) {
			*nc=1050+nd;
		} else if (md == 1) {
			*nc=1150+nd;
		} else if (md == 2) {
			*nc=1250+nd;
		} else if (md == 3) {
			*nc= 200+nd;
		} else {
			*nc= 199;
		}
	} else if (id == 6) {
		nd=ia1[ic];
		md=nd/100;
		nd= nd % 100;
		if (md == 0) {
			*nc= 500+nd;
		} else if (md == 1) {
			*nc= 600+nd;
		} else if (md == 2) {
			*nc= 700+nd;
		} else if (md == 3) {
			*nc= 200+nd;
		} else {
			*nc= 199;
		}
	} else if (id == 7) {
		nd=ia1[ic];
		md=nd/100;
		nd= nd%100;
		if (md == 0) {
			*nc= 550+nd;
		} else if (md == 1) {
			*nc= 650+nd;
		} else if (md == 2) {
			*nc= 750+nd;
		} else if (md == 3) {
			*nc= 200+nd;
		} else {
			*nc= 199;
		}
	} else if (id == 8) {
		nd=ia1[ic];
		md=nd/100;
		nd= nd%100;
		if (md == 0) {
			*nc=1300+nd;
		} else if (md == 1) {
			*nc=1400+nd;
		} else if (md == 2) {
			*nc=1700+nd;
		} else if (md == 3) {
			*nc= 200+nd;
		} else {
			*nc= 199;
		}
	} else if (id == 9) {
		nd=ia1[ic];
		md=nd/100;
		nd= nd%100;
		if (md == 0) {
			*nc=1500+nd;
		} else if (md == 1) {
			*nc=1600+nd;
		} else if (md == 2) {
			*nc=1700+nd;
		} else if (md == 3) {
			*nc= 200+nd;
		} else {
			*nc= 199;
		}
	} else if (id == 10) {
		nd=ia1[ic];
		md=nd/100;
		nd= nd%100;
		if (md == 0) {
			*nc=1800+nd;
		} else if (md == 1) {
			*nc=1900+nd;
		} else if (md == 2) {
			*nc=1700+nd;
		} else if (md == 3) {
			*nc= 200+nd;
		} else {
			*nc= 199;
		}
	} else if (id == 11) {
		nd=ia3[ic];
		md=nd/100;
		nd= nd%100;
		if (md == 0 && nd != 0) {
			*nc=  26+nd;
		} else if (md == 1) {
			*nc= 126+nd;
		} else if (md == 2) {
			*nc= 200+nd;
		} else if (md == 3) {
			*nc= 200+nd;
		} else if (md == 4) {
			*nc= 199;
		} else {
			*nc= 0;
		}
	} else if (id == 12) {
		if (ic == 32) {
			*nc=199;
		} else if (ic >32 && ic < 65) {
			*nc= 1300+ic-32;
		} else {
			*nc=0;
		}
	} else if (id == 13) {
		if (ic == 32) {
			*nc=199;
		} else if (ic >32 && ic < 65) {
			*nc= 1400+ic-32;
		} else {
			*nc=0;
		}
	} else if (id == 14) {
		if (ic == 32) {
			*nc=199;
		} else if (ic >32 && ic < 65) {
			*nc= 800+ic-32;
		} else {
			*nc=0;
		}
	} else if (id == 15) {
		if (ic == 32) {
			*nc=199;
		} else if (ic >32 && ic < 65) {
			*nc= 900+ic-32;
		} else {
			*nc=0;
		}
	} else if (id == 16) {
		if (ic == 32) {
			*nc=199;
		} else if (ic >32 && ic < 45) {
			*nc= 400+ic-32;
		} else {
			*nc=0;
		}
	} else if (id == 17) {
		if (ic == 32) {
			*nc=199;
		} else if (ic >32 && ic < 128) {
			*nc= 200+ic;
		} else {
			*nc=0;
		}
	} else if (id == 18) {
		if (ic == 32) {
			*nc=199;
		} else if (ic >32 && ic < 65) {
			*nc= 300+ic-32;
		} else {
			*nc=0;
		}
	} else if (id == 19) {
		if (ic == 32) {
			*nc=199;
		} else if (ic >32 && ic < 65) {
			*nc= 300+ic-32+50;
		} else {
			*nc=0;
		}
	}
}

void gsgl_type1_depth (int ic, float *dlx, gsglPage *p)
{
	float tfxx,tfxy,tfyx,tfyy,tflx,tfly,tfsp,tfspx,tfspy;
	int depth;
	int ifnt;
	int id;

	gsaf_inqfnt (&ifnt);
	gsgl_inq_font_scale_info (&tfxx, &tfxy, &tfyx, &tfyy, &tflx, &tfly, &tfsp, &tfspx,
				  &tfspy, p);

	if (ifnt>=32 && ifnt <= 44) {
		id=ifnt-31;
		depth = type1_depth (ic,id);
		if (ifnt < 40) {
			*dlx=depth/600.0*tfly*(8.0/9.0);
		} else if (ifnt < 44) { 
			*dlx=depth/600.0*tfly;
		} else {
			*dlx=depth/600.0*tfly*(8.0/9.0);
		}
	} else {
		*dlx=0.0;
	}
}

void gsgl_type1_height (int ic, float *dlx, gsglPage *p)
{
	float tfxx,tfxy,tfyx,tfyy,tflx,tfly,tfsp,tfspx,tfspy;
	int height;
	int ifnt;
	int id;

	gsaf_inqfnt (&ifnt);
	gsgl_inq_font_scale_info (&tfxx, &tfxy, &tfyx, &tfyy, &tflx, &tfly,
				  &tfsp, &tfspx, &tfspy, p);

	if (ifnt>=32 && ifnt <= 44) {
		id=ifnt-31;
		height = type1_height (ic,id);
		if (ifnt < 40) {
			*dlx=height/600.0*tfly*(8.0/9.0);
		} else if (ifnt < 44) { 
			*dlx=height/600.0*tfly;
		} else {
			*dlx=height/600.0*tfly*(8.0/9.0);
		}
	} else {
		*dlx=0.0;
	}
}

void gsgl_1000_scale_factor_to_cm (int ifnt, float din, float *dout, gsglPage *p)
{
	float tfxx,tfxy,tfyx,tfyy,tflx,tfly,tfsp,tfspx,tfspy;
	/*float scale;*/

	gsgl_inq_font_scale_info (&tfxx, &tfxy, &tfyx, &tfyy, &tflx, &tfly,
				  &tfsp, &tfspx, &tfspy, p);
	
	if (ifnt>=32 && ifnt <= 44) {
		if (ifnt < 40) {
			*dout=din/600.0*tfly*(8.0/9.0);
		} else if (ifnt < 44) { 
			*dout=din/600.0*tfly;
		} else {
			*dout=din/600.0*tfly*(8.0/9.0);
		}
	} else if (ifnt>=500 && ifnt < 1000) {
		if (ifnt < 510) {
			*dout=din/600.0*tfly*(8.0/9.0);
		} else if (ifnt < 514) { 
			*dout=din/600.0*tfly;
		} else {
			*dout=din/600.0*tfly*(8.0/9.0);
		}
	} else if (ifnt>=1000) {
		*dout = din/600.0*tfly*(8.0/9.0);
	} else {
		*dout = din/600.0*tfly*(8.0/9.0);   
	}
	/*gsgl_inq_projection_scale (&scale);*/
	/**dout = (*dout) * scale;*/
}
