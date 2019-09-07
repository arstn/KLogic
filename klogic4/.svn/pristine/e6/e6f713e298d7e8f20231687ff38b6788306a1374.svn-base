#ifndef __XMLEXPORT__
#define __XMLEXPORT__

#include <qxml.h>
#include <xmlField.h>

class Circuit;
class XDevice;
class XWire;
class XmlSymbolMap;

/*******************************************/
/* XML Export Interface                    */
/*                                         */
/* Andreas Rostin 23.08.2001               */
/*                                         */
/* adding symbols:                         */
/* - add a static XmlSymbolContainer sXXX  */
/* - add an unique ID for that             */
/* - in xmlExport.cpp, instanciate them    */
/*   together                              */
/*******************************************/
class XmlObject: public XmlField
{
public:
	XmlObject(QString);
	XmlObject(QString, int, int, int);
	virtual ~XmlObject();

	// create an instance from the xml string definition
	// using the xmlImport classes
	XDevice * createInstance(Circuit *, int, int);

	void setValid(bool);
	bool isValid();

public:
	static const char * ATT_CIRCUIT;
	static const char * ATT_SUBCIRCUIT;
	static const char * ATT_LIBRARY;

protected:
	int dx, dy;
	bool _valid;
private:
	virtual void createXML(bool);
};

/*******************************************/
/* XML representation of a circuit         */
/*                                         */
/* Andreas Rostin 24.08.2001               */
/*******************************************/
class XmlNet: public XmlObject
{
public:
	XmlNet(Circuit *, bool, int, int, int);
	XmlNet();

private:
	virtual void createXML(bool selected);
	Circuit *m_poCircuit;
};

/*******************************************/
/* XML representation of a connection      */
/*     between devices and wires           */
/*                                         */
/* Andreas Rostin 24.08.2001               */
/*******************************************/
class XmlDeviceConnection: public XmlObject
{
public:
	XmlDeviceConnection(XWire *, bool, int, int, int);

private:
	virtual void createXML(bool selected);
	void setField(XDevice *, QPoint, int);
	XWire *wire;
};

/*******************************************/
/* XML representation of a connection      */
/*            between wires                */
/*                                         */
/* Andreas Rostin 24.08.2001               */
/*******************************************/
class XmlWireConnection: public XmlObject
{
public:
	XmlWireConnection(XWire *, bool, int, int, int);

private:
	virtual void createXML(bool selected);
	XWire *wire;
};

/*******************************************/
/* XML representation of a XWire           */
/*                                         */
/* Andreas Rostin 23.08.2001               */
/*******************************************/
class XmlWire: public XmlObject
{
public:
	XmlWire(XWire *, bool, int, int, int);

private:
	virtual void createXML(bool selected);
	XWire *wire;
};

/*******************************************/
/* XML representation of a XDevice         */
/*                                         */
/* Andreas Rostin 23.08.2001               */
/*******************************************/
class XmlDevice: public XmlObject
{
public:
	XmlDevice(XDevice *, bool, int, int, int);
	XmlDevice();

private:
	virtual void createXML(bool selected);
	XDevice *dev;
};

#endif
