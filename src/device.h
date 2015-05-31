#ifndef __device_h__
#define __device_h__

#ifdef __cplusplus
extern "C"{
#endif

void gsgl_set_mode (int mode, gsglLibrary *lib);
int gsgl_inq_devicetype (gsglLibrary *);
int gsgl_inq_page_number (gsglLibrary *);
void gsgl_finish_common (gsglPage *);
void gsgl_finish_gui (gsglPage *);
void gsgl_finish_tcp (gsglPage *);
void gsgl_device_judge (gsglLibrary *);
void gsgl_device_type (int4 *ich, gsglLibrary *);
void gsgl_device_open (int4 *ich, gsglLibrary *);
void gsgl_device_close (int4 *ich, gsglLibrary *);
void gsgl_finish_every_page (gsglPage *);
void gsgl_device_pagee (int4 *ich, gsglPage *);
void gsgl_usedtime (float time, gsglLibrary *);
gsglWindow * gsgl_device_pages (int4 npage, float sizex, float sizey, int4 lkeep, gsglPage *);
gsglPage * gsgl_page_new (gsglLibrary *lib, gsglWindow *w);

#ifdef __cplusplus
}
#endif

#endif
