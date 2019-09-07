/*************************************************/
/* old methods for classes Circuit XDevice,   */
/* Device and XWire                              */
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
#include "DeviceType.h"
#include "deviceOsc.h"
#include "devicePwr.h"
#include "deviceSwitch.h"

#define DLM 1 // string delimiter (io operations): ctrl-A

/***************************************************/
/* import/export operations on a net-widget        */
/***************************************************/
// get subnet-pointer by name (used by import methods)
Circuit * Circuit::subCircuit(QString& s)
{
	int epos = s.indexOf(DLM);
	if (epos == -1) epos = s.length() + 1;
	QString sid = s.mid(1, epos - 1);

	XDevice *poDevice = devices[devmap.mapID(sid)];
	if (poDevice)
		return poDevice->devIsCircuit();
	return 0;
}

void Circuit::initImport()
{
	wiremap.init();
	devmap.init();
}

// reading a sub circuit from file: create a new sub circuit device and initialize it
bool Circuit::importInstance(QString s, int dx, int dy)
{
	// newer files have some kind of version number ..
	if (!Circuit::magic_version) {
		int posa = s.lastIndexOf(DLM);
		if (posa != -1) {	// of course not!
			Circuit::magic_version = s.right(s.length() - posa - 1).toInt();
		}
	}

	importDevice(s, 0, dx, dy);

	return true;
}

bool Circuit::importWire(QString s, int select, int dx, int dy)
{
	int epos;
	QString sid;
	XWire *w;

	epos = s.indexOf(DLM, 0);
	if (epos == -1) return false;
	sid = s.mid(1, epos - 1);
	s = s.right(s.length() - epos - 1);

	if (!(w = new XWire())) return false;
	w->setExportDelta(dx, dy);
	bool ret = *w << s;
	if (!ret) {
		delete w;
		return false;
	}

	wires.Append(w, w->getID());

	wiremap.fileID(sid, w->getID());

	// select wire?
	if (select) w->select(1);

	return true;
}

bool Circuit::importDevice(QString s, int select, int dx, int dy)
{
	int apos, epos;
	int func;
	QString sid;
	QString devext;
	XDevice *d;
	bool ret = true;

	apos = 1;
	epos = s.indexOf(DLM, 0);
	if (epos == -1) return false;
	sid = s.mid(apos, epos - apos);

	apos = epos + 1;
	epos = s.indexOf(DLM, apos);
	if (epos == -1) return false;
	func = s.mid(apos, epos - apos).toInt();

	// create the new device
	d = createDevice(func, QPoint(0, 0), 4);
	if (!d) qFatal("Circuit::importDevice(): unable to create device ..\n");

	// call member method to import device parameter
	ret = d->string2device(s, dx, dy);

	devmap.fileID(sid, d->getID());

	// add device to a selection?
	if (select) d->select(1);

	if (d->type() == DeviceType::fIN || d->type() == DeviceType::fOUT) {
		// the text of the i/o-device is currently the old id ... (or -1 if it is an old format)
		if (d->type() == DeviceType::fIN) {
			mapInputName(sid.toInt(), d->getID(), d->getName());
		}
		if (d->type() == DeviceType::fOUT) {
			mapOutputName(sid.toInt(), d->getID(), d->getName());
		}

		// now set the correct text
		setDeviceName(d->getID(), d->getName());
		setImage();
	}

	return ret;
}

bool Circuit::importWireConn(QString s, int, int dx, int dy)
{
	int apos, epos;
	XWire *w1, *w2;
	QPoint pt;
	QString refWire, conWire;
	int refWireID, conWireID;

	epos = s.indexOf(DLM, 0);
	if (epos == -1) return false;
	refWire = s.mid(1, epos - 1);

	refWireID = wiremap.mapID(refWire);

	apos = epos + 1;
	epos = s.indexOf(DLM, apos);
	if (apos == -1 || epos == -1) return false;
	conWire = s.mid(apos, epos - apos);

	conWireID = wiremap.mapID(conWire);

	apos = epos + 1;
	epos = s.indexOf(DLM, apos);
	if (apos == -1 || epos == -1) return false;
	pt.setX(s.mid(apos, epos - apos).toInt() + dx);

	apos = epos + 1;
	epos = s.length();
	if (apos == -1) return false;
	pt.setY(s.mid(apos, epos - apos).toInt() + dy);

	w1 = wires.Get(refWireID);
	w2 = wires.Get(conWireID);
	if (!w1 || !w2) return false;
	if (!w1->lockNode(pt)) {
		if (!w2->lockNode(pt))
			return false;
		else
			w1->checkConnection(w2);
	}
	else
		w2->checkConnection(w1);

	return true;
}

bool Circuit::importDevConn(QString s, int, int dx, int dy)
{
	int apos, epos;
	QString sid;
	QString dev1, dev2;
	int wire_id;
	int dev1_id = 0, dev2_id = 0;
	int inverted1 = 0, inverted2 = 0;
	QPoint p1, p2;
	XWire *w;

	epos = s.indexOf(DLM, 0);
	if (epos == -1) return false;
	sid = s.mid(1, epos - 1);

	wire_id = wiremap.mapID(sid);

	apos = epos + 1;
	epos = s.indexOf(DLM, apos);
	if (apos == -1 || epos == -1) return false;
	dev1 = s.mid(apos, epos - apos);
	dev1_id = devmap.mapID(dev1);

	apos = epos + 1;
	epos = s.indexOf(DLM, apos);
	if (apos == -1 || epos == -1) return false;
	inverted1 = s.mid(apos, epos - apos).toInt();

	apos = epos + 1;
	epos = s.indexOf(DLM, apos);
	if (apos == -1 || epos == -1) return false;
	p1.setX(s.mid(apos, epos - apos).toInt() + dx);

	apos = epos + 1;
	epos = s.indexOf(DLM, apos);
	if (epos == -1)
		epos = s.length();
	if (apos == -1) return false;
	p1.setY(s.mid(apos, epos - apos).toInt() + dy);

	apos = epos + 1;
	epos = s.indexOf(DLM, apos);
	if (apos != -1 && epos != -1) {
		dev2 = s.mid(apos, epos - apos);
		dev2_id = devmap.mapID(dev2);

		apos = epos + 1;
		epos = s.indexOf(DLM, apos);
		if (apos == -1 || epos == -1) return false;
		inverted2 = s.mid(apos, epos - apos).toInt();

		apos = epos + 1;
		epos = s.indexOf(DLM, apos);
		if (apos == -1 || epos == -1) return false;
		p2.setX(s.mid(apos, epos - apos).toInt() + dx);

		apos = epos + 1;
		epos = s.length();
		if (apos == -1) return false;
		p2.setY(s.mid(apos, epos - apos).toInt() + dy);
	}
	else {
		p2.setX(0);
		p2.setY(0);
	}
	// connect wire with devices
	w = wires.Get(wire_id);
	if (!w) return false;
	if (!w->contains(p1)) return false;

	XDevice *dev = getDevice(dev1_id);
	if (dev) {
		checkConnection(dev1_id, w);
		if (w->getInputIndex(dev) != Wire::WFAIL) {
			w->invertInput(dev, inverted1);
		}
		if (w->getOutputIndex(dev) != Wire::WFAIL) {
			w->invertOutput(dev, inverted1);
		}
		if (!p2.isNull()) {
			if (!w->contains(p2)) return false;

			dev = getDevice(dev2_id);
			if (dev) {
				checkConnection(dev2_id, w);
				if (w->getInputIndex(dev) != Wire::WFAIL) {
					w->invertInput(dev, inverted2);
				}
				if (w->getOutputIndex(dev) != Wire::WFAIL) {
					w->invertOutput(dev, inverted2);
				}
			}
		}

	}
	return true;
}

bool XDevice::string2device(QString s, int dx, int dy)
{
	bool ret = true;
	QString xs;
	int apos, epos, cnt, i;
	int pos;
	QPoint pt;
	int redraw = 0;
	int finished = 0;
	bool id_flag;
	int dev_id;

	ret = Device::string2device(s);
	if (!ret) return ret;

	cnt = s.length() - s.indexOf((char)2);
	xs = s.right(cnt);

	epos = xs.indexOf(DLM, 0);
	if (epos == -1) return false;

	// size
	apos = epos + 1;
	epos = xs.indexOf(DLM, apos);
	if (epos == -1) return false;
	redraw = setSize(xs.mid(apos, epos - apos).toInt());

	// position
	apos = epos + 1;
	epos = xs.indexOf(DLM, apos);
	if (epos == -1) return false;
	pt.setX(xs.mid(apos, epos - apos).toInt() + dx);
	apos = epos + 1;
	epos = xs.indexOf(DLM, apos);
	if (epos == -1) {
		finished = 1;
		epos = xs.length();
	}
	pt.setY(xs.mid(apos, epos - apos).toInt() + dy);
	// the image of switch has changed (only relevant for this import)
	if (Circuit::magic_version != 1300 && type() == DeviceType::fSWI) {
		pt.setX(pt.x() + 10);
	}

	moveTo(pt);

	// named output
	apos = epos + 1;
	if (!finished && -1 != (epos = xs.indexOf((char)3, apos))) {
		apos = epos + 1;
		epos = xs.indexOf(DLM, apos);
		if (epos == -1) {
			ret = false;
			finished = 1;
		}
		cnt = xs.mid(apos, epos - apos).toInt();
		for(i=0; i < cnt; i++) {
			apos = epos + 1;
			epos = xs.indexOf(DLM, apos);
			if (epos == -1) {
				ret = true;        // hist. programming error: ignore!
				finished = 1;
				continue;
			}
			QString dev_id_string = xs.mid(apos, epos - apos);
			dev_id = dev_id_string.toInt(&id_flag);
			if (id_flag == FALSE) dev_id = 0;


			apos = epos + 1;
			epos = xs.indexOf(DLM, apos);
			if (epos == -1) {
				ret = false;
				finished = 1;
				continue;
			}
			pos = xs.mid(apos, epos - apos).toInt();

			apos = epos + 1;
			epos = xs.indexOf(DLM, apos);
			if (epos == -1) {
				ret = false;
				finished = 1;
				continue;
			}
			int internal = xs.mid(apos, epos - apos).toInt();

			apos = epos + 1;
			epos = xs.indexOf(DLM, apos);
			if (epos == -1) {
				finished = 1;
				epos = xs.length();
			}
			QString equation = xs.mid(apos, epos - apos);

			if (!internal) dev_id = addOutputName(dev_id_string, pos, dev_id);
			else dev_id = addInternalName(dev_id_string);
			Device::setEquation(equation, dev_id);
		}
	}

	// named input
	if (epos != -1) apos = epos + 1;
	if (!finished && -1 != (epos = xs.indexOf((char)4, apos))) {
		apos = epos + 1;
		epos = xs.indexOf(DLM, apos);
		if (epos == -1) {
			ret = false;
			finished = 1;
		}
		cnt = xs.mid(apos, epos - apos).toInt();
		for(i=0; i < cnt; i++) {
			apos = epos + 1;
			epos = xs.indexOf(DLM, apos);
			if (epos == -1) {
				ret = false;
				finished = 1;
				continue;
			}
			QString dev_id_string = xs.mid(apos, epos - apos);
			dev_id = dev_id_string.toInt(&id_flag);
			if (id_flag == FALSE) dev_id = 0;

			apos = epos + 1;
			epos = xs.indexOf(DLM, apos);
			if (epos == -1) {
				finished = 1;
				epos = xs.length();
			}
			pos = xs.mid(apos, epos - apos).toInt();

			apos = epos + 1;
			epos = xs.indexOf(DLM, apos);
			if (epos == -1) {
				finished = 1;
				epos = xs.length();
			}
			int internal = xs.mid(apos, epos - apos).toInt();

			if (!internal) dev_id = addInputName(dev_id_string, pos, dev_id);
			else addInternalName(dev_id_string);
		}
	}

	if (!finished) {
		if (epos != -1) apos = epos + 1;
		epos = xs.indexOf(DLM, apos);
		if (epos == -1) {
			finished = 1;
			epos = xs.length();
		}
		displayName(xs.mid(apos, epos - apos).toInt());
	}

	if (!finished && type() == DeviceType::fRAM) {
		if (epos != -1) apos = epos + 1;
		epos = xs.indexOf(DLM, apos);
		setRAMSize(xs.mid(apos, epos - apos).toInt());
		char *newram = getRAMStorageCopy();
		for(i = 0; i < RAMSize(); i++) {
			apos = epos + 1;
			epos = xs.indexOf(DLM, apos);
			if (epos == -1) {
				finished = 1;
				epos = xs.length();
			}
			newram[i] = xs.mid(apos, epos - apos).toInt();
		}
		setRAMStorage(newram);
		delete [] newram;
	}

	if (!finished) {
		if (epos != -1) apos = epos + 1;
		epos = xs.indexOf(DLM, apos);
		if (epos == -1) {
			finished = 1;
			epos = xs.length();
		}
		if (xs.mid(apos, epos - apos).toInt()) {                // isTristate!
			// retrieve the "EN" input properties
			apos = epos + 1;
			epos = xs.indexOf(DLM, apos);
			if (epos == -1) {
				qFatal("XDevice::string2device: what?");
			}
			QString dev_id_string = xs.mid(apos, epos - apos);
			dev_id = dev_id_string.toInt(&id_flag);
			if (id_flag == FALSE) dev_id = 0;

			apos = epos + 1;
			epos = xs.indexOf(DLM, apos);
			if (epos == -1) {
				finished = 1;
				epos = xs.length();
			}
			pos = xs.mid(apos, epos - apos).toInt();

			// create the "EN" input at the appropriate position, or use the existing one
			setTristateControlPos(pos);
			setTristateInputImport();
		}
	}

	setName(getName());

	if (redraw ||
			type() == DeviceType::fRS || type() == DeviceType::fJK ||
			type() == DeviceType::fDFF || type() == DeviceType::fSUB ||
			type() == DeviceType::fEQU || type() == DeviceType::fSS || type() == DeviceType::fOSC)
		setImage();

	parseEquation();

	return ret;
}

bool Device::string2device(QString s)
{       int apos, epos;
	QString original_text;
	QString alternate_text;

	// name
	apos = 1;
	epos = s.indexOf(DLM, 0);
	if (epos == -1) return false;
	alternate_text = s.mid(apos, epos - apos);

	// function
	apos = epos + 1;
	epos = s.indexOf(DLM, apos);
	if (epos == -1) return false;

	// delay
	apos = epos + 1;
	epos = s.indexOf(DLM, apos);
	if (epos == -1) return false;
	setDelay(s.mid(apos, epos - apos).toInt());

	// undefined value
	apos = epos + 1;
	epos = s.indexOf(DLM, apos);
	if (epos == -1) return false;
	setUndefinedValue(s.mid(apos, epos - apos).toInt());

	// clock type
	apos = epos + 1;
	epos = s.indexOf(DLM, apos);
	if (epos == -1) return false;
	//Global::Device::CLOCKTYPE iClock = (Global::Device::CLOCKTYPE)s.mid(apos, epos - apos).toInt();
	//setClock(s.mid(iClock));

	// master ff flag
	apos = epos + 1;
	epos = s.indexOf(DLM, apos);
	if (epos == -1) return false;
	//setMaster(s.mid(apos, epos - apos).toInt());	// to be entirely deleted!

	// oscillator on time
	apos = epos + 1;
	epos = s.indexOf(DLM, apos);
	if (epos == -1) return false;
	//if (poOscDev) poOscDev->setOszOn(s.mid(apos, epos - apos).toInt());

	// oscillator off time
	apos = epos + 1;
	epos = s.indexOf(DLM, apos);
	if (epos == -1) return false;
	//if (poOscDev) poOscDev->setOszOff(s.mid(apos, epos - apos).toInt());

	// output value
	apos = epos + 1;
	epos = s.indexOf(DLM, apos);
	if (epos == -1) return false;
	flush(s.mid(apos, epos - apos).toInt());

	if (type() == DeviceType::fPWR) {
		//poPwrDev->setInputValue(s.mid(apos, epos - apos).toInt());
	}
	if (type() == DeviceType::fSWI) {
		//poSwDev->setInputValue(s.mid(apos, epos - apos).toInt());
	}

	// osz_curr_step
	apos = epos + 1;
	epos = s.indexOf(DLM, apos);
	if (epos == -1) return false;
	//if (poOscDev) poOscDev->setCurrOszStep(s.mid(apos, epos - apos).toInt());

	// text label
	apos = epos + 1;
	epos = s.indexOf(DLM, apos);
	if (epos == -1) return false;
	original_text = s.mid(apos, epos - apos);
	if (original_text.length() > 0) setName(original_text);
	else setName(alternate_text);

	return true;
}

