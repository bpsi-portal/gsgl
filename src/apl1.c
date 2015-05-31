#include "gsaf.h"
#include "gsgl.h"
#include "apl1.h"
#include "bsc1.h"
#include "bsc2.h"
#include "bsc2d.h"
#include "bsc3d.h"
#include "utils.h"
#include "matrix.h"

static void movept_dim (float x, float y, float z, int ipat, int dim, gsglPage *page);

void gsgl_init_hold_text_position (gsglPage *p)
{
	PATTERN(p)->Hold_xy = GSGL_FALSE;
}

gsglboolean gsgl_inq_hold_text_position (gsglPage *p)
{
	return PATTERN(p)->Hold_xy;
}

/* draw delimited text */
static void textx_dim (char *ktextx, int dim)
{
	char *ktext;
	int len;

	len = strlen (ktextx);
	if (len == 0) return;
	ktext = (char *) gsgl_malloc_atomic ((len+1)*sizeof(char));

	gsaf_gudstr (ktextx, ktext);
	if (dim == 1)
		gsaf_text (ktext);
	else if (dim == 2)
		gsaf_text2d (ktext);
	else if (dim == 3)
		gsaf_text3d (ktext);
}

void gsaf_textx (char *ktextx)
{
	if (!gsgl_inq_page_is_open ()) return;

	textx_dim (ktextx, 1);
}

void gsaf_textx2d (char *ktextx)
{
	if (!gsgl_inq_page_is_open ()) return;

	textx_dim (ktextx, 2);
}

void gsaf_textx3d (char *ktextx)
{
	if (!gsgl_inq_page_is_open ()) return;

	textx_dim (ktextx, 3);
}


/* draw justified text */
static void gtext_dim (float x, float y, float z, char *ktext, int nchar, int ijust, int dim, gsglPage *p)
{
	float deg = 0.0174533;
	float pxmin,pxmax,pymin,pymax,xmin,xmax,ymin,ymax;
	float facx = 0.0, facy = 0.0;
	float chh,chw,chsp,angl,tilt;
	float del = 0.0;
	float size, size1, size2;
	int i;
	float xx, yy;
	gsglboolean normal = GSGL_FALSE;
	float xworld, yworld, zworld;
	float x3d, y3d, z3d;
	char *tmp;

	gsaf_inqchr (&chh, &chw, &chsp, &angl, &tilt);

	if (dim == 3) normal = gsgl_inq_text3d_normal (p);
	
	if (ijust == 0) {
		del = 0.0;
	} else if (ijust == 1) {
		gsaf_inqtsz (ktext, &size);
		del = -size;
	} else if (ijust == 2) {
		gsaf_inqtsz (ktext, &size);
		del = -0.5*size;
	} else if (ijust == 3) {
		tmp = (char *) gsgl_malloc_atomic ((nchar+1)*sizeof(char));
		for (i=0; i<nchar; i++) {
			tmp[i] = ktext[i];
			if (ktext[i] == '.') {
				tmp[i] = '\0';
				gsaf_inqtsz (tmp, &size1);
				tmp[i] = '.';

				tmp[i+1] = '\0';
				gsaf_inqtsz (tmp, &size2);

				del = -0.5 * (size1 + size2);
				break;
			} else if (i == nchar-1) {
				gsaf_inqtsz (ktext, &size);
				del = -0.5 * size;
			}
		}
	} else {
		del = 0.0;
	}

	if (dim == 1) {
		gsaf_inqvew (&pxmin, &pxmax, &pymin, &pymax, &xmin, &xmax, &ymin, &ymax);
		facx = (xmax-xmin)/(pxmax-pxmin);
		facy = (ymax-ymin)/(pymax-pymin);

		xx = x + del * cos(angl*deg) * facx;
		yy = y + del * sin(angl*deg) * facy;
		gsaf_move (xx, yy);
		gsaf_text (ktext);
	} else if (dim == 2) {
		gsaf_move2d (x, y);
		gsaf_move (del*cos(angl*deg), del*sin(angl*deg));
		p->pattern->Hold_xy = GSGL_TRUE;
		gsaf_text2d (ktext);
		p->pattern->Hold_xy = GSGL_FALSE;
	} else if (dim == 3) {
		if (normal) {
			gsglproject3d (x, y, z, &xworld, &yworld, &zworld, p);
			
			xx = xworld+del * cos(angl*deg);
			yy = yworld+del * sin(angl*deg);
			
			gsglunproject3d(xx, yy, zworld, &x3d, &y3d, &z3d, p);
			gsaf_move3d (x3d, y3d, z3d);
			p->pattern->Hold_xy = GSGL_FALSE;
		} else {
			gsaf_move (del * cos(angl*deg), del * sin(angl*deg));
			gsaf_move3d (x, y, z);
			p->pattern->Hold_xy = GSGL_TRUE;
		}
		gsaf_text3d (ktext);
		p->pattern->Hold_xy = GSGL_FALSE;
	}
}

void gsaf_gtext (float x, float y, char *ktext, int ijust)
{
	if (!gsgl_inq_page_is_open ()) return;

	gtext_dim (x, y, 0.0, ktext, strlen(ktext), ijust, 1, gsgl_inq_page());
}

void gsaf_gtext2d (float x, float y, char *ktext, int ijust)
{
	if (!gsgl_inq_page_is_open ()) return;

	gtext_dim (x, y, 0.0, ktext, strlen(ktext), ijust, 2, gsgl_inq_page());
}

void gsaf_gtext3d (float x, float y, float z, char *ktext, int ijust)
{
	if (!gsgl_inq_page_is_open ()) return;

	gtext_dim (x, y, z, ktext, strlen(ktext), ijust, 3, gsgl_inq_page());
}

/* draw justified delimitted text */
static void gtextx_dim (float x, float y, float z, char *ktextx, int ijust, int dim)
{
	char *ktext;
	int ntext;

	ntext = strlen (ktextx);
	if (ntext == 0) return;

	ktext = (char *) gsgl_malloc_atomic ((ntext+1)*sizeof(char));
	gsaf_gudstr (ktextx, ktext);
	if (dim == 1)
		gsaf_gtext (x, y, ktext, ijust);
	else if (dim == 2)
		gsaf_gtext2d (x, y, ktext, ijust);
	else if (dim == 3)
		gsaf_gtext3d (x, y, z, ktext, ijust);
}

void gsaf_gtextx (float x, float y, char *ktextx, int ijust)
{
	if (!gsgl_inq_page_is_open ()) return;

	gtextx_dim (x, y, 0.0, ktextx, ijust, 1);
}

void gsaf_gtextx2d (float x, float y, char *ktextx, int ijust)
{
	if (!gsgl_inq_page_is_open ()) return;

	gtextx_dim (x, y, 0.0, ktextx, ijust, 2);
}

void gsaf_gtextx3d (float x, float y, float z, char *ktextx, int ijust)
{
	if (!gsgl_inq_page_is_open ()) return;

	gtextx_dim (x, y, z, ktextx, ijust, 3);
}

/* draw integer number */
static void numbi_dim (int i, char *kform, int nchar, int dim)
{
	char *ksend;
	
	ksend = (char *) gsgl_malloc_atomic (sizeof(char)*(nchar+1));
	if (sprintf (ksend, kform, i) != nchar) {
		gsgl_warning ("numbi", "sprintf returned != nchar");
		return;
	}
	if (dim == 1)
		gsaf_text (ksend);
	else if (dim == 2)
		gsaf_text2d (ksend);
	else if (dim == 3)
		gsaf_text3d (ksend);
}

void gsaf_numbi (int i, char *kform, int nchar)
{
	if (!gsgl_inq_page_is_open ()) return;

	numbi_dim (i, kform, nchar, 1);
}

void gsaf_numbi2d (int i, char *kform, int nchar)
{
	if (!gsgl_inq_page_is_open ()) return;

	numbi_dim (i, kform, nchar, 2);
}
void gsaf_numbi3d (int i, char *kform, int nchar)
{
	if (!gsgl_inq_page_is_open ()) return;

	numbi_dim (i, kform, nchar, 3);
}

/* draw double precision real number */
static void numbd_dim (double d, char *kform, int nchar, int dim)
{
	char *ksend;
	int n;
	
	ksend = (char *) gsgl_malloc_atomic (sizeof(char)*(nchar+1));
	if (sprintf (ksend, kform, d) != nchar) {
		gsgl_warning ("numbd", "sprintf returned != nchar");
		return;
	}
	for (n=0; n<nchar; n++) {
		if (ksend[n] == '-') {
			if (n >= 1 && n+1 < nchar && ksend[n+1] == '.') {
				ksend[n-1] = '-';
				ksend[n] = '0';
			}
		} else if (ksend[n] == '.') {
			if (n >= 1) {
				ksend[n-1] = '0';
			}
		}
	}
	if (dim == 1)
		gsaf_text (ksend);
	else if (dim == 2)
		gsaf_text2d (ksend);
	else if (dim == 3)
		gsaf_text3d (ksend);
}

void gsaf_numbd (double d, char *kform, int nchar)
{
	if (!gsgl_inq_page_is_open ()) return;

	numbd_dim (d, kform, nchar, 1);
}

void gsaf_numbd2d (double d, char *kform, int nchar)
{
	if (!gsgl_inq_page_is_open ()) return;

	numbd_dim (d, kform, nchar, 2);
}

void gsaf_numbd3d (double d, char *kform, int nchar)
{
	if (!gsgl_inq_page_is_open ()) return;

	numbd_dim (d, kform, nchar, 3);
}


/* draw real number */
static void numbr_dim (float r, char *kform, int nchar, int dim)
{
	double d;
	
	d = r;
	numbd_dim (d, kform, nchar, dim);
}

void gsaf_numbr (float r, char *kform, int nchar)
{
	if (!gsgl_inq_page_is_open ()) return;
	numbr_dim (r, kform, nchar, 1);
}

void gsaf_numbr2d (float r, char *kform, int nchar)
{
	if (!gsgl_inq_page_is_open ()) return;
	numbr_dim (r, kform, nchar, 2);
}

void gsaf_numbr3d (float r, char *kform, int nchar)
{
	if (!gsgl_inq_page_is_open ()) return;
	numbr_dim (r, kform, nchar, 3);
}

/* draw justified integer number */
static void gnumbi_dim (float x, float y, float z, int i, int ijust, int dim)
{
	char kvalue[26], *ktext;
	int n;
	int nchar;

	sprintf (kvalue, "%25d", i);

	for (n=0; n<25; n++) {
		if (kvalue[n] != ' ') break;
	}
	ktext = &kvalue[n];
	nchar = 25-n;
	if (dim == 1)
		gsaf_gtext (x, y, ktext, ijust);
	else if (dim == 2)
		gsaf_gtext2d (x, y, ktext, ijust);
	else if (dim == 3)
		gsaf_gtext3d (x, y, z, ktext, ijust);
}

void gsaf_gnumbi (float x, float y, int i, int ijust)
{
	if (!gsgl_inq_page_is_open ()) return;
	gnumbi_dim (x, y, 0.0, i, ijust, 1);
}

void gsaf_gnumbi2d (float x, float y, int i, int ijust)
{
	if (!gsgl_inq_page_is_open ()) return;
	gnumbi_dim (x, y, 0.0, i, ijust, 2);
}

void gsaf_gnumbi3d (float x, float y, float z, int i, int ijust)
{
	if (!gsgl_inq_page_is_open ()) return;
	gnumbi_dim (x, y, z, i, ijust, 3);
}


/* draw justified double precision real number */
static void gnumbd_dim (float x, float y, float z, double d, int ind, int ijust, int dim)
{
	char kvalue[26];
	char *ktext = NULL;
	/*int nchar;*/
	int m, n, nn;
	int is;
	
	if (ind > 0) {
		sprintf (kvalue, "%25.*f", ind, d);
	} else if (ind == 0) {
		if (d > 0.0) 
			sprintf (kvalue, "%25d", (int) (d+0.5));
		else
			sprintf (kvalue, "%25d", (int) (d-0.5));
	} else if (ind > -100) {
		sprintf (kvalue, "%25.*E", -ind, d);
	} else {
		sprintf (kvalue, "%25.*E", -ind-100, d);
	}

	for (n=0; n<25; n++)
		if (kvalue[n] != ' ') break;

	if (kvalue[n] == '-') {
		if (n >= 1 && kvalue[n+1] == '.') {
			kvalue[n-1] = '-';
			kvalue[n] = '0';
			n=n-1;
		}
	} else if (kvalue[n] == '.') {
		if (n >= 1) {
			kvalue[n-1] = '0';
			n=n-1;
		}
	}
	
	if (ind > -100) {
		ktext = &kvalue[n];
		/*nchar = 25-n;*/
	} else {
		ktext = (char *) gsgl_malloc_atomic (256*sizeof(char));

		for (m=n; m<25; m++)
			if (kvalue[m] == 'E') break;
		nn=m-1;
		if (ind == -100) nn=nn-1;
		
		m=m+1;
		if (kvalue[m] == '-') {
			is=-1;
		} else if (kvalue[m] == '+') {
			is=1;
		} else {
			is=1;
			m=m-1;
		}
		do {
			m=m+1;
		} while (m <= 24 && kvalue[m] == '0');
		if (is == 1) {
			if (ind == -100) nn += 1;
			sprintf (ktext, "%.*s$;$0x$#10$+%.*s$=",
				 nn-n+1, &kvalue[n], 25-m, &kvalue[m]);
			/*nchar=nn-n+1 + 11 + 24-m+1 + 2;*/
		} else {
			if (ind == -100) nn += 1;
			sprintf (ktext, "%.*s$;$0x$#10$+-%.*s$=", 
				 nn-n+1, &kvalue[n], 25-m, &kvalue[m]);
			/*nchar=nn-n+1 + 12 + 24-m+1 + 2;*/
		}
	}
	if (dim == 1)
		gsaf_gtext (x,y,ktext,ijust);
	else if (dim == 2)
		gsaf_gtext2d (x,y,ktext,ijust);
	else if (dim == 3)
		gsaf_gtext3d (x,y,z,ktext,ijust);
}

void gsaf_gnumbd (float x, float y, double d, int ind, int ijust)
{
	if (!gsgl_inq_page_is_open ()) return;
	gnumbd_dim (x, y, 0.0, d, ind, ijust, 1);
}

void gsaf_gnumbd2d (float x, float y, double d, int ind, int ijust)
{
	if (!gsgl_inq_page_is_open ()) return;
	gnumbd_dim (x, y, 0.0, d, ind, ijust, 2);
}

void gsaf_gnumbd3d (float x, float y, float z, double d, int ind, int ijust)
{
	if (!gsgl_inq_page_is_open ()) return;
	gnumbd_dim (x, y, z, d, ind, ijust, 3);
}

/* draw justified real number */
static void gnumbr_dim (float x, float y, float z, float r, int ind, int ijust, int dim)
{
	double d;

	d = r;
	if (dim == 1)
		gsaf_gnumbd (x, y, d, ind, ijust);
	else if (dim == 2)
		gsaf_gnumbd2d (x, y, d, ind, ijust);
	else if (dim == 3)
		gsaf_gnumbd3d (x, y, z, d, ind, ijust);
}

void gsaf_gnumbr (float x, float y, float r, int ind, int ijust)
{
	if (!gsgl_inq_page_is_open ()) return;
	gnumbr_dim (x, y, 0.0, r, ind, ijust, 1);
}

void gsaf_gnumbr2d (float x, float y, float r, int ind, int ijust)
{
	if (!gsgl_inq_page_is_open ()) return;
	gnumbr_dim (x, y, 0.0, r, ind, ijust, 2);
}

void gsaf_gnumbr3d (float x, float y, float z, float r, int ind, int ijust)
{
	if (!gsgl_inq_page_is_open ()) return;
	gnumbr_dim (x, y, z, r, ind, ijust, 3);
}


/* draw justified integer power */
static void gnumbp_dim (float x, float y, float z, int i, int j, int ijust, int dim)
{
	char kvalue[26], kpower[26], ktext[26];
	int n, m;
	int nchar;

	sprintf (kvalue, "%25d", i);
	for (n=0; n<25; n++)
		if (kvalue[n] != ' ') break;
	
	sprintf (kpower, "%25d", j);
	for (m=0; m<25; m++)
		if (kpower[m] != ' ') break;

	sprintf (ktext, "%.*s$+%.*s$=", 25-n, &kvalue[n], 25-m, &kpower[m]);
	nchar=25-n+2+25-m+2;
	if (dim == 1)
		gsaf_gtext (x,y,ktext,ijust);
	else if (dim == 2)
		gsaf_gtext2d (x,y,ktext,ijust);
	else if (dim == 3)
		gsaf_gtext3d (x,y,z,ktext,ijust);
}

void gsaf_gnumbp (float x, float y, int i, int j, int ijust)
{
	if (!gsgl_inq_page_is_open ()) return;
	gnumbp_dim (x, y, 0.0, i, j, ijust, 1);
}

void gsaf_gnumbp2d (float x, float y, int i, int j, int ijust)
{
	if (!gsgl_inq_page_is_open ()) return;
	gnumbp_dim (x, y, 0.0, i, j, ijust, 2);
}

void gsaf_gnumbp3d (float x, float y, float z, int i, int j, int ijust)
{
	if (!gsgl_inq_page_is_open ()) return;
	gnumbp_dim (x, y, z, i, j, ijust, 3);
}

/* draw pattern line */
static void drawpt_dim (float x, float y, float z, int dim, gsglPage *page)
{
	float dx, dy, dz = 0.0, dv, v;
	int ne, n;
	float x1 = 0.0, y1 = 0.0, z1 = 0.0;
	float x2, y2, z2;
	float xt2, yt2, zt2;
	float dx2, dy2, dz2;
	gsglpattern *p = page->pattern;

	if (p->Npatt <= 1) {
		if (dim == 1)
			gsaf_draw (x,y);
		else if (dim == 2) 
			gsaf_draw2d (x,y);
		else if (dim == 3) 
			gsaf_draw3d (x,y,z);
	} else {
		if (p->Nt%2 == 1) {
			if (dim == 1)
				gsaf_move (p->Xt, p->Yt);
			else if (dim == 2) 
				gsaf_move2d (p->Xt, p->Yt);
			else if (dim == 3) 
				gsaf_move3d (p->Xt, p->Yt, p->Zt);
		}
		dx = x-p->Xt;
		dy = y-p->Yt;
		if (dim == 3)
			dz = z-p->Zt;

		if (dim == 1) {
			x2 = x;
			y2 = y;
			xt2 = p->Xt;
			yt2 = p->Yt;
		} else if (dim == 2) {
			gsglproject2d (x, y, &x2, &y2, page);
			gsglproject2d (p->Xt, p->Yt, &xt2, &yt2, page);
		} else if (dim == 3) {
			gsglproject3d (x, y, z, &x2, &y2, &z2, page);
			gsglproject3d (p->Xt, p->Yt, p->Zt, &xt2, &yt2, &zt2, page);
		}
		dx2 = x2 - xt2;
		dy2 = y2 - yt2;
		if (dim == 3) {
			dz2 = z2 - zt2;
		} else {
			dz2 = 0.0;
		}
		dv = sqrt(dx2*dx2+dy2*dy2+dz2*dz2);
		v = p->Vt + dv;
		if (isnan(v) || isinf(v)) {
			gsgl_warning ("drawpt_dim", "vector length is invalid (%f)", v);
			gsgl_warning ("drawpt_dim", "your data are (x,y,z)=(%f,%f,%f)", x, y, z);
			return;
		}
		
		if (dv > GSGL_PAPER_WIDTH && dim != 2) {
			gsgl_warning ("drawpt_dim", "vector length is too long (%f)", dv);
			gsgl_warning ("drawpt_dim", "your data are (x,y,z)=(%f,%f,%f)", x, y, z);
			movept_dim (x, y, z, p->ipat_exception, dim, page);
			return;
		}
			
		/*printf ("v = %f\n", v);*/

		for (;;) {
			/*printf ("v (in loop) = %f\n", v);*/
			if (v > p->Pat[p->Npatt-1]) { /*!!!*/
				ne = p->Npatt;
				/*printf ("ne1 = %d\n", ne);*/
			} else {
				for (n=1; n<=p->Npatt; n++) {
					/*printf ("ne2 = %d, v=%f\n", n, v);*/
					if (v <= p->Pat[n-1]) break; /*!!!*/
				}
				ne=n-1;
			}
			/*printf ("ne = %d\n", ne);*/
			for (n=p->Nt; n<=ne; n++) {
				x1 = p->Xt+dx*(p->Pat[n-1]-p->Vt)/dv; /*!!!*/
				y1 = p->Yt+dy*(p->Pat[n-1]-p->Vt)/dv; /*!!!*/
				if (dim == 3) z1 = p->Zt+dz*(p->Pat[n-1]-p->Vt)/dv; /*!!!*/
				if (n%2 == 0) {
					if (dim == 1)
						gsaf_move (x1, y1);
					else if (dim == 2) 
						gsaf_move2d (x1, y1);
					else if (dim == 3) 
						gsaf_move3d (x1, y1, z1);
				} else {
					if (dim == 1)
						gsaf_draw (x1, y1);
					else if (dim == 2) 
						gsaf_draw2d (x1, y1);
					else if (dim == 3) 
						gsaf_draw3d (x1, y1, z1);
				}
			}
			if (ne != p->Npatt) break;
			p->Xt=x1;
			p->Yt=y1;
			if (dim == 3) p->Zt = z1;
			v=v-p->Pat[p->Npatt-1]; /*!!!*/
			p->Vt=0.0;
			p->Nt=1;
		}

		if (ne%2 == 0) {
			if (dim == 1)
				gsaf_draw (x,y);
			else if (dim == 2) 
				gsaf_draw2d (x,y);
			else if (dim == 3) 
				gsaf_draw3d (x,y,z);
		}
		p->Xt=x;
		p->Yt=y;
		if (dim == 3) p->Zt=z;
		p->Vt=v;
		p->Nt=ne+1;
	}
}

void gsaf_drawpt (float x, float y)
{
	if (!gsgl_inq_page_is_open ()) return;
	drawpt_dim (x, y, 0.0, 1, gsgl_inq_page());
}

void gsaf_drawpt2d (float x, float y)
{
	if (!gsgl_inq_page_is_open ()) return;
	drawpt_dim (x, y, 0.0, 2, gsgl_inq_page());
}
void gsaf_drawpt3d (float x, float y, float z)
{
	if (!gsgl_inq_page_is_open ()) return;
	drawpt_dim (x, y, z, 3, gsgl_inq_page());
}

/* initialize pattern line */
static void movept_dim (float x, float y, float z, int ipat, int dim, gsglPage *page)
{
	int npatss[15] = {2,2,2,4,4,6,6,0,0,0,0,0,0,0,0};
	float patss [15][10] = {
		{0.05, 0.20, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
		{0.25, 0.40, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
		{0.60, 0.80, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
		{0.50, 0.60, 0.7, 0.8, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
		{1.15, 1.30,1.45, 1.6, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
		{0.425,0.500,0.575,0.650,0.725,0.800,0.0, 0.0, 0.0, 0.0},
		{1.1,  1.2,  1.3, 1.4, 1.5, 1.6, 0.0, 0.0, 0.0, 0.0}};
	int i, n;
	gsglpattern *p = page->pattern;
	p->ipat_exception = ipat;

	for (i=0; i<15; i++) {
		p->Npats[i] = npatss[i];
		for (n=0; n<10; n++) {
			p->Pats[i][n] = patss[i][n];
		}
	}

	if (ipat >= 1 && ipat <= 15) {
		p->Npatt = p->Npats[ipat-1]; /*!!!*/
		for (n=0; n<p->Npatt; n++) {
			p->Pat[n] = p->Pats[ipat-1][n]; /*!!!*/
		}
	} else if (ipat <= -1 && ipat >= -15) {
		p->Npatt = p->Npats[-ipat-1]+1;
		for (n=0; n<p->Npatt-1; n++) {
			p->Pat[n] = p->Pats[-ipat-1][p->Npatt-1-1] - p->Pats[-ipat-1][p->Npatt-n-1-1]; /*!!!*/
		}
		p->Pat[p->Npatt-1] = p->Pats[-ipat-1][p->Npatt-1-1]; /*!!!*/
	} else if (ipat == 0) {
		p->Npatt=1;
	}

	if (p->Npatt == 1) {
		if (dim == 1)
			gsaf_move (x,y);
		else if (dim == 2)
			gsaf_move2d (x,y);
		else if (dim == 3)
			gsaf_move3d (x,y,z);
	} else {
		p->Xt=x;
		p->Yt=y;
		if (dim == 3) p->Zt = z;
		p->Vt=0.0;
		p->Nt=1;
	}
}

void gsaf_movept (float x, float y, int ipat)
{
	if (!gsgl_inq_page_is_open ()) return;
	movept_dim (x, y, 0.0, ipat, 1, gsgl_inq_page());
}

void gsaf_movept2d (float x, float y, int ipat)
{
	if (!gsgl_inq_page_is_open ()) return;
	movept_dim (x, y, 0.0, ipat, 2, gsgl_inq_page());
}

void gsaf_movept3d (float x, float y, float z, int ipat)
{
	if (!gsgl_inq_page_is_open ()) return;
	movept_dim (x, y, z, ipat, 3, gsgl_inq_page());
}

/* define line pattern */
void gsaf_setlpt (int ipat, int npat, float pat[10])
{
	float patsum;
	int n;
	gsglPage *page = gsgl_inq_page ();
	gsglpattern *p = page->pattern;

	if (!gsgl_inq_page_is_open ()) return;
	if (ipat < 8 || ipat > 15) return;
	if (npat < 0) return;

	p->Npats[ipat] = MIN(npat,10);
	patsum=0.0;
	for (n=0; n<p->Npats[ipat]; n++) {
		patsum = patsum + pat[n];
		p->Pats[ipat][n] = patsum;
	}
}

/* find minimum and maxmum of 1d array */
void gsaf_gmnmx1 (float *a, int ns, int ne, int nstep, float *amin, float *amax)
{
	int n;

	*amin = a[ns];
	*amax = a[ns];

	for (n=ns+nstep; n<=ne; n+=nstep) {
		*amin = MIN(*amin, a[n]);
		*amax = MAX(*amax, a[n]);
	}
}

int findex2 (int i, int j, int nxa)
{
	return i + j * nxa;
}

/* find minimum and maxmum of 2d array */
void gsaf_gmnmx2 (float *a, int nxa, int nxs, int nxe, int nxstep,
		  int nys, int nye, int nystep, float *amin, float *amax)
{
	int nx, ny;

	*amin = a[findex2(nxs, nys, nxa)];
	*amax = a[findex2(nxs, nys, nxa)];	
	for (nx=nxs; nx<=nxe; nx+=nxstep) {
		for (ny=nys; ny<=nye; ny+=nystep) {
			*amin = MIN(*amin, a[findex2(nx,ny,nxa)]);
			*amax = MAX(*amax, a[findex2(nx,ny,nxa)]);
		}
	}
}

/* choose graph minimum, maxmum and step */
void gsaf_gqscal (float amin, float amax, float *gmin1, float *gmax1, float *scale1)
{
	double dw, dlw;
	const float eps = 0.01;
	float raw;
	int ns, nlw;
	float gmin, gmax, scale;

	dw = (double) amax - (double) amin;
	if (dw <= 0.0) {
		if (dw < 0.0) {
			gmin = amax;
			gmax = amin;
			dw = -dw;
		} else if (fabs(amin) > 0.0) {
			gmin = amin - fabs(amin);
			gmax = amin + fabs(amin);
			dw = 2.0 * fabs((double) amin);
		} else {
			gmin = -1.0;
			gmax =  1.0;
			dw = 2.0;
		}
	} else {
		gmin = amin;
		gmax = amax;
	}

	dlw = log10 (dw);
	nlw = (int) (dlw + eps);
	if (dlw + eps < 0.0) nlw = nlw-1;
	raw = dw * pow(10.0, -nlw);

	if (raw > 6.0) {
		scale = 1.00 * pow(10.0, nlw);
	} else if (raw > 3.0) {
		scale = 0.50 * pow(10.0, nlw);
	} else if (raw > 2.0) {
		scale = 0.25 * pow(10.0, nlw);
	} else if (raw > 1.2) {
		scale = 0.20 * pow(10.0, nlw);
	} else { 
		scale = 0.10 * pow(10.0, nlw);
	}
	
	if (gmax >= 0.0) {
		if (gmin <= 0.0) {
			ns = (int) ( gmax/scale-eps+1.0);
			gmax = scale * ns;
			ns = (int) (-gmin/scale-eps+1.0);
			gmin = -scale * ns;
		} else {
			ns = (int) ( gmax/scale-eps+1.0);
			gmax = scale * ns;
			ns = (int) ( gmin/scale+eps);
			gmin = scale * ns;
		}
	} else {
		ns = (int) (-gmax/scale+eps);
		gmax = -scale * ns;
		ns = (int) (-gmin/scale-eps+1.0);
		gmin = -scale * ns;
	}
	*gmax1 = gmax;
	*gmin1 = gmin;
	*scale1 = scale;
}
