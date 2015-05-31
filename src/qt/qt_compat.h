#ifndef __qt_compat_h
#define __qt_compat_h
#include "config.h"
#ifdef USE_QT3
#    define WFLAGS WFlags
#    define TOPWINDOW WType_TopLevel
#else
#    define WFLAGS Qt::WindowFlags
#    define TOPWINDOW Qt::Window
#endif
#endif
