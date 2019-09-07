#include <stdio.h>
#include <tool.h>
#include <qstring.h>
#include<QDebug>

Referenced::Referenced()
{
	m_iRef = 1;
}

Referenced::~Referenced()
{
}

int Referenced::addRef() const
{
	return ++m_iRef;
}

int Referenced::relRef() const
{
	m_iRef--;

	int iRet = m_iRef;
	if (iRet <= 0)
		delete this;
	return iRet;
}

BitSet::BitSet()
{
	insert(0);
	for (int i = 0; i <= Tool::MAXPOW; i++)
		insert(Tool::powTab[i]);
}

BitSet Tool::m_oBitSet;

unsigned int Tool::powTab[] = {1,2,4,8,16,32,64,128,256,512,
                              1024,2048,4096,8192,16384,32768,65536,131072,262144,524288,
                              1048576,2097152,4194304,8388608,16777216,
                              33554432,67108864,134217728,268435456, 536870912,
			      1073741824};

unsigned int Tool::pow2(int exp)
{
	if (exp > MAXPOW) {
		qDebug() << "tool::pow2(" << exp << ") overflow";
		return 0;
	}
	if (exp < 0) {
		qDebug() << "tool::pow2(" << exp << ") negative exponent";
		return 0;
	}
	return powTab[exp];
}

// returns nearest lower match
unsigned int Tool::exp(unsigned int value)
{
	if (value == 0) return 0;

	int i = 0;
	while(value > powTab[i] && i <= MAXPOW) i++;
	if (i > MAXPOW) {
		qDebug() << "tool::exp(" << value << ") overflow";
		return 0;
	}
	if (value < powTab[i]) i--;
	if (i < 0)
		return 0;
	return i;
}

// static
int Tool::pow2complement(int base, int clearbit)
{
	int value = pow2(base);
	value += value - 1;             //all bits on
	value -= pow2(clearbit);        //clear bit
	return value;
}

// static
QString Tool::bitString(unsigned char byte, int len)
{
	QString s;

	for(int i = len - 1; i >= 0; i--) {
		if (byte & Tool::pow2(i))
			s += '1';
		else
			s += '0';
	}
	return s;
}

// static
int Tool::singleBitToggled(int iVal1, int iVal2)
{
	return singleBitSet(xOr(iVal1,iVal2));
}

// static
int Tool::singleBitSet(int iXORVal)
{
	return m_oBitSet.find(iXORVal) != m_oBitSet.end();
}

// static
int Tool::xOr(int iVal1, int iVal2)
{
	return iVal1 ^ iVal2;
}


