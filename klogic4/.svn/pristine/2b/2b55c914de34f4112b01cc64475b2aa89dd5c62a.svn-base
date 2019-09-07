// ------------------------------------------------
// T flipflop device implementation
//
// Andreas Rostin
// 21.02.2003
// ------------------------------------------------

#ifndef TFFDevice_CLASS_H
#define TFFDevice_CLASS_H

#include <qpainter.h>
#include "xdevice.h"
#include "grid.h"

class TFFDevice : public XDevice
{
public:
	TFFDevice(const QPoint& oPos, int iSize = -1);
	virtual ~TFFDevice();

	virtual void init();
	virtual void setEquation();
	virtual bool hasNamedInput() const;
	virtual bool sizeChangeable() const;
	virtual Image * createImage(const ImageKey& oKey);
	virtual void printImage(QPainter *p, const QRect &r);
	virtual bool outputIsActive(int);

private:
	static const QString INPUT_FF_T;
	static const QString INTERNAL_FF_MASTER_Q;

	int m_iMasterQID;
};

#endif
