#ifndef __fileio_base_h__
#define __fileio_base_h__

#ifdef __cplusplus
extern "C"{
#endif

void gsgl_file_read_close (gsglLibrary *);
void gsgl_file_write_close (gsglLibrary *);
gsglboolean gsgl_file_write_open (const char *file, gsglLibrary *);
void gsgl_file_read_open (char *filename, gsglLibrary *);
FILE *gsgl_get_fp_write (gsglLibrary *);
FILE *gsgl_get_fp_read (gsglLibrary *);

void gsfopn (char *fcsnamn, int *ind, gsglLibrary *);
void gsfcls (gsglLibrary *);
void gsfon (void);
void gsfoff (void);
char * flname (void);
void flsave (int npage, gsglPage *);

#ifdef __cplusplus
}
#endif

#endif
