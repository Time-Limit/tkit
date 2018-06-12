#ifndef _SESSION_H_
#define _SESSION_H_

#include <functional>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <memory>
#include <sys/types.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include "lock.h"
#include "octets.h"

namespace TCORE
{

class Session
{
private:
	virtual void NetRecv() = 0;
	virtual void NetSend() = 0;
	
public:
	Session(int f) : fd(f), close_flag(false) {}
	virtual ~Session() {}

	void Handle(const epoll_event *event);

	int get_fd() const { return fd; }

protected:
	bool IsClose() { return close_flag; }
	void Close();

	void RegisterRecvEvent();
	void RegisterSendEvent();

private:
	int fd;
	bool close_flag;
};

class ExchangeSession final: public Session
{
	Mutex send_buff_lock;
	Octets send_buff;

	Mutex recv_buff_lock;
	Octets recv_buff;

	virtual void NetRecv();
	virtual void NetSend();

	struct TypeBase
	{
		virtual void Seriable(Octets &data) = 0;
	};

	template<typename PROTOCOL>
	struct Type final : public TypeBase
	{
		typedef std::function<void (const PROTOCOL &p)> HandleFunc;

		HandleFunc handle;

		Type(HandleFunc h) : handle(h) {}

		void Seriable(Octets &data)
		{
			PROTOCOL p;
			handle(p);
			OctetsStream os(data);
			try
			{
				for(;;)
				{
					os >> OctetsStream::START >> p >> OctetsStream::COMMIT;
					handle(p);
					data = os.GetData();
				}
			}
			catch(...)
			{
				os >> OctetsStream::REVERT;
			}
		}
	};

	std::shared_ptr<TypeBase> type_base_ptr;
	enum
	{
		TMP_BUFF_SIZE = 1024*128,
	};
	char tmp_buff[TMP_BUFF_SIZE];
	size_t cur_cursor;

public:

	template<typename PROTOCOL>
	bool InitRecvHandle(std::function<void (const PROTOCOL &p)> handle)
	{
		if(type_base_ptr) { return false; }

		type_base_ptr.reset(new Type<PROTOCOL>(handle));

		return true;
	}

public:

	ExchangeSession(int fd)
	: Session(fd)
	, cur_cursor(0)
	{}

	void Send(const Octets &data)
	{
		MutexGuard guard(send_buff_lock);
		send_buff.insert(send_buff.end(), data.begin(), data.size());
	}
};

class AcceptSession final: public Session
{
public :
	typedef std::shared_ptr<ExchangeSession> ExchangeSessionPtr;
	typedef std::function<void (ExchangeSessionPtr)> AcceptHandle;

	AcceptSession(int fd, AcceptHandle ah)
	: Session(fd)
	, accept_handle(ah)
	{}

	virtual void NetRecv();
	virtual void NetSend() {}

	static bool Listen(const char *p, int port, AcceptHandle ah);
private:
	AcceptHandle accept_handle;
};

}

#endif
