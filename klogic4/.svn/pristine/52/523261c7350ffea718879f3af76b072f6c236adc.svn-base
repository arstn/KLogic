// ------------------------------------------------
// JK flipflop device implementation
//
// Andreas Rostin
// 31.07.2003
// ------------------------------------------------
#include "deviceTFF.h"

const QString TFFDevice::INPUT_FF_T = "T";
const QString TFFDevice::INTERNAL_FF_MASTER_Q = "MQ";

TFFDevice::TFFDevice(const QPoint& oPos, int iSize)
	: XDevice(DeviceType::fTFF, oPos, iSize)
{
}

TFFDevice::~TFFDevice()
{
}

// initialize device
void TFFDevice::init()
{
	deviceOutputInverted = false;
	addInputName(INPUT_FF_T, 1);
	m_iMasterQID = addInternalName(INTERNAL_FF_MASTER_Q);

	XDevice::init();
}

void TFFDevice::setEquation()
{
	// forward
	Device::setEquation("/T & MQ + T & /_this_", m_iMasterQID);
	Device::setEquation("T & _this_ + /T & MQ");
	// backward
	//Device::setEquation("T & MQ + /T & /_this_", m_iMasterQID);
	//Device::setEquation("/T & _this_ + T & MQ");
}

bool TFFDevice::hasNamedInput() const
{
	return true;
}

bool TFFDevice::sizeChangeable() const
{
	return false;
}

Image * TFFDevice::createImage(const ImageKey&)
{
	const Image *part = m_oBasicImageCache.getImage(BasicImageCache::BODY_TFF);
	Image *poImage = new Image(part->width(), part->height());

	QPainter p;
	p.begin(poImage);
	p.drawPixmap(0, 0, *part);
	p.end();
	return poImage;
}

void TFFDevice::printImage(QPainter *p, const QRect &r)
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

bool TFFDevice::outputIsActive(int)
{
	return true;
}

