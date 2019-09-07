#ifndef __CIRCUIT_CLASS__
#define __CIRCUIT_CLASS__

class QString;
class QPoint;
class QPainter;
class QTextStream;

#include "value.h"
#include "tool.h"
#include "deviceCustom.h"
#include "deviceList.h"

class DeviceFactory;
class XmlNet;
class Selection;
class XWire;

#define EXP_ALL		0
#define EXP_SELECTION	1

#define NO_SELECTION	0
#define SELECT_IT	1

/*********************************************
 * a network of devices with their connections
 * is a device itself
 *********************************************/
class Circuit: public CustomDevice
{
public:
	static int instance;

	Circuit(const QPoint&, int);
	virtual ~Circuit();

	//***** net operations
	XDevice *checkDeviceCollision(QPoint);		// check all devices for containing the point, returns first
	XDevice *checkDeviceCollision(XDevice *);	// check all devices for containing the device, returns first
	XWire *checkWireCollision(QPoint);		// check all connections for containing the point, returns first

	int checkCircuitNames();		// check if all names within this circuit are unique
	int unifyCircuitNames();		// make all names within this circuit unique
	bool hasImpossibleEquations();		// check if all equations are displayable
	virtual Circuit *devIsCircuit();	// get pointer of net from netdevice
	Circuit *rootParent();		// get pointer of root circuit
	virtual void setParent(XDevice *parent);	// set pointer of parent circuit
	Circuit *parent();			// get pointer of parent circuit
	bool empty();				// true if circuit contains nothing

	// get list of equations describing the content of the circuit
	virtual void getAllEquations(map<QString,OutputInfo>&, const QString&, bool);

	void applyDefaults();			// apply device defaults to all devices
	bool garbageCollection();	// remove useless wires/nodes
	virtual void setUndefinedValue(int);	// set undefined value of all connected output-interface devices

	//***** select groups of objects contained in rect
	void selectDev(QRect, KlogicList<XDevice> *);
	void selectWire(QRect, KlogicList<XWire> *);
	void grabSelection(Selection *);

	//***** drawing methods
	void draw(QPaintDevice *, QPainter *);			// draw all devices and wires

	void forceOutputChange();

	//***** device methods
	XDevice * createDevice(int func, const QPoint&, int size = -1);
	virtual bool addChild(XDevice *, bool);
	void deleteDevice(XDevice *);		// remove device properly
	void deleteAll();					// remove entire content
	void setDeviceName(int, const QString&);			// set new text for device with given id
	virtual bool hasBooleanOutput(int output_id) const;
	virtual void setHasBooleanOutput(int output_id, bool bHasBooleanInput);
	XDevice * getDevice(int devId);			// for import purposes

	//***** connection methods
	void checkConnection(XWire *);				// check wire for connections
	void checkConnection(int, XWire *);	// used as import-function!

	bool hasShortcut();

	//***** wire methods
	XWire * newWire();						// for import purposes
	XWire * newWire(QPoint);
	int removeNode(QPoint);
	bool releaseWire(XWire *);				// final works for locked wire
	void deleteWire(XWire *);				// remove wire properly
	XWire * getWire(int);					// for import purposes

	//***** calculation operations
	void Burst(int burst);					// this is the method to calculate a circuit!
	void flush();

	//***** import/export methods
	bool exportXML(XmlNet&);
	bool exportXML(QIODevice&, bool, int, int dx = 0, int dy = 0);
	void mapImportID(int old_id, int dev_id);		// change named_xxx list from old save values to new ones

	DeviceList::iterator getDeviceIterator();
	DeviceList::iterator getDeviceEndIterator();

	virtual void Calculate(int);
	virtual bool Propagate(int);

private:
	//***** calculation operations
	bool exportXML(QIODevice*, XmlNet *, bool, int, int dx = 0, int dy = 0);

	static const int IF_INPUT = 0;
	static const int IF_OUTPUT = 1;
	void switchInterface(int);

	// list of all devices in the net
	DeviceList devices;

	// list of all connections in the net
	KlogicList<XWire> wires;

	// interface to the parent circuit
	DeviceList inputDevices;
	DeviceList outputDevices;

	// list of all sub-circuits in the net
	Circuit *parentCircuit;

	bool wireOK(XWire *);		// check wire for a minimum of 2 points

	map<int, bool> m_mHasBooleanOutput;

	static DeviceFactory *m_poDeviceFactory;

public:
	// performance measurement
	static int m_iCalcCalls;
	static int m_iPropCalls;

public:
	// -----------------------
	// *** old file format ***
	// -----------------------
	// name mapping for import operation
	IDMap devmap;
	IDMap wiremap;

	void initImport();
	bool importInstance(QString, int dx = 0, int dy = 0);
	bool importDevice(QString, int select, int dx = 0, int dy = 0);
	bool importWire(QString, int select, int dx = 0, int dy = 0);
	bool importWireConn(QString, int select, int dx = 0, int dy = 0);
	bool importDevConn(QString, int select, int dx = 0, int dy = 0);
	Circuit *subCircuit(QString&);		// get pointer of circuit by name
	static int magic_version;


};

#endif

