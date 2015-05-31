
#include "config.h"
#ifdef HAVE_OSMESA
#include "gsaf.h"
#include "utils.h"
#include <GL/osmesa.h>

static OSMesaContext  cox=NULL;
static void           *OSMesaBuffer=NULL;

static void gsglMesaMakeCurrent (unsigned int width, unsigned int height)
{
/*#if OSMESA_MAJOR_VERSION * 100 + OSMESA_MINOR_VERSION >= 305
  OSMesaContext ctx = OSMesaCreateContextExt( OSMESA_RGBA, 16, 0, 0, NULL );
  #else
  OSMesaContext ctx = OSMesaCreateContext( OSMESA_RGBA, NULL );
  #endif*/
	cox = OSMesaCreateContext (OSMESA_RGBA, NULL);
	if (!cox) gsgl_error ("", "cox == NULL");
	OSMesaBuffer = (void *) gsgl_malloc_atomic_ignore_off_page (width * height * 4 * sizeof(GLubyte));
	if (!OSMesaBuffer) gsgl_error ("", "osmesabuffer == NULL");
	if (!OSMesaMakeCurrent (cox, OSMesaBuffer, GL_UNSIGNED_BYTE, width, height))
		gsgl_error ("", "OSMesaMakeCurrent failed");;
}

static void gsglMesaFreeContext (void)
{
	free (OSMesaBuffer);
	/*OSMesaDestroyContext (cox);*/
}

void gsgl_make_drawable_osmesa (int width, int height)
{
	unsigned int w, h;
	w = width;
	h = height;
	gsglMesaMakeCurrent (w, h);
}

char * gsgl_get_osmesa_buffer (void)
{
	return OSMesaBuffer;
}

#endif
