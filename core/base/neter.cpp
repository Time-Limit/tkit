#include "neter.h"
#include "log.h"
#include "task.h"
#include "thread.h"
#include "lock.h"

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
	int result = 0;
	struct epoll_event *begin = NULL, *end = NULL;
	while(1)
	{
		result = epoll_wait(epoll_fd, events, POLL_MAX_SIZE, timeout);

		begin = events, end = events + result;

		for(; begin < end; ++begin)
		{
			if(begin->data.ptr)
			{
				((EventHandler *)(begin->data.ptr))->Handle(*begin);
			}
		}
	}
}

bool Neter::Listen(unsigned int port, TaskCreator * creator)
{
	int sockfd = 0;
	int optval = -1;
	struct sockaddr_in server;
	socklen_t socklen = sizeof(struct sockaddr_in);

	sockfd = socket(PF_INET, SOCK_STREAM, 0);

	if(sockfd < 0)
	{
		Log::Error("Neter::Listen, error=%s\n", strerror(errno));
		return false;
	}

	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
	memset(&server, 0, socklen);
	
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(port);
	server.sin_family = AF_INET;

	if(bind(sockfd, (struct sockaddr *)&server, socklen) < 0)
	{
		Log::Error("Neter::Listen, error=%s\n", strerror(errno));
		return false ;
	}

	struct epoll_event ev;
	memset(&ev, 0, sizeof(ev));
	ev.events = EPOLLIN|EPOLLET;
	ev.data.ptr = new Acceptor(sockfd, creator);
	Ctl(EPOLL_CTL_ADD, sockfd, &ev);
	listen(sockfd, 5);

	return true;
};

void Acceptor::Handle(const struct epoll_event &event)
{
	struct sockaddr_in accept_addr;
	int server_addr_len;

	int connect_fd = accept(fd, (struct sockaddr *)&accept_addr, (socklen_t *)&server_addr_len);

	if(connect_fd < 0)
	{
		Log::Error("Acceptor::Handle, accept fialed\n");
		return ;
	}

	struct epoll_event ev;
	memset(&ev, 0, sizeof(ev));
	ev.events = EPOLLIN | EPOLLOUT | EPOLLET;
	ev.data.ptr = new Connector(connect_fd, this);
	Neter::GetInstance().Ctl(EPOLL_CTL_ADD, connect_fd, &ev);
	Log::Trace("Acceptor::Handle, add new socket, fd=%d\n", connect_fd);
}

void Connector::Handle(const struct epoll_event &event)
{
	if(event.events | EPOLLIN)
	{
		int sum = 0;
		MutexGuard guard(data_in_lock);
		while((part = read(fd, buf, BUF_SIZE)) > 0)
		{
			sum += part;
			data_in.insert(data_in.end(), buf, part);
		}
		
		if(sum)
		{
			ThreadPool::GetInstance().AddTask(acceptor.Create(this));
		}
	}
}
