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
public:
	typedef std::shared_ptr<Session> SessionPtr;
private:
	class Session
	{
		struct CallbackBase
		{
			virtual void Deserialize(SessionPtr, Octets &) = 0;
			virtual ~CallbackBase() {};
		};

		template<typename PROTOCOL>
		struct Callback : public CallbackBase
		{
			virtual void Deserialize(SessionPtr ptr, Octets &data) override
			{
				OctetsStream os(data);
				PROTOCOL p;
				try
				{
					for(;;)
					{
						os >> OctetsStream::START >> p >> OctetsStream::COMMIT;
						callback(p, ptr);
						Log::Trace("Neter::Session::Callback, serialize success !!!");
					}
				}
				catch(...)
				{
					os >> OctetsStream::REVERT;
				}

				data = os.GetData();
			}

			typedef std::function<void (const PROTOCOL &, SessionPtr)> CallbackFunc;

			explicit Callback(CallbackFunc cb) : callback(cb) {}
			Callback(const Callback &c) : callback(c.callback) {}

		private:
			CallbackFunc callback;
		};

		typedef std::shared_ptr<CallbackBase> CallbackPtr;
		CallbackPtr callback_ptr;

	public:
		template<typename CALLBACKPTR>
		void InitCallback(CALLBACKPTR cb)
		{
			if(callback_ptr.get())
			{
				Log::Error("Neter::Session::InitCallback, forbid reset callback!");
				return ;
			}

			callback_ptr = cb;
		}

		template<typename PROTOCOL>
		void InitCallback(std::function<void (const PROTOCOL &, SessionPtr)> callback)
		{
			if(callback_ptr.get())
			{
				Log::Error("Neter::Session::InitCallback, forbid reset callback!");
				return ;
			}
			callback_ptr.reset(new Callback<PROTOCOL>(callback));
		}

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

		static void NotifyReadAccess(SessionPtr);
		static void NotifyWriteAccess(SessionPtr);

		void Read(SessionPtr ptr);
		void Write(SessionPtr ptr);
		void Send(const Octets &data);

	private:
		mutable SpinLock event_flag_lock;
		event_flag_t event_flag;

		Octets read_data;

		typedef std::list<Octets> SendDataList;
		SendDataList send_data_list;
		mutable SpinLock send_data_list_lock;

	public:
		bool TestAndSetEventFlag(event_flag_t t, event_flag_t except = 0)
		{
			SpinLockGuard guard(event_flag_lock);
			if((event_flag&t) == except)
			{
				(event_flag |= t) &= EVENT_FLAG_MASK;
				return true;
			}
			return false;
		}

		bool TestEventFlag(event_flag_t e) const
		{
			SpinLockGuard guard(event_flag_lock);
			return ((event_flag&e) == e) && e;
		}
		void SetEventFlag(event_flag_t e)
		{
			SpinLockGuard guard(event_flag_lock);
			(event_flag |= e) &= EVENT_FLAG_MASK;
		}
		void ClrEventFlag(event_flag_t e)
		{
			SpinLockGuard guard(event_flag_lock);
			event_flag &= ((~e)&EVENT_FLAG_MASK);
		}

	private:
		typedef void (Session::*InnerReadFuncPtr)();
		InnerReadFuncPtr read_func_ptr;

		void DefaultReadFunc()
		{
			Log::Error("Session::DefaultReadFunc, you should never call me.");
		}

		void AcceptorReadFunc();
		void ExchangerReadFunc();
		void SecureExchangerReadFunc();

		typedef void (Session::*InnerWriteFuncPtr)();
		InnerWriteFuncPtr write_func_ptr;

		void DefaultWriteFunc()
		{
			Log::Error("Session::DefaultWriteFunc, you should never call me.");
		}

	public:
		void Close();

		enum SESSION_TYPE
		{
			INVALID_SESSION = -1,
			ACCEPTOR_SESSION = 0,
			EXCHANGE_SESSION = 1,
			SECURE_EXCHANGE_SESSION = 2,
		};

		Session(SESSION_TYPE type, int fd);
		~Session()
		{
			int res = close(fd);
			if(res == -1)
			{
				Log::Error("Session::~Session, close fd failed, info=", strerror(errno));
			}
			else
			{
				Log::Debug("Sesssion::~Session, close success!");
			}
		}
		bool IsInitSuccess() const { return type != INVALID_SESSION; }

	private:
		int fd;
		SESSION_TYPE type;

		bool IsExchangeSession() const
		{
			return type == EXCHANGE_SESSION || type == SECURE_EXCHANGE_SESSION;
		}

	public:
		SESSION_TYPE GetType() const { return type; }
		int GetFD() const { return fd; }
	};

	struct SessionWriteTask : public Task
	{
		SessionWriteTask(SessionPtr p) : Task(RDWR_TASK),  ps(p) {} 

		void Exec()
		{
			ps->ClrEventFlag(Session::WRITE_READY);
			ps->Write(ps);
		}

	private:
		SessionPtr ps;
	};

	struct SessionReadTask : public Task
	{
		SessionReadTask(SessionPtr p) : Task(RDWR_TASK), ps(p) {}

		void Exec()
		{
			ps->ClrEventFlag(Session::READ_READY);
			ps->Read(ps);
		}

	private:
		SessionPtr ps;
	};

	struct NeterPollTask : public Task
	{
		NeterPollTask() : Task(POLL_TASK) {}

		void Exec()
		{
			//TODO zmx
			while(true)
			{
				Neter::GetInstance().Wait();
			}

		}
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

	ThreadPool threadpool;

	enum
	{
		EPOLL_EVENT_SIZE = 1024,
	};
	struct epoll_event events[EPOLL_EVENT_SIZE];

	int epoll_instance_fd;

	Neter()
	: threadpool(THREAD_COUNT, ThreadPool::PT_XT_TO_XQ, [](task_id_t task_id, size_t thread_size)->size_t { return task_id; })
	, epoll_instance_fd(epoll_create(1))
	{
		if(epoll_instance_fd == -1)
		{
			Log::Error("Neter::Neter, create epoll instance failed, info=", strerror(errno));
			return ;
		}
		if(!threadpool.AddTask(TaskPtr(new NeterPollTask())))
		{
			Log::Error("Neter::Neter, add poll task failed!!!");
		}
	}

	~Neter()
	{
		if(epoll_instance_fd != -1)
		{
			int res = close(epoll_instance_fd);
			if(res == -1)
			{
				Log::Error("Neter::~Neter, close epoll instance failed, info=", strerror(errno));
			}
		}
	}

	bool Ctrl(int op, int fd, struct epoll_event *event);

	typedef std::list<ptrdiff_t> SessionKeyList;
	SessionKeyList ready_close_session_list;
	SpinLock ready_close_session_list_lock;

public:

	void AddReadyCloseSession(ptrdiff_t session_addr)
	{
		SpinLockGuard guard(ready_close_session_list_lock);
		ready_close_session_list.push_back(session_addr);
	}

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
	fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFL) | O_NONBLOCK);
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
	Neter::GetInstance().session_container.insert(std::make_pair((ptrdiff_t)(ptr.get()), ptr));
	ev.data.ptr = ptr.get();
	Neter::GetInstance().Ctrl(EPOLL_CTL_ADD, ptr->GetFD(), &ev);

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
