#include "neter.h"
#include <errno.h>
#include <assert.h>
#include <unistd.h>

Neter& Neter::GetInstance()
{
	static Neter _instance;
	return _instance;
}

Neter::Neter()
{
	epoll_fd = epoll_create(POLL_MAX_SIZE);
	if(epoll_fd == -1)
	{
		perror(strerror(errno));
		assert(false);
	}
}

Neter::~Neter()
{
	if(epoll_fd != -1)
	{
		close(epoll_fd);
	}
}

void Neter::Work(time_t timeout)
{
	int result = 0; 		
	while(1)
	{
		result = epoll_wait(epoll_fd, events, POLL_MAX_SIZE, timeout);

		for(int i = 0; i < result; ++i)
		{
		}
	}
}
