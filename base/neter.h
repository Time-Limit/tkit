#ifndef _NETER_H_
#define _NETER_H_

#include <sys/epoll.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

class Neter
{
public:

	enum POLL_CONFIG
	{
		POLL_MAX_SIZE = 4096,
	};

	Neter();
	~Neter();

	Neter& GetInstance();

	void Work(time_t timeout = 1);
private:
	int epoll_fd;

	struct epoll_event events[POLL_MAX_SIZE];
};

#endif
