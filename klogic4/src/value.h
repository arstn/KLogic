#ifndef __COMMON_CLASSES__
#define __COMMON_CLASSES__

#include <qstring.h>

#include "KlogicGlobal.h"
#include "fifo.h"
#include <iostream>

class OutputInfo;
class Value;
class DelayedValue;

// equation information for the Equations dialog
class OutputInfo
{
public:
	OutputInfo()
	{
		device_id = 0;
		output_id = 0;
		output_type = KlogicGlobal::Device::NOTYPE;
	}

	~OutputInfo() {}

	QString getOutputName() const
	{
		int pos = output_name.indexOf('.');
		if (pos >= 0) {
			int length = output_name.length() - pos - 1;
			return output_name.right(length);
		}
		return output_name;
	}

	int outputChanged()
	{
		return output_name != original_output_name;
	}

	int device_id;
	int output_id;
	QString equation;
	QString output_name;
	QString original_output_name;

	KlogicGlobal::Device::IOTYPE output_type;
	QString prefix;		// object prefix
	QString suffix;		// output name extension: "(final)"|"(intern)"
};

// a basic input or output
class Value
{
public:
	Value() {
		internal = false;
		result = 0;
		undefined_result = 0;
		tristate = false;
	}

	// -----------------------------------
	inline void setValue(int val) {
		result = val;
	}

	inline int getValue() const {
		return result;
	}

	// -----------------------------------
	inline void setInternal(bool val) {
		internal = val;
	}

	inline bool isInternal() const {
		return internal;
	}

	// -----------------------------------
	inline void setUndefinedValue(int _undef) {
		undefined_result = _undef;
	}

	inline int getUndefinedValue() const {
		return undefined_result;
	}

	// -----------------------------------
	inline bool isTristate() const {
		return tristate;
	}
	 
	inline void setTristate(bool flag) {
		tristate = flag;
	}

protected:
	int undefined_result;
	int result;

private:
	bool internal;
	bool tristate;
};

// a basic output with a delayed output value
class DelayedValue : public Value, public Fifo
{
public:
	DelayedValue(int delay) : Value(), Fifo(delay) {
		delayed_result = 0;
	}

	inline void flush(int _value) {
		Fifo::flush(_value);
		result = _value;
		delayed_result = _value;
	}

	// return delayed result
	inline int getResult() const {
		return delayed_result;
	}

	// shift result value into fifo, return dropped value
	inline int shift() {
		int ret = put(result);
		delayed_result = get();
		//cout << "new output value: " << delayed_result << " current input value " << result << endl;
		return ret;
	}

	// set new result value
	inline void setValue(int _value) {
		result = _value;
	}

private:
	int delayed_result;
};

#endif

