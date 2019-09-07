#ifndef __FIFO_CLASS__
#define __FIFO_CLASS__

#include <list>

using namespace std;

class Fifo: private list<int>
{
public:
	static int instance;

	Fifo(int);
	~Fifo();
	int put(int);
	int get();
	void setDelay(int);
	int getDelay();
	void flush(int);
private:
	int m_iDepth;
};

#endif


