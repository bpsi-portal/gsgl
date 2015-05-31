#include "gsaf.h"
#include "text.h"
#include "textin.h"
#include "text_freetype.h"
#include "text_texture.h"
#include "utils.h"
#include "configfile.h"
#include "drv_wrapper.h"

/*#define DEBUGTEXTUREMAX*/
/*#define DEBUGTEXTURE*/
/*#define LCD*/

static gsglTexture * gsglTexture_new (int unicode, gsglbitmap bitmap, textblock *texts)
{
	gsglTexture *texture;
	texture = (gsglTexture *) gsgl_malloc (sizeof(gsglTexture));
	texture->name = 0;
	texture->fontnum = texts->fontnum;
	texture->unicode = unicode;
        texture->angle = texts->angle;
        texture->tilt = texts->tilt;
        texture->width = bitmap.width;
        texture->rows = bitmap.rows;
        texture->left = bitmap.left;
        texture->top = bitmap.top;

	return texture;
}

static gsglTexture * gsglTexture_from_name (GLuint name, gsglText *text)
{
	gsgllist *list = text->textures;
	gsglTexture *texture;

	while (list) {
		texture = (gsglTexture *) list->data;
		if (texture->name == name)
			return texture;
		list = list->next;
	}
	gsgl_warning ("gsglTexture_from_name", "Can not find a texture from name (name = %d)", name);
	return NULL;
}

GLuint gsgl_search_texture_object (unsigned int unicode, textblock *text, gsglText *t)
{
	gsgllist *list = t->textures;
	gsglTexture *texture;
	
	while (list) {
		texture = (gsglTexture *) list->data;
		if (texture->fontnum == text->fontnum &&
		    texture->unicode == unicode &&
		    fabs(texture->angle - text->angle) < 1.e-3 &&
		    fabs(texture->tilt - text->tilt) < 1.e-3) {
#ifdef DEBUGTEXTURE
			fprintf (stderr, "texture name = %d (search)\n", texture->name);
#endif
			return texture->name;
		}
#ifdef DEBUGTEXTUREMAX
		fprintf (stderr, "Not correspond %d %d %f %f\n", 
			 texture->fontnum, texture->unicode, texture->angle, texture->tilt);
#endif		
		list = list->next;
	}
#ifdef DEBUGTEXTUREMAX
	fprintf (stderr, "texture name = 0, unicode = %d, fontnum = %d (search)\n", unicode, text->fontnum);
#endif
	return 0;
}

void gsgl_textures_delete (gsglPage *p)
{
	gsglText *t = p->text;
	gsgllist *list;
	gsglTexture *texture;

	list = t->textures;

	while (list) {
		texture = (gsglTexture *) list->data;
		if (texture && texture->name) {
#ifdef DEBUGTEXTUREMAX
			fprintf (stderr, "delete texture %d\n", texture->name);
#endif
			glDeleteTextures(1, &(texture->name));
		}
		gsgl_free (texture);
		list = list->next;
	}
}

#define RES 64
int gsgl_bind_texture (int i, textblock *texts, gsglText *t, 
			float *w, float *h, float *left, float *top)
{       
	GLuint error;
	gsglTexture *texture;	

	if (texts->texturename[i]) {
		glBindTexture(GL_TEXTURE_2D, texts->texturename[i]);
		
		texture = gsglTexture_from_name (texts->texturename[i], t);
		
		*left = (texture->left) / texts->pixel2cm * RES;
		*top = (texture->top) / texts->pixel2cm * RES;
		*w = texture->width / texts->pixel2cm * RES;
		*h = texture->rows / texts->pixel2cm * RES;
		
	} else {
		/*printf ("%d %d\n", texts->bitmap[i].width,texts->bitmap[i].rows);*/
		
		if (!texts->bitmap[i].buffer)
			return 1;
		
		texture = gsglTexture_new (texts->text[i], texts->bitmap[i], texts);
		t->textures = gsgl_list_append (t->textures, texture);
		
		glGenTextures(1, &(texture->name));
#ifdef DEBUGTEXTURE
		fprintf (stderr, "gen texture %d, unicode = %d\n", texture->name, texts->text[i]);
#endif
		glBindTexture(GL_TEXTURE_2D, texture->name);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		/*anchor*/				
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		
		/*anchor*/
		error = gluBuild2DMipmaps (GL_TEXTURE_2D, GL_ALPHA, 
					  texts->bitmap[i].width, texts->bitmap[i].rows,
					  GL_ALPHA, GL_UNSIGNED_BYTE, texts->bitmap[i].buffer);
		if (error)
			gsgl_warning ("gsgl_bind_texture", "failed in gluBuild2DMipmaps (%d)", error);
		
		*left = (texts->bitmap[i].left) / texts->pixel2cm * RES;
		*top = (texts->bitmap[i].top) / texts->pixel2cm * RES;
		*w = texts->bitmap[i].width / texts->pixel2cm * RES;
		*h = texts->bitmap[i].rows / texts->pixel2cm * RES;
	}
	return 0;
}

void gsgl_render_texture (float x, float y, float w, float h, float left, float top, float angle, float tilt)
{
	int j;
	float xlt, ylt;
	float dx = 0.0, dy = 0.0;
	GLdouble vertex[4][2];
	static GLdouble texcoord[4][2] = {{0.0, 0.0}, {0.0, 1.0}, {1.0, 1.0}, {1.0, 0.0}};
	float a, t;

#ifdef LCD
	left = left / 3.0;
	w = w / 3.0;
#endif

	a = angle * GSGL_DEG;
	t = tilt * GSGL_DEG;

	dx = left * cos(a) - top * sin (a) - top * tan(t); 
	dy = left * sin(a) + top * cos (a);
	
	xlt = x + dx;
	ylt = y + dy;

	vertex[0][0] = xlt;
	vertex[0][1] = ylt;
	vertex[1][0] = xlt + h * sin (a) + h * tan(t);
	vertex[1][1] = ylt - h * cos (a);
	vertex[2][0] = vertex[1][0] + w * cos (a);
	vertex[2][1] = vertex[1][1] + w * sin (a);
	vertex[3][0] = xlt + w * cos (a);
	vertex[3][1] = ylt + w * sin (a);
	
	glBegin(GL_QUADS);
	for (j = 0; j < 4; ++j) {
		glTexCoord2dv(texcoord[j]);
		glVertex2dv(vertex[j]);
	}
	glEnd();
}

void gsgl_get_texture_data_direct (int i, int glyph, textblock *texts, gsglFontStatus *status)
{
#ifdef HAVE_FREETYPE2
	FT_Bitmap b;
	int j;
	gsglText *t = status->p->text;

	if (fabs(status->angle) >= 1.e-3 || fabs(status->tilt) >= 1.e-3)
		gsft_set_transform_matrix (status->p);

	gsft_load_glyph (glyph, status->p);
	
#ifdef LCD
	if (FT_Render_Glyph (t->Face->glyph, FT_RENDER_MODE_LCD))
#else
        if (FT_Render_Glyph (t->Face->glyph, ft_render_mode_normal))
#endif		
		gsgl_warning ("ft_render_glyph", "error");

	b = t->Face->glyph->bitmap;
	
	if (fabs(status->angle) >= 1.e-3 || fabs(status->tilt) >= 1.e-3)
		gsft_set_transform_null (status->p);
	
	/*printf ("glyph=%d, rows=%d, width=%d\n",
	  glyph, b.rows, b.width);*/
	if (b.rows < 1 || b.width < 1) 
		return;
								
	texts->bitmap[i].left = t->Face->glyph->bitmap_left;
	texts->bitmap[i].top = t->Face->glyph->bitmap_top;
	texts->bitmap[i].rows = b.rows;
	texts->bitmap[i].width = b.width;
	
	texts->bitmap[i].buffer = (unsigned char *) gsgl_malloc(sizeof(unsigned char)*
								texts->bitmap[i].rows*
								texts->bitmap[i].width);
	
	for (j = 0; j < texts->bitmap[i].rows * texts->bitmap[i].width; j++)
		texts->bitmap[i].buffer[j] = b.buffer[j];
#endif
}


void gsgl_get_texture_data_cache (int fnt, int i, textblock *texts, gsglText *t)
{
	texts->bitmap[i] = t->bitmaps[fnt-32][texts->text[i]-32];
}

static char * basename (char *name)
{
        char *base;
        unsigned len;

        base = NULL;
        for (len = strlen (name); len > 0; len--) {
                if (name[len - 1] == '/') {
                        base = name + len;
                        break;
                }
	}
        if (!base)
                base = name;

        return base;
}

static char * gsft_cache_dir (void)
{
	int len;
	char *home;
	char *dir;

	home = gsgl_strdup (gsgl_get_home_dir());
	/* cache dir = $HOME/.gsgl */
	len = strlen(home) + 6;

	if (len < 6)
		gsgl_error ("gsft_cache_dir", "len = %d", len);
		
	dir = (char *) gsgl_malloc (sizeof(char) * (len + 1));
	sprintf (dir, "%s/.gsgl", home);

	return dir;
}

static char * gsft_cache_name (int fnt, char *file, int size)
{
	int len;
	char *dir;
	char *cache;

	if (fnt < 32 || fnt > 44)
		gsgl_error ("gsft_have_bitmap_cache", "fnt = %d", fnt);
	if (!file)
		gsgl_error ("gsft_have_bitmap_cache", "file == NULL");
	if (!basename(file))
		gsgl_error ("gsft_have_bitmap_cache", "file = %s", file);
	if (size < 10)
		gsgl_error ("gsft_have_bitmap_cache", "size = %d", size);
	
	dir = gsft_cache_dir ();
	/* cache name = $HOME/.gsgl/32_pfbfilename_size */
	len = strlen(dir) + strlen(basename(file)) + 4 + 1 + log10(size) + 1;
	/*printf ("len = %d\n", len);*/
	if (len < 13)
		gsgl_error ("gsft_have_bitmap_cache", "len = %d", len);
		
	cache = (char *) gsgl_malloc (sizeof(char) * (len + 1));
	sprintf (cache, "%s/%2d_%s_%d", dir, fnt, basename(file), size);
	/*printf ("cache = %s\n", cache);*/

	return cache;
}

gsglboolean gsft_have_bitmap_cache (int fnt, char *file, int size)
{
	char *cache;
        struct stat st;

	cache = gsft_cache_name (fnt, file, size);
	
	if (stat(cache, &st) != 0) {
		return GSGL_FALSE;
		/* dosen't exist */
	} else {
		if (S_ISREG(st.st_mode)) {
		} else {
			return GSGL_FALSE;
		}
	}
	return GSGL_TRUE;
}

gsglboolean gsft_inq_loaded_bitmap_cache (int fnt, gsglFontStatus *status)
{
	gsglText *t = status->p->text;

	if (fnt < 32 || fnt > 44)
		gsgl_error ("gsft_inq_loaded_bitmap_cache", "fnt = %d", fnt);

	if (t->bitmaps && t->bitmaps[fnt-32]) 
		return GSGL_TRUE;
	else
		return GSGL_FALSE;
}

void gsft_create_bitmap_cache (int fnt, char *file, gsglFontStatus *status)
{
#ifdef HAVE_FREETYPE2
	int i, j;
	int glyph;
	FT_Bitmap b;
	char *cache;
	char *dir;
	FILE *fp;
	int left, top;
	unsigned int width, rows;
	unsigned char *buffer;
	unsigned long offset[255-31];
	int tmp;
	float save_angle;
        struct stat st;
	gsglText *t = status->p->text;

	dir = gsft_cache_dir ();

	if (stat (dir, &st)) {
		mkdir (dir, 0755);
	} else {
		if (!S_ISDIR(st.st_mode)) {
			gsgl_error ("gsft_create_bitmap_cache", "please remove a file %s", dir);
		}
	}

	save_angle = status->angle;

	gsft_init_font (file, status);

	gsft_setcharsize (0.35, 0.35, 0.0, 0.0, status->p);
	cache = gsft_cache_name (fnt, file, t->size_point);
	if ((fp = fopen(cache, "wb")) == NULL) {
		gsgl_warning("gsft_create_bitmap_cache", "Can not open file %s", cache);
		return;
	}
		
	for (i = 32; i <= 255; i++) {
		glyph = FT_Get_Char_Index (t->Face, i);
		gsft_load_glyph (glyph, status->p);
		
#ifdef LCD
		if (FT_Render_Glyph (t->Face->glyph, FT_RENDER_MODE_LCD))
#else
		if (FT_Render_Glyph (t->Face->glyph, ft_render_mode_normal))
#endif
			gsgl_warning ("ft_render_glyph", "error");
		
		b = t->Face->glyph->bitmap;
		
		if (b.rows < 1 || b.width < 1) {
			left = 0;
			top = 0;
			rows = 0;
			width = 0;
			buffer = NULL;
		} else {								
			left = t->Face->glyph->bitmap_left;
			top = t->Face->glyph->bitmap_top;
			rows = b.rows;
			width = b.width;			 
			buffer = b.buffer;
		}
		/*printf ("write %d %d\n", width, rows);*/
		
		offset[i-32] = (unsigned int) ftell (fp); 
		fprintf (fp, "%c", (unsigned char) i);
		if (width * rows == 0) {
			fprintf (fp, "%c", (unsigned char) 0);
			fprintf (fp, "%c", (unsigned char) 0);
		} else {
			fprintf (fp, "%c", (unsigned char) (width / 256));
			fprintf (fp, "%c", (unsigned char) (width % 256));
			fprintf (fp, "%c", (unsigned char) (rows / 256));
			fprintf (fp, "%c", (unsigned char) (rows % 256));
			
			tmp = left + 32768;
			fprintf (fp, "%c", (unsigned char) (tmp / 256));
			fprintf (fp, "%c", (unsigned char) (tmp % 256));
			
			tmp = top + 32768;
			fprintf (fp, "%c", (unsigned char) (tmp / 256));
			fprintf (fp, "%c", (unsigned char) (tmp % 256));

#ifndef LCD
			for (j = 0; j < rows * width; j++)
				fprintf (fp, "%c", (unsigned char) buffer[j]);
#else
			int l;
			for (j = 0; j < rows; j++) {
				for (l = 0; l < b.pitch; l++) {
					if (l < b.width) {
						fprintf (fp, "%c", (unsigned char) b.buffer[j * b.pitch + l]);
					}
				}
			}
#endif
		}
	}
	
	for (i = 32; i <= 255; i++) {
		fprintf (fp, "%c", (unsigned char) ((offset[i-32] >> 24) % 256));
		fprintf (fp, "%c", (unsigned char) ((offset[i-32] >> 16) % 256));
		fprintf (fp, "%c", (unsigned char) ((offset[i-32] >>  8) % 256));
		fprintf (fp, "%c", (unsigned char) (offset[i-32] % 256));
		/*printf ("offset = %d (%d %d %d %d)\n", offset[i], (offset[i] >> 24) % 256, (offset[i] >> 16) % 256, (offset[i] >> 8) % 256, (offset[i] >> 0) % 256);*/
	}
	fclose (fp);
#endif
}

void gsft_read_bitmap_cache (int fnt, char *file, gsglFontStatus *status)
{
	int i;
	char *cache;
	FILE *fp;
	unsigned long offset[255-31];
	unsigned char word[4];
	gsglbitmap *b;
	gsglText *t = status->p->text;

	if (fnt < 32 || fnt > 44)
		gsgl_error ("gsft_have_bitmap_cache", "fnt = %d", fnt);

	if (!t->bitmaps) {
		t->bitmaps = (gsglbitmap **) gsgl_malloc (sizeof(gsglbitmap **) * 12); /*32--44*/
		for (i = 0; i < 12; i++)
			t->bitmaps[i] = NULL;
	}
	if (!t->bitmaps[fnt-32]) {
		t->bitmaps[fnt-32] = (gsglbitmap *) gsgl_malloc (sizeof(gsglbitmap) * (255-31));
		for (i = 32; i <= 255; i++) {
			t->bitmaps[fnt-32][i-32].left = 0;
			t->bitmaps[fnt-32][i-32].top = 0;
			t->bitmaps[fnt-32][i-32].rows = 0;
			t->bitmaps[fnt-32][i-32].width = 0;
			t->bitmaps[fnt-32][i-32].buffer = NULL;
		}
		
		cache = gsft_cache_name (fnt, file, t->size_point);
		if ((fp = fopen(cache, "rb")) == NULL) {
			gsgl_warning("gsft_read_bitmap_cache", "Can not open file %s", cache);
			return;
		}
		if (fseek (fp, -(255-31)*4, SEEK_END))
			gsgl_error ("gsft_read_bitmap_cache", "failed to seek");
		for (i = 32; i <= 255; i++) {
			fread (word, 1, 4, fp);
			/*printf ("offset = %d %d %d %d\n", word[0], word[1], word[2], word[3]);*/
			offset[i-32] = (word[0] << 24) + (word[1] << 16) + (word[2] << 8) + word[3];
		}
		for (i = 32; i <= 255; i++) {
			if (fseek (fp, offset[i-32], SEEK_SET))
				gsgl_error ("gsft_read_bitmap_cache", "failed to seek 2");			
			/*printf ("offset = %d\n", offset[i]);*/
			
			fread (word, 1, 1, fp);
			if (word[0] != i)
				gsgl_error ("gsft_read_bitmap_cache", "word[0] = %d, i = %d", word[0], i);
			
			b = &(t->bitmaps[fnt-32][i-32]);
			
			fread (word, 1, 2, fp);
			b->width = word[0] * 256 + word[1];
			if (!b->width) 
				continue;
			
			fread (word, 1, 2, fp);
			b->rows = word[0] * 256 + word[1];
			
			fread (word, 1, 2, fp);
			b->left = word[0] * 256 + word[1] - 32768;
			
			fread (word, 1, 2, fp);
			b->top = word[0] * 256 + word[1] - 32768;
			
			b->buffer = (unsigned char *) gsgl_malloc (sizeof(unsigned char) * b->width * b->rows);
			fread (b->buffer, 1, b->width * b->rows, fp);
			
			/*printf ("read %d %d\n", b->width, b->rows);*/
		}
		fclose (fp);  
	}
}

void gsgl_render_bitmap_server (int fnt, 
				float chh, float chw, float chsp, float angle, float tilt,
				float *x, float *y, unsigned int *iascs, int nmax,
				gsglPage *p)
{
	textblock *texts;
        int i;
	float w, h, left, top;
	gsglFontStatus *status;

	if (p->text->Fontnum != fnt)
		gsglsetfnt (fnt, p);
	gsglsetcharsize (chh, chw, chsp, angle, tilt, p);
	status =  gsglFontStatus_new (p);

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
        texts->polygonfont = GSGL_FALSE;
        texts->ntext = nmax;
        texts->pixel2cm = status->pixel2cm;
        texts->text = (unsigned int *) gsgl_malloc_atomic (sizeof(unsigned int)*nmax);
        texts->x = (float *) gsgl_malloc_atomic (sizeof(float)*nmax);
        texts->y = (float *) gsgl_malloc_atomic (sizeof(float)*nmax);
        for (i=0; i<nmax; i++) {
                texts->text[i] = iascs[i];
                texts->x[i] = x[i];
                texts->y[i] = y[i];
	}
        texts->outlines = NULL;
        texts->bitmap = (gsglbitmap *) gsgl_malloc (sizeof(gsglbitmap) * nmax);
        texts->texturename = (GLuint *) gsgl_malloc (sizeof(GLuint) * nmax);
        for (i=0; i<nmax; i++) {
                texts->bitmap[i].width = 0;
                texts->bitmap[i].rows = 0;
                texts->bitmap[i].buffer = NULL;
                texts->texturename[i] = gsgl_search_texture_object (texts->text[i], texts, p->text);
        }

        for (i=0; i<nmax; i++) {
                if (!texts->texturename[i])
			gsgl_get_texture_data_cache (fnt, i, texts, p->text);
        }

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glEnable(GL_ALPHA_TEST);
	glEnable(GL_TEXTURE_2D);
		
	for (i=0; i<texts->ntext; i++) {
		gsgl_enable_drv_gl (p);
		if (gsgl_bind_texture (i, texts, p->text, &w, &h, &left, &top))
			continue;
		gsgl_render_texture (x[i], y[i], w, h, left, top, angle, tilt);
	}
	
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_ALPHA_TEST);
}
