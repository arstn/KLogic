#include "deviceFactory.h"
#include "DeviceType.h"
#include "circuit.h"
#include "deviceMuxDemux.h"
#include "deviceOsc.h"
#include "deviceRAM.h"
#include "deviceJKFF.h"
#include "deviceRSFF.h"
#include "deviceDFF.h"
#include "deviceTFF.h"
#include "deviceIO.h"
#include "deviceSwitch.h"
#include "devicePwr.h"
#include "deviceText.h"
#include "deviceCustom.h"
#include "deviceColored.h"
#include "deviceInverter.h"
#include "klogic4.h"

DeviceFactory * DeviceFactory::m_pInstance = 0;

// create a new device
// if size is given, it's an import call
XDevice *DeviceFactory::createDevice(XDevice *poParent, int iFunction, const QPoint& oPos, int iSize) {
    XDevice *poDev = 0;

    // instantiate lib device
    if (iFunction >= DeviceType::fFIRST_LIBDEV && iFunction <= DeviceType::fLAST_LIBDEV) {
		if (klogic4::getInstance()->getActiveLibrary() && poParent->devIsCircuit()) {
			poDev = klogic4::getInstance()->getActiveLibrary()->createInstance(poParent->devIsCircuit(), iFunction, oPos.x(), oPos.y());
		}
		return poDev;
    }

    // instantiate new device
    switch (iFunction) {
    case DeviceType::fSUB:
        poDev = new Circuit(oPos, iSize);
        break;
    case DeviceType::fTRI:
        poDev = new TristateDevice(oPos, iSize);
        break;
    case DeviceType::fEQU:
        poDev = new EquationDevice(oPos, iSize);
        break;
    case DeviceType::fRAM:
        poDev = new RAMDevice(oPos, iSize);
        break;
    case DeviceType::fOSC:
        poDev = new OscillatorDevice(oPos, iSize);
        break;
    case DeviceType::fBUS:
        poDev = new BusDevice(oPos, iSize);
        break;
    case DeviceType::fJK:
        poDev = new JKFFDevice(oPos, iSize);
        break;
    case DeviceType::fRS:
        poDev = new RSFFDevice(oPos, iSize);
        break;
    case DeviceType::fDFF:
        poDev = new DFFDevice(oPos, iSize);
        break;
    case DeviceType::fIN:
        poDev = new InputDevice(oPos, iSize);
        break;
    case DeviceType::fOUT:
        poDev = new OutputDevice(oPos, iSize);
        break;
    case DeviceType::fPWR:
        poDev = new PowerDevice(oPos, iSize);
        break;
    case DeviceType::fSWI:
        poDev = new SwitchDevice(oPos, iSize);
        break;
    case DeviceType::fTXT:
        poDev = new TextDevice(oPos, iSize);
        break;
    case DeviceType::fLED:
        poDev = new LedDevice(oPos, iSize);
        break;
    case DeviceType::fLED1:
        poDev = new LedDevice(oPos, iSize);
        poDev->setColor(LedDevice::RED);
        break;
    case DeviceType::fLED2:
        poDev = new LedDevice(oPos, iSize);
        poDev->setColor(LedDevice::BLUE);
        break;
    case DeviceType::fLED3:
        poDev = new LedDevice(oPos, iSize);
        poDev->setColor(LedDevice::YELLOW);
        break;
    case DeviceType::fSS:
        poDev = new S7Device(oPos, iSize);
        break;
    case DeviceType::fINV_INTERNAL:
        poDev = new InverterDevice(oPos, iSize);
        break;
    case DeviceType::fAND:
        poDev = new SimpleDevice(DeviceType::fAND, oPos, iSize);
        break;
    case DeviceType::fNAND:
        poDev = new SimpleDevice(DeviceType::fAND, oPos, iSize);
        poDev->setInverted(true);
        break;
    case DeviceType::fOR:
        poDev = new SimpleDevice(DeviceType::fOR, oPos, iSize);
        break;
    case DeviceType::fNOR:
        poDev = new SimpleDevice(DeviceType::fOR, oPos, iSize);
        poDev->setInverted(true);
        break;
    case DeviceType::fXOR:
        poDev = new SimpleDevice(DeviceType::fXOR, oPos, iSize);
        break;
    case DeviceType::fONE:
        poDev = new SimpleDevice(DeviceType::fONE, oPos, iSize);
        break;
    case DeviceType::fTFF:
        poDev = new TFFDevice(oPos, iSize);
        break;
    default:
        poDev = new XDevice(iFunction, oPos, iSize);
        break;
    }
    poDev->init();

    if (poParent) {
        poDev->setParent(poParent);
        bool bImport = (iSize != -1);
        if (!poParent->addChild(poDev, bImport)) {
            delete poDev;
            poDev = 0;
        }
    }

    return poDev;
}
