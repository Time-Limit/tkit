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
#include "lock.h"

class EventHandler
{
public:
	virtual void Handle(const struct epoll_event &event) = 0;
};

class Connector;

class Acceptor : public EventHandler
{
public:
	virtual void Handle(const struct epoll_event &event);
	
	Acceptor(int f, TaskCreator * c)
	: fd(f)
	, creator(c)
	{};
public:
	Task * Create(Connector * c) const
	{
		Task * task = creator->Create();
		((NetTask *)task)->SetConnector(c);
		return task;
	}

private:
	int fd;

	TaskCreator * creator;
};

class Connector : public EventHandler
{
public:
	virtual void Handle(const struct epoll_event &event);

	Connector(int f, Acceptor * a)
	: fd(f)
	, acceptor(*a)
	{};
	
	enum
	{
		BUF_SIZE = 1024,
	};

	Mutex data_in_lock;

	class Queue
	{
	private:
		char * buf;
		size_t head, tail;
	public:
		Queue() : head(0), tail(0)
		{
			buf = (char *)malloc(BUF_SIZE);
		}
		~Queue()
		{
			free(buf);
		}
	};

private:
	Octets data_in;
	int part;
	int fd;
	const Acceptor &acceptor;
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

	bool Listen(unsigned int port, TaskCreator * creator);

private:
	int epoll_fd;

	struct epoll_event events[POLL_MAX_SIZE];
};

#endif
