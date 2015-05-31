#include <qdir.h>
#include <qstring.h>
#include "gsaf.h"
#include "qt_utils.h"
#include "utils.h"

const char * gsgl_qt_get_home_dir ()
{
	QString str = QDir::homeDirPath ();	
	return gsgl_strdup (str.latin1 ());
}
