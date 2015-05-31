#ifndef __harddisk_h__
#define __harddisk_h__

void gsgl_init_hd_every_page (gsglboolean use_hd, gsglPage *);
void gsgl_finish_hd_every_page (gsglPage *p);
void gsgl_hd_push (unsigned int token, gsglPage *);
void gsgl_hd_push_float (float x);
void gsgl_finish_hd_every_page (gsglPage *p);
void gsgl_finish_hd_for_anime (gsglPage *p);
void gsgl_init_hd_for_anime (gsglPage *);
void gsgl_end_hd_for_anime (gsglPage *);
unsigned long int gsgl_hd_ftell (int *surplus, gsglPage *);

#endif
