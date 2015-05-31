#ifndef __qt_tcp_h
#define __qt_tcp_h

#ifdef __cplusplus
extern "C"{
#endif

void gui_tcp_callback_client (int socket, gsglPage *);
void gsgl_qt_tcp_callback_server (int socket, gsglPage *);

#ifdef __cplusplus
}
#endif

#endif
