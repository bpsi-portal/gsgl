#include "gsaf.h"
#include "gsgl.h"
#include "utils.h"
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h> 
#include <signal.h>
#include "fileformat.h"
#include "base.h"
#include "drv_wrapper.h"
#include "displaylist.h"
#include "matrix.h"
#include "clientstate.h"
#include "tcpip.h"
#include "configfile.h"
#include "anime.h"
#include "fileio_read.h"
#include "bsc1.h"
#include "guiitems.h"
#include "libserver.h"
#include "gui_common.h"

/*void close_child ()
  {
  int  pid;
  
  pid = wait (0);
  signal (SIGCHLD, close_child);
  }
*/
#define GSGL_DEBUG(x)

static int InitSignal (void);
static gsglPage * interpret (server_data *data, gsglPage *p);

int main (int argc, char *argv[])
{
	int connected_socket, listening_socket;
	struct sockaddr_in sin;
	unsigned int len;
	int sock_optval = 1;
	int port = GSGL_TCP_DEFAULT_PORT;
	extern char *optarg;
	extern int optopt;
	char c;

	while ((c = getopt(argc, argv, ":hp:")) != -1) {
                switch (c) {
                case 'h':
			fprintf (stderr, "usage: %s [-h] [-p port]\n", argv[0]);
			fprintf (stderr, "           -h      : print this message\n");
			fprintf (stderr, "           -p port : sepecify port number\n");
			exit (1);
                case 'p':
                        port = atoi (optarg);
                        break;
                case ':':        /* -ens without arguments */
                        gsgl_error ("gsglserver", "Option -%c requires an argument", optopt);
                case '?':
                        gsgl_error ("gsglserver", "Unrecognized option: - %c", optopt);
                }
	}
	/*signal (SIGCHLD, close_child);*/
	InitSignal();

        /* create a listening socket */
	listening_socket = socket (AF_INET, SOCK_STREAM, 0);
	
        /* set socket options */
	if (setsockopt (listening_socket, SOL_SOCKET, SO_REUSEADDR,
			&sock_optval, sizeof(sock_optval)) == -1){
		gsgl_error ("", "setsockopt failed");
	}
        /* setup address family, port number, ip address */
	/*bzero((char *) &sin, sizeof(sin));*/
	memset((char *) &sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons (port);
	sin.sin_addr.s_addr = htonl (INADDR_ANY);
	

        /* allocate address to the socket */
	if (bind (listening_socket, (struct sockaddr *)&sin, sizeof(sin)) < 0){
		gsgl_error ("server.c", "bind failed");
	}
        /* list the port */
	if (listen (listening_socket, SOMAXCONN) == -1){
		gsgl_error ("server.c", "listen failed");
	}
	
	while (1) {
		struct hostent *peer_host;
		struct sockaddr_in peer_sin;
		/*int pfd[2];*/
		int pid;

		len = sizeof (sin);
                /* wait for connections */
		connected_socket = accept (listening_socket, (struct sockaddr *)&sin, &len);
		if (connected_socket == -1){
			gsgl_error ("", "accept failed");
		}
		
                /* display client's host-port informations */
		len = sizeof (peer_sin);
		getpeername (connected_socket, (struct sockaddr *)&peer_sin, &len);
		
		peer_host = gethostbyaddr ((char *)&peer_sin.sin_addr.s_addr,
					   sizeof(peer_sin.sin_addr), AF_INET);
		
		GSGL_DEBUG(fprintf (stderr, "GSAF/OpenGL : connection from %s [%s] port %d", peer_host->h_name, inet_ntoa(peer_sin.sin_addr), ntohs(peer_sin.sin_port)));
		  
		pid = fork ();
		if (pid == -1) {
			perror ("fork");
			exit (1);
		} else if (pid == 0) { /* child process */
			server_data *data;
			gsglPage *p = NULL;

			close (listening_socket);
			listening_socket = -1;
			
			data = (server_data *) gsgl_malloc (sizeof (server_data));
			data->socket = connected_socket;

			data->get_normal = GSGL_FALSE;
			data->set_normal_already = GSGL_FALSE;
			data->in_line = GSGL_FALSE;
			data->in_polygon = GSGL_FALSE;

			while (1) {
				int read_size;
				unsigned int token;
				
				read_size = gsgl_server_read_token1 (connected_socket, &token);
				GSGL_DEBUG(fprintf(stderr, "server token = %d\n", token));
				if (read_size == 0) break;

				data->token = token;
				p = interpret (data, p);
			}
			_exit (0);
		} else { /* parent process */

			close (connected_socket);
			connected_socket = -1;

			GSGL_DEBUG(fprintf (stderr, " ... connection lost\n"));
		}		
	}
	close (listening_socket);
	return 0;
}

static void read_and_putenv (int socket, const char *format)
{
	unsigned int token;
	char *str;

	gsgl_tcp_read (socket, &token);
	str = gsgl_malloc_atomic (sizeof(char)*256);
	sprintf (str, format, token); putenv (str);
}

static void gsgl_config_read_server (int socket)
{
	int c1;
	unsigned int id, id1, id2;
	char *str;

	putenv ("GSGL_I_AM_A_SERVER=true");

	gsgl_tcp_read (socket, (unsigned int *) &c1);
	str = gsgl_malloc_atomic (sizeof(char)*256);
	sprintf (str, "GSGDP=%cc", (char) c1); putenv (str);

	gsgl_tcp_read (socket, &id1);
	gsgl_tcp_read (socket, &id2);
	id = gsgl_convert_tokens_to_4byte (id1, id2);
	str = gsgl_malloc_atomic (sizeof(char)*256);
	sprintf (str, "GSGL_WINDOW_ID=%d", id); putenv (str);

	read_and_putenv (socket, "GSGL_BG=%d");
	read_and_putenv (socket, "GSGL_BOX_R=%d");
	read_and_putenv (socket, "GSGL_BOX_G=%d");
	read_and_putenv (socket, "GSGL_BOX_B=%d");
	read_and_putenv (socket, "GSGL_FRAME_R=%d");
	read_and_putenv (socket, "GSGL_FRAME_G=%d");
	read_and_putenv (socket, "GSGL_FRAME_B=%d");
	read_and_putenv (socket, "GSGL_WINDOW_W=%d");
	read_and_putenv (socket, "GSGL_WINDOW_H=%d");
	read_and_putenv (socket, "GSGL_WINDOW_X=%d");
	read_and_putenv (socket, "GSGL_WINDOW_Y=%d");
}

static void gsgl_set_server_socket (int socket)
{
	char *str;
	str = gsgl_malloc_atomic (sizeof(char)*256);
	sprintf (str, "GSGL_SERVER_SOCKET=%d", socket); putenv (str);
}

static void server_default (unsigned int tokeni, void *data1, gsglPage *p, READ_TOKEN)
{
	server_data *data = data1;

	switch (tokeni) {
	case GSGL_TCP_OPEN:
		GSGL_DEBUG(fprintf (stderr, "open\n"));
		gsgl_config_read_server (data->socket);
		gsgl_set_server_socket (data->socket);
		gsaf_gsopen ();
		gsgl_servercallback_set (gsgl_server_interpret);
		break;
	case GSGL_TCP_CLOSE:
		GSGL_DEBUG(fprintf (stderr, "close\n"));
		gsaf_gsclos ();
		_exit (0);
		break;
	case GSGL_TCP_GUI_CLEAR_LIST:
		gsaf_eras ();
		break;
	default:
		break;
	}
}

static gsglPage * interpret (server_data *data, gsglPage *p)
{
	return gsgl_server_interpret (data, p, server_default);
}

static void CloseChild (int i)
{
	int pid;

	pid = wait (0);
	signal (SIGCHLD, CloseChild);
}

static int InitSignal (void)
{
	/*signal (SIGINT, CloseSocket);
	  signal (SIGTERM, CloseSocket);*/
	signal (SIGCHLD, CloseChild);

	return 0;
}
