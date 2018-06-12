#include "neter.h"

using namespace TCORE;

Neter::Session::Session(SESSION_TYPE t, int f)
: fd(f)
, type(t)
{
	switch(type)
	{
		case ACCEPTOR_SESSION:
		{
			read_fd_func = AcceptorReadFDFunc;
		}break;
		case EXCHANGE_SESSION:
		{
			read_fd_func = ExchangeReadFDFunc;
		}break;
		case SECURE_EXCHANGE_SESSION:
		{
			read_fd_func = SecureExchangeReadFDFunc;
		}break;
		default:
		{
			fd = -1;
			type = INVALID_SESSION;
			read_fd_func = ReadFDFunc(nullptr);
		}
	}
}

void Neter::Wait(time_t timeout)
{
	int res = epoll_wait(epoll_instance_fd, events, EPOLL_EVENT_SIZE, timeout);

	struct epoll_event *begin = events, *end = events + res;

	for( ; begin < end; ++begin)
	{
		// do something
	}
};

