#ifndef __configfile_h__
#define __configfile_h__

#ifdef __cplusplus
extern "C"{
#endif

const char * gsgl_get_home_dir (void);
void gsgl_config_write (gsglLibrary *);
void gsgl_config_read (gsglLibrary *);
void gsgl_config_read_server_ttf (gsglLibrary *lib);
void gsgl_config_send_to_server (int c1, int id, gsglLibrary *);
char * gsgl_inq_print_command (gsglLibrary *);
char * gsgl_inq_man_command (gsglLibrary *);
char * gsgl_inq_eps_vec_command (gsglLibrary *);
char * gsgl_inq_eps_ras_command (gsglLibrary *);
char * gsgl_inq_png_command (gsglLibrary *);
char * gsgl_inq_jpg_command (gsglLibrary *);
int gsgl_inq_userdefined_winwidth (gsglLibrary *);
int gsgl_inq_userdefined_winheight (gsglLibrary *);
int gsgl_inq_userdefined_corner_x (gsglLibrary *);
int gsgl_inq_userdefined_corner_y (gsglLibrary *);
gsglConfigColor * gsgl_inq_config_color (gsglLibrary *);
void gsgl_set_print_command (const char *command, gsglLibrary *);
void gsgl_set_man_command (const char *command, gsglLibrary *);
void gsgl_set_eps_vec_command (const char *command, gsglLibrary *);
void gsgl_set_eps_ras_command (const char *command, gsglLibrary *);
void gsgl_set_png_command (const char *command, gsglLibrary *);
void gsgl_set_jpg_command (const char *command, gsglLibrary *);
void gsgl_set_userdefined_corner_y (int value, gsglLibrary *);
void gsgl_set_userdefined_winwidth (int value, gsglLibrary *);
void gsgl_set_userdefined_winheight (int value, gsglLibrary *);
void gsgl_set_userdefined_corner_x (int value, gsglLibrary *);
void gsgl_set_userdefined_corner_y (int value, gsglLibrary *);
char * gsgl_inq_ttfname (int i, gsglLibrary *);
char * gsgl_inq_ttfname_server (int i, gsglLibrary *);
void gsgl_set_ttfname (int i, const char *name, gsglLibrary *);
void gsgl_init_bg_color_once_in_program (gsglLibrary *);
int gsgl_inq_bg_color (gsglLibrary *);
void gsgl_config_set_server (gsglLibrary *lib);

#ifdef __cplusplus
}
#endif

#endif
