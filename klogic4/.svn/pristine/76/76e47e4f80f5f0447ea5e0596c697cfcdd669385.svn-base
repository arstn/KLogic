/*************************************************/
// member methods for class fifo
//
// signal delay
//
// Andreas Rostin
// 15.03.99
//
// Peter Fischer 19.09.03 int array replaced with set<int>
/************************************************************/
#include <fifo.h>

int Fifo::instance = 0;

Fifo::Fifo(int iDepth)
{
	instance++;
	setDelay(iDepth);
}

Fifo::~Fifo()
{
	instance--;
}

// size of the fifo, is used as a delay
void Fifo::setDelay(int iDepth)
{
	m_iDepth = iDepth;
	if (!m_iDepth)
		assign(1,0);
	else
		assign(m_iDepth, 0);
}

// size of the fifo, is used as a delay
int Fifo::getDelay()
{
	return m_iDepth;
}

void Fifo::flush(int val)
{
	if (!m_iDepth)
		assign(1,0);
	else
		assign(m_iDepth, val);
}

// shift queue by 1
// insert newval on first place
// return dropped value
int Fifo::put(int iVal)
{
	int iRet = back();
	pop_back();
	push_front(iVal);
	return iRet;
}

int Fifo::get()
{
	return back();
}
