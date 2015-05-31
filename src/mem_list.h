#ifndef __mem_list_h__
#define __mem_list_h__

#ifdef __cplusplus
extern "C"{
#endif

void gsgl_mem_list_inq_size_and_data (gsglDisplaylist *d, gsglPage *);
void gsgl_mem_list_start (gsglPage *);
void gsgl_list_push (unsigned int token, gsglPage *);
void gsgl_mem_list_free_buffer (gsglPage *);
void gsgl_init_mem_list_every_page (gsglPage *p);
void gsgl_mem_list_clear (unsigned int count);
unsigned int gsgl_mem_list_read_token (unsigned short *mem, unsigned int count, unsigned int *token);

#ifdef __cplusplus
}
#endif

#endif
