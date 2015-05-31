#ifndef __utils_h__
#define __utils_h__

#ifdef HAVE_GCDIR
#  include "gc/gc.h"
#else
#  include "gc.h"
#endif

struct _gsgllist {
	void *data;
	struct _gsgllist *prev;
	struct _gsgllist *next;
};

#ifdef __cplusplus
extern "C"{
#endif 

void * gsgl_malloc (int n);
void * gsgl_malloc_atomic (int n);
void * gsgl_malloc_ignore_off_page (int n);
void * gsgl_malloc_atomic_ignore_off_page (int n);
void gsgl_free (void *p);
float gsgl_convert_token_to_0to1 (unsigned int token);
unsigned int gsgl_convert_0to1_to_2byte (float token);
unsigned int gsgl_convert_4byteint_to_1st2byte (unsigned int token);
unsigned int gsgl_convert_4byteint_to_2nd2byte (unsigned int token);
unsigned int gsgl_convert_tokens_to_4byte (unsigned int token1, unsigned int token2);
float gsgl_convert_token_to_float (unsigned int tokens, unsigned int tokenk);
unsigned int gsgl_float_to_int4_ieee754 (float f);
float gsgl_int4_to_float_ieee754 (unsigned int token1, unsigned int token2);
void gsgl_message (char *func, char *format, ...);
void gsgl_warning (char *func, char *format, ...);
void gsgl_error (char *func, char *format, ...);
void gsgl_set_normal_vector1 (float *x, float *y, float *z, float normal[3]);
void gsgl_set_normal_vector2 (float *p1, float *p2, float *p3, float normal[3]);
void gsgl_mat16_mult_mat16 (float in1[16], float in2[16], float out[16]);
void gsgl_mat16_mult_vec4 (float matrix[16], float vec1[4], float vec2[4]);
void gsgl_inq_matrix_translate (float x, float y, float z, float matrix[16]);
void gsgl_inq_matrix_scale (float xratio, float yratio, float zratio, float matrix[16]);
void gsgl_inq_matrix_rotate (float degree, float x, float y, float z, float matrix[16]);
void gsgl_inq_matrix_frustum (float left, float right, float bottom, float top,
			      float near, float far, float matrix[16]);
void gsgl_inq_matrix_orthogonal (float left, float right, float bottom, float top,
				 float near, float far, float matrix[16]);
char * gsgl_strdup (const char *src);
char * gsgl_strndup (const char *src, int size);
gsgllist * gsgl_list_new (void);
gsgllist * gsgl_list_append (gsgllist *list1, void *item);
gsgllist * gsgl_list_insert (gsgllist *pre, void *item);
unsigned int gsgl_list_count (gsgllist *list1);
gsgllist * gsgl_list_nth (gsgllist *list1, unsigned int i);
gsgllist * gsgl_list_last (gsgllist *list0);
gsgllist * gsgl_list_connect (gsgllist *prev, gsgllist *next);
void gsgl_list_delete_link (gsgllist *list);

#ifdef __cplusplus
}
#endif

#endif
