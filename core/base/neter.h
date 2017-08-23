#ifndef _NETER_H_
#define _NETER_H_

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <assert.h>
#include "task.h"
#include "octets.h"
#include "allocer.h"

class Neter
{
	enum
	{
		EPOLL_EVENT_SIZE = 1024,
	};
public:
	Neter();
	~Neter();

	static Neter& GetInstance();

	void Wait(time_t timeout = 1);

	bool Ctl(int op, int fd, struct epoll_event *event);

private:
	int epoll_fd;

	struct epoll_event events[EPOLL_EVENT_SIZE];
};

#endif
