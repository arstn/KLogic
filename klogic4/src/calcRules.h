#ifndef __CALC_STACK_CLASS__
#define __CALC_STACK_CLASS__

#include "Operator.h"
#include "QueueInfo.h"
#include <iostream>
#include <vector>
#include <qstring.h>
#include <QDebug>

using namespace std;

// ----------------------------------
// defines all known klogic operators
// ----------------------------------

// ----------------------------------
// Calculator parsing results
// ----------------------------------
class CalcRules
{
public:
	CalcRules()
	{
		clear();
	}

	inline unsigned int push(const QueueInfo& oInfo)
	{
		m_vQueueInfo.push_back(oInfo);
		return m_vQueueInfo.size() - 1;
	}

	inline QueueInfo& operator [] (int idx)
	{
		if (m_bIsStatic) {
		  return m_vQueueInfo[0];
		}
		return m_vQueueInfo[idx];
	}

	inline void setStatic(bool bStatic)
	{
	  m_bIsStatic = bStatic;
	}
	
	inline bool isStatic() const
	{
		return m_bIsStatic;
	}

	inline int size() const
	{
		return m_vQueueInfo.size();
	}

	inline void dump() const
	{
		int sz = size();
		for (int i = 0; i < sz; ++i) {
		  qDebug() << m_vQueueInfo[i].inputName << " at offset " << m_vQueueInfo[i].posInEquation << ", length " << m_vQueueInfo[i].lenInEquation;
		}
	}

	inline void clear()
	{
		m_vQueueInfo.clear();
		m_bIsStatic = false;
	}

	static QString ITEM_THIS;

private:
	bool m_bIsStatic;
	vector<QueueInfo> m_vQueueInfo;
};

#endif
