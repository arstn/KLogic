// ------------------------------------------------
// JK flipflop device implementation
//
// Andreas Rostin
// 21.02.2003
// ------------------------------------------------

#ifndef __DEVICE_JKFF__
#define __DEVICE_JKFF__

#include <qpainter.h>
#include "xdevice.h"
#include "grid.h"

class JKFFDevice : public XDevice
{
public:
	JKFFDevice(const QPoint& oPos, int iSize = -1);
	virtual ~JKFFDevice();

	virtual void init();
	int defaultInput(int, bool&);
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
	static const QString INPUT_FF_1J;
	static const QString INPUT_FF_S;
	static const QString INPUT_FF_C1;
	static const QString INPUT_FF_R;
	static const QString INPUT_FF_1K;
	static const QString INTERNAL_FF_MASTER_Q;
	static const QString INTERNAL_C1_PREVIOUS;

	int m_iMasterQID;
	int m_iC1Delayed;

	KlogicGlobal::Device::CLOCKTYPE m_iClock;
	bool m_bMaster;
};

#endif
