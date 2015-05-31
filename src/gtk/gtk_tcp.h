#ifndef __gtk_tcp_h__
#define __gtk_tcp_h__

#ifdef __cplusplus
extern "C"{
#endif

void gui_tcp_callback_client (int socket, gsglPage *p);
int gsgl_gtk_tcp_input_add (gsglPage *p);
void gsgl_gtk_tcp_input_remove (int handler, gsglPage *p);

#ifdef __cplusplus
}
#endif

#endif
