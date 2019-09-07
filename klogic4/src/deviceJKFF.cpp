// ------------------------------------------------
// JK flipflop device implementation
//
// Andreas Rostin
// 31.07.2003
// ------------------------------------------------
#include "deviceJKFF.h"

const QString JKFFDevice::INPUT_FF_S = "S";
const QString JKFFDevice::INPUT_FF_R = "R";
const QString JKFFDevice::INPUT_FF_C1 = "C1";
const QString JKFFDevice::INPUT_FF_1J = "1J";
const QString JKFFDevice::INPUT_FF_1K = "1K";
const QString JKFFDevice::INTERNAL_FF_MASTER_Q = "MQ";
const QString JKFFDevice::INTERNAL_C1_PREVIOUS = "C1-1";

JKFFDevice::JKFFDevice(const QPoint& oPos, int iSize)
	: XDevice(DeviceType::fJK, oPos, iSize)
{
	m_iClock = KlogicGlobal::Device::CLK_NONE;
	m_bMaster = true;
}

JKFFDevice::~JKFFDevice()
{
}

// initialize device
void JKFFDevice::init()
{
	deviceOutputInverted = false;
	addInputName(INPUT_FF_1J, 0);
	addInputName(INPUT_FF_S, 1);
	addInputName(INPUT_FF_C1, 2);
	addInputName(INPUT_FF_R, 3);
	addInputName(INPUT_FF_1K, 4);
	m_iMasterQID = addInternalName(INTERNAL_FF_MASTER_Q);
	m_iC1Delayed = addInternalName(INTERNAL_C1_PREVIOUS);

	setClock(KlogicGlobal::Device::getClock());
	setMaster(true);

	XDevice::init();
}

/*
 * Due to the fact that R and S do usually remain unused, they are pulled to low value per default.
 */
int JKFFDevice::defaultInput(int input_id, bool&bIsBoolean) {
	return 0;
}

void JKFFDevice::setEquation()
{
	if (!hasMaster()) {
		qFatal("Device::setEquation: JK without master??");
		exit(-1);
	}

	// 17.02.2001: asynchroneous RS inputs added
	// 21.02.2003: single edge usual JK added
	switch(m_iClock) {
		case KlogicGlobal::Device::CLK_RISING_2EDGE:
			Device::setEquation("(/(C1 & /C1-1) & MQ + /1J & /1K & MQ + C1 & /C1-1 & 1J & /1K + C1 & /C1-1 & 1J & 1K & /MQ) & /R + S", m_iMasterQID);
			Device::setEquation("C1", m_iC1Delayed);
			Device::setEquation("(/(/C1 & C1-1) & _this_ + /C1 & C1-1 & MQ) & /R + S");
			break;
		case KlogicGlobal::Device::CLK_FALLING_2EDGE:
			Device::setEquation("(/(/C1 & C1-1) & MQ + /1J & /1K & MQ + /C1 & C1-1 & 1J & /1K + /C1 & C1-1 & 1J & 1K & /MQ) & /R + S", m_iMasterQID);
			Device::setEquation("C1", m_iC1Delayed);
			Device::setEquation("(/(C1 & /C1-1) & _this_ + C1 & /C1-1 & MQ) & /R + S");
			break;
		case KlogicGlobal::Device::CLK_RISING_1EDGE:
			Device::setEquation("C1 & MQ + (1J & /_this_ + /1K & _this_) & /C1", m_iMasterQID);
			Device::setEquation("(/C1 & _this_ + C1 & MQ) & /R + S");
			break;
		case KlogicGlobal::Device::CLK_FALLING_1EDGE:
			Device::setEquation("/C1 & MQ + (1J & /_this_ + /1K & _this_) & C1", m_iMasterQID);
			Device::setEquation("(C1 & _this_ + /C1 & MQ) & /R + S");
			break;
		case KlogicGlobal::Device::CLK_HIGH_VALUE:
			Device::setEquation("(/C1 & MQ + /1J & /1K & MQ + C1 & 1J & /1K + C1 & 1J & 1K & /MQ) & /R + S", m_iMasterQID);
			Device::setEquation((char *)NULL, m_iC1Delayed);
			Device::setEquation("(C1 & _this_ + /C1 & MQ) & /R + S");
			break;
		case KlogicGlobal::Device::CLK_LOW_VALUE:
			Device::setEquation("(C1 & MQ + /1J & /1K & MQ + /C1 & 1J & /1K + /C1 & 1J & 1K & /MQ) & /R + S", m_iMasterQID);
			Device::setEquation((char *)NULL, m_iC1Delayed);
			Device::setEquation("(/C1 & _this_ + C1 & MQ) & /R + S");
			break;
		default:
			break;
	}
}

void JKFFDevice::setClock(KlogicGlobal::Device::CLOCKTYPE iClock)
{
	m_iClock = iClock;

	m_oImageKey.setClock(m_iClock);
        setEquation();
        parseEquation();
        setImage();
}

KlogicGlobal::Device::CLOCKTYPE JKFFDevice::clock() const
{
	return m_iClock;
}

bool JKFFDevice::hasClock() const
{
	return true;
}

bool JKFFDevice::hasMaster() const
{
        return m_bMaster;
}

// enable/disable master of a master-slave flipflop
void JKFFDevice::setMaster(bool bMaster)
{
        m_bMaster = bMaster;

	m_oImageKey.setMaster(bMaster);
        setEquation();
        parseEquation();
	setImage();
}

bool JKFFDevice::hasNamedInput() const
{
	return true;
}

bool JKFFDevice::sizeChangeable() const
{
	return false;
}

Image * JKFFDevice::createImage(const ImageKey& oKey)
{
	KlogicGlobal::Device::CLOCKTYPE iClk = oKey.getClock();

	const Image *part = 0;
	if (iClk == KlogicGlobal::Device::CLK_RISING_1EDGE ||
	    iClk == KlogicGlobal::Device::CLK_FALLING_1EDGE ||
	    iClk == KlogicGlobal::Device::CLK_FALLING_2EDGE ||
	    iClk == KlogicGlobal::Device::CLK_RISING_2EDGE)
		part = m_oBasicImageCache.getImage(BasicImageCache::BODY_JKF);
	else
		part = m_oBasicImageCache.getImage(BasicImageCache::BODY_JK);

	Image *poImage = new Image(part->width(), part->height());
	QPainter p;
	p.begin(poImage);
	p.drawPixmap(0, 0, *part);
	p.drawLine(poImage->width() - 9, 3, poImage->width() - 9, 7);
	switch (iClk) {
		case KlogicGlobal::Device::CLK_FALLING_1EDGE:
		case KlogicGlobal::Device::CLK_FALLING_2EDGE:
		case KlogicGlobal::Device::CLK_LOW_VALUE:
			p.drawLine(poImage->width() - 15, 3, poImage->width() - 9, 3);
			break;
		case KlogicGlobal::Device::CLK_RISING_1EDGE:
		case KlogicGlobal::Device::CLK_RISING_2EDGE:
		case KlogicGlobal::Device::CLK_HIGH_VALUE:
			p.drawLine(poImage->width() - 15, 7, poImage->width() - 9, 7);
			break;
		default:
			break;
	}
	p.end();
	return poImage;
}

void JKFFDevice::printImage(QPainter *p, const QRect &r)
{
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
	int picwidth = getImage()->width();
	p->drawText(r.x() + (picwidth / 2), r.y() - 2, picwidth / 2 - 7, 10, Qt::AlignRight, "Q");

	for (ConnectorList::iterator iter = m_oNamedOutput.begin(); iter != m_oNamedOutput.end(); iter++) {
		int iPos = (*iter).getPosition() * Grid::GRID;
		if (iPos == 0)
			p->drawText(r.x() + (picwidth / 2), r.y() + iPos - 2,
					picwidth / 2 - 7, 10, Qt::AlignRight, (*iter).getName());
		else if (iPos == (getSize() * Grid::GRID))
			p->drawText(r.x() + (picwidth / 2), r.y() + iPos - 4,
					picwidth / 2 - 7, 10, Qt::AlignRight, (*iter).getName());
		else
			p->drawText(r.x() + (picwidth / 2), r.y() + iPos - 2,
					picwidth / 2 - 7, 10, Qt::AlignRight, (*iter).getName());
	}
}

bool JKFFDevice::outputIsActive(int)
{
	return true;
}

