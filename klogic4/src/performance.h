#ifndef PERFORMANCE_CLASS_H
#define PERFORMANCE_CLASS_H

#include <iostream>
#include <sys/time.h>

class Performance
{
public:
	Performance()
	{
		m_iState = NONE;
	}

	inline void start()
	{
		m_iState = START;
		gettimeofday(&t_start, 0);
	}

	inline void end()
	{
		if (m_iState != START)
		{
			cerr << "aborting Performance::end() - start() first!" << endl;
			return;
		}
		m_iState = END;
		gettimeofday(&t_end, 0);
	}

	inline double getMilliSeconds()
	{
		if (m_iState != END)
		{
			cerr << "aborting Performance::getMilliSeconds() - end() first!" << endl;
			return 0.0;
		}
		return (1000 * (t_end.tv_sec - t_start.tv_sec)) + ((t_end.tv_usec - t_start.tv_usec) / 1000);
	}

private:
	enum STATE { NONE, START, END };
	int m_iState;
        // for performance measurement purposes
        struct timeval t_start;
        struct timeval t_end;
};

#endif

