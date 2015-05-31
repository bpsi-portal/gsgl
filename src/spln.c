#include "gsaf.h"
#include "gsgl.h"
#include "utils.h"
#include "spln.h"
#include "bsc1.h"

#define INDEX(xmax,x,y)	(x-1+(y-1)*(xmax))
#define IFGOTO(v,l1,l2,l3) if(v<0) goto l1; else if(v==0) goto l2; else goto l3;

/*
C     ##########     SPLINE 1D     ##########
C
      SUBROUTINE  SPLN1D(X,F,C,NX,XN,FN,NXN,IP)
C
C     X(NX)           : ORIGINAL COORDINATES
C     F(NX)           : ORIGINAL DATA
C     C(NX,3)         : WORKING AREA
C                     : DF/DX (ON BOUNDARY NX=1 OR NI, MAY BE GIVEN)
C     NX              : NUMBER OF ORIGINAL DATA IN X OR Y DIMENSION
C     XN(NXN)         : NEW COORDINATES
C     FN(NXN)         : SPLINED NEW DATA
C     NXN             : NUMBER OF NEW DATA IN X OR Y DIMENSION
C     IP              : 0 : CALCULATE DF INTERNALLY
C                       1 : DF IS GIVEN IN C(1,1) AND C(NX,1)
C                       2 : DDF IS GIVEN IN C(1,1) AND C(NX,1)
C
      IMPLICIT REAL * 4 ( A - H , O - Z )
      DIMENSION  X(NX),F( NX)
      DIMENSION  C(NX,3)
      DIMENSION  XN(NXN),FN(NXN)
      DIMENSION  DF(2),IOPT(2)
 */
void gsaf_spln1d (float *x, float *f, float *c, int nx, float *xn, float *fn, int nxn, int ip)
{
	int ier,iopt[2];
	float df[2];

	/*if (!gsgl_inq_page_is_open ()) return;*/

	switch (ip) {
	case 0:
		iopt[0]=3;
		iopt[1]=3;
		break;
	case 1:
		iopt[0]=2;
		iopt[1]=2;
		df[0]=c[INDEX(nx, 1 ,1)];
		df[1]=c[INDEX(nx,nx,1)];
		break;
	case 2:
		iopt[0]=1;
		iopt[1]=1;
		df[0]=c[INDEX(nx, 1 ,1)];
		df[1]=c[INDEX(nx,nx,1)];
		break;
	}
	gsaf_gusplc (x, nx, f, df, iopt,  c,  nx, &ier);
	gsaf_gusplf (x, nx, f,  c,   nx, xn, nxn, fn, &ier);
}

/*
      SUBROUTINE GUSPLC(X, N, Y, DF, IOPT, C, NC, IER)
C***********************************************************************
C  COMPUTE THE COEFFICIENTS OF THE CUBIC SPLINE.                       *
C  PARAMETERS                                                          *
C    (1) X: 1-DIM. ARRAY FOR KNOWN POINTS                              *
C    (2) N: NUMBER OF KNOWN POINTS                                     *
C    (3) Y: 1-DIM. ARRAY FOR FUNCTION'S VALUES ON KNOWN POINTS         *
C    (4) DF: 1-DIM. ARRAY FOR DIFFERENTIALS AT END POINTS              *
C    (5) IOPT: 1-DIM. ARRAY SPECIFYING THE CONTENT OF DF               *
C    (6) C: 2-DIM. WORKING ARRAY                                       *
C    (7) NC: ROW SIZE OF THE ARRAY (C)                                 *
C    (8) IER: ERROR CODE                                               *
C  COPYRIGHT   T. OGUNI   JUNE 30 1989    VERSION 1.0                  *
C***********************************************************************
       IMPLICIT REAL*4(A-H,O-Z)
       DIMENSION X(N), Y(N), DF(2), IOPT(2), C(NC,3), EC(4), D(2)
 */
void gsaf_gusplc(float *x, int n, float *y, float *df, int *iopt, float *c, int nc, int *ier)
{
	int i,ii,ks,ke,ider,i1,i2,k,j;
	float ec[4],d[2],h,hy,a2,a1,x1,x2,x3,h1,h2,y1,y2,hh,piv,dy1,dy2;

	/*if (!gsgl_inq_page_is_open ()) return;*/

	if (n<2 || nc<n-1 || iopt[0]<1 || iopt[0]>3 ||
	   iopt[1]<1 || iopt[1]>3) {
		   *ier = 2;
		   gsgl_warning ("gusplc", "invalid argument %d %d %d %d",
				 n,nc,iopt[0],iopt[1]);
		   return;
	}
	for (i=1;i<=n-1;i++) {
		if (x[i-1]>=x[i]) {
			*ier = 1;
			gsgl_warning ("gusplc","x should satisfy upward order.");
			return;
		}
	}
	*ier = 0;
/*
C  SET THE END CONDITIONS.
 */
	ii = 2;
	ks = 1;
	ke = MIN(4,n);
	ider = 1;
	for(i=1;i<=2;i++) {
		i1 = 2 * i - 1;
		i2 = 2 * i;
		switch (iopt[i-1]) {
		case 1:
			ec[i1-1] = 0;
			ec[i2-1] = 2 * df[i-1];
			break;
		case 2:
			d[i-1] = df[i-1];
			if (i == 2)
				ii=n;
			h = x[ii-1] - x[ii-2];
			ec[i1-1] = 1;
			hy = y[ii-1] - y[ii-2];
			ec[i2-1] = 6 * (hy / h - d[i-1]) / h;
			if (i == 2)
				ec[i2-1] = - ec[i2-1];
			break;
		case 3:
			if (i != 1) {
				ks = MAX(1,n-3);
				ke = n;
				ider = n;
			}
			a2 = 0;
			d[i-1] = 0;
			for (k=ks;k<=ke;k++) {
				if (ider != k) {
					a1 = 1;
					for (j=ks;j<=ke;j++) {
						if (j != ider && j != k) {
							x1  = x[ider-1] - x[j-1];
							x2  = x[k-1] - x[j-1];
							a1 *= x1 / x2;
						}
					}
					x3 = x[k-1] - x[ider-1];
					d[i-1] += a1 * y[k-1] / x3;
					a2 -= 1 / x3;
				}
			}
			d[i-1] += y[ider-1] * a2;
			if (i == 2)
				ii = n;
			h = x[ii-1] - x[ii-2];
			ec[i1-1] = 1;
			hy = y[ii-1] - y[ii-2];
			ec[i2-1] = 6 * (hy / h - d[i-1]) / h;
			if (i == 2)
				ec[i2-1] = - ec[i2-1];
			break;
		}
	}
/*
C  SET THE ELEMENTS FOR THE SYMMETRIC TRIDIAGONAL EQUATION.
 */
	if (n != 2) {
		h1 = x[1] - x[0];
		y1 = y[1] - y[0];
		for(i=2;i<=n-1;i++) {
			h2 = x[i] - x[i-1];
			y2 = y[i] - y[i-1];
			hh = h1 + h2;
			c[INDEX(nc,i,1)] = h2 / hh;
			c[INDEX(nc,i,2)] = 1 - c[INDEX(nc,i,1)];
			c[INDEX(nc,i,3)] = 6 * (y2 / h2 - y1 / h1) / hh;
			h1 = h2;
			y1 = y2;
		}
	}
/*
C  SOLVE THE EQUATION
 */
	c[INDEX(nc,1,1)] = - ec[0] * 0.5;
	c[INDEX(nc,1,2)] =   ec[1] * 0.5;
	if (n != 2) {
		for (k=2;k<=n-1;k++) {
			piv = 2 + c[INDEX(nc,k,2)] * c[INDEX(nc,k-1,1)];
			c[INDEX(nc,k,1)] = - c[INDEX(nc,k,1)] / piv;
			c[INDEX(nc,k,2)] =  (c[INDEX(nc,k,3)] - c[INDEX(nc,k,2)] * c[INDEX(nc,k-1,2)]) / piv;
		}
	}
	dy1 = (ec[3] - ec[2] * c[INDEX(nc,n-1,2)]) / (2 + ec[2] * c[INDEX(nc,n-1,1)]);
	for (i=1;i<=n-1;i++) {
		k = n - i;
		dy2 = c[INDEX(nc,k,1)] * dy1 + c[INDEX(nc,k,2)];
		h = x[k] - x[k-1];
		c[INDEX(nc,k,3)] = (dy1 - dy2) / (6 * h);
		c[INDEX(nc,k,2)] = 0.5 * dy2;
		c[INDEX(nc,k,1)] = (y[k] - y[k-1]) / h - (c[INDEX(nc,k,2)] + c[INDEX(nc,k,3)] * h) * h;
		dy1 = dy2;
	}
}

/*
      SUBROUTINE GUSPLF(X, N, Y, C, NC, V, M, F, IER)
C***********************************************************************
C  INTERPOLATION BY THE CUBIC SPLINE.                                  *
C  PARAMETERS                                                          *
 C    (1) X: 1-DIM. ARRAY FOR KNOWN POINTS                              *
 C    (2) N: NUMBER OF KNOWN POINTS                                     *
 C    (3) Y: 1-DIM. ARRAY FOR FUNCTION'S VALUES ON KNOWN POINTS         *
 C    (4) C: 2-DIM. WORKING ARRAY                                       *
 C    (5) NC: ROW SIZE OF THE ARRAY (C)                                 *
 C    (6) V: 1-DIM. ARRAY FOR POINTS WHICH INTERPOLATION MUST BE MADE   *
 C    (7) M: NUMBER OF POINTS FOR WHICH INTERPOLATION MUST BE MADE      *
 C    (8) F: 1-DIM. WORKING ARRAY                                       *
 C    (9) IER: ERROR CODE                                               *
C  COPYRIGHT   T. OGUNI   JUNE 30 1989   VERSION 1.0                   *
C***********************************************************************
       IMPLICIT REAL*4(A-H,O-Z)
       DIMENSION X(N), Y(N), C(NC,3), V(M), F(M)
 */
void gsaf_gusplf (float *x, int n, float *y, float *c, int nc, float *v, int m, float *f, int *ier)
{
	int i,k;
	float v1,v2;

	/*if (!gsgl_inq_page_is_open ()) return;*/

	if (n < 2 || m < 1 || nc < n-1) {
		*ier = 2;
		gsgl_warning ("gusplf","invalid argument. %d %d %d", n, nc, m);
		return;
	}
	*ier = 0;
	i = 1;
	for (k=1;k<=m;k++) {
		v1 = v[k-1] - x[i-1];
		IFGOTO(v1,l1,l3,l4);
l1:
		if (i>1) goto l2;
		*ier = 1;
		goto l8;
l2:
		i--;
		v1 = v[k-1] - x[i-1];
		IFGOTO (v1,l1,l3,l8);
l3:
		f[k-1] = y[i-1];
		continue;
l4:
		if (i<n) goto l5;
		*ier = 1;
		i = n - 1;
		goto l8;
l5:
		v2 = v[k-1] - x[i];
		IFGOTO (v2,l8,l6,l7);
l6:
		i++;
		goto l3;
l7:
		i++;
		v1 = v2;
		goto l4; 
l8:
		f[k-1] = y[i-1] + v1 * (c[INDEX(nc,i,1)] + v1 * (c[INDEX(nc,i,2)] + v1 * c[INDEX(nc,i,3)]));
#if 0
		v1 = v[k-1] - x[i-1];
		if (v1 == 0) {
			f[k-1] = y[i-1];
			continue;
		} else if (v1 < 0) {
			if (i <= 1) {
				*ier = 1;
				goto gusplf1;
			}
			while (v1 < 0) {
				i--;
				v1 = v[k-1] - x[i-1];
			}
			if (v1 == 0) {
				f[k-1] = y[i-1];
				continue;
			}
		} else {  /* v1 > 0 */
			if (i >= n) {
				*ier = 1;
				i = n - 1;
				goto gusplf1;
			}
			v2 = v[k-1] - x[i];
			while (v2 > 0) {
				i++;
				v1 = v2;
				v2 = v[k-1] - x[i];
			}
			if (v2 == 0 ) {
				i++;
				f[k-1] = y[i-1];
				continue;
			}
		}
	gusplf1:
		f[k-1] = y[i-1] + v1 * (c[INDEX(nc,i,1)] + v1 * (c[INDEX(nc,i,2)] + v1 * c[INDEX(nc,i,3)]));
#endif
	}
}

/*
C
C     ##########     SPLINE 2D     ##########
C  
      SUBROUTINE  SPLN2D(X,Y,F,P,Q,S,A,IM,NI,NJ,XG,YG,FG,IN,NNI,NNJ,IP) 
C    
 C     X(NI),Y(NJ)     : ORIGINAL COORDINATES
 C     F(IM,NJ)        : ORIGINAL DATA
 C     P(IM,NJ)        : DF/DX (ON BOUNDARY NX=1 OR NI, MAY BE GIVEN)
 C     Q(IM,NJ)        : DF/DY (ON BOUNDARY NY=1 OR NJ, MAY BE GIVEN)
 C     R(IM,NJ)        : DDF/DXDY     (AT FOUR CORNERS, MAY BE GIVEN)
 C     A(4,MAX(NI,NJ)) : WORK AREA
 C     IM              : FIRST DIMENSION OF ORIGINAL DATA AND WORK AREA  
 C     NI,NJ           : NUMBER OF ORIGINAL DATA IN X OR Y DIMENSION
 C     XG(NNI),YG(NNJ) : NEW COORDINATES
 C     FG(IN,NNJ)      : SPLINED NEW DATA
 C     IM              : FIRST DIMENSION OF NEW DATA
 C     NNI, NNJ        : NUMBER OF NEW DATA IN X OR Y DIMENSION
 C     IP              : 0 : CALCULATE P,Q,S INTERNALLY 
 C                       1 : P,Q,S IS GIVEN
C
      IMPLICIT REAL * 4 ( A - H , O - Z )
      DIMENSION  X( * ),Y( * ),F( IM,* )
      DIMENSION  XG( * ),YG( * ),FG( IN,* )
      DIMENSION  P( IM,* ),Q( IM,* ),S( IM,* )
      DIMENSION  GAM( 4,4 ),A( 4,* )
      REAL*8 DXY
C
 */
void gsaf_spln2d (float *x, float *y, float *f, float *p, float *q, float *s, float *a,
	     int im, int ni, int nj, float *xg, float *yg, float *fg, int in, int nni, int nnj,
	     int ip)
{
	int ido,i,ixl,ixu,jdo,j,iyl,iyu,ix,iy,iom,ion;
	float gam[4][4],xx,yy;
	double dxy;

	/*if (!gsgl_inq_page_is_open ()) return;*/

	if( ni<4 || nj<4 )
		gsgl_warning ("spln2d","   NI < 4   OR   NJ < 4");

	gsaf_guspln( x,y,f,ni,nj,p,q,s,a,im,ip );

	if( xg[0]<x[0] || xg[nni-1]>x[ni-1] ||
	    yg[0]<y[0] || yg[nnj-1]>y[nj-1] )
		    gsgl_warning ("spln2d","   interpolation is over segment");

	for (ido=2;ido<=ni;ido++) {
		i=ido;
		ixl = 1;
		while ( x[i-2] > xg[ixl-1] ) {
			ixl++;
			if(ixl>nni)
				break;
		}
		if(ixl>nni)
			break;
		ixu = ixl;
		while( x[i-1] > xg[ixu-1] ) {
			ixu++;
			if( ixu>nni )
				break;
		}
		if(fabs(x[i-1]-xg[ixu-1])<=1e-72)
			ixu++;

		for (jdo=2;jdo<=nj;jdo++) {
			j = jdo;
			iyl = 1;
			while ( y[j-2] > yg[iyl-1] ) {
				iyl++;
				if(iyl>nnj)
					break;
			}
			if(iyl>nnj)
				break;
			iyu = iyl;
			while ( y[j-1] > yg[iyu-1] ) {
				iyu++;
				if ( iyu > nnj )
					break;
			}
			if (fabs(y[j-1]-yg[iyu-1]) <= 1e-72)
				iyu++;

			gsaf_gucalg (x,y,f,p,q,s,&gam[0][0],im,i,j);

			for (ix=ixl;ix<=ixu-1;ix++)
				for (iy=iyl;iy<=iyu-1;iy++)
					fg[INDEX(in,ix,iy)] = 0;

			for (ix=ixl;ix<=ixu-1;ix++) {
				for (iy=iyl;iy<=iyu-1;iy++) {
					for (iom=0;iom<=3;iom++) {
						if ( iom != 0 )
							xx = pow( xg[ix-1] - x[i-2], iom);
						else
							xx = 1;
						for (ion=0;ion<=3;ion++) {
							if ( ion != 0 )
								yy = pow( yg[iy-1] - y[j-2], ion);
							else
								yy = 1;
							dxy=((double)xx)*((double)yy);
							if(fabs(dxy)<1e-30)
								dxy=0;
							fg[INDEX(in,ix,iy)] = ((double) fg[INDEX(in,ix,iy)]) + ((double) gam[ion][iom])*dxy;
						}
					}
				}
			}
		}
	}
}

/*
C
C     ##########     GUSPLN     ##########
C
      SUBROUTINE  GUSPLN( X,Y,F,NI,NJ,P,Q,S,A,IM,IP )
C
      IMPLICIT REAL * 4 ( A - H , O - Z )
      DIMENSION  X( * ),Y( * ),F( IM,* ),
     &           P( IM,* ),Q( IM,* ),S( IM,* ),
     &           A( 4,* )
C
 */
void gsaf_guspln (float *x, float *y, float *f, int ni, int nj, float *p, float *q, float *s,
	     float *a, int im, int ip)
{
	int i,j;
	float dx1,dx2,as,dxi1,dxi,dy1,dy2,dyj1,dyj;
	int _value;

	/*if (!gsgl_inq_page_is_open ()) return;*/

	if (ip == 0)
		gsaf_guencn (p,q,s,x,y,f,im,ni,nj);

	for (j=1;j<=nj;j++) {
		dx1 = x[1] - x[0];
		dx2 = x[2] - x[1];
		as = 3 * ( dx1 / dx2 * ( f[INDEX(im,3,j)] - f[INDEX(im,2,j)] )
			 + dx2 / dx1 * ( f[INDEX(im,2,j)] - f[INDEX(im,1,j)] ) );
		a[INDEX(4,4,1)] = as - dx2 * p[INDEX(im,1,j)];
		a[INDEX(4,2,1)] = 2 * ( dx1 + dx2 );
		a[INDEX(4,3,1)] = dx1;
		for (i=3;i<=ni-2;i++) {
			dxi1 = x[i-1] - x[i-2];
			dxi  = x[ i ] - x[i-1];
			as = 3 * (dxi1 / dxi  * ( f[INDEX(im,i+1,j)] - f[INDEX(im, i ,j)] )
				+ dxi  / dxi1 * ( f[INDEX(im, i ,j)] - f[INDEX(im,i-1,j)] ) );
			a[INDEX(4,3,i-1)] = dxi1;
			a[INDEX(4,2,i-1)] = 2 * ( dxi1 + dxi );
			a[INDEX(4,1,i-1)] = dxi;
			a[INDEX(4,4,i-1)] = as;
		}
		i = ni-1;
		dxi1 = x[i-1] - x[i-2];
		dxi  = x[ i ] - x[i-1];
		as = 3 * ( dxi1 / dxi  * ( f[INDEX(im,i+1,j)] - f[INDEX(im, i ,j)] )
			 + dxi  / dxi1 * ( f[INDEX(im, i ,j)] - f[INDEX(im,i-1,j)] ) );
		a[INDEX(4,4,i-1)] = as - dxi1 * p[INDEX(im,ni,j)];
		a[INDEX(4,2,i-1)] = 2 * ( dxi1 + dxi );
		a[INDEX(4,1,i-1)] = dxi;

		_value = ni-2;
		gsaf_gubnda ( a,&_value );
		for (i=2;i<=ni-1;i++)
			p[INDEX(im,i,j)] = a[INDEX(4,4,i-1)];
	}

	for (i=1;i<=ni;i++) {
		dy1 = y[1] - y[0];
		dy2 = y[2] - y[1];
		as = 3 * ( dy1 / dy2 * ( f[INDEX(im,i,3)] - f[INDEX(im,i,2)] )
			 + dy2 / dy1 * ( f[INDEX(im,i,2)] - f[INDEX(im,i,1)] ) );
		a[INDEX(4,4,1)] = as - dy2 * q[INDEX(im,i,1)];
		a[INDEX(4,2,1)] = 2 * ( dy1 + dy2 );
		a[INDEX(4,3,1)] = dy1;
		for (j=3;j<=nj-2;j++) {
			dyj1 = y[j-1] - y[j-2];
			dyj  = y[ j ] - y[j-1];
			as = 3 * ( dyj1 / dyj  * ( f[INDEX(im,i,j+1)] - f[INDEX(im,i,j)] )
				 + dyj  / dyj1 * ( f[INDEX(im,i, j )] - f[INDEX(im,i,j-1)] ) );
			a[INDEX(4,3,j-1)] = dyj1;
			a[INDEX(4,2,j-1)] = 2 * ( dyj1 + dyj );
			a[INDEX(4,1,j-1)] = dyj;
			a[INDEX(4,4,j-1)] = as;
		}
		j = nj-1;
		dyj1 = y[j-1] - y[j-2];
		dyj  = y[ j ] - y[j-1];
		as = 3 * ( dyj1 / dyj  * ( f[INDEX(im,i,j+1)] - f[INDEX(im,i, j )] )
			 + dyj  / dyj1 * ( f[INDEX(im,i, j )] - f[INDEX(im,i,j-1)] ) );
		a[INDEX(4,4,j-1)] = as - dyj1 * q[INDEX(im,i,nj)];
		a[INDEX(4,2,j-1)] = 2 * ( dyj1 + dyj );
		a[INDEX(4,1,j-1)] = dyj;

		_value = nj-2;
		gsaf_gubnda ( a,&_value );
		for (j=2;j<=nj-1;j++)
			q[INDEX(im,i,j)] = a[INDEX(4,4,j-1)];
	}

	for (j=1;j<=nj;j+=nj-1) {
		dx1 = x[1] - x[0];
		dx2 = x[2] - x[1];
		as = 3 * ( dx1 / dx2 * ( q[INDEX(im,3,j)] - q[INDEX(im,2,j)] )
			 + dx2 / dx1 * ( q[INDEX(im,2,j)] - q[INDEX(im,1,j)] ) );
		a[INDEX(4,4,1)] = as - dx2 * s[INDEX(im,1,j)];
		a[INDEX(4,2,1)] = 2 * ( dx1 + dx2 );
		a[INDEX(4,3,1)] = dx1;
		for (i=3;i<=ni-2;i++) {
			dxi1 = x[i-1] - x[i-2];
			dxi  = x[ i ] - x[i-1];
			as = 3 * (dxi1 / dxi  * ( q[INDEX(im,i+1,j)] - q[INDEX(im, i ,j)] )
				+ dxi  / dxi1 * ( q[INDEX(im, i ,j)] - q[INDEX(im,i-1,j)] ) );
			a[INDEX(4,3,i-1)] = dxi1;
			a[INDEX(4,2,i-1)] = 2 * ( dxi1 + dxi );
			a[INDEX(4,1,i-1)] = dxi;
			a[INDEX(4,4,i-1)] = as;
		}
		i = ni-1;
		dxi1 = x[i-1] - x[i-2];
		dxi  = x[ i ] - x[i-1];
		as = 3 * ( dxi1 / dxi  * ( q[INDEX(im,i+1,j)] - q[INDEX(im, i ,j)] )
			 + dxi  / dxi1 * ( q[INDEX(im, i ,j)] - q[INDEX(im,i-1,j)] ) );
		a[INDEX(4,4,i-1)] = as - dxi1 * s[INDEX(im,ni,j)];
		a[INDEX(4,2,i-1)] = 2 * ( dxi1 + dxi );
		a[INDEX(4,1,i-1)] = dxi;

		_value=ni-2;
		gsaf_gubnda ( a,&_value );
		for (i=2;i<=ni-1;i++)
			s[INDEX(im,i,j)] = a[INDEX(4,4,i-1)];
	}

	for (i=1;i<=ni;i++) {
		dy1 = y[1] - y[0];
		dy2 = y[2] - y[1];
		as = 3 * ( dy1 / dy2 * ( p[INDEX(im,i,3)] - p[INDEX(im,i,2)] )
			 + dy2 / dy1 * ( p[INDEX(im,i,2)] - p[INDEX(im,i,1)] ) );
		a[INDEX(4,4,1)] = as - dy2 * s[INDEX(im,i,1)];
		a[INDEX(4,2,1)] = 2 * ( dy1 + dy2 );
		a[INDEX(4,3,1)] = dy1;
		for (j=3;j<=nj-2;j++) {
			dyj1 = y[j-1] - y[j-2];
			dyj  = y[ j ] - y[j-1];
			as = 3 * ( dyj1 / dyj  * ( p[INDEX(im,i,j+1)] - p[INDEX(im,i,j)] )
				 + dyj  / dyj1 * ( p[INDEX(im,i,j)] - p[INDEX(im,i,j-1)] ) );
			a[INDEX(4,3,j-1)] = dyj1;
			a[INDEX(4,2,j-1)] = 2 * ( dyj1 + dyj );
			a[INDEX(4,1,j-1)] = dyj;
			a[INDEX(4,4,j-1)] = as;
		}
		j = nj-1;
		dyj1 = y[j-1] - y[j-2];
		dyj  = y[ j ] - y[j-1];
		as = 3 * ( dyj1 / dyj  * ( p[INDEX(im,i,j+1)] - p[INDEX(im,i,j)] )
			 + dyj  / dyj1 * ( p[INDEX(im,i,j)] - p[INDEX(im,i,j-1)] ) );
		a[INDEX(4,4,j-1)] = as - dyj1 * s[INDEX(im,i,nj)];
		a[INDEX(4,2,j-1)] = 2 * ( dyj1 + dyj );
		a[INDEX(4,1,j-1)] = dyj;

		_value=nj-2;
		gsaf_gubnda ( a,&_value );
		for (j=2;j<=nj-1;j++)
			s[INDEX(im,i,j)] = a[INDEX(4,4,j-1)];
	}
}

/*
C
C     ##########     GUENCN     ##########
C
      SUBROUTINE  GUENCN( P,Q,S,X,Y,F,IM,NI,NJ )
C     
      IMPLICIT REAL * 4 ( A - H , O - Z )
      DIMENSION  P( IM,* ),Q( IM,* ),S( IM,* )
      DIMENSION  X( * ),Y( * ),F( IM,* )
C
 */
void gsaf_guencn (float *p, float *q, float *s, float *x, float *y, float *f, int im, int ni, int nj)
{
	int i,j;
	float dx1,dxi,dy1,dyj;

	/*if (!gsgl_inq_page_is_open ()) return;*/

	dx1 = x[1] - x[0];
	dxi = x[ni-1] - x[ni-2];
	dy1 = y[1] - y[0];
	dyj = y[nj-1] - y[nj-2];
	for (i=1;i<=ni;i++) {
		q[INDEX(im,i,1)] = ( f[INDEX(im,i,2)] - f[INDEX(im,i,1)] ) / dy1;
		q[INDEX(im,i,nj)] = ( f[INDEX(im,i,nj)] - f[INDEX(im,i,nj-1)] ) / dyj;
	}
	for (j=1;j<=nj;j++) {
		p[INDEX(im,1 ,j)] = ( f[INDEX(im,2 ,j)] - f[INDEX(im,1,j)] ) / dx1;
		p[INDEX(im,ni,j)] = ( f[INDEX(im,ni,j)] - f[INDEX(im,ni-1,j)] ) / dxi;
	}
	s[INDEX(im,1,1)]     = ( p[INDEX(im,1,2)]     - p[INDEX(im,1,1)] )       / dy1;
	s[INDEX(im,1,nj)]   = ( p[INDEX(im,1,nj)]   - p[INDEX(im,1,nj-1)] )   / dyj;
	s[INDEX(im,ni,1)]   = ( p[INDEX(im,ni,2)]   - p[INDEX(im,ni,1)] )     / dy1;
	s[INDEX(im,ni,nj)] = ( p[INDEX(im,ni,nj)] - p[INDEX(im,ni,nj-1)] ) / dyj;
}

/*
C
C     ##########     GUBNDA     ##########
C
      SUBROUTINE  GUBNDA( A,L1 )
C
      IMPLICIT REAL * 4 ( A - H , O - Z )
      DIMENSION  A( 4,* )
C
 */
void gsaf_gubnda(a,l1)
int *l1;
float *a;
{
	int i,ii;
	float co,pa;

	/*if (!gsgl_inq_page_is_open ()) return;*/

	for (i=1;i<=*l1-1;i++) {
		co = a[INDEX(4,2,i)];
		pa = a[INDEX(4,1,i+1)];
		a[INDEX(4,2,i+1)] = a[INDEX(4,2,i+1)] - a[INDEX(4,3,i)]*pa/co;
		a[INDEX(4,4,i+1)] = a[INDEX(4,4,i+1)] - a[INDEX(4,4,i)]*pa/co;
	}

	a[INDEX(4,4,*l1)] = a[INDEX(4,4,*l1)] / a[INDEX(4,2,*l1)];
	for (ii=1;ii<=*l1-1;ii++) {
		i = *l1-ii;
		a[INDEX(4,4,i)] = ( a[INDEX(4,4,i)] - a[INDEX(4,3,i)]
				   *a[INDEX(4,4,i+1)] ) / a[INDEX(4,2,i)];
	}
}

/*
C
C     ##########     GUCALG     ##########
C
      SUBROUTINE  GUCALG( X,Y,F,P,Q,S,GAM,IM,I,J )
C
      IMPLICIT REAL * 4 ( A - H , O - Z )
      DIMENSION  X( * ),Y( * ),F( IM,* )
      DIMENSION  P( IM,* ),Q( IM,* ),S( IM,* )
      DIMENSION  GAM( 4,4 ),GSM( 4,4 ),AX( 4,4 ),AY( 4,4 )
      DIMENSION  FK( 4,4 )    
C
 */
void gsaf_gucalg (float *x, float *y, float *f, float *p, float *q, float *s, float *gam,
		  int im, int i, int j)
{
	int i1,j1,m1,m2,m3;
	float ax[4][4],ay[4][4],gsm[4][4],fk[4][4];
	float _value;

	/*if (!gsgl_inq_page_is_open ()) return;*/

	for (i1=1;i1<=4;i1++)
		for (j1=1;j1<=4;j1++) {
			gam[INDEX(4,i1,j1)] = 0.0;
			gsm[j1-1][i1-1] = 0.0;
		}
	_value=x[i-1]-x[i-2];
	gsaf_gucala(_value,&ax[0][0]);
	_value=y[j-1]-y[j-2];
	gsaf_gucala (_value, &ay[0][0]);
	gsaf_gucalk (p,q,s,f,&fk[0][0],im,i,j);
	for (m1=1;m1<=4;m1++)
		for (m2=1;m2<=4;m2++) 
			for (m3=1;m3<=4;m3++)
				gsm[m2-1][m1-1] = gsm[m2-1][m1-1] + ax[m3-1][m1-1]*fk[m2-1][m3-1];
	for (m1=1;m1<=4;m1++)
		for (m2=1;m2<=4;m2++) 
			for (m3=1;m3<=4;m3++)
				gam[INDEX(4,m1,m2)] = gam[INDEX(4,m1,m2)] + gsm[m3-1][m1-1]*ay[m3-1][m2-1];
}

/*
C     
C     ##########     GUCALA     ##########
C
      SUBROUTINE  GUCALA( H,A )
C
      IMPLICIT REAL * 4 ( A - H , O - Z )
      DIMENSION  A( 4,4 )
C
 */
void gsaf_gucala (float h, float *a)
{
	int i,j;

	/*if (!gsgl_inq_page_is_open ()) return;*/

	for (i=1;i<=4;i++)
		for(j=1;j<=4;j++)
			a[INDEX(4,i,j)] = 0.0;

	a[INDEX(4,1,1)] =  1.0;
	a[INDEX(4,2,2)] =  1.0;
	a[INDEX(4,3,1)] = -3.0/(h*h);
	a[INDEX(4,3,2)] = -2.0/h;
	a[INDEX(4,3,3)] =  3.0/(h*h);
	a[INDEX(4,3,4)] = -1.0/h;
	a[INDEX(4,4,1)] =  2.0/(h*h*h);
	a[INDEX(4,4,2)] =  1.0/(h*h);
	a[INDEX(4,4,3)] = -2.0/(h*h*h);
	a[INDEX(4,4,4)] =  1.0/(h*h);
}

/*
C     
C     ##########     GUCALK     ##########
C
      SUBROUTINE  GUCALK( P,Q,S,F,FK,IM,I,J )
C
      IMPLICIT REAL * 4 ( A - H , O - Z )
      DIMENSION  P( IM,* ),Q( IM,* ),S( IM,* ),F( IM,* )
      DIMENSION  FK( 4,4 )
C
 */
void gsaf_gucalk (float *p, float *q, float *s, float *f, float *fk, int im, int i, int j)
{
	/*if (!gsgl_inq_page_is_open ()) return;*/

	fk[INDEX(4,1,1)] = f[INDEX(im,i-1,j-1 )];
	fk[INDEX(4,1,2)] = q[INDEX(im,i-1,j-1 )];
	fk[INDEX(4,1,3)] = f[INDEX(im,i-1, j  )];
	fk[INDEX(4,1,4)] = q[INDEX(im,i-1, j  )];
	fk[INDEX(4,2,1)] = p[INDEX(im,i-1,j-1 )];
	fk[INDEX(4,2,2)] = s[INDEX(im,i-1,j-1 )];
	fk[INDEX(4,2,3)] = p[INDEX(im,i-1, j  )];
	fk[INDEX(4,2,4)] = s[INDEX(im,i-1, j  )];
	fk[INDEX(4,3,1)] = f[INDEX(im, i ,j-1 )];
	fk[INDEX(4,3,2)] = q[INDEX(im, i ,j-1 )];
	fk[INDEX(4,3,3)] = f[INDEX(im, i , j  )];
	fk[INDEX(4,3,4)] = q[INDEX(im, i , j  )];
	fk[INDEX(4,4,1)] = p[INDEX(im, i ,j-1 )];
	fk[INDEX(4,4,2)] = s[INDEX(im, i ,j-1 )];
	fk[INDEX(4,4,3)] = p[INDEX(im, i , j  )];
	fk[INDEX(4,4,4)] = s[INDEX(im, i , j  )];
}
