#ifndef CONNECTION_CLASS_H
#define CONNECTION_CLASS_H

#include "device.h"

class Connection
{
public:
	Connection(Device *poDevice, int iInputID, int iOutputID)
	{
		m_iInputID = iInputID;
		m_iOutputID = iOutputID;
		m_poDevice = poDevice;
	}

	inline bool operator== (const Connection& oOther) const
	{
		return (oOther.m_iInputID == m_iInputID &&
			oOther.m_iOutputID == m_iOutputID &&
			oOther.m_poDevice == m_poDevice);
	}

	inline Device * getDevice() const
	{
		return m_poDevice;
	}

	inline int getInputID() const
	{
		return m_iInputID;
	}

	inline int getOutputID() const
	{
		return m_iOutputID;
	}

	inline int getInput() const
	{
		return m_poDevice->output(m_iOutputID);
	}

	inline bool hasBooleanInput() const
	{
		return m_poDevice->hasBooleanOutput(m_iOutputID);
	}

	inline bool inputChanged() const
	{
		return m_poDevice->outputChanged();
	}

private:
	Device *m_poDevice;	// foreign device
	int m_iInputID;		// foreign device output id

	int m_iOutputID;	// this output id
};

class ConnectionList
{
public:
	class iterator
	{
	public:
		iterator(ConnectionList *poParent, int iIdx) { m_poParent = poParent; m_iIdx = iIdx; }
		inline void operator++ (int) { m_iIdx++; }
		inline bool operator!= (const iterator& other) const { return (m_iIdx != other.m_iIdx); }
		inline void operator= (const iterator& other) { m_iIdx = other.m_iIdx; }
		inline Connection * operator* () { return (*m_poParent)[m_iIdx]; }
	private:
		ConnectionList *m_poParent;
		int m_iIdx;
	};

	ConnectionList()
		: m_oBeginIterator(this, 0),
		  m_oEndIterator(this, 0)
	{
		m_iIndex = 0;
	}

	~ConnectionList()
	{
		for (iterator iter = begin(); iter != end(); iter++)
			delete *iter;
	}

	inline bool add(Device *poDevice, int iInputID, int iOutputID)
	{
		if (m_iIndex < CONNECTION_LIST_SIZE) {
			m_oConnections[m_iIndex] = new Connection(poDevice, iInputID, iOutputID);
			m_iIndex++;
			m_oEndIterator = iterator(this, m_iIndex);
			return true;
		}
		return false;
	}

	inline bool remove(Device *poDevice, int iInputID, int iOutputID)
	{
		Connection oConn(poDevice, iInputID, iOutputID);
		for (int i = 0; i < m_iIndex; i++) {
			if (*m_oConnections[i] == oConn) {
				delete m_oConnections[i];
				int iStartIndex = i + 1;
				int iEndIndex = m_iIndex;
				while (iStartIndex < iEndIndex) {
					m_oConnections[iStartIndex - 1] = m_oConnections[iStartIndex];
					iStartIndex++;
				}
				m_iIndex--;
				m_oEndIterator = iterator(this, m_iIndex);
				return true;
			}
		}
		return false;
	}

	inline Connection * get(int iInputID)
	{
		for (int i = 0; i < m_iIndex; i++) {
			if (m_oConnections[i]->getInputID() == iInputID)
				return m_oConnections[i];
		}
		return 0;
	}

	inline Device * getFirstDevice()
	{
		if (m_iIndex)
			return m_oConnections[0]->getDevice();
		return 0;
	}

	inline Connection * getFirstConnection()
	{
		if (m_iIndex)
			return m_oConnections[0];
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

	inline Connection * operator[] (int iIndex)
	{
		return m_oConnections[iIndex];
	}

private:
	static const int CONNECTION_LIST_SIZE = 100;

	Connection * m_oConnections[CONNECTION_LIST_SIZE];
	int m_iIndex;
	iterator m_oBeginIterator;
	iterator m_oEndIterator;
};

#endif

