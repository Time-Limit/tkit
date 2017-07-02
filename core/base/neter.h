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

	enum POLL_CONFIG
	{
		READ_BUF_SIZE = 256,
	};

	Allocer<READ_BUF_SIZE> read_buf_allocer;

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

	Octets &GetDataIn() { return data_in; }

private:
	int fd;
	Acceptor &acceptor;
	Mutex data_in_lock;
	Octets data_in;
};

class Neter
{
public:

	enum NETER_CONFIG
	{
		POLL_MAX_SIZE = 1024,
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
