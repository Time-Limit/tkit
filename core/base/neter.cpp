#include "neter.h"
#include "log.h"
#include "task.h"
#include "thread.h"
#include "lock.h"
#include "channel.h"
#include "signal.h"

Neter& Neter::GetInstance()
{
	static Neter _instance;
	return _instance;
}

Neter::Neter()
{
	epoll_fd = epoll_create(1);
	if(epoll_fd == -1)
	{
		Log::Error("Neter::Neter, error=%s\n", strerror(errno));
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

bool Neter::Ctl(int op, int fd, struct epoll_event *event)
{
	if(epoll_ctl(epoll_fd, op, fd, event))
	{
		Log::Error("Neter::Ctl, error=%s\n", strerror(errno));
		return false;
	}

	return true;
}


void Neter::Wait(time_t timeout)
{
	ChannelManager::GetInstance().Close();

	int result = epoll_wait(epoll_fd, events, EPOLL_EVENT_SIZE, timeout);

	if(result < 0)
	{
		Log::Error("Neter::Wait, errno=%d, info=%s\n", errno, strerror(errno));
		return ;
	}

	struct epoll_event *begin = events, *end = events + result;

	for(; begin < end; ++begin)
	{
		if(begin->data.ptr)
		{
			((Channel *)(begin->data.ptr))->Handle(begin);
		}
	}
}
