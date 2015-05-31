#include "gsaf.h"
#include "gsgl.h"
#include <sys/types.h>  /* socket() */
#include <sys/socket.h> /* socket() */
#include <netinet/in.h> /* struct sockaddr_in */
#include <netdb.h>      /* gethostbyname() */
#include <unistd.h>     /* write, read, close */
#include "utils.h"
#include "fileformat.h"
#include "tcpip.h"
#include "device.h"
#include "xwin.h"
#include "bsc1.h"
#include "configfile.h"
#include "displaylist.h"
#include "anime.h"
#include "harddisk.h"
#include "drv_wrapper.h"
#include "bsc1.h"
#include "guiitems.h"
#include "tcp_guiitems.h"
#include "gui_common.h"
#ifdef USE_GTK
#    include "gtk_tcp.h"
#endif
#ifdef USE_QT
#    include "qt_tcp.h"
#endif

#define GSGL_DEBUG(x)

void gsgl_tcp_read (int socket, unsigned int * token)
{
	int read_size;
	unsigned char buffer[2];

	read_size = read (socket, buffer, 2);
	if (read_size == 0) gsgl_error ("gsgl_tcp_read", "connection lost");

	*token = buffer[0] * 256 + buffer[1];
}

void gsgl_tcp_read_1byte (int socket, unsigned char * token)
{
	int read_size;

	read_size = read (socket, token, 1);
	if (read_size == 0) gsgl_error ("gsgl_tcp_read", "connection lost");
}

void gsgl_tcp_write (int socket, unsigned int token)
{
	int write_size;
	unsigned char buffer[2];

	buffer[0] = token / 256;
	buffer[1] = token - buffer[0] * 256;

	if ((write_size = write (socket, buffer, 2)) < 0) {
		gsgl_error ("gsgl_tcp_write", "write token %d returns %d", token, write_size);
	}
}

void gsgl_tcp_write_1byte (int socket, unsigned char token)
{
	int write_size;

	if ((write_size = write (socket, &token, 1)) < 0) {
		gsgl_error ("gsgl_tcp_write", "write token %d returns %d", token, write_size);
	}
}

/*************************/
/* client side functions */
/*************************/

char * gsgl_inq_server_host (int *port)
{
	char *str = NULL;
	char *host = NULL;
	int i, len;
	gsglboolean have_colon = GSGL_FALSE;
	int colon = 0;

	str = getenv ("GSGL_SERVER");
	if (str && (len = strlen (str)) > 0) {
		for (i=0; i<len; i++) {
			if (str[i] == ':') {
				have_colon = GSGL_TRUE;
				colon = i;
				break;
			}
			
		}
		if (have_colon) {
			host = malloc ((colon+1) * sizeof (char));
			for (i=0; i<colon; i++)
				host[i] = str[i];
			host[colon] = '\0';
			if (len >= colon+1) {
				*port = atoi (&str[colon+1]);
			} else {
				*port = GSGL_TCP_DEFAULT_PORT;
			}
		} else {
			host = str;
			*port = GSGL_TCP_DEFAULT_PORT;
		}
	} else {
		host = NULL;
		*port = 0;
	}
	return host;
}

/*****************************/
/*     Initialize TCP/IP     */
/*****************************/
gsglboolean gsgl_init_tcp_once_in_program (char *hostname, int port, int c1, gsglLibrary *lib)
{
        struct hostent *hostent;
        struct sockaddr_in addr;
	
	lib->Client_Socket = -1;

	addr.sin_family = AF_INET ;
	if ((hostent = gethostbyname (hostname)) == NULL) {
		gsgl_message ("gsgl_init_tcp_once_in_program", "unknown host %s", hostname);
		gsgl_message ("gsgl_init_tcp_once_in_program", "falling into local mode");
		return GSGL_FALSE;
	}

	memcpy (&addr.sin_addr, hostent->h_addr, hostent->h_length);
	addr.sin_port = htons (port);
	
	if ((lib->Client_Socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		gsgl_message ("gsgl_init_tcp_once_in_program", "socket");
		gsgl_message ("gsgl_init_tcp_once_in_program", "falling into local mode");
		return GSGL_FALSE;
	}
	
	if (connect (lib->Client_Socket, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
		close (lib->Client_Socket);
		gsgl_message ("gsgl_init_tcp_once_in_program", "Could not connect to server in %s", hostname);
		gsgl_message ("gsgl_init_tcp_once_in_program", "falling into local mode");
		return GSGL_FALSE;
	}

	gsgl_tcp_write (lib->Client_Socket, GSGL_TCP_OPEN);
	gsgl_config_send_to_server (c1, inq_parent_window (lib), lib);

	return GSGL_TRUE;
}

void gsgl_finish_tcp_once_in_program (gsglLibrary *lib)
{
	gsgl_tcp_write (lib->Client_Socket, GSGL_TCP_CLOSE);
	close (lib->Client_Socket);
}


void gsgl_page_end_tcpip (gsglPage *p)
{
	gsgl_tcp_push (GSGL_FILE_END_PAGE, p);
	
	if (gsgl_inq_anime_is_used (p)) {
		gsgl_finish_hd_for_anime (p);
		gsgl_anime_backup_displaylist (p);
	}

	gui_tcp_callback_client (p->library->Client_Socket, p);
}

int gsgl_inq_client_socket (gsglLibrary *lib)
{
	return lib->Client_Socket;
}

/*************************/
/* server side functions */
/*************************/

void gsgl_tcp_send_quit_to_client (gsglLibrary *lib)
{
	if (!gsgl_inq_i_am_a_server (lib)) return;
	gsgl_tcp_write (lib->Server_socket, GSGL_TCP_QUIT);
}

void gsgl_tcp_remake_tell_to_client (gsglLibrary *lib)
{
	if (!gsgl_inq_i_am_a_server (lib)) return;
	
	gsgl_tcp_write (lib->Server_socket, GSGL_TCP_REMADE);
}

void gsgl_check_i_am_a_server (gsglLibrary *lib)
{
	char *str = NULL;

	lib->I_am_a_server = GSGL_FALSE;

	str = getenv ("GSGL_I_AM_A_SERVER");
	if (str && strlen (str) > 0) {
		if (!strcmp (str, "true")) lib->I_am_a_server = GSGL_TRUE;
	}
}

gsglboolean gsgl_inq_i_am_a_server (gsglLibrary *lib)
{
	return lib->I_am_a_server;
}

int gsgl_inq_server_socket (gsglPage *p)
{
	return p->library->Server_socket;
}

/******************/
/*  push a token  */
/******************/
void gsgl_tcp_push (unsigned int token, gsglPage *p)
{
	gsgl_tcp_write (p->library->Client_Socket, token);
}

void gsgl_tcp_push_1byte (unsigned char token, gsglPage *p)
{
	gsgl_tcp_write_1byte (p->library->Client_Socket, token);
}

void gsgl_tcp_exit_from_pagee (gsglPage *p)
{
	gsgl_tcp_push (GSGL_TCP_EXIT_FROM_PAGEE, p);
}

void gsgl_tcp_push_server (unsigned int token, gsglPage *p)
{
	gsgl_tcp_write (p->library->Server_socket, token);
}

/*static void print_matrix (unsigned int * matrix)
  {
  unsigned int *buf;
  
  buf = matrix;
  printf ("%f %f %f %f\n", 
  gsgl_convert_token_to_float (buf[0],buf[1]),
  gsgl_convert_token_to_float (buf[8],buf[9]),
  gsgl_convert_token_to_float (buf[16],buf[17]),
  gsgl_convert_token_to_float (buf[24],buf[25]));
  printf ("%f %f %f %f\n", 
  gsgl_convert_token_to_float (buf[2],buf[3]),
  gsgl_convert_token_to_float (buf[10],buf[11]),
  gsgl_convert_token_to_float (buf[18],buf[19]),
  gsgl_convert_token_to_float (buf[26],buf[27]));
  printf ("%f %f %f %f\n", 
  gsgl_convert_token_to_float (buf[4],buf[5]),
  gsgl_convert_token_to_float (buf[12],buf[13]),
  gsgl_convert_token_to_float (buf[20],buf[21]),
  gsgl_convert_token_to_float (buf[28],buf[29]));
  printf ("%f %f %f %f\n\n", 
  gsgl_convert_token_to_float (buf[6],buf[7]),
  gsgl_convert_token_to_float (buf[14],buf[15]),
  gsgl_convert_token_to_float (buf[22],buf[23]),
  gsgl_convert_token_to_float (buf[30],buf[31]));
  }*/

void gsgl_tcp_send_list_info_to_client (gsglPage *p)
{
	gsglboolean remade, lighting;
	int         num_list;
	int         num_projection;
	int         num_modelview;
	unsigned int *projection;
	unsigned int *modelview;
	int i, j;

	if (!gsgl_inq_i_am_a_server (p->library)) return;

	gsgl_displaylist_inq_list_info (&remade, &lighting, &num_list, &num_projection, &num_modelview, &projection, &modelview, p);

	/*printf ("server %d %d, %d %d %d\n", remade, lighting, num_list, num_projection, num_modelview);*/
	gsgl_tcp_write (p->library->Server_socket, GSGL_TCP_REMADE_INFO);
	gsgl_tcp_write (p->library->Server_socket, remade);
	gsgl_tcp_write (p->library->Server_socket, lighting);
	gsgl_tcp_write (p->library->Server_socket, num_list);
	gsgl_tcp_write (p->library->Server_socket, num_projection);
	gsgl_tcp_write (p->library->Server_socket, num_modelview);
	for (i=0; i<num_projection; i++) {
		for (j=0; j<32; j++) {
			gsgl_tcp_write (p->library->Server_socket, projection[i*32+j]);
		}
		/*print_matrix (&projection[i*32]);*/
	}
	for (i=0; i<num_modelview; i++) {
		for (j=0; j<32; j++) {
			gsgl_tcp_write (p->library->Server_socket, modelview[i*32+j]);
		}
		/*print_matrix (&modelview[i*32]);*/
	}
}

void gsgl_tcp_receive_list_info_from_server (gsglboolean *remade, gsglboolean *lighting, int *num_list, int *num_projection, int *num_modelview, float **projection_float, float **modelview_float, gsglPage *p)
{
	int i, j;
	unsigned int token;
	unsigned int token1, token2;
	
	if (gsgl_inq_mode (p->library) != GSGL_MODE_TCPIP) return;
	
	gsgl_tcp_push (GSGL_TCP_REMADE_INFO, p);

	gsgl_tcp_read (p->library->Client_Socket, &token);
	if (token != GSGL_TCP_REMADE_INFO) 
		gsgl_error ("receive_list_info", "token != GSGL_TCP_REMADE_INFO, %d", token);
	gsgl_tcp_read (p->library->Client_Socket, (unsigned int *) remade);
	if (*remade != GSGL_TRUE) 
		gsgl_error ("receive_list_info", "remade != TRUE");
	gsgl_tcp_read (p->library->Client_Socket, (unsigned int *)  lighting);
	if (*lighting != GSGL_FALSE && *lighting != GSGL_TRUE) 
		gsgl_error ("receive_list_info", "lighting != FALSE & TRUE");
	gsgl_tcp_read (p->library->Client_Socket, (unsigned int *) num_list);
	if (*num_list <= 0) 
		gsgl_error ("receive_list_info", "number of displaylist is negative");
	gsgl_tcp_read (p->library->Client_Socket, (unsigned int *) num_projection);
	if (*num_projection <= 0) 
		gsgl_error ("receive_list_info", "number of projection matrix is negative");
	gsgl_tcp_read (p->library->Client_Socket, (unsigned int *) num_modelview);
	if (*num_modelview <= 0) 
		gsgl_error ("receive_list_info", "number of modelview matrix is negative");

	*projection_float = (float *) gsgl_malloc_atomic (*num_projection * 16 * sizeof (float));
	for (i=0; i<*num_projection; i++) {
		for (j=0; j<16; j++) {
			gsgl_tcp_read (p->library->Client_Socket, &token1);
			gsgl_tcp_read (p->library->Client_Socket, &token2);
			(*projection_float)[i*16+j] = gsgl_convert_token_to_float (token1, token2);
		}
	}

	*modelview_float = (float *) gsgl_malloc_atomic (*num_modelview * 16 * sizeof (float));
	for (i=0; i<*num_modelview; i++) {
		for (j=0; j<16; j++) {
			gsgl_tcp_read (p->library->Client_Socket, &token1);
			gsgl_tcp_read (p->library->Client_Socket, &token2);
			(*modelview_float)[i*16+j] = gsgl_convert_token_to_float (token1, token2);
		}
	}
}

/****************************/
/*   GUI callback functions */
/****************************/

void gsgl_tcp_server_send_command (unsigned int com, gsglLibrary *lib)
{
	if (!gsgl_inq_i_am_a_server (lib)) return;
	gsgl_tcp_write (lib->Server_socket, com);
}

/*void gsgl_tcp_print (gsglLibrary *lib)
  {
  if (!gsgl_inq_i_am_a_server (lib)) return;
  gsgl_tcp_write (lib->Server_socket, GSGL_TCP_GUI_PRINT);
  }
  
  void gsgl_tcp_export_vec_eps (gsglLibrary *lib)
  {
  if (!gsgl_inq_i_am_a_server (lib)) return;
  gsgl_tcp_write (lib->Server_socket, GSGL_TCP_GUI_EXPORT_VEC_EPS);
  }
  
  void gsgl_tcp_export_ras_eps (gsglLibrary *lib)
  {
  if (!gsgl_inq_i_am_a_server (lib)) return;
  gsgl_tcp_write (lib->Server_socket, GSGL_TCP_GUI_EXPORT_RAS_EPS);
  }
  
  void gsgl_tcp_export_png (gsglLibrary *lib)
  {
  if (!gsgl_inq_i_am_a_server (lib)) return;
	gsgl_tcp_write (lib->Server_socket, GSGL_TCP_GUI_EXPORT_PNG);
	}
	
	void gsgl_tcp_export_jpeg (gsglLibrary *lib)
	{
	if (!gsgl_inq_i_am_a_server (lib)) return;
	gsgl_tcp_write (lib->Server_socket, GSGL_TCP_GUI_EXPORT_JPEG);
	}*/

void gsgl_tcp_dump_window (gsglWindow *w, gsglLibrary *lib)
{
	unsigned int id;

	if (!gsgl_inq_i_am_a_server (lib)) return;
	gsgl_tcp_write (lib->Server_socket, GSGL_TCP_GUI_DUMP);
	id = gsgl_inq_main_window (w);
	gsgl_tcp_write (lib->Server_socket, gsgl_convert_4byteint_to_1st2byte (id));
	gsgl_tcp_write (lib->Server_socket, gsgl_convert_4byteint_to_2nd2byte (id));
}

/*void gsgl_tcp_save (gsglLibrary *lib)
  {
  if (!gsgl_inq_i_am_a_server (lib)) return;
  gsgl_tcp_write (lib->Server_socket, GSGL_TCP_GUI_SAVE);
  }
  
  void gsgl_tcp_save_and_close (gsglLibrary *lib)
  {
  if (!gsgl_inq_i_am_a_server (lib)) return;
  gsgl_tcp_write (lib->Server_socket, GSGL_TCP_GUI_SAVE_AND_CLOSE);
  }
  
  void gsgl_tcp_save_as (gsglLibrary *lib)
  {
  if (!gsgl_inq_i_am_a_server (lib)) return;
  gsgl_tcp_write (lib->Server_socket, GSGL_TCP_GUI_SAVE_AS);
  }
  
  void gsgl_tcp_save_this_frame (gsglLibrary *lib)
  {
  if (!gsgl_inq_i_am_a_server (lib)) return;
  gsgl_tcp_write (lib->Server_socket, GSGL_TCP_GUI_SAVE_THIS_FRAME);
  }*/

void gsgl_tcp_nth_frame (unsigned int n, gsglLibrary *lib)
{
	if (!gsgl_inq_i_am_a_server (lib)) return;
	gsgl_tcp_write (lib->Server_socket, GSGL_TCP_NTH_FRAME);
	gsgl_tcp_write (lib->Server_socket, n);
}

/*void gsgl_tcp_config_color (gsglLibrary *lib)
  {
  if (!gsgl_inq_i_am_a_server (lib)) return;
  gsgl_tcp_write (lib->Server_socket, GSGL_TCP_GUI_CONFIG_COLOR);
  }
  
  void gsgl_tcp_config_command (gsglLibrary *lib)
  {
  if (!gsgl_inq_i_am_a_server (lib)) return;
  gsgl_tcp_write (lib->Server_socket, GSGL_TCP_GUI_CONFIG_COMMAND);
  }
  
  void gsgl_tcp_config_ttf (gsglLibrary *lib)
  {
  if (!gsgl_inq_i_am_a_server (lib)) return;
  gsgl_tcp_write (lib->Server_socket, GSGL_TCP_GUI_CONFIG_FONT);
  }
  
  void gsgl_tcp_config_window (gsglLibrary *lib)
  {
  if (!gsgl_inq_i_am_a_server (lib)) return;
  gsgl_tcp_write (lib->Server_socket, GSGL_TCP_GUI_CONFIG_WINDOW);
  }*/

/* client side callback function template */
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
				    void (*func_config_window) (void))
{
	unsigned int token, token1, token2, id, i;
	void (*quitcallback)();
	float value;
	unsigned int item, num;
	int active;
	char *text;
	int vali;
	float x, y, size, angle;
	unsigned int fnt, ijust;

	gsgl_tcp_read (socket, &token);
	
	switch (token) {
	case GSGL_TCP_REMADE:
		gsgl_displaylist_remake (p);
		break;
	case GSGL_TCP_EXIT_FROM_PAGEE:
		func_exit_from_pagee ();
		break;
	case GSGL_TCP_QUIT:
		gsgl_finish_tcp (p);
		quitcallback = gsaf_inq_quit_callback ();
		if (quitcallback != NULL)
			quitcallback ();
		exit (0);
	case GSGL_TCP_PING:
		break;
	case GSGL_TCP_GUI_SAVE_AND_CLOSE:
		func_save_and_close ();
		break;
	case GSGL_TCP_GUI_SAVE_AS:
		func_save_as ();
		break;
	case GSGL_TCP_GUI_SAVE:
		func_save ();
		break;
	case GSGL_TCP_GUI_SAVE_THIS_FRAME:
		func_save_this_frame ();
		break;
	case GSGL_TCP_NTH_FRAME:
		gsgl_tcp_read (socket, &token);
		gsgl_disable_drv_tcp (p);
		gsgl_anime_init_replay (p);
		gsgl_anime_nth_frame (token, p);
		gsgl_enable_drv_tcp (p);
		break;
	case GSGL_TCP_GUI_EXPORT_VEC_EPS:
		func_export_vec_eps ();
		break;
	case GSGL_TCP_GUI_EXPORT_RAS_EPS:
		func_export_ras_eps ();
		break;
	case GSGL_TCP_GUI_EXPORT_PNG:
		func_export_png ();
		break;
	case GSGL_TCP_GUI_EXPORT_JPEG:
		func_export_jpg ();
		break;
	case GSGL_TCP_GUI_PRINT:
		func_print ();
		break;
	case GSGL_TCP_GUI_DUMP:
		gsgl_tcp_read (socket, &token);
		gsgl_tcp_read (socket, &token2);
		gsgl_dumpwin_id (gsgl_convert_tokens_to_4byte (token, token2), p);
		break;
	case GSGL_TCP_GUI_CONFIG_COLOR:
		func_config_color ();
		break;
	case GSGL_TCP_GUI_CONFIG_COMMAND:
		func_config_command ();
		break;
	case GSGL_TCP_GUI_CONFIG_FONT:
		func_config_font ();
		break;
	case GSGL_TCP_GUI_CONFIG_WINDOW:
		func_config_window ();
		break;
	case GSGL_TCP_GUI_BUTTON_CLICKED:
		GSGL_DEBUG(fprintf (stderr, "client callback button\n"));
		gsgl_tcp_read (socket, &token);
		gsgl_tcp_guiitems_callback (token, NULL, p); /* token = id */
		break;
	case GSGL_TCP_GUI_COMBO_CHANGED:
		GSGL_DEBUG(fprintf (stderr, "client callback combo\n"));
		gsgl_tcp_read (socket, &id);
		gsgl_tcp_read (socket, &token); /* token == GSGL_TCP_GUI_COMBO_ITEM */
		gsgl_tcp_read (socket, &item);
		gsgl_tcp_guiitems_callback (id, &item, p);
		break;
	case GSGL_TCP_GUI_SPINF_VALUECHANGED:
		GSGL_DEBUG(fprintf (stderr, "client callback spinf\n"));
		gsgl_tcp_read (socket, &id);
		gsgl_tcp_read (socket, &token); /* token == GSGL_TCP_GUI_SPINF_VALUE */
		gsgl_tcp_read (socket, &token1);
		gsgl_tcp_read (socket, &token2);
		value = gsgl_convert_token_to_float (token1, token2);
		gsgl_tcp_guiitems_callback (id, &value, p);
		break;
	case GSGL_TCP_GUI_SPINI_VALUECHANGED:
		GSGL_DEBUG(fprintf (stderr, "client callback spini\n"));
		gsgl_tcp_read (socket, &id);
		gsgl_tcp_read (socket, &token); /* token == GSGL_TCP_GUI_SPINI_VALUE */
		gsgl_tcp_read (socket, &token1);
		gsgl_tcp_read (socket, &token2);
		vali = gsgl_convert_tokens_to_4byte (token1, token2) - 2147483647;
		gsgl_tcp_guiitems_callback (id, &vali, p);
		break;
	case GSGL_TCP_GUI_CHECK_TOGGLED:
		GSGL_DEBUG(fprintf (stderr, "client callback check\n"));
		gsgl_tcp_read (socket, &id);
		gsgl_tcp_read (socket, &token); /* token == GSGL_TCP_GUI_CHECK_ACTIVE */
		gsgl_tcp_read (socket, &token);
		active = token;
		gsgl_tcp_guiitems_callback (id, &active, p);
		break;
	case GSGL_TCP_GUI_EDIT_CHANGED:
		GSGL_DEBUG(fprintf (stderr, "client callback edit\n"));
		gsgl_tcp_read (socket, &id);
		gsgl_tcp_read (socket, &token); /* token == GSGL_TCP_GUI_EDIT_TEXT */
		gsgl_tcp_read (socket, &num);
		text = (char *) gsgl_malloc_atomic ((num+1)*sizeof(char));
		for (i=0; i<num; i++) {
			gsgl_tcp_read (socket, &token);
			text[i] = token;
		}
		text[num] = '\0';
		gsgl_tcp_guiitems_callback (id, text, p);
		break;
	case GSGL_TCP_GUI_ADD_TEXT:
		GSGL_DEBUG(fprintf (stderr, "client callback add text\n"));
		gsgl_tcp_read (socket, &num);
		text = (char *) gsgl_malloc_atomic ((num+1)*sizeof(char));
		for (i=0; i<num; i++) {
			gsgl_tcp_read (socket, &token);
			text[i] = token;
		}
		text[num] = '\0';
		
		gsgl_tcp_read (socket, &token1);
		gsgl_tcp_read (socket, &token2);
		x = gsgl_convert_token_to_float (token1, token2);
		
		gsgl_tcp_read (socket, &token1);
		gsgl_tcp_read (socket, &token2);
		y = gsgl_convert_token_to_float (token1, token2);

		gsgl_tcp_read (socket, &fnt);
		
		gsgl_tcp_read (socket, &token1);
		gsgl_tcp_read (socket, &token2);
		size = gsgl_convert_token_to_float (token1, token2);

		gsgl_tcp_read (socket, &token1);
		gsgl_tcp_read (socket, &token2);
		angle = gsgl_convert_token_to_float (token1, token2);

		gsgl_tcp_read (socket, &ijust);

		gsgl_tcp_begin_objects_after_pagee (p);
		gsaf_setfnt (fnt);
		gsaf_setchs ((float) size, (float) angle);
		gsaf_gtext (x, y, text, ijust);
		gsgl_tcp_end_objects_after_pagee (p);
		break;
	default:
		gsgl_warning ("gsgl_client_callback_template", "unknown token %d", token);
		break;
	}
}

static void server_default (unsigned int token, void *data1, gsglPage *p, READ_TOKEN)
{
        switch (token) {
        case GSGL_TCP_GUI_BEGIN_OBJECTS:
                gsgl_begin_guiitem (p);
                break;
        case GSGL_TCP_GUI_END_OBJECTS:
                gsgl_end_guiitem (p);
                return;
        case GSGL_TCP_GUI_CLEAR_LIST:
                gsaf_eras ();
                break;
	case GSGL_TCP_EXIT_FROM_PAGEE:
		gsaf_guiexitloop ();
		break;
        default:
                gsgl_warning ("server_default", "unknown token %d", token);
                break;
        }
}

/* server side callback function template */
void gsgl_server_callback_template (int socket, gsglPage *p,
				    void (*func_save_and_close)(void))
{
	unsigned int token;
	server_data *data;
	gsglPage * (*server_interpret)(server_data *, gsglPage *, void (*func)(unsigned int, void *, gsglPage *, READ_TOKEN));

	gsgl_tcp_read (socket, &token);
	
	switch (token) {
	case GSGL_TCP_REMADE_INFO:
		gsgl_tcp_send_list_info_to_client (p);
		break;
	case GSGL_TCP_GUI_SAVE_AND_CLOSE:
		func_save_and_close ();
		break;
	case GSGL_TCP_GUI_BEGIN_OBJECTS:
		server_interpret = gsgl_inq_servercallback ();
		if (!server_interpret) {
			gsgl_warning ("gsgl_server_callback_template", "server_interpret == NULL");
			return;
		}

		data = (server_data *) gsgl_malloc (sizeof (server_data));
		data->socket = socket;
		data->get_normal = GSGL_FALSE;
		data->set_normal_already = GSGL_FALSE;
		data->in_line = GSGL_FALSE;
		data->in_polygon = GSGL_FALSE;

		data->token = GSGL_TCP_GUI_BEGIN_OBJECTS;
		gsgl_begin_guiitem (p);
		while (data->token != GSGL_TCP_GUI_END_OBJECTS) {
			gsgl_tcp_read (socket, &token);
			data->token = token;
			p = server_interpret (data, p, server_default);
		}
		break;
	default:
		gsgl_warning ("gsgl_server_callback_template", "unknown token %d", token);
		break;
	}
}	
