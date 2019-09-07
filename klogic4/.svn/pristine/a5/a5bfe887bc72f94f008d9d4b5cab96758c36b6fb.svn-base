/*************************************************/
/* methods for class Circuit                  */
/*                                               */
/* widget containing a circuit                   */
/* actions with/at objects in the net-widget     */
/*                                               */
/* Andreas Rostin                                */
/* 15.03.99                                      */
/*************************************************/
#include <qfile.h>
#include <qtextstream.h>
#include <qregexp.h>
#include <qstring.h>
#include <qpoint.h>
#include <qpaintdevice.h>

#include "KlogicGlobal.h"
#include "FileIO.h"
#include "CircuitWidget.h"
#include "circuit.h"
#include "xwire.h"
#include "devLib.h"
#include "selection.h"
#include "xmlExport.h"
#include "DeviceType.h"
#include "deviceFactory.h"
#include "deviceMuxDemux.h"
#include "deviceOsc.h"
#include "deviceRAM.h"
#include "deviceJKFF.h"
#include "performance.h"
#include "Simulator.h"

/***************************************************/
/* methods of Circuit                           */
/***************************************************/
int Circuit::instance = 0;
int Circuit::magic_version = 0;	// to be removed soon!
DeviceFactory * Circuit::m_poDeviceFactory = DeviceFactory::getInstance();
int Circuit::m_iCalcCalls = 0;
int Circuit::m_iPropCalls = 0;

Circuit::Circuit(const QPoint& oPos, int iSize)
        :CustomDevice(DeviceType::fSUB, oPos, iSize)
{
    instance++;
    parentCircuit = 0;
	Simulator::getInstance()->circuitAdd(this);
}

Circuit::~Circuit() {
	Simulator::getInstance()->circuitRemove(this);
    instance--;
    deleteAll();
}

void Circuit::deleteAll()
{
    // destroy all devices
    for (DeviceList::iterator iter = devices.begin(); iter != devices.end(); ++iter)
        delete iter.getDevice();
    devices.clear();

    // destroy all wires
    KlogicList<XWire> *w = wires.First();
    while (w) {
        delete w->Get();
        w = w->Next();
    }

    // destroy lists
    wires.Destroy();

    initImport();
}

bool Circuit::hasImpossibleEquations()
{
    for (DeviceList::iterator iter = devices.begin(); iter != devices.end(); ++iter) {
        XDevice *poDevice = iter.getDevice();
        if (poDevice->type() == DeviceType::fRAM || poDevice->isDeviceTristateEnabled())
            return true;
    }
    return false;
}

// returns 1 if all names are unique, otherwise 0
int Circuit::checkCircuitNames()
{
    for (DeviceList::iterator iter = devices.begin(); iter != devices.end(); ++iter) {
        // sub circuits ...
        XDevice *poDevice = iter.getDevice();
        Circuit *poCircuit = poDevice->devIsCircuit();
        if (poCircuit && !poCircuit->checkCircuitNames())
            return 0;

        // devices ...
        for (DeviceList::iterator iter2 = devices.begin(); iter2 != devices.end(); ++iter2) {
            if (iter2.getDevice() != poDevice)
                if (poDevice->getName() == iter2.getDevice()->getName())
                    return 0;
        }
    }
    return 1;
}

// change all duplicate names to be unique
int Circuit::unifyCircuitNames()
{
    for (DeviceList::iterator iter = devices.begin(); iter != devices.end(); ++iter) {
        // sub circuits ...
        XDevice *poDevice = iter.getDevice();
        Circuit *poCircuit = poDevice->devIsCircuit();
        if (poCircuit && !poCircuit->unifyCircuitNames())
            return 0;

        // devices ...
        for (DeviceList::iterator iter2 = devices.begin(); iter2 != devices.end(); ++iter2) {
            if (iter2.getDevice() != poDevice) {
                if (poDevice->getName() == iter2.getDevice()->getName()) {
                    QString buffer;
                    buffer.sprintf("uni%d", iter2.getDevice()->getID());
                    setDeviceName(iter2.getDevice()->getID(), buffer);
                }
            }
        }
    }
    return 1;
}

void Circuit::getAllEquations(map<QString,OutputInfo>& oEquations, const QString& sQPrefix, bool bIsolate)
{
    QString sNewPrefix = sQPrefix + getName() + ".";
    sNewPrefix.replace(QRegExp(" "), "_" );

    for (DeviceList::iterator iter = devices.begin(); iter != devices.end(); ++iter) {
        Device *dev = iter.getDevice();
        dev->getAllEquations(oEquations, sNewPrefix, bIsolate);
    }
}

Circuit * Circuit::rootParent()
{
    Circuit *top;

    top = this;
    while (top->parent()) top = top->parent();
    return top;
}

void Circuit::setParent(XDevice *parent)
{
    parentCircuit = parent->devIsCircuit();
}

Circuit * Circuit::parent()
{
    return parentCircuit;
}

DeviceList::iterator Circuit::getDeviceIterator()
{
    return devices.begin();
}

DeviceList::iterator Circuit::getDeviceEndIterator()
{
    return devices.end();
}

bool Circuit::empty()
{
    return (!devices.size() && !wires.First());
}

void Circuit::applyDefaults()
{
    setDelay(KlogicGlobal::Device::getDelay());
    setInverted(KlogicGlobal::Device::isInverted());
    setWidth(KlogicGlobal::Device::getWidth());
    setUndefinedValue(KlogicGlobal::Device::getUndefinedValue());
    displayName(KlogicGlobal::Device::isDisplayNames());
    setImage();

    for (DeviceList::iterator iter = devices.begin(); iter != devices.end(); ++iter) {
        XDevice *poDevice = iter.getDevice();
        Circuit *poCircuit = poDevice->devIsCircuit();
        if (poCircuit) poCircuit->applyDefaults();
        else {
            poDevice->setDelay(KlogicGlobal::Device::getDelay());
            poDevice->setInverted(KlogicGlobal::Device::isInverted());
            poDevice->setWidth(KlogicGlobal::Device::getWidth());
            poDevice->setUndefinedValue(KlogicGlobal::Device::getUndefinedValue());
            poDevice->displayName(KlogicGlobal::Device::isDisplayNames());
            poDevice->setEquation();
            poDevice->parseEquation();
            setDeviceName(poDevice->getID(), poDevice->getName());
            poDevice->setImage();
        }
    }
}

void Circuit::mapImportID(int iOldDeviceID, int iNewDeviceID)
{
    XDevice *poInputDev = inputDevices[iNewDeviceID];
    if (poInputDev) {
        mapInputName(iOldDeviceID, iNewDeviceID, poInputDev->getName());
    }

    XDevice *poOutputDev = outputDevices[iNewDeviceID];
    if (poOutputDev) {
        mapOutputName(iOldDeviceID, iNewDeviceID, poOutputDev->getName());
    }
}

void Circuit::setDeviceName(int iDeviceID, const QString& sName)
{
    XDevice *poDevice = devices[iDeviceID];
    if (poDevice) {
        poDevice->setName(sName);
        if ((poDevice->type() == DeviceType::fIN || poDevice->type() == DeviceType::fOUT) && !sName.isEmpty() && sName.length()) {
            // update names within circuit image
            if (poDevice->type() == DeviceType::fIN) {
                if (!m_oNamedInput.setName(iDeviceID, sName))
                    qWarning() << "Circuit::setName - no such input";
            }
            if (poDevice->type() == DeviceType::fOUT) {
                if (!m_oNamedOutput.setName(iDeviceID, sName))
                    qWarning() << "Circuit::setName - no such output";
            }
            if (!Device::IMPORT_IGNORE_GLOBAL) setImage();
        }
    }
}

bool Circuit::hasBooleanOutput(int output_id) const
{
    map<int, bool>::const_iterator iter = m_mHasBooleanOutput.find(output_id);
    if (iter != m_mHasBooleanOutput.end())
        return iter->second;
    return true;
}

void Circuit::setHasBooleanOutput(int output_id, bool bHasBooleanInput)
{
    m_mHasBooleanOutput[output_id] = bHasBooleanInput;
}

bool Circuit::garbageCollection()
{
    bool retCode = FALSE;
    KlogicList<XWire> *lw = wires.First();

    while (lw) {
        lw->Get()->Wire::garbageCollection();
        if (!wireOK(lw->Get())) {
            lw = wires.First();
	    retCode = TRUE;
        } else {
            lw = lw->Next();
        }
    }
    return retCode;
}

//*************************************************************************
//* drawing methods
//*************************************************************************

// draw all devices and wires in the net
// if paintd is NULL then drawing device is a printer
void Circuit::draw(QPaintDevice *paintd, QPainter *p) {
	for (DeviceList::iterator iter = devices.begin(); iter != devices.end(); ++iter) {
		if (paintd) {
			iter.getDevice()->draw(paintd, p);
		} else {
			iter.getDevice()->draw(p);
		}
	}

	KlogicList<XWire> *lw = wires.First();
	while (lw) {
		lw->Get()->draw(p);
		lw = lw->Next();
	}
}

// force setting status dependant images
void Circuit::forceOutputChange()
{
    Device::forceOutputChange();
    for (DeviceList::iterator iter = devices.begin(); iter != devices.end(); ++iter)
        iter.getDevice()->forceOutputChange();
}

void Circuit::setUndefinedValue(int iUndefinedValue)
{
    for (DeviceList::iterator iter = outputDevices.begin(); iter != outputDevices.end(); ++iter)
        iter.getDevice()->setUndefinedValue(iUndefinedValue);
    Device::setUndefinedValue(iUndefinedValue);
}

//*************************************************************************
//* device methods
//*************************************************************************

// create new device
XDevice * Circuit::createDevice(int iFunction, const QPoint& oPos, int iSize) {
    return m_poDeviceFactory->createDevice(this, iFunction, oPos, iSize);
}

// add new device
bool Circuit::addChild(XDevice *dev, bool bImport)
{
    int type = dev->type();
    if (type == DeviceType::fIN || type == DeviceType::fOUT) {
        QString text = dev->getName();
        if (type == DeviceType::fIN) {
            if (!bImport) {
                if (addInputName(text, -1, dev->getID()) < 0)
                    return false;
            }
            inputDevices.add(dev, dev->getID());
        }
        if (type == DeviceType::fOUT) {
            if (!bImport) {
                QString text = dev->getName();
                if (addOutputName(text, -1, dev->getID()) < 0)
                    return false;
            }
            dev->setUndefinedValue(getUndefinedValue());
            outputDevices.add(dev, dev->getID());
        }
    }
    devices.add(dev, dev->getID());

    return true;
}

// virtual
// returns pointer to the net-device if device is a net-device
Circuit * Circuit::devIsCircuit()
{
    return this;
}

// look for a device containing point
XDevice * Circuit::checkDeviceCollision(QPoint p) {
    for (DeviceList::iterator iter = devices.begin(); iter != devices.end(); ++iter) {
        if (iter.getDevice()->contains(p))
            return iter.getDevice();
    }
    return 0;
}

// look for a device containing device
XDevice * Circuit::checkDeviceCollision(XDevice *dev)
{
    for (DeviceList::iterator iter = devices.begin(); iter != devices.end(); ++iter) {
        XDevice *poDevice = iter.getDevice();
        if (poDevice != dev) {
            if (poDevice->contains(dev->getPos())) {
                return poDevice;
            }
        }
    }
    return 0;
}

// mark all devices in rect as selected
void Circuit::selectDev(QRect r, KlogicList<XDevice> *d)
{
    for (DeviceList::iterator iter = devices.begin(); iter != devices.end(); ++iter) {
        XDevice *poDevice = iter.getDevice();
        if (poDevice->contains(r)) {
            if (!d->With(poDevice))
                d->Append(poDevice);
            poDevice->select(true);
        }
        else {
            d->Destroy(poDevice);
            poDevice->select(false);
        }
    }
}

// called from class Selection
// add all selected devices to the selection (paste operation)
void Circuit::grabSelection(Selection *sel)
{
    for (DeviceList::iterator iter = devices.begin(); iter != devices.end(); ++iter) {
        XDevice *poDevice = iter.getDevice();
        if (poDevice->isSelected())
            sel->add(poDevice);
    }

    KlogicList<XWire> *lw = wires.First();
    while (lw) {
        if (lw->Get()->isSelected()) sel->add(lw->Get());
        lw = lw->Next();
    }
}

// delete a device
void Circuit::deleteDevice(XDevice *dev)
{
    if (dev->type() == DeviceType::fIN) {
        inputDevices.remove(dev->getID());
        removeInputName(dev->getID());
    }
    if (dev->type() == DeviceType::fOUT) {
        outputDevices.remove(dev->getID());
        removeOutputName(dev->getID());
    }

    // erase bitmap from screen
    devices.remove(dev->getID());
    delete dev;
}

//*************************************************************************
//* wire methods
//*************************************************************************

// look for a wire containing point
XWire * Circuit::checkWireCollision(QPoint p)
{
    KlogicList<XWire> *l = wires.First();

    while (l) {
        // if wire contains point, the node will be locked automaticly
        // if not, a new node is created
        if (l->Get()->contains(p)) return l->Get();
        l = l->Next();
    }
    return (XWire *)NULL;
}

// look for a wire containing (parts of) rect and select them
void Circuit::selectWire(QRect r, KlogicList<XWire> *w)
{
    KlogicList<XWire> *l = wires.First();

    while (l) {
        // catch nodes, mark them as selected
        if (l->Get()->select(r)) {
            if (!w->With(l->Get())) w->Append(l->Get());
        }
        else {
            w->Destroy(l->Get());
        }
        l = l->Next();
    }
}

int Circuit::removeNode(QPoint pt) {
    // can remove more than one node if required
    int ret = 0;
    KlogicList<XWire> *lw = wires.First();
    while (lw) {
    	XWire *wire = lw->Get();
        bool wireListModified = false;
        if (wire->contains(pt)) {
            wire->removeNode();
            ret++;
            if (!wireOK(wire)) {
                lw = wires.First();
                wireListModified = true;
            }
		}
        if (!wireListModified) {
        	lw = lw->Next();
        }
    }

//    lw = netwire.First();
//	while (lw) {
//		lw->Get()->debug();
//		lw = lw->Next();
//	}

    return ret;
}

// look for a device or wire, connected to given wire
// connections will be made automaticly on active node of wire
void Circuit::checkConnection(XWire *w)
{
    // look for device connection/disconnection
    for (DeviceList::iterator iter = devices.begin(); iter != devices.end(); ++iter) {
        iter.getDevice()->checkConnection(w);
    }
    // look for wire connection/disconnection
    KlogicList<XWire> *lw = wires.First();
    while (lw) {
    	lw->Get()->checkConnection(w);
        lw = lw->Next();
    }
}

// look for a device, connected to given wire
// (used as import-method only)
void Circuit::checkConnection(int devId, XWire *w)
{
    if (!devices[devId]) {
        return;
    }
    devices[devId]->checkConnection(w);
}

XDevice * Circuit::getDevice(int devId) {
	return devices[devId];
}

bool Circuit::hasShortcut() {
    KlogicList<XWire> *lw = wires.First();
    while (lw) {
    	if (lw->Get()->getStatus() == Wire::SHORTCUT) {
    		return true;
    	}
        lw = lw->Next();
    }
    return false;
}

// create a new wire (import)
XWire *Circuit::newWire()
{
    XWire *w = new XWire();
    wires.Append(w, w->getID());
    return w;
}

// get a wire (import)
XWire *Circuit::getWire(int id)
{
    return wires.Get(id);
}

// create a new wire
XWire *Circuit::newWire(QPoint pt)
{
    // create new wire and its first node
    XWire *w = new XWire(pt);

    // look for connection in the first point
    checkConnection(w);
    // append new wire to wirelist
    wires.Append(w, w->getID());
    return w;
}

// no wires with single nodes allowed
bool Circuit::wireOK(XWire *w)
{
    if (w->countNodes() < 2) {
    	if (w->lockLastNode()) {
    		XWire *w2 = w->removeNode();
			if (w2) {
				wireOK(w2);
			}
    	}

        // remove nodeless wire
        wires.Destroy(w);
        delete w;
        return FALSE;
    }
    return TRUE;
}

// wire released after clicked with the mouse
bool Circuit::releaseWire(XWire *w)
{
    w->Wire::garbageCollection();

    // check, that wire has at leat two nodes
    if (!wireOK(w)) return FALSE;
    w->releaseNode();
    return TRUE;
}

// delete wire
void Circuit::deleteWire(XWire *w)
{
    wires.Destroy(w);
    delete w;
}

//*************************************************************************
//* calculation methods
//*************************************************************************

// set net to init-state (flush all calculation-fifos)
void Circuit::flush()
{
    for (DeviceList::iterator iter = devices.begin(); iter != devices.end(); ++iter)
        iter.getDevice()->flush(0);
}

// calculate burst
// usually, burst will be 1 that means single calculation and propagation
// delays are reduced until no delay is left when burst is greater than 1
void Circuit::Burst(int burst)
{
    /*
    	Performance p;
    	m_iCalcCalls = 0;
    	m_iPropCalls = 0;
    	p.start();
    */
    // calculate circuit <burst> times
    for (int burst_step = 0; burst_step < burst; ++burst_step) {
        Calculate(burst_step);
        Propagate(burst_step);
    }
    /*
    	p.end();
    	cout << "measurement complete:" << endl;
    	cout << "\t" << m_iCalcCalls << " calls to ::Calculate" << endl;
    	cout << "\t" << m_iPropCalls << " calls to ::Propagate" << endl;
    	cout << "\t" << "time (ms) " << p.getMilliSeconds() << endl;
    */
}

// switch input- or output devices of this
void Circuit::switchInterface(int if_type)
{
    // set output-values of input-devices by requesting named inputs of this sub circuit
    // input devices are part of this, and connected to named inputs of this
    if (if_type == IF_INPUT) {
        for (DeviceList::iterator iter = inputDevices.begin(); iter != inputDevices.end(); ++iter) {
            XDevice *oInputDev = iter.getDevice();
            // set input device from the named input value
            bool bIsBoolean;
            int iVal = input(oInputDev->getID(), bIsBoolean);
            oInputDev->setInputValue(iVal);
            oInputDev->setHasBooleanOutput(0, bIsBoolean);
        }
    }

    // set output values of named outputs by requesting output devices
    // output devices are part of this, and named outputs are connected to them
    if (if_type == IF_OUTPUT) {
        for (DeviceList::iterator iter = outputDevices.begin(); iter != outputDevices.end(); ++iter) {
            XDevice *oOutputDev = iter.getDevice();
            // set the named output value from output device
            setOutputValue(oOutputDev->getID(), oOutputDev->output());
            setHasBooleanOutput(oOutputDev->getID(), oOutputDev->hasBooleanOutput(0));
        }
    }
}

// virtual
// calculate all devices of this circuit and all sub circuits
void Circuit::Calculate(int burst_step) {
    m_iCalcCalls++;
    switchInterface(IF_INPUT);
    for (DeviceList::iterator iter = devices.begin(); iter != devices.end(); ++iter) {
    	if (iter.getDevice()->type() != DeviceType::fSUB) {
			m_iCalcCalls++;
			iter.getDevice()->Calculate(burst_step);       // recursive by virtual
    	}
    }
}

// virtual
// propagate values to the outputs of this circuit and all sub circuits
bool Circuit::Propagate(int burst_step) {
    m_iPropCalls++;
    bool outputHasChanged = false;
    for (DeviceList::iterator iter = devices.begin(); iter != devices.end(); ++iter) {
    	if (iter.getDevice()->type() != DeviceType::fSUB) {
			m_iPropCalls++;
			outputHasChanged |= iter.getDevice()->Propagate(burst_step);
			setOutputChanged(outputHasChanged);
    	}
    }
    switchInterface(IF_OUTPUT);
    return outputHasChanged;
}

/***************************************************/
/* import/export operations on a net-widget        */
/***************************************************/
// XML export method for library devices: export to XmlDevice
bool Circuit::exportXML(XmlNet& net)
{
    return exportXML((QIODevice *)NULL, &net, false, 0, 0, 0);
}

// XML export method for files: write directly to the file
bool Circuit::exportXML(QIODevice &ioDev, bool selected, int level, int dx, int dy)
{
    // top level circuit with selected flag ignored!
    XmlNet net(this, false, level, dx, dy);
    return exportXML(&ioDev, &net, selected, level, dx, dy);
}

// XML export method
bool Circuit::exportXML(QIODevice *ioDev, XmlNet *net, bool selected, int level, int dx, int dy)
{
    bool ownNet = (net == (XmlNet *)NULL);

    // circuit parameter and header
    if (ownNet) {
        net = new XmlNet(this, selected, level, dx, dy);
        if (!net->isValid()) {
            delete net;
            return false;
        }
    }

    if (ioDev) {
        ioDev->write(net->getFieldStart().toUtf8());
        ioDev->write(net->getFieldContent().toUtf8());
        level++;
    }

    // circuit wires
    KlogicList<XWire> *lw = wires.First();
    while (lw) {
        XmlWire wire(lw->Get(), selected, level, dx, dy);
        if (wire.isValid()) {
            if (ioDev) ioDev->write(wire.getField().toUtf8());
            else net->addContent(wire.getField());
        }

        lw = lw->Next();
    }

    // circuit devices
    for (DeviceList::iterator iter = devices.begin(); iter != devices.end(); ++iter) {
        Circuit *poCircuit = iter.getDevice()->devIsCircuit();
        if (poCircuit) {
            XmlNet subCircuit(poCircuit, selected, level, dx, dy);
            if (subCircuit.isValid()) {
                // for sub-circuits always set selected=false!
                poCircuit->exportXML(ioDev, &subCircuit, false, level, 0, 0);	// recursion!

                // add net content (if ioDev avail: content already added)
                if (!ioDev) net->addContent(subCircuit.getField());
            }
        } else {
            XmlDevice dev(iter.getDevice(), selected, level, dx, dy);
            if (dev.isValid()) {
                if (ioDev) ioDev->write(dev.getField().toUtf8());
                else net->addContent(dev.getField());
            }
        }
    }

    // circuit wire <-> wire connections
    lw = wires.First();
    while (lw) {
        XmlWireConnection wire_conn(lw->Get(), selected, level, dx, dy);
        if (wire_conn.isValid()) {
            if (ioDev) ioDev->write(wire_conn.getField().toUtf8());
            else net->addContent(wire_conn.getField());
        }

        lw = lw->Next();
    }

    // circuit device <-> wire connections
    lw = wires.First();
    while (lw) {
        XmlDeviceConnection dev_conn(lw->Get(), selected, level, dx, dy);
        if (dev_conn.isValid()) {
            if (ioDev) ioDev->write(dev_conn.getField().toUtf8());
            else net->addContent(dev_conn.getField());
        }

        lw = lw->Next();
    }

    if (ioDev) {
        level--;
        ioDev->write(net->getFieldEnd().toUtf8());
    }

    if (ownNet) {
        delete net;
    }
    return true;
}


