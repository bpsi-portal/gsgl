#include "gsaf.h"
#include "utils.h"
#ifdef HAVE_FREETYPE2
# include <ft2build.h>
# include FT_FREETYPE_H
# include FT_OUTLINE_H
# include FT_BBOX_H
# include FT_MODULE_H
# include FT_TRUETYPE_TABLES_H
# include FT_TYPE1_TABLES_H
#endif
#include "text.h"
#include "textin.h"
#include "text_freetype.h"

/*#define DEBUGFREETYPE*/
/*#define DEBUGSCRIPT*/

#define POINT_START 10
#define POINT_END   11
#define POINT_MED   12

void gsft_init_freetype_once_in_program (gsglLibrary *lib)
{
#ifdef HAVE_FREETYPE2
	FT_Error error;

	error = FT_Init_FreeType (&(lib->Library));
	if (error) gsgl_error ("gsgl_init_freetype", "failed to initialize FreeType");
#endif
}

/* Check whether glyph is fixed width or not */
#ifdef HAVE_FREETYPE2
gsglboolean gsft_check_width (FT_Face face, int size_point)
{
	FT_Glyph_Metrics metrics[2];
	int              width[2];

	FT_Set_Char_Size (face, 0, 64*size_point, GSFT_RES, GSFT_RES);
	/*FT_Set_Pixel_Sizes (face, 0, 32);*/

	FT_Load_Char (face, SMALL_I, FT_LOAD_DEFAULT);
	metrics[0] = face->glyph->metrics;
	width[0] = metrics[0].horiAdvance;
	    
	FT_Load_Char (face, CAPITAL_W, FT_LOAD_DEFAULT);
	metrics[1] = face->glyph->metrics;
	width[1] = metrics[1].horiAdvance;
	
	if (width[0] != width[1]) return GSGL_TRUE;
	else return GSGL_FALSE;
}
#endif

void gsft_init_font (char *fontname, gsglFontStatus *status)
{
#ifdef HAVE_FREETYPE2
	FT_Error     error;
	int          i, i_face;
	gsft_font_info    *info = NULL;
	int n;
	int found[6] = {0, 0, 0, 0, 0, 0};
	FT_CharMap  charmap;	
	gsglText *t = status->p->text;

#ifdef DEBUGFREETYPE
	fprintf (stderr, "\nOpen %s\n\n", fontname);
#endif
	error = FT_New_Face (status->p->library->Library, fontname, 0, &t->Face);
	if (error == FT_Err_Unknown_File_Format) {
		gsgl_error ("gsft_init_font", 
			    "The font file could be opened and read,\n"
			    "but it appears that its font format is unsupported.\n"
			    "Font name is \"%s\"", fontname);
	} else if (error) {
		gsgl_error ("gsft_init_font", 
			    "The font file could not be opened or read.\n"
			    "File name is \"%s\"", fontname);
	}
#ifdef DEBUGFREETYPE
	fprintf (stderr, "This font file has %ld face(s).\n\n", t->Face->num_faces);
#endif
	if (t->Face->num_faces != 1) {

		info = (gsft_font_info *) gsgl_malloc (sizeof(gsft_font_info) * t->Face->num_faces);
		
		for (i = 0; i < t->Face->num_faces; i++) {
			FT_New_Face (status->p->library->Library, fontname, i, &t->Face);
#ifdef DEBUGFREETYPE
			fprintf (stderr, "Face %d -- %s --\n", i, t->Face->family_name);
#endif
			if (FT_IS_SCALABLE (t->Face)) {
				if (FT_HAS_KERNING (t->Face)) {
#ifdef DEBUGFREETYPE
					fprintf (stderr,
						 "  with kerning information.\n");
#endif
					info[i].kerning = GSGL_TRUE;
				} else {
#ifdef DEBUGFREETYPE
					fprintf (stderr,
						 "  without kerning information.\n");
#endif
					info[i].kerning = GSGL_FALSE;
					
				}
				if (FT_IS_FIXED_WIDTH (t->Face)) {
#ifdef DEBUGFREETYPE
					fprintf (stderr, "  fixed width.\n");
#endif
					info[i].proportional = GSGL_FALSE;
				} else {
#ifdef DEBUGFREETYPE
					fprintf (stderr, "  not fixed width.\n");
					fprintf (stderr, "  realy? ");
#endif
					if (gsft_check_width (t->Face, t->size_point)) {
#ifdef DEBUGFREETYPE
						fprintf (stderr, " yes.\n");
#endif
						info[i].proportional = GSGL_TRUE;
					} else {
#ifdef DEBUGFREETYPE
						fprintf (stderr, " no.\n");
#endif
						info[i].proportional = GSGL_FALSE;
					} 
				}
#ifdef DEBUGFREETYPE
				fprintf (stderr, "\n");
#endif
			}
		}
		
		i_face = -1;
		for (i = 0; i < t->Face->num_faces; i++) {
			if (info[i].kerning) {
				i_face = i;
				break;
			}
		}
		if (i_face == -1) {
			for (i = 0; i < t->Face->num_faces; i++) {
				if (info[i].proportional) {
					i_face = i;
					break;
				}
			}
		}
		if (i_face == -1) i_face = 0;
		
		FT_New_Face (status->p->library->Library, fontname, i_face, &t->Face);
		gsgl_free (info);
		
#ifdef DEBUGFREETYPE
		fprintf (stderr, "Selected face is \"%s\".\n\n", t->Face->family_name);
#endif
	}
#ifdef DEBUGFREETYPE
	fprintf(stderr, "charmaps = %d\n", t->Face->num_charmaps);
#endif
	for (n = 0; n < t->Face->num_charmaps; n++) {
		charmap = t->Face->charmaps[n];
#ifdef DEBUGFREETYPE
		fprintf(stderr, "encoding = %d\n", charmap->encoding);
#endif		
		/*printf ("%d\n", FT_ENCODING_UNICODE);
		  printf ("%d\n", FT_ENCODING_MS_SJIS);
		  printf ("%d\n", FT_ENCODING_MS_SYMBOL);
		  printf ("%d\n", FT_ENCODING_APPLE_ROMAN);
		  printf ("%d\n", FT_ENCODING_ADOBE_STANDARD);
		  printf ("%d\n", FT_ENCODING_ADOBE_EXPERT);
		  printf ("%d\n", FT_ENCODING_ADOBE_CUSTOM);
		  printf ("%d\n", FT_ENCODING_ADOBE_LATIN_1);
		  printf ("%d\n", FT_ENCODING_OLD_LATIN_2);*/
		if (charmap->encoding == ft_encoding_adobe_custom) {
			found[ADOBE_CUSTOM] = n+1;
		} else if (charmap->encoding == ft_encoding_unicode) {
			found[UNICODE] = n+1;
		} else if (charmap->encoding == ft_encoding_apple_roman) {
			found[APPLE_ROMAN] = n+1;
		} else if (charmap->encoding == ft_encoding_sjis) {
			found[SJIS] = n+1;
		} else if (charmap->encoding == ft_encoding_symbol) {
			found[SYMBOL] = n+1;
		} else if (charmap->encoding == ft_encoding_adobe_standard) {
			found[ADOBE_STANDARD] = n+1;
		}
	}
	if (found[ADOBE_CUSTOM]) {
		status->fontenc = ADOBE_CUSTOM;
		if (FT_Select_Charmap (t->Face, ft_encoding_adobe_custom))
			gsgl_error ("FT_Select_Charmap", "adobe custom");
	} else if (found[UNICODE]) {
		status->fontenc = UNICODE;
		if (FT_Select_Charmap (t->Face, ft_encoding_unicode))
			gsgl_error ("FT_Select_Charmap", "unicode");
	} else if (found[APPLE_ROMAN]) {
		status->fontenc = APPLE_ROMAN;
		FT_Select_Charmap (t->Face, ft_encoding_apple_roman);
	} else if (found[SJIS]) {
		status->fontenc = SJIS;
		FT_Select_Charmap (t->Face, ft_encoding_sjis);
	} else if (found[SYMBOL]) {
		status->fontenc = SYMBOL;
		FT_Select_Charmap (t->Face, ft_encoding_symbol);
	} else if (found[ADOBE_STANDARD]) {
		status->fontenc = ADOBE_STANDARD;
		FT_Select_Charmap (t->Face, ft_encoding_adobe_standard);
	} else {
		status->fontenc = NONENC;
		gsgl_error ("gsgl_setfnt", "unimplemented charmap");
	}
#ifdef DEBUGFREETYPE
	fprintf (stderr, "font encode = %d\n", status->fontenc);
#endif
#endif
}

void gsft_setcharsize (float chh, float chw, float angle, float tilt, gsglPage *p)
{
#ifdef HAVE_FREETYPE2
	float ratio;
	float mat1 [2][2];
	float mat2 [2][2];
	float mat3 [2][2];
	gsglText *t = p->text;

	ratio = chw / chh;

	if (FT_Set_Char_Size (t->Face, 64 * t->size_point * ratio,
			      /* char_width in 1/64th of points */
			      64 * t->size_point, /* char_height in 1/64th of points */
			      GSFT_RES, /* horizontal device resolution */
			      GSFT_RES) /* vertical device resolution */) {
		gsgl_error ("gsft_setcharsize",  "Error in FT_Set_Char_Size");
	}
	/*if (FT_Set_Pixel_Sizes (t->Face, 0, 32))
	  gsgl_error ("gsft_setcharsize",  "Error in FT_Set_Pixel_Size");*/

	mat1 [0][0] = 1.0;
	mat1 [0][1] = -tan (tilt * GSGL_DEG);
	mat1 [1][0] = 0.0;
	mat1 [1][1] = 1.0;
	
	mat2 [0][0] = cos(angle * GSGL_DEG);
	mat2 [0][1] = -sin(angle * GSGL_DEG);
	mat2 [1][0] = sin(angle * GSGL_DEG);
	mat2 [1][1] = cos(angle * GSGL_DEG);
	
	mat3 [0][0] = mat1[0][0]*mat2[0][0] + mat1[0][1]*mat2[1][0];
	mat3 [0][1] = mat1[0][0]*mat2[0][1] + mat1[0][1]*mat2[1][1];
	mat3 [1][0] = mat1[1][0]*mat2[0][0] + mat1[1][1]*mat2[1][0];
	mat3 [1][1] = mat1[1][0]*mat2[0][1] + mat1[1][1]*mat2[1][1];
	
	t->Matrix.xx = 65536.0 * mat3 [0][0];
	t->Matrix.xy = 65536.0 * mat3 [0][1];
	t->Matrix.yx = 65536.0 * mat3 [1][0];
	t->Matrix.yy = 65536.0 * mat3 [1][1];	
	/*FT_Set_Transform (t->Face, &Matrix, NULL);*/
#endif
}

void gsft_get_height_depth_char (int text, float *h, float *d, gsglFontStatus *status)
{
#ifdef HAVE_FREETYPE2
	FT_Long text_fontenc;
	int glyph;
	FT_Outline   outline;
	FT_BBox bbox;
	float pixel2cm = status->pixel2cm;
	gsglText *t = status->p->text;

	if (status->fontenc == UNICODE) {
		text_fontenc = text;
	} else if (status->fontenc == SJIS) {
		text_fontenc = gsgl_convert_uni2fontcode (text, "SHIFT_JIS");
	} else if (status->fontenc == SYMBOL) {
		text_fontenc = gsgl_convert_uni2fontcode (text, "ASCII");
		/* symbol = 0xf0XX */
		text_fontenc = text_fontenc + 15 * pow (16, 3);
	} else {
		text_fontenc = text;
	}
	
	glyph = FT_Get_Char_Index (t->Face, text_fontenc);
	gsft_load_glyph (glyph, status->p);
	outline = t->Face->glyph->outline;
	FT_Outline_Get_BBox (&outline, &bbox);

	*h = bbox.yMax / pixel2cm;
	*d = bbox.yMin / pixel2cm;
#endif
}

void gsft_load_glyph (int glyph_index, gsglPage *p)
{
#ifdef HAVE_FREETYPE2
	gsglText *t = p->text;

	if (FT_Load_Glyph (t->Face, glyph_index, FT_LOAD_DEFAULT))
		gsgl_warning ("gsft_load_glyph", "Error in FT_Load_Glyph");
#endif
}

/*static void gsft_4buaki (float *x, float *y, gsglFontStatus *status)
  {*/
	/* 4-bu-a-ki ... a space between japanese and latin characters */
	/*if (!status->disable4buaki) {
	 *x += 0.25 * status->currentchh	* cos(status->angle*GSGL_DEG);
	 *y += 0.25 * status->currentchh	* sin(status->angle*GSGL_DEG);
	 }
	 }*/

float gsft_get_underline_thickness (gsglFontStatus *status)
{
#ifdef HAVE_FREETYPE2
	return status->p->text->Face->underline_thickness / status->pixel2cm;
#else
	return 0.0;
#endif
}

float gsft_get_italic_angle (gsglPage *p)
{
#ifdef HAVE_FREETYPE2
	TT_Postscript *tt_pos = NULL;
	T1_FontInfo afont_info;
	float degree;
	gsglText *t = p->text;

	/* ySubscriptXSize and ySubscriptYSize are too small */

	tt_pos = FT_Get_Sfnt_Table (t->Face, ft_sfnt_post);
	if (tt_pos) {
		degree = (double) tt_pos->italicAngle / 65536.0;
		/*printf ("tt %f\n", degree);*/
	} else {
#ifdef HAVE_FT_GET_PS_FONT_INFO
		if (FT_Get_PS_Font_Info (t->Face, &afont_info)) {
			degree = 0.0;
		} else {
			degree = afont_info.italic_angle;
			/*printf ("ps %f\n", degree);*/
		}
#else
		degree = 0.0;
#endif
	}
	return degree;
#endif
}

#ifdef HAVE_FREETYPE2
float gsft_get_kerning (int pre_glyph, FT_Long *text_fontenc, int i, FT_Vector *kerning, gsglPage *p)
{
	int glyph;
	gsglText *t = p->text;

	if (i != 0) {
		glyph = FT_Get_Char_Index (t->Face, text_fontenc[i]);
		FT_Get_Kerning (t->Face, pre_glyph, glyph, ft_kerning_default, kerning);
	} else {
		glyph = FT_Get_Char_Index (t->Face, text_fontenc[i]);
		kerning->x = kerning->y = 0;
	}
	return glyph;
}

FT_Outline gsft_get_outline (gsglPage *p)
{
	gsglText *t = p->text;
	return t->Face->glyph->outline;
}

FT_Glyph_Metrics gsft_get_metrics (gsglPage *p)
{
	gsglText *t = p->text;
	return t->Face->glyph->metrics;
}
#endif

void gsft_set_transform_matrix (gsglPage *p)
{
#ifdef HAVE_FREETYPE2
	gsglText *t = p->text;
	FT_Set_Transform (t->Face, &t->Matrix, NULL);
#endif
}

void gsft_set_transform_null (gsglPage *p)
{
#ifdef HAVE_FREETYPE2
	gsglText *t = p->text;
	FT_Set_Transform (t->Face, NULL, NULL);
#endif
}

