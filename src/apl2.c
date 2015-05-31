#include "gsaf.h"
#include "gsgl.h"
#include "base.h"
#include "apl1.h"
#include "apl2.h"
#include "bsc1.h"
#include "bsc2d.h"
#include "utils.h"
#include "matrix.h"

gsglboolean gsgl_inq_gdefin_is_called (gsglPage *p)
{
	return p->TwoDInfo->Lgf;
}

void gsaf_inqgdefin (float *pxs,float *pxe, float *pys, float *pye,
		     float *gxs,float *gxe, float *gys, float *gye)
{
	gsglPage *p;

	if (!gsgl_inq_page_is_open ()) return;
	p = gsgl_inq_page ();

	*pxs = p->TwoDInfo->Pxs;
	*pys = p->TwoDInfo->Pys;
	*pxe = p->TwoDInfo->Pxe;
	*pye = p->TwoDInfo->Pye;
	*gxs = p->TwoDInfo->Gxs;
	*gys = p->TwoDInfo->Gys;
	*gxe = p->TwoDInfo->Gxe;
	*gye = p->TwoDInfo->Gye;
}

#define SET_F77_COMMON_GSGFX_F77 F77_FUNC_(set_f77_common_gsgfx,SET_F77_COMMON_GSGFX)
void SET_F77_COMMON_GSGFX_F77 (float *PXS1,float *PYS1,float *PXE1,float *PYE1,
			       float *GXS1,float *GYS1,float *GXE1,float *GYE1,
			       int *LGF1);

/* define graph size */
void gsaf_gdefin (float pxmin, float pxmax, float pymin, float pymax,
		  float gxmin, float gxmax, float gymin, float gymax)
{
	gsglPage *p;
	gsgl2dinfo *info;

	if (!gsgl_inq_page_is_open ()) return;
	p = gsgl_inq_page ();
	info = p->TwoDInfo;

	gsgl_init_hold_text_position (p);

	if (fabs(gxmax-gxmin) < 1.e-32 || fabs(gymax-gymin) < 1.e-32) {
		gsgl_warning ("gdefin", 
			      "GXMIN,GXMAX,GYMIN,GYMAX=%15.7E,%15.7E,%15.7E,%15.7E",
			      gxmin,gxmax,gymin,gymax);
		info->Lgf = GSGL_FALSE;
		SET_F77_COMMON_GSGFX_F77 (&pxmin, &pymin, &pxmax, &pymax,
					  &gxmin, &gymin, &gxmax, &gymax, &info->Lgf);
		return;
	}
	if (pxmin > pxmax || pymin > pymax) {
		gsgl_warning ("gdefin", 
			      "PXMIN,PXMAX,PYMIN,PYMAX=%15.7E,%15.7E,%15.7E,%15.7E",
			      pxmin,pxmax,pymin,pymax);
		info->Lgf = GSGL_FALSE;
		SET_F77_COMMON_GSGFX_F77 (&pxmin, &pymin, &pxmax, &pymax,
					  &gxmin, &gymin, &gxmax, &gymax, &info->Lgf);
		return;
	}

	info->Dx=(pxmax-pxmin)/(gxmax-gxmin);
	info->Dy=(pymax-pymin)/(gymax-gymin);
	info->Pxs=pxmin;
	info->Pys=pymin;
	info->Pxe=pxmax;
	info->Pye=pymax;
	info->Gxs=gxmin;
	info->Gys=gymin;
	info->Gxe=gxmax;
	info->Gye=gymax;
	info->Lgf=GSGL_TRUE;
	SET_F77_COMMON_GSGFX_F77 (&pxmin, &pymin, &pxmax, &pymax,
				  &gxmin, &gymin, &gxmax, &gymax, &info->Lgf);

	gsgl_set_2d_coord (pxmax,pxmin,pymax,pymin,gxmax,gxmin,gymax,gymin,p);
}

/* draw graph frame */
void gsaf_gframe ()
{
	gsglPage *p;

	if (!gsgl_inq_page_is_open ()) return;
	p = gsgl_inq_page ();
	if (!gsgl_inq_gdefin_is_called (p)) return;

	gsaf_move2d (p->TwoDInfo->Gxs, p->TwoDInfo->Gys);
	gsaf_draw2d (p->TwoDInfo->Gxe, p->TwoDInfo->Gys);
	gsaf_draw2d (p->TwoDInfo->Gxe, p->TwoDInfo->Gye);
	gsaf_draw2d (p->TwoDInfo->Gxs, p->TwoDInfo->Gye);
	gsaf_draw2d (p->TwoDInfo->Gxs, p->TwoDInfo->Gys);
}

/* draw ticks on axis */
void gsaf_gscale (float xorg, float xstep, float yorg, float ystep, float sleng, int ind)
{
	const float eps = 0.01f;
	gsglboolean lzero, lin, lout, lbot, ltop, lnbot, lntop;
	int ipat;
	float dpx, px0;
	float dpy, py0;
	int ns, ne;
	int i;
	float px, py, px1, px2, py1, py2;
	float scl;
	int n;
	float Pxs, Pxe, Pys, Pye;
	float Gxs, Gxe, Gys, Gye;
	float dx, dy;
	gsglPage *p;
	if (!gsgl_inq_page_is_open ()) return;

	p = gsgl_inq_page ();
	if (!gsgl_inq_gdefin_is_called (p)) return;

	lzero = ind%16 == 0;
	lin = ind%2 == 1;
	lout = (ind/2)%2 == 1;
	lbot = ((ind+4)/4)%2 == 1;
	ltop = ((ind+4)/8)%2 == 1;
	lnbot = (ind/16)%2 == 1;
	lntop = (ind/32)%2 == 1;

	if (lzero) {
		ipat=(int) (sleng+0.5);
	} else {
		ipat=0;
	}

	gsaf_inqgdefin (&Pxs, &Pxe, &Pys, &Pye, &Gxs, &Gxe, &Gys, &Gye);
	dx=(Pxe-Pxs)/(Gxe-Gxs);
	dy=(Pye-Pys)/(Gye-Gys);

	dpx = fabs(dx * xstep);
	if (dpx > eps && (Gxe-xorg)*(xorg-Gxs) >= 0.0) {
		px0 = dx * (xorg - Gxs) + Pxs;
		if (lnbot) {
			ns = (int) ((Pxs-px0+eps)/dpx);
		} else {
			ns = (int) ((Pxs-px0-eps)/dpx);
		}
		if (lntop) {
			ne = (int) ((Pxe-px0-eps)/dpx);
		} else {
			ne = (int) ((Pxe-px0+eps)/dpx);
		}

		for (i=1; i<=2; i++) {
			if (i == 1) {
				if (!lbot) continue;
				py=Pys;
				scl=sleng;
			} else {
				if (!ltop) continue;
				py=Pye;
				scl=-sleng;
			}
			py1=py;
			if (lout) py1=py-scl;
			py2=py;
			if (lin) py2=py+scl;
			if (lzero) py2=Pye;

			for (n=ns; n<=ne; n++) {
				px=px0+dpx*n;
				gsaf_movept(px,py1,ipat);
				gsaf_drawpt(px,py2);
			}
		}
	}
	
	dpy=fabs(dy * ystep);
	if (dpy > eps && (Gye-yorg)*(yorg-Gys) >= 0.0) {
		py0=dy*(yorg-Gys)+Pys;
		if (lnbot) {
			ns=(int)((Pys-py0+eps)/dpy);
		} else {
			ns=(int)((Pys-py0-eps)/dpy);
		}
		if (lntop) {
			ne=(int)((Pye-py0-eps)/dpy);
		} else {
			ne=(int)((Pye-py0+eps)/dpy);
		}

		for (i=1; i<=2; i++) {
			if (i == 1) {
				if (!lbot) continue;
				px=Pxs;
				scl=sleng;
			} else {
				if (!ltop) continue;
				px=Pxe;
				scl=-sleng;
			}
			px1=px;
			if (lout) px1=px-scl;
			px2=px;
			if (lin) px2=px+scl;
			if (lzero) px2=Pxe;

			for (n=ns; n<=ne; n++) {
				py=py0+dpy*n;
				gsaf_movept(px1,py,ipat);
				gsaf_drawpt(px2,py);
			}
		}
      }
}

/* draw numbers on axis */
void gsaf_gvalue (float xorg, float xstep, float yorg, float ystep, int ind)
{
	float chh,chw,chsp,angl,tilt;
	float bxmin, bxmax, bymin, bymax;
	int ind1, ind2;
	float pos;
	float dpx, dpy;
	float xstp, ystp;
	float px0, py0, px, py, gx, gy;
	int i, n, ns, ne, ijust;
	gsglboolean lbot, ltop, lnbot, lntop;
	const float posd[4] = {2.0,1.5,1.0,0.5};
	const float eps = 0.01f;
	float Pxs, Pxe, Pys, Pye;
	float Gxs, Gxe, Gys, Gye;
	float dx, dy;
	gsglPage *p;
	if (!gsgl_inq_page_is_open ()) return;

	p = gsgl_inq_page ();
	if (!gsgl_inq_gdefin_is_called (p)) return;
	
	gsaf_inqgdefin (&Pxs, &Pxe, &Pys, &Pye, &Gxs, &Gxe, &Gys, &Gye);
	dx=(Pxe-Pxs)/(Gxe-Gxs);
	dy=(Pye-Pys)/(Gye-Gys);

	gsaf_inqchr (&chh,&chw,&chsp,&angl,&tilt);

	bxmin = Pxs;
	bxmax = Pxe;
	bymin = Pys;
	bymax = Pye;

	ind1 = abs(ind)%400;
	if (ind < 0) ind1 = -ind1;
	ind2 = abs(ind)/400*4;
	pos = posd[ind2%4+1-1];
	lbot = ((ind2+4)/4)%2 == 1;
	ltop = ((ind2+4)/8)%2 == 1;
	lnbot = (ind2/16)%2 == 1;
	lntop = (ind2/32)%2 == 1;

	dpx = fabs(dx*xstep);
	if (dpx > eps && (Gxe-xorg)*(xorg-Gxs) >= 0.0) {
		if (dx >= 0.0) {
			xstp= fabs(xstep);
		} else {
			xstp=-fabs(xstep);
		}

		px0=dx*(xorg-Gxs)+Pxs;
		if (lnbot) {
			ns=(int)((Pxs-px0+eps)/dpx);
		} else {
			ns=(int)((Pxs-px0-eps)/dpx);
		}
		if (lntop) {
			ne=(int)((Pxe-px0-eps)/dpx);
		} else {
			ne=(int)((Pxe-px0+eps)/dpx);
		}

		for (i=1; i<=2; i++) {
			if (i == 1) {
				if (!lbot) continue;
				py=Pys-0.4*(pos+2)*chh;
				bymin = py;
				ijust=3;
			} else {
				if (!ltop) continue;
				py=Pye+0.5*pos*chh;
				bymax = py+chh;
				ijust=3;
			}

			for (n=ns; n<=ne; n++) {
				gx=xorg+xstp*n;
				px=px0+dpx*n;
				gsaf_gnumbr(px,py,gx,ind1,ijust);
			}
		}
	}

	dpy=fabs(dy*ystep);
	if (dpy > eps && (Gye-yorg)*(yorg-Gys) >= 0.0) {
		if (dy >= 0.0) {
			ystp= fabs(ystep);
		} else {
			ystp=-fabs(ystep);
		}
		py0=dy*(yorg-Gys)+Pys;
		if (lnbot) {
			ns=(int)((Pys-py0+eps)/dpy);
		} else {
			ns=(int)((Pys-py0-eps)/dpy);
		}
		if (lntop) {
			ne=(int)((Pye-py0-eps)/dpy);
		} else {
			ne=(int)((Pye-py0+eps)/dpy);
		}
		
		for (i=1; i<=2; i++) {
			if (i == 1) {
				if (!lbot) continue;
				px=Pxs-0.5*pos*chw;
				ijust=1;
			} else {
				if (!ltop) continue;
				px=Pxe+0.5*pos*chw;
				ijust=0;
			}
			
			for (n=ns; n<=ne; n++) {
				gy=yorg+ystp*n;
				py=py0+dpy*n-0.5*chh;
				gsaf_gnumbr(px,py,gy,ind1,ijust);
			}
		}
	}
}

/* draw ticks on log axis */
void gsaf_gscall (float xorg, int nxstep, float yorg, int nystep, float sleng, int ind)
{
	const float scal[5][9] = {{0.0000,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0},
				  {0.0000,0.4771,1.0,1.0,1.0,1.0,1.0,1.0,1.0},
				  {0.0000,0.3010,0.6990,1.0,1.0,1.0,1.0,1.0,1.0},
				  {0.0000,0.3010,0.4771,0.6990,0.8451,1.0,1.0,1.0,1.0},
				  {0.0000,0.3010,0.4771,0.6021,0.6990,0.7782,0.8451,0.9031,0.9542}};
	int ipat;
	float px, px0, py, py0, pxsl, pxel, pysl, pyel, scl;
	int ns, ne, i, n, m, isc, nsc;
	int ixorg, iyorg;
	float dsc1, dsc2;
	float py1, py2, pxl, pyl, px1, px2;
	gsglboolean lzero, lin, lout, lbot, ltop, lnbot, lntop;
	const float eps = 0.01f;
	float Pxs, Pxe, Pys, Pye;
	float Gxs, Gxe, Gys, Gye;
	float Dx, Dy;
	gsglPage *p;
	if (!gsgl_inq_page_is_open ()) return;

	p = gsgl_inq_page ();
	if (!gsgl_inq_gdefin_is_called (p)) return;
	
	gsaf_inqgdefin (&Pxs, &Pxe, &Pys, &Pye, &Gxs, &Gxe, &Gys, &Gye);
	Dx=(Pxe-Pxs)/(Gxe-Gxs);
	Dy=(Pye-Pys)/(Gye-Gys);

	lzero = ind%16 == 0;
	lin = ind%2 == 1;
	lout = (ind/2)%2 == 1;
	lbot = ((ind+4)/4)%2 == 1;
	ltop = ((ind+4)/8)%2 == 1;
	lnbot = (ind/16)%2 == 1;
	lntop = (ind/32)%2 == 1;

	if (lzero) {
		ipat= (int) (sleng+0.5);
	} else {
		ipat=0;
	}

	if (nxstep > 0 && (Gxe-xorg)*(xorg-Gxs) >= 0.0) {
		ixorg=(int)(xorg+eps);
		px0=Dx*(ixorg-Gxs)+Pxs;
		if (lnbot) {
			pxsl=Pxs-px0+eps;
		} else {
			pxsl=Pxs-px0-eps;
		}
		if (lntop) {
			pxel=Pxe-px0-eps;
		} else {
			pxel=Pxe-px0+eps;
		}
		ns=(int)(pxsl/fabs(Dx));
		ne=(int)(pxel/fabs(Dx));
		
		for (i=1; i<=2; i++) {
			if (i == 1) {
				if (!lbot) continue;
				py=Pys;
				scl=sleng;
			} else {
				if (!ltop) continue;
				py=Pye;
				scl=-sleng;
			}
			dsc1=0.0;
			if (lout) dsc1=-scl;
			dsc2=0.0;
			if (lin)  dsc2= scl;
			if (lzero) dsc2=Pye-Pys;
			if (nxstep <= 3) {
				nsc=nxstep;
				isc=nxstep;
			} else if (nxstep <= 5) {
				nsc=5;
				isc=4;
			} else {
				nsc=9;
				isc=5;
			}
			for (n=ns-1; n<=ne; n++) {
				for (m=1; m<=nsc; m++) {
					pxl = fabs(Dx) * (n+scal[isc-1][m-1]);
					if (pxl < pxsl || pxl > pxel) break;
					if (lzero) {
						py1=py+dsc1;
						py2=py+dsc2;
					} else if (m == 1) {
						py1=py+2.0*dsc1;
						py2=py+2.0*dsc2;
					} else if ((nsc == 9 && m == 5) || 
						   (nsc == 5 && m == 4)) {
						py1=py+1.5*dsc1;
						py2=py+1.5*dsc2;
					} else {
						py1=py+dsc1;
						py2=py+dsc2;
					}
					gsaf_movept(px0+pxl,py1,ipat);
					gsaf_drawpt(px0+pxl,py2);
				}
			}
		}
	}

	if (nystep > 0 && (Gye-yorg)*(yorg-Gys) >= 0.0) {
		iyorg=(int)(yorg+eps);
		py0=Dy*(iyorg-Gys)+Pys;
		if (lnbot) {
			pysl=Pys-py0+eps;
		} else {
			pysl=Pys-py0-eps;
		}
		if (lntop) {
			pyel=Pye-py0-eps;
		} else {
			pyel=Pye-py0+eps;
		}
		ns=(int)(pysl/fabs(Dy));
		ne=(int)(pyel/fabs(Dy));

		for (i=1; i<=2; i++) {
			if (i == 1) {
				if (!lbot) continue;
				px=Pxs;
				scl=sleng;
			} else {
				if (!ltop) continue;
				px=Pxe;
				scl=-sleng;
			}
			dsc1=0.0;
			if (lout) dsc1=-scl;
			dsc2=0.0;
			if (lin) dsc2= scl;
			if (lzero) dsc2=Pxe-Pxs;
			if (nystep <= 3) {
				nsc=nystep;
				isc=nystep;
			} else if (nystep <= 5) {
				nsc=5;
				isc=4;
			} else {
				nsc=9;
				isc=5;
			}

			for (n=ns-1; n<=ne; n++) {
				for (m=1; m<=nsc; m++) {
					pyl=fabs(Dy)*(n+scal[isc-1][m-1]);
					if (pyl < pysl || pyl > pyel) break;
					if (lzero) {
						px1=px+dsc1;
						px2=px+dsc2;
					} else if (m == 1) {
						px1=px+2.0*dsc1;
						px2=px+2.0*dsc2;
					} else if ((nsc == 9 && m == 5) || 
						   (nsc == 5 && m == 4)) {
						px1=px+1.5*dsc1;
						px2=px+1.5*dsc2;
					} else {
						px1=px+dsc1;
						px2=px+dsc2;
					}
					gsaf_movept(px1,py0+pyl,ipat);
					gsaf_drawpt(px2,py0+pyl);
				}
			}
		}
	}
}

/* draw numbers on log axis */
void gsaf_gvalul (float xorg, int nxstep, float yorg, int nystep, int ind)
{
	const float posd[4] = {2.0,1.5,1.0,0.5};
	const float scal[5][9] = {{0.0000,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0},
				  {0.0000,0.4771,1.0,1.0,1.0,1.0,1.0,1.0,1.0},
				  {0.0000,0.3010,0.6990,1.0,1.0,1.0,1.0,1.0,1.0},
				  {0.0000,0.3010,0.4771,0.6990,0.8451,1.0,1.0,1.0,1.0},
				  {0.0000,0.3010,0.4771,0.6021,0.6990,0.7782,0.8451,0.9031,0.9542}};
	const int inum[5][9] = {{0,0,0,0,0,0,0,0,0},
				{0,3,0,0,0,0,0,0,0},
				{0,2,5,0,0,0,0,0,0},
				{0,2,3,5,7,0,0,0,0},
				{0,2,3,4,5,6,7,8,9}};
	float chh,chw,chsp,angl,tilt;
	int ind1, ind2;
	float pos;
	gsglboolean lbot, ltop, lnbot, lntop;
	int ixorg, iyorg;
	int ixstp, iystp;
	float px0, py0, pxsl, pysl, pxel, pyel;
	float py, pxl, px, pyl;
	int igx, igy;
	int ns, ne;
	int i, n, m;
	int ijust, nsc, isc;
	const float eps = 0.01f;
	float Pxs, Pxe, Pys, Pye;
	float Gxs, Gxe, Gys, Gye;
	float Dx, Dy;
	gsglPage *p;
	if (!gsgl_inq_page_is_open ()) return;

	p = gsgl_inq_page ();
	if (!gsgl_inq_gdefin_is_called (p)) return;
	
	gsaf_inqgdefin (&Pxs, &Pxe, &Pys, &Pye, &Gxs, &Gxe, &Gys, &Gye);
	Dx=(Pxe-Pxs)/(Gxe-Gxs);
	Dy=(Pye-Pys)/(Gye-Gys);

	gsaf_inqchr (&chh,&chw,&chsp,&angl,&tilt);

	ind1 = abs(ind)%400;
	if (ind < 0) ind1=-ind1;
	ind2 = abs(ind)/400*4;
	pos = posd[ind2%4+1-1];
	lbot = ((ind2+4)/4)%2 == 1;
	ltop = ((ind2+4)/8)%2 == 1;
	lnbot = (ind2/16)%2 == 1;
	lntop = (ind2/32)%2 == 1;

	if (nxstep > 0 && (Gxe-xorg)*(xorg-Gxs) >= 0.0) {
		ixorg=(int)(xorg+eps);
		if (Dx >= 0.0) {
			ixstp= 1;
		} else {
			ixstp=-1;
		}
		px0=Dx*(ixorg-Gxs)+Pxs;
		if (lnbot) {
			pxsl=Pxs-px0+eps;
		} else {
			pxsl=Pxs-px0-eps;
		}
		if (lntop) {
			pxel=Pxe-px0-eps;
		} else {
			pxel=Pxe-px0+eps;
		}
		ns=(int)(pxsl/fabs(Dx));
		ne=(int)(pxel/fabs(Dx));

		for (i=1; i<=2; i++) {
			if (i == 1) {
				if (!lbot) continue;
				py=Pys-0.5*(pos+2.0)*chh;
				ijust=2;
			} else {
				if (!ltop) continue;
				py=Pye+0.5*pos*chh;
				ijust=2;
			}
			if (nxstep <= 3) {
				nsc=nxstep;
				isc=nxstep;
			} else if (nxstep <= 5) {
				nsc=5;
				isc=4;
			} else {
				nsc=9;
				isc=5;
			}

			for (n=ns-1; n<=ne; n++) {
				for (m=1; m<=nsc; m++) {
					igx=ixorg+ixstp*n;
					pxl=fabs(Dx)*(n+scal[isc-1][m-1]);
					if (pxl < pxsl || pxl > pxel) break;
					if (m == 1) {
						gsaf_gnumbp(px0+pxl,py,10,igx,ijust);
					} else {
						gsaf_gnumbi(px0+pxl,py,inum[isc-1][m-1],ijust);
					}
				}
			}
		}
	}

	if (nystep > 0 && (Gye-yorg)*(yorg-Gys) >= 0.0) {
		iyorg=(int)(yorg+eps);
		if (Dy >= 0.0) {
			iystp= 1;
		} else {
			iystp=-1;
		}
		py0=Dy*(iyorg-Gys)+Pys;
		if (lnbot) {
			pysl=Pys-py0+eps;
		} else {
			pysl=Pys-py0-eps;
		}
		if (lntop) {
			pyel=Pye-py0-eps;
		} else {
			pyel=Pye-py0+eps;
		}
		ns=(int)(pysl/fabs(Dy));
		ne=(int)(pyel/fabs(Dy));

		for (i=1; i<=2; i++) {
			if (i == 1) {
				if (!lbot) continue;
				px=Pxs-0.5*pos*chw;
				ijust=1;
			} else {
				if (!ltop) continue;
				px=Pxe+0.5*pos*chw;
				ijust=0;
			}
			if (nystep <= 3) {
				nsc=nystep;
				isc=nystep;
			} else if (nystep <= 5) {
				nsc=5;
				isc=4;
			} else {
				nsc=9;
				isc=5;
			}

			for (n=ns-1; n<=ne; n++) {
				for (m=1; m<=nsc; m++) {
					igy=iyorg+iystp*n;
					pyl=fabs(Dy)*(n+scal[isc-1][m-1]);
					if (pyl < pysl || pyl > pyel) break;
					py=py0+pyl-0.5*chh;
					if (m == 1) {
						gsaf_gnumbp(px,py,10,igy,ijust);
					} else {
						gsaf_gnumbi(px,py,inum[isc-1][m-1],ijust);
					}
				}
			}
		}
	}
}

/* draw xy graph */
void gsaf_gplotp (float *gx, float *gy, int nsrt, int nend, int nstep, int imark,int istep,int ipat)
{
	int imarks;
	float hmrk,wmrk,angl,tilt;
	int n;
	gsglPage *p;

	if (!gsgl_inq_page_is_open ()) return;
	
	p = gsgl_inq_page ();
	if (!gsgl_inq_gdefin_is_called (p)) return;

	if (imark == 0) {
		gsaf_movept2d (gx[nsrt], gy[nsrt], ipat);
		if (nstep == 0) nstep = 1;
		for (n=nsrt+nstep; n<=nend; n+=nstep)
			gsaf_drawpt2d (gx[n], gy[n]);
	} else if (imark > 0) {
		gsaf_inqmrk (&imarks, &hmrk, &wmrk, &angl, &tilt);
		gsaf_setmrk (imark, hmrk, wmrk, angl, tilt);
		if (nstep == 0) nstep = 1;
		for (n=nsrt; n<=nend; n+=nstep)
			gsaf_mark2d (gx[n], gy[n]);

		gsaf_setmrk (imarks, hmrk, wmrk, angl, tilt);
	} else {
		gsaf_movept2d (gx[nsrt], gy[nsrt], ipat);
		if (nstep == 0) nstep = 1;
		for (n=nsrt+nstep; n<=nend; n+=nstep)
			gsaf_drawpt2d (gx[n], gy[n]);

		gsaf_inqmrk (&imarks, &hmrk, &wmrk, &angl, &tilt);
		gsaf_setmrk (abs(imark), hmrk, wmrk, angl, tilt);
		if (istep == 0) istep = 1;
		for (n=nsrt; n<=nend; n+=nstep*istep) 
			gsaf_mark2d(gx[n], gy[n]);

		gsaf_setmrk (imarks, hmrk, wmrk, angl, tilt);
	}
	gsgl_separate_objects (p);
}


/* draw xy errorbar graph */
void gsaf_gplotpe (float *gx, float *gy1, float *gy2, int nsrt, int nend, int nstep, float scal)
{
	int n;
	float xw1, yw1;
	float xw2, yw2;
	float scal2;
	float Pxs, Pxe, Pys, Pye, Gxs, Gxe, Gys, Gye, Dx, Dy;
	gsglPage *p;
	if (!gsgl_inq_page_is_open ()) return;

	p = gsgl_inq_page ();
	if (!gsgl_inq_gdefin_is_called (p)) return;
	
	gsaf_inqgdefin (&Pxs, &Pxe, &Pys, &Pye, &Gxs, &Gxe, &Gys, &Gye);
	Dx=(Pxe-Pxs)/(Gxe-Gxs);
	Dy=(Pye-Pys)/(Gye-Gys);

	/*call setclp(pxs,pxe,pys,pye)
	  call gugrps*/
	if (scal > 0.0) {
		gsglunproject2d (0.0, 0.0, &xw1, &yw1, p);
		gsglunproject2d (scal, 0.0, &xw2, &yw2, p);
		scal2 = sqrt((xw2-xw1)*(xw2-xw1)+(yw2-yw1)*(yw2-yw1));
	} else {
		scal2 = 0.0;
	}
	if (nstep == 0) nstep = 1;
	for (n=nsrt; n<=nend; n+=nstep) {
		/*call gugrps*/
		if (scal > 0.0) {
			gsaf_move2d (gx[n]-0.5*scal2, gy1[n]);
			gsaf_draw2d (gx[n]+0.5*scal2, gy1[n]);
			gsaf_move2d (gx[n]-0.5*scal2, gy2[n]);
			gsaf_draw2d (gx[n]+0.5*scal2, gy2[n]);
		}
		gsaf_move2d (gx[n], gy1[n]);
		gsaf_draw2d (gx[n], gy2[n]);
		/*call gugrpe*/
	}
	/*call gugrpe
	  call offclp*/
}

/* draw vector graph */
void gsaf_gvectr (float *vx, float *vy, float *x, float *y, 
		  int nxa, int nxs, int nxe, int nxstep,
		  int nys, int nye, int nystep, float xn, float yn, float al)
{
	const float root3 = 1.732051;
	float pxn, pyn;
	int nx, ny;
	float px, py, pvx, pvy, pva, acos1, asin1, acos2, asin2, pxt, pyt;
	float pxe, pye, px1, py1, px2, py2;
	float Pxs, Pxe, Pys, Pye;
	float Gxs, Gxe, Gys, Gye;
	float Dx, Dy;
	gsglPage *p;
	if (!gsgl_inq_page_is_open ()) return;

	p = gsgl_inq_page ();
	if (!gsgl_inq_gdefin_is_called (p)) return;
	
	gsaf_inqgdefin (&Pxs, &Pxe, &Pys, &Pye, &Gxs, &Gxe, &Gys, &Gye);
	Dx=(Pxe-Pxs)/(Gxe-Gxs);
	Dy=(Pye-Pys)/(Gye-Gys);

	pxn=1.0/xn;
	pyn=1.0/yn;

	if (nxstep == 0) nxstep = 1;
	if (nystep == 0) nystep = 1;
	for (nx=nxs; nx<=nxe; nx+=nxstep) {
		for (ny=nys; ny<=nye; ny+=nystep) {
			px=Pxs+Dx*(x[nx]-Gxs);
			py=Pys+Dy*(y[ny]-Gys);
			pvx=vx[findex2(nx,ny,nxa)]*pxn;
			pvy=vy[findex2(nx,ny,nxa)]*pyn;
			pva=sqrt(pvx*pvx+pvy*pvy);

			if (pva > 0.01) {
				acos1=-root3*pvx      -pvy;
				asin1=       pvx-root3*pvy;
				acos2=-root3*pvx      +pvy;
				asin2=      -pvx-root3*pvy;

				pxt=px;
				pyt=py;
				pxe=px+pvx;
				pye=py+pvy;
				px1=pxe+al*acos1;
				py1=pye+al*asin1;
				px2=pxe+al*acos2;
				py2=pye+al*asin2;

				gsaf_move(pxt,pyt);
				gsaf_draw(pxe,pye);
				gsaf_draw(px1,py1);
				gsaf_move(px2,py2);
				gsaf_draw(pxe,pye);
			}
		}
	}
	gsaf_offclp ();
}

/* draw vector graph */
void gsaf_gvectr5 (float *vx, float *vy, float *x, float *y,
		   int nxa, int nxs, int nxe, int nxstep, int nys, int nye, int nystep,
		   float xn, float yn, float al)
{
	const float root3 = 1.732051;
	float pxn, pyn;
	int nx, ny;
	float px, py, pvx, pvy, pva, acos1, asin1, acos2, asin2, pxt, pyt;
	float pxe1, pye1, px1, py1, px2, py2;
	float Pxs, Pxe, Pys, Pye;
	float Gxs, Gxe, Gys, Gye;
	float Dx, Dy;
	gsglPage *p;
	if (!gsgl_inq_page_is_open ()) return;

	p = gsgl_inq_page ();
	if (!gsgl_inq_gdefin_is_called (p)) return;
	
	gsaf_inqgdefin (&Pxs, &Pxe, &Pys, &Pye, &Gxs, &Gxe, &Gys, &Gye);
	Dx=(Pxe-Pxs)/(Gxe-Gxs);
	Dy=(Pye-Pys)/(Gye-Gys);

	pxn=1.0/xn;
	pyn=1.0/yn;

	if (nxstep == 0) nxstep = 1;
	if (nystep == 0) nystep = 1;
	for (nx=nxs; nx<=nxe; nx+=nxstep) {
		for (ny=nys; ny<=nye; ny+=nystep) {
			px=Pxs+Dx*(x[findex2(nx,ny,nxa)]-Gxs);
			py=Pys+Dy*(y[findex2(nx,ny,nxa)]-Gys);
			pvx=vx[findex2(nx,ny,nxa)]*pxn;
			pvy=vy[findex2(nx,ny,nxa)]*pyn;
			pva=sqrt(pvx*pvx+pvy*pvy);

			if (pva > 0.01) {
				acos1=-root3*pvx      -pvy;
				asin1=       pvx-root3*pvy;
				acos2=-root3*pvx      +pvy;
				asin2=      -pvx-root3*pvy;

				pxt=px;
				pyt=py;
				pxe1=px+pvx;
				pye1=py+pvy;
				px1=pxe1+al*acos1;
				py1=pye1+al*asin1;
				px2=pxe1+al*acos2;
				py2=pye1+al*asin2;

				gsaf_move (pxt,pyt);
				gsaf_draw (pxe1,pye1);
				gsaf_draw (px1,py1);
				gsaf_move (px2,py2);
				gsaf_draw (pxe1,pye1);
			}
		}
	}
	gsaf_offclp ();
}
