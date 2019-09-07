#ifndef __XOBJECT_H
#define __XOBJECT_H

#include "imageCache.h"

//*************************************************************************
//* screen object                                                         *
//*************************************************************************
class XObject
{
public:
	~XObject();

	void moveBy(const QPoint&);
	bool moveTo(const QPoint&);

	bool contains(const QPoint&);
	bool contains(const QRect&) const;

	// undo movement
	void setOrigin();
	void toOrigin();
	bool isOrigin(const QPoint&) const;

	// vertical/horizontal image offset
	int getVOffset() const;
	int getHOffset() const;

	// pixmap methods
	const Image* getImage();

	// selection methods
	void select(bool);
	bool isSelected() const;

	QPoint getPos() const;
	const QRect& getGeometry() const;

protected:
	XObject(const QPoint&);

	void setVOffset( int );
	void setHOffset( int );

	void draw(QPaintDevice*, QPainter*);
	void setImage(const Image *);

	void hasInputRegion(bool bHasInputRegion) { m_bHasInputRegion = bHasInputRegion; }
	void hasOutputRegion(bool bHasOutputRegion) { m_bHasOutputRegion = bHasOutputRegion; }

	QPoint getOldPos() const;

	static const int IOREG = 4;	// input/output region (x-pixel)

	// connection regions
	QRect m_oInputRegion;
	QRect m_oOutputRegion;
	QRect m_oDeviceRegion;

private:
	void initPos(const QPoint &);
	void setOffset(const QPoint&);

	void setIORegion();
	void moveIORegion(const QPoint&);

	static const int VOFFSET = 3;	// default vertical aligment of an image

	QPoint m_oOriginPosition;

	// positions on the screen
	QRect m_oOldPosition;
	QRect m_oPosition;

	// IO region support
	bool m_bHasInputRegion;
	bool m_bHasOutputRegion;

	// a positive value indicating the difference between grabbed and 
	// true position, i.e. between mouse hotspot and top left of object
	QPoint m_oMouseStartPos;

	// image
	const Image *m_poImage;
	QPoint	m_ptOffset;		// image offset at screen
	bool m_bIsSelected;		// image is selected on screen?
};

#endif


