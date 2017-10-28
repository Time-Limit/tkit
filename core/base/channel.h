#ifndef _CHANNLE_H_ 
#define _CHANNLE_H_


#include "octets.h"
#include "lock.h"
#include "log.h"
#include "parser.h"
#include "enums.h"
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
#include <functional>
#include <netdb.h>  
#include <net/if.h>  
#include <arpa/inet.h>  
#include <sys/ioctl.h>  
#include <sys/types.h>  
#include <sys/socket.h> 

class ChannelManager;

class SockAddr
{
        Octets addr;
public: 
        SockAddr() {}
        template<typename T> SockAddr(const T &sa) : addr(&sa, sizeof(sa)) { }
        SockAddr(const SockAddr &rhs) : addr(rhs.addr) { }
        socklen_t GetLen() const { return addr.size(); }
        template<typename T> operator T* () { addr.resize(sizeof(T)); return (T *)addr.begin(); }
        template<typename T> operator const T* () const { return (const T *)addr.begin(); }
};

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
	bool ready_send;
public:
	void RegisteSendEvent();
};

class Exchanger : public Channel
{
public:
	Exchanger(int f, Parser *p, const void *data = nullptr, size_t size = 0)
	: Channel(f), parser(p), obuff(data, size), cur_cursor(0)
	{
		InitPeerName();
	}

	~Exchanger()
	{
		delete parser;
		parser = nullptr;
	}

protected:
	void Send(const void *buf, size_t size);
	void OnSend();
	void Recv();
	void OnRecv();
protected:
	Parser * parser;
	Mutex olock;
	Octets ibuff, obuff;
	enum
	{
		TMP_BUFF_SIZE = 512,
	};
	char tmp_buff[TMP_BUFF_SIZE];
	char ip[IP_SIZE] /*xxx.xxx.xxx.xxx*/;
	size_t cur_cursor;
private:
	void InitPeerName();
};

using ExchangerHatcher = Exchanger* (*) (int, const std::string &);
class Acceptor final : public Channel
{
public:
public:
	Acceptor(int f, ExchangerHatcher h, const std::string &p)
	: Channel(f), hatcher(h), param(p)
	{};

	static bool Listen(const char * ip, int port, ExchangerHatcher hatcher, const std::string &param = "");

protected:
	void Send(const void *, size_t) {}
	void OnSend() {}
	void Recv() {}
	void OnRecv();

	ExchangerHatcher hatcher;
	const std::string &param;
};

struct Connector
{
	static channel_id_t Connect(const std::string &ip, int port, ExchangerHatcher hatcher, const std::string &param = "");

	static bool GetIPByDomain(const char *domain, char *ip)
	{  
		char **pptr;  
		struct hostent *hptr;  

		hptr = gethostbyname(domain);  

		for(pptr = hptr->h_addr_list ; *pptr != NULL; pptr++)  
		{  
			if (NULL != inet_ntop(hptr->h_addrtype, *pptr, ip, IP_SIZE) )  
			{  
				return 0; // 只获取第一个 ip  
			}  
		}  
		LOG_ERROR("Connector::GetIPByDomain, gethostbyname error for host:%s", domain);  
		return false;
	}
};

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
		if(++cid == 0) cid = 1;
		return cid;
	}
	static ChannelManager &GetInstance() { static ChannelManager channel_manager; return channel_manager; }
};

#endif
