// ------------------------------------------------
// D flipflop device implementation
//
// Andreas Rostin
// 21.12.2003
// ------------------------------------------------
#include "deviceDFF.h"

const QString DFFDevice::INPUT_FF_1D = "1D";
const QString DFFDevice::INPUT_FF_C1 = "C1";
const QString DFFDevice::INTERNAL_FF_MASTER_Q = "MQ";
const QString DFFDevice::INTERNAL_C1_PREVIOUS = "C1-1";

DFFDevice::DFFDevice(const QPoint& oPos, int iSize)
	: XDevice(DeviceType::fDFF, oPos, iSize)
{
	m_iClock = KlogicGlobal::Device::CLK_NONE;
	m_bMaster = true;
}

DFFDevice::~DFFDevice()
{
}

// initialize device
void DFFDevice::init()
{
	deviceOutputInverted = false;
	addInputName(INPUT_FF_1D, 0);
	addInputName(INPUT_FF_C1, 3);
	m_iMasterQID = addInternalName(INTERNAL_FF_MASTER_Q);
	m_iC1Delayed = addInternalName(INTERNAL_C1_PREVIOUS);

	setClock(KlogicGlobal::Device::getClock());
	setMaster(true);

	XDevice::init();
}

void DFFDevice::setEquation()
{
	if (m_bMaster) {
		switch(m_iClock) {
		case KlogicGlobal::Device::CLK_RISING_1EDGE:
			// equations for internal outputs
			Device::setEquation("/(C1 & /C1-1) & MQ + C1 & /C1-1 & 1D", m_iMasterQID);
			Device::setEquation("C1", m_iC1Delayed);
			// equation for unnamed output
			Device::setEquation("/(/C1 & C1-1) & _this_ + /C1 & C1-1 & MQ");
			break;
		case KlogicGlobal::Device::CLK_FALLING_1EDGE:
			Device::setEquation("/(/C1 & C1-1) & MQ + /C1 & C1-1 & 1D", m_iMasterQID);
			Device::setEquation("C1", m_iC1Delayed);
			Device::setEquation("/(C1 & /C1-1) & _this_ + C1 & /C1-1 & MQ");
			break;
		case KlogicGlobal::Device::CLK_HIGH_VALUE:
			Device::setEquation("/C1 & MQ + C1 & 1D", m_iMasterQID);
			Device::setEquation((char *)NULL, m_iC1Delayed);
			Device::setEquation("C1 & _this_ + /C1 & MQ");
			break;
		case KlogicGlobal::Device::CLK_LOW_VALUE:
			Device::setEquation("C1 & MQ + /C1 & 1D", m_iMasterQID);
			Device::setEquation((char *)NULL, m_iC1Delayed);
			Device::setEquation("/C1 & _this_ + C1 & MQ");
			break;
		default:
			break;
		}
	} else {
		switch(m_iClock) {
		case KlogicGlobal::Device::CLK_HIGH_VALUE:
			Device::setEquation(0, m_iMasterQID);
			Device::setEquation(0, m_iC1Delayed);
			Device::setEquation("/C1 & _this_ + C1 & 1D");
			break;
		case KlogicGlobal::Device::CLK_LOW_VALUE:
			Device::setEquation(0, m_iMasterQID);
			Device::setEquation(0, m_iC1Delayed);
			Device::setEquation("C1 & _this_ + /C1 & 1D");
			break;
		default:
			break;
		}
	}
}

void DFFDevice::setClock(KlogicGlobal::Device::CLOCKTYPE iClock)
{
        // enum movements in past seem to cause crases
        if (iClock == KlogicGlobal::Device::CLK_FALLING_2EDGE) {
                iClock = KlogicGlobal::Device::CLK_FALLING_1EDGE;
        }
        if (iClock == KlogicGlobal::Device::CLK_RISING_2EDGE) {
                iClock = KlogicGlobal::Device::CLK_RISING_1EDGE;
        }

	m_iClock = iClock;

	m_oImageKey.setClock(m_iClock);
	setEquation();
	parseEquation();
	setImage();
}

KlogicGlobal::Device::CLOCKTYPE DFFDevice::clock() const
{
	return m_iClock;
}

bool DFFDevice::hasClock() const
{
	return true;
}

bool DFFDevice::hasMaster() const
{
	return m_bMaster;
}

// enable/disable master of a master-slave flipflop
void DFFDevice::setMaster(bool bMaster)
{
	m_bMaster = bMaster;

	m_oImageKey.setMaster(bMaster);
	setEquation();
	parseEquation();
	setImage();
}

bool DFFDevice::hasNamedInput() const
{
	return true;
}

bool DFFDevice::sizeChangeable() const
{
	return false;
}

Image * DFFDevice::createImage(const ImageKey& oKey)
{
	const Image *part = 0;
	if (oKey.getMaster())
		part = m_oBasicImageCache.getImage(BasicImageCache::BODY_DM);
	else
		part = m_oBasicImageCache.getImage(BasicImageCache::BODY_D);

	Image *poImage = new Image(part->width(), part->height());
	QPainter p;
	p.begin(poImage);
	p.drawPixmap(0, 0, *part);
	p.end();
	return poImage;
}

void DFFDevice::printImage(QPainter *p, const QRect &r)
{
	int picwidth = getImage()->width();

	p->setFont(KlogicGlobal::Device::getFont());
        for (ConnectorList::iterator iter = m_oNamedInput.begin(); iter != m_oNamedInput.end(); iter++) {
                int iPos = (*iter).getPosition() * Grid::GRID;
		if ((*iter).getName() == "C1" && hasMaster()) {
			int a = r.x();
			int b = r.x() + 6;
			int c = r.y() + iPos + 2;
			p->drawLine(a, c - 3, b, c);
			p->drawLine(a, c + 3, b, c);
			p->drawText(r.x() + 6, r.y() + iPos + 5, (*iter).getName());
		}
	}
	p->drawText(r.x() + (picwidth / 2), r.y() - 2, picwidth / 2 - 7, 10, Qt::AlignRight, "Q");
}

bool DFFDevice::outputIsActive(int)
{
	return true;
}

