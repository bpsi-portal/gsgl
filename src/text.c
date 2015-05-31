#include "gsaf.h"
#include "gsgl.h"
#include "base.h"
#include "drv_wrapper.h"
#include "utils.h"
#include "displaylist.h"
#include "matrix.h"
#include "device.h"
#include "configfile.h"
#ifdef HAVE_LIBGICONV
#  include <giconv.h>
#else
#  include <iconv.h>
#endif
#ifdef HAVE_FREETYPE2
# include <ft2build.h>
# include FT_FREETYPE_H
# include FT_OUTLINE_H
# include FT_BBOX_H
# include FT_MODULE_H
# include FT_TRUETYPE_TABLES_H
# include FT_TYPE1_TABLES_H
# include FT_IMAGE_H
#endif
#include "text.h"
#include "textin.h"
#include "text_freetype.h"
#include "text_type1_kerning.h"
#include "bsc1.h"
#include "bsc2.h"
#include "text_hersey.h"
#include "text_texture.h"
#include "tcpip.h"

/*#define DEBUGFREETYPE*/
/*#define DEBUGSCRIPT*/
#define CACHE

#define MAX_CHAR 1024

#define NODE 5
#define MAX_FONTNUM 1028

#define NULLCHAR  -1
#define LATIN     0
#define GREEK     1
#define JAPANESE  2

#define SUBSIZE      0.7
#define JAPANESESIZE 0.961 /* 10pt vs 13Q */

static void gsgl_render_text (float *x, float *y, int *text, int num, gsglFontStatus *status);
static void gsgl_analyse_strings (int *text_uni, int *n, int nmax, float *x, float *y,
				  gsglFontStatus *status);
static void inq_intertext_size (int *text_uni, int nchar, float *width, float *height, 
				float *depth, gsglFontStatus *status);
static void textunicode (int *text_uni,int nmax,float *x,float *y, int dimension,
			 gsglboolean analyse, gsglPage *p);
static void push_charsize (gsglFontStatus *status);

static int fontnum_is (int num) 
{
	if (FONTNUM_IS_BASIC_LATIN(num)) {
		return LATIN;
	} else if (FONTNUM_IS_GREEK(num)) {
		return GREEK;
	} else if (FONTNUM_IS_JAPANESE(num)) {
		return JAPANESE;
	} else if (FONTNUM_IS_BAKOMA(num)) {
		return LATIN;
	} else {
		gsgl_error ("fontnum_is", "Unexpected font number %d", num);
		return LATIN;
	}
}

static gsglboolean use_freetype (int fnt, gsglPage *p)
{
#ifdef HAVE_FREETYPE2
	if (fnt >= 500) return GSGL_TRUE;
	else if (fnt >= 32 && fnt <= 44) {
		if (p->text->UseType1) return GSGL_TRUE;
		else return GSGL_FALSE;
	} else if (fnt <= 19) {
		return GSGL_FALSE;
	} else {
		gsgl_error ("use_freetype", "invalid fnt = %d", fnt);
		return GSGL_FALSE;
	}
#else
	return GSGL_FALSE;
#endif
	
}

static gsglboolean use_stroke_font (int fnt)
{
	if (fnt <= 19) {
		return GSGL_TRUE;
	} else {
		return GSGL_FALSE;
	}
}

static gsglboolean use_ps_font (int fnt)
{
	if (fnt >= 32 && fnt <= 44) {
		return GSGL_TRUE;
	} else {
		return GSGL_FALSE;
	}
}

static gsglboolean use_bitmap (int fnt, gsglFontStatus *s)
{
#ifdef HAVE_FREETYPE2
	if (s->p->text->UseBitmap_explicit) {
		return GSGL_TRUE;
	} else if (!s->dontdraw && use_ps_font(fnt) && s->p->text->UseBitmap_env && 
		   s->p->win->Antialiasing &&
		   (gsgl_inq_devicetype (s->p->library) == GSGL_DEVICE_X11 ||
		    gsgl_inq_devicetype (s->p->library) == GSGL_DEVICE_RASTER)) {
		return GSGL_TRUE;
	} else {
		return GSGL_FALSE;
	}
#else
	return GSGL_FALSE;
#endif
}

static gsglboolean use_bitmap_tcpip (int fnt, gsglFontStatus *s)
{
	if (!s->dontdraw && use_ps_font(fnt) && s->p->text->UseBitmap_env && 
	    gsgl_inq_devicetype (s->p->library) == GSGL_DEVICE_TCPIP)
		return GSGL_TRUE;
	else
		return GSGL_FALSE;
}

static gsglboolean use_tt_font (int fnt)
{
#ifdef HAVE_FREETYPE2
	if (fnt >= 1000) {
		return GSGL_TRUE;
	} else {
		return GSGL_FALSE;
	}
#else
	return GSGL_FALSE;
#endif
}

static gsglboolean use_type1ft_font (int fnt)
{
#ifdef HAVE_FREETYPE2
	if (fnt >= 500 && fnt < 1000) {
		return GSGL_TRUE;
	} else {
		return GSGL_FALSE;
	}
#else
	return GSGL_FALSE;
#endif
}

static void gsgl_scale_charsize (gsglFontStatus *status, float scale)
{
	status->user_chh = status->user_chh * scale;
	status->user_chw = status->user_chw * scale;
	status->user_chspace = status->user_chspace * scale;
	status->chh = status->chh * scale;
	status->chw = status->chw * scale;
	status->chspace = status->chspace * scale;

	push_charsize (status);
#ifndef CACHE
	if (use_freetype(status->fontnum, status->p) || use_bitmap(status->fontnum, status))
#else
	if (use_freetype(status->fontnum, status->p))
#endif
		gsft_setcharsize (status->chh, status->chh, status->angle, status->tilt, status->p);

	gs_sfstch (status->user_chh, status->user_chw, status->user_chspace,
		   status->angle, status->tilt, status->p);
}

static void gsgl_setcharsize (gsglFontStatus *status)
{
	push_charsize (status);
#ifndef CACHE
	if (use_freetype(status->fontnum, status->p) || use_bitmap(status->fontnum, status))
#else
	if (use_freetype(status->fontnum, status->p))
#endif
		gsft_setcharsize (status->chh, status->chh, status->angle, status->tilt, status->p);

	gs_sfstch (status->user_chh, status->user_chw, status->user_chspace,
		   status->angle, status->tilt, status->p);
}

static void gsgl_setfnt (gsglFontStatus *status)
{
	char *file;
	gsglText *t = status->p->text;

	if (status->fontnum < 0 || status->fontnum >= MAX_FONTNUM) {
		gsgl_warning ("gsgl_setfnt","setfnt is called with ifnt = %d", status->fontnum);
		gsgl_warning ("gsgl_setfnt", "ifnt is set to 32");
		status->fontnum = 32;
	}
	if (use_tt_font(status->fontnum)) {
		t->Pre_ttf_num[fontnum_is(status->fontnum)] = status->fontnum;
	} else if (use_type1ft_font(status->fontnum)) {
		t->Pre_type1_font_num[fontnum_is(status->fontnum)] = status->fontnum;
	} else if (use_ps_font(status->fontnum)) {
		t->Pre_ps_font_num[fontnum_is(status->fontnum)] = status->fontnum;
	} else if (use_stroke_font (status->fontnum)) {
		t->Pre_stroke_font_num[fontnum_is(status->fontnum)] = status->fontnum;
	}
#ifdef DEBUGFREETYPE
	fprintf(stderr, "fnt = %d\n", status->fontnum);
#endif
	if (use_tt_font(status->fontnum)) {
		gsft_init_font (gsgl_inq_ttfname (status->fontnum-1000, status->p->library), status);
	} else if (use_type1ft_font(status->fontnum)) {
		gsft_init_font (gsgl_inq_ttfname (status->fontnum-474, status->p->library), status);
	} else if (use_ps_font(status->fontnum) && t->UseType1) {
		gsft_init_font (gsgl_inq_ttfname (status->fontnum-4, status->p->library), status);
	} else if (use_bitmap(status->fontnum, status)) {
#ifdef CACHE
		if (!gsft_inq_loaded_bitmap_cache (status->fontnum, status)) {
		
			if (gsgl_inq_i_am_a_server (status->p->library))
				file = gsgl_inq_ttfname_server (status->fontnum-4, status->p->library);
			else
				file = gsgl_inq_ttfname (status->fontnum-4, status->p->library);

			if (!gsft_have_bitmap_cache (status->fontnum, file, t->size_point)) {
				gsft_create_bitmap_cache (status->fontnum, file, status);
			}
			gsft_read_bitmap_cache (status->fontnum, file, status);
		}
#else
		gsft_init_font (gsgl_inq_ttfname (status->fontnum-4, status->p->library), status);
#endif
	} else {
		status->fontenc = NONENC;
	}
	t->Fontenc = status->fontenc;
	t->Fontnum = status->fontnum;
	gsgl_setcharsize (status);
}

void gsglsetfnt (int fnt, gsglPage *p)
{
	gsglFontStatus *status;
	gsglText *t = p->text;

	status = gsglFontStatus_new (p);
	status->fontnum = fnt;
	status->user_chh = t->User_chh;
	status->user_chw = t->User_chw;
	status->user_chspace = t->User_chspace;
	status->chh = t->Chh;
	status->chw = t->Chw;
	status->chspace = t->Chspace;
	status->angle = t->Angle;
	status->tilt = t->Tilt;

	gsgl_setfnt (status);
}

void gsglinqfnt (int *fnt, gsglPage *p)
{
	*fnt = p->text->Fontnum;
}

static void push_charsize (gsglFontStatus *status)
{
	float charheight_point;
	gsglText *t = status->p->text;

	if (FONTNUM_IS_JAPANESE(t->Fontnum)) {
		status->currentchh = status->chh * JAPANESESIZE;
		status->currentchw = status->chw * JAPANESESIZE;
		status->currentchspace = status->chspace * JAPANESESIZE;
	} else {
		status->currentchh = status->chh;
		status->currentchw = status->chw;
		status->currentchspace = status->chspace;
	}
	
	charheight_point = status->chh / 2.54 * 72.0;
	status->pixel2cm = 64.0 * GSFT_RES / 2.54 * t->size_point / charheight_point;

	t->User_chh = status->user_chh;
	t->Chh = status->chh;
	t->Currentchh = status->currentchh;
	t->User_chw = status->user_chw;
	t->Chw = status->chw;
	t->Currentchw = status->currentchw;
	t->User_chspace = status->user_chspace;
	t->Chspace = status->chspace;
	t->Currentchspace = status->currentchspace;
	t->Pixel2cm = status->pixel2cm;
	t->Angle = status->angle;
	t->Tilt = status->tilt;
}

void gsglsetcharsize (float chh, float chw, float chsp, float angle, float tilt, gsglPage *p)
{
	gsglFontStatus *status;
	
	status = gsglFontStatus_new (p);
	
	status->user_chh = chh;
	status->user_chw = chw;
	status->user_chspace = chsp;
	status->chh = chh * 1.466667;
	status->chw = chw * 1.466667;
	status->chspace = chsp * 1.466667;
	status->angle = angle;
	status->tilt = tilt;

	gsgl_setcharsize (status);
}

void gsglinqchr (float *chh, float *chw, float *chsp, float *angle, float *tilt, gsglPage *p)
{
	gsglText *t = p->text;

	*chh = t->User_chh;
	*chw = t->User_chw;
	*chsp = t->User_chspace;
	*angle = t->Angle;
	*tilt = t->Tilt;
}

static void gsgl_get_height_depth_char (int text, float *h, float *d, gsglFontStatus *status)
{
	if (use_freetype(status->fontnum, status->p)) {
		gsft_get_height_depth_char (text, h, d, status);
	} else if (use_ps_font(status->fontnum)) {
		gsgl_type1_depth (text, d, status->p);
		gsgl_type1_height (text, h, status->p);
	} else {
		*h = status->user_chh;
		*d = 0.0;
	}
}

static void gsgl_TeX_math (float *x, float *y, gsglFontStatus *status)
{
	/* This function implements super/subscript typesetting rule
	 * described in TeX Book Appendix G.
	 */
	float u, v, hd;
	float sup_drop, sub_drop;
	float sub1, sup1;
	float sub2;
	float x_height;
	float theta, theta1, psi;
	float hx, dx;
	float hy, dy;

#ifdef DEBUGSCRIPT	
	printf ("%f, %f\n", status->atom1->h, status->atom1->d);
#endif
	hd = status->atom1->h - status->atom1->d;

	/* rule 18a */

	sup_drop = hd * 0.3;
	sub_drop = hd * 0.3;
	u = status->atom1->h - sup_drop;
	v = status->atom1->d + sub_drop;

	/* rule 18b */
	
	x_height = 0.6 * status->user_chh;
		
	if (status->atom1->sup == NULL) { /* subscript only */
		gsgl_get_height_depth_char (status->atom1->sub->text[0], &hx, &dx, status);
		sub1 = sub_drop;
		*y -= cos (status->angle * GSGL_DEG) * 
			MAX(v, MAX(sub1, hx * SUBSIZE - 0.8 * x_height));
		*x -= sin (-status->angle * GSGL_DEG) * 
			MAX(v, MAX(sub1, hx * SUBSIZE - 0.8 * x_height));
		return;
	}

	/* rule 18c */

	gsgl_get_height_depth_char (status->atom1->sup->text[0], &hx, &dx, status);
	sup1 = sup_drop;
	u = MAX(u, MAX(sup1, dx * SUBSIZE + 0.25 * x_height));

	/* rule 18d */

	if (status->atom1->sub == NULL) { /* superscript only */
		*y += u * cos(status->angle * GSGL_DEG);
		*x += u * sin(-status->angle * GSGL_DEG);;
		return;
	}

	sub2 = hd * 0.40;
	v = MAX(v, sub2);

	/* rule 18e */

	if (use_freetype(status->fontnum, status->p)) {
		theta = gsft_get_underline_thickness (status);
	} else if (use_ps_font(status->fontnum)) {
		if (status->fontnum <= 35) {
			theta1 = 50.0;
		} else if (status->fontnum <= 39) {
			if (status->fontnum <= 37) {
				theta1 = 50.0;
			} else {
				theta1 = 69.0;
			}
		} else if (status->fontnum <= 42) {
			theta1 = 50.0;
		} else {
			theta1 = 46.0;
		}
		gsgl_1000_scale_factor_to_cm (status->fontnum, theta1, &theta, status->p);
	} else {
		theta1 = 50.0;
		gsgl_1000_scale_factor_to_cm (32, theta1, &theta, status->p);
	}
	gsgl_get_height_depth_char (status->atom1->sup->text[0], &hx, &dx, status);
	gsgl_get_height_depth_char (status->atom1->sub->text[0], &hy, &dy, status);

	if ((u - dx * SUBSIZE)-(hy * SUBSIZE - v) >= 4.0 * theta) {
	} else {
		v = 4.0 * theta - u + hy * SUBSIZE + dx * SUBSIZE;
		psi = 0.8 * x_height - (u - dx * SUBSIZE);
		if (psi > 0) {
			u += psi;
			v -= psi;
		}
	}

	/* rule 18f */

	if (status->atom1->sup != NULL && status->atom1->sup->num != 0) {
		*y += u * cos(status->angle * GSGL_DEG);
		*x += u * sin(-status->angle * GSGL_DEG);
	} else if (status->atom1->sub != NULL && status->atom1->sub->num != 0) {
		*y -= v * cos(status->angle * GSGL_DEG);
		*x -= v * sin(-status->angle * GSGL_DEG);
	}
}

static void sub_string (int *from, int *to, int nmax)
{
	int n;

	for (n = 0; n < nmax; n++)
		to [n] = from [n];
}

static atom * atom_new (void)
{
	atom *atom1;

	atom1 = (atom *) gsgl_malloc (sizeof(atom));
	atom1->nucleus = GSGL_FALSE;
	atom1->sup = NULL;
	atom1->sub = NULL;
	atom1->text = NULL;
	atom1->num = 0;

	return atom1;
}

static void atom_free (atom *atom1)
{
	if (atom1 != NULL && atom1->text != NULL)
		gsgl_free (atom1->text);
	if (atom1 != NULL)
		gsgl_free (atom1);
}

static void gsgl_swap_script (int *n, int nmax, int *i, float *x, float *y, gsglFontStatus *status)
{
	float x_base, y_base;
	int   ntmp;
	atom  *atom2;
	float scriptspace;
	float degree;
	float width = -10.0;
	float len;
	float theta;

	if (use_freetype(status->fontnum, status->p)) {
		degree = gsft_get_italic_angle (status->p);
	} else if (use_ps_font(status->fontnum)) {
		if (status->fontnum == 32) degree = 0.0;
		else if (status->fontnum == 33) degree = -15.0;
		else if (status->fontnum == 34) degree = 0.0;
		else if (status->fontnum == 35) degree = -15.0;
		else if (status->fontnum == 36) degree = 0.0;
		else if (status->fontnum == 37) degree = -12.0;
		else if (status->fontnum == 38) degree = 0.0;
		else if (status->fontnum == 39) degree = -12.0;
		else if (status->fontnum == 40) degree = 0.0;
		else if (status->fontnum == 41) degree = -12.0;
		else if (status->fontnum == 42) degree = 0.0;
		else if (status->fontnum == 43) degree = -12.0;
		else if (status->fontnum == 44) degree = 0.0;
		else degree = 0.0;
	} else {
		degree = 0.0;
	}

	if (status->atom1->sup != NULL && status->atom1->sup->num != 0) {
		x_base = *x;
		y_base = *y;
		gsgl_TeX_math (x, y, status);
		
		scriptspace = (status->atom1->h + status->atom1->d) *
			(1.0 / 10.0 - tan(degree * GSGL_DEG) / 10.0); 

		/**x += tan(degree * GSGL_DEG) * (y_base + status->atom1->h - *y) * cos(status->angle*GSGL_DEG);
		 *y += tan(degree * GSGL_DEG) * (y_base + status->atom1->h - *y) * sin(status->angle*GSGL_DEG);*/
		
		gsgl_scale_charsize (status, SUBSIZE);
		
		ntmp = 0;
		atom2 = status->atom1;
		status->atom1 = status->atom1->sup;
		gsgl_analyse_strings (status->atom1->text, &ntmp, status->atom1->num, x, y, status);

		*x += scriptspace * cos(status->angle * GSGL_DEG);
		*y += scriptspace * sin(status->angle * GSGL_DEG);
		*i = 0;
		status->atom1 = atom2;
		atom_free (status->atom1->sup);
		status->atom1->sup = NULL;

		len = sqrt((*x-x_base)*(*x-x_base)+(*y-y_base)*(*y-y_base));
		theta = atan2 (-(*y-y_base), *x-x_base);
		width = len * cos (status->angle * GSGL_DEG + theta);
		
		
		if (status->atom1->sub != NULL && status->atom1->sub->num != 0) {
			*x = x_base;
			*y = y_base;
		} else {
			*x = x_base + width * cos(status->angle * GSGL_DEG); 
			*y = y_base + width * sin(status->angle * GSGL_DEG); 
		}
		gsgl_scale_charsize (status, 1.0/SUBSIZE);
	}
	if (status->atom1->sub != NULL && status->atom1->sub->num != 0) {
		x_base = *x;
		y_base = *y;
		gsgl_TeX_math (x, y, status);

		scriptspace = (status->atom1->h + status->atom1->d) *
			(1.0 / 10.0 - tan(degree * GSGL_DEG) / 10.0); 
		
		/**x += tan(degree * GSGL_DEG) * (y_base + status->atom1->h - *y) * cos(status->angle*GSGL_DEG);
		 *y += tan(degree * GSGL_DEG) * (y_base + status->atom1->h - *y) * sin(status->angle*GSGL_DEG);*/

		gsgl_scale_charsize (status, SUBSIZE);

		atom2 = status->atom1;
		status->atom1 = status->atom1->sub;
		ntmp = 0;
		gsgl_analyse_strings (status->atom1->text, &ntmp, status->atom1->num,
				      x, y, status);
		*x += scriptspace * cos(status->angle * GSGL_DEG);
		*y += scriptspace * sin(status->angle * GSGL_DEG);

		len = sqrt((*x-x_base)*(*x-x_base)+(*y-y_base)*(*y-y_base));
		theta = atan2 (-(*y-y_base), *x-x_base);
		width = MAX(width, len * cos (status->angle * GSGL_DEG + theta));

		*x = x_base + width * cos(status->angle * GSGL_DEG); 
		*y = y_base + width * sin(status->angle * GSGL_DEG);

		*i = 0;
		status->atom1 = atom2;
		atom_free (status->atom1->sub);
		status->atom1->sub = NULL;

		gsgl_scale_charsize (status, 1.0/SUBSIZE);
	}
}

static void gsgl_search_script (int *text_uni, int *n, int nmax, int *text_block, int *i,
				gsglFontStatus *status)
{
	int ntmp;
	int code, code_n1;
	int count = 0;

	for (ntmp = *n; ntmp < nmax; ntmp++) {
		code = text_uni [ntmp];
		code_n1 = text_uni [ntmp+1];
		
		/* search for $= */
		if ((code == DOLLAR || code == BACKSLASH) &&
			   (code_n1 == EQUAL)) {
			if (count == 0) {
				if (status->atom1->sup != NULL && 
				    status->atom1->sup->num == 0) {
					status->atom1->sup->num = *i;
					status->atom1->sup->text = gsgl_malloc_atomic (sizeof(int)*(*i));
					sub_string (text_block, status->atom1->sup->text,*i);
				} else if (status->atom1->sub != NULL &&
					   status->atom1->sub->num == 0) {
					status->atom1->sub->num = *i;
					status->atom1->sub->text = gsgl_malloc_atomic (sizeof(int)*(*i));
					sub_string (text_block, status->atom1->sub->text,*i);
				} else {
					gsgl_error ("gsgl_search_script", "");
				}
				*n = ntmp+2;
				break;
			} else {
				count -= 1;
				text_block [*i] = code;
				*i += 1;
			}
		} else if (ntmp == nmax - 1) {
			text_block [*i] = code;
			*i += 1;
			if (status->atom1->sup != NULL && status->atom1->sup->num == 0) {
				status->atom1->sup->num = *i;
				status->atom1->sup->text = gsgl_malloc_atomic (sizeof(int)*(*i));
				sub_string (text_block, status->atom1->sup->text,*i);
			} else if (status->atom1->sub != NULL && status->atom1->sub->num == 0) {
				status->atom1->sub->num = *i;
				status->atom1->sub->text = gsgl_malloc_atomic (sizeof(int)*(*i));
				sub_string (text_block, status->atom1->sub->text,*i);
			} else {
				gsgl_error ("gsgl_search_script", "");
			}
			*n = ntmp+1;
			/* search for $+ or $- */
		} else if ((code == DOLLAR || code == BACKSLASH) &&
			   (code_n1 == PLUS || code_n1 == MINUS)) {
			text_block [*i] = code;
			*i += 1;
			count += 1;
		} else {
			text_block [*i] = code;
			*i += 1;
		}
	}
	*i = 0;
}

static void gsgl_analyse_script (int *text_uni, int *n, int nmax, 
				 int *text_block, int *i, float *x, float *y,
				 gsglFontStatus *status)
{
	int code, code_n1, code_n2;

	if (!(*n < nmax - 2)) return;

#ifdef DEBUGSCRIPT
	printf ("nmin = %d, nmax = %d, code = 0x%0x (script)\n", *n, nmax, text_uni[*n]);
#endif	
	code = text_uni [*n];
	code_n1 = text_uni [*n+1];
	code_n2 = text_uni [*n+2];

	if (status->atom1 == NULL) status->atom1 = atom_new ();

	/* followed by $+ or $- */ /* X$+ or X$- */
	if ((code_n1 == DOLLAR || code_n1 == BACKSLASH) &&
	    (code_n2 == PLUS || code_n2 == MINUS)) {
		if (code == DOLLAR || code == BACKSLASH) {
			status->atom1->nucleus = GSGL_FALSE;
			return;
		} else {
			status->atom1->nucleus = GSGL_TRUE;
		}

		text_block [*i] = code;
		*i += 1;
		gsgl_render_text (x, y, text_block, *i, status);

		*i = 0;
		status->atom1->nucleus = GSGL_FALSE;
		
		if (code_n2 == PLUS) {
			status->atom1->sup = atom_new ();
		} else if (code_n2 == MINUS) {
			status->atom1->sub = atom_new ();
		} else {
			gsgl_error ("gsgl_analyse_script", "+ or - but neither + nor - ?");
		}
		*n += 3;

		/* $+ or $- ... for example $=$+  */
	} else if  ((code == DOLLAR || code == BACKSLASH) &&
		    (code_n1 == PLUS || code_n1 == MINUS)) {
		gsgl_render_text (x, y, text_block, *i, status);

		*i = 0;
		status->atom1->nucleus = GSGL_FALSE;
		status->atom1->h = status->p->text->Tmp_h;
		status->atom1->d = status->p->text->Tmp_d;

		if (code_n1 == PLUS) {
			status->atom1->sup = atom_new ();
		} else if (code_n1 == MINUS) {
			status->atom1->sub = atom_new ();
		} else {
			gsgl_error ("gsgl_analyse_script", "+ or - but neither + nor - ?");
		}
		*n += 2;
		
	} else {
		status->atom1->nucleus = GSGL_FALSE;
		return;
	}

	gsgl_search_script (text_uni, n, nmax, text_block, i, status);
	*i = 0;
	
#ifdef DEBUGSCRIPT
	printf ("*n = %d\n", *n);
#endif
	if (*n >= nmax-2) { /* don't followed by $+ or $- */
		status->disable4buaki = GSGL_TRUE;
		gsgl_swap_script (n, nmax, i, x, y, status);
		status->disable4buaki = GSGL_FALSE;
		gsgl_analyse_strings (text_uni, n, nmax, x, y, status);
#ifdef DEBUGSCRIPT
		printf ("exit script 1\n");
#endif
		return;
	}
	
	code = text_uni [*n];
	code_n1 = text_uni [*n+1];
	/* dosen't followed by $- or $+ */
	if (!(
		    (code == DOLLAR || code == BACKSLASH) &&
		    (code_n1 == PLUS || code_n1 == MINUS)
	      )
		) {
		status->disable4buaki = GSGL_TRUE;
		gsgl_swap_script (n, nmax, i, x, y, status);
		status->disable4buaki = GSGL_FALSE;
		gsgl_analyse_strings (text_uni, n, nmax, x, y, status);
#ifdef DEBUGSCRIPT
		printf ("exit script 2\n");
#endif
		return;	
	}

	/* followed by $- or $+ */
	if (code_n1 == PLUS) {
		status->atom1->sup = atom_new ();
	} else {
		status->atom1->sub = atom_new ();
	}
	*n += 2;
	gsgl_search_script (text_uni, n, nmax, text_block, i, status);
	status->disable4buaki = GSGL_TRUE;
	gsgl_swap_script (n, nmax, i, x, y, status);
	status->disable4buaki = GSGL_FALSE;
	gsgl_analyse_strings (text_uni, n, nmax, x, y, status);
#ifdef DEBUGSCRIPT
	printf ("exit script 3\n");
#endif
}

static int *gsgl_get_one_argument (int *text_uni, int *nmin, int nmax, int *size)
{
	int i;
	int *argument;
	int count = 0;

	for (i = *nmin; i < nmax; i++) {
		if (text_uni [i] == CURLY_KET) {
			if (count == 1) break;
			else count--;
		} else if (text_uni [i] == CURLY_BRA) {
			count++;
		}
	}
	*size = i - (*nmin) - 1;
	argument = (int *) gsgl_malloc_atomic ((*size)*sizeof(int));
	argument = &(text_uni [*nmin + 1]);
	return argument;
}

static int *gsgl_get_left_right_argument (int *text_uni, int *nmin, int nmax, int *size)
{
	int i;
	int *argument;
	int count = 0;

	for (i = *nmin; i < nmax-5; i++) {
		if ((text_uni [i] == BACKSLASH || text_uni [i] == DOLLAR) &&
		    text_uni[i+1] == SMALL_R &&
		    text_uni[i+2] == SMALL_I &&
		    text_uni[i+3] == SMALL_G &&
		    text_uni[i+4] == SMALL_H &&
		    text_uni[i+5] == SMALL_T) {
			if (count == 0) break;
			else count--;
		} else if ((text_uni [i] == BACKSLASH || text_uni [i] == DOLLAR) &&
			   text_uni[i+1] == SMALL_L &&
			   text_uni[i+2] == SMALL_E &&
			   text_uni[i+3] == SMALL_F &&
			   text_uni[i+4] == SMALL_T) 
			count++;
	}
	*size = i - (*nmin);
	argument = (int *) gsgl_malloc_atomic ((*size)*sizeof(int));
	argument = &(text_uni [*nmin]);
	return argument;
}

void gsgl_draw_parenthesis (float *x, float *y, int unicode, int fontnum, float size, float height, float depth, gsglFontStatus *status)
{
	int prefont;
	int argument[1];
	float size2, height2, depth2;
	float x_tmp, y_tmp;
	float x_save, y_save;
	float w;

	prefont = status->fontnum;
	status->fontnum = fontnum;
	gsgl_setfnt (status);
	
	argument[0] = unicode;
	inq_intertext_size (argument, 1, &size2, &height2, &depth2, status);

	y_tmp = *y + ((height+depth)*0.5-(height2+depth2)*0.5) * cos(status->angle*GSGL_DEG);
	x_tmp = *x - ((height+depth)*0.5-(height2+depth2)*0.5) * sin(status->angle*GSGL_DEG);
	x_save = x_tmp;
	y_save = y_tmp;

	gsgl_render_text (&x_tmp, &y_tmp, argument, 1, status);
	
	w = sqrt(pow(x_save-x_tmp,2.0)+pow(y_save-y_tmp,2.0));

	*y += w * sin(status->angle*GSGL_DEG);
	*x += w * cos(status->angle*GSGL_DEG);

	status->fontnum = prefont;
	gsgl_setfnt (status);
}

void gsgl_draw_sqrt (float *x0,float *y0, float depth, float height, float size,
		     float chh, float chw, gsglFontStatus *status)
{
	float x, y;
	float w, w1, ws;
	int prefont;
	int rt;
	float sizert, depthrt, heightrt;
	float factor;

	prefont = status->fontnum;
	if (use_tt_font(status->fontnum)) {
		status->fontnum = 1027;
	} else if (use_type1ft_font(status->fontnum)) {
		status->fontnum = 514;
	} else if (use_ps_font(status->fontnum)) {
		status->fontnum = 44;
	} else {
		status->fontnum = 17;
	}
	gsgl_setfnt (status);
	
	if (status->fontnum == 1027) {
		rt = SMALL_Q;
	} else if (status->fontnum == 514 || status->fontnum == 44) {
		rt = 3*64+2*8+6;
	} else {
		rt = SEVEN;
	}

	inq_intertext_size (&rt,1,&sizert,&heightrt,&depthrt,status);
	
	factor = (chh * 0.22 + height - depth) / (heightrt - depthrt);
	if (status->fontnum == 17) {
		factor *= 0.85;
	}

	gsgl_scale_charsize (status, factor);

	if (status->fontnum == 44 || status->fontnum == 514 || status->fontnum == 17) {  
		y = *y0 + depth; 
	} else if (status->fontnum == 1027) {
		y = *y0 + depth - factor * depthrt;
	} else {
		y = *y0;
	}
	gsgl_render_text (x0, &y, &rt, 1, status);
	if (status->fontnum == 17) {
		*x0 += heightrt * 0.05;
	}

	gsaf_inqlnw (&ws);
	w1 = 50.0 * 0.5;
	gsgl_1000_scale_factor_to_cm (status->fontnum, w1, &w, status->p);
	gsaf_setlnw (w);
		
	if (status->fontnum == 44 || status->fontnum == 514) {
		x = *x0 + (height + chh * 0.22 - 1.5*w) * sin(-status->angle * GSGL_DEG)
			+ (-heightrt * factor * 0.065) * cos(status->angle * GSGL_DEG);
		
		y = *y0 + (height + chh * 0.22 - 1.5*w) * cos(status->angle * GSGL_DEG)
			+ (-heightrt * factor * 0.065) * sin(status->angle * GSGL_DEG);
		
		if (!status->dontdraw) {
			gsgl_move (x, y, status->p);
		}

		x = *x0 + (chw * 0.13 + size) * cos(status->angle * GSGL_DEG)
			+ (height + chh * 0.22 -1.5*w) * sin(-status->angle * GSGL_DEG);
		
		y = *y0 + (chw * 0.13 + size) * sin(status->angle * GSGL_DEG)
			+ (height + chh * 0.22 -1.5*w) * cos(status->angle * GSGL_DEG);
	} else if (status->fontnum == 1027) {
		x = *x0 + (height + chh * 0.22) * sin(-status->angle * GSGL_DEG)
			+ (-heightrt * factor * 0.065) * cos(status->angle * GSGL_DEG);
		
		y = *y0 + (height + chh * 0.22) * cos(status->angle * GSGL_DEG)
			+ (-heightrt * factor * 0.065) * sin(status->angle * GSGL_DEG);
		
		if (!status->dontdraw) {
			gsgl_move (x, y, status->p);
		}

		x = *x0 + (chw * 0.13 + size) * cos(status->angle * GSGL_DEG)
			+ (height + chh * 0.22) * sin(-status->angle * GSGL_DEG);
		
		y = *y0 + (chw * 0.13 + size) * sin(status->angle * GSGL_DEG)
			+ (height + chh * 0.22) * cos(status->angle * GSGL_DEG);
	} else if (status->fontnum == 17) {
		x = *x0 + (height + chh * 0.22 + w) * sin(-status->angle * GSGL_DEG)
			+ (-heightrt * factor * 0.06) * cos(status->angle * GSGL_DEG);
		
		y = *y0 + (height + chh * 0.22 + w) * cos(status->angle * GSGL_DEG)
			+ (-heightrt * factor * 0.06) * sin(status->angle * GSGL_DEG);
		
		if (!status->dontdraw) {
			gsgl_move (x, y, status->p);
		}
		
		x = *x0 + (chw * 0.13 + size) * cos(status->angle * GSGL_DEG)
			+ (height + chh * 0.22 + w) * sin(-status->angle * GSGL_DEG);
		
		y = *y0 + (chw * 0.13 + size) * sin(status->angle * GSGL_DEG)
			+ (height + chh * 0.22 + w) * cos(status->angle * GSGL_DEG);
	} else {
		gsgl_warning ("gsgl_draw_sqrt","status->fontnum=%d",status->fontnum);
		x = *x0;
		y = *y0;
	}
		
	if (!status->dontdraw) {
		if (status->dimension == 1)
			gsgl_draw (x, y, status->p);
		else if (status->dimension == 2)
			gsgl_draw_text2d (x, y, status->p);
		else if (status->dimension == 3)
			gsgl_draw_text3d (x, y, status->p);
	}
	gsaf_setlnw (ws);

	status->fontnum = prefont;
	gsgl_setfnt (status);

	gsgl_scale_charsize (status, 1.0/factor);
}

typedef struct _control_sequence {
	int chars;
	int unicode[20];
} control_sequence;

static gsglboolean gsgl_analyse_control_sequence (int *text_uni, int *nmin, int nmax, float *x, float *y, gsglFontStatus *status)
{
	int i, j;
	int *sequence;
	gsglboolean bool;
	int length, length2;
	int *argument1, *argument2;
	float size, height, depth;
	float size2, height2, depth2;
	float x_tmp, y_tmp;
	float h, w;
	int nmin_tmp;
	int prefont;
	int bra, ket;
	float lw, lws, lw1;
	static float save_size = 0.0, save_height = 0.0, save_depth = 0.0;
#define CS_NUM 5
	control_sequence cs[CS_NUM] = {
		{4, {SMALL_S, SMALL_Q, SMALL_R, SMALL_T}}, /* sqrt */
		{4, {SMALL_F, SMALL_R, SMALL_A, SMALL_C}}, /* frac */
		{3, {SMALL_D, SMALL_E, SMALL_L}}, /* del */
		{4, {SMALL_L, SMALL_E, SMALL_F, SMALL_T}}, /* left */
		{5, {SMALL_R, SMALL_I, SMALL_G, SMALL_H, SMALL_T}} /* right */
	};

	for (i = *nmin; i < nmax; i++) {
		if (text_uni [i] == CURLY_KET ||
		    text_uni [i] == CURLY_BRA ||
		    text_uni [i] == SPACE ||
		    text_uni [i] == BACKSLASH ||
		    text_uni [i] == DOLLAR ||
		    text_uni [i] == LEFT_SQUARE_BRACKET ||
		    text_uni [i] == LEFT_PARENTHESIS ||
		    text_uni [i] == RIGHT_SQUARE_BRACKET ||
		    text_uni [i] == RIGHT_PARENTHESIS ||
		    text_uni [i] == PERIOD ||
		    text_uni [i] == VERTICAL_LINE) {
			break;
		}
	}
	length = i - (*nmin);
	
	sequence = (int *) gsgl_malloc_atomic (length*sizeof(int));
	sequence = &(text_uni [*nmin]);

	for (i=0; i<CS_NUM; i++) {
	    if (cs[i].chars == length) {
		 bool = GSGL_TRUE;
		 for (j=0; j<length; j++) {
		     if (cs[i].unicode[j] != sequence[j])
			     bool = GSGL_FALSE;
		 }
		 if (bool) {
		     (*nmin) += length;
		     length = 0;
		     switch (i) {
		     case 0: /* sqrt */
			     argument1 = gsgl_get_one_argument (text_uni, nmin, nmax, &length);
			     inq_intertext_size (argument1,length,&size,&height,&depth,status);
			     gsgl_draw_sqrt (x, y, depth, height, size, 
					     status->user_chh, status->user_chw, status);

			     /* draw text */
			     nmin_tmp = 0;
			     gsgl_analyse_strings (argument1, &nmin_tmp, length, x, y, status);

			     *x += status->user_chw * 0.13 * cos(status->angle * GSGL_DEG);
			     *y += status->user_chw * 0.13 * sin(status->angle * GSGL_DEG);
			     *nmin += length + 1;
			     return GSGL_TRUE;
		     case 1: /* frac */ 
			     argument1 = gsgl_get_one_argument (text_uni, nmin, nmax, &length);
			     *nmin += length + 2;
			     argument2 = gsgl_get_one_argument (text_uni, nmin, nmax, &length2);
			     *nmin += length2 + 1;
			     inq_intertext_size (argument1, length, &size, 
						 &height, &depth, status);
			     inq_intertext_size (argument2, length2, &size2, 
						 &height2, &depth2, status);
			     /* draw a line */
			     *x += status->user_chw * 0.2 * cos(status->angle * GSGL_DEG);
			     *y += status->user_chw * 0.2 * sin(status->angle * GSGL_DEG);
			     x_tmp = *x;
			     y_tmp = *y;
			     
			     *y += status->user_chh * 0.5 * cos(status->angle * GSGL_DEG);
			     *x -= status->user_chh * 0.5 * sin(status->angle * GSGL_DEG);

			     if (!status->dontdraw) {
				     gsaf_inqlnw (&lws);
				     lw1 = 50.0 * 0.5;
				     gsgl_1000_scale_factor_to_cm (status->fontnum, lw1, &lw, status->p);
				     gsaf_setlnw (lw);
			     }

			     if (!status->dontdraw) {
				     gsgl_move (*x, *y, status->p);
			     }
			     *x += MAX(size, size2) * cos(status->angle * GSGL_DEG);
			     *y += MAX(size, size2) * sin(status->angle * GSGL_DEG);
			     if (!status->dontdraw) {
				     if (status->dimension == 1)
					     gsgl_draw (*x, *y, status->p);
				     else if (status->dimension == 2)
					     gsgl_draw_text2d (*x, *y, status->p);
				     else if (status->dimension == 3)
					     gsgl_draw_text3d (*x, *y, status->p);
				     gsaf_setlnw (lws);
			     }
			     /* draw tests */
			     nmin_tmp = 0;

			     h = status->user_chh * 0.5 - depth + 0.25 * status->user_chh;
			     w = 0.5 * (MAX(size,size2) - size);
			     *y = y_tmp + h * cos(status->angle * GSGL_DEG)
				        + w * sin(status->angle * GSGL_DEG);
			     *x = x_tmp - h * sin(status->angle * GSGL_DEG)
				        + w * cos(status->angle * GSGL_DEG);

			     gsgl_analyse_strings (argument1, &nmin_tmp, length, x, y, status);
			     nmin_tmp = 0;

			     h = status->user_chh * 0.5 - height2 - 0.25 * status->user_chh;
			     w = 0.5 * (MAX(size,size2) - size2);
			     *y = y_tmp + h * cos(status->angle * GSGL_DEG)
				        + w * sin(status->angle * GSGL_DEG);
			     *x = x_tmp - h * sin(status->angle * GSGL_DEG)
				        + w * cos(status->angle * GSGL_DEG);

			     gsgl_analyse_strings (argument2, &nmin_tmp, length2, x, y, status);
			     
			     w = MAX(size, size2) + status->user_chw * 0.2;

			     *x = x_tmp + w * cos(status->angle * GSGL_DEG);
			     *y = y_tmp + w * sin(status->angle * GSGL_DEG);
			     
			     return GSGL_TRUE;
		     case 2: /* del */
			     prefont = status->fontnum;
			     if (use_tt_font(status->fontnum)) {
				     status->fontnum = 1025;
			     } else if (use_type1ft_font(status->fontnum)) {
				     status->fontnum = 514;
			     } else if (use_ps_font(status->fontnum)) {
				     status->fontnum = 44;
			     } else {
				     status->fontnum = 17;
			     }
			     gsgl_setfnt (status);
			     
			     argument1 = (int *) gsgl_malloc_atomic (sizeof(int));
			     if (status->fontnum == 1025) {
				     argument1[0] = AT;
			     } else if (status->fontnum == 514 || status->fontnum == 44) {
				     argument1[0] = 182;
			     } else {
				     argument1[0] = CAPITAL_A;
			     }
			     gsgl_render_text (x, y, argument1, 1, status);
			     
			     if (text_uni [*nmin] != SPACE)
				     *nmin -= 1;
			     
			     status->fontnum = prefont;
			     gsgl_setfnt (status);

			     return GSGL_TRUE;
		     case 3: /* left */
			     if (text_uni[*nmin] == LEFT_SQUARE_BRACKET ||
				 text_uni[*nmin] == LEFT_PARENTHESIS ||
				 text_uni[*nmin] == VERTICAL_LINE) {
				     bra = text_uni[*nmin];
				     (*nmin) ++;
			     } else if (
				     (text_uni[*nmin] == BACKSLASH ||
				      text_uni[*nmin] == DOLLAR) && 
				     text_uni [(*nmin)+1] == CURLY_BRA) {
				     bra = text_uni[(*nmin)+1];
				     (*nmin) += 2;
			     } else if (text_uni[*nmin] == PERIOD) {
				     bra = -1;
				     (*nmin) ++;
			     } else {
				     bra = text_uni[*nmin];
				     (*nmin) ++;
			     }
			     argument1 = gsgl_get_left_right_argument 
				     (text_uni, nmin, nmax, &length);
			     inq_intertext_size (argument1, length, &size, 
						 &height, &depth, status);
			     save_size = size;
			     save_height = height;
			     save_depth = depth;
			     if (bra >= 0) {
				     if (bra == LEFT_PARENTHESIS) {
					     if (height-depth < status->chh) {
						     gsgl_draw_parenthesis (x, y, 0x28, 1022, size, height, depth, status);
					     } else if (height-depth < status->chh * 2.2) {
						     gsgl_draw_parenthesis (x, y, 0xb3, 1027, size, height, depth, status);
					     } else {
						     gsgl_draw_parenthesis (x, y, 0x20, 1027, size, height, depth, status);
					     }
				     }
			     }
			     nmin_tmp = 0;
			     gsgl_analyse_strings (argument1, &nmin_tmp, length, x, y, status);
			     *nmin += length;
			     *nmin += -1;
			     
			     return GSGL_TRUE;
		     case 4: /* right */
			     if (text_uni[*nmin] == RIGHT_SQUARE_BRACKET ||
				 text_uni[*nmin] == RIGHT_PARENTHESIS ||
				 text_uni[*nmin] == VERTICAL_LINE) {
				     ket = text_uni[*nmin];
				     (*nmin) ++;
			     } else if (
				     (text_uni[*nmin] == BACKSLASH ||
				      text_uni[*nmin] == DOLLAR) && 
				     text_uni [(*nmin)+1] == CURLY_KET) {
				     ket = text_uni[(*nmin)+1];
				     (*nmin) += 2;
			     } else if (text_uni[*nmin] == PERIOD) {
				     ket = -1;
				     (*nmin) ++;
			     } else {
				     ket = text_uni[*nmin];
				     (*nmin) ++;
			     }
			     
			     size = save_size;
			     height = save_height;
			     depth = save_depth;
			     if (ket >= 0) {
				     if (ket == RIGHT_PARENTHESIS) {
					     if (height-depth < status->chh) {
						     gsgl_draw_parenthesis (x, y, 0x29, 1022, size, height, depth, status);
					     } else if (height-depth < status->chh * 2.2) {
						     gsgl_draw_parenthesis (x, y, 0xb4, 1027, size, height, depth, status);
					     } else {
						     gsgl_draw_parenthesis (x, y, 0x21, 1027, size, height, depth, status);
					     }
				     }
			     }
			     save_size = 0.0;
			     save_height = 0.0;
			     save_depth = 0.0;
			     (*nmin)--;
			     return GSGL_TRUE;
		     }
		 }
	    }
	}
	return GSGL_FALSE;
}

static void gsgl_analyse_strings (int *text_uni, int *nmin, int nmax, 
				  float *x, float *y, gsglFontStatus *status)
{
	int code;
	int i = 0;
	gsglboolean esc = GSGL_FALSE;
	int text_block [MAX_CHAR];
	int *argument;
	int tmp;
	gsglboolean math = GSGL_FALSE;
	int j, order;
	gsglText *t = status->p->text;

#ifdef DEBUGSCRIPT
	printf ("nmin = %d nmax = %d, code = 0x%0x (string)\n", 
		*nmin, nmax, text_uni[*nmin]);
#endif
	for (; *nmin < nmax ;) {
		code = text_uni [*nmin];
		if (!esc) {
			gsgl_analyse_script (text_uni, nmin, nmax, text_block, &i, x, y, status);
#ifdef DEBUGSCRIPT
			printf ("nmin = %d (ex string)\n", *nmin);
#endif
			if (*nmin < nmax) {
				code = text_uni [*nmin];
			} else {
#ifdef DEBUGSCRIPT			
				printf ("exit string 1\n");
#endif
				return;
			}
			switch (code) {
			case BACKSLASH:
			case DOLLAR:
				esc = GSGL_TRUE;
				break;
			default:
				text_block [i] = code;
				i += 1;
				break;
			}
		} else {
			gsgl_render_text (x, y, text_block, i, status);
			i = 0;
			switch (code) {
			case SHARP: /* greek */
				if (status->changefonttemp) {
					status->fontnum = status->prevfont;
					gsgl_setfnt (status);
				} else if (!math) {
					if (use_tt_font(status->fontnum)) {
						status->fontnum = t->Pre_ttf_num [GREEK];
					} else if (use_type1ft_font(status->fontnum)) {
						status->fontnum = t->Pre_type1_font_num [GREEK];
					} else if (use_ps_font(status->fontnum)) {
						status->fontnum = t->Pre_ps_font_num [GREEK];
					} else {
						status->fontnum = t->Pre_stroke_font_num [GREEK];
					}
					gsgl_setfnt (status);
					math = GSGL_TRUE;
				} else {
					if (use_tt_font(status->fontnum)) {
						status->fontnum = t->Pre_ttf_num [LATIN];
					} else if (use_type1ft_font(status->fontnum)) {
						status->fontnum = t->Pre_type1_font_num [LATIN];
					} else if (use_ps_font(status->fontnum)) {
						status->fontnum = t->Pre_ps_font_num [LATIN];
					} else {
						status->fontnum = t->Pre_stroke_font_num [LATIN];
					}
					status->changefonttemp = GSGL_FALSE;
					gsgl_setfnt (status);
					math = GSGL_FALSE;
				}
				break;
			case ZERO:
			case ONE:
			case TWO:
			case THREE:
			case FOUR:
			case FIVE:
			case SIX:
			case SEVEN:
			case EIGHT:
			case NINE:
				status->prevfont = status->fontnum;
				if (use_tt_font(status->fontnum)) {
					status->fontnum = code - ZERO + 1000; 
				} else if (use_type1ft_font(status->fontnum)) {
					status->fontnum = code - ZERO + 502; 
				} else if (use_ps_font(status->fontnum)) {
					if (code == ZERO) {
						/* Fix me ... incompatible with GSAF */
						status->fontnum = code - ZERO + 40;
					} else if (code == ONE) {
						status->fontnum = 41;
					} else {
						status->fontnum = code - ZERO + 30;
					}
				} else {
					status->fontnum = code - ZERO;
				}
				status->changefonttemp = GSGL_TRUE;
				gsgl_setfnt (status);
				break;
			case COMMA:
				*x += status->currentchh / 6.0*cos(status->angle*GSGL_DEG);
				*y += status->currentchh / 6.0*sin(status->angle*GSGL_DEG);
				break;
			case SEMICOLON:
				*x += status->currentchh*2.0/9.0*cos(status->angle*GSGL_DEG);
				*y += status->currentchh*2.0/9.0*sin(status->angle*GSGL_DEG);
				break;
			case EXCLAMATION:
				*x -= status->currentchh / 6.0*cos(status->angle*GSGL_DEG);
				*y -= status->currentchh / 6.0*sin(status->angle*GSGL_DEG);
				break;
			case QUESTION:
				*x -= status->currentchh * 2.0/9.0*cos(status->angle*GSGL_DEG);
				*y -= status->currentchh * 2.0/9.0*sin(status->angle*GSGL_DEG);
				break;
			case ASTERISK:
				*x -= status->currentchh*cos(status->angle*GSGL_DEG);
				*y -= status->currentchh*sin(status->angle*GSGL_DEG);
				break;
			case SLASH:
				text_block [i] = BACKSLASH;
				i += 1;
				break;
			case CURLY_BRA:
				argument = gsgl_get_one_argument (text_uni, nmin, nmax, &order);
				tmp = 0;
				for (j = 0; j < order; j++) {
					tmp += (argument[j] - ZERO) * pow (10, order-j-1);
				}
				if (status->fontnum>=1000) {
					tmp += 1000;
				} else if (status->fontnum>=502) {
					tmp += 500;
				} else if (status->fontnum>=32) {
					if (tmp >= 2) tmp += 30;
					else tmp += 40;
				} else {
				}
				*nmin += order + 1;
				status->fontnum = tmp;
				gsgl_setfnt (status);
				break;
			default:
				if (!gsgl_analyse_control_sequence 
				    (text_uni, nmin, nmax, x, y, status)) {
					text_block [i] = code;
					i = i+1;
				}
				break;
			}
			esc = GSGL_FALSE;
		}
		*nmin += 1;
	}
	/*fprintf (stderr, "textnum = %d fontnum = %d\n", i, status->fontnum);*/
	gsgl_render_text (x, y, text_block, i, status);
	i = 0;
	status->prechar = NULLCHAR;
#ifdef DEBUGSCRIPT
	printf ("exit string 2\n");
#endif
}

gsglFontStatus * gsglFontStatus_new (gsglPage *p)
{
	gsglFontStatus *status;
	gsglText *t = p->text;

	status = gsgl_malloc (sizeof(gsglFontStatus));	
	
	status->user_chh = t->User_chh;
	status->chh = t->Chh;
	status->currentchh = t->Currentchh;
	status->user_chw = t->User_chw;
	status->chw = t->Chw;
	status->currentchw = t->Currentchw;
	status->user_chspace = t->User_chspace;
	status->chspace = t->Chspace;
	status->currentchspace = t->Currentchspace;
	status->pixel2cm = t->Pixel2cm;
	status->angle = t->Angle;
	status->tilt = t->Tilt;

	status->fontenc = t->Fontenc;
	status->fontnum = t->Fontnum;
	status->polygonfont = t->Polygonfont;

	status->atom1 = NULL;
	status->dontdraw = GSGL_FALSE;
	status->disable4buaki = GSGL_FALSE;
	status->prechar = t->Prechar;
	status->dimension = 0;
	status->ymin = 0;
	status->ymax = 0;
	status->texts = NULL;
	status->p = p;
	status->changefonttemp = GSGL_FALSE;
	status->prevfont = 0;

	return status;
}

void text_to_unicode (char *text1, int textnum, int *text_uni, int *textnum_uni, gsglPage *p)
{
	char buf [MAX_CHAR];
	char* out = buf;
	size_t bufsize = MAX_CHAR;
	size_t bufleft = bufsize;
	size_t len;
	iconv_t cd;
	int tmp;
	int i, j, n;
	char  text [MAX_CHAR];
	char *text_iconv;

	for (i=0; i < textnum; i++) {
		text [i] = text1 [i];
	}
	text [textnum] = '\0';
	len = strlen(text);
	text_iconv = text;

	cd = iconv_open("UCS-4BE", p->text->In_code);
	iconv(cd, &text_iconv, &len, &out, &bufleft);
	iconv_close(cd);

	n = bufsize - bufleft;

	if (n%4 != 0) {
		gsgl_warning ("text_to_unicode", "Failed to convert to unicode");
		return;
	}
	*textnum_uni = n/4;

	for (j=0; j < *textnum_uni; j++) {
		text_uni [j] = 0;
		for (i=0; i<4; i++) {
			tmp = buf[j*4+i];
			if (tmp < 0) tmp = 256 + tmp;
			text_uni [j] += tmp * (int) pow (256, 4-i-1);
		}
	}
}

static void store_status (float *user_chh, float *user_chw, float *user_chspace,
			  float *chh, float *chw, float *chspace,
			  float *tilt, float *angle, int *fontnum, gsglFontStatus *status)
{
	*user_chh = status->user_chh;
	*user_chw = status->user_chw;
	*user_chspace = status->user_chspace;
	*chh = status->chh;
	*chw = status->chw;
	*chspace = status->chspace;
	*angle = status->angle;
	*tilt = status->tilt;
	*fontnum = status->fontnum;
}

static void restore_status (float user_chh, float user_chw, float user_chspace,
			    float chh, float chw, float chspace,
			    float tilt, float angle, int fontnum, gsglFontStatus *status)
{
	status->user_chh = user_chh;
	status->user_chw = user_chw;
	status->user_chspace = user_chspace;
	status->chh = chh;
	status->chw = chw;
	status->chspace = chspace;
	status->tilt = tilt;
	status->angle = angle;
	status->fontnum = fontnum;

	gsgl_setcharsize (status);
}

static void text_unicode_analyse_and_draw (int *text_uni, int nmax, float *x, float *y, gsglFontStatus *status, gsglPage *p)
{
	int i;
	float x2, y2;
	float user_chh, user_chw, user_chspace, chh, chw, chspace, tilt, angle;
	int   fontnum, fontnum_old;
	
	store_status (&user_chh, &user_chw, &user_chspace, 
		      &chh, &chw, &chspace, &tilt, &angle, &fontnum, status);

	i = 0;
	x2 = *x;
	y2 = *y;
	gsgl_analyse_strings (text_uni, &i, nmax, &x2, &y2, status);
	*x = x2;
	*y = y2;
	fontnum_old = status->fontnum;

	restore_status (user_chh, user_chw, user_chspace,
			chh, chw, chspace, tilt, angle, fontnum, status);
	if (fontnum != fontnum_old)
		gsgl_setfnt (status);
}

void gsgltext (char *text1, int textnum, float *x, float *y, int dimension, gsglPage *p)
{
#if defined DEBUGSCRIPT || defined DEBUGTEXTURE || defined DEBUGTEXTUREMAX 
	printf ("text = %s, fontnum = %d\n", text1, p->text->Fontnum);
#endif
	gsgltext_analyse (text1, textnum, x, y, dimension, GSGL_TRUE, p);
}

void gsgltext_analyse (char *text1, int textnum, float *x, float *y, int dimension, gsglboolean analyse, gsglPage *p)
{
	int text_uni [MAX_CHAR];
	int nmax;

	text_to_unicode (text1, textnum, text_uni, &nmax, p);
	textunicode (text_uni, nmax, x, y, dimension, analyse, p);
}

static void textunicode (int *text_uni, int nmax, float *x, float *y, int dim, gsglboolean analyse, gsglPage *p)
{
	gsglFontStatus *status;
	gsglboolean mem = GSGL_FALSE, hd = GSGL_FALSE, list = GSGL_FALSE;
	gsgllist *textlist;
	textblock *texts;
	int i;
	int ifnts, ifntr;
	float w, h, left, top;
	gsglText *t = p->text;

	status = gsglFontStatus_new (p);
	status->dimension = dim;

	if (analyse) text_unicode_analyse_and_draw (text_uni, nmax, x, y, status, p);
	else gsgl_render_text (x, y, text_uni, nmax, status);

	textlist = status->texts;
	while (textlist != NULL) {
		texts = textlist->data;
		ifnts = texts->fontnum;
		if (use_ps_font(ifnts)) {
			if (t->UseType1) {
				ifntr = ifnts + 470;
			} else {
				if (ifnts <= 35) {
					ifntr = ifnts - 30;
				} else if (ifnts <= 39) {
					ifntr = 6;
				} else if (ifnts <= 43) {
					ifntr = 0;
				} else if (ifnts == 44) {
					ifntr = 11;
				} else {
					gsgl_error ("textunicode", "ifnts=%d", ifnts);
					ifntr = 0;
				}
			}
			
			if (use_bitmap_tcpip (status->fontnum, status)) {
				gsgl_pstext (ifnts, ifntr,
					     texts->user_chh, texts->user_chh,
					     texts->user_chh,
					     texts->angle, texts->tilt,
					     texts->x[0], texts->y[0], dim, 
					     texts->text, texts->ntext, GSGL_TRUE, p);
				gsgl_drv_pstext_position (texts->x, texts->y, texts->ntext, p);
			} else {
				gsgl_pstext (ifnts, ifntr,
					     texts->user_chh, texts->user_chh,
					     texts->user_chh,
					     texts->angle, texts->tilt,
					     texts->x[0], texts->y[0], dim, 
					     texts->text, texts->ntext, GSGL_FALSE, p);
			}

			mem = gsgl_inq_drv_mem (p);
			hd = gsgl_inq_drv_hd (p);
			list = gsgl_inq_displaylist_is_enabled (p);

			if (mem) gsgl_disable_drv_mem (p);
			if (hd) gsgl_disable_drv_hd (p);
			if (list) gsgl_disable_displaylist (p);
		}
		if (use_bitmap_tcpip (status->fontnum, status)) {
		} else if (use_bitmap(texts->fontnum, status)) {

			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glEnable(GL_ALPHA_TEST);
			glEnable(GL_TEXTURE_2D);

			for (i=0; i<texts->ntext; i++) {
				gsgl_enable_drv_gl (p);
				if (gsgl_bind_texture (i, texts, t, &w, &h, &left, &top))
					continue;
				gsgl_render_texture (texts->x[i], texts->y[i], w, h, left, top, 
						     texts->angle, texts->tilt);
			}

			glDisable(GL_TEXTURE_2D);
			glDisable(GL_ALPHA_TEST);

		} else if (use_stroke_font(texts->fontnum) || 
			   (use_ps_font(texts->fontnum) && !t->UseType1 && 
			    gsgl_inq_devicetype (status->p->library) != GSGL_DEVICE_PS)) {
			
			t->Fontnum = texts->fontnum;
			if (use_ps_font(texts->fontnum)) {
				if (texts->fontnum == 37 || texts->fontnum == 39)
					texts->tilt = -12.0;
				if (texts->fontnum == 41 || texts->fontnum == 43)
					texts->tilt = -12.0;
				texts->user_chw = texts->user_chh * 0.6666667;
			}
			
			gs_sfstch (texts->user_chh, texts->user_chw, texts->user_chspace,
				   texts->angle, texts->tilt, status->p);
			
			if (use_ps_font(texts->fontnum)) {
				gsaf_inqlnw (&w);
				setlnw_text (p);
			}
			
			gs_sftext (texts->x[0], texts->y[0], texts->text, 
				   texts->ntext, dim, p);
			
			t->Fontnum = status->fontnum;
			
			gs_sfstch (status->user_chh, status->user_chw, status->user_chspace,
				   status->angle, status->tilt, status->p);
			
			if (use_ps_font(texts->fontnum)) {
				gsaf_setlnw (w);
			}
		} else if (gsgl_inq_devicetype (status->p->library) != GSGL_DEVICE_PS) {

			for (i=0; i<texts->ntext; i++) {
				if (texts->polygonfont) {
					gsft_render_polygonized_font 
						(texts->outlines[i], texts->x[i], texts->y[i], 
						 texts->pixel2cm, dim, p);
				} else {
					gsft_render_vectorized_font 
						(texts->outlines[i], texts->x[i], texts->y[i], 
						 texts->pixel2cm, dim, p);
				}
			}
		}
		if (use_ps_font(status->fontnum)) {
			gsgl_drv_pstext_end (p);
			if (mem) gsgl_enable_drv_mem (p);
			if (hd) gsgl_enable_drv_hd (p);
			if (list) gsgl_enable_displaylist (p);
		}
		textlist = textlist->next;
	}
	t->Prechar = status->prechar;
}

void text_unicode_without_analysis (int *text_uni, int nmax, float *x, float *y, int dimension, gsglPage *p)
{
	textunicode (text_uni, nmax, x, y, dimension, GSGL_FALSE, p);
}

void gsglinqtextsize (char *ktext, int nchar, float *size, gsglPage *p)
{
	float height, depth;

	gsglinqtextbbox (ktext, nchar, size, &height, &depth, p);
}

void gsglinqtextbbox (char *ktext, int nchar, float *size, float *height, float *depth, gsglPage *p)
{
	float x = 0.0, y = 0.0;
	gsglFontStatus *status;
	float user_chh, user_chw, user_chspace, chh, chw, chspace, tilt, angle;
	int fontnum, fontnum_old;
	int nmax;
	int text_uni [MAX_CHAR];

	status = gsglFontStatus_new (p);
	status->dontdraw = GSGL_TRUE;

	store_status (&user_chh, &user_chw, &user_chspace,
		      &chh, &chw, &chspace, &tilt, &angle, &fontnum, status);

	text_to_unicode (ktext, nchar, text_uni, &nmax, p);
	text_unicode_analyse_and_draw (text_uni, nmax, &x, &y, status, p);

	*size = x;
	*height = status->ymax;
	*depth = status->ymin;
	fontnum_old = status->fontnum;

	restore_status (user_chh, user_chw, user_chspace,
			chh, chw, chspace, tilt, angle, fontnum, status);
	if (fontnum != fontnum_old)
		gsgl_setfnt (status);
}

static void inq_intertext_size (int *text_uni, int nchar, 
				float *width, float *height, float *depth,
				gsglFontStatus *status_old)
{
	float x = 0.0, y = 0.0;
	gsglFontStatus *status;
	float user_chh, user_chw, user_chspace, chh, chw, chspace, tilt, angle;
	int   fontnum, fontnum_old;
	int   i = 0;

	status = gsglFontStatus_new (status_old->p);
	status->dontdraw = GSGL_TRUE;
	status->fontenc = status_old->fontenc;
	status->fontnum = status_old->fontnum;

	store_status (&user_chh, &user_chw, &user_chspace,
		      &chh, &chw, &chspace, &tilt, &angle, &fontnum, status);
	status->angle = 0.0;

	gsgl_analyse_strings (text_uni, &i, nchar, &x, &y, status);

	*width = x;
	*height = status->ymax;
	*depth = status->ymin;
	fontnum_old = status->fontnum;

	restore_status (user_chh, user_chw, user_chspace,
			chh, chw, chspace, tilt, angle, fontnum, status);
	if (fontnum != fontnum_old)
		gsgl_setfnt (status);
}

int gsgl_convert_uni2fontcode (int in, const char *out_code)
{
	char buf[1000];
	char* out = buf;
	size_t bufsize = 1000;
	size_t bufleft = bufsize;
	size_t len = 4;
	iconv_t cd;
	unsigned int code;
	int i, n, tmp;
	char inchar[4];
	char *p_in = inchar;

	for (i=0; i<4; i++) {
		inchar[i] = in/pow (256, 3-i);
		in -= inchar[i] * pow (256, 3-i);
	}

	/* Get a converter to convert "in_code" to "out_code" */
	cd = iconv_open(out_code, "UCS-4BE");
	
	/*   bufleft : buffer number of out 
	 *   len     : buffer number of in which will be converted
	 */
	iconv(cd, &p_in, &len, &out, &bufleft);
	
	/*   bufleft : surplus buffer number of out
	 *   len     : not converted buffer number of in
	 */

	iconv_close(cd);

	n = bufsize - bufleft;

	code = 0;
	for (i=0; i<n; i++) {
		tmp = buf[i];
		if (tmp < 0) tmp = 256 + tmp;
		code += tmp * (int) pow (256, n-i-1);
	}

	return code;
}

/*static void gsgl_4buaki (float *x, float *y, gsglFontStatus *status)
  {*/
  /* 4-bu-a-ki ... a space between japanese and latin characters */
/*if (!status->disable4buaki) {
 *x += 0.25 * status->currentchh	* cos(status->angle*GSGL_DEG);
 *y += 0.25 * status->currentchh	* sin(status->angle*GSGL_DEG);
 }
 }*/

static gsgllist * gsgl_textblock_append (int *text, int textnum, gsglFontStatus *status)
{
	int i;
	textblock *texts;
	
	texts = (textblock *) gsgl_malloc (sizeof(textblock));
	texts->user_chh = status->user_chh;
	texts->user_chw = status->user_chw;
	texts->user_chspace = status->user_chspace;
	texts->chh = status->chh;
	texts->chw = status->chw;
	texts->chspace = status->chspace;
	texts->angle = status->angle;
	texts->tilt = status->tilt;
	texts->fontnum = status->fontnum;	
	texts->polygonfont = status->polygonfont;
	texts->ntext = textnum;
	texts->pixel2cm = status->pixel2cm;
	texts->text = (unsigned int *) gsgl_malloc_atomic (sizeof(unsigned int)*textnum);
	for (i=0; i<textnum; i++)
		texts->text[i] = text[i];
	texts->x = (float *) gsgl_malloc_atomic (sizeof(float)*textnum);
	texts->y = (float *) gsgl_malloc_atomic (sizeof(float)*textnum);
	texts->outlines = (gsgloutline *) gsgl_malloc (sizeof(gsgloutline) * textnum);
	texts->bitmap = (gsglbitmap *) gsgl_malloc (sizeof(gsglbitmap) * textnum);
	texts->texturename = (GLuint *) gsgl_malloc (sizeof(GLuint) * textnum);
	for (i=0; i<textnum; i++) {
		texts->outlines[i].n_contours = 0;
		texts->outlines[i].contours = NULL;
		texts->bitmap[i].width = 0;
		texts->bitmap[i].rows = 0;
		texts->bitmap[i].buffer = NULL;
		texts->texturename[i] = 0;
	}

	return gsgl_list_append (status->texts, texts);
}

static void gsgl_render_text (float *x,float *y,int *text,int textnum,gsglFontStatus *status)
{
	int i;
	MYFT_Long *text_fontenc;
#ifdef HAVE_FREETYPE2
	FT_Outline outline;
	FT_Glyph_Metrics metrics;
	FT_Vector kerning;
	FT_BBox bbox;
#endif
	float kx, ky;
	int glyph = 0;
	int tmp;
	float ymin, ymax;
	gsgllist *list;
	textblock *texts;
	float tfxx, tfxy, tfyx, tfyy, tflx, tfly, tfsp, tfspx, tfspy;
	float had;
	gsglText *t = status->p->text;
	
#ifdef DEBUGSCRIPT
	printf ("textnum = %d (render)\n", textnum);
	printf ("text[0] = 0x%0x(render)\n", text[0]);
	printf ("x, y = %f %f (render)\n", *x, *y);
#endif
	
	if (textnum <= 0) return;
	
	status->texts = gsgl_textblock_append (text, textnum, status);
	list = gsgl_list_last (status->texts);
	texts = list->data;

	if (status->atom1 == NULL) status->atom1 = atom_new ();

	text_fontenc = (MYFT_Long *) gsgl_malloc_atomic (sizeof(MYFT_Long) * (textnum+1));

	for (i=0; i<textnum; i++) {
		if (!FONTNUM_IS_BAKOMA(status->fontnum)) {
			if (UNICODE_IS_BASIC_LATIN(text[i])) {
				/*fprintf (stderr, "basic latin\n");*/
				/*if (status->prechar == JAPANESE) {
				  gsgl_4buaki (x, y, status);
				  }*/
				status->prechar = LATIN;
				if (FONTNUM_IS_BASIC_LATIN(status->fontnum)) {
					status->prechar = LATIN;
				} else if (FONTNUM_IS_GREEK(status->fontnum)) {
					status->prechar = GREEK;
				} else if (!FONTNUM_IS_BASIC_LATIN(status->fontnum) &&
					   !FONTNUM_IS_GREEK(status->fontnum)) {
					tmp = t->Pre_ttf_num[LATIN];
					t->Pre_ttf_num[fontnum_is(status->fontnum)] = status->fontnum;
					status->fontnum = tmp;
					gsgl_setfnt (status);
				}
			} else if (UNICODE_IS_SPACE(text[i])) {
				status->prechar = NULLCHAR;
				/*fprintf (stderr, "space\n");*/
			} else if (UNICODE_IS_GREEK(text[i])) {
				/*if (status->prechar == JAPANESE) {
				  gsgl_4buaki (x, y, status);
				  }*/
				status->prechar = GREEK;
				/*fprintf (stderr, "greek\n");*/
				if (!FONTNUM_IS_GREEK(status->fontnum)) {
					if (status->fontnum >= 1000) {
						tmp = t->Pre_ttf_num[GREEK];
						t->Pre_ttf_num[fontnum_is(status->fontnum)]
							= status->fontnum;
					} else {
						tmp = t->Pre_type1_font_num[GREEK];
						t->Pre_type1_font_num[fontnum_is(status->fontnum)]
							= status->fontnum;
					}
					status->fontnum = tmp;
					gsgl_setfnt (status);
				}			
			} else if (text[i] >= 0x3000) { /* Japanese? */
				status->prechar = JAPANESE;
				if (!FONTNUM_IS_JAPANESE(status->fontnum)) {
					tmp = t->Pre_ttf_num[JAPANESE];
					t->Pre_ttf_num[fontnum_is(status->fontnum)] = status->fontnum;
					status->fontnum = tmp;
					gsgl_setfnt (status);
				}
			}
		}
		
		if (status->fontenc == UNICODE) {
			text_fontenc[i] = text[i];
		} else if (status->fontenc == SJIS) {
			text_fontenc[i] = gsgl_convert_uni2fontcode (text[i], "SHIFT_JIS");
		} else if (status->fontenc == SYMBOL) {
			text_fontenc[i] = gsgl_convert_uni2fontcode (text[i], "ASCII");
			/* symbol = 0xf0XX */
			text_fontenc[i] = text_fontenc[i] + 15 * pow (16, 3);
		} else {
			text_fontenc[i] = text[i];
		}
#ifdef DEBUGFREETYPE
		fprintf (stderr, "unicode = 0x%0x, fontenc = 0x%0x\n",text[i],(int)text_fontenc[i]);
#endif


#ifdef HAVE_FREETYPE2
		if (use_bitmap(status->fontnum, status)) {
#ifndef CACHE
			glyph = gsft_get_kerning (glyph, text_fontenc, i, &kerning, status->p);
#endif
			texts->texturename[i] = gsgl_search_texture_object (texts->text[i], texts, t);
			if (!texts->texturename[i]) {
#ifndef CACHE
				gsgl_get_texture_data_direct (i, glyph, texts, status);
#else
				gsgl_get_texture_data_cache (status->fontnum, i, texts, t);
#endif
			}
		} else if (use_freetype(status->fontnum, status->p)) {

			glyph = gsft_get_kerning (glyph, text_fontenc, i, &kerning, status->p);
			
			gsft_load_glyph (glyph, status->p);
			
			kx = kerning.x / status->pixel2cm;
			ky = kerning.y / status->pixel2cm;
			
			outline = gsft_get_outline (status->p);
			metrics = gsft_get_metrics (status->p);
			
			FT_Outline_Get_BBox (&outline, &bbox);
			
			ymin = bbox.yMin / status->pixel2cm;
			ymax = bbox.yMax / status->pixel2cm;
		}
#endif

		if (use_freetype(status->fontnum, status->p)) {
		} else if (use_ps_font(status->fontnum)) {
			gsgl_get_height_depth_char(text[i], &ymax, &ymin, status);
			
			if (i == 0) {
				kx = 0.0;
				ky = 0.0;
			} else {
				ky = type1_kerning (status->fontnum, text[i-1], text[i], status->p);
				gsgl_1000_scale_factor_to_cm (status->fontnum, ky, &kx, status->p);
				ky = 0.0;
			}			
		} else {
			ymin = 0.0;
			ymax = status->user_chh;
			kx = 0.0;
			ky = 0.0;
		}
		status->ymin = MIN(status->ymin, *y + ymin);
		status->ymax = MAX(status->ymax, *y + ymax);
		if (status->atom1->nucleus && i == textnum-1) {
			status->atom1->h = ymax;
			status->atom1->d = ymin;
		} else if (i == textnum-1) {
			t->Tmp_h = ymax;
			t->Tmp_d = ymin;
		}
#ifdef HAVE_FREETYPE2
		if (fabs(status->angle) >= 1.e-3 || fabs(status->tilt) >= 1.e-3) {
			if (use_freetype(status->fontnum, status->p)) {
				gsft_set_transform_matrix (status->p);
				gsft_load_glyph (glyph, status->p);
				outline = gsft_get_outline (status->p);
			}
		}
#endif
#ifdef DEBUGFREETYPE
		fprintf (stderr, "index = %d, ", glyph);
		fprintf (stderr, "contours = %d, points = %d, flag = %d\n",
			 outline.n_contours, outline.n_points, outline.flags);
		fprintf (stderr, "ad = %ld, ", metrics.horiAdvance);
		fprintf (stderr, "kern_x = %ld\n", kerning.x);
#endif
		*x += kx * cos(status->angle * GSGL_DEG);
		*y += kx * sin(status->angle * GSGL_DEG);

		texts->x[i] = *x;
		texts->y[i] = *y;
		
		if (use_freetype(status->fontnum, status->p)) {
#ifdef HAVE_FREETYPE2
			if (!status->dontdraw) {
				gsft_get_gsgloutline_from_ft_outline 
					(outline, texts->x[i], texts->y[i],
					 texts->pixel2cm, &(texts->outlines[i]), status->p);
			}
			status->atom1->horiad = metrics.horiAdvance / status->pixel2cm;
			*x += metrics.horiAdvance*cos(status->angle*GSGL_DEG)/status->pixel2cm;
			*y += metrics.horiAdvance*sin(status->angle*GSGL_DEG)/status->pixel2cm;
			if (fabs(status->angle) >= 1.e-3 || fabs(status->tilt) >= 1.e-3)
				gsft_set_transform_null (status->p);
#endif
		} else {
			if (use_bitmap(status->fontnum, status)) {
				gs_grsize (text[i], &had, status->p);
			} else {
				gs_grsize (text_fontenc[i], &had, status->p);
			}
			if (use_ps_font(status->fontnum)) {
				status->atom1->horiad = had;
			} else {
				gsgl_inq_font_scale_info (&tfxx, &tfxy, &tfyx, &tfyy,
							  &tflx, &tfly, &tfsp, &tfspx, 
							  &tfspy, status->p);
				status->atom1->horiad = tfxx*had + tfspx;
			}
			*x += status->atom1->horiad * cos(status->angle*GSGL_DEG);
			*y += status->atom1->horiad * sin(status->angle*GSGL_DEG);
#ifdef DEBUGSCRIPT
			printf ("x, y = %f %f (render2)\n", *x, *y);
#endif
		}
	}
	gsgl_free (text_fontenc);
	text_fontenc = NULL;
}

void gsgl_polygontext ()
{
	gsglPage *p;

	if (!gsgl_inq_page_is_open ()) return;
	p = gsgl_inq_page ();
	
	p->text->Polygonfont = GSGL_TRUE;
}
void gsgl_vectortext () 
{
	gsglPage *p;

	if (!gsgl_inq_page_is_open ()) return;
	p = gsgl_inq_page ();

	p->text->Polygonfont = GSGL_FALSE;
}

void gsgl_setfreetypenode (int node)
{
	gsglPage *p;

	if (!gsgl_inq_page_is_open ()) return;
	p = gsgl_inq_page ();
	if (node < 1) return;

	p->text->Node = node;
}

int gsft_inq_node_num (gsglPage *p)
{
	return p->text->Node;
}

void gsgl_usetype1 ()
{
	gsglPage *p;

	if (!gsgl_inq_page_is_open ()) return;
	p = gsgl_inq_page ();

	p->text->UseType1 = GSGL_TRUE;
}

void gsgl_init_text (gsglPage *p)
{
	char *str;
	int i;
	gsglboolean number;
	gsglText *t = p->text;

	t->In_code = gsgl_strdup("EUC-JP"); 
	t->Node = NODE;
	t->Prechar = NULLCHAR;
	t->Pre_ttf_num[0] = LATIN_DEFAULT;
	t->Pre_ttf_num[1] = GREEK_DEFAULT;
	t->Pre_ttf_num[2] = JAPANESE_DEFAULT;
	t->Pre_type1_font_num[0] = 502;
	t->Pre_type1_font_num[1] = 514;
	t->Pre_ps_font_num[0] = 32;
	t->Pre_ps_font_num[1] = 44;
	t->Pre_stroke_font_num[0] = 2;
	t->Pre_stroke_font_num[1] = 11;
	t->Tmp_h = 0.0;
	t->Tmp_d = 0.0;
	t->UseType1 = GSGL_FALSE;
	t->Fontenc = NONENC;
	t->Fontnum = LATIN_DEFAULT;
	t->Polygonfont = GSGL_TRUE;
	t->User_chh = 0.35;
	t->Chh = 0.35 * 1.466667;
	t->Currentchh = 0.35;
	t->User_chw = 0.35 * 0.666667;
	t->Chw = 0.35 * 0.66667 * 1.466667;
	t->Currentchw = 0.35 * 0.66667 * 1.466667;
	t->User_chspace = 0.35;
	t->Chspace = 0.35 * 1.466667;
	t->Currentchspace = 0.35 * 1.466667;
	t->Pixel2cm = 1.0;
	t->Angle = 0.0;
	t->Tilt = 0.0;
	
	str = getenv("GSGL_BITMAPFONT");
	if (str && strlen(str) > 0) {
		t->UseBitmap_env = GSGL_TRUE;
		number = GSGL_TRUE;
		for (i=0; i<strlen(str); i++) {
			if (!isdigit(str[i])) {
				number = GSGL_FALSE;
				break;
			}
		}
		if (number) {
			t->size_point = atoi(str);
			if (t->size_point < 10)
				t->size_point = 10;
		} else
			t->size_point = GSFT_SIZE_POINT;
	} else {
		t->UseBitmap_env = GSGL_FALSE;
		t->size_point = GSFT_SIZE_POINT;
	}
	t->UseBitmap_explicit = GSGL_FALSE;

	t->textures = NULL;
	t->bitmaps = NULL;
}

void gsgl_use_bitmap_font_explicit (gsglboolean b, gsglPage *p)
{
	p->text->UseBitmap_explicit = b;
}
