#ifndef SIMPLE_DEVICE_CLASS
#define SIMPLE_DEVICE_CLASS

#include <qpainter.h>
#include "xdevice.h"

// simple device with same operation for all input and common output
class SimpleDevice : public XDevice
{
public:
	SimpleDevice(int iFunction, const QPoint& oPos, int iSize = -1);
	virtual ~SimpleDevice();

	virtual void init();

	virtual void pushInputValuesToCalculationStack(Calculator *calculator);
	virtual void setEquation();
	virtual Image * createImage(const ImageKey& oKey);
};

#endif
