#ifndef __text_freetype_h__
#define __text_freetype_h__

#ifdef HAVE_FREETYPE2
typedef FT_Long MYFT_Long;
#else
typedef signed long MYFT_Long;
#endif

void gsft_render_polygonized_font (gsgloutline outline, float org_x, float org_y, float pixel2cm, int dimension, gsglPage *);
void gsft_render_vectorized_font (gsgloutline outline, float ox, float oy, float pixel2cm, int dimension, gsglPage *);
void gsft_load_glyph (int glyph_index, gsglPage *p);
void gsft_init_font (char *fontname, gsglFontStatus *status);
void gsft_get_height_depth_char (int text, float *h, float *d, gsglFontStatus *status);
void gsft_setcharsize (float chh, float chw, float angle, float tilt, gsglPage *);
float gsft_get_underline_thickness (gsglFontStatus *status);
float gsft_get_italic_angle (gsglPage *p);
void gsft_set_transform_matrix (gsglPage *);
void gsft_set_transform_null (gsglPage *);

#ifdef HAVE_FREETYPE2
void gsft_get_gsgloutline_from_ft_outline (FT_Outline outline1, float org_x, float org_y, float pixel2cm, gsgloutline *outline2, gsglPage *p);
float gsft_get_kerning (int pre_glyph, FT_Long *text_fontenc, int i, FT_Vector *kerning, gsglPage *p);
FT_Outline gsft_get_outline (gsglPage *);
FT_Glyph_Metrics gsft_get_metrics (gsglPage *);
#endif

#define GSFT_SIZE_POINT 70
#define GSFT_RES  72
/*#define GSFT_SIZE_POINT 100
  #define GSFT_RES  72*/

#endif
