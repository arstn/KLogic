#ifndef DEVICELIST_CLASS_H
#define DEVICELIST_CLASS_H

class XDevice;

class DeviceListEntry
{
public:
	DeviceListEntry()
	{
		m_poDevice = 0;
		m_iDeviceID = 0;
	}

	DeviceListEntry(XDevice *poXDevice, int iDeviceID)
	{
		m_poDevice = poXDevice;
		m_iDeviceID = iDeviceID;
	}

	inline XDevice * operator* () const
	{
		return m_poDevice;
	}

	inline bool operator== (int iDeviceID) const
	{
		return (m_iDeviceID == iDeviceID);
	}

	inline void clear()
	{
		m_poDevice = 0;
		m_iDeviceID = 0;
	}

	inline XDevice * getDevice() const
	{
		return m_poDevice;
	}

	inline void setDeviceID(int iDeviceID)
	{
		m_iDeviceID = iDeviceID;
	}

	inline int getDeviceID() const
	{
		return m_iDeviceID;
	}

private:
	XDevice *m_poDevice;
	int m_iDeviceID;
};

class DeviceList
{
public:
	class iterator
	{
	public:
		iterator(DeviceList *poParent, int iIdx) { m_poParent = poParent; m_iIdx = iIdx; }
		inline void operator++ () { m_iIdx++; }
		inline void operator++ (int) { m_iIdx++; }
		inline bool operator!= (const iterator& other) const { return (m_iIdx != other.m_iIdx); }
		inline void operator= (const iterator& other) { m_iIdx = other.m_iIdx; }
		inline DeviceListEntry& operator* () { return m_poParent->get(m_iIdx); }
		inline XDevice * getDevice() { return m_poParent->getDevice(m_iIdx); }
	private:
		DeviceList *m_poParent;
		int m_iIdx;
	};

	DeviceList()
		: m_oBeginIterator(this, 0),
		  m_oEndIterator(this, 0)
	{
		m_iArraySize = CONNECTOR_LIST_SIZE;
		m_poDevices = new DeviceListEntry[m_iArraySize];
		for (int i = 0; i < CONNECTOR_LIST_SIZE; i++)
			m_poDevices[i] = DeviceListEntry();
		m_iIndex = 0;
	}

	inline void clear()
	{
		for (int i = 0; i < m_iArraySize; i++)
			m_poDevices[i].clear();
		m_iIndex = 0;
		m_oEndIterator = iterator(this, m_iIndex);
	}

	inline XDevice * operator[] (int iDeviceID) const
	{
		for (int i = 0; i < m_iIndex; i++)
			if (m_poDevices[i] == iDeviceID)
				return *m_poDevices[i];
		return 0;
	}

	inline const DeviceListEntry * find(int iDeviceID) const
	{
		for (int i = 0; i < m_iIndex; i++)
			if (m_poDevices[i] == iDeviceID)
				return &m_poDevices[i];
		return 0;
	}

	inline bool add(XDevice *poXDevice, int iDeviceID)
	{
		if (m_iIndex >= m_iArraySize) {
			DeviceListEntry *poDevices = new DeviceListEntry[m_iArraySize + 10];
			memcpy(poDevices, m_poDevices, sizeof(DeviceListEntry) * m_iArraySize);
			m_poDevices = poDevices;
			m_iArraySize += 10;
		}

		m_poDevices[m_iIndex] = DeviceListEntry(poXDevice, iDeviceID);
		m_iIndex++;
		m_oEndIterator = iterator(this, m_iIndex);
		return true;
	}

	inline XDevice * remove(int iDeviceID)
	{
		for (int i = 0; i < m_iIndex; i++) {
			if (m_poDevices[i] == iDeviceID) {
				XDevice *poDevice = *m_poDevices[i];
				int iStartIndex = i + 1;
				int iEndIndex = m_iIndex;
				while (iStartIndex < iEndIndex) {
					m_poDevices[iStartIndex - 1] = m_poDevices[iStartIndex];
					iStartIndex++;
				}
				m_iIndex--;
				m_oEndIterator = iterator(this, m_iIndex);
				return poDevice;
			}
		}
		return 0;
	}

	inline const iterator& begin() const
	{
		return m_oBeginIterator;
	}

	inline const iterator& end() const
	{
		return m_oEndIterator;
	}

	inline int size()
	{
		return m_iIndex;
	}

	inline DeviceListEntry& get(int iIndex)
	{
		return m_poDevices[iIndex];
	}

	inline XDevice * getDevice(int iIndex)
	{
		return m_poDevices[iIndex].getDevice();
	}

private:
	static const int CONNECTOR_LIST_SIZE = 5;

	DeviceListEntry *m_poDevices;
	int m_iIndex;
	int m_iArraySize;
	iterator m_oBeginIterator;
	iterator m_oEndIterator;
};

#endif

