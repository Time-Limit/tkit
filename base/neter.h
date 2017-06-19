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

class Channel
{
public:
	virtual void Handle(const struct epoll_event &event) = 0;
};

class Acceptor : public Channel
{
public:
	virtual void Handle(const struct epoll_event &event);
	
	Acceptor(int f) : fd(f) {};

private:
	int fd;
};

class NeterTask : public Task
{
public:
	virtual void Exec();
};

class Neter
{
public:

	enum POLL_CONFIG
	{
		POLL_MAX_SIZE = 4096,
	};

	Neter();
	~Neter();

	static Neter& GetInstance();

	void Wait(time_t timeout = 1);

	bool Ctl(int op, int fd, struct epoll_event *event);

	bool Listen(unsigned int port);

private:
	int epoll_fd;

	struct epoll_event events[POLL_MAX_SIZE];
};

#endif
