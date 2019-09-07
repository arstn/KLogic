#include <stdlib.h>

#include <qstring.h>
#include <qxml.h>
#include <qbuffer.h>

#include "KlogicGlobal.h"
#include "circuit.h"
#include "xdevice.h"
#include "DeviceType.h"
#include "deviceOsc.h"
#include "xwire.h"
#include "xmlExport.h"
#include "xmlImport.h"
#include "xmlSymbolMap.h"
#include "klogicList.h"

/*******************************************/
/* XML Export Interface                    */
/*                                         */
/* Andreas Rostin 23.08.2001               */
/*******************************************/
const char * XmlObject::ATT_CIRCUIT = "CIRCUIT";
const char * XmlObject::ATT_SUBCIRCUIT = "SUBCIRCUIT";
const char * XmlObject::ATT_LIBRARY = "LIBRARY";

XmlObject::XmlObject(QString name)
	:XmlField(name, 0)
{
	dx = 0;
	dy = 0;
	_valid = true;
}

XmlObject::XmlObject(QString name, int level, int _dx, int _dy)
	:XmlField(name, level)
{
	dx = _dx;
	dy = _dy;
	_valid = false;
}

// virtual
XmlObject::~XmlObject()
{
}

// private virtual; to be redefined..
void XmlObject::createXML(bool)
{
	return;
}

// return value can be both, a circuit or a device
// use XDevice::devIsNet() to retrieve Circuit pointer from XDevice
XDevice * XmlObject::createInstance(Circuit *poCircuit, int dx, int dy)
{
	QString content = getField();

	// xml reader does not work with string based textstreams!
	QBuffer f;
	f.open(QFile::WriteOnly|QFile::Truncate);
	QTextStream ts3(&f);
	ts3 << content;
	f.close();
	f.open(QFile::ReadOnly);
	return XMLImportHandler::import(poCircuit, dx, dy, f);
}

void XmlObject::setValid(bool __valid)
{
	_valid = __valid;
}

bool XmlObject::isValid()
{
	return _valid;
}

/*******************************************/
/* XML representation of a circuit         */
/*                                         */
/* because a circuit can consume a lot of  */
/* memory, this export is mainly done by   */
/* the Circuit itself!                  */
/* see FileIO::writeXML() for details    */
/*                                         */
/* Andreas Rostin 24.08.2001               */
/*******************************************/
XmlNet::XmlNet(Circuit *poCircuit, bool selected, int level, int dx, int dy)
	: XmlObject(XmlSymbolMap::m_oMap[XmlSymbol::CIRCUIT], level, dx, dy)
{
	m_poCircuit = poCircuit;
	createXML(selected);
}

XmlNet::XmlNet()
	: XmlObject(XmlSymbolMap::m_oMap[XmlSymbol::CIRCUIT], 0, 0, 0)
{
	m_poCircuit = (Circuit *)NULL;
}

void XmlNet::createXML(bool selected)
{
	if (selected && !m_poCircuit->isSelected()) return;
	setValid(true);
	addAttribute(XmlSymbolMap::m_oMap[XmlSymbol::ID], m_poCircuit->getID());
	addAttribute(XmlSymbolMap::m_oMap[XmlSymbol::FUNCTION], m_poCircuit->type());
	addAttribute(XmlSymbolMap::m_oMap[XmlSymbol::SIZE], m_poCircuit->getSize());
	int x = m_poCircuit->getPos().x() - m_poCircuit->getHOffset() - dx;
	if (x < 0) x = 0;
	int y = m_poCircuit->getPos().y() + m_poCircuit->getVOffset() - dy;
	if (y < 0) y = 0;
	addAttribute(XmlSymbolMap::m_oMap[XmlSymbol::XPOS], x);
	addAttribute(XmlSymbolMap::m_oMap[XmlSymbol::YPOS], y);

	XmlDevice dev(m_poCircuit, selected, -1, dx, dy);
	setContent(dev.getFieldContent());
}

/*******************************************/
/* XML representation of a connection      */
/*     between devices and wires           */
/*                                         */
/* Andreas Rostin 24.08.2001               */
/*******************************************/
XmlDeviceConnection::XmlDeviceConnection(XWire *_wire, bool selected, int level, int dx, int dy)
	: XmlObject(XmlSymbolMap::m_oMap[XmlSymbol::DEV_CONN], level, dx, dy)
{
	wire = _wire;
	createXML(selected);
}

void XmlDeviceConnection::createXML(bool selected)
{	int inverter;

	if (selected && !wire->isSelected()) return;

	QPoint pi1 = wire->getInputPosition(0, selected);
	QPoint pi2 = wire->getInputPosition(1, selected);
	QPoint po1 = wire->getOutputPosition(0, selected);
	QPoint po2 = wire->getOutputPosition(1, selected);
	QPoint pt;
	XDevice *dev;

	// maximum of 2 points
	if (!pi1.isNull() || !pi2.isNull() || !po1.isNull() || !po2.isNull()) {
		setValid(true);
		addAttribute(XmlSymbolMap::m_oMap[XmlSymbol::ID], wire->getID());

		if (!pi1.isNull()) {
			dev = wire->inputDevice(0);
			inverter = wire->inputIsInverted(dev);
			pt.setX(pi1.x() - dx);
			pt.setY(pi1.y() - dy);
			setField(dev, pt, inverter);
		}
		if (!pi2.isNull()) {
			dev = wire->inputDevice(1);
			inverter = wire->inputIsInverted(dev);
			pt.setX(pi2.x() - dx);
			pt.setY(pi2.y() - dy);
			setField(dev, pt, inverter);
		}
		if (!po1.isNull()) {
			dev = wire->outputDevice(0);
			inverter = wire->outputIsInverted(dev);
			pt.setX(po1.x() - dx);
			pt.setY(po1.y() - dy);
			setField(dev, pt, inverter);
		}
		if (!po2.isNull()) {
			dev = wire->outputDevice(1);
			inverter = wire->outputIsInverted(dev);
			pt.setX(po2.x() - dx);
			pt.setY(po2.y() - dy);
			setField(dev, pt, inverter);
		}
	}
}

void XmlDeviceConnection::setField(XDevice *dev, QPoint pt, int inverter)
{	XmlField field;

	field.set(XmlSymbolMap::m_oMap[XmlSymbol::POS], pt);
	field.addAttribute(XmlSymbolMap::m_oMap[XmlSymbol::ID], dev->getID());
	field.addAttribute(XmlSymbolMap::m_oMap[XmlSymbol::INV], inverter);
	addContent(field.getField());
}

/*******************************************/
/* XML representation of a connection      */
/*            between wires                */
/*                                         */
/* Andreas Rostin 24.08.2001               */
/*******************************************/
XmlWireConnection::XmlWireConnection(XWire *_wire, bool selected, int level, int dx, int dy)
	: XmlObject(XmlSymbolMap::m_oMap[XmlSymbol::WIRE_CONN], level, dx, dy)
{
	wire = _wire;
	createXML(selected);
}

void XmlWireConnection::createXML(bool selected)
{	XmlField field;

	if (selected && !wire->isSelected()) return;

	QPoint pi = wire->getIOPosition(1, selected);
	QPoint po = wire->getIOPosition(0, selected);
	QPoint pt;

	if ((!pi.isNull()) || (!po.isNull())) {
		setValid(true);
		addAttribute(XmlSymbolMap::m_oMap[XmlSymbol::ID], wire->getID());

		if (!pi.isNull()) {
			pt.setX(pi.x() - dx);
			pt.setY(pi.y() - dy);
			
			field.set(XmlSymbolMap::m_oMap[XmlSymbol::POS], pt);
			field.addAttribute(XmlSymbolMap::m_oMap[XmlSymbol::ID], wire->ioWire(1)->getID());
			addContent(field.getField());
		}

		if (!po.isNull()) {
			pt.setX(po.x() - dx);
			pt.setY(po.y() - dy);
			field.set(XmlSymbolMap::m_oMap[XmlSymbol::POS], pt);
			field.addAttribute(XmlSymbolMap::m_oMap[XmlSymbol::ID], wire->ioWire(0)->getID());
			addContent(field.getField());
		}
	}
}

/*******************************************/
/* XML representation of a XWire           */
/*                                         */
/* Andreas Rostin 23.08.2001               */
/*******************************************/
XmlWire::XmlWire(XWire *_wire, bool selected, int level, int dx, int dy)
	: XmlObject(XmlSymbolMap::m_oMap[XmlSymbol::WIRE], level, dx, dy)
{
	wire = _wire;
	createXML(selected);
}

void XmlWire::createXML(bool selected)
{	QString _content;

	if (selected && !wire->isSelected()) return;
	setValid(true);
 
	// wire properties
	addAttribute(XmlSymbolMap::m_oMap[XmlSymbol::ID], wire->getID());

	// wire content
	wire->setExportSelected(selected);
	wire->setExportDelta(dx, dy);
	*wire >> _content;

	setContent(_content);
}

/*******************************************/
/* XML representation of a XDevice         */
/*                                         */
/* Andreas Rostin 23.08.2001               */
/*******************************************/
XmlDevice::XmlDevice(XDevice *_dev, bool selected, int level, int dx, int dy)
	: XmlObject(XmlSymbolMap::m_oMap[XmlSymbol::DEVICE], level, dx, dy)
{
	dev = _dev;
	createXML(selected);
}

XmlDevice::XmlDevice()
	: XmlObject(XmlSymbolMap::m_oMap[XmlSymbol::DEVICE], 0, 0, 0)
{
	dev = (Circuit *)NULL;
}

void XmlDevice::createXML(bool selected)
{	XmlField field;
	QString text;

	if (selected && !dev->isSelected()) return;
	setValid(true);

	// device attributes
	addAttribute(XmlSymbolMap::m_oMap[XmlSymbol::ID], dev->getID());
	addAttribute(XmlSymbolMap::m_oMap[XmlSymbol::FUNCTION], dev->type());
	addAttribute(XmlSymbolMap::m_oMap[XmlSymbol::SIZE], dev->getSize());
	addAttribute(XmlSymbolMap::m_oMap[XmlSymbol::XPOS], dev->getPos().x() - dev->getHOffset() - dx);
	addAttribute(XmlSymbolMap::m_oMap[XmlSymbol::YPOS], dev->getPos().y() + dev->getVOffset() - dy);

	// device content 
	text = dev->getName();
	field.set(XmlSymbolMap::m_oMap[XmlSymbol::TEXT], XmlField::quote(text));
	addContent(field.getField());

	if (dev->nameDisplayed() != KlogicGlobal::Device::DEFAULT_DISPLAY_NAMES) {
		field.set(XmlSymbolMap::m_oMap[XmlSymbol::TEXT_DISP], "");
		addContent(field.getField());
	}

	if (dev->getDelay() != KlogicGlobal::Device::DEFAULT_DELAY) { 
		field.set(XmlSymbolMap::m_oMap[XmlSymbol::DELAY], dev->getDelay());
		addContent(field.getField());
	}

	if (dev->getUndefinedValue() != KlogicGlobal::Device::DEFAULT_UNDEFINED_VALUE) { 
		field.set(XmlSymbolMap::m_oMap[XmlSymbol::UNDEF], dev->getUndefinedValue());
		addContent(field.getField());
	}
 
	if (dev->hasStaticInput()) {
		field.set(XmlSymbolMap::m_oMap[XmlSymbol::STAT], dev->output());
		addContent(field.getField());
	}
 
	// flipflop properties
	if (dev->hasClock()) {
		field.set(XmlSymbolMap::m_oMap[XmlSymbol::CLK_TYPE], dev->clock());
		addContent(field.getField());

		field.set(XmlSymbolMap::m_oMap[XmlSymbol::MASTER], dev->hasMaster());
		addContent(field.getField());
	}

	// switch properties
	if (dev->isToggle()) {
		field.set(XmlSymbolMap::m_oMap[XmlSymbol::PUSH]);
		addContent(field.getField());
	}

	// width property: possible only for equ, circuit
	if (dev->width() != KlogicGlobal::Device::DEFAULT_WIDTH) {
		field.set(XmlSymbolMap::m_oMap[XmlSymbol::WIDTH], dev->width());
		addContent(field.getField());
	}

	// default inverted output
	if (dev->isInverted() != KlogicGlobal::Device::DEFAULT_INVERTED) {
		field.set(XmlSymbolMap::m_oMap[XmlSymbol::INV]);
		addContent(field.getField());
	}

	// oscillator properties
	if (dev->type() == DeviceType::fOSC) {
		field.set(XmlSymbolMap::m_oMap[XmlSymbol::CLK_TYPE], dev->getOscType());
		addContent(field.getField());

		if (dev->getOscOn() != OscillatorDevice::DEFAULT_OSC_PERIOD) {
			field.set(XmlSymbolMap::m_oMap[XmlSymbol::OSC_ON], dev->getOscOn());
			addContent(field.getField());
		}
	 
		if (dev->getOscOff() != OscillatorDevice::DEFAULT_OSC_PERIOD) {
			field.set(XmlSymbolMap::m_oMap[XmlSymbol::OSC_OFF], dev->getOscOff());
			addContent(field.getField());
		}
	 
		field.set(XmlSymbolMap::m_oMap[XmlSymbol::OSC_STEP], dev->getCurrOscStep());
		addContent(field.getField());
	}

	// tristate flag
	if (dev->isDeviceTristateEnabled()) {
		field.set(XmlSymbolMap::m_oMap[XmlSymbol::TRI], dev->getTristateControlInputPosition());
		addContent(field.getField());
	}

	// named output
	if (dev->hasNamedOutput() && dev->type() != DeviceType::fRAM) {
		ConnectorList::iterator iter = dev->getNamedOutputIterator();
		iter++;
		while (iter != dev->getNamedOutputEndIterator()) {
			Connector& oConnector = *iter;
			Calculator *poCalculator = *oConnector;
			if (dev->type() == DeviceType::fEQU) {
				text = poCalculator->getEquation();
				field.set(XmlSymbolMap::m_oMap[XmlSymbol::OUT_EQU], XmlField::quote(text));
				field.addAttribute(XmlSymbolMap::m_oMap[XmlSymbol::POS], oConnector.getPosition());
				field.addAttribute(XmlSymbolMap::m_oMap[XmlSymbol::INTERN], poCalculator->isInternal());
				text = oConnector.getName();
				field.addAttribute(XmlSymbolMap::m_oMap[XmlSymbol::NAME], XmlField::quote(text));
			} else {
				//qDebug() << "output " << oConnector.getName() << " pos=" << oConnector.getPosition() << " id=" << oConnector.getConnectorID();
				field.set(XmlSymbolMap::m_oMap[XmlSymbol::OUT_ID], oConnector.getConnectorID());
				field.addAttribute(XmlSymbolMap::m_oMap[XmlSymbol::POS], oConnector.getPosition());
				field.addAttribute(XmlSymbolMap::m_oMap[XmlSymbol::INTERN], poCalculator->isInternal());
			}

			addContent(field.getField());
			iter++;
		}
	}

	// named input 
	if (dev->hasNamedInput()) {
		ConnectorList::iterator iter = dev->getNamedInputIterator();
		while (iter != dev->getNamedInputEndIterator()) {
			Connector& oConnector = *iter;
			if (dev->type() == DeviceType::fEQU) {
				text = oConnector.getName();
				field.set(XmlSymbolMap::m_oMap[XmlSymbol::IN_NAME], XmlField::quote(text));
				field.addAttribute(XmlSymbolMap::m_oMap[XmlSymbol::POS], oConnector.getPosition());
			} else {
				field.set(XmlSymbolMap::m_oMap[XmlSymbol::IN_ID], oConnector.getConnectorID());
				field.addAttribute(XmlSymbolMap::m_oMap[XmlSymbol::POS], oConnector.getPosition());
			}

			addContent(field.getField());
			iter++;
		}
	}

	// ram content
	if (dev->type() == DeviceType::fRAM) {
		field.set(XmlSymbolMap::m_oMap[XmlSymbol::RAM], dev->RAMSize(), dev->getRAMStorageCopy());
		addContent(field.getField());
	}

	// bus type
	if (dev->type() == DeviceType::fBUS) {
		field.set(XmlSymbolMap::m_oMap[XmlSymbol::BUSTYPE], dev->isMux());
		addContent(field.getField());
	}

	// color (LED devices)
	if (dev->getColor()) {
		field.set(XmlSymbolMap::m_oMap[XmlSymbol::COLOR], dev->getColor());
		addContent(field.getField());
	}
}

