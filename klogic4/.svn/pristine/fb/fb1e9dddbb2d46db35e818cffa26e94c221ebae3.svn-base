#ifndef __DEVICE_SWITCH__
#define __DEVICE_SWITCH__

#include <qpainter.h>
#include "deviceSimple.h"

// interface devices for circuit
class SwitchDevice : public SimpleDevice
{
public:
	SwitchDevice(const QPoint& oPos, int iSize = -1)
		: SimpleDevice(DeviceType::fSWI, oPos, iSize)
	{
		m_iInputValue = 0;
		m_oImageKey.setOutput(m_iInputValue);
	}

	virtual ~SwitchDevice()
	{
	}

	virtual void init()
	{
		deviceOutputInverted = false;
		maxInputCount = 0;
		hasInputRegion(false);
		m_bToggle = false;

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

	virtual bool isInteractive()
	{
		return true;
	}

	virtual bool isEditable()
	{
		return false;
	}

	virtual bool hasStaticInput() const
	{
		return true;
	}

	// push button or switch button?
	virtual bool isToggle() const
	{
		return m_bToggle;
	}

	virtual void setToggle(bool bToggle)
	{
		if (m_bToggle != bToggle) {
			m_bToggle = bToggle;
			if (!Device::IMPORT_IGNORE_GLOBAL)
				setImage();
		}
	}

	// set static output-value
	virtual void toggleStaticInput()
	{
		m_iInputValue = !m_iInputValue;
		m_oImageKey.setOutput(m_iInputValue);

		const Image *poCachedImage = m_oImageCache.getImage(m_oImageKey);
		if (poCachedImage) {
			XObject::setImage(poCachedImage);
		} else {
			Image *poImage = createImage(m_oImageKey);
			m_oImageCache.addImage(m_oImageKey, poImage);
			XObject::setImage(poImage);
		}
		forceOutputChange();
	}

	// set static input-value
	virtual void setInputValue(int val)
	{
		if (val != m_iInputValue) {
			toggleStaticInput();
		}
	}

	virtual void setImage()
	{
		XDevice::setImage();
		setHOffset(-2);
	}

	virtual Image * createImage(const ImageKey& oKey)
	{
		setHOffset(-2);
		if (oKey.getOutput())
			return m_oBasicImageCache.getImage(BasicImageCache::BODY_SWI1);
		else
			return m_oBasicImageCache.getImage(BasicImageCache::BODY_SWI0);
	}

	virtual void printImage(QPainter *p, const QRect &r)
	{
		QRect rr(r);
		rr.setY(r.y() + 12);
		rr.setHeight(r.height() - 20);
		p->drawRect(rr);
		int a = rr.x() + 8;
		int b = rr.x() + rr.width() - 7;
		int c = rr.y() + (rr.height() / 2) + 1;
		p->drawLine(rr.x() + 2, c, a, c);
		p->drawLine(b, c, rr.x() + rr.width() - 2, c);
		p->drawLine(a, c, b + 1, c - 3);
		p->drawLine(b, c, b, c - 4);
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
		}
		setOutputChanged(m_bChangedOutput);
		return m_bChangedOutput;
	}

private:
	int m_iInputValue;
	bool m_bChangedOutput;
	bool m_bLastChangedOutput;
	bool m_bToggle;		// push button or switch?
};

#endif
