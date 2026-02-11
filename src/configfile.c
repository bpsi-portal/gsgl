#include "gsaf.h"
#include "utils.h"
#include "configfile.h"
#include "tcpip.h"
#include <unistd.h>
#include <regex.h>
#ifdef USE_GTK
#include <glib.h>
#endif
#ifdef USE_QT
#include "qt_utils.h"
#endif

const char * gsgl_get_home_dir (void)
{
#ifdef USE_GTK
	return g_get_home_dir ();
#else
	return gsgl_qt_get_home_dir ();
#endif	
}

void gsgl_init_bg_color_once_in_program (gsglLibrary *lib)
{
	char *str;

	if (!gsgl_inq_i_am_a_server (lib)) {
		lib->Bgcolor = GSGL_BACK_COLOR_WHITE;
		str = getenv("GSAF_BGCOLOR");
		/* gsgl2ps sets GSAF_BGCOLOR */
		if (str) {
			if(!strcmp(str,"white")) lib->Bgcolor = GSGL_BACK_COLOR_PS;
		}
	} else {
		lib->Bgcolor = lib->Configcolor.bg;
	}
}

int gsgl_inq_bg_color (gsglLibrary *lib)
{
	return lib->Bgcolor;
}

char * gsgl_inq_ttfname (int i, gsglLibrary *lib)
{
	if (i <= -1 || i >= 41) return NULL;
	return lib->Ttf_name[i];
}

char * gsgl_inq_ttfname_server (int i, gsglLibrary *lib)
{
	if (i <= -1 || i >= 41) return NULL;
	return lib->Ttf_name_server[i];
}

void gsgl_set_ttfname (int i, const char *name, gsglLibrary *lib)
{
	if (i <= -1 || i >= 41) return;

	lib->Ttf_name[i] = gsgl_strdup (name);
}

gsglConfigColor * gsgl_inq_config_color (gsglLibrary *lib)
{
	return &lib->Configcolor;
}

char * gsgl_inq_print_command (gsglLibrary *lib)
{
	return lib->Command_print;
}

char * gsgl_inq_man_command (gsglLibrary *lib)
{
	return lib->Command_man;
}

char * gsgl_inq_eps_vec_command (gsglLibrary *lib)
{
	return lib->Command_eps_vec;
}

char * gsgl_inq_eps_ras_command (gsglLibrary *lib)
{
	return lib->Command_eps_ras;
}

char * gsgl_inq_png_command (gsglLibrary *lib)
{
	return lib->Command_png;
}

char * gsgl_inq_jpg_command (gsglLibrary *lib)
{
	return lib->Command_jpg;
}

void gsgl_set_print_command (const char *command, gsglLibrary *lib)
{
	lib->Command_print = gsgl_strdup (command);
}

void gsgl_set_man_command (const char *command, gsglLibrary *lib)
{
	lib->Command_man = gsgl_strdup (command);
}

void gsgl_set_eps_vec_command (const char *command, gsglLibrary *lib)
{
	lib->Command_eps_vec = gsgl_strdup (command);
}

void gsgl_set_eps_ras_command (const char *command, gsglLibrary *lib)
{
	lib->Command_eps_ras = gsgl_strdup (command);
}

void gsgl_set_png_command (const char *command, gsglLibrary *lib)
{
	lib->Command_png = gsgl_strdup (command);
}

void gsgl_set_jpg_command (const char *command, gsglLibrary *lib)
{
	lib->Command_jpg = gsgl_strdup (command);
}

int gsgl_inq_userdefined_winwidth (gsglLibrary *lib)
{
	return lib->Userdefined_windowwidth;
}

int gsgl_inq_userdefined_winheight (gsglLibrary *lib)
{
	return lib->Userdefined_windowheight;
}

int gsgl_inq_userdefined_corner_x (gsglLibrary *lib)
{
	return lib->Userdefined_corner_x;
}

int gsgl_inq_userdefined_corner_y (gsglLibrary *lib)
{
	return lib->Userdefined_corner_y;
}

void gsgl_set_userdefined_winwidth (int value, gsglLibrary *lib)
{
	lib->Userdefined_windowwidth = value;
}

void gsgl_set_userdefined_winheight (int value, gsglLibrary *lib)
{
	lib->Userdefined_windowheight = value;
}

void gsgl_set_userdefined_corner_x (int value, gsglLibrary *lib)
{
	lib->Userdefined_corner_x = value;
}

void gsgl_set_userdefined_corner_y (int value, gsglLibrary *lib)
{
	lib->Userdefined_corner_y = value;
}

void gsgl_config_write (gsglLibrary *lib)
{
	FILE *rc;
	char *dir;
	int len;

	len = strlen(gsgl_get_home_dir ());
	dir = (char *) gsgl_malloc_atomic ((len+1+8)*sizeof(char));
	sprintf (dir, "%s/.gsglrc", gsgl_get_home_dir ());

	if ((rc = fopen (dir, "w")) == NULL) {
		gsgl_warning ("gsgl_config_write", "Failed to open %s", dir);
		return;
	}

	if (lib->Configcolor.bg == GSGL_BACK_COLOR_BLACK) {
		fprintf (rc, "background=black\n");
	} else if (lib->Configcolor.bg == GSGL_BACK_COLOR_WHITE) {
		fprintf (rc, "background=white\n");
	} else {
		gsgl_warning ("gsgl_config_write", "Unsupported background color (%d)\n", lib->Configcolor.bg);
	}

	fprintf (rc, "select_box=(%f, %f, %f)\n", lib->Configcolor.box[0], lib->Configcolor.box[1], lib->Configcolor.box[2]);
	fprintf (rc, "select_frame=(%f, %f, %f)\n", lib->Configcolor.frame[0], lib->Configcolor.frame[1], lib->Configcolor.frame[2]);
	
	fprintf (rc, "window_width=%d\n", lib->Userdefined_windowwidth);
	fprintf (rc, "window_height=%d\n", lib->Userdefined_windowheight);
	fprintf (rc, "window_corner_x=%d\n", lib->Userdefined_corner_x);
	fprintf (rc, "window_corner_y=%d\n", lib->Userdefined_corner_y);

	fprintf (rc, "print=%s\n", lib->Command_print);
	fprintf (rc, "man=%s\n", lib->Command_man);
	fprintf (rc, "eps_vec=%s\n", lib->Command_eps_vec);
	fprintf (rc, "eps_ras=%s\n", lib->Command_eps_ras);
	fprintf (rc, "png=%s\n", lib->Command_png);
	fprintf (rc, "jpg=%s\n", lib->Command_jpg);
	
	fprintf (rc, "ttf_alphabetdefault=%s\n", lib->Ttf_name[0]);
	fprintf (rc, "ttf_mathdefault=%s\n", lib->Ttf_name[1]);
	fprintf (rc, "ttf_timesroman=%s\n", lib->Ttf_name[2]);
	fprintf (rc, "ttf_timesitalic=%s\n", lib->Ttf_name[3]);
	fprintf (rc, "ttf_timesbold=%s\n", lib->Ttf_name[4]);
	fprintf (rc, "ttf_timesbolditalic=%s\n", lib->Ttf_name[5]);
	fprintf (rc, "ttf_arialroman=%s\n", lib->Ttf_name[6]);
	fprintf (rc, "ttf_arialitalic=%s\n", lib->Ttf_name[7]);
	fprintf (rc, "ttf_arialbold=%s\n", lib->Ttf_name[8]);
	fprintf (rc, "ttf_arialbolditalic=%s\n", lib->Ttf_name[9]);
	fprintf (rc, "ttf_courierroman=%s\n", lib->Ttf_name[10]);
	fprintf (rc, "ttf_courieritalic=%s\n", lib->Ttf_name[11]);
	fprintf (rc, "ttf_courierbold=%s\n", lib->Ttf_name[12]);
	fprintf (rc, "ttf_courierbolditalic=%s\n", lib->Ttf_name[13]);

	fprintf (rc, "ttf_greekdefault=%s\n", lib->Ttf_name[14]);
	fprintf (rc, "ttf_greekroman=%s\n", lib->Ttf_name[15]);
	fprintf (rc, "ttf_greekitalic=%s\n", lib->Ttf_name[16]);
	fprintf (rc, "ttf_greekbold=%s\n", lib->Ttf_name[17]);
	fprintf (rc, "ttf_greekbolditalic=%s\n", lib->Ttf_name[18]);

	fprintf (rc, "ttf_japanesedefault=%s\n", lib->Ttf_name[19]);
	fprintf (rc, "ttf_japanesemincho=%s\n", lib->Ttf_name[20]);
	fprintf (rc, "ttf_japanesegothic=%s\n", lib->Ttf_name[21]);

	fprintf (rc, "ttf_cmr10=%s\n", lib->Ttf_name[22]);
	fprintf (rc, "ttf_cmti10=%s\n", lib->Ttf_name[23]);
	fprintf (rc, "ttf_cmtt10=%s\n", lib->Ttf_name[24]);
	fprintf (rc, "ttf_cmmi10=%s\n", lib->Ttf_name[25]);
	fprintf (rc, "ttf_cmsy10=%s\n", lib->Ttf_name[26]);
	fprintf (rc, "ttf_cmex10=%s\n", lib->Ttf_name[27]);

	fprintf (rc, "type1_Times_Roman=%s\n", lib->Ttf_name[28]);
	fprintf (rc, "type1_Times_Italic=%s\n", lib->Ttf_name[29]);
	fprintf (rc, "type1_Times_Bold=%s\n", lib->Ttf_name[30]);
	fprintf (rc, "type1_Times_BoldItalic=%s\n", lib->Ttf_name[31]);
	fprintf (rc, "type1_Helvetica=%s\n", lib->Ttf_name[32]);
	fprintf (rc, "type1_Helvetica_Oblique=%s\n", lib->Ttf_name[33]);
	fprintf (rc, "type1_Helvetica_Bold=%s\n", lib->Ttf_name[34]);
	fprintf (rc, "type1_Helvetica_BoldOblique=%s\n", lib->Ttf_name[35]);
	fprintf (rc, "type1_Courier=%s\n", lib->Ttf_name[36]);
	fprintf (rc, "type1_Courier_Oblique=%s\n", lib->Ttf_name[37]);
	fprintf (rc, "type1_Courier_Bold=%s\n", lib->Ttf_name[38]);
	fprintf (rc, "type1_Courier_BoldOblique=%s\n", lib->Ttf_name[39]);
	fprintf (rc, "type1_Symbol=%s\n", lib->Ttf_name[40]);
                
	fclose (rc);
}

static char * config_read_string (char *str, char *regex, char *out)
{
	regex_t preg;
	int len;
	regmatch_t pmatch[4];
	char *out2;

	regcomp (&preg, regex, REG_EXTENDED);
	if (regexec (&preg, str, 1, pmatch, 0) != REG_NOMATCH) {
		out2 = gsgl_strdup (&str[pmatch[0].rm_eo]);
		len = strlen (out2);
		if (out2[len-1] == '\n') out2[len-1] = '\0';
		regfree (&preg);
		return out2;
	}
	regfree (&preg);
	return out;
}

static void gsgl_config_fix_missing_type1 (char **ttf_names)
{
	static const char *defaults[] = {
		TYPE1_Times_Roman,
		TYPE1_Times_Italic,
		TYPE1_Times_Bold,
		TYPE1_Times_BoldItalic,
		TYPE1_Helvetica,
		TYPE1_Helvetica_Oblique,
		TYPE1_Helvetica_Bold,
		TYPE1_Helvetica_BoldOblique,
		TYPE1_Courier,
		TYPE1_Courier_Oblique,
		TYPE1_Courier_Bold,
		TYPE1_Courier_BoldOblique,
		TYPE1_Symbol
	};
	int i;

	for (i = 0; i < 13; i++) {
		int idx = 28 + i;
		if (!ttf_names[idx] || access (ttf_names[idx], R_OK) != 0) {
			if (ttf_names[idx])
				gsgl_free (ttf_names[idx]);
			ttf_names[idx] = gsgl_strdup (defaults[i]);
		}
	}
}

static void gsgl_config_fix_missing_ttf (char **ttf_names)
{
	static const char *defaults[] = {
		TTF_ALPHABET,
		TTF_MATH,
		TTF_TIMESROMAN,
		TTF_TIMESITALIC,
		TTF_TIMESBOLD,
		TTF_TIMESBOLDITALIC,
		TTF_ARIALROMAN,
		TTF_ARIALITALIC,
		TTF_ARIALBOLD,
		TTF_ARIALBOLDITALIC,
		TTF_COURIERROMAN,
		TTF_COURIERITALIC,
		TTF_COURIERBOLD,
		TTF_COURIERBOLDITALIC,
		TTF_GREEK,
		TTF_GREEKROMAN,
		TTF_GREEKITALIC,
		TTF_GREEKBOLD,
		TTF_GREEKBOLDITALIC,
		TTF_JAPANESE,
		TTF_JAPANESEMINCHO,
		TTF_JAPANESEGOTHIC,
		TTF_CMR10,
		TTF_CMTI10,
		TTF_CMTT10,
		TTF_CMMI10,
		TTF_CMSY10,
		TTF_CMEX10
	};
	int i;

	for (i = 0; i < 28; i++) {
		if (!ttf_names[i] || access (ttf_names[i], R_OK) != 0) {
			if (ttf_names[i])
				gsgl_free (ttf_names[i]);
			ttf_names[i] = gsgl_strdup (defaults[i]);
		}
	}
}

void gsgl_config_read (gsglLibrary *lib)
{
	int i;
	FILE *rc;
	char *dir;
	char str[1000], *ptr;
	regex_t preg;
	char *regex_bg = "background=([a-z]+)";
	char *regex_box = "select_box=.([.0-9]+),[ \t]([.0-9]+),[ \t]([.0-9]+).";
	char *regex_frame = "select_frame=.([.0-9]+),[ \t]([.0-9]+),[ \t]([.0-9]+).";
	char *regex_print = "^print=";
	char *regex_man = "^man=";
	char *regex_eps_vec = "^eps_vec=";
	char *regex_eps_ras = "^eps_ras=";
	char *regex_png = "^png=";
	char *regex_jpg = "^jpg=";
	char *regex_windowwidth = "window_width=";
	char *regex_windowheight = "window_height=";
	char *regex_windowcornerx = "window_corner_x=";
	char *regex_windowcornery = "window_corner_y=";
	char *regex_ttf[41] = {"ttf_alphabetdefault=", "ttf_mathdefault=",
			       "ttf_timesroman=", "ttf_timesitalic=", "ttf_timesbold=",
			       "ttf_timesbolditalic=", "ttf_arialroman=", "ttf_arialitalic=",
			       "ttf_arialbold=", "ttf_arialbolditalic=", "ttf_courierroman=",
			       "ttf_courieritalic=", "ttf_courierbold=", 
			       "ttf_courierbolditalic=", "ttf_greekdefault=", "ttf_greekroman=",
			       "ttf_greekitalic=", "ttf_greekbold=", "ttf_greekbolditalic=",
			       "ttf_japanesedefault=", "ttf_japanesemincho=", 
			       "ttf_japanesegothic=",
			       "ttf_cmr10=", "ttf_cmti10=", "ttf_cmtt10=",
			       "ttf_cmmi10=", "ttf_cmsy10=", "ttf_cmex10=",
			       "type1_Times_Roman=", 
			       "type1_Times_Italic=",
			       "type1_Times_Bold=",
			       "type1_Times_BoldItalic=",
			       "type1_Helvetica=",
			       "type1_Helvetica_Oblique=",
			       "type1_Helvetica_Bold=",
			       "type1_Helvetica_BoldOblique=",
			       "type1_Courier=",
			       "type1_Courier_Oblique=",
			       "type1_Courier_Bold=",
			       "type1_Courier_BoldOblique=",
			       "type1_Symbol="
	};
	regmatch_t pmatch[4];
	char *tmp;
	int len;

	lib->Configcolor.bg = GSGL_BACK_COLOR_WHITE;
	lib->Configcolor.box[0] = DEFAULT_BOX_R;
	lib->Configcolor.box[1] = DEFAULT_BOX_G;
	lib->Configcolor.box[2] = DEFAULT_BOX_B;
	lib->Configcolor.frame[0] = DEFAULT_FRAME_R;
	lib->Configcolor.frame[1] = DEFAULT_FRAME_G;
	lib->Configcolor.frame[2] = DEFAULT_FRAME_B;
	lib->Command_print = gsgl_strdup (COMMAND_PRINT);
	lib->Command_man = gsgl_strdup (COMMAND_MAN);
	lib->Command_eps_vec = gsgl_strdup (COMMAND_EPS_VEC);
	lib->Command_eps_ras = gsgl_strdup (COMMAND_EPS_RAS);
	lib->Command_png = gsgl_strdup (COMMAND_PNG);
	lib->Command_jpg = gsgl_strdup (COMMAND_JPG); 
	lib->Userdefined_windowwidth = GSGLBASIC_WINDOWWIDTH;
	lib->Userdefined_windowheight = GSGLBASIC_WINDOWHEIGHT;
	lib->Userdefined_corner_x = 0;
	lib->Userdefined_corner_y = 0;
	lib->Ttf_name[0] = gsgl_strdup (TTF_ALPHABET);
	lib->Ttf_name[1] = gsgl_strdup (TTF_MATH);
	lib->Ttf_name[2] = gsgl_strdup (TTF_TIMESROMAN);
	lib->Ttf_name[3] = gsgl_strdup (TTF_TIMESITALIC);
	lib->Ttf_name[4] = gsgl_strdup (TTF_TIMESBOLD);
	lib->Ttf_name[5] = gsgl_strdup (TTF_TIMESBOLDITALIC);
	lib->Ttf_name[6] = gsgl_strdup (TTF_ARIALROMAN);
	lib->Ttf_name[7] = gsgl_strdup (TTF_ARIALITALIC);
	lib->Ttf_name[8] = gsgl_strdup (TTF_ARIALBOLD);
	lib->Ttf_name[9] = gsgl_strdup (TTF_ARIALBOLDITALIC);
	lib->Ttf_name[10] = gsgl_strdup (TTF_COURIERROMAN);
	lib->Ttf_name[11] = gsgl_strdup (TTF_COURIERITALIC);
	lib->Ttf_name[12] = gsgl_strdup (TTF_COURIERBOLD);
	lib->Ttf_name[13] = gsgl_strdup (TTF_COURIERBOLDITALIC);
	lib->Ttf_name[14] = gsgl_strdup (TTF_GREEK);
	lib->Ttf_name[15] = gsgl_strdup (TTF_GREEKROMAN);
	lib->Ttf_name[16] = gsgl_strdup (TTF_GREEKITALIC);
	lib->Ttf_name[17] = gsgl_strdup (TTF_GREEKBOLD);
	lib->Ttf_name[18] = gsgl_strdup (TTF_GREEKBOLDITALIC);
	lib->Ttf_name[19] = gsgl_strdup (TTF_JAPANESE);
	lib->Ttf_name[20] = gsgl_strdup (TTF_JAPANESEMINCHO);
	lib->Ttf_name[21] = gsgl_strdup (TTF_JAPANESEGOTHIC);
	lib->Ttf_name[22] = gsgl_strdup (TTF_CMR10);
	lib->Ttf_name[23] = gsgl_strdup (TTF_CMTI10);
	lib->Ttf_name[24] = gsgl_strdup (TTF_CMTT10);
	lib->Ttf_name[25] = gsgl_strdup (TTF_CMMI10);
	lib->Ttf_name[26] = gsgl_strdup (TTF_CMSY10);
	lib->Ttf_name[27] = gsgl_strdup (TTF_CMEX10);
	lib->Ttf_name[28] = gsgl_strdup (TYPE1_Times_Roman);
	lib->Ttf_name[29] = gsgl_strdup (TYPE1_Times_Italic);
	lib->Ttf_name[30] = gsgl_strdup (TYPE1_Times_Bold);
	lib->Ttf_name[31] = gsgl_strdup (TYPE1_Times_BoldItalic);
	lib->Ttf_name[32] = gsgl_strdup (TYPE1_Helvetica);
	lib->Ttf_name[33] = gsgl_strdup (TYPE1_Helvetica_Oblique);
	lib->Ttf_name[34] = gsgl_strdup (TYPE1_Helvetica_Bold);
	lib->Ttf_name[35] = gsgl_strdup (TYPE1_Helvetica_BoldOblique);
	lib->Ttf_name[36] = gsgl_strdup (TYPE1_Courier);
	lib->Ttf_name[37] = gsgl_strdup (TYPE1_Courier_Oblique);
	lib->Ttf_name[38] = gsgl_strdup (TYPE1_Courier_Bold);
	lib->Ttf_name[39] = gsgl_strdup (TYPE1_Courier_BoldOblique);
	lib->Ttf_name[40] = gsgl_strdup (TYPE1_Symbol);

	len = strlen (gsgl_get_home_dir ());
	dir = (char *) gsgl_malloc_atomic ((len+1+8)*sizeof(char));
	sprintf (dir, "%s/.gsglrc", gsgl_get_home_dir ());
	
	if ((rc = fopen (dir, "r")) != NULL) {
		while (1) {
			ptr = fgets (str, sizeof(str), rc);
			if (ptr == NULL) break;
			regcomp (&preg, regex_bg, REG_EXTENDED);
			if (regexec (&preg, str, 2, pmatch, 0) != REG_NOMATCH) {
				tmp = gsgl_strndup (&str[pmatch[1].rm_so],
						    pmatch[1].rm_eo-pmatch[1].rm_so);
				if (lib->Bgcolor == GSGL_BACK_COLOR_WHITE && !strcmp(tmp,"black")) {
					lib->Configcolor.bg = GSGL_BACK_COLOR_BLACK;
					lib->Bgcolor = GSGL_BACK_COLOR_BLACK;
				} else if (lib->Bgcolor==GSGL_BACK_COLOR_PS && !strcmp(tmp,"black")) {
					lib->Configcolor.bg = GSGL_BACK_COLOR_BLACK;
					lib->Bgcolor = GSGL_BACK_COLOR_WHITE;
				} else if (lib->Bgcolor==GSGL_BACK_COLOR_PS && !strcmp(tmp,"white")) {
					lib->Configcolor.bg = GSGL_BACK_COLOR_WHITE;
					lib->Bgcolor = GSGL_BACK_COLOR_WHITE;
				}
			}
			regfree (&preg);
			regcomp (&preg, regex_box, REG_EXTENDED);
			if (regexec (&preg, str, 4, pmatch, 0) != REG_NOMATCH) {
				tmp = gsgl_strndup (&str[pmatch[1].rm_so],
						    pmatch[1].rm_eo-pmatch[1].rm_so);
				lib->Configcolor.box[0] = atof(tmp);
				tmp = gsgl_strndup (&str[pmatch[2].rm_so],
						    pmatch[2].rm_eo-pmatch[2].rm_so);
				lib->Configcolor.box[1] = atof(tmp);
				tmp = gsgl_strndup (&str[pmatch[3].rm_so],
						    pmatch[3].rm_eo-pmatch[3].rm_so);
				lib->Configcolor.box[2] = atof(tmp);
			}
			regfree (&preg);
			regcomp (&preg, regex_frame, REG_EXTENDED);
			if (regexec (&preg, str, 4, pmatch, 0) != REG_NOMATCH) {
				tmp = gsgl_strndup (&str[pmatch[1].rm_so],
						    pmatch[1].rm_eo-pmatch[1].rm_so);
				lib->Configcolor.frame[0] = atof(tmp);
				tmp = gsgl_strndup (&str[pmatch[2].rm_so],
						    pmatch[2].rm_eo-pmatch[2].rm_so);
				lib->Configcolor.frame[1] = atof(tmp);
				tmp = gsgl_strndup (&str[pmatch[3].rm_so],
						    pmatch[3].rm_eo-pmatch[3].rm_so);
				lib->Configcolor.frame[2] = atof(tmp);
			}
			regfree (&preg);
			lib->Command_print = config_read_string (str, regex_print, lib->Command_print);
			lib->Command_man = config_read_string (str, regex_man, lib->Command_man);
			lib->Command_eps_vec = config_read_string (str, regex_eps_vec, lib->Command_eps_vec);
			lib->Command_eps_ras = config_read_string (str, regex_eps_ras, lib->Command_eps_ras); 
			lib->Command_png = config_read_string (str, regex_png, lib->Command_png); 
			lib->Command_jpg = config_read_string (str, regex_jpg, lib->Command_jpg); 

			regcomp (&preg, regex_windowwidth, REG_EXTENDED);
			if (regexec (&preg, str, 1, pmatch, 0) != REG_NOMATCH) {
				tmp = gsgl_strdup (&str[pmatch[0].rm_eo]);
				len = strlen (tmp);
				if (tmp[len-1] == '\n')
					tmp[len-1] = '\0';
				lib->Userdefined_windowwidth = atoi (tmp);
			}
			regfree (&preg);
			regcomp (&preg, regex_windowheight, REG_EXTENDED);
			if (regexec (&preg, str, 1, pmatch, 0) != REG_NOMATCH) {
				tmp = gsgl_strdup (&str[pmatch[0].rm_eo]);
				len = strlen (tmp);
				if (tmp[len-1] == '\n')
					tmp[len-1] = '\0';
				lib->Userdefined_windowheight = atoi (tmp);
			}
			regfree (&preg);
			regcomp (&preg, regex_windowcornerx, REG_EXTENDED);
			if (regexec (&preg, str, 1, pmatch, 0) != REG_NOMATCH) {
				tmp = gsgl_strdup (&str[pmatch[0].rm_eo]);
				len = strlen (tmp);
				if (tmp[len-1] == '\n')
					tmp[len-1] = '\0';
				lib->Userdefined_corner_x = atoi (tmp);
			}
			regfree (&preg);
			regcomp (&preg, regex_windowcornery, REG_EXTENDED);
			if (regexec (&preg, str, 1, pmatch, 0) != REG_NOMATCH) {
				tmp = gsgl_strdup (&str[pmatch[0].rm_eo]);
				len = strlen (tmp);
				if (tmp[len-1] == '\n')
					tmp[len-1] = '\0';
				lib->Userdefined_corner_y = atoi (tmp);
			}
			regfree (&preg);
			for (i=0; i<41; i++)
				lib->Ttf_name[i] = config_read_string (str, regex_ttf[i], lib->Ttf_name[i]);
		}
		fclose (rc);
	}
	gsgl_config_fix_missing_ttf (lib->Ttf_name);
	gsgl_config_fix_missing_type1 (lib->Ttf_name);
}

void gsgl_config_read_server_ttf (gsglLibrary *lib)
{
	int i;
	FILE *rc;
	char *dir;
	char str[1000], *ptr;
	char *regex_ttf[41] = {"ttf_alphabetdefault=", "ttf_mathdefault=",
			       "ttf_timesroman=", "ttf_timesitalic=", "ttf_timesbold=",
			       "ttf_timesbolditalic=", "ttf_arialroman=", "ttf_arialitalic=",
			       "ttf_arialbold=", "ttf_arialbolditalic=", "ttf_courierroman=",
			       "ttf_courieritalic=", "ttf_courierbold=", 
			       "ttf_courierbolditalic=", "ttf_greekdefault=", "ttf_greekroman=",
			       "ttf_greekitalic=", "ttf_greekbold=", "ttf_greekbolditalic=",
			       "ttf_japanesedefault=", "ttf_japanesemincho=", 
			       "ttf_japanesegothic=",
			       "ttf_cmr10=", "ttf_cmti10=", "ttf_cmtt10=",
			       "ttf_cmmi10=", "ttf_cmsy10=", "ttf_cmex10=",
			       "type1_Times_Roman=", 
			       "type1_Times_Italic=",
			       "type1_Times_Bold=",
			       "type1_Times_BoldItalic=",
			       "type1_Helvetica=",
			       "type1_Helvetica_Oblique=",
			       "type1_Helvetica_Bold=",
			       "type1_Helvetica_BoldOblique=",
			       "type1_Courier=",
			       "type1_Courier_Oblique=",
			       "type1_Courier_Bold=",
			       "type1_Courier_BoldOblique=",
			       "type1_Symbol="
	};
	int len;

	lib->Ttf_name_server[0] = gsgl_strdup (TTF_ALPHABET);
	lib->Ttf_name_server[1] = gsgl_strdup (TTF_MATH);
	lib->Ttf_name_server[2] = gsgl_strdup (TTF_TIMESROMAN);
	lib->Ttf_name_server[3] = gsgl_strdup (TTF_TIMESITALIC);
	lib->Ttf_name_server[4] = gsgl_strdup (TTF_TIMESBOLD);
	lib->Ttf_name_server[5] = gsgl_strdup (TTF_TIMESBOLDITALIC);
	lib->Ttf_name_server[6] = gsgl_strdup (TTF_ARIALROMAN);
	lib->Ttf_name_server[7] = gsgl_strdup (TTF_ARIALITALIC);
	lib->Ttf_name_server[8] = gsgl_strdup (TTF_ARIALBOLD);
	lib->Ttf_name_server[9] = gsgl_strdup (TTF_ARIALBOLDITALIC);
	lib->Ttf_name_server[10] = gsgl_strdup (TTF_COURIERROMAN);
	lib->Ttf_name_server[11] = gsgl_strdup (TTF_COURIERITALIC);
	lib->Ttf_name_server[12] = gsgl_strdup (TTF_COURIERBOLD);
	lib->Ttf_name_server[13] = gsgl_strdup (TTF_COURIERBOLDITALIC);
	lib->Ttf_name_server[14] = gsgl_strdup (TTF_GREEK);
	lib->Ttf_name_server[15] = gsgl_strdup (TTF_GREEKROMAN);
	lib->Ttf_name_server[16] = gsgl_strdup (TTF_GREEKITALIC);
	lib->Ttf_name_server[17] = gsgl_strdup (TTF_GREEKBOLD);
	lib->Ttf_name_server[18] = gsgl_strdup (TTF_GREEKBOLDITALIC);
	lib->Ttf_name_server[19] = gsgl_strdup (TTF_JAPANESE);
	lib->Ttf_name_server[20] = gsgl_strdup (TTF_JAPANESEMINCHO);
	lib->Ttf_name_server[21] = gsgl_strdup (TTF_JAPANESEGOTHIC);
	lib->Ttf_name_server[22] = gsgl_strdup (TTF_CMR10);
	lib->Ttf_name_server[23] = gsgl_strdup (TTF_CMTI10);
	lib->Ttf_name_server[24] = gsgl_strdup (TTF_CMTT10);
	lib->Ttf_name_server[25] = gsgl_strdup (TTF_CMMI10);
	lib->Ttf_name_server[26] = gsgl_strdup (TTF_CMSY10);
	lib->Ttf_name_server[27] = gsgl_strdup (TTF_CMEX10);
	lib->Ttf_name_server[28] = gsgl_strdup (TYPE1_Times_Roman);
	lib->Ttf_name_server[29] = gsgl_strdup (TYPE1_Times_Italic);
	lib->Ttf_name_server[30] = gsgl_strdup (TYPE1_Times_Bold);
	lib->Ttf_name_server[31] = gsgl_strdup (TYPE1_Times_BoldItalic);
	lib->Ttf_name_server[32] = gsgl_strdup (TYPE1_Helvetica);
	lib->Ttf_name_server[33] = gsgl_strdup (TYPE1_Helvetica_Oblique);
	lib->Ttf_name_server[34] = gsgl_strdup (TYPE1_Helvetica_Bold);
	lib->Ttf_name_server[35] = gsgl_strdup (TYPE1_Helvetica_BoldOblique);
	lib->Ttf_name_server[36] = gsgl_strdup (TYPE1_Courier);
	lib->Ttf_name_server[37] = gsgl_strdup (TYPE1_Courier_Oblique);
	lib->Ttf_name_server[38] = gsgl_strdup (TYPE1_Courier_Bold);
	lib->Ttf_name_server[39] = gsgl_strdup (TYPE1_Courier_BoldOblique);
	lib->Ttf_name_server[40] = gsgl_strdup (TYPE1_Symbol);

	len = strlen (gsgl_get_home_dir ());
	dir = (char *) gsgl_malloc_atomic ((len+1+8)*sizeof(char));
	sprintf (dir, "%s/.gsglrc", gsgl_get_home_dir ());
	
	if ((rc = fopen (dir, "r")) != NULL) {
		while (1) {
			ptr = fgets (str, sizeof(str), rc);
			if (ptr == NULL) break;
			for (i=0; i<41; i++)
				lib->Ttf_name_server[i] = config_read_string (str, regex_ttf[i], lib->Ttf_name_server[i]);
		}
		fclose (rc);
	}
	gsgl_config_fix_missing_ttf (lib->Ttf_name_server);
	gsgl_config_fix_missing_type1 (lib->Ttf_name_server);
}

void gsgl_config_set_server (gsglLibrary *lib)
{
	char *str;

	str = getenv("GSGL_BG");
	if (str) lib->Bgcolor = atoi (str);

	str = getenv("GSGL_BOX_R");
	if (str) lib->Configcolor.box[0] = gsgl_convert_token_to_0to1 (atoi (str));

	str = getenv("GSGL_BOX_G");
	if (str) lib->Configcolor.box[1] = gsgl_convert_token_to_0to1 (atoi (str));

	str = getenv("GSGL_BOX_B");
	if (str) lib->Configcolor.box[2] = gsgl_convert_token_to_0to1 (atoi (str));

	str = getenv("GSGL_FRAME_R");
	if (str) lib->Configcolor.frame[0] = gsgl_convert_token_to_0to1 (atoi (str));

	str = getenv("GSGL_FRAME_G");
	if (str) lib->Configcolor.frame[1] = gsgl_convert_token_to_0to1 (atoi (str));

	str = getenv("GSGL_FRAME_B");
	if (str) lib->Configcolor.frame[2] = gsgl_convert_token_to_0to1 (atoi (str));

	str = getenv("GSGL_WINDOW_W");
	if (str) lib->Userdefined_windowwidth = atoi (str);

	str = getenv("GSGL_WINDOW_H");
	if (str) lib->Userdefined_windowheight = atoi (str);

	str = getenv("GSGL_WINDOW_X");
	if (str) lib->Userdefined_corner_x = atoi (str);

	str = getenv("GSGL_WINDOW_Y");
	if (str) lib->Userdefined_corner_y = atoi (str);

	str = getenv("GSGL_SERVER_SOCKET");
	if (str) lib->Server_socket = atoi (str);
}

/*static char * gsgl_config_read_string (int socket)
  {
  int len;
  int i;
  unsigned char *str = NULL;
  
  gsgl_tcp_read (socket, &len);
  str = (unsigned char *) g_malloc (len * sizeof (unsigned char));
  for (i=0; i<len; i++)
  gsgl_tcp_read_1byte (socket, (unsigned char *) &str[i]);
  str[len] = '\0';
  
  return str;
  }*/

/*static void gsgl_config_send_string (char *str)
  {
  int len;
  int i;
  
  len = strlen (str);
  gsgl_tcp_push (len);
  
  for (i=0; i<len; i++)
  gsgl_tcp_push_1byte (str[i]);
  }*/

void gsgl_config_send_to_server (int c1, int id, gsglLibrary *lib)
{
	gsgl_tcp_write (lib->Client_Socket, c1);
	gsgl_tcp_write (lib->Client_Socket, (unsigned int) gsgl_convert_4byteint_to_1st2byte (id));
	gsgl_tcp_write (lib->Client_Socket, (unsigned int) gsgl_convert_4byteint_to_2nd2byte (id));
	gsgl_tcp_write (lib->Client_Socket, lib->Configcolor.bg);
	gsgl_tcp_write (lib->Client_Socket, gsgl_convert_0to1_to_2byte (lib->Configcolor.box[0]));
	gsgl_tcp_write (lib->Client_Socket, gsgl_convert_0to1_to_2byte (lib->Configcolor.box[1]));
	gsgl_tcp_write (lib->Client_Socket, gsgl_convert_0to1_to_2byte (lib->Configcolor.box[2]));
	gsgl_tcp_write (lib->Client_Socket, gsgl_convert_0to1_to_2byte (lib->Configcolor.frame[0]));
	gsgl_tcp_write (lib->Client_Socket, gsgl_convert_0to1_to_2byte (lib->Configcolor.frame[1]));
	gsgl_tcp_write (lib->Client_Socket, gsgl_convert_0to1_to_2byte (lib->Configcolor.frame[2]));

	gsgl_tcp_write (lib->Client_Socket, lib->Userdefined_windowwidth);
	gsgl_tcp_write (lib->Client_Socket, lib->Userdefined_windowheight);
	gsgl_tcp_write (lib->Client_Socket, lib->Userdefined_corner_x);
	gsgl_tcp_write (lib->Client_Socket, lib->Userdefined_corner_y);
}
