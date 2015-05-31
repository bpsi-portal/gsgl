#ifndef __fileio_read_base_h__
#define __fileio_read_base_h__

struct _gsglReadFile {
	unsigned int surplus;
	unsigned int bytebuffer[2];
};

unsigned char * gsglfread_token (unsigned char * loc, unsigned int *i4, gsglReadFile *r);
gsglReadFile * gsgl_init_read_file (void);

#endif
