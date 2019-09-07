#ifndef __DEVICE_PWR__
#define __DEVICE_PWR__

#include <qpainter.h>
#include "xdevice.h"

// static output value device
class PowerDevice : public XDevice
{
public:
	PowerDevice(const QPoint& oPos, int iSize = -1)
		: XDevice(DeviceType::fPWR, oPos, iSize)
	{
		m_iInputValue = 0;
	}

	virtual ~PowerDevice()
	{
	}

	virtual void init()
	{
		m_bChangedOutput = true;
		m_bLastChangedOutput = false;
		deviceOutputInverted = false;
		maxInputCount = 0;
		maxOutputCount = 1;

		XDevice::init();
	}

	virtual void setEquation()
	{
		Device::setEquation(Operator::CHR_NONE);
	}

	virtual bool sizeChangeable() const
	{
		return false;
	}

	virtual bool isEditable()
	{
		return false;
	}

	virtual bool hasStaticInput() const
	{
		return true;
	}

	// set static output-value
	virtual void toggleStaticInput()
	{
		m_bChangedOutput = true;
		if (m_iInputValue) m_iInputValue = 0;
		else m_iInputValue = 1;
		m_oImageKey.setOutput(m_iInputValue);
	}

	// set static input-value
	virtual void setInputValue(int val)
	{
		if (val != m_iInputValue)
			m_bChangedOutput = true;
		m_iInputValue = val;
		m_oImageKey.setOutput(m_iInputValue);
	}

	virtual void setImage()
	{
		XDevice::setImage();
		setVOffset(5);
	}

	virtual Image * createImage(const ImageKey& oKey) {
		if (oKey.getOutput()) {
			return m_oBasicImageCache.getImage(BasicImageCache::BODY_POS);
		} else {
			return m_oBasicImageCache.getImage(BasicImageCache::BODY_NEG);
		}
	}

	virtual void printImage(QPainter *p, const QRect &r)
	{
		int a = r.x() + 2;
		int b = r.y() + (r.height() / 2) + 2;
		int c = r.x() + r.width() - XObject::IOREG - 2;
		int d = r.x() + ((r.width() - XObject::IOREG) / 2);
		p->drawArc(r.x(), r.y() + 3, r.width() - XObject::IOREG, r.height() - 3, 0, 5760);
		p->drawLine(a, b, c, b);
		if (m_iInputValue)
			p->drawLine(d, r.y() + 5, d, r.y() + r.height() - 2);
		// extended connection line
		p->drawLine(r.x() + r.width() - XObject::IOREG, b, r.x() + r.width(), b);
	}

	virtual bool isObservable()
	{
		return false;
	}

	virtual int output(int = 0)
	{
		return m_iInputValue;
	}

	virtual bool outputChanged()
	{
		return m_bChangedOutput;
	}

	virtual void forceOutputChange()
	{
		m_bChangedOutput = true;
	}

	virtual void Calculate(int)
	{
	}

	virtual bool Propagate(int)
	{
		if (m_bChangedOutput) {
			if (!m_bLastChangedOutput) {
				m_bLastChangedOutput = true;
			} else {
				m_bLastChangedOutput = false;
				m_bChangedOutput = false;
			}
			
			const Image *poImage = 0;
			if (m_iInputValue)
				poImage = m_oBasicImageCache.getImage(BasicImageCache::BODY_POS);
			else
				poImage = m_oBasicImageCache.getImage(BasicImageCache::BODY_NEG);
			XObject::setImage(poImage);
		}
		setOutputChanged(m_bChangedOutput);
		return m_bChangedOutput;
	}

private:
	int m_iInputValue;
	bool m_bChangedOutput;
	bool m_bLastChangedOutput;
};

#endif
