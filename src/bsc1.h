#ifndef __bsc1_h_
#define __bsc1_h_

#include "bsc3d.h"

struct _gsglstatus {
	int           indexcolor; /* -1 if setrgb is called */
	gsglrgb       rgb; /* rgb.r < 0 if indexcolor is called */
	int           linewidth_index; /* -1 if setlnw is called */
	float         linewidth_cm; /* < 0 if linewidth_index is called */
        float         modelview_matrix [16];
        float         projection_matrix [16];
        int           clip_plane[6];
        int           clip_plane_tmp;
        float         clip_eq[6][4];
        float         clip_proj [6][16];
        float         clip_model[6][16];
	gsgl3DInfo    threeDinfo;
	int           coord_mode;
	gsglboolean   lighting_enabled_by_user;
	gsglboolean   lighting_enabled_temporary;
	gsglboolean   antialiasing;
};
/* coordinate mode is available by gsgl_coordinate_inq (int *); */
/* 3d-related attribues are in Gsgl3DInfo1 */

struct _gsglBase
{
	gsglrect Setvew_physical;
	gsglrect Setvew_logical;
	gsglrect Clip;
	int Imk;
	float Xmkx,Xmky,Ymkx,Ymky;
	float Smrks[4];
	float Xchx,Xchy,Ychx,Ychy,Xchsp,Ychsp;
	float Tfxx,Tfxy,Tfyx,Tfyy,Tflx,Tfly;
	float Tfsp,Tfspx,Tfspy;
	int Fntreal;
	int LineType, LineWidth, LineColor;
	gsglvec2f Position;    /* store current position in world coord */
	gsglvec2f Position2d;  /* store current position in 2d mode */
	gsglvec3f Position3d;  /* store current position in 3d mode */
	gsglstatus Allstatus;
	gsglboolean Lighting_enabled_by_user;
	gsglboolean Lighting_enabled_temporary;
};

#ifdef __cplusplus
extern "C"{
#endif

void gsgl_keep_header (void);
/*void gserror_pages (int *ierror);*/
void gsgl_close_message (gsglLibrary *);
void gsaf_f77_gudstr (char *kin, int nin, char *kout, int *nout);
gsglboolean gsgl_inq_savefile_opened (void);
gsglboolean gsgl_inq_save_to_file (void);
gsglboolean gsgl_inq_page_is_open (void);
gsglboolean gsgl_inq_gsopen_is_called (void);
void gsgl_set_savefile_is_opened (void);
void gsgl_set_savefile_is_closed (void);
void gsgl_set_savetofile (gsglboolean);
gsglboolean gsgl_inq_ask_in_save (void);
void gsgl_inq_fntreal (int* fntr, gsglPage *p);
void gsgl_inq_font_scale_info (float *tfxx, float *tfxy, float *tfyx, float *tfyy,
			       float *tflx, float *tfly, float *tfsp, float *tfspx,
			       float *tfspy, gsglPage *p);
void gs_sftext (float x, float y, unsigned int *iasc, int nchar, int idim, gsglPage *);
void gs_sfstch (float chh, float chw, float chsp, float angl, float tilt, gsglPage *);
void setlnw_text (gsglPage *);
void (*gsaf_inq_quit_callback(void))(void);
int gsaf_inq_rank (void);
gsglPage * gsgl_inq_page (void);
gsglLibrary * gsgl_inq_lib (void);

#ifdef __cplusplus
}
#endif

#endif
