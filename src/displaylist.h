#ifndef __displaylist_h__
#define __displaylist_h__

struct _gsglDisplaylist {
	int             type;
	GLuint          name;
	GLuint          gllist;
	unsigned short *mem_list;
	unsigned int    size;
	gsglboolean     closed;
	struct _gsglDisplaylist *color_gui;
	struct _gsglDisplaylist *matrix_stack;
	struct _gsglDisplaylist *matrix_gui;
	struct _gsglDisplaylist *matrix;
	struct _gsglDisplaylist *color_restore;
};

struct _gsglDisplay
{
	gsgllist  *List0; /* gsgl displaylist のリストの始め */
	GLuint Objects_name;
	gsglDisplaylist  *List_utility;
	gsglDisplaylist  *List_projection;
	gsgllist  *List_of_list_lighting_enable;
	gsglboolean Use_list;
	gsgllist *Guiitems;
	gsglboolean List_is_remade;
};

#define DISPLAYLIST(x) ((gsglDisplaylist *)(x->data))

#ifdef __cplusplus
extern "C"{
#endif

void gsgl_lists_call_template (gsglDisplaylist *list, gsglPage *p, void *data, 
			       void (*action)(gsglDisplaylist *, gsglPage *, void *));
gsgllist * gsgl_displaylist_inq_start (gsglPage *);
gsgllist * gsgl_displaylist_inq_last (gsglPage *);
void gsgl_displaylist_begin (int type, gsglPage *);
void gsgl_displaylist_end (gsglPage *);
void gsgl_lists_call (gsgllist *start, int make_name, gsglPage *);
void gsgl_lists_delete (gsglPage *);
void gsgl_lists_delete_ogl (void);
void gsgl_clear (gsglPage *);
void gsgl_displaylist_clear (gsgllist *last);
gsgllist * gsgl_displaylist_inq_by_name (GLuint name0, gsglPage *);
void gsgl_displaylist_remake (gsglPage *);
void gsgl_displaylist_remake_for_save (gsglPage *);
void gsgl_displaylist_remake_for_save_tcpip (gsglPage *);
gsglDisplaylist * gsgl_displaylist_new (gsglPage *);
void gsgl_displaylist_init (gsglPage *);
void gsgl_displaylist_init_remade (gsglPage *);
gsglboolean gsgl_displaylist_inq_remade (gsglPage *);
void gsgl_displaylist_begin_guiitems (gsglPage *p);
gsgllist * gsgl_displaylist_inq_after_header (gsglPage *);
gsglDisplaylist * gsgl_displaylist_inq_gui_color (gsglDisplaylist * list0);
gsglDisplaylist * gsgl_displaylist_inq_gui_matrix (gsglDisplaylist * list0);
gsglDisplaylist * gsgl_displaylist_inq_pre_stack (gsglDisplaylist * list0);
gsglDisplaylist * gsgl_displaylist_inq_list_of_utility (gsglPage *);
gsglDisplaylist * gsgl_displaylist_inq_list_of_projection (gsglPage *);
gsgllist * gsgl_displaylist_inq_list_of_list_lighting_enable (gsglPage *);
gsgllist * gsgl_displaylist_inq_list_of_list_lighting_disable (gsglPage *);
void gsgl_displaylist_inq_list_info (gsglboolean *remade, gsglboolean *lighting, int *num_list, int *num_projection, int *num_modelview, unsigned int **projection, unsigned int **modelview, gsglPage *);
void gsgl_lists_call_header (gsglPage *);
void gsgl_disable_displaylist (gsglPage *);
void gsgl_enable_displaylist (gsglPage *);
void gsgl_displaylist_copy (gsglDisplaylist *from, gsglDisplaylist *to);
gsglboolean gsgl_inq_displaylist_is_enabled (gsglPage *);
void gsgl_displaylist_remake_added (gsglPage *);
gsgllist * gsgl_displaylist_inq_guiitems (gsglPage *);
void gsgl_displaylist_free_data (gsgllist *list);

#ifdef __cplusplus
}
#endif

#endif
