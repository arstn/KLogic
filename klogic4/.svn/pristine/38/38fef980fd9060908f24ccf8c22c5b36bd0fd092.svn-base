#ifndef __DEVICE_CUSTOM__
#define __DEVICE_CUSTOM__

#include <qpainter.h>
#include "imageCache.h"
#include "xdevice.h"
#include "xwire.h"
#include "busOutputArray.h"
#include "DeviceType.h"
#include "grid.h"

class EquationDevice;
class TristateDevice;

class CustomDevice : public XDevice
{
public:
	CustomDevice(int iFunction, const QPoint& oPos, int iSize = -1)
		: XDevice(iFunction, oPos, iSize)
	{
		if (Device::IMPORT_IGNORE_GLOBAL)
			m_iWidth = KlogicGlobal::Device::DEFAULT_WIDTH;
		else
			m_iWidth = KlogicGlobal::Device::getWidth();
	}

	virtual ~CustomDevice()
	{
	}

	virtual void init()
	{
		XDevice::init();
	}

	virtual bool hasNamedInput() const
	{
		return true;
	}

	virtual bool hasNamedOutput() const
	{
		return true;
	}

	void setWidth(int iWidth)
	{
		bool bChanged = (m_iWidth != iWidth);
		m_iWidth = iWidth;

		if (bChanged) {
			setImage();
			KlogicList<XWire> *lw = m_oOutgoingWires.First();
			while (lw) {
				// set new width in connected wire node
				XWire *oWire = lw->Get();
				int idx = oWire->getOutputIndex(this);
				QPoint oPt = oWire->getOutputPosition(idx);

				oPt.setX(getGeometry().x() + getGeometry().width() - 1);
				oWire->setOutputPosition(oPt, idx);

				lw = lw->Next();
			}
			m_oImageKey.setWidth(iWidth);
			if (!Device::IMPORT_IGNORE_GLOBAL) setImage();
		}
	}

	virtual int width()
	{
		return m_iWidth;
	}

	virtual void setImage()
	{
		const Image *part = 0;
		QPoint pt(0,0);
		QPoint pp(15,35);
		int ypos;
		
		int iHeight;
		if (width() == 3) {
			part = m_oBasicImageCache.getImage(BasicImageCache::BODY_TOP_HUGE);
			iHeight = part->height();
			part = m_oBasicImageCache.getImage(BasicImageCache::BODY_MID_HUGE);
			iHeight += part->height() * getSize();
			part = m_oBasicImageCache.getImage(BasicImageCache::BODY_BOT_HUGE);
			iHeight += part->height();
		} else {
			part = m_oBasicImageCache.getImage(BasicImageCache::BODY_TOP_LARGE);
			iHeight = part->height();
			part = m_oBasicImageCache.getImage(BasicImageCache::BODY_MID_LARGE);
			iHeight += part->height() * getSize();
			part = m_oBasicImageCache.getImage(BasicImageCache::BODY_BOT_LARGE);
			iHeight += part->height();
		}
		Image *poImage = new Image(part->width(), iHeight);

		QPainter p;
		p.begin(poImage);

		if (width() == 3) {
			// top of the device
			part = m_oBasicImageCache.getImage(BasicImageCache::BODY_TOP_HUGE);
			p.drawPixmap(pt, *part);
			pt.setY(pt.y() + part->height());

			// middle-part of device
			part = m_oBasicImageCache.getImage(BasicImageCache::BODY_MID_HUGE);
			for(int i = 0; i < getSize(); i++) {
				p.drawPixmap(pt, *part);
				pt.setY(pt.y() + part->height());
			}

			// bottom of device
			p.drawPixmap(pt, *(m_oBasicImageCache.getImage(BasicImageCache::BODY_BOT_HUGE)));
		} else {
			// top of the device
			part = m_oBasicImageCache.getImage(BasicImageCache::BODY_TOP_LARGE);
			p.drawPixmap(pt, *part);
			pt.setY(pt.y() + part->height());

			// middle-part of device
			part = m_oBasicImageCache.getImage(BasicImageCache::BODY_MID_LARGE);
			for(int i = 0; i < getSize(); i++) {
				p.drawPixmap(pt, *part);
				pt.setY(pt.y() + part->height());
			}

			// bottom of device
			p.drawPixmap(pt, *(m_oBasicImageCache.getImage(BasicImageCache::BODY_BOT_LARGE)));
		}

		// draw input- and output names
		int xpos;
		p.setFont(KlogicGlobal::Device::getFont());
		xpos = XObject::IOREG + 3;
		for (ConnectorList::iterator iter = m_oNamedInput.begin(); iter != m_oNamedInput.end(); iter++) {
			const Connector& oConnector = *iter;
			ypos = oConnector.getPosition() * Grid::GRID;
			if (oConnector.getPosition() == 0)
				p.drawText(xpos, ypos + 8, oConnector.getName());
			else if (ypos == (getSize() * Grid::GRID))
				p.drawText(xpos, ypos + 6, oConnector.getName());
			else
				p.drawText(xpos, ypos + 7, oConnector.getName());
		}
		xpos = poImage->width() / 2;
		int xwidth = xpos - 6;
		ConnectorList::iterator iter = m_oNamedOutput.begin();
		iter++;
		while(iter != m_oNamedOutput.end()) {
			const Connector& oConnector = *iter;
			if (!((*oConnector)->isInternal())) {
				ypos = oConnector.getPosition() * Grid::GRID;
				if (ypos == 0)
					p.drawText(xpos, ypos - 5, xwidth, 20,
							Qt::AlignRight, oConnector.getName());
				else if (ypos == (getSize() * Grid::GRID))
					p.drawText(xpos, ypos - 5 , xwidth, 20,
							Qt::AlignRight, oConnector.getName());
				else
					p.drawText(xpos, ypos - 6, xwidth, 20,
							Qt::AlignRight, oConnector.getName());
			}
			iter++;
		}
		p.end();
		
		// set image dimension, clear position
		XObject::setImage(poImage);
		poImage->relRef();

		if (m_poTextDev) m_poTextDev->setImage();
	}

	virtual void printImage(QPainter *, const QRect &)
	{
	}
private:
	int m_iWidth;
};

class TristateDevice : public CustomDevice
{
public:
	TristateDevice(const QPoint& oPos, int iSize = -1)
		: CustomDevice(DeviceType::fTRI, oPos, iSize)
	{
	}

	virtual ~TristateDevice() {}

	virtual void init()
	{
		enableTristateInput(true);
		m_poOutputBus = new BusOutputArray(this, true, true);
		deviceOutputInverted = false;
		maxInputCount = 9;
		maxOutputCount = 8;
		setSize(neededSize());
		CustomDevice::init();
	}

	virtual void setEquation()
	{
		Device::setEquation("D0", (*m_poOutputBus)[0]);
		Device::setEquation("D1", (*m_poOutputBus)[1]);
		Device::setEquation("D2", (*m_poOutputBus)[2]);
		Device::setEquation("D3", (*m_poOutputBus)[3]);
		Device::setEquation("D4", (*m_poOutputBus)[4]);
		Device::setEquation("D5", (*m_poOutputBus)[5]);
		Device::setEquation("D6", (*m_poOutputBus)[6]);
		Device::setEquation("D7", (*m_poOutputBus)[7]);
	}
private:
	BusOutputArray *m_poOutputBus;
};

class EquationDevice : public CustomDevice
{
public:
	EquationDevice(const QPoint& oPos, int iSize = -1)
		: CustomDevice(DeviceType::fEQU, oPos, iSize)
	{
	}

	virtual ~EquationDevice() {}

	virtual void init()
	{
		CustomDevice::init();
	}
};

#endif
