#ifndef __XMLIMPORT__
#define __XMLIMPORT__

#include <qxml.h>
#include <xmlField.h>

class XDevice;
class Circuit;
class XWire;

/*******************************************/
/* XML line parser                         */
/* (used only for libraries)               */
/*                                         */
/* Andreas Rostin 25.09.2001               */
/*******************************************/
class XmlLineParser
{
public:
	void setLine(const QString&);
	int tag();
	bool isEndTag();
	QString getTag();
	QString getAttributes();
	QString getUnquotedContent();
	QString getContent();

private:
	bool endTag;
	int iTag;
	QString sTag;
	QString attributes;
	QString content;
};

/*******************************************/
/* XML SAX2 Import Interface               */
/*                                         */
/* Andreas Rostin 29.08.2001               */
/*******************************************/
class XMLImportHandler: private QXmlDefaultHandler
{
public:
	// this can also be used by other instances beneth normal file import
	XMLImportHandler(Circuit *, bool, int, int);
	XMLImportHandler(Circuit *, int, int);
	virtual ~XMLImportHandler();

	// this is the method called for normal file operation
	static bool import(Circuit *, bool, int, int, QIODevice&);

	// this method is used for library devices
	static XDevice * import(Circuit *, int, int, QIODevice&);

	XDevice * getDevice();

	static int errorsOccured();	// return no of errors occured during import
	static QString& getErrors();	// return error messages

private:
	static const int MAX_DEPTH = 10000;	// usually you'll need maybe 10 circuit level in depth
	void init();

	// these methods are called by the base class 
	virtual bool startDocument();
	virtual bool startElement(const QString&, const QString&, const QString& qName, const QXmlAttributes& atts);
	virtual bool characters (const QString& ch);
	virtual bool endElement(const QString&, const QString&, const QString& qName);
	virtual bool endDocument();

	bool setDeviceTag(XDevice *, const QString&);	// called from "characters" method

	Circuit *circuit;	// parent net

	// only important for the top level imported circuit
	bool imp_selected;	// select imported devices
	bool imp_selected_hidden;
	int imp_dx;		// x graphical offset for import
	int imp_dx_hidden;
	int imp_dy;		// y graphical offset for import
	int imp_dy_hidden;

	bool importSub;		// wether to import a sub circuit or a main circuit

	int activeTag[MAX_DEPTH];	// tag for which content will be received
	int activeTagIdx;

	// temp. attributes store
	int activeID;		// device/wire id attribute
	bool activeInv;		// inverter attribute
	int activePos;		// input/output position in device
	int activeIntern;	// internal input/output
	QString activeName;	// name of input/output

	Circuit *activeCircuit[MAX_DEPTH];
	int activeNetIdx;

	XDevice *activeDev;
	XWire *activeWire;
	XWire *activeWireConnWire;
	XWire *activeDevConnWire;

	XDevice *importedDev;	// the top level device/circuit (library purposes)
	bool m_bImportDev;	// single device import (library purposes)
	bool localImport;	// ignore global parameters such as AUTHOR field (library purposes)

	IDMap wiremap[MAX_DEPTH];
	IDMap devmap[MAX_DEPTH];

	static int error_cnt;
	static QString error_msg;

	int characters_call_cnt;
};
#endif
