#ifndef __text_h__
#define __text_h__

typedef struct _atom
{
	gsglboolean nucleus;
	float horiad;
	float h; /* height */
	float d; /* depth */
	int   *text;
	int   num; /* text num */
	struct _atom *sup;
	struct _atom *sub;
} atom;

#define POINT_START 10
#define POINT_END   11
#define POINT_MED   12

typedef struct _gsgloutline {
	int n_contours;
	gsgllist *contours;
} gsgloutline;

typedef struct _gsglTexture {
	GLuint name;
	unsigned int fontnum;
	unsigned int unicode;
	float angle;
	float tilt;
	unsigned int width;
	unsigned int rows;
	int left;
	int top;
} gsglTexture;

typedef struct _gsglFontStatus {
	float user_chh;
	float user_chw;
	float user_chspace;
	float chh;
	float chw;
	float chspace;
	float currentchh;
	float currentchw;
	float currentchspace;
	float pixel2cm;
	float angle;
	float tilt;
	int   fontenc;
	int   fontnum;
	atom  *atom1;
	gsglboolean polygonfont;
	gsglboolean dontdraw;
	gsglboolean disable4buaki;
	int prechar;
	int dimension;
	float ymin;
	float ymax;
	gsgllist *texts;
	gsglPage *p;
	gsglboolean changefonttemp;
	unsigned int prevfont;
} gsglFontStatus;

typedef struct _gsft_font_info {
	gsglboolean kerning;
	gsglboolean proportional;
} gsft_font_info;

typedef struct _gsglbitmap {
        unsigned int width;
        unsigned int rows;
        int left;
        int top;
        unsigned char *buffer;
} gsglbitmap;

typedef struct _textblock {
	float user_chh;
	float user_chw;
	float user_chspace;
	float chh;
	float chw;
	float chspace;
	float angle;
	float tilt;
	int   fontnum;
	gsglboolean polygonfont;
	float pixel2cm;
	int ntext;
	unsigned int *text;
	float *x;
	float *y;
	gsgloutline *outlines;
	gsglbitmap *bitmap;
	GLuint *texturename;
} textblock;

struct _gsglText
{
	char *In_code; 
	int Node;
	int Prechar;
	int Pre_ttf_num[3];
	int Pre_type1_font_num[2];
	int Pre_ps_font_num[2];
	int Pre_stroke_font_num[2];
	float Tmp_h;
	float Tmp_d;
	gsglboolean UseType1;
	int Fontenc;
	int Fontnum;
	gsglboolean Polygonfont;
	float User_chh;
	float Chh;
	float Currentchh;
	float User_chw;
	float Chw;
	float Currentchw;
	float User_chspace;
	float Chspace;
	float Currentchspace;
	float Pixel2cm;
	float Angle;
	float Tilt;
#ifdef HAVE_FREETYPE2
	FT_Face Face;
	FT_Matrix Matrix;
#endif
	GLenum Triangle_type;
	gsglboolean UseBitmap_env;
	gsglboolean UseBitmap_explicit;
	gsgllist *textures;
	gsglbitmap **bitmaps;
	int size_point;
};

#ifdef __cplusplus
extern "C"{
#endif

void gsgl_init_text (gsglPage *p);
void gsgl_textures_delete (gsglPage *p);
int gsgl_convert_uni2fontcode (int in, const char *out_code);
int gsft_inq_node_num (gsglPage *);
void gsglsetcharsize (float chh, float chw, float chsp, float angle, float tilt, gsglPage *);
void gsglinqchr (float *chh, float *chw, float *chsp, float *angle, float *tilt, gsglPage *);
void gsglsetfnt (int fnt, gsglPage *);
void gsglinqfnt (int *fnt, gsglPage *);
void gsglinqtextbbox (char *ktext, int nchar, float *size, float *height, float *depth, gsglPage *p);
void gsglinqtextsize (char *ktext, int nchar, float *size, gsglPage *);
void gsgltext (char *text1, int textnum, float *x, float *y, int dimension, gsglPage *p);
void gsgltext_analyse (char *text1, int textnum, float *x, float *y, int dimension, gsglboolean analyse, gsglPage *p);
void text_unicode_without_analysis (int *text_uni, int nmax, float *x, float *y, int dimension, gsglPage *p);
void gsft_init_freetype_once_in_program (gsglLibrary *lib);
void gsgl_use_bitmap_font_explicit (gsglboolean b, gsglPage *p);
gsglFontStatus * gsglFontStatus_new (gsglPage *p);

#ifdef __cplusplus
}
#endif

#endif
