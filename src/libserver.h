#ifndef __libserver_h__
#define __libserver_h__

typedef struct _server_data {
	unsigned int token;
	int socket;
	gsglboolean get_normal;
	gsglboolean set_normal_already;
	gsglboolean in_line;
	gsglboolean in_polygon;
	gsglboolean Mem;
	gsglboolean Hd;
	gsglboolean List;
} server_data;

#ifdef __cplusplus
extern "C"{
#endif

int gsgl_server_read_token1 (int connected_socket, unsigned int *token);
gsglPage * gsgl_server_interpret (server_data *data, gsglPage *, void (*func_default)(unsigned int, void *, gsglPage *, READ_TOKEN));

#ifdef __cplusplus
}
#endif

#endif
