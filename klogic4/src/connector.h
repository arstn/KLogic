#ifndef CONNECTOR_CLASS_H
#define CONNECTOR_CLASS_H

class Calculator;

class Connector
{
public:
	Connector()
	{
		m_poCalculator = 0;
		m_iConnectorID = 0;
		m_iPosition = 0;
	}

	Connector(Calculator *poCalculator, int iConnectorID, const QString& sName, int iPosition)
	{
		m_poCalculator = poCalculator;
		m_iConnectorID = iConnectorID;
		m_sName = sName;
		m_iPosition = iPosition;
	}

	inline Calculator * getCalculator() const
	{
		return m_poCalculator;
	}

	inline Calculator * operator* () const
	{
		return m_poCalculator;
	}

	inline bool operator== (int iConnectorID) const
	{
		return (m_iConnectorID == iConnectorID);
	}

	inline bool operator== (const QString& sName) const
	{
		//cout << "equal? " << m_sName << " <-> " << sName << " result=" << (m_sName == sName) << endl;
		return (m_sName == sName);
	}

	inline void setConnectorID(int iConnectorID)
	{
		m_iConnectorID = iConnectorID;
	}

	inline int getConnectorID() const
	{
		return m_iConnectorID;
	}

	inline void setName(const QString& sName)
	{
		m_sName = sName;
	}

	inline const QString& getName() const
	{
		return m_sName;
	}

	inline void setPosition(int iPosition)
	{
		m_iPosition = iPosition;
	}

	inline int getPosition() const
	{
		return m_iPosition;
	}

private:
	Calculator *m_poCalculator;
	int m_iConnectorID;
	QString m_sName;
	int m_iPosition;
};

class ConnectorList
{
public:
	class iterator
	{
	public:
		iterator(ConnectorList *poParent, int iIdx) { m_poParent = poParent; m_iIdx = iIdx; }
		inline void operator++ (int) { m_iIdx++; }
		inline bool operator!= (const iterator& other) const { return (m_iIdx < other.m_iIdx); }
		inline void operator= (const iterator& other) { m_iIdx = other.m_iIdx; }
		inline Connector& operator* () { return m_poParent->get(m_iIdx); }
		inline Calculator * getCalculator() { return m_poParent->getCalculator(m_iIdx); }
	private:
		ConnectorList *m_poParent;
		int m_iIdx;
	};

	ConnectorList()
		: m_oBeginIterator(this, 0),
		  m_oEndIterator(this, 0)
	{
		m_iArraySize = CONNECTOR_LIST_SIZE;
		m_poConnectors = new Connector[m_iArraySize];
		for (int i = 0; i < CONNECTOR_LIST_SIZE; i++)
			m_poConnectors[i] = Connector();
		m_iIndex = 0;
	}

	inline Calculator * operator[] (int iConnectorID) const
	{
		for (int i = 0; i < m_iIndex; i++)
			if (m_poConnectors[i] == iConnectorID)
				return *m_poConnectors[i];
		return 0;
	}

	inline bool hasConnector(const QString& sName) const
	{
		for (int i = 0; i < m_iIndex; i++) {
			if (m_poConnectors[i] == sName) {
				return true;
			}
		}
		return false;
	}

	inline const Connector * find(int iConnectorID) const
	{
		for (int i = 0; i < m_iIndex; i++)
			if (m_poConnectors[i] == iConnectorID)
				return &m_poConnectors[i];
		return 0;
	}

	inline const Connector * find(const QString& sName) const
	{
		for (int i = 0; i < m_iIndex; i++)
			if (m_poConnectors[i] == sName)
				return &m_poConnectors[i];
		return 0;
	}

	inline bool setName(int iConnectorID, const QString& sName)
	{
		for (int i = 0; i < m_iIndex; i++) {
			if (m_poConnectors[i] == iConnectorID) {
				m_poConnectors[i].setName(sName);
				return true;
			}
		}
		return false;
	}

	inline const QString& getName(int iConnectorID) const
	{
		for (int i = 0; i < m_iIndex; i++) {
			if (m_poConnectors[i] == iConnectorID) {
				return m_poConnectors[i].getName();
			}
		}
		return KlogicGlobal::EMPTYSTRING;
	}

	inline int getConnectorID(const QString& sName) const
	{
		for (int i = 0; i < m_iIndex; i++) {
			if (m_poConnectors[i] == sName) {
				return m_poConnectors[i].getConnectorID();
			}
		}
		return 0;
	}

	inline bool mapTo(int iConnectorID, int iNewConnectorID, const QString& sName)
	{
		for (int i = 0; i < m_iIndex; i++) {
			if (m_poConnectors[i] == iConnectorID) {
				m_poConnectors[i].setConnectorID(iNewConnectorID);
				m_poConnectors[i].setName(sName);
				return true;
			}
		}
		return false;
	}

	inline bool add(Calculator *poCalculator, int iConnectorID, const QString& sName, int iPosition)
	{
		if (m_iIndex >= m_iArraySize) {
			Connector *poConnectors = new Connector[m_iArraySize + 10];
			memcpy(poConnectors, m_poConnectors, sizeof(Connector) * m_iArraySize);
			m_poConnectors = poConnectors;
			m_iArraySize += 10;
		}

		m_poConnectors[m_iIndex] = Connector(poCalculator, iConnectorID, sName, iPosition);
		m_iIndex++;
		m_oEndIterator = iterator(this, m_iIndex);
		return true;
	}

	inline Calculator * remove(int iConnectorID)
	{
		for (int i = 0; i < m_iIndex; i++) {
			if (m_poConnectors[i] == iConnectorID) {
				Calculator *poCalculator = *m_poConnectors[i];
				int iStartIndex = i + 1;
				int iEndIndex = m_iIndex;
				while (iStartIndex < iEndIndex) {
					m_poConnectors[iStartIndex - 1] = m_poConnectors[iStartIndex];
					iStartIndex++;
				}
				m_iIndex--;
				m_oEndIterator = iterator(this, m_iIndex);
				return poCalculator;
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

	inline int size() const
	{
		return m_iIndex;
	}

	inline Connector& get(int iIndex)
	{
		return m_poConnectors[iIndex];
	}

	inline Calculator * getCalculator(int iIndex)
	{
		return m_poConnectors[iIndex].getCalculator();
	}

private:
	static const int CONNECTOR_LIST_SIZE = 5;

	Connector *m_poConnectors;
	int m_iIndex;
	int m_iArraySize;
	iterator m_oBeginIterator;
	iterator m_oEndIterator;
};

#endif

