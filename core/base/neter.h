#ifndef _NETER_H_
#define _NETER_H_

#include <netinet/in.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <functional>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <memory>
#include <set>
#include <map>

#include "threadpool.h"
#include "exptype.h"
#include "octets.h"
#include "lock.h"
#include "log.h"

namespace TCORE
{

class Neter
{
	class Session;
	typedef std::shared_ptr<Session> SessionPtr;
	class Session
	{
		struct CallbackBase
		{
			virtual void Deseriable(SessionPtr, Octets &) = 0;
			virtual ~CallbackBase() {};
		};

		template<typename PROTOCOL>
		struct Callback : public CallbackBase
		{
			virtual void Deseriable(SessionPtr ptr, Octets &data) {}

			typedef std::function<void (const PROTOCOL &, SessionPtr)> CallbackFunc;
			bool InitCallback(CallbackFunc func)
			{
				//if(callback)
				//{
				//	Log::Error("Neter::Session::InitCallback, forbid reset");
				//	return false;
				//}
				callback = func;
				return true;
			}
		private:
			CallbackFunc callback;
		};
	public:
		enum EVENT_FLAG
		{
			READ_ACCESS = 0x1,
			READ_READY  = 0x2,
			WRITE_ACCESS= 0x4,
			WRITE_READY = 0x8,
			CLOSE_READY = 0x10,

			EVENT_FLAG_MASK = READ_ACCESS | READ_READY | WRITE_ACCESS | WRITE_READY | CLOSE_READY,
		};

		typedef unsigned char event_flag_t;

	private:
		event_flag_t event_flag;

	public:
		bool TestEventFlag(event_flag_t e) const
		{
			return (event_flag&e == e) && e;
		}
		void SetEventFlag(event_flag_t e)
		{
			(event_flag |= e) &= EVENT_FLAG_MASK;
		}
		void ClrEventFlag(event_flag_t e)
		{
			event_flag &= ((~e)&EVENT_FLAG_MASK);
		}

	private:
		typedef std::function<void ()> ReadFDFunc;
		ReadFDFunc read_fd_func;

		static void DefaultReadFDFunc()
		{
			//do nothing
		}

		static void AcceptorReadFDFunc()
		{
		}

		static void ExchangeReadFDFunc()
		{
		}

		static void SecureExchangeReadFDFunc()
		{
		}
	public:
		enum SESSION_TYPE
		{
			INVALID_SESSION = -1,
			ACCEPTOR_SESSION = 0,
			EXCHANGE_SESSION = 1,
			SECURE_EXCHANGE_SESSION = 2,
		};

		Session(SESSION_TYPE type, int fd);
		bool IsInitSuccess() const { return type != INVALID_SESSION; }

	private:
		int fd;
		SESSION_TYPE type;
	public:
		SESSION_TYPE GetType() const { return type; }
		int GetFD() const { return fd; }

		template<typename PROTOCOL>
		void InitCallback(std::function<void (const PROTOCOL &, SessionPtr)> callback) {}
	};
	typedef std::map<ptrdiff_t, SessionPtr> SessionContainer;
	SessionContainer session_container;
private:
	enum
	{
		POLL_TASK = 0,
		RDWR_TASK = 1,

		THREAD_COUNT = 2,
	};
	ThreadPool threadpool(THREAD_COUNT, ThreadPool::PT_XT_TO_XQ/*, [](task_id_t task_id, size_t thread_size)->size_t { return task_id; }*/);

	enum
	{
		EPOLL_EVENT_SIZE = 1024,
	};
	struct epoll_event events[EPOLL_EVENT_SIZE];

	int epoll_instance_fd;

	Neter()
	: epoll_instance_fd(epoll_create(1))
	{
		if(epoll_instance_fd == -1)
		{
			Log::Error("Neter::Neter, create epoll instance failed, info=", strerror(errno));
		}
	}

	bool Ctl(int op, int fd, struct epoll_event *event) { return true; }

public:
	void Wait(time_t timeout = 1000);

	template<typename PROTOCOL>
	static bool Listen(const char *ip, int port, std::function<void (const PROTOCOL &, SessionPtr)> callback);

	static Neter& GetInstance() { static Neter n; return n; }
	bool IsInitSuccess() { return epoll_instance_fd != -1; }
};

template<typename PROTOCOL>
bool Neter::Listen(const char *ip, int port, std::function<void (const PROTOCOL &, SessionPtr)> callback)
{
	if(!ip)
	{
		Log::Error("Neter::Listen, invalid ip address.");
		return false;
	}

	int sockfd = 0;
	int optval = -1;
	struct sockaddr_in server;
	socklen_t socklen = sizeof(struct sockaddr_in);

	sockfd = socket(PF_INET, SOCK_STREAM, 0);

	if(sockfd < 0)
	{
		Log::Error("Neter::Listen, socket, error=", strerror(errno));
		return false;
	}

	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
	memset(&server, 0, socklen);
	struct in_addr address;
	if(inet_pton(AF_INET, ip, &address) == -1)
	{
		Log::Error("Neter::inet_pton, ip=", ip, ", error=", strerror(errno));
		return false;
	}
	server.sin_addr.s_addr = address.s_addr;
	server.sin_port = htons(port);
	server.sin_family = AF_INET;

	if(bind(sockfd, (struct sockaddr *)&server, socklen) < 0)
	{
		Log::Error("Neter::Listen, bind, error=", strerror(errno));
		return false ;
	}

	listen(sockfd, 0);
	Log::Debug("Neter::Listen, fd=", sockfd);	

	epoll_event ev;
	ev.events = EPOLLIN|EPOLLET;
	SessionPtr ptr(new Session(Session::ACCEPTOR_SESSION, sockfd));
	ptr->InitCallback<PROTOCOL>(callback);
	Neter::GetInstance().session_container.insert(std::make_pair((ptrdiff_t)ptr.get(), ptr));
	ev.data.ptr = ptr.get();
	Neter::GetInstance().Ctl(EPOLL_CTL_ADD, ptr->GetFD(), &ev);

	return true;
}

}

namespace
{
struct NeterInit
{
	NeterInit()
	{
		TCORE::Neter::GetInstance();
	}
};
NeterInit _neter_init_;
}

#endif
