#include "gsaf.h"
#include "internal.h"
#include "ogl.h"
#include "displaylist.h"
#include "matrix.h"
#include "utils.h"

#define UNIT 3000000

static void call_lists_glonly_internal (gsglDisplaylist *list, gsglPage *p, void *data)
{
	GLuint gllist;
	
	gllist = list->gllist;
	if (gllist > 0) glCallList (gllist);
}

static void gsgl_lists_call_glonly (gsgllist *start, int type, gsglPage *p)
{
	gsgllist *list;

	list = start;
	while (list) {
		gsgl_lists_call_template (list->data, p, NULL, call_lists_glonly_internal);
		if (DISPLAYLIST(list)->type == type) break;
		list = list->next;
	}
}

gsglGLFeedbackBuffer * gsgl_internal_format_make (gsglPage *p)
{
	GLint            filesize = 1, size=0;
	GLuint           tmplist1;
	GLfloat          *feedbackBuffer;
	gsglGLFeedbackBuffer *buffer0;
	gsgllist  *list;
	
	buffer0 = (gsglGLFeedbackBuffer *) gsgl_malloc(sizeof(gsglGLFeedbackBuffer));
	
	gsgl_init_matrix_cache (p);
	gsgl_init_gl_every_page (p);
	
	tmplist1 = glGenLists(1);
	glNewList(tmplist1, GL_COMPILE);
	glPassThrough((GLfloat) GSGLINTERNAL_BEGIN_HEAD);
	list = gsgl_displaylist_inq_start (p);

	gsgl_lists_call_glonly (list, GSGL_LIST_END_HEADER, p);

	glPassThrough((GLfloat) GSGLINTERNAL_END_HEAD);

	glPassThrough((GLfloat) GSGLINTERNAL_BEGIN_PAGE);
	list = gsgl_displaylist_inq_after_header (p);
	gsgl_lists_call (list, GSGL_FALSE, p);
	glPassThrough((GLfloat) GSGLINTERNAL_END_PAGE);

	glEndList();
	
        /* save it to buffer */
	size = 0;
	filesize = -1;
	do {
		if (filesize < 0) size = size + UNIT;
		gsgl_init_gl_every_page (p);
		
		feedbackBuffer = gsgl_malloc_atomic_ignore_off_page (size*sizeof(GLfloat));
		glFeedbackBuffer(size, GL_3D_COLOR, feedbackBuffer);
		(void) glRenderMode(GL_FEEDBACK);
		glCallList(tmplist1);
		filesize = glRenderMode(GL_RENDER);
		if (filesize < 0)
			gsgl_free (feedbackBuffer);
	} while (filesize < 0);  
	
	glDeleteLists (tmplist1, 1); 
	
	buffer0->fbuffer = feedbackBuffer;
	buffer0->size = filesize;
	
	return buffer0;
}
