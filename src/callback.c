#include "gsaf.h"
#include "gsgl.h"
#include "base.h"
#include "xwin.h"
#include "ogl.h"
#include "callback.h"
#include "displaylist.h"
#include "matrix.h"
#include "drv_gl.h"
#include "utils.h"
#include "configfile.h"

#define GSGL_MOTION_3D_NON          0
#define GSGL_MOTION_3D_ROTATE       1
#define GSGL_MOTION_3D_SCALE        2
#define GSGL_MOTION_3D_TRANSLATE_XY 3
#define GSGL_MOTION_3D_TRANSLATE_Z  4
#define GSGL_MOTION_2D_NON          10
#define GSGL_MOTION_2D_ROTATE       11
#define GSGL_MOTION_2D_SCALE        12
#define GSGL_MOTION_2D_TRANSLATE    13
#define GSGL_MOTION_PROJ_NON        20
#define GSGL_MOTION_PROJ_ROTATE     21
#define GSGL_MOTION_PROJ_SCALE      22
#define GSGL_MOTION_PROJ_TRANSLATE  23
#define GSGL_MOTION_NON            100
#define GSGL_MOTION_IS_2D(m)        (m/10 == 1 ? GSGL_TRUE : GSGL_FALSE)
#define GSGL_MOTION_IS_3D(m)        (m/10 == 0 ? GSGL_TRUE : GSGL_FALSE)
#define GSGL_MOTION_IS_PROJ(m)      (m/10 == 2 ? GSGL_TRUE : GSGL_FALSE)

#define FACT 1.0
#define ANGLE_FACT 57.2
#define EVENT_NULL     0
#define EVENT_PRESS    1
#define EVENT_RELEASE  2
#define EVENT_MOVE     3

gsglboolean gsgl_2d_is_selected (gsglPage *p)
{
	return p->callback->Select_2d;
}

gsglboolean gsgl_3d_is_selected (gsglPage *p)
{
	return p->callback->Select_3d;
}

void gsgl_init_select (gsglPage *p)
{
	gsglCallBack *c = p->callback;

	c->Button = GSGL_FALSE;
	c->Prev_list = NULL;
	c->Prev_proj = 0;
	c->Init_motion = GSGL_FALSE;
	c->Selected_obj = NULL;
	c->Select_2d = GSGL_FALSE;
	c->Select_3d = GSGL_FALSE;
	c->Prev_Event = EVENT_NULL;
	c->Prev_angle.x = 0.0;
	c->Prev_angle.y = 0.0;
	c->Prev_angle.z = 0.0;
	c->Prev_scale = 1.0;
	c->Prev_displace.x = 0.0;
	c->Prev_displace.y = 0.0; 
	c->Prev_displace.z = 0.0;
	c->Key = GSGL_FALSE;	
}

/*-----------------------------------------------------*/
/* make displaylists to change selected objects' color */
/*-----------------------------------------------------*/
static void set_objects_color (gsglDisplaylist *list, gsglPage *p)
{
	gsglDisplaylist *list_color;
	GLuint gllist_old;
	gsglConfigColor *gsglcolor;
	
	list_color = gsgl_displaylist_inq_gui_color (list);
	if (list_color == NULL) {
		gsgl_error ("set_objects_color", "color list is null");
		return;
	}

	gllist_old = list_color->gllist;
	list_color->gllist = glGenLists(1);
	glNewList (list_color->gllist, GL_COMPILE);
	gsglcolor = gsgl_inq_config_color (p->library);
	glColor3dv (gsglcolor->frame); 
	glEndList ();
	glDeleteLists(gllist_old, 1);
}

static void make_stack_identity (gsglDisplaylist *list0)
{
	gsglDisplaylist *list_stack;
	GLuint gllist;

	list_stack = gsgl_displaylist_inq_pre_stack (list0);
	gllist = list_stack->gllist;

	list_stack->gllist = glGenLists(1);
	glNewList (list_stack->gllist, GL_COMPILE);
	glLoadIdentity ();
	glEndList ();
	glDeleteLists (gllist, 1);
}

static void make_stack_pop_push (gsglDisplaylist *list0)
{
	gsglDisplaylist *list_stack;
	GLuint gllist;

	list_stack = gsgl_displaylist_inq_pre_stack (list0);
	gllist = list_stack->gllist;

	list_stack->gllist = glGenLists(1);
	glNewList (list_stack->gllist, GL_COMPILE);
	glPopMatrix ();
	glPushMatrix ();
	glEndList ();
	glDeleteLists (gllist, 1);
}

static unsigned int inq_selected_obj_num (gsglPage *p)
{
	return gsgl_list_count (p->callback->Selected_obj);
}

/*****************************/
/*      Cancel selections    */
/*****************************/
void gsgl_unselect_objects (gsglPage *p)
{
	gsglDisplaylist *color;
	GLuint gllist;
	selected_obj *obj, *obj_old;
	gsglCallBack *c = p->callback;

	c->Select_2d = GSGL_FALSE;
	c->Select_3d = GSGL_FALSE;

	if (c->Selected_obj) {
		obj = c->Selected_obj;
		while (obj) {
			/* restore selected objects' color */
			color = gsgl_displaylist_inq_gui_color (obj->data);
			if (color) {
				gllist = color->gllist;
			} else {
				gsgl_error ("gsgl_unselect_objects", "color list is null");
				break;
			}
			if (gllist > 0) glDeleteLists(gllist, 1);
			color->gllist = 0;
			/* */
			make_stack_identity (obj->data);

			/* */
			obj = obj->next;
		}
	}

	/* release Selected_obj lists */
	if (c->Selected_obj) {
		obj = c->Selected_obj;
		while (obj) {
			obj_old = obj;
			obj = obj->next;
			gsgl_free (obj_old);
		}
	}
	c->Selected_obj = NULL;
}

/**********************************************/
/*   Select a object whose name is given      */
/**********************************************/
void gsgl_select_by_name (GLuint name, gsglPage *p)
{
	gsgllist *list;
	gsglDisplaylist *d;
	selected_obj *obj;
	gsglCallBack *c = p->callback;

	if (!name) return;

	gsgl_displaylist_remake (p);
	list = gsgl_displaylist_inq_after_header (p);
	obj = gsgl_list_last (c->Selected_obj);

	while (list) {
		d = list->data;
		if (d->name == name) {
			make_stack_pop_push (list->data);
			if (GSGL_LIST_IS_2DOBJ(d->type)) {
				c->Select_2d = GSGL_TRUE;
				c->Select_3d = GSGL_FALSE;
			} else if (GSGL_LIST_IS_3DOBJ(d->type)) {
				c->Select_2d = GSGL_FALSE;
				c->Select_3d = GSGL_TRUE;
			} else {
				gsgl_warning ("gsgl_select_by_name", "selected something not object");
					return;
			}
			c->Selected_obj = gsgl_list_append (c->Selected_obj, list->data);
			set_objects_color (list->data, p);

			return;
		}
		list = list->next;
	}
}

static void gsgl_select_all_objects (int dim, gsglboolean color, gsglPage *p)
{
	gsgllist *list;
	gsglDisplaylist *d;
	gsglCallBack *c = p->callback;

	gsgl_displaylist_remake (p);

	gsgl_unselect_objects (p);

	if (dim == 2) {
		c->Select_2d = GSGL_TRUE;
		c->Select_3d = GSGL_FALSE;
	} else {
		c->Select_2d = GSGL_FALSE;
		c->Select_3d = GSGL_TRUE;
	}

	/* select newly */
	list = gsgl_displaylist_inq_after_header (p);
	while (list) {
		d = list->data;
		if ((dim == 2 && GSGL_LIST_IS_2DOBJ(d->type)) ||  /* 2D only */
		    (dim == 3 && GSGL_LIST_IS_3DOBJ(d->type))) {  /* 3D only */
			c->Selected_obj = gsgl_list_append (c->Selected_obj, list->data);
			make_stack_pop_push (list->data);

			if (color) set_objects_color (list->data, p);
		}
		list = list->next;
	}
	gsgl_redraw (p);	
}

/***********************************************/
/*           Select all 2d Objects             */
/***********************************************/
void gsgl_2d_select_objects (gsglPage *p)
{
	gsgl_select_all_objects (2, GSGL_TRUE, p);
}

void gsgl_2d_select_objects_without_color (gsglPage *p)
{
	gsgl_select_all_objects (2, GSGL_FALSE, p);
}

/***********************************************/
/*           Select all 3d Objects             */
/***********************************************/
void gsgl_3d_select_objects (gsglPage *p)
{
	gsgl_select_all_objects (3, GSGL_TRUE, p);
}

void gsgl_3d_select_objects_without_color (gsglPage *p)
{
	gsgl_select_all_objects (3, GSGL_FALSE, p);
}

static void button_press (gsglPage *p)  /* x, y are mouse position */
{
	int i;
	gsgllist *list;
	gsglDisplaylist *d;
	selected_obj *obj;
	gsglCallBack *c = p->callback;

	if (GSGL_MOTION_IS_2D(c->Motion) || GSGL_MOTION_IS_3D(c->Motion)) {
		if (c->Prev_list != NULL) {
			gsgl_free (c->Prev_list);
			c->Prev_list = NULL;
		}
		if (c->Selected_obj == NULL) return;
		obj = c->Selected_obj;
		c->Prev_list = (GLuint *) gsgl_malloc_atomic (sizeof(GLuint)*inq_selected_obj_num(p));
		for (i=0; i<inq_selected_obj_num (p); i++) {
			d = obj->data;
			list = gsgl_displaylist_inq_by_name (d->name, p);
			d = gsgl_displaylist_inq_gui_matrix (list->data);
			c->Prev_list[i] = d->gllist;
			/*list->gllist = 0;*/
			/* Without upper line, Prev_list[i]'s list will be removed
                           by glDeleteLists on mouse moving */
			obj = obj->next;
		}
	} else if (GSGL_MOTION_IS_PROJ(c->Motion)) {
		d = gsgl_displaylist_inq_list_of_projection (p);
		c->Prev_proj = d->gllist;
		/*GList_projection->gllist = 0;*/
	}
}

/**************************/
/*    button is pressed   */
/**************************/
void gsgl_button_press (int x, int y, gsglPage *p)  /* x, y are mouse position */
{	
	gsglCallBack *c = p->callback;

	c->Button = GSGL_TRUE;
	c->Prev_pos.x = x;
	c->Prev_pos.y = y;
	
	c->Init_motion = GSGL_TRUE;
	c->Prev_Event = EVENT_PRESS;
}

/*--------------------------*/
/*  button is released (2D) */
/*--------------------------*/
static void gsgl_button_release_2d (gsglPage *p)
{
	gsglCallBack *c = p->callback;

	switch (c->Motion) {
	case GSGL_MOTION_2D_ROTATE:
		if (fabs(c->Prev_angle.z) > FLOAT_EPS) {
			glTranslatef (c->Center.x, c->Center.y, 0.0);
			glRotatef (c->Prev_angle.z, 0.0, 0.0, 1.0);
			glTranslatef (-c->Center.x, -c->Center.y, 0.0);
		}
		break;
	case GSGL_MOTION_2D_SCALE:
		if (fabs(c->Prev_scale - 1.0) > FLOAT_EPS) {
			glTranslatef (c->Center.x, c->Center.y, 0.0);
			glScalef (c->Prev_scale, c->Prev_scale, 1.0);
			glTranslatef (-c->Center.x, -c->Center.y, 0.0);
		}
		break;
	case GSGL_MOTION_2D_TRANSLATE:
		if (fabs(c->Prev_displace.x) > FLOAT_EPS || fabs(c->Prev_displace.y) > FLOAT_EPS) {
			glTranslatef (c->Prev_displace.x, c->Prev_displace.y, 0.0);
		}
		break;
	default:
		break;
	}
}

/*-------------------------*/
/*  Button is released (3D) */
/*-------------------------*/
static void gsgl_button_release_3d (gsglPage *p)
{
	gsglCallBack *c = p->callback;

	switch (c->Motion) {
	case GSGL_MOTION_3D_ROTATE:
		if (fabs(c->Prev_angle.x) > FLOAT_EPS || fabs(c->Prev_angle.y) > FLOAT_EPS) {
			glTranslatef (c->Center.x, c->Center.y, c->Center.z);
			glRotatef (c->Prev_angle.x, 0.0, 1.0, 0.0);
			glRotatef (c->Prev_angle.y, 1.0, 0.0, 0.0);
			glTranslatef (-c->Center.x, -c->Center.y, -c->Center.z);
		}
		break;
	case GSGL_MOTION_3D_SCALE:
		if (fabs(c->Prev_scale - 1.0) > FLOAT_EPS) {
			glTranslatef (c->Center.x, c->Center.y, c->Center.z);
			glScalef (c->Prev_scale, c->Prev_scale, c->Prev_scale);
			glTranslatef (-c->Center.x, -c->Center.y, -c->Center.z);
		}
		break;
	case GSGL_MOTION_3D_TRANSLATE_XY:
		if (fabs(c->Prev_displace.x) > FLOAT_EPS || fabs(c->Prev_displace.y) > FLOAT_EPS) {
			glTranslatef (c->Prev_displace.x, c->Prev_displace.y, 0.0);
		}
		break;
	case GSGL_MOTION_3D_TRANSLATE_Z:
		if (fabs(c->Prev_displace.z) > FLOAT_EPS) {
			glTranslatef (0.0, 0.0, c->Prev_displace.z);
		}
		break;
	default:
		break;
	}
}

/*----------------------------------*/
/*  Button is released (Projection) */
/*----------------------------------*/
static void gsgl_button_release_proj (gsglPage *p)
{
	float center_proj_x ,center_proj_y;
	gsglCallBack *c = p->callback;

	switch (c->Motion) {
	case GSGL_MOTION_PROJ_ROTATE:
		center_proj_x = 2.0*c->Center.x/(GLfloat) GSGL_PAPER_WIDTH-1.0;
		center_proj_y = 2.0*c->Center.y/(GLfloat) GSGL_PAPER_HEIGHT-1.0;
		if (fabs(c->Prev_angle.z) > FLOAT_EPS) {
			glTranslatef (center_proj_x, center_proj_y, 0.0);
			glRotatef (c->Prev_angle.z, 0.0, 0.0, 1.0);
			glTranslatef (-center_proj_x, -center_proj_y, 0.0);
		}
		break;
	case GSGL_MOTION_PROJ_SCALE:
		center_proj_x = 2.0*c->Center.x/(GLfloat) GSGL_PAPER_WIDTH-1.0;
		center_proj_y = 2.0*c->Center.y/(GLfloat) GSGL_PAPER_HEIGHT-1.0;
		if (fabs(c->Prev_scale - 1.0) > FLOAT_EPS) {
			glTranslatef (center_proj_x, center_proj_y, 0.0);
			glScalef (c->Prev_scale, c->Prev_scale, 1.0);
			glTranslatef (-center_proj_x, -center_proj_y, 0.0);
		}
		break;
	case GSGL_MOTION_PROJ_TRANSLATE:
		if (fabs(c->Prev_displace.x) > FLOAT_EPS || fabs(c->Prev_displace.y) > FLOAT_EPS) {
			glTranslatef (c->Prev_displace.x, c->Prev_displace.y, 0.0);
		}
		break;
	default:
		break;
	}
}

/************************/
/*  Button is released  */
/************************/
void gsgl_button_release (gsglPage *p) 
{
	GLuint gllist, gllist_old = 0;
	int i;
	selected_obj *obj;
	gsgllist *list;
	gsglDisplaylist *d;
	GLdouble   matrix[16];
	gsglCallBack *c = p->callback;
	
	c->Button = GSGL_FALSE;
	if (c->Prev_Event != EVENT_MOVE) {
		c->Prev_Event = EVENT_RELEASE;
		return;
	}
	c->Prev_Event = EVENT_RELEASE;

	if (GSGL_MOTION_IS_2D(c->Motion) || GSGL_MOTION_IS_3D(c->Motion)) {
		if (!c->Prev_list) return;
		obj = c->Selected_obj;
		for (i=0; i<inq_selected_obj_num (p); i++) {
			d = obj->data;
			list = gsgl_displaylist_inq_by_name (d->name, p);
			d = gsgl_displaylist_inq_gui_matrix (list->data);
			/* Get modelview matrix correspond to c->Prev_list[i] */
			glLoadIdentity ();
			glCallList (c->Prev_list[i]);
			glGetDoublev (GL_MODELVIEW_MATRIX, matrix);
			glLoadIdentity ();
			/* */
			gllist = glGenLists(1);
			gllist_old = d->gllist;
			glNewList (gllist, GL_COMPILE);
			if (GSGL_MOTION_IS_2D(c->Motion)) {
				gsgl_button_release_2d (p);
				glMultMatrixd (matrix);
				glEndList();
			} else if (GSGL_MOTION_IS_3D(c->Motion)) {
				gsgl_button_release_3d (p);
				glMultMatrixd (matrix);
				glEndList();
			}
			d->gllist = gllist;
			glDeleteLists (gllist_old, 1);
			obj = obj->next;
		}
		for (i=0; i<inq_selected_obj_num (p); i++)
			glDeleteLists (c->Prev_list[i], 1);
		if (c->Prev_list != NULL)
			gsgl_free (c->Prev_list);
		c->Prev_list = NULL;

		glPopMatrix ();
		glLoadIdentity ();
		glPushMatrix ();
	} else if (GSGL_MOTION_IS_PROJ(c->Motion)) {
		/* Get modelview matrix correspond to c->Prev_proj */
		glMatrixMode (GL_PROJECTION);
		glPopMatrix ();
		glLoadIdentity ();
		glPushMatrix ();
		glCallList (c->Prev_proj);
		glMatrixMode (GL_MODELVIEW);
		/* */
		glMatrixMode (GL_PROJECTION);
		glPopMatrix ();
		glGetDoublev (GL_PROJECTION_MATRIX, matrix);
		glLoadIdentity ();
		glPushMatrix ();
		glMatrixMode (GL_MODELVIEW);
		/* */
		gllist = glGenLists(1);
		d = gsgl_displaylist_inq_list_of_projection (p);
		gllist_old = d->gllist;
		glNewList (gllist, GL_COMPILE);
		glMatrixMode (GL_PROJECTION);
		glPopMatrix ();
		gsgl_button_release_proj (p);
		glMultMatrixd (matrix);
		glPushMatrix ();
		glMatrixMode (GL_MODELVIEW);
		glEndList();
		glDeleteLists (gllist_old, 1);
		d->gllist = gllist;

		c->Prev_proj = 0;
		glMatrixMode (GL_PROJECTION);
		glPopMatrix ();
		glLoadIdentity ();
		glPushMatrix ();
		glMatrixMode (GL_MODELVIEW);
	}
	c->Prev_angle.x = 0.0;
	c->Prev_angle.y = 0.0;
	c->Prev_angle.z = 0.0;
	c->Prev_scale = 1.0;
	c->Prev_displace.x = 0.0;
	c->Prev_displace.y = 0.0;
	c->Prev_displace.z = 0.0;	
}

/*--------------------*/
/*  Mouse moving (2D) */
/*--------------------*/
static void gsgl_mouse_motion_2d (int x, int y, gsglPage *p)
{
	float           angle_z;
	double          scale;
	double          length, prev_length;
	double          dx, dy;
	double          win_length;
	double          cost, sint;
	int             center_x, center_y; 
	float           a;
	gsglCallBack *c = p->callback;

	center_x = c->Center.x / gsgl_inq_x_ratio (p->win);
	center_y = gsgl_inq_window_height (p->win) - c->Center.y / gsgl_inq_y_ratio (p->win);
	win_length = sqrt(pow(center_x * 2.0,2.0)+pow(center_y * 2.0,2.0));
	switch (c->Motion) {
	case GSGL_MOTION_2D_ROTATE:
		glTranslatef (c->Center.x, c->Center.y, 0.0);

		if (!c->Key) {
			cost = (x - center_x) * (c->Prev_pos.x - center_x) +
				(y - center_y) * (c->Prev_pos.y - center_y);
			cost /= sqrt(pow(x-center_x, 2.0) + pow(y-center_y, 2.0));
			cost /= sqrt(pow(c->Prev_pos.x-center_x, 2.0)+pow(c->Prev_pos.y-center_y, 2.0));
			sint = (x - center_x) * (c->Prev_pos.y - center_y) -
				(c->Prev_pos.x - center_x) * (y - center_y);
			sint /= sqrt(pow(x-center_x, 2.0) + pow(y-center_y, 2.0));
			sint /= sqrt(pow(c->Prev_pos.x-center_x, 2.0)+pow(c->Prev_pos.y-center_y, 2.0));
			if (sint > 0.0) sint = 1.0;
			else if (sint < 0.0) sint = -1.0;
			else sint = 0.0;
			
			angle_z = sint * ANGLE_FACT * acos(cost) + c->Prev_angle.z;
		} else {
			if (sqrt((x-c->Prev_pos.x)*(x-c->Prev_pos.x)+(y-c->Prev_pos.y)*(y-c->Prev_pos.y)) > 5.0) {
				a = 5.0;
			} else {
				a = 0.5;
			}
			if (x-c->Prev_pos.x > 0.5) {
				angle_z = c->Prev_angle.z-a; 
			} else if (x-c->Prev_pos.x < -0.5) {
				angle_z = c->Prev_angle.z+a; 
			} else if (y-c->Prev_pos.y > 0.5) {
				angle_z = c->Prev_angle.z+a; 
			} else if (y-c->Prev_pos.y < -0.5) {
				angle_z = c->Prev_angle.z-a; 
			} else {
				angle_z = c->Prev_angle.z;
			}
		}
		glRotatef (angle_z, 0.0, 0.0, 1.0);
		glTranslatef (-c->Center.x, -c->Center.y, 0.0);
		c->Prev_angle.z = angle_z;
		break;
	case GSGL_MOTION_2D_SCALE:
		glTranslatef (c->Center.x, c->Center.y, 0.0);
		if (!c->Key) {
			dx = x - center_x;
			dy = y - center_y;
			length = sqrt(pow(dx, 2.0) + pow(dy, 2.0));
			dx = c->Prev_pos.x - center_x;
			dy = c->Prev_pos.y - center_y;
			prev_length = sqrt(pow(dx, 2.0) + pow(dy, 2.0));
			scale = (1.0+1.5*(length-prev_length)/win_length)*c->Prev_scale;
		} else {
			if (sqrt((x-c->Prev_pos.x)*(x-c->Prev_pos.x)+(y-c->Prev_pos.y)*(y-c->Prev_pos.y)) > 5.0) {
				a = 1.05;
			} else {
				a = 1.005;
			}
			if (x-c->Prev_pos.x > 0.5) {
				scale = c->Prev_scale * a; 
			} else if (x-c->Prev_pos.x < -0.5) {
				scale = c->Prev_scale / a; 
			} else if (y-c->Prev_pos.y > 0.5) {
				scale = c->Prev_scale / a; 
			} else if (y-c->Prev_pos.y < -0.5) {
				scale = c->Prev_scale * a; 
			} else {
				scale = c->Prev_scale;
			}
		}
		glScalef (scale, scale, 1.0);
		glTranslatef (-c->Center.x, -c->Center.y, 0.0);
		c->Prev_scale = scale;
		break;
	case GSGL_MOTION_2D_TRANSLATE:
		dx = gsgl_inq_x_ratio (p->win) * (x - c->Prev_pos.x) + c->Prev_displace.x;
		dy =-gsgl_inq_y_ratio (p->win) * (y - c->Prev_pos.y) + c->Prev_displace.y;
		glTranslatef (dx, dy, 0.0);
		c->Prev_displace.x = dx;
		c->Prev_displace.y = dy;
		break;
	default:
		break;
	}
}

/*--------------------*/
/*  Mouse moving (3D) */
/*--------------------*/
static void gsgl_mouse_motion_3d (int x, int y, gsglPage *p)
{
	float           angle_x, angle_y;
	double          scale;
	double          length, prev_length;
	double          dx, dy, dz;
	double          win_length=sqrt(pow(gsgl_inq_window_width (p->win),2.0)+pow(gsgl_inq_window_height (p->win),2.0));
	float           a;
	gsglCallBack *c = p->callback;

	switch (c->Motion) {
	case GSGL_MOTION_3D_ROTATE:
		glTranslatef (c->Center.x, c->Center.y, c->Center.z);
		angle_x = (x-c->Prev_pos.x)/(float)gsgl_inq_window_width (p->win) *180.0* FACT + c->Prev_angle.x;
		angle_y = (y-c->Prev_pos.y)/(float)gsgl_inq_window_height (p->win) *180.0* FACT + c->Prev_angle.y;
		glRotatef (angle_x, 0.0, 1.0, 0.0);
		glRotatef (angle_y, 1.0, 0.0, 0.0);
		glTranslatef (-c->Center.x, -c->Center.y, -c->Center.z);
		c->Prev_angle.x = angle_x;
		c->Prev_angle.y = angle_y;
		break;
	case GSGL_MOTION_3D_SCALE:
		glTranslatef (c->Center.x, c->Center.y, c->Center.z);
		if (!c->Key) {
			dx = x - gsgl_inq_window_width (p->win) / 2.0;
			dy = y - gsgl_inq_window_height (p->win) / 2.0;
			length = sqrt(pow(dx, 2.0) + pow(dy, 2.0));
			dx = c->Prev_pos.x - gsgl_inq_window_width (p->win) / 2.0;
			dy = c->Prev_pos.y - gsgl_inq_window_height (p->win) / 2.0;
			prev_length = sqrt(pow(dx, 2.0) + pow(dy, 2.0));
			scale = (1.0+1.5*(length-prev_length)/win_length)*c->Prev_scale;
		} else {
			if (sqrt((x-c->Prev_pos.x)*(x-c->Prev_pos.x)+(y-c->Prev_pos.y)*(y-c->Prev_pos.y)) > 5.0) {
				a = 1.05;
			} else {
				a = 1.005;
			}
			if (x-c->Prev_pos.x > 0.5) {
				scale = c->Prev_scale * a; 
			} else if (x-c->Prev_pos.x < -0.5) {
				scale = c->Prev_scale / a; 
			} else if (y-c->Prev_pos.y > 0.5) {
				scale = c->Prev_scale / a; 
			} else if (y-c->Prev_pos.y < -0.5) {
				scale = c->Prev_scale * a; 
			} else {
				scale = c->Prev_scale;
			}
		}
		glScalef (scale, scale, scale);
		glTranslatef (-c->Center.x, -c->Center.y, -c->Center.z);
		c->Prev_scale = scale;
		break;
	case GSGL_MOTION_3D_TRANSLATE_XY:
		dx = FACT * gsgl_inq_x_ratio (p->win) * (x - c->Prev_pos.x) + c->Prev_displace.x;
		dy =-FACT * gsgl_inq_y_ratio (p->win) * (y - c->Prev_pos.y) + c->Prev_displace.y;
		glTranslatef (dx, dy, 0.0);
		c->Prev_displace.x = dx;
		c->Prev_displace.y = dy;
		break;
	case GSGL_MOTION_3D_TRANSLATE_Z:
		dz = gsgl_inq_y_ratio (p->win) * (y-c->Prev_pos.y) + c->Prev_displace.z;
		glTranslatef (0.0, 0.0, dz);
		c->Prev_displace.z = dz;
		break;
	default:
		break;
	}
}

/*----------------------------*/
/*  Mouse moving (Porjection) */
/*----------------------------*/
static void gsgl_mouse_motion_proj (int x, int y, gsglPage *p)
{
	float           angle_z;
	double          scale;
	double          length, prev_length;
	double          dx, dy;
	double          win_length;
	double          cost, sint;
	int             center_x, center_y; 
	float           center_proj_x, center_proj_y;
	float           a;
	gsglCallBack *c = p->callback;
	
	center_x = c->Center.x / gsgl_inq_x_ratio (p->win);
	center_y = gsgl_inq_window_height (p->win) - c->Center.y / gsgl_inq_y_ratio (p->win);
	win_length = sqrt(pow (center_x * 2.0, 2.0) + pow (center_y * 2.0, 2.0));
	switch (c->Motion) {
	case GSGL_MOTION_PROJ_ROTATE:
		center_proj_x = 2.0*c->Center.x/(GLfloat) GSGL_PAPER_WIDTH-1.0;
		center_proj_y = 2.0*c->Center.y/(GLfloat) GSGL_PAPER_HEIGHT-1.0;

		glTranslatef (center_proj_x, center_proj_y, 0.0);
		if (!c->Key) {
			cost = (x - center_x) * (c->Prev_pos.x - center_x) +
				(y - center_y) * (c->Prev_pos.y - center_y);
			cost /= sqrt(pow(x-center_x, 2.0) + pow(y-center_y, 2.0));
			cost /= sqrt(pow(c->Prev_pos.x-center_x, 2.0)+pow(c->Prev_pos.y-center_y, 2.0));
			sint = (x - center_x) * (c->Prev_pos.y - center_y) -
				(c->Prev_pos.x - center_x) * (y - center_y);
			sint /= sqrt(pow(x-center_x, 2.0) + pow(y-center_y, 2.0));
			sint /= sqrt(pow(c->Prev_pos.x-center_x, 2.0)+pow(c->Prev_pos.y-center_y, 2.0));
			if (sint > 0.0) sint = 1.0;
			else if (sint < 0.0) sint = -1.0;
			else sint = 0.0;

			angle_z = sint * ANGLE_FACT * acos(cost) + c->Prev_angle.z;
		} else {
			if (sqrt((x-c->Prev_pos.x)*(x-c->Prev_pos.x)+(y-c->Prev_pos.y)*(y-c->Prev_pos.y)) > 5.0) {
				a = 5.0;
			} else {
				a = 0.5;
			}
			if (x-c->Prev_pos.x > 0.5) {
				angle_z = c->Prev_angle.z-a; 
			} else if (x-c->Prev_pos.x < -0.5) {
				angle_z = c->Prev_angle.z+a; 
			} else if (y-c->Prev_pos.y > 0.5) {
				angle_z = c->Prev_angle.z+a; 
			} else if (y-c->Prev_pos.y < -0.5) {
				angle_z = c->Prev_angle.z-a; 
			} else {
				angle_z = c->Prev_angle.z;
			}
		}
		glRotatef (angle_z, 0.0, 0.0, 1.0);
		glTranslatef (-center_proj_x, -center_proj_y, 0.0);
		c->Prev_angle.z = angle_z;
		break;
	case GSGL_MOTION_PROJ_SCALE:
		center_proj_x = 2.0*c->Center.x/(GLfloat) GSGL_PAPER_WIDTH-1.0;
		center_proj_y = 2.0*c->Center.y/(GLfloat) GSGL_PAPER_HEIGHT-1.0;
		glTranslatef (center_proj_x, center_proj_y, 0.0);
		if (!c->Key) {
			dx = x - center_x;
			dy = y - center_y;
			length = sqrt(pow(dx, 2.0) + pow(dy, 2.0));
			dx = c->Prev_pos.x - center_x;
			dy = c->Prev_pos.y - center_y;
			prev_length = sqrt(pow(dx, 2.0) + pow(dy, 2.0));
			scale = (1.0+1.5*(length-prev_length)/win_length)*c->Prev_scale;
		} else {
			if (sqrt((x-c->Prev_pos.x)*(x-c->Prev_pos.x)+(y-c->Prev_pos.y)*(y-c->Prev_pos.y)) > 5.0) {
				a = 1.05;
			} else {
				a = 1.005;
			}
			if (x-c->Prev_pos.x > 0.5) {
				scale = c->Prev_scale * a; 
			} else if (x-c->Prev_pos.x < -0.5) {
				scale = c->Prev_scale / a; 
			} else if (y-c->Prev_pos.y > 0.5) {
				scale = c->Prev_scale / a; 
			} else if (y-c->Prev_pos.y < -0.5) {
				scale = c->Prev_scale * a; 
			} else {
				scale = c->Prev_scale;
			}
		}
		glScalef (scale, scale, 1.0);
		glTranslatef (-center_proj_x, -center_proj_y, 0.0);
		c->Prev_scale = scale;
		break;
	case GSGL_MOTION_PROJ_TRANSLATE:
		dx = 2.0 * gsgl_inq_x_ratio (p->win) * (x - c->Prev_pos.x) / (GLfloat) GSGL_PAPER_WIDTH 
			+ c->Prev_displace.x;
		dy =-2.0 * gsgl_inq_y_ratio (p->win) * (y - c->Prev_pos.y) / (GLfloat) GSGL_PAPER_HEIGHT
			+ c->Prev_displace.y;
		glTranslatef (dx, dy, 0.0);
		c->Prev_displace.x = dx;
		c->Prev_displace.y = dy;
		break;
	default:
		break;
	}
}

/*****************/
/*  Mouse moving */
/*****************/
static void gsgl_motion_common (int x, int y, gsglboolean key, gsglPage *p) 
{
	int      i;
	GLuint   gllist, gllist_old = 0;
	selected_obj    *obj;
	gsglDisplaylist *d;
	gsgllist *list;
	gsglCallBack *c = p->callback;

	if (!c->Button) return;
	if (c->Prev_Event == EVENT_PRESS) {
		button_press (p);
       		c->Prev_Event = EVENT_MOVE;
	}
	c->Prev_Event = EVENT_MOVE;
	if (key) 
		c->Key = GSGL_TRUE;
	else
		c->Key = GSGL_FALSE;
	
	if (GSGL_MOTION_IS_2D(c->Motion) || GSGL_MOTION_IS_3D(c->Motion)) {
		if (c->Selected_obj == NULL) return;
		glPopMatrix ();
		glLoadIdentity();
		if (GSGL_MOTION_IS_2D(c->Motion)) {
			gsgl_mouse_motion_2d (x, y, p);
		} else if (GSGL_MOTION_IS_3D(c->Motion)) {
			gsgl_mouse_motion_3d (x, y, p);
		}
		glPushMatrix ();
		
		obj = c->Selected_obj;
		for (i=0; i<inq_selected_obj_num (p); i++) {
			d = obj->data;
			list = gsgl_displaylist_inq_by_name (d->name, p);
			d = gsgl_displaylist_inq_gui_matrix (list->data);
			
			gllist = glGenLists(1);
			gllist_old = d->gllist;
			d->gllist = gllist;
			
			glNewList (gllist, GL_COMPILE);
			glCallList (c->Prev_list[i]);
			glEndList();
			
			if (!c->Init_motion)
				glDeleteLists (gllist_old, 1);
			obj = obj->next;
		}
	} else if (GSGL_MOTION_IS_PROJ(c->Motion)) {
		d = gsgl_displaylist_inq_list_of_projection (p);
		gllist_old = d->gllist;
		gllist = glGenLists(1);
		glNewList (gllist, GL_COMPILE);
		glMatrixMode (GL_PROJECTION);
		glPopMatrix ();
		gsgl_mouse_motion_proj (x, y, p);
		glPushMatrix ();
		glCallList (c->Prev_proj);
		glEndList();
		
		d->gllist = gllist;
		if (!c->Init_motion)
			glDeleteLists (gllist_old, 1);
	}
	c->Init_motion = GSGL_FALSE;
	gsgl_redraw (p);
	c->Prev_pos.x = x;
	c->Prev_pos.y = y;
}

void gsgl_mouse_motion (int x, int y, gsglPage *p) 
{
	gsgl_motion_common (x, y, GSGL_FALSE, p);
}

void gsgl_key_motion (int x, int y, gsglPage *p) 
{
	gsgl_motion_common (x, y, GSGL_TRUE, p);
}

/********************    *********************/

void gsgl_gui_pointer (gsglPage *p, gsglboolean redraw)
{
	gsglCallBack *c = p->callback;

	if (!gsgl_displaylist_inq_remade (p)) return;

        /* initialization */
	c->Button = GSGL_FALSE;
	c->Motion = GSGL_MOTION_NON;
	c->Prev_Event = EVENT_NULL;
	c->Prev_displace.x = 0.0;
	c->Prev_displace.y = 0.0;
	
	if (c->Selected_obj != NULL) {
		gsgl_unselect_objects (p);
		if (redraw)
			gsgl_redraw (p);	
	}
}

/******************** 3d *********************/

void gsgl_3d_restore (gsglPage *p)
{
	gsgllist *list;
	gsglDisplaylist *d;
	gsglCallBack *c = p->callback;

	if (!gsgl_displaylist_inq_remade (p)) return;

	list = gsgl_displaylist_inq_after_header (p);
	while (list) {
		d = list->data;
		if (GSGL_LIST_IS_3DOBJ(d->type)) {
			d = gsgl_displaylist_inq_gui_matrix (d);
			if (d) {
				glDeleteLists (d->gllist, 1);
				d->gllist = 0;
			}
		}
		list = list->next;
	}	
	gsgl_redraw (p);

	c->Motion = GSGL_MOTION_3D_NON;

	c->Prev_angle.x = 0.0;
	c->Prev_angle.y = 0.0;
	c->Prev_scale = 1.0;
	c->Prev_displace.x = 0.0;
	c->Prev_displace.y = 0.0;
	c->Prev_displace.z = 0.0;
}

void gsgl_3d_trans_z (gsglPage *p)
{
	gsglCallBack *c = p->callback;

        gsgl_displaylist_remake (p);  
        /* initialize */
	c->Button = GSGL_FALSE;
	c->Motion = GSGL_MOTION_3D_TRANSLATE_Z;
	c->Prev_displace.z = 0.0;
}

void gsgl_3d_trans_xy (gsglPage *p)
{
	gsglCallBack *c = p->callback;

        gsgl_displaylist_remake (p);
        /* initialize */
	c->Button = GSGL_FALSE;
	c->Motion = GSGL_MOTION_3D_TRANSLATE_XY;
	c->Prev_displace.x = 0.0;
	c->Prev_displace.y = 0.0;
}

void gsgl_3d_scale (float x, float y, float z, gsglPage *p)
{
	gsglCallBack *c = p->callback;

	gsgl_displaylist_remake (p);
        /* initialize */
	c->Button = GSGL_FALSE;
	c->Motion = GSGL_MOTION_3D_SCALE;
	c->Prev_scale = 1.0;
	c->Center.x = x;
	c->Center.y = y;
	c->Center.z = -z;
}

void gsgl_3d_rotate (float x, float y, float z, gsglPage *p)
{
	gsglCallBack *c = p->callback;

	gsgl_displaylist_remake (p);
        /* initialize */
	c->Prev_angle.x = 0.0;
	c->Prev_angle.y = 0.0;
	c->Button = GSGL_FALSE;
	c->Motion = GSGL_MOTION_3D_ROTATE;
	c->Center.x = x;
	c->Center.y = y;
	c->Center.z = -z;
}

void gsgl_lighting_toggle (gsglPage *p)
{
	GLuint gllist;
	gsgllist *list_of_list;
	gsglDisplaylist *d;

	gsgl_displaylist_remake (p);

	list_of_list = gsgl_displaylist_inq_list_of_list_lighting_enable (p);
	while (list_of_list) {
		d = list_of_list->data;
		gllist = d->gllist;
		if (gllist > 0)
			glDeleteLists (gllist, 1);

		gllist = glGenLists(1);
		glNewList (gllist, GL_COMPILE);
		if (!gsgl_inq_lighting_is_enabled (p))
			gsgl_gl_lighting_enable ();
		glEndList();
		
		d->gllist = gllist;
		list_of_list = list_of_list->next;
	}
	if (gsgl_inq_lighting_is_enabled (p)) {
		gsgl_set_lighting_disable (p);
	} else {
		gsgl_set_lighting_enable (p);
	}
	gsgl_redraw (p);
}

/******************** 2d *********************/

void gsgl_2d_restore (gsglPage *p)
{
	gsgllist *list;
	gsglDisplaylist *d;
	gsglCallBack *c = p->callback;

	if (!gsgl_displaylist_inq_remade (p)) return;

	list = gsgl_displaylist_inq_after_header (p);
	while (list) {
		d = list->data;
		if (GSGL_LIST_IS_2DOBJ(d->type)) {
			d = gsgl_displaylist_inq_gui_matrix (d);
			if (d) {
				glDeleteLists (d->gllist, 1);
				d->gllist = 0;
			}
		}
		list = list->next;
	}
	gsgl_redraw (p);
	
	c->Motion = GSGL_MOTION_2D_NON;

	c->Prev_angle.z = 0.0;
	c->Prev_scale = 1.0;
	c->Prev_displace.x = 0.0;
	c->Prev_displace.y = 0.0;
}

void gsgl_2d_trans (gsglPage *p)
{
	gsglCallBack *c = p->callback;

        gsgl_displaylist_remake (p);
        /* initialization */
	c->Button = GSGL_FALSE;
	c->Motion = GSGL_MOTION_2D_TRANSLATE;
	c->Prev_displace.x = 0.0;
	c->Prev_displace.y = 0.0;
}

void gsgl_2d_scale (float x, float y, gsglPage *p)
{
	gsglCallBack *c = p->callback;

	gsgl_displaylist_remake (p);
        /* initialize */
	c->Button = GSGL_FALSE;
	c->Motion = GSGL_MOTION_2D_SCALE;
	c->Prev_scale = 1.0;
	c->Center.x = x;
	c->Center.y = y;
}

void gsgl_2d_rotate (float x, float y, gsglPage *p)
{
	gsglCallBack *c = p->callback;

	gsgl_displaylist_remake (p);
        /* initialize */
	c->Prev_angle.z = 0.0;
	c->Button = GSGL_FALSE;
	c->Motion = GSGL_MOTION_2D_ROTATE;
	c->Center.x = x;
	c->Center.y = y;
}

/******************** proj *********************/

void gsgl_proj_restore (gsglPage *p)
{
	GLuint gllist, gllist_old;
	gsglDisplaylist *list_projection;
	gsglCallBack *c = p->callback;

	if (!gsgl_displaylist_inq_remade (p)) return;

	list_projection = gsgl_displaylist_inq_list_of_projection (p);
	gllist_old = list_projection->gllist;
	gllist = glGenLists(1);
	glNewList (gllist, GL_COMPILE);
	glEndList();
	list_projection->gllist = gllist;
	glDeleteLists (gllist_old ,1);

	gsgl_redraw (p);
	
	c->Motion = GSGL_MOTION_PROJ_NON;

	c->Prev_angle.z = 0.0;
	c->Prev_scale = 1.0;
	c->Prev_displace.x = 0.0;
	c->Prev_displace.y = 0.0;
}

void gsgl_proj_trans (gsglPage *p)
{
	gsglCallBack *c = p->callback;

        gsgl_displaylist_remake (p);
        /* initialization */
	c->Button = GSGL_FALSE;
	c->Motion = GSGL_MOTION_PROJ_TRANSLATE;
	c->Prev_displace.x = 0.0;
	c->Prev_displace.y = 0.0;
}

void gsgl_proj_scale (float x, float y, gsglPage *p)
{
	gsglCallBack *c = p->callback;
	
	gsgl_displaylist_remake (p);
        /* initialize */
	c->Button = GSGL_FALSE;
	c->Motion = GSGL_MOTION_PROJ_SCALE;
	c->Prev_scale = 1.0;
	c->Center.x = x;
	c->Center.y = y;
}

void gsgl_proj_rotate (float x, float y, gsglPage *p)
{
	gsglCallBack *c = p->callback;

	gsgl_displaylist_remake (p);
        /* initialize */
	c->Prev_angle.z = 0.0;
	c->Button = GSGL_FALSE;
	c->Motion = GSGL_MOTION_PROJ_ROTATE;
	c->Center.x = x;
	c->Center.y = y;
}

/******************** utility *********************/

void gsgl_utility_cross (float x, float y, float z, gsglPage *page)
{
	GLuint gllist_old, gllist_new;
	gsglDisplaylist *list_utility;
	float eyepos, eyepos_x, eyepos_y;

	list_utility = gsgl_displaylist_inq_list_of_utility (page);
	gllist_old = list_utility->gllist;
	gllist_new = glGenLists(1);
	glNewList (gllist_new, GL_COMPILE);
	glInitNames ();
	glPushName (0);
	
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gsgl_inq_eyepoint (&eyepos_x, &eyepos_y, &eyepos, page);
	if (gsgl_inq_projectiontype (page) == GSGL_ORTHOGONAL) {
		glOrtho ((GLdouble) (-0.5*GSGL_PAPER_WIDTH),
			 (GLdouble) ( 0.5*GSGL_PAPER_WIDTH),
			 (GLdouble) (-0.5* GSGL_PAPER_HEIGHT),
			 (GLdouble) ( 0.5* GSGL_PAPER_HEIGHT),
			 (GLdouble) eyepos, 
			 (GLdouble) eyepos + (GLdouble) GSGL_VIEW_DEPTH);
		glTranslatef (-0.5*GSGL_PAPER_WIDTH,
			      -0.5*GSGL_PAPER_HEIGHT,
			      -eyepos);
	} else if (gsgl_inq_projectiontype (page) == GSGL_FRUSTUM) {
		glFrustum ((GLdouble) -0.5*GSGL_PAPER_WIDTH,
			   (GLdouble) 0.5*GSGL_PAPER_WIDTH, 
			   (GLdouble) -0.5*GSGL_PAPER_HEIGHT,
			   (GLdouble) 0.5*GSGL_PAPER_HEIGHT,
			   (GLdouble) eyepos,
			   (GLdouble) (eyepos+GSGL_VIEW_DEPTH));
		glTranslatef(-0.5*GSGL_PAPER_WIDTH, 
			     -0.5*GSGL_PAPER_HEIGHT,
			     -eyepos);
	}
	glMatrixMode(GL_MODELVIEW);
	
	glLineWidth (1.0f);
	glBegin (GL_LINES);
	/*glColor3f (0.5, 0.2, 0.2);*/
	glColor3f (1.0, 0.0, 0.0);
	glVertex3f (x+1.0, y, -z);
	glVertex3f (x-1.0, y, -z);
	glVertex3f (x, y+1.0, -z);
	glVertex3f (x, y-1.0, -z);
	glVertex3f (x, y, -z+1.0);
	glVertex3f (x, y, -z-1.0);
	glEnd ();
	
	glEndList ();
	list_utility->gllist = gllist_new;
	glDeleteLists (gllist_old, 1);
	gsgl_redraw (page);
}

void gsgl_utility_box (int Xstart, int Ystart, int x, int y, gsglPage *p)
{
	GLuint gllist_old, gllist_new;
	float eyepos, ex, ey;
	gsglDisplaylist *list_utility;
	gsglConfigColor *gsglcolor;

	list_utility = gsgl_displaylist_inq_list_of_utility (p);
	gllist_old = list_utility->gllist;
	gllist_new = glGenLists(1);
	glNewList (gllist_new, GL_COMPILE);
	glEnable (GL_BLEND);
	glInitNames ();
	glPushName (0);
	
	if (Xstart >= 0) {
		glPushAttrib(GL_CURRENT_BIT | GL_LINE_BIT);
		glPushMatrix();
		glLoadIdentity();
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		gsgl_inq_eyepoint (&ex, &ey, &eyepos, p);
		if (gsgl_inq_projectiontype (p) == GSGL_ORTHOGONAL) {
			glOrtho (-0.5*GSGL_PAPER_WIDTH, 0.5*GSGL_PAPER_WIDTH,
				 -0.5*GSGL_PAPER_HEIGHT, 0.5*GSGL_PAPER_HEIGHT,
				 eyepos, eyepos + GSGL_VIEW_DEPTH);
			glTranslatef (-0.5*GSGL_PAPER_WIDTH, -0.5*GSGL_PAPER_HEIGHT,
				      -eyepos);
		} else if (gsgl_inq_projectiontype (p) == GSGL_FRUSTUM) {
			glFrustum (-0.5*GSGL_PAPER_WIDTH, 0.5*GSGL_PAPER_WIDTH, 
				   -0.5*GSGL_PAPER_HEIGHT, 0.5*GSGL_PAPER_HEIGHT,
				   eyepos, eyepos+GSGL_VIEW_DEPTH);
			glTranslatef(-0.5*GSGL_PAPER_WIDTH, -0.5*GSGL_PAPER_HEIGHT, 
				     -eyepos);
		}
		glMatrixMode(GL_MODELVIEW);
		glLineWidth (1.0f);

		glBegin (GL_LINE_LOOP);
		gsglcolor = gsgl_inq_config_color (p->library);
		glColor3f (gsglcolor->box[0], gsglcolor->box[1], gsglcolor->box[2]);
		glVertex2f (Xstart*gsgl_inq_x_ratio (p->win), (gsgl_inq_window_height (p->win)-Ystart+1)*gsgl_inq_y_ratio (p->win));
		glVertex2f (x*gsgl_inq_x_ratio (p->win), (gsgl_inq_window_height (p->win)-Ystart+1)*gsgl_inq_y_ratio (p->win));
		glVertex2f (x*gsgl_inq_x_ratio (p->win), (gsgl_inq_window_height (p->win)-y+1)*gsgl_inq_y_ratio (p->win));
		glVertex2f (Xstart*gsgl_inq_x_ratio (p->win), (gsgl_inq_window_height (p->win)-y+1)*gsgl_inq_y_ratio (p->win));
		glEnd ();
		glBegin (GL_POLYGON);
		glColor4f (gsglcolor->box[0], gsglcolor->box[1], gsglcolor->box[2], 0.3);
		glVertex2f (Xstart*gsgl_inq_x_ratio (p->win), (gsgl_inq_window_height (p->win)-Ystart+1)*gsgl_inq_y_ratio (p->win));
		glVertex2f (x*gsgl_inq_x_ratio (p->win), (gsgl_inq_window_height (p->win)-Ystart+1)*gsgl_inq_y_ratio (p->win));
		glVertex2f (x*gsgl_inq_x_ratio (p->win), (gsgl_inq_window_height (p->win)-y+1)*gsgl_inq_y_ratio (p->win));
		glVertex2f (Xstart*gsgl_inq_x_ratio (p->win), (gsgl_inq_window_height (p->win)-y+1)*gsgl_inq_y_ratio (p->win));
		glEnd ();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
		glPopAttrib();
	}

	glEndList ();
	list_utility->gllist = gllist_new;
	glDeleteLists (gllist_old, 1);
	gsgl_redraw (p);
}

void gsgl_utility_clean (gsglPage *p)
{
	GLuint gllist_old, gllist_new;
	gsglDisplaylist *list_utility;

	list_utility = gsgl_displaylist_inq_list_of_utility (p);
	gllist_old = list_utility->gllist;
	gllist_new = glGenLists(1);
	glNewList (gllist_new, GL_COMPILE);
	glEndList ();
	list_utility->gllist = gllist_new;
	glDeleteLists (gllist_old, 1);
	gsgl_redraw (p);
}
