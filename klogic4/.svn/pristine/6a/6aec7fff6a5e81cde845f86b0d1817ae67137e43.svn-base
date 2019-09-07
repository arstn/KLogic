// ------------------------------------------------
// RS flipflop device implementation
//
// Andreas Rostin
// 21.12.2003
// ------------------------------------------------
#include "deviceRSFF.h"

const QString RSFFDevice::INPUT_FF_S = "S";
const QString RSFFDevice::INPUT_FF_R = "R";
const QString RSFFDevice::INPUT_FF_1S = "1S";
const QString RSFFDevice::INPUT_FF_C1 = "C1";
const QString RSFFDevice::INPUT_FF_1R = "1R";
const QString RSFFDevice::INTERNAL_FF_MASTER_Q = "MQ";
const QString RSFFDevice::INTERNAL_C1_PREVIOUS = "C1-1";

RSFFDevice::RSFFDevice(const QPoint& oPos, int iSize)
	: XDevice(DeviceType::fRS, oPos, iSize)
{
	m_iClock = KlogicGlobal::Device::CLK_NONE;
	m_bMaster = true;
}

RSFFDevice::~RSFFDevice()
{
}

// initialize device
void RSFFDevice::init()
{
	deviceOutputInverted = false;
	if (KlogicGlobal::Device::getClock() == KlogicGlobal::Device::CLK_NONE) {
		addInputName(INPUT_FF_S, 0);
		addInputName(INPUT_FF_R, 3);
	} else {
		addInputName(INPUT_FF_1S, 0);
		addInputName(INPUT_FF_C1, 2);
		addInputName(INPUT_FF_1R, 4);
	}
	m_iMasterQID = addInternalName(INTERNAL_FF_MASTER_Q);
	m_iC1Delayed = addInternalName(INTERNAL_C1_PREVIOUS);

	setClock(KlogicGlobal::Device::getClock());
	setMaster(true);

	XDevice::init();
}

void RSFFDevice::setEquation()
{
	if (m_bMaster) {
		switch(m_iClock) {
			case KlogicGlobal::Device::CLK_RISING_1EDGE:
				Device::setEquation("/(C1 & /C1-1) & MQ + /1S & /1R & MQ + C1 & /C1-1 & 1S", m_iMasterQID);
				Device::setEquation("C1", m_iC1Delayed);
				Device::setEquation("/(/C1 & C1-1) & _this_ + /C1 & C1-1 & MQ");
				break;
			case KlogicGlobal::Device::CLK_FALLING_1EDGE:
				Device::setEquation("/(/C1 & C1-1) & MQ + /1S & /1R & MQ + /C1 & C1-1 & 1S", m_iMasterQID);
				Device::setEquation("C1", m_iC1Delayed);
				Device::setEquation("/(C1 & /C1-1) & _this_ + C1 & /C1-1 & MQ");
				break;
			case KlogicGlobal::Device::CLK_HIGH_VALUE:
				Device::setEquation("/C1 & MQ + /1S & /1R & MQ + C1 & 1S", m_iMasterQID);
				Device::setEquation((char *)NULL, m_iC1Delayed);
				Device::setEquation("C1 & _this_ + /C1 & MQ");
				break;
			case KlogicGlobal::Device::CLK_LOW_VALUE:
				Device::setEquation("C1 & MQ + /1S & /1R & MQ + /C1 & 1S", m_iMasterQID);
				Device::setEquation((char *)NULL, m_iC1Delayed);
				Device::setEquation("/C1 & _this_ + C1 & MQ");
				break;
			default:
				break;
		}
	} else {
		switch(m_iClock) {
			case KlogicGlobal::Device::CLK_NONE:
				Device::setEquation((char *)NULL, m_iMasterQID);
				Device::setEquation((char *)NULL, m_iC1Delayed);
				Device::setEquation("/S & /R & _this_ + S");
				break;
			case KlogicGlobal::Device::CLK_HIGH_VALUE:
				Device::setEquation((char *)NULL, m_iMasterQID);
				Device::setEquation((char *)NULL, m_iC1Delayed);
				Device::setEquation("/C1 & _this_ + /1S & /1R & _this_ + C1 & 1S");
				break;
			case KlogicGlobal::Device::CLK_LOW_VALUE:
				Device::setEquation((char *)NULL, m_iMasterQID);
				Device::setEquation((char *)NULL, m_iC1Delayed);
				Device::setEquation("C1 & _this_ + /1S & /1R & _this_ + /C1 & 1S");
				break;
			default:
				break;
		}
	}
}

void RSFFDevice::setClock(KlogicGlobal::Device::CLOCKTYPE iClock)
{
	if (iClock == KlogicGlobal::Device::CLK_RISING_2EDGE)
		iClock = KlogicGlobal::Device::CLK_RISING_1EDGE;
	if (iClock == KlogicGlobal::Device::CLK_FALLING_2EDGE)
		iClock = KlogicGlobal::Device::CLK_FALLING_1EDGE;

	if (m_iClock == KlogicGlobal::Device::CLK_NONE) {
		if (iClock != KlogicGlobal::Device::CLK_NONE) {
			// remove all named inputs...
			ConnectorList::iterator iter = m_oNamedInput.begin();
			while(iter != m_oNamedInput.end()) {
				removeInputName((*iter).getConnectorID());
				iter = m_oNamedInput.begin();
			}
			// ... and add the new ones
			addInputName(INPUT_FF_1S, 0);
			addInputName(INPUT_FF_C1, 2);
			addInputName(INPUT_FF_1R, 4);
		}
	} else {
		if (iClock == KlogicGlobal::Device::CLK_NONE) {
			// remove all named inputs...
			ConnectorList::iterator iter = m_oNamedInput.begin();
			while(iter != m_oNamedInput.end()) {
				removeInputName((*iter).getConnectorID());
				iter = m_oNamedInput.begin();
			}
			// ... and add the new ones
			addInputName(INPUT_FF_S, 0);
			addInputName(INPUT_FF_R, 3);
		}
	}

	m_iClock = iClock;

	m_oImageKey.setClock(m_iClock);
	setEquation();
	parseEquation();
	setImage();
}

KlogicGlobal::Device::CLOCKTYPE RSFFDevice::clock() const
{
	return m_iClock;
}

bool RSFFDevice::hasClock() const
{
	return true;
}

bool RSFFDevice::hasMaster() const
{
	return m_bMaster;
}

// enable/disable master of a master-slave flipflop
void RSFFDevice::setMaster(bool bMaster)
{
	m_bMaster = bMaster;

	m_oImageKey.setMaster(bMaster);
	setEquation();
	parseEquation();
	setImage();
}

bool RSFFDevice::hasNamedInput() const
{
	return true;
}

bool RSFFDevice::sizeChangeable() const
{
	return false;
}

Image * RSFFDevice::createImage(const ImageKey& oKey)
{
	KlogicGlobal::Device::CLOCKTYPE iClk = oKey.getClock();

	const Image *part = 0;
	if (!oKey.getMaster()) {
		if (iClk == KlogicGlobal::Device::CLK_NONE)
			part = m_oBasicImageCache.getImage(BasicImageCache::BODY_RS);
		else
			part = m_oBasicImageCache.getImage(BasicImageCache::BODY_RSC);
	} else {
		if (iClk == KlogicGlobal::Device::CLK_RISING_1EDGE || iClk == KlogicGlobal::Device::CLK_FALLING_1EDGE)
			part = m_oBasicImageCache.getImage(BasicImageCache::BODY_RSCMF);
		else
			part = m_oBasicImageCache.getImage(BasicImageCache::BODY_RSCM);
	}

	Image *poImage = new Image(part->width(), part->height());
	QPainter p;
	p.begin(poImage);
	p.drawPixmap(0, 0, *part);
	switch (iClk) {
		case KlogicGlobal::Device::CLK_FALLING_1EDGE:
		case KlogicGlobal::Device::CLK_LOW_VALUE:
			p.drawLine(poImage->width() - 15, 3, poImage->width() - 9, 3);
			p.drawLine(poImage->width() - 9, 3, poImage->width() - 9, 7);
			break;
		case KlogicGlobal::Device::CLK_RISING_1EDGE:
		case KlogicGlobal::Device::CLK_HIGH_VALUE:
			p.drawLine(poImage->width() - 15, 7, poImage->width() - 9, 7);
			p.drawLine(poImage->width() - 9, 3, poImage->width() - 9, 7);
			break;
		default:
			break;
	}
	p.end();
	return poImage;
}

void RSFFDevice::printImage(QPainter *p, const QRect &r)
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

bool RSFFDevice::outputIsActive(int)
{
	return true;
}

