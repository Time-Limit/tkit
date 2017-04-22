#include "net.h"

Net::Net()
{
	epoll_fd = epoll_create(EPOLL_SIZE);
}

Net::~Net()
{
	close(epoll_fd);
}

int Add(int fd)
{
	static epoll_event event;
	event.data.fd = fd;
	event.events = EPOLLIN|EPOLLOUT;
	return epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event);
}

int Mod(int fd)
{
	//TODO zmx 
}

int Del(int fd)
{

}
