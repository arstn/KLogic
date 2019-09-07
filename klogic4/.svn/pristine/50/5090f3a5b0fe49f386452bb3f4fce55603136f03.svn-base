#ifndef __DEVICE_IO__
#define __DEVICE_IO__

#include <set>
using namespace std;

#include <qpainter.h>
#include "deviceSimple.h"

// interface devices for circuit
class IODevice : public SimpleDevice
{
public:
	IODevice(int iFunction, const QPoint& oPos, int size = -1)
		:SimpleDevice(iFunction, oPos, size)
	{
		m_poImage = new Image;
	}

	virtual ~IODevice()
	{
		m_poImage->relRef();
	}

	virtual void init()
	{
		deviceOutputInverted = false;
		m_bHasBooleanOutput = true;
		SimpleDevice::init();
	}

	virtual void setEquation()
	{
		Device::setEquation(Operator::CHR_INTERFACE);
	}

	virtual bool sizeChangeable() const
	{
		return false;
	}

	virtual bool isEditable()
	{
		return false;
	}

	virtual bool hasBooleanOutput(int) const
	{
		return m_bHasBooleanOutput;
	}

	virtual void setHasBooleanOutput(int, bool bHasBooleanInput)
	{
		m_bHasBooleanOutput = bHasBooleanInput;
	}

	virtual void printImage(QPainter *p, const QRect &r)
	{
		p->setFont(KlogicGlobal::Device::getFont());
		p->drawText(r.x() + 6, r.y() + 9, getName());
	}

protected:
	Image *m_poImage;		// they are not cached, and each device has its own image

private:
	int m_bHasBooleanOutput;	// if not boolean, wires need to be drawn fat
};

// interface devices for circuit
class InputDevice : public IODevice
{
public:
	InputDevice(const QPoint& oPos, int iSize = -1)
		: IODevice(DeviceType::fIN, oPos, iSize)
	{
		setInputValue(0);
	}

	virtual ~InputDevice()
	{
	}

	virtual void init()
	{
		m_bChangedOutput = true;
		maxInputCount = 0;
		hasInputRegion(false);
		IODevice::init();
	}

	// set static input-value
	virtual void setInputValue(int iInputValue)
	{
		m_bChangedOutput = (iInputValue != m_iInputValue || m_bForcedChangedOutput);
		m_bForcedChangedOutput = false;
		m_iInputValue = iInputValue;
	}

	virtual void setImage()
	{
		QPoint pt(0, 0);
		QPainter p;
		QPoint pp(15, 35);

		Image *part = m_oBasicImageCache.getImage(BasicImageCache::BODY_IN);
		m_poImage->relRef();
		m_poImage = new Image(part->width(), part->height());

		p.begin(m_poImage);
		p.drawPixmap(pt, *part);
		// draw symbol
		p.setFont(KlogicGlobal::Device::getFont());
		p.drawText(10, 9, getName());
		p.end();

		setVOffset(5);
		XObject::setImage(m_poImage);
		if (m_poTextDev) m_poTextDev->setImage();
	}

	virtual int output(int = 0)
	{
		return m_iInputValue;
	}

	virtual bool outputChanged()
	{
		return m_bChangedOutput;
	}

	virtual bool inputChanged()
	{
		return m_bChangedOutput;
	}

	virtual void forceOutputChange()
	{
		m_bForcedChangedOutput = true;
	}

	virtual void Calculate(int)
	{
	}

	virtual bool Propagate(int)	{
		setOutputChanged(m_bChangedOutput);
		return m_bChangedOutput;
	}

private:
	int m_iInputValue;
	bool m_bChangedOutput;
	bool m_bForcedChangedOutput;
};

class OutputDevice : public IODevice
{
public:
	OutputDevice(const QPoint& oPos, int iSize = -1)
		: IODevice(DeviceType::fOUT, oPos, iSize)
	{
	}

	virtual ~OutputDevice()
	{
	}

	virtual void init()
	{
		maxInputCount = 1;
		hasOutputRegion(false);
		IODevice::init();
	}

	virtual bool hasBooleanOutput(int) const
	{
		for (ConnectionList::iterator iter = m_poConnectionList->begin(); iter != m_poConnectionList->end(); iter++) {
			if (!(*iter)->hasBooleanInput())
				return false;
		}
		return true;
	}

	virtual void setImage()
	{
		QPoint pt(0, 0);
		QPainter p;
		QPoint pp(15, 35);

		const Image *part = m_oBasicImageCache.getImage(BasicImageCache::BODY_OUT);
		m_poImage->relRef();
		m_poImage = new Image(part->width(), part->height());

		p.begin(m_poImage);
		p.drawPixmap(pt, *part);
		// draw symbol
		p.setFont(KlogicGlobal::Device::getFont());
		p.drawText(10, 9, getName());
		p.end();

		setVOffset(5);
		XObject::setImage(m_poImage);
		if (m_poTextDev) m_poTextDev->setImage();
	}
};

#endif
