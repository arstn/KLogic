/*************************************************/
/* this interface class is used for customizing  */
/* devices                                       */
/*                                               */
/* all methods that are not part of device.cpp   */
/* can be ivirtually added here, to make them    */
/* visible in xdevice                            */
/*                                               */
/* Andreas Rostin                                */
/* 06.10.01                                      */
/*************************************************/
#ifndef __DEVICE_INTERFACE__
#define __DEVICE_INTERFACE__
#include "xdevice.h"
#include "device.h"

// ------------------------------------------------------------
// all methods defined here should be virtual
// ------------------------------------------------------------
class DeviceInterface : public Device
{
public:
	DeviceInterface(int iFunction)
		:Device(iFunction)
	{
	}

	virtual ~DeviceInterface()
	{
	}

	// -----------------------------------------------------
	// Oscillator device
	// -----------------------------------------------------
	virtual int getOscOn() { return 0; }
	virtual void setOscOn(int) {}
	virtual int getOscOff() { return 0; }
	virtual void setOscOff(int) {}
	virtual int getCurrOscStep() { return 0; }
	virtual void setCurrOscStep(int) {}
	virtual void setOscType(int) {}
	virtual int getOscType() const { return 0; }

	// -----------------------------------------------------
	// RAM device
	// -----------------------------------------------------
	virtual int RAMSize() { return 0; }			// size of ram device
	virtual void setRAMSize(int) {}
	virtual void setRAMStorage(char *) {}			// replace ram content copletely
	virtual char *getRAMStorageCopy() { return NULL; }	// returns a copy of the ram

	// -----------------------------------------------------
	// Bus device
	// -----------------------------------------------------
	virtual bool isMux() { return false; }
	virtual void setMux(bool) {}

	// -----------------------------------------------------
	// Switch/Power device
	// -----------------------------------------------------
	virtual bool hasStaticInput() const { return false; }
	virtual bool isToggle() const { return false; }
	virtual void setToggle(bool) {}
	virtual void toggleStaticInput() {}
	virtual void setInputValue(int) {}

	// -----------------------------------------------------
	// IO device, Switch device
	// -----------------------------------------------------
	virtual bool isEditable() { return true; }
	virtual bool isInteractive() { return false; }
	virtual void setHasBooleanOutput(int, bool) {}

	// -----------------------------------------------------
	// all FF devices
	// -----------------------------------------------------
	virtual void setClock(KlogicGlobal::Device::CLOCKTYPE) {}
	virtual KlogicGlobal::Device::CLOCKTYPE clock() const { return KlogicGlobal::Device::CLK_NONE; }
	virtual bool hasClock() const { return false; }
	virtual bool hasMaster() const { return false; }
	virtual void setMaster(bool) {}

	// common
	virtual void setUndefinedValue(int val) { Device::setUndefinedValue(val); }
	virtual void setEquation() {}
protected:

private:

};

#endif
