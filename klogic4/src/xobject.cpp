//*************************************************************************
//* methods for class XObject                                             *
//*                                                                       *
//* pixmap management for objects on the screen                           *
//*                                                                       *
//* Andreas Rostin                                                        *
//* 15.03.99                                                              *
//*************************************************************************
#include <qpaintdevice.h>
#include <qpainter.h>
#include <qpixmap.h>

#include "xobject.h"
#include "grid.h"
#include "KlogicGlobal.h"

//=====================================================================
//== constructor/destructor/initializing                             ==
//=====================================================================
XObject::XObject(const QPoint& oPosition)
{
	m_poImage = 0;

	m_ptOffset = QPoint( 0, VOFFSET );
	m_bIsSelected = false;
	m_bHasInputRegion = true;
	m_bHasOutputRegion = true;

	// creating the grid as singleton
	Grid();

	initPos(oPosition);
}

XObject::~XObject()
{
	if (m_poImage) {
		m_poImage->relRef();
		m_poImage = 0;
	}
}

// set initial position on the screen
void XObject::initPos(const QPoint& oMoveToPos)
{
	m_oPosition.moveTopLeft(Grid::getAligned(oMoveToPos) - m_ptOffset);
	m_oOldPosition = m_oPosition;
	m_oMouseStartPos = QPoint();
}

//=====================================================================
//== object movement                                                 ==
//=====================================================================
QPoint XObject::getPos() const
{
	return m_oPosition.topLeft();
}

QPoint XObject::getOldPos() const
{
	return m_oOldPosition.topLeft();
}

const QRect& XObject::getGeometry() const
{
	return m_oPosition;
}

// relative move
void XObject::moveBy(const QPoint& oDelta)
{
	m_oPosition.translate(oDelta.x(), oDelta.y());
	moveIORegion(oDelta);
}

// absolute move - returns true if moved
bool XObject::moveTo(const QPoint& oMousePos)
{
	QPoint oMouseToPos = oMousePos - m_oMouseStartPos;
	QPoint oMoveToPos = Grid::getAligned(oMouseToPos) - m_ptOffset;
	if (m_oPosition.topLeft() != oMoveToPos)
	{
		// prevents hitting the screen boundaries
		if( oMoveToPos.x() + m_ptOffset.x() < Grid::GRID ) {
			oMoveToPos.setX( Grid::GRID + m_ptOffset.x() );
			oMoveToPos = Grid::getAligned(oMoveToPos) - m_ptOffset;
		}
		if( oMoveToPos.x() - m_ptOffset.x() + m_oPosition.width() > KlogicGlobal::Screen::VIRT_SCREEN_SIZE_X ) {
			oMoveToPos.setX( KlogicGlobal::Screen::VIRT_SCREEN_SIZE_X - m_oPosition.width() - m_ptOffset.x() );
			oMoveToPos = Grid::getAligned(oMoveToPos) - m_ptOffset;
		}

		if( oMoveToPos.y() + m_ptOffset.y() < Grid::GRID ) {
			oMoveToPos.setY( Grid::GRID + m_ptOffset.y() );
			oMoveToPos = Grid::getAligned(oMoveToPos) - m_ptOffset;
		}
		if( oMoveToPos.y() - m_ptOffset.y() + m_oPosition.height() > KlogicGlobal::Screen::VIRT_SCREEN_SIZE_Y ) {
			oMoveToPos.setY( KlogicGlobal::Screen::VIRT_SCREEN_SIZE_Y - m_oPosition.height());
			oMoveToPos = Grid::getAligned(oMoveToPos) - m_ptOffset;
		}

		// prevent moving mouse and this in opposite directions
		QPoint oMouseDelta = oMouseToPos - m_oPosition.topLeft();// - m_ptOffset;
		QPoint oMoveDelta = oMoveToPos - m_oPosition.topLeft();
		bool bXMov = ((oMoveDelta.x() >= 0 && oMouseDelta.x() >= 0) ||
			      (oMoveDelta.x() <= 0 && oMouseDelta.x() <= 0));
		bool bYMov = ((oMoveDelta.y() >= 0 && oMouseDelta.y() >= 0) ||
			      (oMoveDelta.y() <= 0 && oMouseDelta.y() <= 0));

		if (bXMov && bYMov) {
			m_oPosition.moveTopLeft(oMoveToPos);
			moveIORegion(oMoveDelta);
			return true;
		} else if (bXMov) {
			oMoveToPos.setY(m_oPosition.y());
			oMoveDelta.setY(0);
			m_oPosition.moveTopLeft(oMoveToPos);
			moveIORegion(oMoveDelta);
			return true;
		} else if (bYMov) {
			oMoveToPos.setX(m_oPosition.x());
			oMoveDelta.setX(0);
			m_oPosition.moveTopLeft(oMoveToPos);
			moveIORegion(oMoveDelta);
			return true;
		}
	}
	return false;
}

//=====================================================================
//== undo movement                                                   ==
//=====================================================================
void XObject::setOrigin()
{
	m_oOriginPosition = m_oPosition.topLeft();
}

void XObject::toOrigin()
{
	m_oPosition.moveTopLeft( m_oOriginPosition );
	setIORegion();
}

bool XObject::isOrigin(const QPoint& pt ) const
{
	QRect r(m_oOriginPosition, m_oPosition.size());
	return r.contains(pt);
}

//=====================================================================
//== vertical/horizontal image offset                                ==
//=====================================================================
//-- get horizontal offset of image
int XObject::getHOffset() const
{
	return m_ptOffset.x();
}

//-- get vertical offset of image
int XObject::getVOffset() const
{
	return m_ptOffset.y();
}

//-- set horizontal offset of image
void XObject::setHOffset(int nHOffset)
{
	setOffset(QPoint(nHOffset, m_ptOffset.y()));
}

//-- set vertical offset of image
void XObject::setVOffset(int nVOffset)
{
	setOffset(QPoint(m_ptOffset.x(), nVOffset));
}

//-- adjust positioning variables if offset changes
void XObject::setOffset(const QPoint& ptNewOffset)
{
	QPoint ptDelta = m_ptOffset - ptNewOffset;

	m_oPosition.translate( ptDelta.x(), ptDelta.y() );
	m_oOldPosition.translate( ptDelta.x(), ptDelta.y() );
	setIORegion();

	m_ptOffset = ptNewOffset;
}

// set region where connections to input/output are possible
void XObject::setIORegion()
{
	if (!m_poImage)
		return;

	m_oInputRegion.setX(m_oPosition.x() - 1);
	m_oInputRegion.setY(m_oPosition.y());
	if (m_bHasInputRegion) {
		m_oInputRegion.setWidth(XObject::IOREG + 2);
		m_oInputRegion.setHeight(m_poImage->height());
	} else {
		m_oInputRegion.setWidth(0);
		m_oInputRegion.setHeight(0);
	}

	m_oOutputRegion.setX(m_oPosition.x() + m_poImage->width() - XObject::IOREG - 1);
	m_oOutputRegion.setY(m_oPosition.y());
	if (m_bHasOutputRegion) {
		m_oOutputRegion.setWidth(XObject::IOREG + 2);
		m_oOutputRegion.setHeight(m_poImage->height());
	} else {
		m_oOutputRegion.setWidth(0);
		m_oOutputRegion.setHeight(0);
	}

	m_oDeviceRegion.setX(m_oInputRegion.x() + m_oInputRegion.width());
	m_oDeviceRegion.setY(m_oPosition.y());
	m_oDeviceRegion.setWidth(m_poImage->width() - m_oOutputRegion.width() - m_oInputRegion.width());
	m_oDeviceRegion.setHeight(m_poImage->height());
}

// move region where connections to input/output are possible
void XObject::moveIORegion(const QPoint& oDelta)
{
	if (!m_poImage) return;
	m_oInputRegion.translate(oDelta.x(), oDelta.y());
	m_oOutputRegion.translate(oDelta.x(), oDelta.y());
	m_oDeviceRegion.translate(oDelta.x(), oDelta.y());
}

//=====================================================================
//== pixmap methods                                                  ==
//=====================================================================
void XObject::draw( QPaintDevice*, QPainter* p )
{
	if (!m_poImage)
		return;

	p->drawPixmap( m_oPosition, *m_poImage);

	if( m_bIsSelected )
	{
		p->setPen( Qt::green );
		p->setBrush( Qt::NoBrush);
		p->drawRect( m_oPosition );
	}
}

void XObject::setImage(const Image *poImage)
{
	if (m_poImage) {
		m_poImage->relRef();
	}

	m_poImage = poImage;

	if (m_poImage) {
		m_poImage->addRef();
		const QSize& oImageSize = m_poImage->rect().size();
		m_oPosition.setSize(oImageSize);
		m_oOldPosition.setSize(oImageSize);
		setIORegion();
	}
}

const Image * XObject::getImage()
{
	return m_poImage;
}

//=====================================================================
//== selection methods                                               ==
//=====================================================================
void XObject::select(bool bIsSelected)
{
	m_bIsSelected = bIsSelected;
}

bool XObject::isSelected() const
{
	return m_bIsSelected;
}

bool XObject::contains(const QRect& pos) const
{
	return m_oPosition.intersects( pos );
}

bool XObject::contains(const QPoint& oMousePos)
{
	if (m_oDeviceRegion.contains(oMousePos)) {
		m_oMouseStartPos = oMousePos - m_oPosition.topLeft();
		return true;
	}
	return false;
}

