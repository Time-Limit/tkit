#include "neter.h"
#include "log.h"
#include "thread.h"
#include "lock.h"
#include "channel.h"
#include "signal.h"

using namespace TCORE;

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
		Log::Error("Neter::Neter, error=", strerror(errno));
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
		Log::Error("Neter::Ctl, error=", strerror(errno));
		return false;
	}

	return true;
}


void Neter::Wait(time_t timeout)
{
	int result = epoll_wait(epoll_fd, events, EPOLL_EVENT_SIZE, timeout);

	if(result < 0)
	{
		Log::Error("Neter::Wait, errno=", errno, " info=", strerror(errno));
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

	ChannelSet::iterator it = channel_set.begin();
	ChannelSet::iterator ie = channel_set.end();

	for(; it != ie; ++it)
	{
		epoll_event event;
		Ctl(EPOLL_CTL_DEL, (*it)->ID(), &event);
		delete *it;
	}

	channel_set.clear();
}

void Neter::ReadyClose(Channel *c)
{
	channel_set.insert(c);
}
