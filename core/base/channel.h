#ifndef _CHANNLE_H_ 
#define _CHANNLE_H_


#include <map>
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
#include <functional>
#include "parser.h"

class ChannelManager;

class Channel
{
public:

	friend ChannelManager;

	Channel(int f);

	~Channel()
	{
		Log::Trace("Channel::~Channel, cid=%d, fd=%d, val=0x%x\n", cid, fd, this);
		if(close(fd))
		{
			Log::Error("Channel::~Channel, close failed, %d(%s)\n", errno, strerror(errno));
		}
	}


	void PutData(const char * buf, size_t size);

	void Handle(const epoll_event *event);
	void SetParser(Parser *p) { parser = p; };

	channel_id_t GetCid() const { return cid; }
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
	bool IsClose() { return ready_close; };
	virtual void Send();
	virtual void Recv();
	virtual void OnRecv();

protected:
	int fd;
	channel_id_t cid;
	Parser * parser;
	bool ready_close;
};

class Acceptor : Channel
{
public:
	Acceptor(int f, ParserHatcher h)
	: hatcher(h)
	, Channel(f)
	{};

	static bool Listen(const char * ip, int port, ParserHatcher hatcher);

private:

	void Recv() {};
	void OnRecv();

	ParserHatcher hatcher;
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
	typedef std::map<channel_id_t, Channel *> ChannelMap;
	
	Mutex lock;
	ChannelMap channel_map;

	typedef std::vector<Channel *> ChannelVector;
	ChannelVector ready_close_vector;
public:
	bool PutData(channel_id_t cid, const char *buf, size_t size);

	void Add(Channel * c);
	void ReadyClose(Channel * c);
	void Close();
	
	static channel_id_t AllocID()
	{
		static channel_id_t cid = 0;
		static SpinLock cid_lock;
		
		SpinLockGuard guarder(cid_lock);
		return cid++;
	}
	static ChannelManager &GetInstance() { static ChannelManager channel_manager; return channel_manager; }
};

#endif
