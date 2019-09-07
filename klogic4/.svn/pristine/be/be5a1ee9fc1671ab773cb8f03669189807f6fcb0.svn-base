#ifndef __Device_CLASS__
#define __Device_CLASS__

#include <map>
using namespace std;

class QString;

#include "calculator.h"
#include "value.h"
#include "fifo.h"
#include "klogicList.h"
#include "DeviceType.h"
#include "connector.h"
class Circuit;
class BusOutputArray;
class ConnectionList;

// a logic device
class Device
{
public:
	static int instance;

	Device(int iFunction);
	virtual ~Device();

	//***** type and status methods
	int type() const;
	int getDelay() const;
	void setDelay(int);
	int getID() const;
	void setID(int);
	QString getName(bool bNoSpace = false) const;
	virtual void setName(const QString&);
	virtual int getUndefinedValue() const;
	virtual void setUndefinedValue(int);

	virtual bool setSize(int) { return false; }
	virtual int getSize() { return 0; }
	virtual int neededSize() { return 0; }
	int getNextInputId();

	virtual bool hasBooleanOutput(int) const { return true; }

	//***** methods to manage named IO
	virtual int addInputName(const QString&, int pos = -1, int input_id = 0);
	virtual int changeInputName(int, const QString&);
	virtual void removeInputName(int);
	virtual void mapInputName(int, int, const QString&);

	virtual int addOutputName(const QString&, int pos = -1, int output_id = 0);
	virtual int changeOutputName(int, const QString&);
	virtual void removeOutputName(int);
	virtual void mapOutputName(int, int, const QString&);
	const QString& getOutputName(int);

	int getInputID(const QString&);
	int getOutputID(const QString&);
	virtual bool hasNamedInput() const;
	virtual bool hasNamedOutput() const;
	bool inputNameIsUnique(const QString&);

	void setOutputNameType(int, KlogicGlobal::Device::IOTYPE);
	virtual bool outputIsActive(int);	// returns wether output is active or tristate
	void setOutputTristateEnabled(int, bool);	// set output able to be tristate
	bool isOutputTristateEnabled(int);		// returns if output could be tristate
	void setDeviceTristateEnabled(bool);		// set device able to be tristate
	virtual bool isDeviceTristateEnabled();			// returns if device could be tristate
	bool isTristateControlInput(int);	// returns if input is the tristate control input
	void addTristateControlInput(int position = 0);
	void removeTristateControlInput();
	bool hasTristateControlInput();
	bool isTristateControlInputEnabled();
	int getTristateControlInputPosition();
	
	int addInternalName(const QString&);
	void removeInternalName(int);

	ConnectorList::iterator getNamedInputIterator();
	ConnectorList::iterator getNamedInputEndIterator();
	ConnectorList::iterator getNamedOutputIterator();
	ConnectorList::iterator getNamedOutputEndIterator();

	//***** calculation methods
	virtual void pushInputValuesToCalculationStack(Calculator *calculator);
	virtual void Calculate(int);		// calculate device
	virtual bool Propagate(int);		// propagate output value
	void flush(int);					// set all outputs to a new value

	//***** get values
	virtual int input(int input_id = 0);
	virtual int input(int input_id, bool& bIsBoolean);
	virtual int defaultInput(int input_id, bool& bIsBoolean);	// what to return if not connected to anything
	virtual int output(int output_id = 0);
	virtual bool outputChanged();		// output changed since last access?
	virtual bool inputChanged();		// input changed since last access?
	virtual void forceOutputChange();	// force output changed since last access

	void setOutputValue(int, int);		// set a named output to a new value (used for net devices only)

	//***** equation related methods
	const Connector * getOutputConnector(const QString&);
	const Connector * getInputConnector(const QString&);
	QString getEquation(int);		// return the equation for a specific output
	void setEquation(const QString&, int output_id = 0);
	virtual QueueInfo parseEquation();	// setup operation stacks from the equation strings
	// returns the correct name for an input
	QString qualifiedOutputName(Device *, const QString&, const QString &qual_pref = (char *)NULL);
	// append all equations of this device to the given list
	virtual void getAllEquations(map<QString, OutputInfo>&, const QString&, bool);
	// actialize device's equations with the given list
	void setAllEquations(KlogicList<OutputInfo> *, bool change_size = true);

	// connection methods
	bool connectDevices(Device *, int, int);
	bool disconnectDevices(Device *, int, int);

	static bool IMPORT_IGNORE_GLOBAL;

protected:
	void setOutputChanged(bool);			// for circuits
	KlogicGlobal::Device::IOTYPE suggestedOutputType(int);

	ConnectorList m_oNamedOutput;	// named output
	ConnectorList m_oNamedInput;	// named input

	// input-connection of device
	ConnectionList *m_poConnectionList;

private:
	static const QString INPUT_TRISTATE_CONTROL_NAME;

	static const bool NOSPACE = false;

	int m_iDeviceId;		// unique device id
	int m_iFunction;		// type/function of the device
	QString m_sName;		// device name

	int forceOutputHasChanged;	// output change detection
	bool outputHasChanged;

	int m_iTristateEnableID;	// tristate "EN" input id
	bool m_bTristateEnabled;	// tristate is active?

public:
	// old file support
	bool string2device(QString);
};

#endif

