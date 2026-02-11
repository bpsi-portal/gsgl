#include "gsaf.h"
#include "gsgl.h"
#include "base.h"
#include "fileformat.h"
#include "displaylist.h"
#include "mem_list_read.h"
#include "xwin.h"
#include "ogl.h"
#include "matrix.h"
#include "utils.h"
#include "drv_gl.h"
#include "drv_file.h"
#include "drv_wrapper.h"
#include "tcpip.h"
#include "mem_list.h"
#include "callback.h"
#include "clientstate.h"
#include "anime.h"
#ifdef USE_QT6
#include "qt_xwin.h"
#endif

/*******************************************************************/
/*   GSAF/OpenGL で実行された命令は display list という形で        */
/*   memory に保管される．GSGL の display list は OpenGL の        */
/*   機能として備わっている display list の他，GSAF/OpenGL の      */
/*   FileFormatをリスト構造で繋いだものも一部使っている.           */
/*   display list は構造体 gsglDisplaylist のリスト構造となって    */
/*   おり，1つの構造体に1つの OpenGLのdiplaylist または FileFormat */
/*   の display list が割り当てられる．FileFormat はさらに         */
/*   サイズが GSGL_FILEFORMAT_SIZE の unsigned int の配列がリスト  */
/*   構造で繋がれている                                            */
/*   listのtypeは,                                                 */
/*       begin_header -> ... -> end_header -> ...                  */
/*   と続く．                                                      */
/*******************************************************************/

#define GSGL_DEBUG(x)

static int gsgl_displaylist_inq_projection_num (gsglPage *);
static int gsgl_displaylist_inq_modelview_num (gsglPage *);
static int gsgl_displaylist_inq_list_num (gsglPage *);
static gsglDisplaylist * gsgl_displaylist_begin_ (int type, gsglboolean execute, gsglPage *p);
static void gsgl_displaylist_end_ (gsglDisplaylist *, gsglPage *p);

gsglDisplaylist * gsgl_displaylist_inq_list_of_utility (gsglPage *p)
{
	return p->dsp->List_utility;
}

gsglDisplaylist * gsgl_displaylist_inq_list_of_projection (gsglPage *p)
{
	return p->dsp->List_projection;
}

gsgllist * gsgl_displaylist_inq_list_of_list_lighting_enable (gsglPage *p)
{
	return p->dsp->List_of_list_lighting_enable;
}

static int gsgl_displaylist_inq_type_ (gsgllist *list0)
{
	gsgllist *list;

	list = gsgl_list_last (list0);
	if (DISPLAYLIST(list)->closed)
		return GSGL_LIST_NULL;
	else
		return DISPLAYLIST(list)->type;
}
  
int gsgl_displaylist_inq_type (gsglPage *p)
{
	return gsgl_displaylist_inq_type_ (p->dsp->List0);
}

void gsgl_displaylist_begin_guiitems (gsglPage *p)
{
	gsgl_displaylist_begin (GSGL_LIST_GUIITEMS, p);
	p->dsp->Guiitems = gsgl_list_append (p->dsp->Guiitems, gsgl_displaylist_inq_last (p)->data);
}

gsgllist * gsgl_displaylist_inq_guiitems (gsglPage *p)
{
	return p->dsp->Guiitems;
}

void gsgl_displaylist_copy (gsglDisplaylist *from, gsglDisplaylist *to)
{
	to->type = from->type;
	to->name = from->name;
	to->gllist = from->gllist;
	to->mem_list = from->mem_list;
	to->size = from->size;
}

static void gsgl_displaylist_move (gsglDisplaylist *from, gsglDisplaylist *to)
{
	to->type = from->type;
	to->name = from->name;
	to->gllist = from->gllist;
	from->gllist = 0;
	to->size = from->size;

	if (to->size > 0) {
		to->mem_list = from->mem_list;
		from->mem_list = NULL;
	}
}

/**********************************/
/*     displaylist の初期化       */
/*********************************/
void gsgl_displaylist_init (gsglPage *p)
{
	p->dsp->List0 = NULL;
	p->dsp->List_utility = NULL;
	p->dsp->List_projection = NULL;
	p->dsp->List_of_list_lighting_enable = NULL;
	p->dsp->Guiitems = NULL;

	p->dsp->Use_list = GSGL_TRUE;
	p->dsp->Objects_name = 0;

	gsgl_displaylist_begin (GSGL_LIST_PAGEINFO, p);

	GSGL_DEBUG(fprintf (stderr, "init\n"));
}

/************************************************/
/*     displaylist のリスト構造の始めを返す     */
/************************************************/
gsgllist * gsgl_displaylist_inq_start (gsglPage *p)
{
	return p->dsp->List0;
}

/******************************************************/
/*     はじめの構造体からたどった最後の構造を返す     */
/******************************************************/
gsgllist * gsgl_displaylist_inq_last (gsglPage *p)
{
	return gsgl_list_last (p->dsp->List0);
}

/*******************************************************/
/*     GSGL_LIST_END_HEADERのリストを返す          */
/*******************************************************/
static gsgllist * gsgl_displaylist_inq_end_header (gsglPage *p)
{
	gsgllist *list;

	list = gsgl_displaylist_inq_start (p);
	while (list) {
		if (DISPLAYLIST(list)->type == GSGL_LIST_END_HEADER) {
			return list;
		}
		list = list->next;
	}
	return NULL;
}

/*******************************************************/
/*     GSGL_LIST_END_HEADERの次のリストを返す          */
/*******************************************************/
gsgllist * gsgl_displaylist_inq_after_header (gsglPage *p)
{
	gsgllist *list;

	list = gsgl_displaylist_inq_end_header (p);
	if (list) 
		return list->next;
	else
		return NULL;
}

/******************************************************/
/*       与えられた name を持つリストを返す           */
/******************************************************/
gsgllist * gsgl_displaylist_inq_by_name (GLuint name0, gsglPage *p)
{
	gsgllist *list;
	
	list = gsgl_displaylist_inq_start (p);
	while (list->next) {
		if (DISPLAYLIST(list)->name == name0) return list; 
		list = list->next;
	}
	return NULL;
}

/***********************************************/
/*                                             */
/***********************************************/
void gsgl_disable_displaylist (gsglPage *p)
{
	p->dsp->Use_list = GSGL_FALSE;
}

void gsgl_enable_displaylist (gsglPage *p)
{
	p->dsp->Use_list = GSGL_TRUE;
}

gsglboolean gsgl_inq_displaylist_is_enabled (gsglPage *p)
{
	return p->dsp->Use_list;
}

/***********************************************/
/*   新しい displaylist の allocate と初期化   */
/***********************************************/
gsglDisplaylist * gsgl_displaylist_new (gsglPage *p)
{
	gsglDisplaylist *list = NULL;
	
	if (p->dsp->Use_list) {
		list = (gsglDisplaylist *) gsgl_malloc (sizeof (gsglDisplaylist));
		list->type = GSGL_LIST_INVALID;
		list->name = 0;
		list->gllist = 0;
		list->mem_list = NULL;
		list->size = 0;
		list->closed = GSGL_FALSE;
		list->color_gui = NULL;
		list->matrix_stack = NULL;
		list->matrix_gui = NULL;
		list->matrix = NULL;
		list->color_restore = NULL;
	} else {
		list = NULL;
	}
	return list;
}

/*******************************************/
/*  タイプ type の displaylist を開始する  */
/*******************************************/
void gsgl_displaylist_begin (int type, gsglPage *p)
{
	gsgllist *list;
	gsglDisplaylist *d;
	
	list = gsgl_list_last (p->dsp->List0);
	if (list && list->data && !DISPLAYLIST(list)->closed) {
		if (DISPLAYLIST(list)->type == type) return;
		else
			gsgl_displaylist_end (p);
	}
	d = gsgl_displaylist_begin_ (type, GSGL_TRUE, p);
	if (d)
		p->dsp->List0 = gsgl_list_append (p->dsp->List0, d);
}

static gsglDisplaylist * gsgl_displaylist_begin_ (int type, gsglboolean execute, gsglPage *p)
{
	gsglDisplaylist *d;
	float mat1[16], mat2[16], mat3[16];

	if (!p->dsp->Use_list) return NULL;
	if (type == GSGL_LIST_NULL)
		return NULL;

#ifdef USE_QT6
	gsgl_qt_ensure_current ();
#endif
	GSGL_DEBUG(fprintf (stderr, "begin (type = %d)\n", type));
	d = gsgl_displaylist_new (p);
	if (!d)
		return NULL;

	if (type == GSGL_LIST_GUIITEMS) {
		d->matrix_stack = gsgl_displaylist_new (p);
		d->matrix_stack->type = GSGL_LIST_GL_ONLY;
		if (gsgl_inq_drv_gl (p)) {
			d->matrix_stack->gllist = glGenLists(1);
			glNewList (d->matrix_stack->gllist, GL_COMPILE_AND_EXECUTE);
			glMatrixMode (GL_PROJECTION);
			glPushMatrix ();
			
			gsgl_inq_matrix_orthogonal 
				(-0.5*GSGL_PAPER_WIDTH, 0.5*GSGL_PAPER_WIDTH,
				 -0.5*GSGL_PAPER_HEIGHT, 0.5*GSGL_PAPER_HEIGHT,
				 GSGLBASIC_EYEPOSITION, 
				 GSGLBASIC_EYEPOSITION+GSGL_VIEW_DEPTH, mat1);
			
			gsgl_inq_matrix_translate 
				(-0.5*GSGL_PAPER_WIDTH, -0.5*GSGL_PAPER_HEIGHT, 
				 -GSGLBASIC_EYEPOSITION-EYEPOS_DELTA, mat2);
			gsgl_mat16_mult_mat16 (mat1, mat2, mat3);
			
			glLoadMatrixf (mat3);
			glMatrixMode (GL_MODELVIEW);				
			glPushMatrix ();
			glLoadIdentity ();
			glEndList();
		}
	}

	if (type == GSGL_LIST_GL_ONLY || type == GSGL_LIST_GL_IGNORE) {
		
		d->type = type;
		d->gllist = gsgl_drv_glGenLists (1, p);
		if (execute)
			gsgl_drv_glNewList (d->gllist, GL_COMPILE_AND_EXECUTE, p);
		else
			gsgl_drv_glNewList (d->gllist, GL_COMPILE, p);

	} else if (type == GSGL_LIST_BEGIN_HEADER || type == GSGL_LIST_END_HEADER ||
		   type == GSGL_LIST_2DOBJ_FILEONLY || type == GSGL_LIST_3DOBJ_FILEONLY ||
		   type == GSGL_LIST_PAGEINFO) {

		d->type = type;
		gsgl_mem_list_start (p);

	} else {

		d->type = type;
		gsgl_mem_list_start (p);
		d->gllist = gsgl_drv_glGenLists (1, p);
		if (execute)
			gsgl_drv_glNewList (d->gllist, GL_COMPILE_AND_EXECUTE, p);
		else
			gsgl_drv_glNewList (d->gllist, GL_COMPILE, p);
			
	}

	if (type == GSGL_LIST_2DOBJ_FILEONLY || type == GSGL_LIST_3DOBJ_FILEONLY ||
	    type == GSGL_LIST_2DOBJ || type == GSGL_LIST_3DOBJ) {

		d->name = ++(p->dsp->Objects_name);

	}

	return d;
}

void gsgl_displaylist_end (gsglPage *p)
{
	gsgllist *list;
	
	list = gsgl_list_last(p->dsp->List0);
	if (list)
		gsgl_displaylist_end_ (list->data, p);
}

static void gsgl_displaylist_end_ (gsglDisplaylist *d, gsglPage *p)
{
	int type;

	if (d->closed) {
		type = GSGL_LIST_NULL;
	} else {
		type = d->type;
	}
	GSGL_DEBUG(fprintf (stderr, "end (type = %d)\n", type));
	if (type == GSGL_LIST_NULL || !p->dsp->Use_list) return;

	if (type == GSGL_LIST_GL_ONLY || type == GSGL_LIST_GL_IGNORE) {
		gsgl_drv_glEndList (p);
	} else if (type == GSGL_LIST_BEGIN_HEADER || type == GSGL_LIST_END_HEADER ||
		   type == GSGL_LIST_2DOBJ_FILEONLY ||type == GSGL_LIST_3DOBJ_FILEONLY ||
		   type == GSGL_LIST_PAGEINFO) {
		gsgl_mem_list_inq_size_and_data (d, p);
	} else {
		gsgl_mem_list_inq_size_and_data (d, p);
		gsgl_drv_glEndList (p);
	}
	d->closed = GSGL_TRUE;

	if (type == GSGL_LIST_GUIITEMS) {
		d->color_restore = gsgl_displaylist_new (p);
		d->color_restore->type = GSGL_LIST_GL_ONLY;
		if (gsgl_inq_drv_gl (p)) {
			d->color_restore->gllist = glGenLists(1);
			glNewList (d->color_restore->gllist, GL_COMPILE_AND_EXECUTE);
			glMatrixMode (GL_PROJECTION);
			glPopMatrix ();
			glMatrixMode (GL_MODELVIEW);				
			glPopMatrix ();
			glEndList();
		}
	}

}

/*******************************************************/
/*     start から始まる displaylist のリストを呼ぶ     */
/*******************************************************/
void gsgl_lists_call_template(gsglDisplaylist *list, gsglPage *p, void *data, 
			      void (*action)(gsglDisplaylist *, gsglPage *, void *))
{
	if (list->color_gui)
		action(list->color_gui, p, data);
	if (list->matrix_stack)
		action(list->matrix_stack, p, data);
	if (list->matrix_gui)
		action(list->matrix_gui, p, data);
	if (list->matrix)
		action(list->matrix, p, data);
	
	action(list, p, data);
	
	if (list->color_restore)
		action(list->color_restore, p, data);
}

static void call_lists_internal (gsglDisplaylist *list, gsglPage *p, void *make_name1)
{
	int type;
	GLuint gllist;
	gsglboolean *make_name = (gsglboolean *) make_name1;

	type = list->type;
	if (*make_name) {
		glLoadName (list->name);
	}
	gllist = list->gllist;

	GSGL_DEBUG(fprintf(stderr,"gllist = %d type = %d pointer= %p\n", gllist, list->type, list));
	if (gllist > 0) glCallList (gllist);
	else {
		GSGL_DEBUG(fprintf(stderr,"gllist = %d type = %d\n", gllist, list->type));
		gsgl_mem_list_read (list->mem_list, list->size, p);
	}
}

void gsgl_lists_call (gsgllist *start, gsglboolean make_name, gsglPage *p)
{
	gsgllist *list;
	
	list = start;
	while (list) {
		gsgl_lists_call_template(list->data, p, &make_name, call_lists_internal);
		list = list->next;
	}
}

/********************************/
/*     headerのリストを呼ぶ     */
/********************************/
void gsgl_lists_call_header (gsglPage *p)
{
	gsgllist *list;
	GLuint gllist;
	gsglDisplaylist *d;
	int type;
	gsglboolean in_header = GSGL_FALSE;
	
	list = gsgl_displaylist_inq_start (p);
	while (list) {
		d = list->data;
		type = d->type;
		if (type == GSGL_LIST_BEGIN_HEADER) in_header = GSGL_TRUE;
		if (type == GSGL_LIST_END_HEADER) return;
		if (in_header) {
			gllist = d->gllist;
			GSGL_DEBUG(fprintf(stderr,"gllist = %d type = %d\n", gllist, d->type));
			if (gllist > 0) glCallList (gllist);
			else gsgl_mem_list_read (d->mem_list, d->size, p);
		}
		list = list->next;
	}
}

void gsgl_displaylist_free_data (gsgllist *list)
{
	unsigned int gllist;
	unsigned short *mem;

	gllist = DISPLAYLIST(list)->gllist;
	if (gllist)
		glDeleteLists (gllist, 1);
	mem = DISPLAYLIST(list)->mem_list;
	if (mem)
		free (mem);
}

/**********************************************************************************/
/* header_end 以降にはじめて現れるprojection matrix以降 の displaylist を消去する */
/**********************************************************************************/
void gsgl_clear (gsglPage *p)
{
	gsgllist *list;
	gsgllist *item1, *item2;
	gsgllist *backup = NULL;

	if (gsgl_inq_mode (p->library) == GSGL_MODE_TCPIP) {
		gsgl_tcp_push (GSGL_TCP_GUI_CLEAR_LIST, p);
	}

	if (gsgl_inq_anime_is_used (p)) {
		gsgl_anime_finish_every_page (p);
		gsgl_init_anime_every_page (p);
	}

	if (p->dsp->Guiitems) {
		/* guiitems と同じ大きさの list を2つ作成 */
		item1 = p->dsp->Guiitems;
		while (item1) {
			backup = gsgl_list_append (backup, gsgl_displaylist_new(p));
			item1 = item1->next;
		}

		item1 = p->dsp->Guiitems;
		item2 = backup;
		while (item1) {			
			gsgl_displaylist_move (item1->data, item2->data);

			item1 = item1->next;
			item2 = item2->next;
		}
	}

	list = gsgl_displaylist_inq_after_header (p);
	if (!list) 
		return;
	while (list) {
		if (p->dsp->List_is_remade && DISPLAYLIST(list)->type == GSGL_LIST_GL_ONLY) { 
			/* gsgl_list_end_headerの後にprojection matrixを*/
                        /* pushするlistが追加されている */
			
			gsgl_displaylist_free_data (list);
			gsgl_free (list->data);
			list = gsgl_list_connect (list->prev, list->next);
		} else if (DISPLAYLIST(list)->type == GSGL_LIST_PROJECTION) {
			break;
		}
		list = list->next;
	}

	if (list) {
		gsgl_displaylist_end (p);
		gsgl_gui_pointer (p, GSGL_FALSE);
		gsgl_displaylist_clear (list);
		/* ここで，pages の初期化状態と同じ状態をつくる必要がある? */
		p->dsp->List_is_remade = GSGL_FALSE;
		gsgl_init_matrix_cache (p);
		gsgl_set_lighting_disable (p);
		gsgl_clientstate_disable (p);
		gsgl_init_info_every_page (p);

		if (gsgl_inq_drv_gl(p)) {
			gsgl_enable_antialiasing (p->win); /* FIX ME */
		}

		if (p->dsp->Guiitems) {

			item1 = p->dsp->Guiitems;
			item2 = backup;
			while (item1) {				
				gsgl_displaylist_move (item2->data, item1->data);
				
				item1 = item1->next;
				item2 = item2->next;
			}
			item1 = p->dsp->Guiitems;
			while (item1) {
				p->dsp->List0 = gsgl_list_append (p->dsp->List0, item1->data);
				item1 = item1->next;
			}
		}
	}
	backup = NULL;
	GC_gcollect ();
}

/*********************************************************/
/*  last の"次"以降 の mem list と gl list を消去する    */
/*  ただし gsglDisplaylist は free しない                */
/*********************************************************/
void gsgl_displaylist_clear (gsgllist *last)
{
	gsgllist *list, *next;

	list = last->next;
	while (list) {
		gsgl_displaylist_free_data (list);
		next = list->next;
		gsgl_list_delete_link (list);
		list = next;
	}
	last->next = NULL;
	GC_gcollect ();
}

/****************************************/
/*   すべての displaylist を消去する    */
/****************************************/
void gsgl_lists_delete (gsglPage *p)
{
	gsgllist *list, *next;

	gsgl_mem_list_free_buffer (p);

	list = gsgl_displaylist_inq_start (p);
	while (list) {
		gsgl_displaylist_free_data (list);
		gsgl_free (list->data);

		next = list->next;

		gsgl_list_delete_link (list);
		gsgl_free (list);

		list = next;
	}
}

static void gsgl_displaylist_make_objects_list (gsglDisplaylist *list0, GLfloat matrix[16], GLfloat rgba[4], gsglboolean use_gl, gsglPage *p)
{
	list0->color_gui = gsgl_displaylist_new (p); /* GUIで選択されたときの色を作用させる */
	list0->matrix_stack = gsgl_displaylist_new (p); /* GUIで変換したいときはpop & push,
							  そうでなければ loadidentity */
	list0->matrix_gui = gsgl_displaylist_new (p); /* GUIで作用されるmatrixを保持 */
	list0->matrix = gsgl_displaylist_new (p); /* 元々のmatrixを保持 */

	list0->color_gui->type = GSGL_LIST_GL_IGNORE;
	list0->matrix_stack->type = GSGL_LIST_GL_ONLY;
	if (use_gl) {
		list0->matrix_stack->gllist = glGenLists(1);
		glNewList (list0->matrix_stack->gllist, GL_COMPILE);
		glLoadIdentity ();
		glEndList();
	}

	list0->matrix_gui->type = GSGL_LIST_GL_ONLY;
	list0->matrix_gui->gllist = 0;

	list0->matrix->type = GSGL_LIST_MODELVIEW;
	if (use_gl) {
		list0->matrix->gllist = glGenLists(1);
		glNewList (list0->matrix->gllist, GL_COMPILE);
		glMultMatrixf (matrix);
		glEndList();
	}

	list0->color_restore = gsgl_displaylist_new (p); /* GUIで選択されたときの色をうち消す */
	list0->color_restore->type = GSGL_LIST_GL_IGNORE;
	if (use_gl) {
		list0->color_restore->gllist = glGenLists(1);
		glNewList (list0->color_restore->gllist, GL_COMPILE);
		glColor4fv (rgba);
		glEndList();
	}
}

gsglDisplaylist * gsgl_displaylist_inq_gui_color (gsglDisplaylist * list0)
{
	return list0->color_gui;
}

gsglDisplaylist * gsgl_displaylist_inq_gui_matrix (gsglDisplaylist * list0)
{
	return list0->matrix_gui;
}

gsglDisplaylist * gsgl_displaylist_inq_pre_stack (gsglDisplaylist * list0)
{
	return list0->matrix_stack;
}

void gsgl_displaylist_init_remade (gsglPage *p)
{
	p->dsp->List_is_remade = GSGL_FALSE;
}

gsglboolean gsgl_displaylist_inq_remade (gsglPage *p)
{
	return p->dsp->List_is_remade;
}

void gsgl_displaylist_remake (gsglPage *p)
{
	gsgllist *list;
	gsglDisplaylist *d;
	int dim;
	GLfloat rgba[4];
	GLfloat matrix[16];
	gsglboolean use_gl = GSGL_TRUE;
	
	if (p->dsp->List_is_remade || !p->dsp->Use_list) return; /*すでに display list が再構成されている*/
	p->dsp->List_is_remade = GSGL_TRUE;
	if (gsgl_inq_i_am_a_server (p->library))
		gsgl_tcp_remake_tell_to_client (p->library);
	if (gsgl_inq_mode (p->library) == GSGL_MODE_TCPIP)
		use_gl = GSGL_FALSE;

	if (use_gl) {
		gsgl_init_matrix_cache (p);
		gsgl_init_gl_every_page (p);
	}
	
	/* gsgl_list_end_header を探す */
	list = gsgl_displaylist_inq_end_header (p);

	/* gsgl_list_end_headerの後にprojection matrixをpushするlistの追加 */
	p->dsp->List_projection = gsgl_displaylist_new (p);
	p->dsp->List_projection->type = GSGL_LIST_GL_ONLY;
	if (use_gl) {
		p->dsp->List_projection->gllist = glGenLists(1);
		glNewList (p->dsp->List_projection->gllist, GL_COMPILE);
		glEndList ();
	}
	gsgl_list_insert (list, p->dsp->List_projection);
	
	/* lighting */
	p->dsp->List_of_list_lighting_enable = NULL;
	list = gsgl_displaylist_inq_after_header (p);
	if (gsgl_inq_lighting_is_enabled (p)) {
		while (list) {
			if (DISPLAYLIST(list)->type == GSGL_LIST_LIGHTING_ENABLE) {
				p->dsp->List_of_list_lighting_enable = gsgl_list_append (p->dsp->List_of_list_lighting_enable, list->data);
			}
			list = list->next;
		}
	} else {
	/* 2d->3d, 3d->2dの間にgl_list_lighting_{enable,disable}を加える */		
	    dim = 2;
	    while (list) {
		if (dim == 2) {
		    if (DISPLAYLIST(list)->type == GSGL_LIST_3DOBJ || 
			DISPLAYLIST(list)->type == GSGL_LIST_3DOBJ_FILEONLY) {
			    
			    d = gsgl_displaylist_begin_ (GSGL_LIST_LIGHTING_ENABLE, GSGL_FALSE, p);
			    gsgl_file_lighting_enable (gsgl_list_push, p);
			    gsgl_displaylist_end_ (d, p);
			    
			    gsgl_list_insert (list->prev, d);
			    
			    dim = 3;
			    
			    p->dsp->List_of_list_lighting_enable = gsgl_list_append (p->dsp->List_of_list_lighting_enable, d);
		    }
		} else /*if (dim == 3)*/ {
		    if (DISPLAYLIST(list)->type == GSGL_LIST_2DOBJ ||
			DISPLAYLIST(list)->type == GSGL_LIST_2DOBJ_FILEONLY) {
			    
			    d = gsgl_displaylist_begin_ (GSGL_LIST_LIGHTING_DISABLE, GSGL_FALSE, p);
			    if (use_gl) {
				    gsgl_gl_lighting_disable ();
			    }
			    gsgl_file_lighting_disable (gsgl_list_push, p);
			    gsgl_displaylist_end_ (d, p);
			    
			    gsgl_list_insert (list->prev, d);
				
			    dim = 2;
		    }
		}
		list = list->next;
	    }
	}

	/* 最後に lighting_disable */
	list = gsgl_displaylist_inq_last (p);

	d = gsgl_displaylist_begin_ (GSGL_LIST_LIGHTING_DISABLE, GSGL_FALSE, p);
	if (use_gl) {
		gsgl_gl_lighting_disable ();
	}
	gsgl_file_lighting_disable (gsgl_list_push, p);
	gsgl_displaylist_end_ (d, p);

	gsgl_list_append (list, d);

	/* utility用のlist */
	p->dsp->List_utility = gsgl_displaylist_begin_ (GSGL_LIST_GL_IGNORE, GSGL_FALSE, p);
	gsgl_displaylist_end_ (p->dsp->List_utility, p);
	gsgl_list_append (gsgl_displaylist_inq_last (p), p->dsp->List_utility);

	/* objectsの前にcolor, matrixを操作するためのlistをつなげる */
	list = gsgl_displaylist_inq_after_header (p);
	while (list) {
		if (DISPLAYLIST(list)->type == GSGL_LIST_STATUS ||
		    DISPLAYLIST(list)->type == GSGL_LIST_MODELVIEW) {
			if (use_gl) {
				if (DISPLAYLIST(list)->gllist > 0)
					glCallList (DISPLAYLIST(list)->gllist);
			}
		}
		if (DISPLAYLIST(list)->type == GSGL_LIST_MODELVIEW) {

			gsgl_displaylist_free_data (list);
			gsgl_free (list->data);
			list = gsgl_list_connect (list->prev, list->next);
			
		} else if (DISPLAYLIST(list)->type == GSGL_LIST_2DOBJ ||
			   DISPLAYLIST(list)->type == GSGL_LIST_3DOBJ ||
			   DISPLAYLIST(list)->type == GSGL_LIST_2DOBJ_FILEONLY || 
			   DISPLAYLIST(list)->type == GSGL_LIST_3DOBJ_FILEONLY) {
			
			if (use_gl) {
				glGetFloatv (GL_MODELVIEW_MATRIX, matrix);
				glGetFloatv (GL_CURRENT_COLOR, rgba);
				
				gsgl_displaylist_make_objects_list (list->data, matrix, rgba, use_gl, p);
			} else {
				gsgl_displaylist_make_objects_list (list->data, matrix, rgba, use_gl, p);
			}
		}
		list = list->next;
	}
}

static void remake_for_save_internal (gsglDisplaylist *list, gsglPage *p, void *data)
{
	GLfloat matrix[16];

	if (list->type == GSGL_LIST_STATUS || list->type == GSGL_LIST_MODELVIEW ||
	    list->type == GSGL_LIST_PROJECTION || list->type == GSGL_LIST_GL_ONLY) {
		if (list->gllist > 0) glCallList (list->gllist);
	}

	if (list->type == GSGL_LIST_MODELVIEW) {
		
		glGetFloatv (GL_MODELVIEW_MATRIX, matrix);
		
		gsgl_mem_list_start (p);
		gsgl_file_modelview_matrix (matrix, gsgl_list_push, p);		
		gsgl_mem_list_inq_size_and_data (list, p);
		
	} else if (list->type == GSGL_LIST_PROJECTION) {
		
		glGetFloatv (GL_PROJECTION_MATRIX, matrix);
		
		gsgl_mem_list_start (p);
		gsgl_file_projection_matrix (matrix, gsgl_list_push, p);
		gsgl_mem_list_inq_size_and_data (list, p);
	}
}

void gsgl_displaylist_remake_for_save (gsglPage *p)
{
	gsgllist *list;

	if (!p->dsp->List_is_remade) return; /*display list が再構成されていない*/
	
	gsgl_init_matrix_cache (p);
	gsgl_init_gl_every_page (p);
	
	list = gsgl_displaylist_inq_start (p);
	/* projection matrixおよびobjectsの前のmodelview matrixのfileformatを更新する */
	while (list) {
		gsgl_lists_call_template(list->data, p, NULL, remake_for_save_internal);
		list = list->next;
	}
}

/*static void print_matrix (gsglMem_List * format)
  {
  unsigned int *buf;
  
  buf = format->ibuffer;
  buf++;
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
  }
  
  static void print_matrix_s (float * mat)
  {
  printf ("from server\n");
  printf ("%f %f %f %f\n",   mat[0], mat[4], mat[8], mat[12]);
  printf ("%f %f %f %f\n",   mat[1], mat[5], mat[9], mat[13]);
  printf ("%f %f %f %f\n",   mat[2], mat[6], mat[10], mat[14]);
  printf ("%f %f %f %f\n\n", mat[3], mat[7], mat[11], mat[15]);
  }*/

typedef struct _save_tcpip
{
	float *projection;
	float *modelview;
	int *p;
	int *m;
} save_tcpip;

static void remake_for_save_tcpip (gsglDisplaylist *list, gsglPage *page, void *data1)
{
	GLfloat *matrix;
	save_tcpip *data = (save_tcpip *) data1;
	float *projection = data->projection;
	float *modelview = data->modelview;
	int *p = data->p;
	int *m = data->m;

	if (list->type == GSGL_LIST_MODELVIEW) {
		
		/*printf ("client\n");
		  if (list->size == 33)
		  print_matrix (list->gsglMem_ListBuffer);
		  else 
		  printf ("size != 33\n");*/
		
		gsgl_mem_list_start (page);
		matrix = &modelview[(*m)*16];
		/*print_matrix_s (matrix);*/
		(*m)++;
		gsgl_file_modelview_matrix (matrix, gsgl_list_push, page);
		gsgl_mem_list_inq_size_and_data (list, page);
		
	} else if (list->type == GSGL_LIST_PROJECTION) {
		
		/*printf ("client\n");
		  if (list->size == 33)
		  print_matrix (list->gsglMem_ListBuffer);
		  else 
		  printf ("size != 33\n");*/
		
		gsgl_mem_list_start (page);
		matrix = &projection[(*p)*16];
		/*print_matrix_s (matrix);*/
		(*p)++;
		gsgl_file_projection_matrix (matrix, gsgl_list_push, page);
		gsgl_mem_list_inq_size_and_data (list, page);
	}
}

void gsgl_displaylist_remake_for_save_tcpip (gsglPage *page)
{
	gsgllist *list;
	gsglboolean remade, lighting;
	int num_list;
	int num_projection;
	int num_modelview;
	float *projection;
	float *modelview;
	int m = 0, p = 0;
	save_tcpip *data;

	if (gsgl_inq_mode (page->library) != GSGL_MODE_TCPIP) return;
	if (!page->dsp->List_is_remade) return; /*display list が再構成されていない*/
	
	gsgl_tcp_receive_list_info_from_server (&remade, &lighting, &num_list, &num_projection, &num_modelview, &projection, &modelview, page);
	
	/*printf ("from server %d %d %d %d %d\n", remade, lighting, num_list, 
	  num_projection, num_modelview);*/

	/*printf ("client %d %d %d %d\n", gsgl_displaylist_inq_remade (page),
	  gsgl_displaylist_inq_list_num (page),
	  gsgl_displaylist_inq_projection_num (page),
	  gsgl_displaylist_inq_modelview_num (page));*/

	if (lighting)
		gsgl_set_lighting_enable (page);
	else
		gsgl_set_lighting_disable (page);

	if (num_projection != gsgl_displaylist_inq_projection_num (page)) {
		gsgl_warning ("gsgl_displaylist_remake_for_save_tcpip", "the number of displaylist of projection matrix is differnt between client(%d) and server(%d)", gsgl_displaylist_inq_projection_num (page), num_projection);
		return;
	}
	if (num_modelview != gsgl_displaylist_inq_modelview_num (page)) {
		gsgl_warning ("gsgl_displaylist_remake_for_save_tcpip", "the number of displaylist of modelview matrix is differnt between client and server");
		return;
	}

	list = gsgl_displaylist_inq_start (page);

	data = gsgl_malloc (sizeof(save_tcpip));
	data->projection = projection;
	data->modelview = modelview;
	data->p = &p;
	data->m = &m;

	/* projection matrixおよびobjectsの前のmodelview matrixのfileformatを更新する */
	while (list) {
		/*printf ("type=%d\n", list->type);*/
		gsgl_lists_call_template(list->data, page, data, remake_for_save_tcpip);
		list = list->next;
	}
}

static void count_list (gsglDisplaylist *list, gsglPage *p, void *data1)
{
	int *i = (int *) data1;
	if (list != NULL)
		(*i)++;
}

static int gsgl_displaylist_inq_list_num (gsglPage *p)
{
	gsgllist *list;
	int i = 0;
	
	list = gsgl_displaylist_inq_start (p);
	while (list) {
		gsgl_lists_call_template(list->data, p, &i, count_list);
		list = list->next;
	}
	return i;	
}

static void count_projection (gsglDisplaylist *list, gsglPage *p, void *data1)
{
	int *i = (int *) data1;
	if (list != NULL && list->type == GSGL_LIST_PROJECTION)
		(*i)++;
}

static int gsgl_displaylist_inq_projection_num (gsglPage *p)
{
	gsgllist *list;
	int i = 0;
	
	list = gsgl_displaylist_inq_start (p);
	while (list) {
		gsgl_lists_call_template(list->data, p, &i, count_projection);
		list = list->next;
	}
	return i;	
}

static void count_modelview (gsglDisplaylist *list, gsglPage *p, void *data1)
{
	int *i = (int *) data1;
	if (list != NULL && list->type == GSGL_LIST_MODELVIEW)
		(*i)++;
}

static int gsgl_displaylist_inq_modelview_num (gsglPage *p)
{
	gsgllist *list;
	int i = 0;
	
	list = gsgl_displaylist_inq_start (p);
	while (list) {
		gsgl_lists_call_template(list->data, p, &i, count_modelview);
		list = list->next;
	}
	return i;	
}

static void gsgl_displaylist_get_matrix (gsglDisplaylist * list, unsigned int * matrix)
{
	unsigned short *mem;
	int i;

	if (!list || !list->mem_list) {
		gsgl_warning ("gsgl_displaylist_get_matrix", "!list or !list->mem_list");
		return;
	}
	if (list->size != 1 + 16 * 2) {
		gsgl_warning ("gsgl_displaylist_get_matrix", "buffer size != 33");
		return;
	}

	mem = list->mem_list;
	mem++;
	for (i=0; i<32; i++) {
		gsgl_mem_list_read_token (mem, i, &matrix[i]);
	}
	return;
}

typedef struct _st_proj
{
	int i;
	unsigned int *matrix;
} st_proj;

static void inq_projection (gsglDisplaylist *list, gsglPage *p, void *data1)
{
	st_proj * data = (st_proj *) data1;
	unsigned int *matrix = data->matrix;

	if (list && list->type == GSGL_LIST_PROJECTION) {
		gsgl_displaylist_get_matrix (list, &matrix[data->i*32]);
		data->i++;
	}
}

static unsigned int * gsgl_displaylist_inq_projection_matrixs (gsglPage *p)
{
	gsgllist *list;
	st_proj *data;
	int i;

	data = (st_proj *) gsgl_malloc (sizeof(st_proj));

	if ((i = gsgl_displaylist_inq_projection_num (p)) <= 0) return NULL;
	else {
		data->matrix = (unsigned int *) gsgl_malloc_atomic (i*32*sizeof(unsigned int));
	}

	list = gsgl_displaylist_inq_start (p);
	data->i = 0;
	while (list) {
		gsgl_lists_call_template(list->data, p, data, inq_projection);
		list = list->next;
	}
	return data->matrix;
}

typedef struct _st_model
{
	int i;
	unsigned int *matrix;
} st_model;

static void inq_modelview (gsglDisplaylist *list, gsglPage *p, void *data1)
{
	st_model * data = (st_model *) data1;
	unsigned int *matrix = data->matrix;

	if (list && list->type == GSGL_LIST_MODELVIEW) {
		gsgl_displaylist_get_matrix (list, &matrix[data->i*32]);
		data->i++;
	}
}

static unsigned int * gsgl_displaylist_inq_modelview_matrixs (gsglPage *p)
{
	gsgllist *list;
	st_model *data;
	int i;

	data = (st_model *) gsgl_malloc (sizeof(st_model));

	if ((i = gsgl_displaylist_inq_modelview_num (p)) <= 0) return NULL;
	else {
		data->matrix = (unsigned int *) gsgl_malloc_atomic (i*32*sizeof(unsigned int));
	}

	data->i = 0;
	list = gsgl_displaylist_inq_start (p);
	while (list) {
		gsgl_lists_call_template(list->data, p, data, inq_modelview);
		list = list->next;
	}
	return data->matrix;
}

void gsgl_displaylist_inq_list_info (gsglboolean *remade, gsglboolean *lighting, int *num_list, int *num_projection, int *num_modelview, unsigned int **projection, unsigned int **modelview, gsglPage *p)
{
	if (!gsgl_inq_i_am_a_server (p->library)) return;

	*remade = gsgl_displaylist_inq_remade (p);
	*lighting = gsgl_inq_lighting_is_enabled (p); 
	if (gsgl_displaylist_inq_remade (p)) {
		gsgl_displaylist_remake_for_save (p);

		*num_list = gsgl_displaylist_inq_list_num (p);
		*num_projection = gsgl_displaylist_inq_projection_num (p);
		*num_modelview = gsgl_displaylist_inq_modelview_num (p);
		*projection = gsgl_displaylist_inq_projection_matrixs (p);
		*modelview = gsgl_displaylist_inq_modelview_matrixs (p);
	} else {
		*num_list = 0;
		*num_projection = 0;
		*num_modelview = 0;
		*projection = NULL;
		*modelview = NULL;
	}
}

/* pagee が呼ばれたあとに追加されたオブジェクトを再構成する */
void gsgl_displaylist_remake_added (gsglPage *p)
{
	gsgllist *list;
	GLfloat rgba[4];
	GLfloat matrix[16];
	gsglboolean use_gl = GSGL_TRUE;
	
	if (!p->dsp->List_is_remade || !p->dsp->Use_list) return; /* display list が再構成されていない */

	if (gsgl_inq_mode (p->library) == GSGL_MODE_TCPIP)
		use_gl = GSGL_FALSE;

	if (use_gl) {
		gsgl_init_matrix_cache (p);
		gsgl_init_gl_every_page (p);
	}
	
	/* objectsの前にcolor, matrixを操作するためのlistをつなげる */
	list = gsgl_displaylist_inq_after_header (p);
	while (list) {
		if (use_gl) {
			if (DISPLAYLIST(list)->type == GSGL_LIST_STATUS || 
			    DISPLAYLIST(list)->type == GSGL_LIST_MODELVIEW) {
				if (DISPLAYLIST(list)->gllist > 0)
					glCallList (DISPLAYLIST(list)->gllist);
			}
			/* pagee 以前に呼ばれていた modelview matrix を適用させるため */
			if (DISPLAYLIST(list)->matrix &&
			    DISPLAYLIST(list)->matrix->type == GSGL_LIST_MODELVIEW) {
				if (DISPLAYLIST(list)->gllist > 0) 
					glCallList (DISPLAYLIST(list)->gllist);
			}
		}
		if (DISPLAYLIST(list)->type == GSGL_LIST_MODELVIEW) {

			gsgl_displaylist_free_data (list);
			gsgl_free (list->data);
			list = gsgl_list_connect (list->prev, list->next);

		} else if (DISPLAYLIST(list)->type == GSGL_LIST_2DOBJ ||
			   DISPLAYLIST(list)->type == GSGL_LIST_3DOBJ ||
			   DISPLAYLIST(list)->type == GSGL_LIST_2DOBJ_FILEONLY || 
			   DISPLAYLIST(list)->type == GSGL_LIST_3DOBJ_FILEONLY) {
			if (!DISPLAYLIST(list)->color_gui) { /* has not been remade */
			    if (use_gl) {
				glGetFloatv (GL_MODELVIEW_MATRIX, matrix);
				glGetFloatv (GL_CURRENT_COLOR, rgba);
				
				gsgl_displaylist_make_objects_list (list->data, matrix, rgba, use_gl, p);
			    } else {
				gsgl_displaylist_make_objects_list (list->data, matrix, rgba, use_gl, p);
			    }
			}
		}
		list = list->next;
	}
}
