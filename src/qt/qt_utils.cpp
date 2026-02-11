#include <QDir>
#include <QString>
#include "gsaf.h"
#include "qt_utils.h"
#include "utils.h"

const char * gsgl_qt_get_home_dir ()
{
	QString str = QDir::homePath ();
	return gsgl_strdup (str.toUtf8().constData());
}
