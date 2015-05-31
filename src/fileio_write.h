#ifndef __fileio_write_h__
#define __fileio_write_h__

struct _gsglWriteToFile {
	gsglboolean little_endian;
	unsigned int byte;
	unsigned int surplus;
	unsigned int bytebuffer[2];
};

#ifdef __cplusplus
extern "C"{
#endif

void gsglfwrite_write (unsigned int i, FILE *fp, gsglWriteToFile *w);
void gsglfwrite_write_float (GLfloat x, FILE *fp, gsglWriteToFile *w);
void gsgl_write_to_file (FILE *fp, gsglWriteToFile *w, gsglPage *p);
gsglWriteToFile * gsgl_init_write_to_file (void);
void gsgl_save (int *size, gsglPage *);

#ifdef __cplusplus
}
#endif

#endif
