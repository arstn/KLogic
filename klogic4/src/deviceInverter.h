#ifndef INVERTER_DEVICE_CLASS
#define INVERTER_DEVICE_CLASS

#include <qpainter.h>
#include "deviceSimple.h"

// internal inverter device
class InverterDevice : public SimpleDevice
{
public:
	InverterDevice(const QPoint& oPos, int size = -1)
		: SimpleDevice(DeviceType::fINV_INTERNAL, oPos, size)
	{
	}

	virtual ~InverterDevice()
	{
	}

	virtual void init()
	{
		XDevice::init();
	}

	virtual void * getInstance()
	{
		return (void *)this;
	}

	virtual void setEquation()
	{
		Device::setEquation(Operator::CHR_NOT);
	}

	virtual bool sizeChangeable() const
	{
		return false;
	}

	virtual bool isEditable()
	{
		return false;
	}

	virtual bool outputChanged()
	{
		// this device does not appear in circuit lists - so it is not calculated as usual!
		return inputChanged();
	}

	virtual int output(int)
	{
		// this device does not appear in circuit lists - so it is not calculated as usual!
		Device::Calculate(0);
		Device::Propagate(0);
		return Device::output();
	}

	virtual void setImage()
	{
	}

	virtual Image * createImage(const ImageKey&)
	{
		return 0;
	}

	virtual void printImage(QPainter *, const QRect &)
	{
	}
};

#endif
