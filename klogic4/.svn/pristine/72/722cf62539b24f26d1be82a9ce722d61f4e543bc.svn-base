#ifndef __DEVLIB_CLASS__
#define __DEVLIB_CLASS__

#include <qmessagebox.h>
#include <qstring.h>
#include <qpixmap.h>
#include <qxml.h>

class QFile;

#include "klogicList.h"
#include "xdevice.h"
#include "DeviceType.h"

class DevLibrary;
class XmlDevice;
class LibDeviceID;
class XmlObject;

/********************************************
* library device identifier                 *
*********************************************/
class LibDeviceID
{
public:
	static int getID();
	static void releaseID(int);
private:
	static const int firstID = DeviceType::fFIRST_LIBDEV;
	static const int lastID = DeviceType::fLAST_LIBDEV;
	static unsigned char *free_entries;
};

/********************************************
* a library of devices                      *
*********************************************/
class DevLibrary
{
public:
	DevLibrary();
	~DevLibrary();

	// UI level methods
	static DevLibrary * newLib(QWidget *parent);
	int openLib(QWidget *parent);

	// library level methods
	QString getFileName();
	int newLib(const QString& newfile);
	int readLib();
	int saveLib();
	void closeLib();

	// device level methods
	int addToLib(const QString&, XDevice *);
	void removeFromLib(const QString&);
	XmlObject * getDev(const QString&);
	int getDevID(const QString&);
	void setName(const QString&, const QString&);
	void setMeta(const QString&, const QString&);

	XDevice *createInstance(Circuit *, int, int, int);		// returns an instance of id at position x,y

	// device list level methods
	void deviceList(QStringList&);

	bool libChanged();
	void setChanged();
private:
	KlogicList<XmlObject> lib_dev_list;
	QFile *file;
	int currentDevice;
	KlogicList<QPixmap> pixmap_list;
	static QString path;

	// statistics
	QString creation;
	QString author;
	QString comment;

	// changed content flag
	bool changed;
};

#endif

