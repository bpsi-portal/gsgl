#include <stdarg.h>
#include <stdio.h>
#include "gsaf.h"
#include "utils.h"

void * gsgl_malloc (int n)
{
	void *p;

	p = GC_malloc (n);
	if (p == NULL) {
		GC_gcollect ();
		p = GC_malloc (n);
		if (p == NULL) {
			gsgl_error ("gsgl_malloc", "failed to malloc %dbytes", n);
		}
	}
	return p;
}

void * gsgl_malloc_atomic (int n)
{
	void *p;

	p = GC_malloc_atomic (n);
	if (p == NULL) {
		GC_gcollect ();
		p = GC_malloc_atomic (n);
		if (p == NULL) {
			gsgl_error ("gsgl_malloc", "failed to malloc %dbytes", n);
		}
	}
	return p;
}

void * gsgl_malloc_ignore_off_page (int n)
{
	void *p;

	p = GC_malloc_ignore_off_page (n);
	if (p == NULL) {
		GC_gcollect ();
		p = GC_malloc_ignore_off_page (n);
		if (p == NULL) {
			gsgl_error ("gsgl_malloc_ignore_off_page", 
				    "failed to malloc %dbytes", n);
		}
	}
	return p;
}

void * gsgl_malloc_atomic_ignore_off_page (int n)
{
	void *p;

	p = GC_malloc_atomic_ignore_off_page (n);
	if (p == NULL) {
		GC_gcollect ();
		p = GC_malloc_atomic_ignore_off_page (n);
		if (p == NULL) {
			gsgl_error ("gsgl_malloc_ignore_off_page", 
				    "failed to malloc %dbytes", n);
		}
	}
	return p;
}

void gsgl_free (void *p)
{
	GC_free (p);
}

#define FACTM16BIT (1.0/65535.0)
#define FACT16BIT 65535.0

float gsgl_convert_token_to_0to1 (unsigned int token)
{
	float f;

	f = token * FACTM16BIT;
	if (f < 0.0) {
		gsgl_warning ("gsgl_convert_token_to_0to1", "f < 0.0");
		f = 0.0;
	} else if (f > 1.0) {
		gsgl_warning ("gsgl_convert_token_to_0to1", "f > 1.0");
		f = 1.0;
	}
	return f;
}

unsigned int gsgl_convert_0to1_to_2byte (float token)
{
	if (token < 0.0) {
		if (token < -0.1)
			gsgl_warning ("gsgl_convert_0to1_to_2byte", "token = %f", token);
		token = 0.0;
	} else if (token > 1.0) {
		if (token > 1.1)
			gsgl_warning ("gsgl_convert_0to1_to_2byte", "token = %f", token);
		token = 1.0;
	}
	return token * FACT16BIT + 0.5;
}

unsigned int gsgl_convert_4byteint_to_1st2byte (unsigned int token)
{
	return token / 65536;
}

unsigned int gsgl_convert_4byteint_to_2nd2byte (unsigned int token)
{
	return token%65536;
}

unsigned int gsgl_convert_tokens_to_4byte (unsigned int token1, unsigned int token2)
{
	return token1 * 65536 + token2;
}

float gsgl_convert_token_to_float (unsigned int tokens, unsigned int tokenk)
{
        float x;
        unsigned int a;
        double b;

        /*fprintf(stderr, "%d, %f ", tokens, FACTM16BIT*((float) tokenk));*/
        if (tokens >= 49151) {/* 仮数 < 0.0 & 指数 < 0.0 */
                a = tokens - 49151;
                b = (double) a;
                b *= -1.0;
                x = -1.0 * FACTM16BIT * ((double) tokenk) * pow (10.0, b);
        } else if (tokens >= 32767) { /* 仮数 < 0.0 & 指数部 >= 0.0 */
                a = tokens - 32767;
                x = -1.0 * FACTM16BIT * ((double)tokenk) * pow (10.0, (double) a);
        } else if (tokens >= 16383) { /* 仮数 > 0.0 & 指数部 < 0.0 */
                a = tokens - 16383;
                b = (double) a;
                b *= -1.0;
                x = FACTM16BIT * ((double) tokenk) * pow (10.0, b);
        } else {/*if (tokens >= 0) { 仮数 > 0.0 & 指数部 >= 0.0 */
                a = tokens;
                x = FACTM16BIT * ((double) tokenk) * pow (10.0, (double) a);
        }
        return x;
}

unsigned int gsgl_float_to_int4_ieee754 (float f)
{
	double d;
	double norm;
	int ex, i;
	unsigned int out;

	if (f > 0.0) {
		out = 0;
	} else if (f < 0.0) {
		out = 1 << 31;
	} else if (f == 0.0) {
		return 0;
	} else if (f == -0.0) {
		return 0;
	} else {
		gsgl_warning ("gsgl_float_to_int4_ieee754", "invalid float = %f", f);
		return 0;
	}
	d = fabs(f);
	norm = frexp(d, &ex);
	if (norm == 0.0) return 0;
	ex -= 1;
	norm *= 2.0;
	norm -= 1.0;

	if (ex < -127 || ex > 128) {
		gsgl_warning ("gsgl_float_to_int4_ieee754", "invalid float = %f", f);
		return 0;
	}
	ex += 127;
	out += (ex << 23);
	for (i=22; i>=0; i--) {
		norm *= 2.0;
		if (norm >= 1.0) {
			out += (1 << i);
			norm -= 1.0;
		}
	}
	return out;
}

float gsgl_int4_to_float_ieee754 (unsigned int token1, unsigned int token2)
{
	int sign;
	int ex;
	unsigned int n;
	/*int i;*/

	if (!token1 && !token2) return 0.0;

	/*for (i=0; i<16; i++) {
	  if (token1 & (1 << (15-i)))
	  printf ("1");
	  else
	  printf ("0");
	  if (i==0 || i==8)
	  printf (" ");
	  }
	  for (i=0; i<16; i++) {
	  if (token2 & (1 << (15-i)))
	  printf ("1");
	  else
	  printf ("0");
	  }
	  printf ("\n");*/

	sign = token1 >> 15;
	/*printf ("sign = %d\n", sign);*/
	ex = ((token1 >> 7) & 255) - 127;
	/*printf ("ex = %d\n", ex);*/
	n = (((token1 & 127) + 128) << 16) + token2;
	/*for (i=0; i<24; i++) {
	  if (n & (1 << (23-i)))
	  printf ("1");
	  else
	  printf ("0");
	  }
	  printf ("\n");
	  printf ("123456789012345678901234\n");*/
	if (sign)
		return -ldexp(n, ex-23);
	else
		return ldexp(n, ex-23);
}

void gsgl_message (char *func, char *format, ...)
{
	va_list argp;

	if (format == NULL) return;
	if (func == NULL) return;

	fprintf(stderr, "GSAF/OpenGL : ");
	va_start(argp, format);
	vfprintf(stderr, format, argp);
	va_end(argp);
	fprintf(stderr, " (%s)\n", func);
}

int gsgl_stdin (const char *question, int args, const char *format, ...)
{
#ifdef HAVE_VSSCANF
	char buffer[256];
	int error = 0;
	va_list argp;

	if (question == NULL) return 1;
	if (format == NULL) return 1;
	if (args <= 0) return 1;
	
	do {
		fprintf (stdout, question);
		if (fgets (buffer, 256, stdin) == NULL) return EOF;
		if (buffer[0] == '/' && strlen (buffer) == 2) return 2;
		va_start (argp, format);
		error = vsscanf (buffer, format, argp);
		va_end (argp);
		
	} while (error != args);

	return 0;
#else
	gsgl_warning ("gsgl_stdin", "Your system doesn't have vsscanf");
	return EOF;
#endif
}

void gsgl_warning (char *func, char *format, ...)
{
	va_list argp;

	if (format == NULL) return;
	if (func == NULL) return;

	fprintf(stderr, "GSAF/OpenGL Warning : ");
	va_start(argp, format);
	vfprintf(stderr, format, argp);
	va_end(argp);
	fprintf(stderr, " (%s)\n", func);
}

void gsgl_error (char *func, char *format, ...)
{
	va_list argp;

	if (format == NULL) return;
	if (func == NULL) return;

	fprintf(stderr, "GSAF/OpenGL Error : ");
	va_start(argp, format);
	vfprintf(stderr, format, argp);
	va_end(argp);
	fprintf(stderr, " (%s)\n", func);
	exit (1);
}

void gsgl_set_normal_vector1 (float *x, float *y, float *z, float normal[3])
{
	gsglvec3f vec1, vec2;
	/*float length;*/

	vec1.x = x[1]-x[0];
	vec1.y = y[1]-y[0];
	vec1.z = z[1]-z[0];
	vec2.x = x[2]-x[1];
	vec2.y = y[2]-y[1];
	vec2.z = z[2]-z[1];

	normal[0] = vec1.y * vec2.z - vec1.z * vec2.y;
	normal[1] = vec1.z * vec2.x - vec1.x * vec2.z;
	normal[2] = vec1.x * vec2.y - vec1.y * vec2.x;

	/*length = sqrt(normal[0]*normal[0]+normal[1]*normal[1]+normal[2]*normal[2]);
	  if (length < 1.e-5) {
	  gsgl_warning ("gsgl_set_normal_vector1", "normal verctor's length is zero");
	  normal[0] = 0.0;
	  normal[1] = 0.0;
	  normal[2] = 1.0;
	  } else {
	  normal[0] = normal[0] / length;	
	  normal[1] = normal[1] / length;	
	  normal[2] = normal[2] / length;	
	  }*/
	/*printf ("%f %f %f\n", normal[0],normal[1],normal[2]);*/
}

void gsgl_set_normal_vector2 (float *p1, float *p2, float *p3, float normal[3])
{
	gsglvec3f vec1, vec2;

	vec1.x = p2[0]-p1[0];
	vec1.y = p2[1]-p1[1];
	vec1.z = p2[2]-p1[2];
	vec2.x = p3[0]-p2[0];
	vec2.y = p3[1]-p2[1];
	vec2.z = p3[2]-p2[2];

	normal[0] = vec1.y * vec2.z - vec1.z * vec2.y;
	normal[1] = vec1.z * vec2.x - vec1.x * vec2.z;
	normal[2] = vec1.x * vec2.y - vec1.y * vec2.x;
	/*printf ("%f %f %f\n", normal[0],normal[1],normal[2]);*/
}

void gsgl_mat16_mult_mat16 (float in1[16], float in2[16], float out[16])
{
        int i, j;

        for (i=0; i<16; i++) {
                out[i] = 0.0;
                for (j=0; j<4; j++) {
                        out[i] = out[i] + in1[i%4+j*4] * in2[(i/4)*4+j];
                }
        }
}

void gsgl_mat16_mult_vec4 (float matrix[16], float vec1[4], float vec2[4])
{
        int i, j;

        for (i=0; i<4; i++) {
                vec2[i] = 0.0;
                for (j=0; j<4; j++) {
                        vec2[i] = vec2[i] + matrix[i+j*4] * vec1[j];
                }
        }
}

void gsgl_inq_matrix_frustum (float left, float right, float bottom, float top,
			      float near, float far, float matrix[16])
{
	matrix[0] = 2.0 * near / (right - left);
	matrix[1] = matrix[2] = matrix[3] = matrix[4] = 0.0;
	matrix[5] = 2.0 * near / (top - bottom);
	matrix[6] = matrix[7] = 0.0;
	matrix[8] = (right + left) / (right - left);
	matrix[9] = (top + bottom) / (top - bottom);
	matrix[10]= - (far + near) / (far - near);
	matrix[11]= -1.0;
	matrix[12]= matrix[13] = 0.0;
	matrix[14]= -2.0 * far * near / (far - near);
	matrix[15]= 0.0; 
}

void gsgl_inq_matrix_orthogonal (float left, float right, float bottom, float top,
				 float near, float far, float matrix[16])
{
	matrix[0] = 2.0 / (right - left);
	matrix[1] = matrix[2] = matrix[3] = matrix[4] = 0.0;
	matrix[5] = 2.0 / (top - bottom);
	matrix[6] = matrix[7] = matrix[8] = matrix[9] = 0.0;
	matrix[10]= - 2.0 / (far - near);
	matrix[11]= 0.0;
	matrix[12]= - (right + left) / (right - left);
	matrix[13]= - (top + bottom) / (top - bottom); 
	matrix[14]= - (far + near) / (far - near);
	matrix[15]= 1.0; 	
}

void gsgl_inq_matrix_translate (float x, float y, float z, float matrix[16])
{
	matrix[0] = matrix[5] = matrix[10] = matrix[15] = 1.0; 
	matrix[1] = matrix[2] = matrix[3] = matrix[4] = 0.0; 
	matrix[6] = matrix[7] = matrix[8] = matrix[9] = 0.0; 
	matrix[11] = 0.0;
	matrix[12] = x;
	matrix[13] = y;
	matrix[14] = z;
}

void gsgl_inq_matrix_scale (float xratio, float yratio, float zratio, float matrix[16])
{
	matrix[0] = xratio;
	matrix[5] = yratio;
	matrix[10] = zratio;
	matrix[15] = 1.0; 
	matrix[1] = matrix[2] = matrix[3] = matrix[4] = 0.0; 
	matrix[6] = matrix[7] = matrix[8] = matrix[9] = 0.0; 
	matrix[11] = matrix[12] = matrix[13] = matrix[14] = 0.0; 
}

void gsgl_inq_matrix_rotate (float degree, float x, float y, float z, float matrix[16])
{
	gsglvec3f v, u;
	float len;
	float s[9];
	float uut[9];

	v.x = x;
	v.y = y;
	v.z = z;
	len = sqrt(v.x*v.x+v.y*v.y+v.z*v.z);
	if (len == 0.0) gsgl_warning ("gsgl_inq_matrix_rotate", "rotation axis length is 0");
	
	u.x = v.x/len;
	u.y = v.y/len;
	u.z = v.z/len;

	s[0] = s[4] = s[8] = 0.0;
	s[1] = u.z;
	s[2] = -u.y;
	s[3] = -u.z;
	s[5] = u.x;
	s[6] = u.y;
	s[7] = -u.x;

	uut[0] = u.x * u.x;
	uut[1] = u.y * u.x;
	uut[2] = u.z * u.x;
	uut[3] = u.x * u.y;
	uut[4] = u.y * u.y;
	uut[5] = u.z * u.y;
	uut[6] = u.x * u.z;
	uut[7] = u.y * u.z;
	uut[8] = u.z * u.z;
	
	degree = degree * GSGL_DEG;

	matrix[0] = uut[0] + cos (degree) * (1.0-uut[0]) + sin (degree) * s[0];
	matrix[1] = uut[1] + cos (degree) * (0.0-uut[1]) + sin (degree) * s[1];
	matrix[2] = uut[2] + cos (degree) * (0.0-uut[2]) + sin (degree) * s[2];
	matrix[3] = 0.0;
	matrix[4] = uut[3] + cos (degree) * (0.0-uut[3]) + sin (degree) * s[3];
	matrix[5] = uut[4] + cos (degree) * (1.0-uut[4]) + sin (degree) * s[4];
	matrix[6] = uut[5] + cos (degree) * (0.0-uut[5]) + sin (degree) * s[5];
	matrix[7] = 0.0;
	matrix[8] = uut[6] + cos (degree) * (0.0-uut[6]) + sin (degree) * s[6];
	matrix[9] = uut[7] + cos (degree) * (0.0-uut[7]) + sin (degree) * s[7];
	matrix[10]= uut[8] + cos (degree) * (1.0-uut[8]) + sin (degree) * s[8];
	matrix[11] = matrix[12] = matrix[13] = matrix[14] = 0.0;
	matrix[15] = 1.0;
}

char * gsgl_strdup (const char *src)
{
	int len;
	char *dest;

	len = strlen(src);
	dest = (char *) gsgl_malloc_atomic ((len+1)*sizeof(char));
	strcpy (dest, src);
	return dest;
}

char * gsgl_strndup (const char *src, int size)
{
	char *dest;
	int i;

	dest = (char *) gsgl_malloc_atomic ((size+1)*sizeof(char));
	for (i=0; i<size; i++)
		dest[i] = src[i];
	dest[size] = '\0';
	
	return dest;
}

gsgllist * gsgl_list_new ()
{
	gsgllist *list;
	list = gsgl_malloc (sizeof(gsgllist));
	list->data = NULL;
	list->prev = NULL;
	list->next = NULL;

	return list;
}

gsgllist * gsgl_list_connect (gsgllist *prev, gsgllist *next)
{
	if (!prev || !next) {
		gsgl_warning ("gsgl_list_connect", "prev = %p, next = %p\n", prev, next);
		return NULL;
	}
	prev->next = next;
	next->prev = prev;

	return prev;
}

gsgllist * gsgl_list_append (gsgllist *listin, void *item)
{
	gsgllist *list, *list1;
	list1 = listin;

	if (!list1) {
		list = gsgl_list_new ();
		list->data = item;
		return list;
	} else {
		list = gsgl_list_new ();
		list->data = item;

		gsgl_list_connect (gsgl_list_last(list1), list);
		return listin;
	}
}

gsgllist * gsgl_list_insert (gsgllist *prev, void *item)
{
	gsgllist *list;
	if (!prev) {
		gsgl_warning ("gsgl_list_insert", "pre == NULL");
		return NULL;
	} else {
		if (!prev->next) {
			return gsgl_list_append (prev, item);
		} else {
			list = gsgl_list_new ();
			list->data = item;

			gsgl_list_connect (list, prev->next);
			gsgl_list_connect (prev, list);
			return prev;
		}
	}
}

unsigned int gsgl_list_count (gsgllist *list1)
{
	gsgllist *list;
	unsigned int c = 0;

	list = list1;
	while (list != NULL) {
		list = list->next;
		c++;
	}
	return c;
}

gsgllist * gsgl_list_nth (gsgllist *list1, unsigned int i)
{
	gsgllist *list;
	unsigned int c = 1;

	if (i < 1) {
		gsgl_warning ("argument must > 0", "gsgl_list_nth");
		return NULL;
	}
	
	list = list1;
	while (list != NULL) {
		if (c == i) return list;
		list = list->next;
		c++;
	}
	return NULL;	
}

gsgllist * gsgl_list_last (gsgllist *list0)
{
	gsgllist *list;
	
	if (!list0) return NULL;

	list = list0;
	while (list->next != NULL)
		list = list->next;
	return list;
}

void gsgl_list_delete_link (gsgllist *list)
{
	list->prev = NULL;
	list->next = NULL;
}
