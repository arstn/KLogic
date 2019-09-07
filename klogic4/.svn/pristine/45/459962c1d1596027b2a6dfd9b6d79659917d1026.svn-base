#ifndef __DEVICE_TEXT__
#define __DEVICE_TEXT__

#include <qpainter.h>
#include "xdevice.h"

class TextDevice : public XDevice
{
public:
	TextDevice(const QPoint& oPos, int iSize = -1)
		: XDevice(DeviceType::fTXT, oPos, iSize)
	{
		m_poImage = new Image(1,1);
	}

	virtual ~TextDevice()
	{
		m_poImage->relRef();
	}

	virtual void init()
	{
		deviceOutputInverted = false;
		setSize(KlogicGlobal::Device::MINSIZE);
		maxInputCount = 0;
		maxOutputCount = 0;
		hasInputRegion(false);
		hasOutputRegion(false);
		XDevice::init();
	}

	virtual void setEquation()
	{
		Device::setEquation(Operator::CHR_NONE);
	}

	virtual void displayName(bool)
	{
	}

	virtual void setName(const QString& sName)
	{
		XDevice::setName(sName);
		setImage();
	}

	virtual bool sizeChangeable() const
	{
		return false;
	}

	virtual void setImage()
	{
		setVOffset(-3);

		QPainter p;
		p.begin(m_poImage);
		p.setFont(KlogicGlobal::Device::getFont());
		QRect r = p.boundingRect(0, 0, 10, 10, Qt::AlignLeft, getName());
		p.end();

		if (r.width() < 1) r.setWidth(10);
		if (r.height() < 1) r.setHeight(8);
		m_poImage->relRef();
		m_poImage = new Image(r.width() + 4, 12);

		p.begin(m_poImage);
		p.setBrush(Qt::white);
		p.setPen(Qt::black);
		p.drawRect(m_poImage->rect());
		p.setFont(KlogicGlobal::Device::getFont());
		p.drawText(3, m_poImage->height() - 2, getName());
		p.end();

		XObject::setImage(m_poImage);
	}

	virtual void printImage(QPainter *, const QRect &)
	{
	}

	// return device's output value
	virtual int output(int)
	{
		return 0;
	}

	virtual bool outputChanged()
	{
		return false;
	}

	virtual void Calculate(int)
	{
	}

	virtual bool Propagate(int) {
		return false;
	}
private:
	Image *m_poImage;
};

#endif
