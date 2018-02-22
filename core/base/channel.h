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
#include "session.h"

class ChannelManager;

class Channel
{
	friend class ChannelManager;
public:
	Channel(int f);
	virtual ~Channel()
	{
		LOG_TRACE("Channel::~Channel, cid=%d, fd=%d, val=0x%p", cid, fd, this);
		if(close(fd))
		{
			LOG_ERROR("Channel::~Channel, close failed, %d(%s)", errno, strerror(errno));
		}
	}
	void Handle(const epoll_event *event);
	channel_id_t ID() const { return cid; }
	bool IsClose() const { return ready_close; }
	void Close();
protected:
	virtual void Send(const void *buf, size_t size) = 0;
	virtual void OnSend() = 0;
	virtual void Recv() = 0;
	virtual void OnRecv() = 0;
protected:
	int fd;
	channel_id_t cid;
	bool ready_close;
};

class Exchanger final : public Channel
{
	friend class Session;
public:
	Exchanger(int f, Session *s)
	: Channel(f), session(s), ready_send(false), cur_cursor(0)
	{
		InitPeerName();
	}

	~Exchanger() { session->Close(); session = nullptr; }

	const char* IP() const { return ip; }

private:
	void Send(const void *buf, size_t size);
	void OnSend();
	void Recv();
	void OnRecv();
	void RegisterSendEvent();

	void InitPeerName();

private:
	Session *session;
	Mutex olock;
	Octets ibuff, obuff;
	enum
	{
		TMP_BUFF_SIZE = 512,
	};
	char tmp_buff[TMP_BUFF_SIZE];
	bool ready_send;
	char ip[16] /*xxx.xxx.xxx.xxx*/;
	size_t cur_cursor;
};

class Acceptor final : public Channel
{
public:
	Acceptor(int f, SessionManager &manager)
	: Channel(f), session_manager(manager)
	{};

	static bool Listen(const char * ip, int port, SessionManager &manager);

protected:
	void Send(const void *, size_t) {}
	void OnSend() {}
	void Recv() {};
	void OnRecv();

	SessionManager &session_manager;
};

/*
 * as client
class Connect() : public Channel final
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
	bool Send(channel_id_t cid, const char *buf, size_t size);

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
