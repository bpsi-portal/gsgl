#ifndef __callback_h__
#define __callback_h__

struct _gsglCallBack
{
	gsglboolean  Button;
	gsglboolean  Key;
	gsglboolean  Init_motion;
	int Motion;
	gsglvec2f Prev_pos;
	gsglvec3f Prev_angle;
	float Prev_scale;
	gsglvec3f Prev_displace;
	gsglvec3f Center;
	unsigned int *Prev_list;
	unsigned int Prev_proj;
	int Prev_Event;
	selected_obj *Selected_obj;
	gsglboolean Select_2d;
	gsglboolean Select_3d;
};

#ifdef __cplusplus
extern "C"{
#endif

void gsgl_button_press (int x, int y, gsglPage *);
void gsgl_button_release (gsglPage *); 
void gsgl_mouse_motion (int x, int y, gsglPage *);
void gsgl_key_motion (int x, int y, gsglPage *);

void gsgl_gui_pointer (gsglPage *, gsglboolean redraw);

void gsgl_3d_restore (gsglPage *);
void gsgl_3d_trans_z (gsglPage *);
void gsgl_3d_trans_xy (gsglPage *);
void gsgl_3d_scale (float x, float y, float z, gsglPage *);
void gsgl_3d_rotate (float x, float y, float z, gsglPage *);
void gsgl_lighting_toggle (gsglPage *);

void gsgl_2d_restore (gsglPage *);
void gsgl_2d_trans (gsglPage *);
void gsgl_2d_scale (float x, float y, gsglPage *);
void gsgl_2d_rotate (float x, float y, gsglPage *);

void gsgl_2d_select_objects (gsglPage *);
void gsgl_3d_select_objects (gsglPage *);
void gsgl_2d_select_objects_without_color (gsglPage *);
void gsgl_3d_select_objects_without_color (gsglPage *);

void gsgl_unselect_objects (gsglPage *);
void gsgl_select_by_name (GLuint name, gsglPage *);

void gsgl_utility_box (int Xstart, int Ystart, int x, int y, gsglPage *p);
void gsgl_utility_cross (float x0, float y0, float z0, gsglPage *p);
void gsgl_utility_clean (gsglPage *);

void gsgl_proj_restore (gsglPage *);
void gsgl_proj_trans (gsglPage *);
void gsgl_proj_scale (float x, float y, gsglPage *);
void gsgl_proj_rotate (float x, float y, gsglPage *);

gsglboolean gsgl_2d_is_selected (gsglPage *);
gsglboolean gsgl_3d_is_selected (gsglPage *);

void gsgl_init_select (gsglPage *);

/*#define GSGL_NAME_IS_FRAME(m) (((m) > GSGL_NAME_FRAME && (m)%2 == 0) ? 1 : 0) 
  #define GSGL_NAME_IS_CORNER(m) (((m) > GSGL_NAME_FRAME && (m)%2 == 1) ? 1 : 0) */

#ifdef __cplusplus
}
#endif

#endif
