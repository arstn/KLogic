/*************************************************/
/* methods for class XWire                       */
/*                                               */
/* visible wire                                  */
/*                                               */
/* Andreas Rostin                                */
/* 15.03.99                                      */
/*************************************************/
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "xwire.h"
#include "device.h"
#include "DeviceType.h"
#include "KlogicGlobal.h"

/***************************************************/
/* methods of XWire (connection between devices)  */
/***************************************************/
int XWire::instance = 0;
DeviceFactory * XWire::m_poDeviceFactory = DeviceFactory::getInstance();

XWire::XWire()
        :Wire()
{
    instance++;

    for (int i=0; i < 2; i++) {
        in_dev[i] = (XDevice *)NULL;		// devices, to which output from out_dev is delivered to
        in_id[i] = 0;
        in_inv[i] = 0;
        input_is_inv[i] = 0;

        out_dev[i] = (XDevice *)NULL;		// devices which may deliver input for this wire
        out_id[i] = 0;
        out_inv[i] = 0;
        output_is_inv[i] = 0;

        io_wire[i] = (XWire *)NULL;
    }

    id = uniqueID::getID();
    scanned = 1;
    scan_add = 1;
}

XWire::XWire(QPoint p)
        :Wire(p)
{
    instance++;

    for (int i=0; i < 2; i++) {
        in_dev[i] = (XDevice *)NULL;		// devices, to which output from out_dev is delivered to
        in_id[i] = 0;
        in_inv[i] = 0;
        input_is_inv[i] = 0;

        out_dev[i] = (XDevice *)NULL;		// devices which may deliver input for this wire
        out_id[i] = 0;
        out_inv[i] = 0;
        output_is_inv[i] = 0;

        io_wire[i] = (XWire *)NULL;
    }

    id = uniqueID::getID();
    scanned = 1;
    scan_add = 1;
}

XWire::~XWire()
{
    instance--;

    // remove all nodes from the wire
    while (lockLastNode()) {
        removeNode();
    }

    if (in_inv[0]) delete in_inv[0];
    if (in_inv[1]) delete in_inv[1];
    if (out_inv[0]) delete out_inv[0];
    if (out_inv[1]) delete out_inv[1];

    io_w.Destroy();
}

// get output device of this wire
XDevice * XWire::outputDevice(int idx)
{
    return out_dev[idx];
}

// get output id of this wire
int XWire::outputID(int idx)
{
    return out_id[idx];
}

// get input device
XDevice * XWire::inputDevice(int idx)
{
    return in_dev[idx];
}

// get output wire
XWire * XWire::ioWire(int idx)
{
    if (idx == 0 || idx == 1) return io_wire[idx];
    return (XWire *)NULL;
}

// look if a node lies near p
bool XWire::contains(QPoint p)
{
    // Wire returns a list!
    return (Wire::contains(p) != NULL);
}

// draw active node and all affected parts of other wires
void XWire::draw(QPainter *p)
{
    Wire::draw(p, getColor());
}

// selection of that part of wire contained in rect
int XWire::select(QRect r)
{
    return Wire::select(r);
}

// selection/deselection of a wire
void XWire::select(bool yes)
{
    QRect r;	// not needed

    // call Wire method
    if (yes) Wire::select(r, true);
    else Wire::unselect();
}

// return the wire which references the node
XWire *XWire::nodeIsBorrowedFrom(QPoint *pt)
{
    KlogicList<XWire> *l = io_w.First();

    while (l) {
        if (l->Get()->nodeIsBorrowed(pt) != WFAIL) {
        	return l->Get();
        }
        l = l->Next();
    }
    return (XWire *)NULL;
}

int XWire::getInputIndex(XDevice *d)
{
    if (d == in_dev[0]) return 0;
    if (d == in_dev[1]) return 1;
    return WFAIL;
}

int XWire::getOutputIndex(XDevice *d)
{
    if (d == out_dev[0]) return 0;
    if (d == out_dev[1]) return 1;
    return WFAIL;
}

int XWire::getConnectionID(XDevice *d)
{
    int idx = getInputIndex(d);
    if (idx != WFAIL) {
      return in_id[idx];
    }
    idx = getOutputIndex(d);
    if (idx != WFAIL) {
      return out_id[idx];
    }

    KlogicList<XDevice> *ld = wire_devs.With(d);
    if (ld) return ld->getID1();
    return 0;
}

// lock connected (if connected) node in other wires
int XWire::createNode()
{
    return Wire::createNode();
}

// device-wire-connections: first diconnect devices, then remove node
// wire-wire-connections  : make a disconnect from the wire which holds a copy
XWire * XWire::removeNode()
{
    // consider wire connections and remove them first
    XWire *w = ioWire(activeIsBorrowed());
    if (!w) {
    	w = nodeIsBorrowedFrom(getActiveNode());
    }
    if (w) {
        w->lockNode(getActiveNode());	// lock node in other wire as well
        disconnectWire(w);
        return w;
    }

    // remove device connections
    int idx;
    if (WFAIL != (idx = activeIsInput()))
        disconnectInput(idx);

    if (WFAIL != (idx = activeIsOutput()))
        disconnectOutput(idx);

    // remove point
    Wire::removeActiveNode();
    return (XWire *)NULL;
}

// returns error if connected already, 1 if succesful
// pay attention to the inverters on device-level!
// if invert is set, invertation is forced
int XWire::connectInput(XDevice *dev, bool invert, int device_id)
{
    int idx;

    // it happens from time to time, that a wire
    // has two nodes in the same position
    // each node should be connected to protect
    // the wire against garbageCollections!
    bool bSlot0Connected = out_dev[0] || in_dev[0] || io_wire[0];
    bool bSlot1Connected = out_dev[1] || in_dev[1] || io_wire[1];

    // find empty index
    if (!bSlot0Connected)
        idx = 0;
    else if (!bSlot1Connected)
        idx = 1;
    else {
        return WFAIL;
    }

    // connect active point
    Wire::connectInput(idx);

    // remember the device, connected to
    in_dev[idx] = dev;
    in_id[idx] = device_id;
    input_is_inv[idx] = invert;
    in_dev[idx]->addInputWire(this);

    setDeviceConnections(CONNECT, idx);
    return WOK;
}

// if called from device: attention!! reference will be destroyed in device!!
// pay attention to the inverters on device-level!
void XWire::disconnectInput(int idx)
{
    // if not two devices are connected anymore, logical disconnection
    setDeviceConnections(DISCONN, idx);

    // remove reference to this wire in device
    in_dev[idx]->removeInputWire(this);

    // remove point-reverence, connected to
    Wire::disconnectInput(idx);

    // remove references to object which was connected
    in_dev[idx] = (XDevice *)NULL;
    in_id[idx] = 0;
}

// connect to output of a device (logical, physical, references)
// pay attention to the inverters on device-level!
// if invert is 1, invertation is forced
// if invert is 0, the default is used
// if invert is -1, non-invertation is forced
int XWire::connectOutput(XDevice *dev, bool invert, int device_id)
{
    int idx;

    // it happens from time to time, that a wire
    // has two nodes in the same position
    // each node should be connected to protect
    // the wire against garbageCollections!
    bool bSlot0Connected = out_dev[0] || in_dev[0] || io_wire[0];
    bool bSlot1Connected = out_dev[1] || in_dev[1] || io_wire[1];

    // find empty index
    if (!bSlot0Connected)
        idx = 0;
    else if (!bSlot1Connected)
        idx = 1;
    else {
        cerr << "XWire::connectOutput() connection failed!\n" << endl;
        return WFAIL;
    }

    // connect active point
    Wire::connectOutput(idx);

    // remember the device, connected to
    out_dev[idx] = dev;
    out_id[idx] = device_id;
    output_is_inv[idx] = invert;
    out_dev[idx]->addOutputWire(this);

    resetScan(START);
    scanOutputDev();	// find/distribute all output devices

    if (checkShortcut() == WSHORT) {
        disconnectOutput(idx);
        return WSHORT;
    }

    return WOK;
}

// disconnect output of a device
// pay attention to the inverters on device-level!
void XWire::disconnectOutput(int idx)
{
    // remove reference to this wire in device
    out_dev[idx]->removeOutputWire(this);

    // remove point-reference (not longer output-point)
    Wire::disconnectOutput(idx);

    resetScan(START);
    if (output_is_inv[idx])
        removeOutputDev(out_inv[idx], 0);
    else
        removeOutputDev(out_dev[idx], out_id[idx]);

    // remove references to object which was connected
    out_dev[idx] = (XDevice *)NULL;
    out_id[idx] = 0;
}

void XWire::invertInput(XDevice *d, bool set)
{
    int idx;

    if (WFAIL == (idx = getInputIndex(d)))
        qFatal("XWire::invertInput: unknown device");

    // nothing to do?
    if (input_is_inv[idx] && set) return;
    if (!input_is_inv[idx] && !set) return;

    setDeviceConnections(DISCONN, idx);
    input_is_inv[idx] = set;
    setDeviceConnections(CONNECT, idx);
}

bool XWire::inputIsInverted(XDevice *dev)
{
    int idx;

    if (WFAIL == (idx = getInputIndex(dev))) qFatal("XWire::inputIsInverted: unknown device");
    return input_is_inv[idx];
}

bool XWire::inputIsInverted(int idx)
{
    return input_is_inv[idx];
}

void XWire::invertOutput(XDevice *dev, bool set)
{
    int idx;

    if (WFAIL == (idx = getOutputIndex(dev))) qFatal("XWire::invertOutput: unknown device");

    // nothing to do?
    if (output_is_inv[idx] && set) return;
    if (!output_is_inv[idx] && !set) return;

    // remove output device from whole wire network
    resetScan(START);
    if (output_is_inv[idx])
        removeOutputDev(out_inv[idx], 0);
    else
        removeOutputDev(out_dev[idx], out_id[idx]);

    // set new inverter flag
    if (set) output_is_inv[idx] = 1;
    else output_is_inv[idx] = 0;

    resetScan(START);
    scanOutputDev();	// find/distribute all output devices
}

bool XWire::outputIsInverted(XDevice *dev)
{
    int idx;

    if (WFAIL == (idx = getOutputIndex(dev))) qFatal("XWire::outputIsInverted: unknown device");
    return output_is_inv[idx];
}

bool XWire::outputIsInverted(int idx)
{
    return output_is_inv[idx];
}

// type: CONNECT or DISCON
void XWire::setDeviceConnections(int type, int idx, XDevice *output_device, int output_id)
{
    if (idx == 0 || idx == 1) {
        if (in_dev[idx]) {
            KlogicList<XDevice> *ld = wire_devs.First();
            if (!ld) setDeviceConnection(NEW_INPUT, type, idx, (XDevice *)NULL, 0);
            while (ld) {
                setDeviceConnection(NEW_INPUT, type, idx, ld->Get(), ld->getID1());
                ld = ld->Next();
            }
        }
    }

    if (idx == -1 && output_device) {
        if (in_dev[0]) setDeviceConnection(NEW_OUTPUT, type, 0, output_device, output_id);
        if (in_dev[1]) setDeviceConnection(NEW_OUTPUT, type, 1, output_device, output_id);
        // inverter only
        if (!in_dev[0] && !in_dev[1]) setDeviceConnection(NEW_OUTPUT, type, -1, output_device, output_id);
    }
}

// new input device: connect devices
void XWire::setDeviceConnection(int whatsnew, int type, int in_idx, XDevice *output_device, int output_id)
{
    int output_inv_idx = -1;

    if (output_device) {
        if (output_device == out_inv[0]) output_inv_idx = 0;
        if (output_device == out_inv[1]) output_inv_idx = 1;
    }

    // input inverter connection/disconnection
    if (!output_device || whatsnew == NEW_INPUT) {
        if (input_is_inv[in_idx]) {
            if (type == CONNECT && !in_inv[in_idx])
                in_inv[in_idx] = m_poDeviceFactory->createDevice(0, DeviceType::fINV_INTERNAL, QPoint(0,0), -1);

            if (type == CONNECT) in_dev[in_idx]->connectDevices(in_inv[in_idx], in_id[in_idx], 0);
            if (type == DISCONN) in_dev[in_idx]->disconnectDevices(in_inv[in_idx], in_id[in_idx], 0);
        }
    }

    if (!output_device)
        return;

    // output inverter connection/disconnection
    if (in_idx == -1 || whatsnew == NEW_OUTPUT) {
        if (output_inv_idx != -1) {
            if (type == CONNECT) output_device->connectDevices(out_dev[output_inv_idx], output_id, out_id[output_inv_idx]);
            if (type == DISCONN) output_device->disconnectDevices(out_dev[output_inv_idx], output_id, out_id[output_inv_idx]);
        }
    }

    if (in_idx == -1)
        return;

    // device connection/disconnection
    if (input_is_inv[in_idx]) {
        if (type == CONNECT && !in_inv[in_idx])
            in_inv[in_idx] = m_poDeviceFactory->createDevice(0, DeviceType::fINV_INTERNAL, QPoint(0,0), -1);

        if (type == CONNECT) in_inv[in_idx]->connectDevices(output_device, 0, output_id);
        if (type == DISCONN) in_inv[in_idx]->disconnectDevices(output_device, 0, output_id);
    } else {
        if (type == CONNECT) in_dev[in_idx]->connectDevices(output_device, in_id[in_idx], output_id);
        if (type == DISCONN) in_dev[in_idx]->disconnectDevices(output_device, in_id[in_idx], output_id);
    }
}

// check if name is an input
// xdevice is usually a net device
int XWire::isNamedInput(XDevice *dev, int dev_id)
{
    if (in_dev[0] == dev && in_id[0] == dev_id) return 1;
    if (in_dev[1] == dev && in_id[1] == dev_id) return 1;
    return 0;
}

// check if name is an output
// xdevice is usually a net device
int XWire::isNamedOutput(XDevice *dev, int dev_id)
{
    if (out_dev[0] == dev && out_id[0] == dev_id) return 1;
    if (out_dev[1] == dev && out_id[1] == dev_id) return 1;
    return 0;
}

// checks, if actice-node of wire w is connected to this wire
// w is the active wire
// this is the foreign wire, the wire to check and perhaps in future the connected wire
int XWire::checkConnection(XWire *w)
{
	if (w == this) {
		return WOK;
	}
    // active node of w must be the first or last node of w
    if (!w->activeIsEnd()) return WOK;

    // dont connect, if active node is an input/output-node in w (already connected)
    if ((w->activeIsInput() != WFAIL) || (w->activeIsOutput() != WFAIL)) return WOK;

    // don't connect if active node is connected
    // (direct wire-loop not allowed)
    if (!io_w.Get(w) && io_wire[0] != w && io_wire[1] != w) {
        // check wire-connection, lock node
        // on success: insert/lock foreign node from w
        if (!Wire::checkWireWireConnection(w->getActiveNode())) {
        	return WOK;
        }
        // connected to other wire - rescan wire input
        return connectWire(w);
    }
    return WOK;
}

// w is the owner of the node
// this now has a pointer to the foreign node of w
int XWire::connectWire(XWire *w)
{
    // remember the connected wire(!)
    io_w.Append(w);

    // no of output-devices must be 0 or 1
    // reset all wires including the new one
    resetScan(START);

    // remember this wire in the other (active) wire
    if (WOK != w->addWireReference(this)) {
        io_w.Destroy(w);
        scanOutputDev();
        w->scanOutputDev();
        return WFAIL;
    }

    // add output devices from other wire to list of output devices
    // establish new device connections
    resetScan(START);
    scanOutputDev();

    // check that only one output is active
    if (checkShortcut() == WSHORT) {
        disconnectWire(w);
        return WSHORT;
    }

    return WCONN;
}

// called by wire which contains a copy of the node
// node must be locked
int XWire::addWireReference(XWire *new_conn)
{
    // it happens from time to time, that a wire
    // has two nodes in the same position
    // each node should be connected to protect
    // the wire against garbageCollections!
    bool bSlot0Connected = out_dev[0] || in_dev[0] || io_wire[0];
    bool bSlot1Connected = out_dev[1] || in_dev[1] || io_wire[1];

    // add reference to the connected wire
    if (!bSlot0Connected) {
        io_w.Append(new_conn);
        Wire::connectWire(0);
        io_wire[0] = new_conn;
    } else if (!bSlot1Connected) {
        io_w.Append(new_conn);
        Wire::connectWire(1);
        io_wire[1] = new_conn;
    } else return WFAIL;

    return WOK;
}

// remove wire-wire connection
// owner/copy information is not relevant
void XWire::disconnectWire(XWire *w)
{
    // remove/destroy the locked nodes
    Wire::removeActiveNode();
    w->Wire::removeActiveNode();

    // node removed - remove references in both wires
    removeWireReference(w);
    w->removeWireReference(this);
}

// node must be locked in the wire
// owner/copy information is not relevant
void XWire::removeWireReference(XWire *old_conn)
{
    // remove wire reference
    io_w.Destroy(old_conn);

    if (io_wire[0] == old_conn) {
        // remove wire reference
        io_wire[0] = (XWire *)NULL;
    } else if (io_wire[1] == old_conn) {
        // remove wire reference
        io_wire[1] = (XWire *)NULL;
    }

    scanAllOutputDev();
}

// determine no of output-devices
int XWire::checkShortcut()
{
    KlogicList<XDevice> *ld;
    int dev_count = wire_devs.counter();

    if (dev_count > 1) {
        ld = wire_devs.First();
        while (ld) {
            XDevice *d = ld->Get();
            int output_id = ld->getID1();

            // one normal device and others (tristate or normal) ...
            if (!(d->type() == DeviceType::fSUB && d->output(output_id) == KlogicGlobal::Device::TRISTATE) &&
                    !(d->isOutputTristateEnabled(output_id)))
                return WSHORT;
            ld = ld->Next();
        }
    }
    return 0;
}

// remove all existing output devices, then perform a rescan of them
// (wire-wire disconnection)
void XWire::scanAllOutputDev()
{
    KlogicList<XDevice> *ld = wire_devs.First();

    while (ld) {
        resetScan(START);
        removeOutputDev(ld->Get(), ld->getID1());
        ld = ld->First();
    }
    wire_devs.Destroy();

    resetScan(START);
    scanOutputDev();
}

// determine output-devices; establish device-device connections
// be sure you have called resetScan() before
void XWire::scanOutputDev()
{
    if (!scanned) {
        scanned = 1;

        if (out_dev[0]) {
            if (output_is_inv[0]) {
                resetScanAdd(START);
                if (!out_inv[0])
                    out_inv[0] = m_poDeviceFactory->createDevice(0, DeviceType::fINV_INTERNAL, QPoint(0,0), -1);
                addOutputDev(out_inv[0], 0);
            } else {
                resetScanAdd(START);
                addOutputDev(out_dev[0], out_id[0]);
            }
        }

        if (out_dev[1]) {
            if (output_is_inv[1]) {
                resetScanAdd(START);
                if (!out_inv[1])
                    out_inv[1] = m_poDeviceFactory->createDevice(0, DeviceType::fINV_INTERNAL, QPoint(0,0), -1);

                addOutputDev(out_inv[1], 0);
            } else {
                resetScanAdd(START);
                addOutputDev(out_dev[1], out_id[1]);
            }
        }

        if (io_wire[0])
            io_wire[0]->scanOutputDev();

        if (io_wire[1])
            io_wire[1]->scanOutputDev();

        KlogicList<XWire> *lw = io_w.First();
        while (lw) {
            lw->Get()->scanOutputDev();
            lw = lw->Next();
        }
    }
}

// remove output-device in this and all connected wires
// be sure you have called resetScan() before
void XWire::removeOutputDev(XDevice *dev, int dev_id)
{
    if (!scanned) {
        scanned = 1;

        if (wire_devs.With(dev, dev_id)) {
            setDeviceConnections(DISCONN, -1, dev, dev_id);
            wire_devs.Destroy(dev, dev_id);
        }

        KlogicList<XWire> *lw = io_w.First();
        while (lw) {
            lw->Get()->removeOutputDev(dev, dev_id);
            lw = lw->Next();
        }
    }
}

void XWire::addOutputDev(XDevice *dev, int dev_id)
{
    if (!scan_add) {
        scan_add = 1;

        if (!wire_devs.With(dev, dev_id)) {
            wire_devs.Append(dev, dev_id);
            setDeviceConnections(CONNECT, -1, dev, dev_id);
        }

        KlogicList<XWire> *lw = io_w.First();
        while (lw) {
            lw->Get()->addOutputDev(dev, dev_id);
            lw = lw->Next();
        }
    }
}

void XWire::resetScanAdd(bool start)
{
    if (scan_add || start) {
        scan_add = 0;
        KlogicList<XWire> *lw = io_w.First();
        while (lw) {
            lw->Get()->resetScanAdd();
            lw = lw->Next();
        }
    }
}

// needed to prevent loops
void XWire::resetScan(bool start)
{
    if (start|| scanned) {
        scanned = 0;
        KlogicList<XWire> *lw = io_w.First();
        while (lw) {
            lw->Get()->resetScan();
            lw = lw->Next();
        }
        if (io_wire[0])
            io_wire[0]->resetScan();
        if (io_wire[1])
            io_wire[1]->resetScan();
    }
}

QColor XWire::getColor() {
	setStatus(Wire::UNCONNECTED);
	setLineSize(1);

	KlogicList<XDevice> *ld = wire_devs.First();
	int theOutputID = -1;
	XDevice *theDevice = NULL;
    while (ld) {
    	XDevice *device = ld->Get();
        int outputID = ld->getID1();
        // check here if output is active (tristate)
        if (device->outputIsActive(outputID)) {
            if (theOutputID != -1) {
                setStatus(Wire::SHORTCUT);
            } else {
				setStatus(Wire::CONNECTED);
				theOutputID = outputID;
				theDevice = device;
            }
        }
        ld = ld->Next();
    }

    if (getStatus() == Wire::SHORTCUT) {
    	return Qt::cyan;
    }
    if (getStatus() == Wire::CONNECTED) {
    	// boolean output or many-state? (bus device) set thickness
		if (theDevice->hasBooleanOutput(theOutputID)) {
			setLineSize(1);
		} else {
			setLineSize(3);
		}
		int outputValue = theDevice->output(theOutputID);
    	if (outputValue == KlogicGlobal::Device::TRISTATE) {
        	return Qt::black;
    	} else if (outputValue) {
        	return Qt::red;
        } else {
        	return Qt::blue;
        }
    }
    return Qt::black;
}

int XWire::getID()
{
    return id;
}
