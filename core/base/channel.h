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
#include "openssl/ssl.h"
#include "openssl/err.h"

class Channel
{
public:
	Channel(int f);
	virtual ~Channel()
	{
		LOG_TRACE("Channel::~Channel, fd=%d, val=0x%p", fd, this);
		if(close(fd))
		{
			LOG_ERROR("Channel::~Channel, close failed, %d(%s)", errno, strerror(errno));
		}
	}
	void Handle(const epoll_event *event);
	channel_id_t ID() const { return fd; }
	bool IsClose() const { return ready_close; }
	void Close();
protected:
	virtual void Send(const void *buf, size_t size) = 0;
	virtual void OnSend() = 0;
	virtual void Recv() = 0;
	virtual void OnRecv() = 0;
protected:
	int fd;
	bool ready_close;
};

class Exchanger : public Channel
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

protected:
	void Send(const void *buf, size_t size);
	void OnSend();
	void Recv();
	void OnRecv();
	void RegisterSendEvent();
	void RegisterRecvEvent();

	void InitPeerName();

protected:
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

class SecureExchanger : public Exchanger
{
public:
	SecureExchanger(int f, Session *s, SSL *_ssl)
	: Exchanger(f, s)
	, is_handshake_finish(false)
	, is_init_ssl(false)
	, ssl(_ssl)
	{
	}

	void OnSend();
	void Recv();
private:
	bool is_handshake_finish;
	bool is_init_ssl;
	SSL *ssl;

private:
	bool Handshake();
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

#endif
