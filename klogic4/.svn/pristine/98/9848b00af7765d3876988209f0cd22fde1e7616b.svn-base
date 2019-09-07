#ifndef __XWire_CLASS__
#define __XWire_CLASS__

#include <device.h>
#include <klogicList.h>
#include <qpixmap.h>
#include <xdevice.h>
#include <wire.h>
#include <deviceFactory.h>

/*********************************************
* a connection between two devices
*********************************************/
class XWire: public Wire
{
public:
	static int instance;

	XWire(QPoint);
	XWire();
	virtual ~XWire();

	// get information about the wire
	XDevice * outputDevice(int);
	int outputID(int);
	XDevice * inputDevice(int);
	XWire * ioWire(int);
	int getID();
	int getInputIndex(XDevice *);
	int getOutputIndex(XDevice *);
	int getConnectionID(XDevice *);

	// erase/draw actice part and affected parts of connected wires
	void draw(QPainter *);

	// node methods
	bool contains(QPoint);
	int createNode();
	XWire * removeNode();

	// set references in this and others, graphics and logical (device-connection)
	int connectInput(XDevice *, bool, int);
	void disconnectInput(int);
	int connectOutput(XDevice *, bool, int);
	void disconnectOutput(int);
	int isNamedInput(XDevice *, int);
	int isNamedOutput(XDevice *, int);

	// set/reset inverter
	void invertInput(XDevice *, bool);
	bool inputIsInverted(XDevice *);
	virtual bool inputIsInverted(int);
	void invertOutput(XDevice *, bool);
	bool outputIsInverted(XDevice *);
	virtual bool outputIsInverted(int);

	// check for wire-connection (this = potential connected wire), update device-connections
	int checkConnection(XWire *);

	void garbageCollection();

	// node is part of another wire as well but this owns it
	XWire * nodeIsBorrowedFrom(QPoint *);

	// select/unselect nodes of wire or the complete wire
	int select(QRect);	// select part of wire contained in rect
	void select(bool);	// select/deselect a complete wire

private:
	int id;

	// -----------------------------------------
	// methods to determine output-devices
	// -----------------------------------------
	void resetScan(bool start = false);	// wire-net loop prevention
	void scanAllOutputDev();		// first delete list, then ..
	void scanOutputDev();			// actualize list of output devices
	void removeOutputDev(XDevice *, int);	// remove actually changed output device
	void resetScanAdd(bool start = false);	// wire-net loop prevention
	void addOutputDev(XDevice *, int);	// add output device to all connected wires
	QColor getColor();
	static const bool START = true;
	// -----------------------------------------

	// -----------------------------------------
	// wire connections
	// -----------------------------------------
	// add wire-connection (node, logical conn, references in other wire)
	int connectWire(XWire *);
	int addWireReference(XWire *);
	// remove wire-connection (node, logical conn, references in other wire)
	void disconnectWire(XWire *);
	void removeWireReference(XWire *);
	// -----------------------------------------

	// -----------------------------------------
	// device connections
	// -----------------------------------------
	void setDeviceConnections(int type, int idx, XDevice *output_device = (XDevice *)NULL, int output_id = 0);
	void setDeviceConnection(int whatsnew, int type, int in_idx, XDevice *output_device, int output_id);
	static const int NEW_INPUT = 1;		// whatsnew
	static const int NEW_OUTPUT = 2;
	static const int CONNECT = 1;		// type
	static const int DISCONN = 2;
	// -----------------------------------------

	// number of output devices
	int checkShortcut();

	// connected input devices (at end of wire)
	XDevice *in_dev[2];
	int in_id[2];

	// connected output devices (at beginning of wire, input giving devices)
	XDevice *out_dev[2];
	int out_id[2];

	// wire output devices
	KlogicList<XDevice> wire_devs;	// content: wire_dev, id1: output_id of wire_dev

	XWire *io_wire[2];
	KlogicList<XWire> io_w;
	int scanned;
	int scan_add;

	Device *in_inv[2];
	int input_is_inv[2];

	XDevice *out_inv[2];
	int output_is_inv[2];

	static DeviceFactory *m_poDeviceFactory;
};

#endif

