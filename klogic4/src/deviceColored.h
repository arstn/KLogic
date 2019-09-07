#ifndef DEVICE_COLORED_CLASS_H
#define DEVICE_COLORED_CLASS_H

#include <qpainter.h>
#include "deviceSimple.h"
#include "xdevice.h"
#include "grid.h"

class LedDevice: public SimpleDevice
{
public:
	LedDevice(const QPoint& oPos, int iSize = -1);

	virtual void init();
	virtual bool sizeChangeable() const;
	virtual void setImageKey();
	virtual Image * createImage(const ImageKey&);
	virtual void updateImage();
	virtual bool Propagate(int);

	virtual void setColor(int iLEDColor);
	virtual int getColor();

	virtual bool isObservable();

	static const int GREEN = 0;
	static const int RED = 1;
	static const int YELLOW = 2;
	static const int BLUE = 3;

private:
	int m_iLEDColor;
};

class S7Device: public XDevice
{
public:
	S7Device(const QPoint& oPos, int iSize = -1);

	virtual void init();
	virtual bool sizeChangeable() const;
	virtual void setEquation();
	virtual bool hasNamedInput() const;
	virtual void setImageKey();
	virtual Image * createImage(const ImageKey&);
	virtual void updateImage();
	virtual bool Propagate(int);
	virtual bool isObservable();

private:
	static const QString INPUT_7S0;
	static const QString INPUT_7S1;
	static const QString INPUT_7S2;
	static const QString INPUT_7S3;
};

#endif
