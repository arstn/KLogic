#include <stdlib.h>

#include <qstring.h>
#include <qxml.h>
#include <QDebug>

#include "KlogicGlobal.h"
#include "circuit.h"
#include "xdevice.h"
#include "DeviceType.h"
#include "xwire.h"
#include "xmlImport.h"
#include "xmlExport.h"
#include "xmlSymbolMap.h"

/*******************************************/
/* very restriced XML line parser          */
/* (used only for libraries)               */
/*                                         */
/* Andreas Rostin 25.09.2001               */
/*******************************************/
void XmlLineParser::setLine(const QString& line)
{	QString __tag;

	// start tag
	int tagstart1 = line.indexOf("<");
	if (tagstart1 == -1) {
		sTag.truncate(0);
		attributes.truncate(0);
		content = line;
		endTag = false;
		return;
	}
	int tagstart2 = line.indexOf("</");

	int tagend1 = line.indexOf(">", 0);
	if (tagend1 == -1) {
		sTag.truncate(0);
		attributes.truncate(0);
		content = line;
		endTag = false;
		return;
	}

	// end tag only
	if (tagstart1 == tagstart2) {
		sTag = line.mid(tagstart2 + 2, line.length() - tagstart2 - 3);
		iTag = XmlSymbolMap::m_oMap[sTag];
		endTag = true;
		content = line.left(tagstart2);
		return;
	}

	// start tag only
	if (tagstart2 == -1) {
		__tag = line.mid(tagstart1 + 1, tagend1 - tagstart1 - 1);

		// check for attributes
		int pos = __tag.indexOf(" ");
		if (pos != -1) {
			sTag = __tag.left(pos);
			attributes = __tag.right(__tag.length() - pos - 1);
		} else {
			sTag = __tag;
			attributes.truncate(0);
		}

		// check for combined start/end tag
		if (sTag[sTag.length() - 1] == '/') {
			sTag = sTag.left(sTag.length() - 1);
			endTag = true;
		} else endTag = false;

		iTag = XmlSymbolMap::m_oMap[sTag];
		content = line.right(line.length() - tagend1 - 1);
		return;
	}

	// start and end tag
	__tag = line.mid(tagstart1 + 1, tagend1 - tagstart1 - 1);
	int pos = __tag.indexOf(" ");
	if (pos != -1) {
		sTag = __tag.left(pos);
		attributes = __tag.right(__tag.length() - pos - 1);
	} else {
		sTag = __tag;
		attributes.truncate(0);
	}
	iTag = XmlSymbolMap::m_oMap[sTag];
	endTag = true;
	content = line.mid(tagend1 + 1, tagstart2 - tagend1 - 1);
}

QString XmlLineParser::getTag()
{
	return sTag;
}

int XmlLineParser::tag()
{
	return iTag;
}

bool XmlLineParser::isEndTag()
{
	return endTag;
}

QString XmlLineParser::getUnquotedContent()
{
	return XmlField::unquote(content);
}

QString XmlLineParser::getContent()
{
	return content;
}

QString XmlLineParser::getAttributes()
{
	return attributes;
}

/*******************************************/
/* XML SAX2 Import Interface               */
/*                                         */
/* Andreas Rostin 29.08.2001               */
/*******************************************/
int XMLImportHandler::error_cnt = 0;
QString XMLImportHandler::error_msg = "";

// static
// import a xml file either into the given circuit (sub circuit) or using the circuit as main circuit
bool XMLImportHandler::import(Circuit *circuit, bool selected, int dx, int dy, QIODevice& ioDev)
{
	Device::IMPORT_IGNORE_GLOBAL = true;

	XMLImportHandler import_handler(circuit, selected, dx, dy);
	QXmlSimpleReader reader;
	QXmlInputSource source(&ioDev);

	reader.setContentHandler(&import_handler);
	reader.parse(source);

	Device::IMPORT_IGNORE_GLOBAL = false;

	if (error_cnt) {
		qCritical() << error_msg;
		return false;
	}
	return true;
}

// static
// create a net device from a xml file
XDevice * XMLImportHandler::import(Circuit *circuit, int dx, int dy, QIODevice &stream)
{
	Device::IMPORT_IGNORE_GLOBAL = true;

	XMLImportHandler import_handler(circuit, dx, dy);
	QXmlSimpleReader reader;
	QXmlInputSource source(&stream);
 
	reader.setContentHandler(&import_handler);
	reader.parse(source);

	Device::IMPORT_IGNORE_GLOBAL = false;

	if (error_cnt) {
		qCritical() << error_msg;
	}

	return import_handler.getDevice();
}

// static
// return no of errors occured during import
int XMLImportHandler::errorsOccured()
{
	return error_cnt;
}

// static
// return error messages  
QString& XMLImportHandler::getErrors()
{
	return error_msg;
}

XMLImportHandler::XMLImportHandler(Circuit *_circuit, bool selected, int dx, int dy)
{
	circuit = _circuit;
	imp_selected = selected;
	imp_dx = dx;
	imp_dy = dy;
	m_bImportDev = false;
	localImport = false;
	init();
}

XMLImportHandler::XMLImportHandler(Circuit *_circuit, int dx, int dy)
{
	circuit = _circuit;
	imp_selected = false;
	imp_dx = dx;
	imp_dy = dy;
	m_bImportDev = true;
	localImport = true;
	init();
}

XMLImportHandler::~XMLImportHandler()
{
}

XDevice * XMLImportHandler::getDevice()
{
	return importedDev;
}

void XMLImportHandler::init()
{
	error_msg = "";
	error_cnt = 0;

	importedDev = (XDevice *)NULL;
	activeTagIdx = 0;
	activeCircuit[activeTagIdx] = circuit;
	activeNetIdx = 0;
	activeDev = (XDevice *)NULL;
	activeID = 0;
	activeWire = (XWire *)NULL;
	activeWireConnWire = (XWire *)NULL;
	activeDevConnWire = (XWire *)NULL;
	importSub = false;

	// debugging
	characters_call_cnt = 0;
}

bool XMLImportHandler::startDocument()
{
	init();
	return true;
}

// currently, this method assues that everything is ok with the file!
// to be optimized by error handling!
bool XMLImportHandler::startElement(const QString&, const QString&, const QString& qName, const QXmlAttributes& atts)
{
	int id = XmlSymbolMap::m_oMap[qName];
	if (!id) {
		error_msg += i18n("unknown tag");
		error_msg += "<";
		error_msg += qName;
		error_msg += ">\n";
		error_cnt++;
	}
	activeTag[++activeTagIdx] = id;

	/*
	qDebug() << "Start Element " << qName;
	for (int i = 0; i < atts.length(); i++) {
		qDebug() << " Attr[" << atts.qName(i) << "=" << atts.value(i);
	}
*/
	// -----------------------------------------
	// main tags
	// -----------------------------------------
	if (id == XmlSymbol::KLOGIC_MAIN) {
		if (XmlSymbolMap::m_oMap.isEqual(atts.qName(0), XmlSymbol::FILE_TYPE)) {
			if (atts.value(0) == XmlObject::ATT_CIRCUIT)
				importSub = false;
			else
				importSub = true;
		}
		return true;
	}

	if (id == XmlSymbol::CIRCUIT) {
		if (atts.length() != 5) {
			error_msg += i18n("circuit: too few attributes\n");
			error_cnt++;
			return false;
		}
		int circuitID = atts.value(XmlSymbol::sID).toInt();
		int function = atts.value(XmlSymbol::sFUNCTION).toInt();
		int size = atts.value(XmlSymbol::sSIZE).toInt();

		int x = imp_dx;
		int y = imp_dy;
		if (!m_bImportDev) {
			x += atts.value(XmlSymbol::sXPOS).toInt();
			y += atts.value(XmlSymbol::sYPOS).toInt();
		}

		activeNetIdx++;
		if (activeNetIdx == 2) {
			// only important in top level circuit
			imp_dx_hidden = imp_dx;
			imp_dx = 0;
			imp_dy_hidden = imp_dy;
			imp_dy = 0;
			imp_selected_hidden = imp_selected;
			imp_selected = false;
		}

		if (!importSub && !m_bImportDev) {
			// the very first circuit to be the existing root circuit
			importSub = true;
			activeCircuit[activeNetIdx] = circuit;
		} else {
			// create sub circuits
			activeDev = activeCircuit[activeNetIdx - 1]->createDevice(function, QPoint(x, y), size);
			if (!activeDev) {
				error_msg += i18n("unable to create circuit device\n");
				error_cnt++;
				return false;
			}
			activeCircuit[activeNetIdx] = activeDev->devIsCircuit();
			if (!activeCircuit[activeNetIdx]) {
				error_msg += i18n("unable to create circuit\n");
				error_cnt++;
				return false;
			}
			activeDev = (XDevice *)NULL;

		}
		wiremap[activeNetIdx].init();

		// the id-mapping of a subcircuit must be known in the parent circuit
		devmap[activeNetIdx - 1].fileID(circuitID, activeCircuit[activeNetIdx]->getID());
		devmap[activeNetIdx].init();

		// but the new sub-circuit does not need to know its own id-mapping(?)
		//devmap[activeNetIdx].fileID(circuitID, activeNet[activeNetIdx]->getID());

		// only a device should be imported?
		if (m_bImportDev) {
			imp_dx = 0;
			imp_dy = 0;
			importSub = true;
			m_bImportDev = false;
			importedDev = activeCircuit[activeNetIdx];
		}

		return true;
	}

	if (id == XmlSymbol::DEVICE) {
		// create devices
		if (!activeCircuit[activeNetIdx]) {
			error_msg += i18n("no parent net for device\n");
			error_cnt++;
			return false;
		}
		if (atts.length() != 5) {
			error_msg += i18n("device: too few attributes\n");
			error_cnt++;
			return false;
		}
		activeID = atts.value(XmlSymbol::sID).toInt();
		int function = atts.value(XmlSymbol::sFUNCTION).toInt();
		int size = atts.value(XmlSymbol::sSIZE).toInt();
		int x = imp_dx;
		int y = imp_dy;
		if (!m_bImportDev) {
			x += atts.value(XmlSymbol::sXPOS).toInt();
			y += atts.value(XmlSymbol::sYPOS).toInt();
		}
		activeDev = activeCircuit[activeNetIdx]->createDevice(function, QPoint(x, y), size);
		if (!activeDev) {
			error_msg += i18n("unable to create device (circuit depth ");
			error_msg += QString::number(activeNetIdx);
			error_msg += ")\n";
			error_cnt++;
			return false;
		}
		devmap[activeNetIdx].fileID(activeID, activeDev->getID());

		// update named_input and named_output list id's
		if (function == DeviceType::fIN || function == DeviceType::fOUT) {
			activeCircuit[activeNetIdx]->mapImportID(activeID, activeDev->getID());
		}

		// only a device should be imported?
		if (m_bImportDev) {
			imp_dx = 0;
			imp_dy = 0;
			importedDev = activeDev;
			m_bImportDev = false;
		}

		return true;
	}

	if (id == XmlSymbol::WIRE) {
		// create wires
		if (!activeCircuit) {
			error_msg += i18n("no parent net for device\n");
			error_cnt++;
			return false;
		}
		if (atts.length() != 1) {
			error_msg += i18n("wire: too few attributes\n");
			error_cnt++;
			return false;
		}
		activeID = atts.value(XmlSymbol::sID).toInt();
		activeWire = activeCircuit[activeNetIdx]->newWire();
		qDebug() << "Start Wire creation for wire ID=" << activeID;
		activeWire->setExportDelta(imp_dx, imp_dy);
		if (!activeWire) {
			error_msg += i18n("unable to create wire (circuit depth ");
			error_msg += QString::number(activeNetIdx);
			error_msg += ")\n";
			error_cnt++;
			return false;
		}

		// id mapping
		wiremap[activeNetIdx].fileID(activeID, activeWire->getID());
		return true;
	}

	if (id == XmlSymbol::WIRE_CONN) {
		// create wire <--> wire connections
		if (!activeCircuit) {
			error_msg += i18n("no parent net for wire <--> wire connections\n");
			error_cnt++;
			return false;
		}
		if (atts.length() != 1) {
			error_msg += i18n("wire<-->wire connection: too few attributes\n");
			error_cnt++;
			return false;
		}
		int fileID = atts.value(XmlSymbol::sID).toInt();
		int wireID = wiremap[activeNetIdx].mapID(fileID);
		activeWireConnWire = activeCircuit[activeNetIdx]->getWire(wireID);
		qDebug() << "Start Wire-Wire connection for wire ID=" << fileID;
		activeID = 0;	// it stores the second id!
		if (!activeWireConnWire) {
			error_msg += i18n("unable to retrieve wire for id %d ");
			error_msg += QString::number(fileID);
			error_msg += i18n("(circuit depth ");
			error_msg += QString::number(activeNetIdx);
			error_msg += ")\n";
			error_cnt++;
			return false;
		}
		return true;
	}

	if (id == XmlSymbol::DEV_CONN) {
		// create dev <--> wire connections
		if (!activeCircuit) {
			error_msg += i18n("no parent net for wire <--> wire connections\n");
			error_cnt++;
			return false;
		}
		if (atts.length() != 1) {
			error_msg += i18n("wire<-->device connection: too few attributes\n");
			error_cnt++;
			return false;
		}
		int fileID = atts.value(XmlSymbol::sID).toInt();
		int wireID = wiremap[activeNetIdx].mapID(fileID);
		activeDevConnWire = activeCircuit[activeNetIdx]->getWire(wireID);
		qDebug() << "Start Wire-Device connection for wire ID=" << fileID;
		activeID = 0;	// it stores the second id!
		if (!activeDevConnWire) {
			error_msg += i18n("unable to retrieve wire for id ");
			error_msg += QString::number(fileID);
			error_msg += i18n("(circuit depth ");
			error_msg += QString::number(activeNetIdx);
			error_msg += ")\n";
			error_cnt++;
			return false;
		}
		return true;
	}

	// -----------------------------------------
	// sub tags with attributes
	// remember the attributes
	// -----------------------------------------
	// connections
	if (id == XmlSymbol::POS) {
		if (atts.length() > 2) {
			error_msg += i18n("position: too few attributes\n");
			error_cnt++;
			return false;
		}
		// a pos tag has id and inverter attribute
		activeID = atts.value(XmlSymbol::sID).toInt();
		if (atts.length() == 2)
			activeInv = atts.value(XmlSymbol::sINV).toInt();
		return true;
	}

	// equation devices
	if (id == XmlSymbol::OUT_EQU) {
		if (atts.length() != 3) {
			error_msg += i18n("equ-device output equations: too few attributes\n");
			error_cnt++;
			return false;
		}
		activePos = atts.value(0).toInt();
		activeIntern = atts.value(1).toInt();
		activeName = atts.value(2);
		return true;
	}

	// equation devices
	if (id == XmlSymbol::IN_NAME) {
		if (atts.length() != 2) {
			error_msg += i18n("equ-device input name: too few attributes\n");
			error_cnt++;
			return false;
		}
		activePos = atts.value(0).toInt();
		activeIntern = atts.value(1).toInt();
		return true;
	}

	// sub circuits
	if (id == XmlSymbol::OUT_ID ||
	    id == XmlSymbol::IN_ID) {
		if (atts.length() != 1) {
			error_msg += i18n("sub circuit input/output: too few attributes\n");
			error_cnt++;
			return false;
		}
		activePos = atts.value(0).toInt();
		return true;
	}

	// --------------------------------------------------------------------
	// sub tags with no content (characters() will not be called or ignored)
	// process the tags
	// --------------------------------------------------------------------
	// device: display text
	if (id == XmlSymbol::TEXT_DISP) {
		if (activeDev) activeDev->displayName(true);
		else activeCircuit[activeNetIdx]->displayName(true);
	}

	// push button
	if (id == XmlSymbol::PUSH) {
		if (activeDev) activeDev->setToggle(true);
	}

	// output inverted by default
	if (id == XmlSymbol::INV) {
		if (activeDev) activeDev->setInverted(true);
	}

	return true;
}

// tag content arrives
// the current tag id is stored in activeTag array
bool XMLImportHandler::characters(const QString& ch)
{
	// debugging
	characters_call_cnt++;

	if (ch.trimmed().isEmpty()) return true;

	int id = activeTag[activeTagIdx];
	if (!id) return true;

	// childs of activeNet
	if (activeWire) {
		if (id == XmlSymbol::WIRE) {
			qDebug() << "Continue Wire creation Nodes=" << ch;
			bool ret = *activeWire << ch;
			if (!ret) {
				error_msg += QString::number(characters_call_cnt);
				error_msg += ": ";
				error_msg += i18n("unable to import wire id");
				error_msg += " <";
				error_msg += QString::number(activeID);
				error_msg += ">\n";
				error_cnt++;
				return true;
			}
		}
		return true;
	}

	if (activeDev) {
		return setDeviceTag(activeDev, ch);
	}

	if (activeWireConnWire) {
		int fileID = activeID;
		activeID = wiremap[activeNetIdx].mapID(activeID);
		XWire *wire = activeCircuit[activeNetIdx]->getWire(activeID);
		if (!wire) {
			error_msg += QString::number(characters_call_cnt);
			error_msg += ": ";
			error_msg += i18n("unable to retrieve second wire for wire<-->wire connection with");
			error_msg += " ID2=";
			error_msg += QString::number(fileID);
			error_msg += "\n";
			error_cnt++;
			return true;
		}
		int delim = ch.indexOf(':');
		if (delim < 0) {
			error_msg += QString::number(characters_call_cnt);
			error_msg += ": ";
			error_msg += i18n("invalid position format for wire<-->wire connection with");
			error_msg += " ID2=";
			error_msg += QString::number(fileID);
			error_msg += "\n";
			error_cnt++;
			return true;
		}
		int x = ch.left(delim).toInt() + imp_dx;
		int y = ch.right(ch.length() - delim - 1).toInt() + imp_dy;
		QPoint pos(x,y);
		qDebug() << "Continue Wire-Wire connection for other wire ID=" << fileID << " in point " << pos;
		if (!activeWireConnWire->lockNode(pos)) {
			if (!wire->lockNode(pos)) {
				error_msg += QString::number(characters_call_cnt);
				error_msg += ": ";
				error_msg += i18n("unable to establish wire<-->wire connection with");
				error_msg += " ID2=";
				error_msg += QString::number(fileID);
				error_msg += i18n(" in position ");
				error_msg += QString::number(x);
				error_msg += ":";
				error_msg += QString::number(y);
				error_msg += " for activeNetIdx <";
				error_msg += QString::number(activeNetIdx);
				error_msg += "\n";
				error_cnt++;
				return true;
			} else activeWireConnWire->checkConnection(wire);
		} else {
			wire->checkConnection(activeWireConnWire);
		}

		return true;
	}

	if (activeDevConnWire) {
		// activeDevConn contains the wire
		// the id of the device is in activeID
		int fileID = activeID;
		activeID = devmap[activeNetIdx].mapID(activeID);
		int delim = ch.indexOf(':');
		if (delim < 0) {
			error_msg += QString::number(characters_call_cnt);
			error_msg += ": ";
			error_msg += i18n("invalid position format for wire<-->dev connection with");
			error_msg += " ID2=";
			error_msg += QString::number(fileID);
			error_msg += ")\n";
			error_cnt++;
			return true;
		}
		int x = ch.left(delim).toInt() + imp_dx;
		int y = ch.right(ch.length() - delim - 1).toInt() + imp_dy;
		QPoint pos(x,y);
		qDebug() << "Continue Wire-Device connection at " << pos << " to device id=" << fileID;
		if (!activeDevConnWire->contains(pos)) {
			error_msg += QString::number(characters_call_cnt);
			error_msg += ": ";
			error_msg += i18n("no such wire node ");
			error_msg += QString::number(x);
			error_msg += ":";
			error_msg += QString::number(y);
			error_msg += "\n";
			error_cnt++;
			return true;
		}
		XDevice *dev = activeCircuit[activeNetIdx]->getDevice(activeID);
		if (dev) {
			activeCircuit[activeNetIdx]->checkConnection(activeID, activeDevConnWire);
			if (activeDevConnWire->getInputIndex(dev) != Wire::WFAIL) {
				activeDevConnWire->invertInput(dev, activeInv);
			}
			if (activeDevConnWire->getOutputIndex(dev) != Wire::WFAIL) {
				activeDevConnWire->invertOutput(dev, activeInv);
			}
		}
		return true;
	}

	if (activeNetIdx > 0) {
		return setDeviceTag(activeCircuit[activeNetIdx], ch);
	}

	// these should be handled the very first, so they almost never be checked
	if (id == XmlSymbol::PROGRAM_VERSION) {
		// informational only
		// always the current version is stored
		return true;
	}

	if (id == XmlSymbol::CREATION_DATE) {
		if (!localImport) KlogicGlobal::CurrentCircuit::creation = ch;
		return true;
	}

	if (id == XmlSymbol::UPDATE_DATE) {
		// informational only
		return true;
	}

	if (id == XmlSymbol::AUTHOR) {
		if (!localImport) KlogicGlobal::CurrentCircuit::author = ch;
		return true;
	}

	if (id == XmlSymbol::COMMENT) {
		if (!localImport) KlogicGlobal::CurrentCircuit::comment = ch;
		return true;
	}

	return true;
}

// check import content of a device tag
// set the more often used tags to the top!
// use "dev" instead of "activeDev" here!
bool XMLImportHandler::setDeviceTag(XDevice *dev, const QString& ch)
{
	int id = activeTag[activeTagIdx];

	if (id == XmlSymbol::TEXT) {
		activeCircuit[activeNetIdx]->setDeviceName(dev->getID(), ch);
		return true;
	}

	if (id == XmlSymbol::DELAY) {
		dev->setDelay(ch.toInt());
		return true;
	}

	if (id == XmlSymbol::UNDEF) {
		dev->setUndefinedValue(ch.toInt());
		return true;
	}

	if (id == XmlSymbol::STAT) {
		dev->setInputValue(ch.toInt());
		return true;
	}

	if (id == XmlSymbol::CLK_TYPE) {
		int iClock = ch.toInt();
		KlogicGlobal::Device::CLOCKTYPE tClock = (KlogicGlobal::Device::CLOCKTYPE)ch.toInt();
		dev->setClock(tClock);
		dev->setOscType(iClock);
		return true;
	}

	if (id == XmlSymbol::MASTER) {
		dev->setMaster(ch.toInt());
		return true;
	}

	if (id == XmlSymbol::OSC_ON) {
		dev->setOscOn(ch.toInt());
		return true;
	}

	if (id == XmlSymbol::OSC_OFF) {
		dev->setOscOff(ch.toInt());
		return true;
	}

	if (id == XmlSymbol::OSC_STEP) {
		dev->setCurrOscStep(ch.toInt());
		return true;
	}

	if (id == XmlSymbol::RAM) {
		int delim_pos;
		int curr_pos = 0;
		if (-1 == (delim_pos = ch.indexOf(';'))) {
			error_msg += i18n("ram error: no length\n");
			error_cnt++;
			return true;
		}
		int cnt = ch.mid(curr_pos, delim_pos - curr_pos).toInt();
		int str_len = ch.length();
		dev->setRAMSize(cnt);
		char *newram = dev->getRAMStorageCopy();
		for(int i = 0; i < cnt; i++) {
			curr_pos = delim_pos + 1;
			if (curr_pos > str_len) {
				error_msg += i18n("ram error: invalid length\n");
				error_cnt++;
				return true;
			}
			delim_pos = ch.indexOf(';', curr_pos);
			if (delim_pos == -1) delim_pos = str_len;
			newram[i] = ch.mid(curr_pos, delim_pos - curr_pos).toInt();
		}
		dev->setRAMStorage(newram);
		delete [] newram;
		return true;
	}

	if (id == XmlSymbol::TRI) {
		if (dev->type() == DeviceType::fRAM) {
			dev->enableTristateInput(true);
		} else {
			dev->setTristateControlPos(ch.toInt());
		}
		return true;
	}

	if (id == XmlSymbol::COLOR) {
		dev->setColor(ch.toInt());
		return true;
	}

	// equation device output
	if (id == XmlSymbol::OUT_EQU) {
		int dev_id;
		if (!activeIntern)
			dev_id = dev->addOutputName(activeName, activePos, 0);
		else
			dev_id = dev->addInternalName(activeName);
		dev->Device::setEquation(ch, dev_id);
		dev->setSize(dev->neededSize());
		return true;
	}

	// equation device input
	if (id == XmlSymbol::IN_NAME) {
		int input_id = ch.toInt();
		if (!activeIntern) dev->addInputName(ch, activePos, input_id);
		else dev->addInternalName(ch);
		dev->setSize(dev->neededSize());
		return true;
	}

	// sub circuit output: it's a reference to a device inside
	// the device name could come in later sections!
	// the mapping will be done while doing device import for this circuit device
	if (id == XmlSymbol::OUT_ID) {
		if (dev->type() != DeviceType::fRAM) {
			int output_id_old = ch.toInt();
			dev->addOutputName(ch, activePos, output_id_old);
		}
		return true;
	}

	// sub circuit input: it's a reference to a device inside
	// the device name could come in later sections!
	// the mapping will be done while doing device import for this circuit device
	if (id == XmlSymbol::IN_ID) {
		if (dev->type() != DeviceType::fRAM) {
			int input_id_old = ch.toInt();
			dev->addInputName(ch, activePos, input_id_old);
		}
		return true;
	}

	// device width: net and equ only
	if (id == XmlSymbol::WIDTH) {
		int width = ch.toInt();
		dev->setWidth(width);
		return true;
	}

	// device width: net and equ only
	if (id == XmlSymbol::BUSTYPE) {
		bool bMux = ch.toInt();
		dev->setMux(bMux);
		return true;
	}

	if (id == XmlSymbol::CREATION_DATE ||
		id == XmlSymbol::UPDATE_DATE ||
		id == XmlSymbol::AUTHOR ||
		id == XmlSymbol::COMMENT) return true;

	error_msg += "XMLImportHandler::setDeviceTag ";
	error_msg += i18n("error: tag ");
	error_msg += QString::number(id);
	error_msg += i18n(" ignored!\n");
	error_cnt++;
	return true;
}

bool XMLImportHandler::endElement(const QString&, const QString&, const QString& qName)
{
	int id = XmlSymbolMap::m_oMap[qName];

	if (id == XmlSymbol::DEV_CONN)
		activeDevConnWire = (XWire *)NULL;

	if (id == XmlSymbol::WIRE_CONN)
		activeWireConnWire = (XWire *)NULL;

	if (id == XmlSymbol::DEVICE) {
		activeDev->parseEquation();

		if (activeDev->isDeviceTristateEnabled())
			activeDev->setTristateInputImport();

		activeDev->setImage();

		activeDev->select(imp_selected);

		activeDev = (XDevice *)NULL;
	}

	if (id == XmlSymbol::WIRE)
		activeWire->select(imp_selected);
		activeWire = (XWire *)NULL;

	if (id == XmlSymbol::CIRCUIT) {
		// only important in top level circuit
		if (activeNetIdx == 2) {
			imp_dx = imp_dx_hidden;
			imp_dy = imp_dy_hidden;
			imp_selected = imp_selected_hidden;
		}

		activeCircuit[activeNetIdx]->select(imp_selected);

		// image could have changed by io devices!
		if (activeCircuit[activeNetIdx]->type() == DeviceType::fSUB)
			activeCircuit[activeNetIdx]->setImage();

		// parse equations
		if (activeCircuit[activeNetIdx]->type() == DeviceType::fEQU)
			activeCircuit[activeNetIdx]->parseEquation();

		// close circuit
		activeNetIdx--;
		if (activeNetIdx < 0) {
			error_msg += i18n("unexpected circuit close\n");
			error_cnt++;
			return false;
		}
	}

	if (id != activeTag[activeTagIdx--]) {
		error_msg += i18n("unexpected tag close <");
		error_msg += qName;
		error_msg += ">\n";
		error_cnt++;
		return false;
	}
	if (activeTagIdx < 0) {
		error_msg += i18n("negative tag stack .. while closing <");
		error_msg += qName;
		error_msg += ">\n";
		error_cnt++;
		return false;
	}

	return true;
}

bool XMLImportHandler::endDocument()
{
	return true;
}


