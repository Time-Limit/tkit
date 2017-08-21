#ifndef _CHANNLE_H_ 
#define _CHANNLE_H_


#include <set>
#include <vector>
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
#include "octets.h"
#include "lock.h"
#include "exptype.h"
#include "log.h"

class ChannelManager;

class Channel
{
public:

	friend ChannelManager;

	Channel(int f) : fd(f), ready_send(false)
	{
		fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
	}

	~Channel()
	{
		Log::Trace("Channel::~Channel, fd=%d, val=0x%x\n", fd, this);
		if(close(fd))
		{
			Log::Error("Channel::~Channel, close failed, %d(%s)\n", errno, strerror(errno));
		}
	}


	void PutData(const char * buf, size_t size);

	void Handle(const epoll_event *event);

private:
	Mutex olock;
	Octets ibuff, obuff;
	enum
	{
		TMP_BUFF_SIZE = 512,
	};
	char tmp_buff[TMP_BUFF_SIZE];
	bool ready_send;

	void Close();
	virtual void Send();
	virtual void Recv();
	virtual void OnRecv();
protected:
	int fd;
};

class Acceptor : Channel
{
public:
	Acceptor(int f)
	: Channel(f)
	{};

	static bool Listen(int port);

private:
	void Recv();
	void OnRecv() {};
};

/*
 * as client
class Connect() : Channel
{
};
*/

class ChannelManager
{
private:
	typedef std::set<Channel *> ChannelSet;
	
	Mutex lock;
	ChannelSet channel_set;

	typedef std::vector<Channel *> ChannelVector;
	ChannelVector ready_close_vector;
public:
	bool PutData(channel_id_t cid, const char *buf, size_t size);

	void Add(Channel * c);
	void ReadyClose(Channel * c);
	void Close();

	static ChannelManager &GetInstance() { static ChannelManager channel_manager; return channel_manager; }
};

#endif
