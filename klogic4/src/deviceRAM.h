#ifndef __DEVICE_RAM__
#define __DEVICE_RAM__

#include <QPainter>
#include <QtGlobal>
#include "deviceCustom.h"
#include "busOutputArray.h"
#include "connection.h"
#include "grid.h"

class RAMDevice : public CustomDevice
{
public:
	RAMDevice(const QPoint& oPos, int size = -1)
		: CustomDevice(DeviceType::fRAM, oPos, size)
	{
	}

	virtual ~RAMDevice()
	{
		delete[] m_poRAM;
		delete m_poBus;
	}

	virtual void init()
	{
		m_iRamSize = 32;
		m_poRAM = new char[m_iRamSize];

		// named output
		m_poBus = new BusOutputArray(this, true, false);

		// named input
		Device::IMPORT_IGNORE_GLOBAL = true;
		m_iWriteEnableID = addInputName(INPUT_RAM_WRITE, 0);
		addInputName(IO_BUS_ADDRESS[0], 9);
		addInputName(IO_BUS_ADDRESS[1], 10);
		addInputName(IO_BUS_ADDRESS[2], 11);
		addInputName(IO_BUS_ADDRESS[3], 12);
		addInputName(IO_BUS_ADDRESS[4], 13);

		// internal variables
		m_iAddressID = addInternalName(INTERNAL_RAM_ADDRESS);
		m_iDataID = addInternalName(INTERNAL_RAM_ADDRESS);
		Device::IMPORT_IGNORE_GLOBAL = false;

		setSize(neededSize());

		CustomDevice::init();
	}

	virtual int RAMSize()
	{
		return m_iRamSize;
	}

	virtual void setRAMSize(int new_size)
	{
		if (new_size > 0 && new_size <= 32) {
			for (int i = m_iRamSize - 1; i < new_size; i++)
				m_poRAM[i] = 0;
			m_iRamSize = new_size;
		}
	}

	virtual void setRAMStorage(char *newram)
	{
		if (sizeof(newram) != sizeof(m_poRAM)) {
			qFatal("RAMDevice::setRAMStorage: invalid RAM size");
			return;
		}
		memcpy(m_poRAM, newram, m_iRamSize * sizeof(char));
	}

	virtual char * getRAMStorageCopy()
	{
		char *newram = new char[m_iRamSize];
		memcpy(newram, m_poRAM, m_iRamSize * sizeof(char));
		return newram;
	}

	virtual void setEquation()
	{
		Device::setEquation(QString::fromUtf8("D0 § D1 § D2 § D3 § D4 § D5 § D6 § D7"), m_iDataID);
		Device::setEquation(QString::fromUtf8("A0 § A1 § A2 § A3 § A4"), m_iAddressID);
	}

	virtual bool sizeChangeable() const
	{
		return false;
	}

	virtual void printImage(QPainter *, const QRect &)
	{
	}

	// return device's output value
	virtual int output(int iOutputID)
	{
		// bus device output: handle internal output without any delay
		int iWeight = m_poBus->getOutputWeight(iOutputID);
		if (iWeight == -1) qFatal("RAMDevice::output(): no valid bus index");

		const Calculator *poCalculator = m_oNamedOutput[m_iDataID];
		int iData = poCalculator->getResult();
 
		int wr = getUndefinedValue();
		Connection *poConnection = m_poConnectionList->get(m_iWriteEnableID);
		if (poConnection)
			wr = poConnection->getInput();

		poCalculator = m_oNamedOutput[m_iAddressID];
		int iAddress = poCalculator->getResult();

		if (wr) m_poRAM[iAddress] = iData;
		iData = m_poRAM[iAddress];
		forceOutputChange();
 
		if (isTristateControlInputEnabled()) {
		    return KlogicGlobal::Device::TRISTATE;
		}
		return 1 && (iData & iWeight);
	}

private:
	static const QString IO_BUS_ADDRESS[8];
	static const QString INPUT_RAM_WRITE;
	static const QString INTERNAL_RAM_ADDRESS;
	static const QString INTERNAL_RAM_DATA;

	// ram
	int m_iAddressID;        // ram device: output id of internal address
	int m_iDataID;           // ram device: output id of internal data
	int m_iWriteEnableID;    // ram input id of write enable

	char *m_poRAM;
	int m_iRamSize;

	BusOutputArray *m_poBus;
};

#endif
