// ------------------------------------------------
// D flipflop device implementation
//
// Andreas Rostin
// 21.12.2003
// ------------------------------------------------

#ifndef DEVICE_DFF_CLASS_H
#define DEVICE_DFF_CLASS_H

#include <qpainter.h>
#include "xdevice.h"
#include "grid.h"

class DFFDevice : public XDevice
{
public:
	DFFDevice(const QPoint& oPos, int size = -1);
	virtual ~DFFDevice();

	virtual void init();
	virtual void setEquation();
	virtual void setClock(KlogicGlobal::Device::CLOCKTYPE);
	virtual KlogicGlobal::Device::CLOCKTYPE clock() const;
	virtual bool hasClock() const;
	virtual void setMaster(bool);
	virtual bool hasMaster() const;
	virtual bool hasNamedInput() const;
	virtual bool sizeChangeable() const;
	virtual Image * createImage(const ImageKey& oKey);
	virtual void printImage(QPainter *p, const QRect &r);
	virtual bool outputIsActive(int);

private:
	static const QString INPUT_FF_1D;
	static const QString INPUT_FF_C1;
	static const QString INTERNAL_FF_MASTER_Q;
	static const QString INTERNAL_C1_PREVIOUS;

	int m_iMasterQID;
	int m_iC1Delayed;

	KlogicGlobal::Device::CLOCKTYPE m_iClock;
	bool m_bMaster;
};

#endif
