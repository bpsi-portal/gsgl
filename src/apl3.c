#include "gsaf.h"
#include "gsgl.h"
#include "bsc1.h"
#include "apl1.h"

/*c
  c     ********************************************
  c     ****** gsaf application v3.5 : graph2 ******
  c     ********************************************
  c
  c     ****** draw ticks on axis ******
  c
*/
void gsaf_gaxis2 (float x0, float y0, float x1, float y1,
		  float fmin, float fmax, float forg, float fstep, float sleng, int ind)
{
	const float eps = 0.01f;
	gsglboolean lin, lout, lnbot, lntop;
	int ipat;
	float scl;
	float al, cosa, sina, xl, yl, fl;
	float dx, dy, dx1, dy1, dx2, dy2;
	int n, ns, ne;
	float x, y;

	if (!gsgl_inq_page_is_open ()) return;
	lin = ind%2 == 1;
	lout = (ind/2)%2 == 1;
	lnbot = (ind/16)%2 == 1;
	lntop = (ind/32)%2 == 1;
	ipat=0;
	scl=sleng;

	al=sqrt((x1-x0)*(x1-x0)+(y1-y0)*(y1-y0));
	if(al <= 0.0) return;
	cosa=(x1-x0)/al;
	sina=(y1-y0)/al;
	xl=al*cosa;
	yl=al*sina;
	fl=fmax-fmin;

	if((fmax-forg)*(forg-fmin) < 0.0 || fl == 0.0) return;
	dx=xl*fstep/fl;
	dy=yl*fstep/fl;
	if(lnbot) {
		ns = (forg-fmin)/fstep-eps;
	} else {
		ns = (forg-fmin)/fstep+eps;
	}
	if(lntop) {
		ne = (fmax-forg)/fstep-eps;
	} else {
		ne = (fmax-forg)/fstep+eps;
	}

	if(lin) {
		dx1= scl*sina;
		dy1=-scl*cosa;
	} else {
		dx1=0.0;
		dy1=0.0;
	}
	if(lout) {
		dx2=-scl*sina;
		dy2= scl*cosa;
	} else {
		dx2=0.0;
		dy2=0.0;
	}

	gsaf_move (x0, y0);
	gsaf_draw (x0+xl,y0+yl);

	for (n=-ns; n<=ne; n++) {
		x=x0+dx*n;
		y=y0+dy*n;
		gsaf_movept(x+dx1,y+dy1,ipat);
		gsaf_drawpt(x+dx2,y+dy2);
	}
}

/*
  c
  c     ****** draw numbers on axis ******
  c
*/
void gsaf_gaxiv2 (float x0, float y0, float x1, float y1,
		  float fmin, float fmax, float forg, float fstep, int indc, int indp)
{
	const int iposd[] = {2,1,3,4};
	const float eps = 0.01f;
	float chh,chw,chsp,angl,tilt;
	int ind2, ipos;
	gsglboolean lpos, ldir, lnbot, lntop;
	float al, ang, cosa, sina, xl, yl, fl;
	float dx, dy, dx1, dy1;
	int n, ns, ne, ijust;
	float ds, f, x, y;

	if (!gsgl_inq_page_is_open ()) return;
	gsaf_inqchr (&chh, &chw, &chsp, &angl, &tilt);

	ind2 = abs(indp);
	ipos = iposd[ind2%4+1-1];
	lpos = (ind2/4)%2 == 1;
	ldir = (ind2/8)%2 == 1;
	lnbot = (ind2/16)%2 == 1;
	lntop = (ind2/32)%2 == 1;

	al=sqrt((x1-x0)*(x1-x0) + (y1-y0)*(y1-y0));
	if(al <= 0.0) return;
	ang=atan2(x1-x0,y1-y0);
	cosa=(x1-x0)/al;
	sina=(y1-y0)/al;
	xl=al*cosa;
	yl=al*sina;
	fl=fmax-fmin;
	if((fmax-forg)*(forg-fmin) < 0.0) return;
	dx=xl*fstep/fl;
	dy=yl*fstep/fl;
	if(lnbot) {
		ns = (forg-fmin)/fstep-eps;
	} else {
		ns = (forg-fmin)/fstep+eps;
	}
	if(lntop) {
		ne = (fmax-forg)/fstep-eps;
	} else {
		ne = (fmax-forg)/fstep+eps;
	}

	if(ldir) {
		if(lpos) {
			ds=0.5*ipos*chh;
			dx1=-ds*sina-0.5*chh*cosa;
			dy1= ds*cosa-0.5*chh*sina;
			ijust=1;
		} else {
			ds=0.5*ipos*chh;
			dx1= ds*sina-0.5*chh*cosa;
			dy1=-ds*cosa-0.5*chh*sina;
			ijust=0;
		}
		if(indp >= 0) {
			gsaf_setchr (chh,chw,chsp,ang-90.0,0.0);
		} else {
			gsaf_setchr(chh,chw,chsp,ang+90.0,0.0);
		}
	} else {
		if(lpos) {
			ds=0.5*ipos*chh;
			dx1=-ds*sina;
			dy1= ds*cosa;
			ijust=3;
		} else {
			ds=0.5*(ipos+2)*chh;
			dx1= ds*sina;
			dy1=-ds*cosa;
			ijust=3;
		}
		if (indp >= 0) {
			gsaf_setchr(chh,chw,chsp,ang,0.0);
		} else {
			gsaf_setchr(chh,chw,chsp,ang+180.0,0.0);
		}
	}

	for (n=ns; n<=ne; n++) {
		f=forg+fstep*n;
		x=x0+dx*n+dx1;
		y=y0+dy*n+dy1;
		gsaf_gnumbr (x,y,f,indc,ijust);
	}
	gsaf_setchr(chh,chw,chsp,angl,tilt);
}

