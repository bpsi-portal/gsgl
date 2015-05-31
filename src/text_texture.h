#ifndef __text_texture_h__
#define __text_texture_h__

#ifdef __cplusplus
extern "C"{
#endif

void gsgl_textures_delete (gsglPage *p);
int gsgl_bind_texture (int i, textblock *texts, gsglText *t, float *w, float *h, float *left, float *top);
void gsgl_render_texture (float x, float y, float w, float h, float left, float top, float angle, float tilte);
void gsgl_get_texture_data_direct (int i, int glyph, textblock *texts, gsglFontStatus *status);
void gsgl_get_texture_data_cache (int fnt, int i, textblock *texts, gsglText *t);
GLuint gsgl_search_texture_object (unsigned int unicode, textblock *text, gsglText *t);
gsglboolean gsft_inq_loaded_bitmap_cache (int fnt, gsglFontStatus *status);
void gsgl_render_bitmap_server (int fnt, 
				float chh, float chw, float chsp, float angle, float tilt,
				float *x, float *y, unsigned int *iascs, int nmax,
				gsglPage *p);
gsglboolean gsft_have_bitmap_cache (int fnt, char *file, int size);
void gsft_create_bitmap_cache (int fnt, char *file, gsglFontStatus *status);
void gsft_read_bitmap_cache (int fnt, char *file, gsglFontStatus *status);

#ifdef __cplusplus
}
#endif

#endif
