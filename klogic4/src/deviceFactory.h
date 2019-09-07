#ifndef KLOGIC_DEV_FACTORY
#define KLOGIC_DEV_FACTORY

#include <qpoint.h>

class XDevice;

class DeviceFactory
{
public:
	static DeviceFactory * getInstance() {
		if (!m_pInstance)
			m_pInstance = new DeviceFactory();
		return m_pInstance;
	}

	XDevice *createDevice(XDevice *, int iFunction, const QPoint& oPos, int iSize = -1);

private:
	DeviceFactory() {}

	static DeviceFactory *m_pInstance;
};

#endif
