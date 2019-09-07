/*************************************************/
/* methods for class XDevice                     */
/*                                               */
/* visible Device                                */
/*                                               */
/* Andreas Rostin                                */
/* 15.03.99                                      */
/*************************************************/
#include <qpainter.h>
#include <qpoint.h>

#include <stdio.h>
#include <string.h>
#include <math.h>

#include "xwire.h"
#include "xdevice.h"
#include "KlogicGlobal.h"
#include "calculator.h"
#include "value.h"
#include "DeviceType.h"
#include "busOutputArray.h"
#include "deviceText.h"
#include "grid.h"

/***************************************************/
/* static methods of XDevice class                 */
/***************************************************/
int XDevice::instance = 0;

BasicImageCache XDevice::m_oBasicImageCache;
ImageCache XDevice::m_oImageCache;

const QString BusOutputArray::IO_BUS_DATA[8] = {"D0", "D1", "D2", "D3", "D4", "D5", "D6", "D7"};

/***************************************************/
/* methods of XDevice instance                     */
/***************************************************/
XDevice::XDevice(int iFunction, const QPoint& oPos, int iSize)
        :XObject(oPos), DeviceInterface(iFunction)
{
    instance++;

    if (iSize > 0) {
        setSize(iSize);
    } else {
        if (Device::IMPORT_IGNORE_GLOBAL) {
            setSize(KlogicGlobal::Device::DEFAULT_SIZE);
        } else {
            setSize(KlogicGlobal::Device::getSize());
        }
    }

    if (Device::IMPORT_IGNORE_GLOBAL)
        deviceOutputInverted = false;
    else
        deviceOutputInverted = KlogicGlobal::Device::isInverted();

    maxInputCount = KlogicGlobal::Device::MAX_INPUT_CONNECTORS;
    maxOutputCount = KlogicGlobal::Device::MAX_OUTPUT_CONNECTORS;
    observed = true;
    m_poTextDev = 0;
    m_iTristateInputMoved = 0;
    m_iTristateImportPos = 0;

    m_oImageKey = ImageKey(type(), getSize(), width(), clock(), hasMaster(), KlogicGlobal::Device::getSymbolSet());
}

// remove device: remove all inputs and outputs first
XDevice::~XDevice()
{
    instance--;

    int idx;

    // remove all inputs
    while (m_oIncomingWires.First()) {
        idx = m_oIncomingWires.First()->Get()->getInputIndex(this);
        m_oIncomingWires.First()->Get()->disconnectInput(idx);
    }
    m_oIncomingWires.Destroy();

    // remove all outputs
    while (m_oOutgoingWires.First()) {
        idx = m_oOutgoingWires.First()->Get()->getOutputIndex(this);
        if (XWire::WFAIL == idx)
            qFatal("XDevice::~XDevice() this is unknown in a connected wire !?");
        m_oOutgoingWires.First()->Get()->disconnectOutput(idx);
    }
    m_oOutgoingWires.Destroy();

    if (m_poTextDev) delete m_poTextDev;
}

int XDevice::getSize()
{
    return m_iSize;
}

bool XDevice::setSize(int iSize)
{
    KlogicList<XWire> *lw;
    XWire *w;
    int iMinSize = KlogicGlobal::Device::MINSIZE;
    int iMaxY = 0;
    bool retval;

    lw = m_oIncomingWires.First();
    while (lw) {
        w = lw->Get();
        int idx = w->getInputIndex(this);
        if (w->getInputPosition(idx).y() - getPos().y() > iMaxY)
            iMaxY = w->getInputPosition(idx).y() - getPos().y();
        lw = lw->Next();
    }
    lw = m_oOutgoingWires.First();
    while (lw) {
        w = lw->Get();
        int idx = w->getOutputIndex(this);
        if (XWire::WFAIL == idx)
            qFatal("XDevice::setSize() this is unknown in a connected wire !?");
        if (w->getOutputPosition(idx).y() - getPos().y() > iMaxY)
            iMaxY = w->getOutputPosition(idx).y() - getPos().y();
        lw = lw->Next();
    }
    iMaxY = iMaxY / Grid::GRID;
    if (iMaxY > iMinSize) iMinSize = iMaxY;

    if (iSize > iMinSize) {
        if (getSize() != iSize) retval = true;
        else retval = false;
        m_iSize = iSize;
    } else {
        if (getSize() != iMinSize) retval = true;
        else retval = false;
        m_iSize = iMinSize;
    }
    m_oImageKey.setSize(iSize);
    return retval;
}

// size is 1 .. x and position is 2 .. x-1 so neededSize is max position + 1
int XDevice::neededSize()
{
    int iSize = KlogicGlobal::Device::MINSIZE;

    for (ConnectorList::iterator iter = m_oNamedOutput.begin(); iter != m_oNamedOutput.end(); iter++) {
        const Connector& oConnector = *iter;
        if (oConnector.getPosition() > iSize)
            iSize = oConnector.getPosition();
    }
    for (ConnectorList::iterator iter = m_oNamedInput.begin(); iter != m_oNamedInput.end(); iter++) {
        const Connector& oConnector = *iter;
        if (oConnector.getPosition() > iSize)
            iSize = oConnector.getPosition();
    }

    if (iSize < KlogicGlobal::Device::MINSIZE) {
    	iSize = KlogicGlobal::Device::MINSIZE;
    }
    return iSize + 1;
}

int XDevice::getMaxInputCount()
{
    return maxInputCount;
}

int XDevice::getCurrentInputCount()
{
    return m_oIncomingWires.counter();
}

void XDevice::setMaxInputCount(int newmax)
{
    if (newmax < getCurrentInputCount()) maxInputCount = getCurrentInputCount();
    else maxInputCount = newmax;
}

int XDevice::getMaxOutputCount()
{
    return maxOutputCount;
}

int XDevice::getCurrentOutputCount()
{
    return m_oOutgoingWires.counter();
}

void XDevice::setMaxOutputCount(int newmax)
{
    if (newmax < getCurrentOutputCount()) maxOutputCount = getCurrentOutputCount();
    else maxOutputCount = newmax;
}

// output inverted by default?
bool XDevice::isInverted()
{
    return deviceOutputInverted;
}

// output inverted by default?
// 1=invert always
void XDevice::setInverted(bool bEnable)
{
    deviceOutputInverted = bEnable;
}

// virtual
void XDevice::setName(const QString& sName)
{
    Device::setName(sName);

    if (nameDisplayed())
        m_poTextDev->setName(sName);
}

// show/hide the name of the display
void XDevice::displayName(bool bEnable)
{
    if (!getID())
        return;

    if (bEnable) {
        QString sText = getName();
        if (sText.isEmpty()) sText = " ";

        bool bWasEnabled = false;
        if (!m_poTextDev)
            m_poTextDev = new TextDevice(getPos());
        else
            bWasEnabled = true;

        m_poTextDev->setName(sText);

        if (!bWasEnabled) {
            QRect oDimensions = m_poTextDev->getGeometry();
            m_poTextDev->moveBy(QPoint(5, -1 * oDimensions.height() - 8));
        }
    } else {
        delete m_poTextDev;
        m_poTextDev = 0;
    }
}

// is the name of the device shown/hidden
bool XDevice::nameDisplayed()
{
    return (m_poTextDev != 0);
}

// set all output to tristate enabled
// add/remove "EN" input
void XDevice::enableTristateInput(bool bEnable)
{
    Device::setDeviceTristateEnabled(bEnable);

    // add/remove "EN" input if nessesary
    if (bEnable) {
      if (!hasTristateControlInput()) {
    	  int nextInputId = getNextInputId();
    	  if (nextInputId > getSize()) {
    		  setSize(nextInputId);
    	  }
		  addTristateControlInput(nextInputId);
      }
    } else {
        removeTristateControlInput();
        setSize(neededSize());
    }
    
    if (!Device::IMPORT_IGNORE_GLOBAL) setImage();
}

/*
 * Checks if the value of the output must be inverted and
 * returns the correct value for displaying it in the oscilloscope.
 * @param value The value to display
 * @param outputId The output ID to analyze
 * @return The original or the inverted value
 */
int XDevice::invertOutputValForObserver(int value, int iOutputID) {
    KlogicList<XWire> *lw = m_oOutgoingWires.First();
    int idx;

    if (!iOutputID) {
        while (lw) {
            if (!lw->Get()->outputIsInverted(this)) {
            	return value;
            }
            lw = lw->Next();
        }
        if (value == 0) {
        	return 1;
        } else {
        	return 0;
        }
    } else {
        while (lw) {
            idx = lw->Get()->getOutputIndex(this);
            if (XWire::WFAIL == idx)
                qFatal("XDevice::invertGraph() this is unknown in a connected wire !?");
            if (lw->Get()->outputID(idx) == iOutputID) {
                if (!lw->Get()->outputIsInverted(this)) return value;
                if (value == 0) return 1;
                else return 0;
            }
            lw = lw->Next();
        }
        // not found??
        return value;
    }
}

/*
 * Returns true if the device could be displayed in the oscilloscope.
 * @return true if the device could be displayed in the oscilloscope
 */
bool XDevice::isObservable() {
    return  getMaxOutputCount() > 0;
}

/*
 * Returns true if the device must be displayed in the oscilloscope.
 * @return true if the device must be displayed in the oscilloscope
 */
bool XDevice::isObserved() {
    return isObservable() && observed;
}

/*
 * Displayed device in the oscilloscope.
 * @param observed if true, the device is displayed, otherwiese it becomes hidden
 */
void XDevice::setObserved(bool observed) {
    this->observed = isObservable() && observed;
}

// garbage collection for connected wires
void XDevice::garbageCollection()
{
    KlogicList<XWire> *lw;

    lw = m_oIncomingWires.First();
    while (lw) {
        lw->Get()->Wire::garbageCollection();
        lw = lw->Next();
    }
    lw = m_oOutgoingWires.First();
    while (lw) {
        lw->Get()->Wire::garbageCollection();
        lw = lw->Next();
    }
}

// update nodes of wires which are connected to the device
void XDevice::updateWires(const QPoint& deltaPos)
{
    KlogicList<XWire> *lw = m_oIncomingWires.First();
    while (lw) {
        XWire *w = lw->Get();
        int idx = w->getInputIndex(this);
        w->setInputPosition(w->getInputPosition(idx) + deltaPos, idx);
        lw = lw->Next();
    }
    lw = m_oOutgoingWires.First();
    while (lw) {
        XWire *w = lw->Get();
        int idx = w->getOutputIndex(this);
        if (XWire::WFAIL == idx)
            qFatal("XDevice::updateWires() this is unknown in a connected wire !?");
        w->setOutputPosition(w->getOutputPosition(idx) + deltaPos, idx);
        lw = lw->Next();
    }
}

// checks, if actice-node of wire is input or output
// connects if given, or disconnects if not given
// wire must be locked!
int XDevice::checkConnection(XWire *poWire)
{
    int ret1, ret2;

    // active node of poWire must be the first or last node of poWire
    if (!poWire->activeIsEnd()) return DOK;

    // check, if wire is already input
    bool found = m_oIncomingWires.Get(poWire);
    // new connected, connection lost or not connected?
    ret1 = checkInput(poWire, found);
    if (ret1 != DOK && ret1 != DDISC) {
    	return ret1;
    }

    // check, if wire is already output
    found = m_oOutgoingWires.Get(poWire);

    // new connected, connection lost or not connected?
    ret2 = checkOutput(poWire, found);
    if (ret2 != DOK && ret2 != DDISC) {
    	return ret2;
    }

    if (ret1 == DDISC || ret2 == DDISC) {
    	return DDISC;
    }
    return DOK;
}

int XDevice::checkInput(XWire *poWire, bool bFound)
{
    if (!bFound) {
        QPoint oActiveNode = poWire->getActive();

        // check input connection
        if (!m_oInputRegion.contains(oActiveNode)) return DOK;

        // connections in all points allowed
        if (!hasNamedInput()) {
            return connectInput(poWire, oActiveNode);
        } else {
            // connections only in some points allowed (named inputs)
            for (ConnectorList::iterator iter = m_oNamedInput.begin(); iter != m_oNamedInput.end(); iter++) {
                int iNewID = (*iter).getConnectorID();
                int iPosition = (*iter).getPosition();
                QPoint oAllowedPt(getPos().x(), iPosition * Grid::GRID + getPos().y() + getVOffset());
                if (oAllowedPt == oActiveNode)
                    return connectInput(poWire, oActiveNode, iNewID);
            }
        }
    } else {
        // check input disconnection
        int idx = poWire->getInputIndex(this);
        QPoint oActiveNode = poWire->getInputPosition(idx);

        if (!hasNamedInput()) {
            if (!m_oInputRegion.contains(oActiveNode)) {
                poWire->disconnectInput(idx);
                return DDISC;
            }
        }
        else {
            int iNewID = 0;
            bool bIsConnected = false;
            ConnectorList::iterator iter = m_oNamedInput.begin();
            while (!bIsConnected && iter != m_oNamedInput.end()) {
                iNewID = (*iter).getConnectorID();
                int iPosition = (*iter).getPosition();
                QPoint oAllowedPt(getPos().x(), iPosition * Grid::GRID + getPos().y() + getVOffset());
                if (oAllowedPt == oActiveNode)
                    bIsConnected = true;
                iter++;
            }
            if (!bIsConnected) {
                poWire->disconnectInput(idx);
                return DDISC;
            } else {
                int iConnectionID = poWire->getConnectionID(this);
                if (iConnectionID != iNewID) {
                    poWire->disconnectInput(idx);
                    connectInput(poWire, oActiveNode, iNewID);
                }
            }
        }
    }
    return DOK;
}

// give wire the information to be an input to this device
// input-id: id of the named input of this device
int XDevice::connectInput(XWire *poWire, QPoint oActiveNode, int iInputID)
{
    if (XWire::WFAIL != poWire->activeIsInput()) return DOK;

    // check that another input is allowed
    if (getMaxInputCount() <= getCurrentInputCount())
        return DFAILMAXI;

    if (XWire::WFAIL == poWire->connectInput(this, false, iInputID))
        return DFAIL;

    // set the position of the input-node of the wire exactly to the connection-point of this device
    oActiveNode.setX(getPos().x());
    poWire->setInputPosition(oActiveNode);

    return DCONN;
}

int XDevice::checkOutput(XWire *poWire, bool bFound)
{
    if (!bFound) {
        QPoint oActiveNode = poWire->getActive();
        // check output connection
        if (!m_oOutputRegion.contains(oActiveNode)) {
            // fSWI became smaller .. workaround in order to load old files!
            if (type() == DeviceType::fSWI && Device::IMPORT_IGNORE_GLOBAL) {
                QRect oOutputRegion(m_oOutputRegion);
                oOutputRegion.setWidth(oOutputRegion.width() + 10);
                if (!oOutputRegion.contains(oActiveNode)) {
                    oOutputRegion.setHeight(oOutputRegion.height() + 10);
                    if (!oOutputRegion.contains(oActiveNode)) {
                        return DOK;
                    } else {
                        oActiveNode.setY(oActiveNode.y() - 10);
                        poWire->updateNode(oActiveNode);
                    }
                } else {
                    oActiveNode.setX(oActiveNode.y() - 10);
                    poWire->updateNode(oActiveNode);
                }
            } else {
                return DOK;
            }
        }

        if (!hasNamedOutput()) {
            if (XWire::WFAIL != poWire->activeIsOutput()) return DOK;
            // if the wire is not connected already, connect it now
            return connectOutput(poWire, oActiveNode);
        }
        // connections in some points allowed only
        else {
            ConnectorList::iterator iter = m_oNamedOutput.begin();
            iter++;
            while (iter != m_oNamedOutput.end()) {
                const Connector& oConnector = *iter;
                const Calculator *poCalculator = *oConnector;
                if (!poCalculator->isInternal()) {
                    int iNewID = oConnector.getConnectorID();
                    const QRect& oGeo = getGeometry();
                    QPoint oAllowedPt(oGeo.x() + oGeo.width() - 1,
                                      oConnector.getPosition() * Grid::GRID + oGeo.y() + getVOffset());
                    if (oAllowedPt == oActiveNode)
                        return connectOutput(poWire, oActiveNode, iNewID);
                }
                iter++;
            }
        }
    }
    // check output disconnection
    // if wire is connected, disconnect it now
    else {
        int idx = poWire->getOutputIndex(this);
        QPoint oActiveNode = poWire->getOutputPosition(idx);
        if (!hasNamedOutput()) {
            if (!m_oOutputRegion.contains(oActiveNode)) {
                poWire->disconnectOutput(idx);
                return DDISC;
            }
        }
        else {
            bool bIsConnected = false;
            int iNewID = 0;
            ConnectorList::iterator iter = m_oNamedOutput.begin();
            iter++;
            while (iter != m_oNamedOutput.end() && !bIsConnected) {
                const Connector& oConnector = *iter;
                const Calculator *poCalculator = *oConnector;
                if (!poCalculator->isInternal()) {
                    iNewID = oConnector.getConnectorID();
                    const QRect& oGeo = getGeometry();
                    QPoint oAllowedPt(oGeo.x() + oGeo.width() - 1,
                                      oConnector.getPosition() * Grid::GRID + oGeo.y() + getVOffset());
                    if (oAllowedPt == oActiveNode)
                        bIsConnected = true;
                }
                iter++;
            }
            if (!bIsConnected) {
                poWire->disconnectOutput(idx);
                return DDISC;
            } else {
                int iConnectionID = poWire->getConnectionID(this);
                if (iConnectionID != iNewID) {
                    poWire->disconnectOutput(idx);
                    return connectOutput(poWire, oActiveNode, iNewID);
                }
            }
        }
    }
    return DOK;
}

// give wire the information to get input from this (output-)device
// output-id: id of the named output of this device
int XDevice::connectOutput(XWire *poWire, QPoint oPos, int iOutputID)
{
    if (XWire::WFAIL != poWire->activeIsInput())
        return DOK;

    // check that another output is allowed
    if (getMaxOutputCount() <= getCurrentOutputCount())
        return DFAILMAXO;

    int ret = poWire->connectOutput(this, isInverted(), iOutputID);
    if (ret != WOK)
        return ret;

    oPos.setX(getGeometry().x() + getGeometry().width() - 1);
    poWire->setOutputPosition(oPos);

    return DCONN;
}

// called by XWire
// returns invertation of another wire connected in the same point
int XDevice::inputIsInvertedAt(QPoint conn_pt)
{
    KlogicList<XWire> *lw = m_oIncomingWires.First();
    while (lw) {
        if (conn_pt == lw->Get()->getInputPosition(0) || conn_pt == lw->Get()->getInputPosition(1)) {
            return lw->Get()->inputIsInverted(this);
        }
        lw = lw->Next();
    }
    return 0;
}

// called by XWire
// add wire to input-wire-list
// draw wire-connection into image of device
void XDevice::addInputWire(XWire *w)
{
    m_oIncomingWires.Append(w);
}

// called by XWire
// remove wire from input-wire-list
void XDevice::removeInputWire(XWire *w)
{
    m_oIncomingWires.Destroy(w);
}

// called by XWire
// returns invertation of another wire connected in the same point
int XDevice::outputIsInvertedAt(QPoint conn_pt)
{
    KlogicList<XWire> *lw = m_oOutgoingWires.First();
    while (lw) {
        if (conn_pt == lw->Get()->getOutputPosition(0) || conn_pt == lw->Get()->getOutputPosition(1)) {
            return lw->Get()->outputIsInverted(this);
        }
        lw = lw->Next();
    }
    return 0;
}

// called by XWire
// add wire to output-wire-list
void XDevice::addOutputWire(XWire *w)
{
    m_oOutgoingWires.Append(w);
}

// called by XWire
// remove wire from output-wire-list
void XDevice::removeOutputWire(XWire *w)
{
    m_oOutgoingWires.Destroy(w);
}

void XDevice::setTristateControlPos(int pos)
{
    Device::setDeviceTristateEnabled(true);
    m_iTristateImportPos = pos;
}

void XDevice::setUndefinedValue(int iUndef)
{
    Device::setUndefinedValue(iUndef);
}

// private, used for import
// shift existing inputs around that the tristate input "EN" can be inserted
void XDevice::prepareTristateInputImport()
{
    // look if given entry is free
    bool moved = false;
    for (ConnectorList::iterator iter = m_oNamedInput.begin(); iter != m_oNamedInput.end(); iter++) {
        Connector& oConnector = *iter;
        if (oConnector.getPosition() >= m_iTristateImportPos) {
            if (!moved) {
                moved = true;
                m_iTristateInputMoved++;
            }
            oConnector.setPosition(oConnector.getPosition() + 1);
        }
    }
}

// import method
void XDevice::setTristateInputImport()
{
    if (!hasTristateControlInput()) {
        setSize(neededSize() + 1);
        prepareTristateInputImport();	// shift input
        addTristateControlInput(m_iTristateImportPos);
    }
}

// called by device-net(-device)
// add an possible named input (no connection!)
int XDevice::addInputName(const QString& sName, int iPosition, int iDeviceID)
{
    bool bExist = false;
    int iTakePosition = iPosition;
    bool bSearch = (iPosition < 0);

    if (bSearch)
        iTakePosition = 0;

    // search for a free entry
    int iMaxPosition = getSize();
    do {
        bExist = false;
        for (ConnectorList::iterator iter = m_oNamedInput.begin(); iter != m_oNamedInput.end(); iter++) {
            const Connector& oConnector = *iter;
            if (oConnector.getPosition() == iTakePosition)
                bExist = true;
        }
        if (bExist)
            iTakePosition++;
    } while (bSearch && bExist && iTakePosition < iMaxPosition);

    if (bExist)
        return -1;

    // add input
    int iInputID = Device::addInputName(sName, iTakePosition, iDeviceID);
    if (!Device::IMPORT_IGNORE_GLOBAL) setImage();
    return iInputID;
}

// change an input name
// if the input does not exist, create a new named input
int XDevice::changeInputName(int iInputID, const QString& sName)
{
    if (!iInputID || !m_oNamedInput.find(iInputID))
        iInputID = addInputName(sName);
    else
        Device::changeInputName(iInputID, sName);
    if (!Device::IMPORT_IGNORE_GLOBAL) setImage();
    return iInputID;
}

// remove a named input
void XDevice::removeInputName(int iInputID)
{
    KlogicList<XWire> *lw = m_oIncomingWires.First();

    // remove all connections to this input
    while (lw) {
        if (lw->Get()->isNamedInput(this, iInputID)) {
            int idx = lw->Get()->getInputIndex(this);
            lw->Get()->disconnectInput(idx);
            lw = m_oIncomingWires.First();
        } else lw = lw->Next();
    }

    // remove input name
    Device::removeInputName(iInputID);
    if (!Device::IMPORT_IGNORE_GLOBAL) setImage();
}

// called by device-net(-device) and equation-device properties
// add an possible named output (no connection!)
int XDevice::addOutputName(const QString& sName, int iPosition, int iDeviceID)
{
    bool bExist = false;
    int iTakePosition = iPosition;
    bool bSearch = (iPosition < 0);

    if (bSearch)
        iTakePosition = 0;

    // ignore ...
    if (!sName.length() || !sName.compare("(null)"))
        return 0;

    // try to find an empty entry
    int iMaxPosition = getSize();
    do {
        bExist = false;
        ConnectorList::iterator iter = m_oNamedOutput.begin();
        while (iter != m_oNamedOutput.end()) {
            if (!(**iter)->isInternal() && (*iter).getPosition() == iTakePosition)
                bExist = true;
            iter++;
        }
        if (bExist)
            iTakePosition++;
    } while (bSearch && bExist && (iTakePosition < iMaxPosition));

    if (bExist)
        return -1;

    // add output
    int iOutputID = Device::addOutputName(sName, iTakePosition, iDeviceID);
    if (!Device::IMPORT_IGNORE_GLOBAL) setImage();
    return iOutputID;
}

int XDevice::changeOutputName(int iOutputID, const QString& sName)
{
    if (!iOutputID || !m_oNamedOutput[iOutputID])
        iOutputID = addOutputName(sName);
    else
        Device::changeOutputName(iOutputID, sName);
    if (!Device::IMPORT_IGNORE_GLOBAL) setImage();
    return iOutputID;
}

// called by device-net(-device) and equation-devices properties
// remove output name
void XDevice::removeOutputName(int iOutputID)
{
    KlogicList<XWire> *lw = m_oOutgoingWires.First();

    // remove all connections to this output
    while (lw) {
        if (lw->Get()->isNamedOutput(this, iOutputID)) {
            int idx = lw->Get()->getOutputIndex(this);
            if (XWire::WFAIL == idx)
                qFatal("XDevice::removeOutputName() this is unknown in a connected wire !?");
            lw->Get()->disconnectOutput(idx);
            lw = m_oOutgoingWires.First();
        } else lw = lw->Next();
    }

    // remove output name
    Device::removeOutputName(iOutputID);
    if (!Device::IMPORT_IGNORE_GLOBAL) setImage();
}

KlogicList<XWire> * XDevice::getIRef()
{
    return &m_oIncomingWires;
}

KlogicList<XWire> * XDevice::getORef()
{
    return &m_oOutgoingWires;
}

// get device image from image cache
// if not inside cache, create a new one
void XDevice::setImage()
{
    setImageKey();
    const Image *poCachedImage = m_oImageCache.getImage(m_oImageKey);
    if (poCachedImage) {
        XObject::setImage(poCachedImage);
        if (m_poTextDev)
            m_poTextDev->setImage();
        return;
    }

    Image *poImage = createImage(m_oImageKey);
    m_oImageCache.addImage(m_oImageKey, poImage);
    XObject::setImage(poImage);

    if (m_poTextDev)
        m_poTextDev->setImage();
}

Image* XDevice::createImage(const ImageKey&)
{
    qFatal("XDevice::createImage called - overload XDevice::setImage or XDevice::createImage !!");
    return NULL;
}

// absolute move of device
bool XDevice::moveTo(const QPoint& newpos)
{
    QPoint oOldPos = getPos();
    bool ret = XObject::moveTo(newpos);
    if (ret) {
        QPoint deltaPos = getPos() - oOldPos;
        if (m_poTextDev) {
            m_poTextDev->moveBy(deltaPos);
        }
        updateWires(deltaPos);
    }
    return ret;
}

// relative move of device
void XDevice::moveBy(const QPoint& deltaPos)
{
    XObject::moveBy(deltaPos);
    if (m_poTextDev)
        m_poTextDev->moveBy(deltaPos);
    updateWires(deltaPos);
}

// draw device to the screen
void XDevice::draw(QPaintDevice *paintd, QPainter *p) {
	updateImage();
    XObject::draw(paintd, p);
    if (m_poTextDev)
        m_poTextDev->draw(paintd, p);
}

void XDevice::updateImage() {
}

// draw device as lines and text to a printer device
void XDevice::draw(QPainter *p)
{
    const Image *poImage = getImage();

    // position
    int x = getPos().x();
    int y = getPos().y();

    x += XObject::IOREG;
    y -= getVOffset() / 2;

    // draw device rect
    QRect r;
    r.setX(x);
    r.setY(y);
    r.setWidth(poImage->width() - (XObject::IOREG * 2) - 1);
    r.setHeight(poImage->height());
    p->setPen(Qt::black);
    if (type() != DeviceType::fPWR && type() != DeviceType::fSWI) {
        p->drawRect(r);
    }
    printImage(p, r);
}

void XDevice::printImage(QPainter *p, const QRect& r)
{
    int picwidth = getImage()->width();
    // draw io-names and type specific symbol
    switch (type()) {
    case DeviceType::fAND:
        p->setFont( QFont( "helvetica", 16, QFont::Bold ) );
        p->drawText(r.x() + (picwidth / 2) - 8, r.y() + (r.height() / 2) + 5, "&");
        break;
    case DeviceType::fOR:
        p->setFont( QFont( "helvetica", 16, QFont::Bold ) );
        p->drawText(r.x() + (picwidth / 2) - 8, r.y() + (r.height() / 2) + 1, ">");
        p->drawText(r.x() + (picwidth / 2) - 8, r.y() + (r.height() / 2) + 1, "_");
        break;
    case DeviceType::fXOR:
        p->setFont( QFont( "helvetica", 16, QFont::Bold ) );
        p->drawText(r.x() + (picwidth / 2) - 8, r.y() + (r.height() / 2) + 3, "=");
        break;
    case DeviceType::fONE:
        p->setFont( QFont( "helvetica", 16, QFont::Bold ) );
        p->drawText(r.x() + (picwidth / 2) - 8, r.y() + (r.height() / 2) + 5, "1");
        break;
    default:
        break;
    }
}

void XDevice::init()
{
    switch (type()) {
    case DeviceType::fONE:
        deviceOutputInverted = true;
        setSize(KlogicGlobal::Device::MINSIZE);
        maxInputCount = 1;
        break;
    }

    if (!Device::IMPORT_IGNORE_GLOBAL) {
        setImage();
    }
    setEquation();
    parseEquation();
    if (KlogicGlobal::Device::isDisplayNames() && type() != DeviceType::fTXT)
        displayName(true);
}

