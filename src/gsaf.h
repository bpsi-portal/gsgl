#ifndef __gsaf_h__
#define __gsaf_h__

#include "config.h"
#ifdef HAVE_TERMIOS_H
#  include <termios.h>
#else
#  ifdef HAVE_SGTTY_H
#    include <sgtty.h>
#  endif
#endif
#ifdef HAVE_STDLIB_H
#  include <stdlib.h>
#else
   extern char *getenv();
#endif
#ifdef HAVE_SYS_TYPES_H
#  include <sys/types.h>
#endif
#ifdef HAVE_SYS_STAT_H
#  include <sys/stat.h>
#endif
#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#endif

#ifndef S_ISREG
#define S_ISREG(m) (((m) & S_IFMT) == S_IFREG)
#endif

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#include "gettext.h"
#define _(String)   gettext(String)
#define N_(String)  gettext_noop(String)

#if SIZEOF_INT == 4
#define		int4	int
#elif SIZEOF_INT == 2
#define		int4	long
#else
#define		int4	int
#endif

#ifndef M_PI
#  define M_PI		3.14159265358979323846
#endif

#define   GSGL_DEG   (M_PI/180.0)
#define   GSGL_RAD   (180.0/M_PI)

#ifndef X_DISPLAY_MISSING
#    include <X11/Xlib.h>
#    include <X11/Xutil.h>
#endif
#if defined(__APPLE__) && defined(USE_QT6)
#    include <OpenGL/gl.h>
#    include <OpenGL/glu.h>
#else
#    include <GL/gl.h>
#    include <GL/glu.h>
#    ifndef X_DISPLAY_MISSING
#        include <GL/glx.h>
#    endif
#endif

#if defined(__APPLE__) && defined(USE_QT6)
typedef void* GLXContext;
#endif

#ifdef HAVE_FREETYPE2
#include <ft2build.h>
#include FT_FREETYPE_H
#endif

#define GSGL_TRUE 1
#define GSGL_FALSE 0

#define   GSGLBASIC_EYEPOSITION   20.0f
#define   EYEPOS_DELTA 0.001 /* >= 0.0 */
#define   GSGLBASIC_WINDOWWIDTH    1024
#define   GSGLBASIC_WINDOWHEIGHT    760
#define   GSGLBASIC_LINEWIDTH      2.0f

#define MAX(A, B) ((A) > (B) ? (A) : (B))
#define MIN(A, B) ((A) < (B) ? (A) : (B))

#define   GSGL_DEVICE_X11 1
#define   GSGL_DEVICE_PS  2
#define   GSGL_DEVICE_RASTER  3
#define   GSGL_DEVICE_TCPIP   4

#define   GSGL_MODE_NO_GL     10001
#define   GSGL_MODE_PIXMAP    10002
#define   GSGL_MODE_WINDOW    10003
#define   GSGL_MODE_TCPIP     10004
#define   GSGL_MODE_INLINE    10005

#define   GSGL_TCP_DEFAULT_PORT 9291

#define   GSGL_DATA_ORTHO_EQUIV_INTERVAL 1
#define   GSGL_DATA_ORTHO_UNEQU_INTERVAL 2
#define   GSGL_DATA_CYLIN_EQUIV_INTERVAL 3
#define   GSGL_DATA_CYLIN_UNEQU_INTERVAL 4
#define   GSGL_DATA_X_Y_Z_2D                 5
#define   GSGL_DATA_FEM_TRIANGLE  100

#define   GSGL_BACK_COLOR_BLACK  0
#define   GSGL_BACK_COLOR_WHITE  1
#define   GSGL_BACK_COLOR_PS     2

#define   GSGL_NORMAL_NO_SPECIFY             0
#define   GSGL_NORMAL_BY_SCAL_PROD           1

#define GSGL_COORD_NULL   0
#define GSGL_COORD_NORMAL 1
#define GSGL_COORD_2D     2
#define GSGL_COORD_3D     3
#define GSGL_COORD_TEXT2D 4
#define GSGL_COORD_TEXT3D 5

#define GSGL_HEADER_FNT 2
#define GSGL_DEFAULT_CHS 0.35
#define GSGL_DEFAULT_FNT 0
#define GSGL_ORIGIN 0.0 

#define   GSGL_LIST_INVALID     0 /* 初期状態 */
#define   GSGL_LIST_NULL       10 /* displaylist が無い */
#define   GSGL_LIST_STATUS     11 /* 色，線の太さの変更など */
#define   GSGL_LIST_MODELVIEW  12 /* 2D,3D座標系 */
#define   GSGL_LIST_PROJECTION 13
#define   GSGL_LIST_2DOBJ      14
#define   GSGL_LIST_3DOBJ      15
#define   GSGL_LIST_GL_ONLY    16
#define   GSGL_LIST_GL_IGNORE  17
#define   GSGL_LIST_BEGIN_HEADER 18
#define   GSGL_LIST_END_HEADER 19
#define   GSGL_LIST_2DOBJ_FILEONLY 20
#define   GSGL_LIST_3DOBJ_FILEONLY 21 /* Vertex, Color, Normal */
#define   GSGL_LIST_LIGHTING_ENABLE  22
#define   GSGL_LIST_LIGHTING_DISABLE 23
#define   GSGL_LIST_PAGEINFO         24
#define   GSGL_LIST_GUIITEMS         25

#define   GSGL_LIST_IS_3DOBJ(type) ((type == GSGL_LIST_3DOBJ || type == GSGL_LIST_3DOBJ_FILEONLY) ? GSGL_TRUE : GSGL_FALSE) 
#define   GSGL_LIST_IS_2DOBJ(type) ((type == GSGL_LIST_2DOBJ || type == GSGL_LIST_2DOBJ_FILEONLY) ? GSGL_TRUE : GSGL_FALSE) 

#define FLOAT_EPS 1.e-6

#define DEFAULT_COLOR 7
#define DEFAULT_LINEWIDTH 2
#define DEFAULT_PROJECTIONTYPE GSGL_FRUSTUM

#define DEFAULT_BG 1
#define DEFAULT_BOX_R 0.5
#define DEFAULT_BOX_G 0.2
#define DEFAULT_BOX_B 0.2
#define DEFAULT_FRAME_R 0.0
#define DEFAULT_FRAME_G 1.0
#define DEFAULT_FRAME_B 1.0

#define COMMAND_PRINT "/usr/local/bin/gsgl2ps -ca '%s' | lpr"
#define COMMAND_MAN "/usr/bin/X11/netscape /usr/local/share/doc/gsgl/manual/index.html"
#define COMMAND_EPS_VEC "/usr/local/bin/gsgl2eps -abcr -o '%s' '%s'"
#define COMMAND_EPS_RAS "/usr/local/bin/gsgl2rastereps -abc -W 1024 -H 760 -o '%s' '%s'"
#define COMMAND_PNG "/usr/local/bin/gsgl2png -abc -W 1024 -H 760 -o '%s' '%s'"
#define COMMAND_JPG "/usr/local/bin/gsgl2jpeg -abc -W 1024 -H 760 -o '%s' '%s'"

#ifdef __APPLE__
#define GSGL_TTF_DIR "/System/Library/Fonts/Supplemental"
#define TTF_ALPHABET GSGL_TTF_DIR "/Arial.ttf"
#define TTF_MATH GSGL_TTF_DIR "/Times New Roman Italic.ttf"
#define TTF_TIMESROMAN GSGL_TTF_DIR "/Times New Roman.ttf"
#define TTF_TIMESITALIC GSGL_TTF_DIR "/Times New Roman Italic.ttf"
#define TTF_TIMESBOLD GSGL_TTF_DIR "/Times New Roman Bold.ttf"
#define TTF_TIMESBOLDITALIC GSGL_TTF_DIR "/Times New Roman Bold Italic.ttf"
#define TTF_ARIALROMAN GSGL_TTF_DIR "/Arial.ttf"
#define TTF_ARIALITALIC GSGL_TTF_DIR "/Arial Italic.ttf"
#define TTF_ARIALBOLD GSGL_TTF_DIR "/Arial Bold.ttf"
#define TTF_ARIALBOLDITALIC GSGL_TTF_DIR "/Arial Bold Italic.ttf"
#define TTF_COURIERROMAN GSGL_TTF_DIR "/Courier New.ttf"
#define TTF_COURIERITALIC GSGL_TTF_DIR "/Courier New Italic.ttf"
#define TTF_COURIERBOLD GSGL_TTF_DIR "/Courier New Bold.ttf"
#define TTF_COURIERBOLDITALIC GSGL_TTF_DIR "/Courier New Bold Italic.ttf"
#define TTF_GREEK  GSGL_TTF_DIR "/Arial Unicode.ttf"
#define TTF_GREEKROMAN GSGL_TTF_DIR "/Arial Unicode.ttf"
#define TTF_GREEKITALIC  GSGL_TTF_DIR "/Arial Unicode.ttf"
#define TTF_GREEKBOLD GSGL_TTF_DIR "/Arial Unicode.ttf"
#define TTF_GREEKBOLDITALIC GSGL_TTF_DIR "/Arial Unicode.ttf"
#define TTF_JAPANESE GSGL_TTF_DIR "/AppleGothic.ttf"
#define TTF_JAPANESEMINCHO GSGL_TTF_DIR "/AppleMyungjo.ttf"
#define TTF_JAPANESEGOTHIC GSGL_TTF_DIR "/AppleGothic.ttf"
#define TTF_CMR10 GSGL_TTF_DIR "/STIXGeneral.otf"
#define TTF_CMTI10 GSGL_TTF_DIR "/STIXGeneralItalic.otf"
#define TTF_CMTT10 GSGL_TTF_DIR "/STIXGeneral.otf"
#define TTF_CMMI10 GSGL_TTF_DIR "/STIXGeneralItalic.otf"
#define TTF_CMSY10 GSGL_TTF_DIR "/STIXGeneral.otf"
#define TTF_CMEX10 GSGL_TTF_DIR "/STIXGeneral.otf"
#else
#define TTF_ALPHABET "/usr/local/share/fonts/truetype/arial.ttf"
#define TTF_MATH "/usr/local/share/fonts/truetype/timesi.ttf"
#define TTF_TIMESROMAN "/usr/local/share/fonts/truetype/times.ttf"
#define TTF_TIMESITALIC "/usr/local/share/fonts/truetype/timesi.ttf"
#define TTF_TIMESBOLD "/usr/local/share/fonts/truetype/timesbd.ttf"
#define TTF_TIMESBOLDITALIC "/usr/local/share/fonts/truetype/timesbi.ttf"
#define TTF_ARIALROMAN "/usr/local/share/fonts/truetype/arial.ttf"
#define TTF_ARIALITALIC "/usr/local/share/fonts/truetype/ariali.ttf"
#define TTF_ARIALBOLD "/usr/local/share/fonts/truetype/arialbd.ttf"
#define TTF_ARIALBOLDITALIC "/usr/local/share/fonts/truetype/arialbi.ttf"
#define TTF_COURIERROMAN "/usr/local/share/fonts/truetype/cour.ttf"
#define TTF_COURIERITALIC "/usr/local/share/fonts/truetype/couri.ttf"
#define TTF_COURIERBOLD "/usr/local/share/fonts/truetype/courbd.ttf"
#define TTF_COURIERBOLDITALIC "/usr/local/share/fonts/truetype/courbi.ttf"
#define TTF_GREEK  "/usr/local/share/fonts/truetype/greekn.ttf"
#define TTF_GREEKROMAN "/usr/local/share/fonts/truetype/greekn.ttf"
#define TTF_GREEKITALIC  "/usr/local/share/fonts/truetype/greeki.ttf"
#define TTF_GREEKBOLD "/usr/local/share/fonts/truetype/greekn.ttf"
#define TTF_GREEKBOLDITALIC "/usr/local/share/fonts/truetype/greeki.ttf"
#define TTF_JAPANESE "/usr/local/share/fonts/truetype/kochi-gothic.ttf"
#define TTF_JAPANESEMINCHO "/usr/local/share/fonts/truetype/kochi-mincho.ttf"
#define TTF_JAPANESEGOTHIC "/usr/local/share/fonts/truetype/kochi-gothic.ttf"
#define TTF_CMR10 "/usr/local/share/fonts/truetype/cmr10.ttf"
#define TTF_CMTI10 "/usr/local/share/fonts/truetype/cmti10.ttf"
#define TTF_CMTT10 "/usr/local/share/fonts/truetype/cmtt10.ttf"
#define TTF_CMMI10 "/usr/local/share/fonts/truetype/cmmi10.ttf"
#define TTF_CMSY10 "/usr/local/share/fonts/truetype/cmsy10.ttf"
#define TTF_CMEX10 "/usr/local/share/fonts/truetype/cmex10.ttf"
#endif
#ifdef __APPLE__
#define GSGL_TYPE1_DIR "/opt/homebrew/share/ghostscript/Resource/Font"
#define TYPE1_Times_Roman GSGL_TYPE1_DIR "/NimbusRoman-Regular"
#define TYPE1_Times_Italic GSGL_TYPE1_DIR "/NimbusRoman-Italic"
#define TYPE1_Times_Bold GSGL_TYPE1_DIR "/NimbusRoman-Bold"
#define TYPE1_Times_BoldItalic GSGL_TYPE1_DIR "/NimbusRoman-BoldItalic"
#define TYPE1_Helvetica GSGL_TYPE1_DIR "/NimbusSans-Regular"
#define TYPE1_Helvetica_Oblique GSGL_TYPE1_DIR "/NimbusSans-Italic"
#define TYPE1_Helvetica_Bold GSGL_TYPE1_DIR "/NimbusSans-Bold"
#define TYPE1_Helvetica_BoldOblique GSGL_TYPE1_DIR "/NimbusSans-BoldItalic"
#define TYPE1_Courier GSGL_TYPE1_DIR "/NimbusMonoPS-Regular"
#define TYPE1_Courier_Oblique GSGL_TYPE1_DIR "/NimbusMonoPS-Italic"
#define TYPE1_Courier_Bold GSGL_TYPE1_DIR "/NimbusMonoPS-Bold"
#define TYPE1_Courier_BoldOblique GSGL_TYPE1_DIR "/NimbusMonoPS-BoldItalic"
#define TYPE1_Symbol GSGL_TYPE1_DIR "/StandardSymbolsPS"
#else
#define TYPE1_Times_Roman "/usr/share/fonts/default/Type1/n021003l.pfb"
#define TYPE1_Times_Italic "/usr/share/fonts/default/Type1/n021023l.pfb"
#define TYPE1_Times_Bold "/usr/share/fonts/default/Type1/n021004l.pfb"
#define TYPE1_Times_BoldItalic "/usr/share/fonts/default/Type1/n021024l.pfb"
#define TYPE1_Helvetica "/usr/share/fonts/default/Type1/n019003l.pfb"
#define TYPE1_Helvetica_Oblique "/usr/share/fonts/default/Type1/n019023l.pfb"
#define TYPE1_Helvetica_Bold "/usr/share/fonts/default/Type1/n019004l.pfb"
#define TYPE1_Helvetica_BoldOblique "/usr/share/fonts/default/Type1/n019024l.pfb"
#define TYPE1_Courier "/usr/share/fonts/default/Type1/n022003l.pfb"
#define TYPE1_Courier_Oblique "/usr/share/fonts/default/Type1/n022023l.pfb"
#define TYPE1_Courier_Bold "/usr/share/fonts/default/Type1/n022004l.pfb"
#define TYPE1_Courier_BoldOblique "/usr/share/fonts/default/Type1/n022024l.pfb"
#define TYPE1_Symbol "/usr/share/fonts/default/Type1/s050000l.pfb"
#endif

#define GUI_SAVE_UN_CLOSE 51
#define GUI_SAVE_DO_CLOSE 52
#define GUI_SAVE_THIS_FRAME 53

#include "gsgltype.h"

struct _fvec2 {
	float x;
	float y;
};

typedef struct _ivec2 {
	int x;
	int y;
} gsglvec2i;

struct _fvec3 {
	float x;
	float y;
	float z;
};

struct _frgb {
	float r;
	float g;
	float b;
};

struct _frec {
	float xmin;
	float xmax;
	float ymin;
	float ymax;
};

struct _gsglBuffers
{
	gsgllist *memLargeBuffer; /* allocate in the mass */
	gsgllist *memCurrentList;
	unsigned short *memCurrentBuffer;
	unsigned int memSize;
	int Hd_token; /* hd_bufferにpushされたtoken数 */
	unsigned short * Hd_buffer;
	FILE *Hd;
	gsglWriteToFile *Write;
	gsglboolean Wrote_signature;
};

struct _gsglConfigColor {
	int   bg;
	double box[3];
	double frame[3];
};

#define TABLE_SIZE 100

struct _gsglLibrary
{
	int DeviceType;
	gsglboolean OpenPage;
	gsglboolean KeepHeader;
	gsglboolean OpenSaveFile;
	gsglboolean SaveToFile;
	gsglboolean AskInSave;
	gsglboolean Bell;
	int Pagenum;
	float Size_x;
	float Size_y;
	char Header[256];
	float TimeOpen;
	void (*Callback)(void);
	gsglConfigColor Configcolor;
	char *Command_print;
	char *Command_man;
	char *Command_eps_vec;
	char *Command_eps_ras;
	char *Command_png;
	char *Command_jpg;
	int  Userdefined_windowwidth;
	int  Userdefined_windowheight;
	int  Userdefined_corner_x;
	int  Userdefined_corner_y;
	char *Ttf_name[41];
	int  Bgcolor;
	Display *Mydisplay;
	XVisualInfo *Vi;
	GLXContext  Cx;
	gsglboolean Fullscreen;
	gsglboolean Userdefined_window;
	int Mode;
	Window Prewindow;
	unsigned int winwidth0;
	unsigned int winheight0;
	int Client_Socket;
	int Server_socket;
	gsglboolean I_am_a_server;
	gsglboolean DoubleBuffer_is_available;
	FILE *rfp;
	FILE *wfp;
	GLfloat *feedbackBuffer;
#ifdef HAVE_FREETYPE2
	FT_Library Library;
#endif
	gsgllist *Table[12][TABLE_SIZE];
	int Rank;
	char *Ttf_name_server[41];
};

struct _gsglWindow
{
	Window Mywindow;
	unsigned int WinWidth;
	unsigned int WinHeight;
	float Deltax;
	float Deltay;
	gsglboolean DoubleBuffer;
	gsglboolean Antialiasing;
	gsglboolean AntialiasingNow;
	GLuint *Select_buffer;
	gsglLibrary *library;
};

struct _gsglPage
{
	gsglBase *base;
	gsglAnime *Anime;
	gsglpattern *pattern;
	gsgl2dinfo *TwoDInfo;
	gsglScale3D *scale3d;
	gsglrgb rgb;
	float Ws;
	gsgl3DInfo *ThreeDInfo;
	gsglCallBack *callback;
	gsglMatrices *matrix;
	gsglText *text;
	gsglBuffers *buffer;
	gsglDrivers *driver;
	gsglDisplay *dsp;
	gsglClientState *client;
	gsglGuiitems *gui;
	gsglboolean Select_objects;
	gsglWindow *win;
	gsglLibrary *library;
};

#define LIB(p) (p->library)
#define WIN(p) (p->win)

#endif
