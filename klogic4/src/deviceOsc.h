#ifndef __DEVICE_OSCOLLATOR__
#define __DEVICE_OSCOLLATOR__

#include <qpainter.h>
#include "xdevice.h"
#include "connection.h"

class OscillatorDevice : public XDevice
{
public:
	enum OSCTYPE {TYPE_MONO = 5, TYPE_MULTI = 8};

	OscillatorDevice(const QPoint& oPos, int iSize = -1)
		: XDevice(DeviceType::fOSC, oPos, iSize)
	{
	}

	virtual ~OscillatorDevice()
	{
	}

	virtual void init()
	{
		m_iOffPeriod = DEFAULT_OSC_PERIOD;
		m_iOnPeriod = DEFAULT_OSC_PERIOD;
		m_iPeriodCnt = 0;

		setOscType(TYPE_MULTI);

		deviceOutputInverted = false;
		setSize(KlogicGlobal::Device::MINSIZE);
		m_iOutputVal = 0;

		XDevice::init();
	}

	virtual void setEquation()
	{
		Device::setEquation(Operator::CHR_NONE);
	}

	virtual void setOscType(int iType) {
		if (iType == TYPE_MULTI || iType == TYPE_MONO) {
			m_iType = iType;
		}
		if (m_iType == TYPE_MULTI)
			maxInputCount = 0;
		else {
			maxInputCount = 1;
		}
		m_oImageKey.setDevType(m_iType);
		if (!Device::IMPORT_IGNORE_GLOBAL) setImage();
	}

	virtual int getOscType() const
	{
		return m_iType;
	}

	virtual bool sizeChangeable() const
	{
		return false;
	}

	virtual Image * createImage(const ImageKey& oKey)
	{
		QPoint pt(0, 0);
		QPainter p;
		QPoint pp(15, 35);

		const Image *part1 = m_oBasicImageCache.getImage(BasicImageCache::BODY_TOP);
		const Image *part2 = m_oBasicImageCache.getImage(BasicImageCache::BODY_MID);
		const Image *part3 = m_oBasicImageCache.getImage(BasicImageCache::BODY_BOT);
		int iHeight =  part1->height() + (part2->height() * getSize()) + part3->height();
		Image *poImage = new Image(part1->width(), iHeight);

		p.begin(poImage);

		// the body
		p.drawPixmap(pt, *part1);
		pt.setY(pt.y() + part1->height());
		for(int i = 0; i < getSize(); i++) {
			p.drawPixmap(pt, *part2);
			pt.setY(pt.y() + part2->height());
		}
		p.drawPixmap(pt, *part3);

		// the symbol
		if (oKey.getDevType() == TYPE_MULTI) {
			part1 = m_oBasicImageCache.getImage(BasicImageCache::SYMBOL_OSC);
			pp.setX(poImage->width()/2 - part1->width()/2);
			pp.setY(poImage->height()/2 - part1->height()/2);
			p.drawPixmap(pp, *part1);
		} else {
			part1 = m_oBasicImageCache.getImage(BasicImageCache::SYMBOL_MONOFLOP);
			pp.setX(poImage->width()/2 - part1->width()/2);
			pp.setY(poImage->height()/2 - part1->height()/2);
			p.drawPixmap(pp, *part1);
		}
		p.end();
		return poImage;
	}

	virtual void printImage(QPainter *p, const QRect &r)
	{	int a,d,e,h;

		if (m_iType == TYPE_MULTI) {
			a = r.x() + 2;
			h = (r.width() - IOREG) / IOREG;
			d = r.y() + (r.height() / 2) - 4;
			e = r.y() + (r.height() / 2) + 4;
			p->drawLine(a, e, a + h, e);
			p->drawLine(a + h, e, a + h, d);
			p->drawLine(a + h, d, a + h + h, d);
			p->drawLine(a + h + h, d, a + h + h, e);
			p->drawLine(a + h + h, e, a + h + h + h, e);
			p->drawLine(a + h + h + h, e, a + h + h + h, d);
			p->drawLine(a + h + h + h, d, a + h + h + h + h, d);
		} else {
			a = r.x() + 2;
			h = (r.width() - IOREG) / IOREG;
			d = r.y() + (r.height() / 2) - 4;
			e = r.y() + (r.height() / 2) + 4;
			p->drawLine(a, e, a + h, e);
			p->drawLine(a + h, e, a + h, d);
			p->drawLine(a + h, d, a + h + h, d);
			p->drawLine(a + h + h, d, a + h + h, e);
			p->drawLine(a + h + h, e, a + h + h + h + h, e);
		}
	}

	// return device's output value
	virtual int output(int) {
		return m_iOutputVal;
	}

	virtual void Calculate(int burst_step) {
		m_iOldOutputVal = m_iOutputVal;
		if (!burst_step) {
			if (m_iType == TYPE_MULTI) {
				m_iPeriodCnt++;
				if (m_iOutputVal && m_iPeriodCnt >= m_iOnPeriod) {
					m_iOutputVal = 0;
					m_iPeriodCnt = 0;
				}
				if (!m_iOutputVal && m_iPeriodCnt >= m_iOffPeriod) {
					m_iOutputVal = 1;
					m_iPeriodCnt = 0;
				}
			} else {
				Device *poDevice = m_poConnectionList->getFirstDevice();
				if (poDevice) {
					if (m_iOutputVal) {
						if (m_iPeriodCnt > m_iOnPeriod) m_iOutputVal = 0;
						else m_iPeriodCnt++;
					} else {
						if (poDevice->output()) {
							if (!m_iPeriodCnt) m_iOutputVal = 1;
						} else m_iPeriodCnt = 0;
					}
				}
			}
		}
	}

	virtual bool Propagate(int) {
		bool outputHasChanged = (m_iOldOutputVal != m_iOutputVal);
		setOutputChanged(outputHasChanged);
		return (outputHasChanged);
	}

	virtual int getOscOn()
	{
		return m_iOnPeriod;
	}
	 
	virtual void setOscOn(int value)
	{
		m_iOnPeriod = value;
	}
	 
	virtual int getOscOff()
	{
		return m_iOffPeriod;
	}

	virtual void setOscOff(int value)
	{
		m_iOffPeriod = value;
	}
	 
	// only for export
	virtual int getCurrOscStep()
	{
		return m_iPeriodCnt;
	}
	 
	// only for import
	virtual void setCurrOscStep(int curr_step)
	{
		m_iPeriodCnt = curr_step;
	}

	static const int MAXOSCCNT = 10000;
	static const int DEFAULT_OSC_PERIOD = 10;

private:
	// oszillator timing
	int m_iOffPeriod;
	int m_iOnPeriod;
	int m_iPeriodCnt;
	int m_iOutputVal;
	int m_iOldOutputVal;
	int m_iType;
};

#endif
