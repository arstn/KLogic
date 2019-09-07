/*************************************************/
/* methods for class DevLibrary                  */
/*                                               */
/* library support for klogic                    */
/* a library contains a set of equation devices  */
/* with the following major content:             */
/*   - within the library unique name of the dev */
/*   - a set of equations                        */
/*   - optionally a pixmap                       */
/*                                               */
/* Andreas Rostin                                */
/* 26.05.01                                      */
/*************************************************/
#include <stdio.h>

#include <kfiledialog.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qregexp.h>

#include "KlogicGlobal.h"
#include "tool.h"
#include "circuit.h"
#include "xdevice.h"
#include "device.h"
#include "dlgLib.h"
#include "devLib.h"
#include "xmlExport.h"
#include "xmlImport.h"
#include "xmlSymbolMap.h"

/***************************************************/
/* methods of static class LibDeviceID             */
/*                                                 */
/* Artificial function ID's for library devices    */
/*                                                 */
/* The class provides runtime device ID's for      */
/* identifying device types in circuits.           */
/*                                                 */
/***************************************************/
// static
unsigned char *LibDeviceID::free_entries = (unsigned char *)NULL;

// static
int LibDeviceID::getID()
{	static int cnt = DeviceType::fLAST_LIBDEV - DeviceType::fFIRST_LIBDEV;
	int byte_idx;
	int bit_idx;
	unsigned char bit_byte;

	if (!free_entries) {
		int byte_cnt = cnt / 8 + 1;
		free_entries = new unsigned char[byte_cnt];
		memset(free_entries, 0, byte_cnt);
	}

	for(int i = 0; i < cnt; i++) {
		byte_idx = i / 8;
		bit_idx = i % 8;
		bit_byte = (unsigned char)Tool::pow2(bit_idx);
		if (!(free_entries[byte_idx] & bit_byte)) {
			free_entries[byte_idx] |= bit_byte;
			return i + DeviceType::fFIRST_LIBDEV;
		}
	}
	return 0;
}

// static
void LibDeviceID::releaseID(int id)
{
	int byte_idx = (id - DeviceType::fFIRST_LIBDEV) / 8;
	int bit_idx = (id - DeviceType::fFIRST_LIBDEV) % 8;
	unsigned char bit_byte = (unsigned char)Tool::pow2(bit_idx);
	free_entries[byte_idx] &= ~bit_byte;
}

/***************************************************/
/* methods of DevLibrary instance                  */
/*                                                 */
/* A library with devices in it                    */
/***************************************************/
QString DevLibrary::path = "";

DevLibrary::DevLibrary()
{
	currentDevice = 0;
	file = (QFile *)NULL;
	changed = false;
}

DevLibrary::~DevLibrary()
{
	closeLib();
	if (file) delete file;
}

// static
// create a new library via dialog interaction
DevLibrary * DevLibrary::newLib(QWidget *parent)
{	DevLibrary *activeLib = 0;
	QString filename;

	// first of all, request the name of the new library
	QString new_filename = KFileDialog::getSaveFileName(path, "*.klogic.lib");
	if (!new_filename.isEmpty()) {
		filename = new_filename;
		if (-1 == filename.indexOf(QRegExp("\\.klogic.lib$")))
			filename.append(".klogic.lib");
	} else return (DevLibrary *)NULL;
 
	// create a new library
	activeLib = new DevLibrary();

	int retval = activeLib->newLib(filename);
	if (1 == retval) {
		return activeLib;	// success!
	} else if (-1 == retval) {
		if (QMessageBox::information(parent, i18n("new library"),
			i18n("the file already exists ..\n press OK to delete it!"),
			QMessageBox::Ok, QMessageBox::Cancel) == QMessageBox::Cancel) {
			delete activeLib;
			return (DevLibrary *)NULL;
		}
		QFile _file(filename);
		if (!_file.remove()) {
			QMessageBox::information(parent, i18n("new library"),
				i18n("unable to delete file"));
			delete activeLib;
			return (DevLibrary *)NULL;
		}

		if (1 != activeLib->newLib(filename)) {
			QMessageBox::information(parent, i18n("new library"),
				i18n("unable to access lib-file"));
			delete activeLib;
			return (DevLibrary *)NULL;
		}
		return activeLib;
	} else if (-2 == retval) {
		QMessageBox::information(parent, i18n("new library"),
			i18n("unable to open lib-file"));
		delete activeLib;
		return (DevLibrary *)NULL;
	} else {
		QMessageBox::information(parent, i18n("new library"),
			i18n("unknown problem accessing choosen file"));
		delete activeLib;
		return (DevLibrary *)NULL;
	}
	return (DevLibrary *)NULL;
}

// create a new library
// return values:
//	1 : new library created
//	-1: file exists
//	-2: unable to create/truncate file
//	-3: what error?
int DevLibrary::newLib(const QString& newfile)
{
	closeLib();
	file = new QFile(newfile);

	// truncate ..
	if (file->exists()) return -1;
	if (!file->open(QFile::Truncate|QFile::WriteOnly)) return -2;
	file->flush();
	file->close();

	// reopen, initialize ..
	if ( 1 == readLib())
		return 1;

	// unknown error
	return -3;
}

void DevLibrary::setMeta(const QString& _author, const QString& _comment)
{
	changed = true;
	author = _author;
	comment = _comment;
}

// open a library via dialog interaction
int DevLibrary::openLib(QWidget *parent)
{	int ret = 0;

	QString new_filename = KFileDialog::getOpenFileName(path, "*.klogic.lib");
	if (!new_filename.isEmpty()) {
		closeLib();
		file = new QFile(new_filename);
		ret = readLib();
		switch(ret) {
		case 1:
			break;
		case -1:
			QMessageBox::information(parent, i18n("open library"),
				i18n("file does not exist"));
			break;
		case -2:
			QMessageBox::information(parent, i18n("open library"),
				i18n("unable to open file for read/write"));
			break;
		case -3:
			QMessageBox::information(parent, i18n("open library"),
				i18n("library parsing error"));
			break;
		case -4:
			QMessageBox::information(parent, i18n("open library"),
				i18n("not enough device ID slots (1000)"));
			break;
		default:
			QMessageBox::information(parent, i18n("open library"),
				i18n("unknown error"));
			break;
		}
	}
	changed = false;
	return ret;
}

// open a new library
//	1 : library successfully opened
//	-1: file does not exist
//	-2: file unaccessable
//	-3: file contains errors
//	-4: not enough device id slots!
int DevLibrary::readLib()
{
	if (!file->exists()) {
		delete file;
		file = (QFile *)NULL;
		return -1;
	}

	// prepare to read content
	if (!file->open(QFile::ReadOnly)) {
		delete file;
		file = (QFile *)NULL;
		return -2;
	}


	// empty files are OK ..
	if (file->atEnd()) {
		// set new default path
		QFileInfo fi(*file);
		path = fi.dir().path();
		file->close();
		return 1;
	}

	// read library content
	QTextStream ts(file);
	QString line;
	XmlLineParser splitter;
	XmlObject *obj = (XmlObject *)NULL;
	bool devActive = false;
	bool devContentActive = false;
	QString devName;
	QString devContent;
	int devLevel = 0;

	// create list of XmlObject
	line = ts.readLine();
	while(!line.isNull()) {
		// remove two tabs if there are two!
		// (XmlField adds level2 tabs to the device content!)
		if (line.length() > 1 && line[0] == '\t' && line[1] == '\t')
			line = line.right(line.length() - 2);

		splitter.setLine(line);
		if (!devActive) {
			// ----------------------------------------------------
			// library attributes
			// ----------------------------------------------------
			switch (splitter.tag()) {
				case XmlSymbol::CREATION_DATE:
					creation = splitter.getContent();
					break;
				case XmlSymbol::AUTHOR:
					author = splitter.getUnquotedContent();
					break;
				case XmlSymbol::COMMENT:
					comment = splitter.getUnquotedContent();
					break;
				case XmlSymbol::LIBDEV:
					if (!splitter.isEndTag()) {
						devActive = true;
						devContentActive = false;
					}
					break;
			}
		} else {
			// ----------------------------------------------------
			// library device attributes
			// ----------------------------------------------------
			switch (splitter.tag()) {
				case XmlSymbol::NAME:
					if (!devContentActive)
						devName = splitter.getUnquotedContent();
					else {
						devContent += line;
						devContent += "\n";
					}
					break;
				case XmlSymbol::CIRCUIT:
					if (!devContentActive) {
						// the very first device tag
						obj = new XmlNet();
						obj->setAttribute(splitter.getAttributes());
						devContentActive = true;
						devContent.truncate(0);
						devLevel = 0;
					} else {
						// ignore very last end tag
						if (devLevel != 0 || (devLevel == 0 && !splitter.isEndTag())) {
							devContent += line;
							devContent += "\n";
						}
						if (splitter.isEndTag()) devLevel--;
						else devLevel++;
					}
					break;
				case XmlSymbol::DEVICE:
					// start of a device definition: create an xml-device to store it
					if (!devContentActive) {
						// the very first device tag
						obj = new XmlDevice();
						obj->setAttribute(splitter.getAttributes());
						devContentActive = true;
						devContent.truncate(0);
						devLevel = 0;
					} else {
						// ignore very last end tag
						if (devLevel != 0 || (devLevel == 0 && !splitter.isEndTag())) {
							devContent += line;
							devContent += "\n";
						}
						if (splitter.isEndTag()) devLevel--;
						else devLevel++;
					}
					break;
				case XmlSymbol::CREATION_DATE:
					if (!obj) qFatal("invalid sequence!");
					obj->setCreation(splitter.getContent());
					break;
				case XmlSymbol::UPDATE_DATE:
					if (!obj) qFatal("invalid sequence!");
					obj->setUpdate(splitter.getContent());
					break;
				case XmlSymbol::AUTHOR:
					if (!obj) qFatal("invalid sequence!");
					obj->setAuthor(splitter.getUnquotedContent());
					break;
				case XmlSymbol::COMMENT:
					if (!obj) qFatal("invalid sequence!");
					obj->setComment(splitter.getUnquotedContent());
					break;
				case XmlSymbol::LIBDEV:
					if (!obj) qFatal("invalid sequence!");
					if (splitter.isEndTag()) {
						// select an empty device slot
						int dev_id = LibDeviceID::getID();
						if (!dev_id) {
							file->close();
							return -4;
						}

						obj->setContent(devContent);
						lib_dev_list.Append(obj, dev_id)->setText(devName);
						devActive = false;
						devContentActive = false;
					}
					break;
				default:
					// the device definition!
					if (devContentActive) {
						devContent += line;
						devContent += "\n";
					}
					break;
			}
		}
		line = ts.readLine();
	}

	// set new default path
	QFileInfo fi(*file);
	path = fi.dir().path();

	file->close();
	changed = false;
	return 1;
}

QString DevLibrary::getFileName()
{
	QFileInfo fi(*file);
	return fi.fileName();
}

// save changes
// returns 0 on error
int DevLibrary::saveLib()
{
	if (!file) return 0;
	if (!file->open(QFile::WriteOnly | QFile::Truncate))
		return 0;
	QTextStream ts(file);

	XmlObject header(XmlSymbolMap::m_oMap[XmlSymbol::KLOGIC_MAIN]);
	header.addAttribute(XmlSymbolMap::m_oMap[XmlSymbol::FILE_TYPE], XmlObject::ATT_LIBRARY);

	// meta tags
	header.setVersion();
	if (creation.isEmpty()) header.setCreation();
	else header.setCreation(creation);
	header.setUpdate();
	header.setAuthor(author);
	header.setComment(comment);

	ts << header.getFieldStart();
	ts << header.getFieldContent();

	// -------------------------------------------------------
	// export the library devices
	// encapsulate each device in a lib tag
	// during reading, they (devices) are taken as strings!
	// -------------------------------------------------------
	XmlField field;
	XmlField name_field;
	XmlField id_field;
	field.set(XmlSymbolMap::m_oMap[XmlSymbol::LIBDEV]);
	QString text;

	KlogicList<XmlObject> *ld = lib_dev_list.First();
	while(ld) {
		// the device name (library name)
		text = ld->getText();
		name_field.set(XmlSymbolMap::m_oMap[XmlSymbol::NAME], XmlField::quote(text));
		field.setContent(name_field.getField());

		// the device
		field.addContent(ld->Get()->getField());
		ts << field.getField();

		ld = ld->Next();
	}

	ts << header.getFieldEnd();

	file->flush();
	file->close();
	changed = false;
	return 1;
}

// close: empty lib (file is always closed)
void DevLibrary::closeLib()
{
	KlogicList<XmlObject> *ld = lib_dev_list.First();
	while(ld) {
		LibDeviceID::releaseID(ld->getID1());
		delete ld->Get();
		ld = ld->Next();
	}
	lib_dev_list.Destroy();

	pixmap_list.Destroy();
	changed = false;
}

// change device name
void DevLibrary::setName(const QString& name, const QString& new_name)
{
	KlogicList<XmlObject> *ld = lib_dev_list.With(name);
	if (ld) ld->setText(new_name);
	changed = true;
}

// create a device from currently set name
XDevice * DevLibrary::createInstance(Circuit *_net, int id, int dx, int dy)
{
	KlogicList<XmlObject> *ld = lib_dev_list.With(id);
	if (!ld || !ld->Get()) return (XDevice *)NULL;

	return ld->Get()->createInstance(_net, dx, dy);
}

// add a new device to the library
// source of XmlDevice will be set to source=1 until the next saveLib operation
// returns -1 if duplicate name
//         -2 if no device slots left
//          0 on success
int DevLibrary::addToLib(const QString& dev_name, XDevice *dev)
{
	// check for duplicates
	KlogicList<XmlObject> *ld = lib_dev_list.With(dev_name);
	if (ld && ld->Get()) {
		return -1;	// duplicate name
	}

	// create device id
	int dev_id = LibDeviceID::getID();
	if (!dev_id) {
		return -2;	// no device slots left
	}

	// create a new library device
	Circuit *net = dev->devIsCircuit();
	if (net) {
		XmlNet *xmlNet = new XmlNet(net, false, 0, 0, 0);
		net->exportXML(*xmlNet);
		lib_dev_list.Append((XmlObject *)xmlNet, dev_id)->setText(dev_name);
	} else {
		XmlDevice *xmlDev = new XmlDevice(dev, false, 0, 0, 0);
		lib_dev_list.Append((XmlObject *)xmlDev, dev_id)->setText(dev_name);
	}
	changed = true;
	return 0;
}

// remove device from the library
void DevLibrary::removeFromLib(const QString& dev_name)
{
	KlogicList<XmlObject> *ld = lib_dev_list.With(dev_name);
	if (ld && ld->Get()) {
		LibDeviceID::releaseID(ld->getID1());
		lib_dev_list.Destroy(ld->Get());
	}
	changed = true;
}

int DevLibrary::getDevID(const QString& dev_name)
{
	KlogicList<XmlObject> *ld = lib_dev_list.With(dev_name);
	if (ld && ld->Get()) {
		return ld->getID1();
	}
	return 0;
}

XmlObject * DevLibrary::getDev(const QString& dev_name)
{
	KlogicList<XmlObject> *ld = lib_dev_list.With(dev_name);
	if (ld && ld->Get()) {
		return ld->Get();
	}
	return (XmlDevice *)NULL;
}

void DevLibrary::deviceList(QStringList& result_list)
{
	KlogicList<XmlObject> *ld = lib_dev_list.First();
	while(ld) {
		result_list.append(ld->getText());
		ld = ld->Next();
	}
}

bool DevLibrary::libChanged()
{
	return changed;
}

void DevLibrary::setChanged()
{
	changed = true;
}

