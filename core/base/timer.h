#ifndef _TIMER_H_
#define _TIMER_H_

#include <time.h>

class Timer
{
public:
	static time_t Now()
	{
		return time(0);
	}
};

#endif
