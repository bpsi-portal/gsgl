#include <qobject.h>
#include <qsocketnotifier.h>
#include "gsgltype.h"

class gsgltcp_base : public QObject
{
	Q_OBJECT

public:
	gsgltcp_base (int, gsglPage *, QObject * parent = 0, const char * name = 0);
	~gsgltcp_base () {};

	gsglPage *Page;
	QSocketNotifier *notify;

public slots:		
	virtual void callback() {};
};


class gsgltcp_client : public gsgltcp_base
{
	Q_OBJECT

public:
	gsgltcp_client (int, gsglPage *, QObject * parent = 0, const char * name = 0);
	~gsgltcp_client () {};

public slots:		
	void callback();
};


class gsgltcp_server : public gsgltcp_base
{
	Q_OBJECT

public:
	gsgltcp_server (int, gsglPage *, QObject * parent = 0, const char * name = 0);
	~gsgltcp_server () {};

public slots:		
	void callback();
};

