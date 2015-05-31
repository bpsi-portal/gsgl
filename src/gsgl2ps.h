#ifndef __gsgl2ps_h__
#define __gsgl2ps_h__

#include <time.h>

#define LINEWIDTH      1.0
#define POS_EPSILON    (1.e-4)
#define POLYGON_OFFSET (5.e-4)
#define PAPER_B5 1
#define PAPER_A4 2

GLfloat *gsgl2ps_getfeedbackbuffer (gsglLibrary *);
void gsgl2psDepthByPoint (GLfloat x, GLfloat y, GLfloat z);
void gsgl2ps_message (GLint errflg, char *argv[]);
void gsgl2ps_open (int interactive, int irotate, int icolor, char filename[], int eps, int gouraud, int paper);
void gsgl2ps_page (int pageno, GLfloat *feedbackBuffer);
void gsgl2ps_close (int pageno);
void gsgl2ps_freefeedbackbuffer (gsglLibrary *);

#endif
