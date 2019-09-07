/*************************************************/
/* methods for class Device                      */
/*                                               */
/* logical simulation for wired gates            */
/*                                               */
/* Andreas Rostin                                */
/* 15.03.99                                      */
/*************************************************/
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <qstring.h>
#include <qregexp.h>

#include "KlogicGlobal.h"
#include "device.h"
#include "tool.h"
#include "DeviceType.h"
#include "busOutputArray.h"
#include "symbolicCalc.h"
#include "calculator.h"
#include "connection.h"

#include <list>

using namespace std;

/***************************************************/
/* static methods of Device                        */
/***************************************************/
bool Device::IMPORT_IGNORE_GLOBAL = false;
int Device::instance = 0;
const QString Device::INPUT_TRISTATE_CONTROL_NAME = "EN";

/***************************************************/
/* methods of Device                               */
/***************************************************/
// Constructor
Device::Device(int iFunction)
{
	instance++;

	m_iDeviceId = uniqueID::getID();
	m_sName = KlogicGlobal::Device::getName(iFunction);
	m_iFunction = iFunction;

	// output change detection
	forceOutputHasChanged = true;
	outputHasChanged = true;

	// all outputs have the same undefined output
	int iUndefinedValue;
	if (Device::IMPORT_IGNORE_GLOBAL)
		iUndefinedValue = KlogicGlobal::Device::DEFAULT_UNDEFINED_VALUE;
	else
		iUndefinedValue = KlogicGlobal::Device::getUndefinedValue();

	int iDelay;
	if (Device::IMPORT_IGNORE_GLOBAL)
		iDelay = KlogicGlobal::Device::DEFAULT_DELAY;
	else
		iDelay = KlogicGlobal::Device::getDelay();

	// the first output in the list of outputs is an unnamed output, each device can use
	Calculator *poCalculator = new Calculator(this, iUndefinedValue, iDelay);
	m_oNamedOutput.add(poCalculator, 0, "", -1);

	m_bTristateEnabled = false;

	// internally used input- or output-id
	m_iTristateEnableID = -1;

	m_poConnectionList = new ConnectionList();
}

// Destructor
Device::~Device()
{
	instance--;

	for (ConnectorList::iterator iter = m_oNamedOutput.begin(); iter != m_oNamedOutput.end(); iter++)
		delete **iter;
}

QString Device::getName(bool bNoSpace) const
{	
	QString ret;

	if (!bNoSpace)
		return m_sName;
	ret = m_sName;
	ret.replace(QRegExp(" "), "_" );
	return ret;
}

void Device::setName(const QString& sName)
{
	m_sName = sName;
}

// return true if output is at low resistance
bool Device::outputIsActive(int iOutputId)
{
	Calculator *poCalculator = m_oNamedOutput[iOutputId];
	return (poCalculator && poCalculator->getResult() != KlogicGlobal::Device::TRISTATE);
}

// enable/disable tristate mode for a single output
// make sure there is an "EN" input available!
void Device::setOutputTristateEnabled(int iOutputId, bool flag)
{
	Calculator *poCalculator = m_oNamedOutput[iOutputId];
	if (poCalculator) {
		if (poCalculator->isInternal()) return;
		poCalculator->setTristate(flag);
	} else return;
}

/**
 * Returns true if the output is enabled for third state.
 * @param iOutputId The output ID to analyze
 * @return true if tristate is enabled
 */
bool Device::isOutputTristateEnabled(int iOutputId)
{
	Calculator *poCalculator = m_oNamedOutput[iOutputId];
	if (poCalculator)
		return poCalculator->isTristate();
	return false;
}

// enable/disable tristate mode for the complete device
void Device::setDeviceTristateEnabled(bool bTristate)
{
	for (ConnectorList::iterator iter = m_oNamedOutput.begin(); iter != m_oNamedOutput.end(); iter++) {
		const Connector& oConnector = *iter;
		setOutputTristateEnabled(oConnector.getConnectorID(), bTristate);
	}
}

// return tristate mode for a single output
bool Device::isDeviceTristateEnabled()
{
	for (ConnectorList::iterator iter = m_oNamedOutput.begin(); iter != m_oNamedOutput.end(); iter++) {
		const Connector& oConnector = *iter;
		if (isOutputTristateEnabled(oConnector.getConnectorID())) {
		    return true;
		}
	}
	return false;
}

// returns wether input is the tristate control input or not
bool Device::isTristateControlInput(int input_id)
{
    return input_id == m_iTristateEnableID;
}

void Device::addTristateControlInput(int position) {
    if (m_iTristateEnableID < 0) {
      m_iTristateEnableID = addInputName(Device::INPUT_TRISTATE_CONTROL_NAME, position);
    }
}

void Device::removeTristateControlInput() {
    if (m_iTristateEnableID >= 0) {
      removeInputName(m_iTristateEnableID);
    }
    m_iTristateEnableID = -1;
}

bool Device::hasTristateControlInput() {
    return m_iTristateEnableID >= 0 && m_oNamedInput.find(m_iTristateEnableID);
}

bool Device::isTristateControlInputEnabled() {
    return m_bTristateEnabled;
}

int Device::getTristateControlInputPosition() {
    if (m_iTristateEnableID >= 0) {
      return m_oNamedInput.find(m_iTristateEnableID)->getPosition();
    } else {
      return -1;
    }
}

// connect two devices
//	out_dev:   connected device
//	input_id:  id of this named input
//	iOutputId: id of out_dev's named output
bool Device::connectDevices(Device *poDevice, int iInputID, int iOutputId)
{
	// cout << "connect " << poDevice->getName() << "(" << iOutputId << ") --> " << getName() << "(" << iInputID << ")" << endl;
	m_poConnectionList->add(poDevice, iInputID, iOutputId);
	return true;
}

// disconnect two devices
bool Device::disconnectDevices(Device *poDevice, int iInputID, int iOutputId)
{
	// cout << "disconnect " << poDevice->getName() << "(" << iOutputId << ") --> " << getName() << "(" << iInputID << ")" << endl;
	m_poConnectionList->remove(poDevice, iInputID, iOutputId);
	return true;
}

// return type of device
int Device::type() const
{	
	return m_iFunction;
}

// check if name is unused
bool Device::inputNameIsUnique(const QString& sName)
{
	return !m_oNamedInput.hasConnector(sName);
}

// add a new named input for this device
// pos contains the the position (GRID times related to device-image) of the named input device
// input_id is used for import purposes!
int Device::addInputName(const QString& sName, int iPosition, int iInputID)
{
	if (!iInputID) {
		iInputID = uniqueID::getID();
	}
	if (iPosition > getSize()) {
		setSize(iPosition);
	}
	m_oNamedInput.add(0, iInputID, sName, iPosition);
	return iInputID;
}

int Device::getNextInputId() {
	  int pos = 0;
	  for (ConnectorList::iterator iter = m_oNamedInput.begin(); iter != m_oNamedInput.end(); iter++) {
		  int cPos = (*iter).getPosition() + 1;
		  if (pos < cPos) {
			  pos = cPos;
		  }
	  }
	  return pos;
}

// change the name of an input
int Device::changeInputName(int iInputID, const QString& sNewName)
{
	m_oNamedInput.setName(iInputID, sNewName);
	return iInputID;
}

// remove an existing named input from this device
void Device::removeInputName(int iInputID)
{
	m_oNamedInput.remove(iInputID);
}

// return the id of an output name (equation devices: unique names!)
int Device::getInputID(const QString& sName)
{
	return m_oNamedInput.getConnectorID(sName);
}

void Device::mapInputName(int iOldInputID, int iNewInputID, const QString &sNewName)
{
	if (!m_oNamedInput.mapTo(iOldInputID, iNewInputID, sNewName))
		qWarning() << "Device::mapInputName - no such input";
}

// add a new named output for this device
// pos contains the the position (GRID times related to device-image) of the named output device
// output_id is used for import purposes!
int Device::addOutputName(const QString& sName, int iPosition, int iOutputId)
{
	Calculator *poCalculator = new Calculator(this, getUndefinedValue(), getDelay());
	if (!iOutputId) iOutputId = uniqueID::getID();
	m_oNamedOutput.add(poCalculator, iOutputId, sName, iPosition);
	return iOutputId;
}

// change the name of an output
int Device::changeOutputName(int iOutputId, const QString& sName)
{
	if (m_oNamedOutput.setName(iOutputId, sName))
		return iOutputId;
	return 0;
}

// remove a named output from this device
void Device::removeOutputName(int iOutputId)
{
	Calculator *poCalculator = m_oNamedOutput.remove(iOutputId);
	if (poCalculator)
		delete poCalculator;
	else
		qWarning() << "Device::removeOutputName - no such output";
}

void Device::mapOutputName(int iOldOutputID, int iNewOutputID, const QString &sNewName)
{
	if (!m_oNamedOutput.mapTo(iOldOutputID, iNewOutputID, sNewName))
		qWarning() << "Device::mapOutputName - no such output";
}

// return the name of an output id (equation devices: unique names!)
const QString& Device::getOutputName(int iOutputId)
{
	return m_oNamedOutput.getName(iOutputId);
}

// return the name of an output id (equation devices: unique names!)
int Device::getOutputID(const QString& sName)
{
	return m_oNamedOutput.getConnectorID(sName);
}

// change type of output
void Device::setOutputNameType(int iOutputId, KlogicGlobal::Device::IOTYPE iType)
{
	Calculator *poCalculator = m_oNamedOutput[iOutputId];
	if (poCalculator) {
		poCalculator->setInternal(iType == KlogicGlobal::Device::INTERNAL_OUTPUT);
		setOutputTristateEnabled(iOutputId, iType != KlogicGlobal::Device::INTERNAL_OUTPUT);
	}
}

// add a new named internal output for this device
int Device::addInternalName(const QString& sName)
{
	Calculator *poCalculator = new Calculator(this, getUndefinedValue(), getDelay());
	poCalculator->setInternal(true);
	int iInternalID = uniqueID::getID();
	m_oNamedOutput.add(poCalculator, iInternalID, sName, 0);
	return iInternalID;
}

// returns whether id is an internal output name, or should be final or temporary
KlogicGlobal::Device::IOTYPE Device::suggestedOutputType(int iInternalID)
{
	Calculator *poCalculator = m_oNamedOutput[iInternalID];
	if (poCalculator) {
		if (poCalculator->isInternal())
			return KlogicGlobal::Device::INTERNAL_OUTPUT;
	}

	if (type() == DeviceType::fLED ||
	    type() == DeviceType::fSS)
		return KlogicGlobal::Device::FINAL_OUTPUT;

	if (type() == DeviceType::fSUB || type() == DeviceType::fEQU) {
		if (m_poConnectionList->get(iInternalID))
			return KlogicGlobal::Device::FINAL_OUTPUT;
	}

	return KlogicGlobal::Device::TMP_OUTPUT;
}

// remove a named internal output from this device
void Device::removeInternalName(int iInternalID)
{
	removeOutputName(iInternalID);
}

ConnectorList::iterator Device::getNamedInputIterator()
{
	return m_oNamedInput.begin();
}

ConnectorList::iterator Device::getNamedInputEndIterator()
{
	return m_oNamedInput.end();
}

ConnectorList::iterator Device::getNamedOutputIterator()
{
	return m_oNamedOutput.begin();
}

ConnectorList::iterator Device::getNamedOutputEndIterator()
{
	return m_oNamedOutput.end();
}

bool Device::hasNamedInput() const
{
	return false;
}

bool Device::hasNamedOutput() const
{
	return false;
}

// set static output-value
void Device::setOutputValue(int iOutputId, int iValue)
{
	Calculator *poCalculator = m_oNamedOutput[iOutputId];
	if (poCalculator) {
		if (poCalculator->getResult() != iValue) {
			outputHasChanged = true;
			poCalculator->flush(iValue);
		}
	}
}

void Device::setID(int iDeviceId)
{
	m_iDeviceId = iDeviceId;
}

// name of device
int Device::getID() const
{
	return m_iDeviceId;
}

// set value for undefined outputs
void Device::setUndefinedValue(int iUndefinedValue)
{
	for (ConnectorList::iterator iter = m_oNamedOutput.begin(); iter != m_oNamedOutput.end(); iter++)
		(**iter)->setUndefinedValue(iUndefinedValue);
}

int Device::getUndefinedValue() const
{
	ConnectorList::iterator iter = m_oNamedOutput.begin();
	return (**iter)->getUndefinedValue();
}

// set signal delay
void Device::setDelay(int iDelay)
{
	if (iDelay > KlogicGlobal::Device::MAXDELAY)
		iDelay = KlogicGlobal::Device::MAXDELAY;
	if (iDelay < KlogicGlobal::Device::MINDELAY)
		iDelay = KlogicGlobal::Device::MINDELAY;

	for (ConnectorList::iterator iter = m_oNamedOutput.begin(); iter != m_oNamedOutput.end(); iter++)
		(**iter)->setDelay(iDelay);
}

int Device::getDelay() const
{
	ConnectorList::iterator iter = m_oNamedOutput.begin();
	return (**iter)->getDelay();
}

// return input value
// tristate check is related to an output for which this method is called
// so it has nothing to do with the input value returned
// changed: using OR function when having more than one connection to a single input
int Device::input(int input_id)
{
	bool bUnused;
	return input(input_id, bUnused);
}

/**
 * The default is usually always tristate. If set to another value that means something like a not-open-collector input.
 * This is actually the case for JK where optional R and S inputs exist.
 */
int Device::defaultInput(int, bool&) {
	return KlogicGlobal::Device::TRISTATE;
}

int Device::input(int input_id, bool& bIsBoolean)
{
	int iValue = defaultInput(input_id, bIsBoolean);

	bIsBoolean = true;
	for (ConnectionList::iterator iter = m_poConnectionList->begin(); iter != m_poConnectionList->end(); iter++) {
		Connection *poConn = *iter;
		if (poConn->getInputID() == input_id) {
			int iValue2 = poConn->getInput();
			if (iValue2 != KlogicGlobal::Device::TRISTATE) {
				if (iValue == KlogicGlobal::Device::TRISTATE) {
					iValue = iValue2;
				} else {
					iValue |= iValue2;
				}
			}
			if (!poConn->hasBooleanInput()) {
				bIsBoolean = false;
			}
		}
	}
	return iValue;
}

// return true if input value has changed since last calculation
bool Device::inputChanged()
{
	for (ConnectionList::iterator iter = m_poConnectionList->begin(); iter != m_poConnectionList->end(); iter++) {
		if ((*iter)->inputChanged())
			return true;
	}
	return false;
}

// return output value
int Device::output(int iOutputId)
{
	Calculator *poCalculator = m_oNamedOutput[iOutputId];
	if (poCalculator)
		return poCalculator->getResult();

	qWarning() << "Device::output: no such output id";
	return 0;
}

void Device::flush(int value)
{
	for (ConnectorList::iterator iter = m_oNamedOutput.begin(); iter != m_oNamedOutput.end(); iter++)
		(**iter)->flush(value);
}

void Device::forceOutputChange()
{
	forceOutputHasChanged = true;
}

// return true if output value has changed since last calculation
bool Device::outputChanged() {
	bool retVal = outputHasChanged;
	outputHasChanged = false;
	return retVal;
}

void Device::setOutputChanged(bool changed) {
	outputHasChanged |= changed;
}

void Device::pushInputValuesToCalculationStack(Calculator *) {
}

// calculate new output-values (currently, always only old values are reachable)
void Device::Calculate(int)
{
	// check wether tristate input is active or not
	if (m_iTristateEnableID != -1) {
	    Connection *poConn = m_poConnectionList->get(m_iTristateEnableID);
	    m_bTristateEnabled = (!poConn || !poConn->getDevice()->output(poConn->getOutputID()));
	} else {
	    m_bTristateEnabled = false;
	}

	// calculate
	for (ConnectorList::iterator iter = m_oNamedOutput.begin(); iter != m_oNamedOutput.end(); iter++) {
	    iter.getCalculator()->calculate(m_bTristateEnabled);
	}
}

// set output values of all devices
// most of it is detection of output changes .. (to be optimized?)
bool Device::Propagate(int) {
	if (forceOutputHasChanged) {
		outputHasChanged = true;
		forceOutputHasChanged = false;
	}
	for (ConnectorList::iterator iter = m_oNamedOutput.begin(); iter != m_oNamedOutput.end(); iter++) {
		Calculator *poCalculator = iter.getCalculator();
		int valBefore = poCalculator->shift();
		int valAfter = poCalculator->getResult();
		if (valBefore != valAfter) {
			outputHasChanged = true;
		}
	}
	return outputHasChanged;
}

const Connector * Device::getInputConnector(const QString& sName) {
	return m_oNamedInput.find(sName);
}

const Connector * Device::getOutputConnector(const QString& sName) {
	return m_oNamedOutput.find(sName);
}

// return equation string for an output
QString Device::getEquation(int iOutputId)
{
	Calculator *poCalculator = m_oNamedOutput[iOutputId];
	if (poCalculator)
		return poCalculator->getEquation();
	return QString("");
}

// set equation string for an output
void Device::setEquation(const QString& sEquation, int iOutputId)
{
	Calculator *poCalculator = m_oNamedOutput[iOutputId];
	if (poCalculator)
		poCalculator->setEquation(sEquation);
	else
		qWarning("Device::setEquation - no such output");
}

// parse all equations, build the calculation stacks for this device
QueueInfo Device::parseEquation()
{
	for (ConnectorList::iterator iter = m_oNamedOutput.begin(); iter != m_oNamedOutput.end(); iter++) {
		Calculator *poCalculator = **iter;
		const QueueInfo& oQI = poCalculator->parse();
		if (oQI.parseError != QueueInfo::NOERR)
			return oQI;
	}
	return QueueInfo();
}

// deliveres a string like [/][qualifier_prefix.]devicename[.outputname]
// depends on the given device and output name
QString Device::qualifiedOutputName(Device *dev, const QString &out_name, const QString &qual_pref)
{	QString buf;

	QString prefix;
	if (!qual_pref.isEmpty())
		prefix = qual_pref;

	if (dev->type() == DeviceType::fINV_INTERNAL) {
		bool bDoubleInv = false;
		ConnectionList::iterator iter = m_poConnectionList->begin();
		if (iter != m_poConnectionList->end()) {
			Connection *poConn = m_poConnectionList->getFirstConnection();
			if (poConn->getDevice()->type() == DeviceType::fINV_INTERNAL) {
				poConn = poConn->getDevice()->m_poConnectionList->getFirstConnection();
				bDoubleInv = true;
			}

			if (poConn) {
				if (!bDoubleInv)
					buf.append('/');
				buf.append(prefix);
				buf.append(poConn->getDevice()->getName(NOSPACE));
			}
		}
	} else {
		if (!out_name.isEmpty()) {
			buf.append(prefix);
			buf.append(dev->getName(NOSPACE));
			buf.append('.');
			buf.append(out_name);
		} else {
			buf.append(prefix);
			buf.append(dev->getName(NOSPACE));
		}
	}

	// remove all spaces from the used names!
	int pos;
	while ( -1 != (pos = buf.indexOf(' ')))
		buf.replace(pos, 1, "_");

	return buf;
}

// append all equations of this device to the given list
// sQPrefix will be appended to each name in all equations
// bIsolate: just look at this device, not outside
void Device::getAllEquations(map<QString,OutputInfo>& oEquations, const QString& sQPrefix, bool bIsolate)
{
	QString sFinal;
	bool bFinal;

	// remember the interface circuits
	if ((sQPrefix.isEmpty() && m_iFunction == DeviceType::fOUT) ||
	                  m_iFunction == DeviceType::fSS ||
			  m_iFunction == DeviceType::fLED) {
		sFinal = "(final)";
		bFinal = true;
	} else {
		sFinal = "";
		bFinal = false;
	}

	if (hasNamedInput()) {
		// device has named inputs

		if (!bIsolate) {
			// build artificial output for each input
			for (ConnectorList::iterator iter2 = m_oNamedInput.begin(); iter2 != m_oNamedInput.end(); iter2++) {
				const Connector& oConnector = *iter2;
				// look for a connection (could be more than one)
				int connected = 0;
				QString eq;
				QString sEqPart;
				for (ConnectionList::iterator iter = m_poConnectionList->begin(); iter != m_poConnectionList->end(); iter++) {
					Connection *poConn = *iter;
					if (oConnector.getConnectorID() == poConn->getInputID()) {
						sEqPart = qualifiedOutputName(poConn->getDevice(),
							poConn->getDevice()->getOutputName(poConn->getOutputID()),
							sQPrefix);
						if (!connected) {
							eq = sEqPart;
							connected = 1;
						} else if (connected == 1) {
							eq = "(" + eq + ") + (" + sEqPart + ")";
							connected = 2;
						} else {
							eq += " + (" + sEqPart + ")";
						}

					}
				}
				if (connected) {
					OutputInfo oi;
					oi.device_id = this->getID();
					oi.output_id = 0;
					oi.output_name = qualifiedOutputName(this, oConnector.getName(), sQPrefix);
					oi.original_output_name = qualifiedOutputName(this, oConnector.getName(), sQPrefix);
					oi.output_type = KlogicGlobal::Device::INPUT;
					oi.equation = eq;
					oi.prefix = sQPrefix;
					oi.suffix = sFinal;
					oEquations[oi.output_name] = oi;
				}
			}
		}
	}

	// named output equation handling
	for (ConnectorList::iterator iter2 = m_oNamedOutput.begin(); iter2 != m_oNamedOutput.end(); iter2++) {
		const Connector& oConnector = *iter2;
		Calculator *poCalculator = *oConnector;
		if (poCalculator->hasEquation()) {
			QString sQualifier = sQPrefix;
			if (!bIsolate) {
				sQualifier.append(getName(NOSPACE));
				sQualifier.append('.');
			}

			OutputInfo oi;
			oi.device_id = getID();
			oi.output_id = oConnector.getConnectorID();
			if (poCalculator->isStatic()) {
				// concatenate all inputs with the static operation
				for (ConnectionList::iterator iter = m_poConnectionList->begin(); iter != m_poConnectionList->end(); iter++) {
					if (iter != m_poConnectionList->begin()) {
						oi.equation.append(" ");
						oi.equation.append(poCalculator->getEquation());	// the static operation
						oi.equation.append(" ");
					}
					Connection *poConn = *iter;
					oi.equation.append(qualifiedOutputName(poConn->getDevice(),
						poConn->getDevice()->getOutputName(poConn->getOutputID()),
						sQPrefix));
				}
			} else {
				// the Calculator instance has the equation
				oi.equation = poCalculator->getEquation(sQualifier);
			}
			if (!bIsolate) {
				oi.output_name = qualifiedOutputName(this, oConnector.getName(), sQPrefix);
				oi.original_output_name = qualifiedOutputName(this, oConnector.getName(), sQPrefix);
			} else {
				oi.output_name = oConnector.getName();
				oi.original_output_name = oConnector.getName();
			}
			if (!bIsolate) {
				if (bFinal) oi.output_type = KlogicGlobal::Device::FINAL_OUTPUT;
				else oi.output_type = suggestedOutputType(oi.output_id);
			} else {
				if (bFinal) oi.output_type = KlogicGlobal::Device::FINAL_OUTPUT;
				else if (suggestedOutputType(oi.output_id) != KlogicGlobal::Device::INTERNAL_OUTPUT) oi.output_type = KlogicGlobal::Device::FINAL_OUTPUT;
				else oi.output_type = KlogicGlobal::Device::INTERNAL_OUTPUT;
			}
			oi.prefix = sQPrefix;
			oi.suffix = sFinal;
			oEquations[oi.output_name] = oi;
		}
	}
}

// Update all equations of this device with the given list
// add needed, and remove obsolete inputs and outputs
// change_size flag: change device size to fit all needed input and output
void Device::setAllEquations(KlogicList<OutputInfo> *equations, bool change_size)
{
	QString dummy;

/* debug code */
	qDebug() << "-----------------------------------";
	qDebug() << "--             ENTER             --";
	qDebug() << "-----------------------------------";
	KlogicList<OutputInfo> *l = equations->First();
	while(l) {
		qDebug() << "incoming: " << l->Get()->output_name << "(" << l->Get()->output_id << ") = " << l->Get()->equation;
		l = l->Next();
	}

	for (ConnectorList::iterator iter = m_oNamedOutput.begin(); iter != m_oNamedOutput.end(); iter++) {
		const Connector& oConnector = *iter;
		Calculator *poCalculator = *oConnector;
		if ((*oConnector)->hasEquation()) {
			qDebug() << "current: " << oConnector.getName() << "(" << oConnector.getConnectorID() << ") = " << poCalculator->getEquation();
		}
	}
/**/

	// -------------------------------------------------------
	// create a list with new, and a list with all output vars
	// -------------------------------------------------------
	KlogicList<QString> all_outputs;
	KlogicList<QString> new_outputs;
	KlogicList<OutputInfo> *lout_new = equations->First();
	while(lout_new) {
		all_outputs.Append((QString *)&dummy)->setText(lout_new->Get()->output_name);
		int id = getOutputID(lout_new->Get()->output_name);
		if (!id) {
//			qDebug() << "output new: " << lout_new->Get()->output_name;
			new_outputs.Append((QString *)&dummy)->setText(lout_new->Get()->output_name);
		} else if (id != lout_new->Get()->output_id) {
//			qDebug() << "output existing but changed id: " << lout_new->Get()->output_name;
			lout_new->Get()->output_id = id;
		} else {
//			qDebug() << "output existing: " << lout_new->Get()->output_name;
		}
		lout_new = lout_new->Next();
	}

	// -------------------------------------------------------
	// replace/remove output vars
	// -------------------------------------------------------
	KlogicList<QString> *lnew = new_outputs.First();
	OutputInfo *oi;
	ConnectorList::iterator iter = m_oNamedOutput.begin();
	iter++;
	while (iter != m_oNamedOutput.end()) {
		const Connector& oConnector = *iter;

		int iOutputId = oConnector.getConnectorID();
		const QString& sOutput = oConnector.getName();

		if (!all_outputs.With(sOutput)) {
			if (lnew) {
				// replace old output with a new one
				oi = equations->Get(lnew->getText());
				iOutputId = changeOutputName(iOutputId, oi->output_name);
				setEquation(oi->equation, iOutputId);
				setOutputNameType(iOutputId, oi->output_type);
				lnew = lnew->Next();
			} else {
				// nothing to replace: remove old output
				removeOutputName(iOutputId);
				iter = m_oNamedOutput.begin();
			}
		} else {
			oi = equations->Get(sOutput);
			iOutputId = changeOutputName(iOutputId, oi->output_name);
			setEquation(oi->equation, iOutputId);
			setOutputNameType(iOutputId, oi->output_type);
		}
		iter++;
	}

	// -------------------------------------------------------
	// add new output vars left in the list
	// -------------------------------------------------------
	while(lnew) {
		if (change_size) setSize(neededSize() + 1);
		const QString& sOutput = lnew->getText();
		oi = equations->Get(sOutput);
		int iOutputId = changeOutputName(0, oi->output_name);
		setEquation(oi->equation, iOutputId);
		setOutputNameType(iOutputId, oi->output_type);
		lnew = lnew->Next();
	}

	// -------------------------------------------------------
	// create a list with new, and a list with all input vars
	// -------------------------------------------------------
	lout_new = equations->First();
	set<QString> newInputNames;
	set<QString> allNewInputNames;
	while(lout_new) {
		// create a list of all input symbols available in equation
		SymbolicCalc oop;
		oop.setEquation(lout_new->Get()->equation);
		oop.parse();

		int i = 0;
		QString input;
		while(NULL != (input = oop.getSymbol(i++))) {
			if (!Calculator::isNumber(input)) {
				if (!equations->With(input) && inputNameIsUnique(input)) {
					newInputNames.insert(input);
				}
				allNewInputNames.insert(input);
			}
		}
		lout_new = lout_new->Next();
	}

	// ----------------------------------------------------------------------------------------
	// replace deleted input vars with new ones,
	// or remove them when no new ones are available anymore
	// remove input vars if there is an output with equal name
	// ----------------------------------------------------------------------------------------
	set<QString>::iterator newInputIter = newInputNames.begin();
	int input_id;
	for (ConnectorList::iterator iter = m_oNamedInput.begin(); iter != m_oNamedInput.end(); iter++) {
		const Connector& oConnector = *iter;
		input_id = oConnector.getConnectorID();
		QString input = oConnector.getName();

		if (!isTristateControlInput(input_id)) {   // never change en control input!
			if (allNewInputNames.find(input) == allNewInputNames.end()) {
				if (newInputIter != newInputNames.end()) {
					// replace old input with a new one
					changeInputName(input_id, *newInputIter);
					newInputIter++;
				} else {
					// nothing to replace: remove old input
					removeInputName(input_id);
					iter = m_oNamedInput.begin();
				}
			}
		}
	}

	// -------------------------------------------------------
	// add remaining input vars
	// -------------------------------------------------------
	while(newInputIter != newInputNames.end()) {
		if (change_size) setSize(neededSize() + 1);
		addInputName(*newInputIter);
		newInputIter++;
	}

/* debug code */
	qDebug() << "-----------------------------------";
	qDebug() << "--             EXIT              --";
	qDebug() << "-----------------------------------";
	for (ConnectorList::iterator iter = m_oNamedInput.begin(); iter != m_oNamedInput.end(); iter++) {
		const Connector& oConnector = *iter;
		qDebug() << "input: " << oConnector.getName() << "(" << oConnector.getConnectorID() << ")";
	}
	for (ConnectorList::iterator iter = m_oNamedOutput.begin(); iter != m_oNamedOutput.end(); iter++) {
		const Connector& oConnector = *iter;
		qDebug() << "output: " << oConnector.getName() << "(" << oConnector.getConnectorID() << ")";
	}
/**/
}

