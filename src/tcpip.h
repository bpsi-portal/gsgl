#ifndef __tcpip_h__
#define __tcpip_h__

#ifdef __cplusplus
extern "C"{
#endif

void gsgl_tcp_write_1byte (int socket, unsigned char token);
void gsgl_tcp_read_1byte (int socket, unsigned char * token);
void gsgl_tcp_push_1byte (unsigned char token, gsglPage *);
void gsgl_tcp_exit_from_pagee (gsglPage *);
void gsgl_tcp_receive_list_info_from_server (gsglboolean *remade, gsglboolean *lighting, int *num_list, int *num_projection, int *num_modelview, float **projection, float **modelview, gsglPage *);
void gsgl_tcp_send_list_info_to_client (gsglPage *);
gsglboolean gsgl_init_tcp_once_in_program (char *hostname, int port, int c1, gsglLibrary *);
void gsgl_finish_tcp_once_in_program (gsglLibrary *);
void gsgl_tcp_read (int socket, unsigned int * token);
void gsgl_tcp_write (int socket, unsigned int token);
void gsgl_tcp_push (unsigned int token, gsglPage *);
void gsgl_tcp_push_server (unsigned int token, gsglPage *);
void gsgl_page_end_tcpip (gsglPage *);
char * gsgl_inq_server_host (int *port);
int gsgl_inq_server_socket (gsglPage *);
void gsgl_check_i_am_a_server (gsglLibrary *);
gsglboolean gsgl_inq_i_am_a_server (gsglLibrary *);
int gsgl_tcp_ping_to_client (gsglPage *);
void gsgl_tcp_send_quit_to_client (gsglLibrary *);
void gsgl_tcp_remake_tell_to_client (gsglLibrary *);
void gsgl_tcp_server_send_command (unsigned int com, gsglLibrary *lib);
/*void gsgl_tcp_print (gsglLibrary *);
  void gsgl_tcp_export_vec_eps (gsglLibrary *);
  void gsgl_tcp_export_ras_eps (gsglLibrary *);
  void gsgl_tcp_export_png (gsglLibrary *);
  void gsgl_tcp_export_jpeg (gsglLibrary *);*/
void gsgl_tcp_dump_window (gsglWindow *w, gsglLibrary *);
/*void gsgl_tcp_save (gsglLibrary *);
  void gsgl_tcp_save_and_close (gsglLibrary *);
  void gsgl_tcp_save_as (gsglLibrary *);
  void gsgl_tcp_config_color (gsglLibrary *);
  void gsgl_tcp_config_command (gsglLibrary *);
  void gsgl_tcp_config_ttf (gsglLibrary *);
  void gsgl_tcp_config_window (gsglLibrary *);
  void gsgl_tcp_save_this_frame (gsglLibrary *);*/
void gsgl_tcp_nth_frame (unsigned int n, gsglLibrary *);
int gsgl_inq_client_socket (gsglLibrary *);
void gsgl_client_callback_template (int socket, gsglPage *p,
				    void (*func_exit_from_pagee)(void),
				    void (*func_save_and_close)(void),
				    void (*func_save_as)(void),
				    void (*func_save)(void),
				    void (*func_save_this_frame)(void),
				    void (*func_export_vec_eps)(void),
				    void (*func_export_ras_eps)(void),
				    void (*func_export_png) (void),
				    void (*func_export_jpg) (void),
				    void (*func_print) (void),
				    void (*func_config_color) (void),
				    void (*func_config_command) (void),
				    void (*func_config_font) (void),
				    void (*func_config_window) (void));
void gsgl_server_callback_template (int socket, gsglPage *p,
				    void (*func_save_and_close)(void));

#ifdef __cplusplus
}
#endif

#endif
