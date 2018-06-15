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
	friend class Session;
	class Session
	{
		friend class Neter;

		// BASE DATA/METHOD BEGIN
		
		enum SESSION_TYPE
		{
			INVALID_SESSION = -1,

			ACCEPTOR_SESSION = 0,
			CONNECTOR_SESSION,
			EXCHANGE_SESSION,
			SECURE_EXCHANGE_SESSION,
		};

		Session(session_id_t sid, SESSION_TYPE type, int fd);
	public:
		~Session();
	private:
		void Close();

		session_id_t sid;
		int fd;
		SESSION_TYPE type;

		bool IsExchangeSession() const { return type == EXCHANGE_SESSION || type == SECURE_EXCHANGE_SESSION; }
		bool IsInitSuccess() const { return type != INVALID_SESSION; }

		SESSION_TYPE GetType() const { return type; }
		int GetFD() const { return fd; }
		session_id_t GetSID() const { return sid; }

		// BASE DATA/METHOD END

		//CALLBACK PART BEGIN
		
		struct CallbackBase
		{
			virtual void Deserialize(session_id_t, Octets &) = 0;
			virtual ~CallbackBase() {};
		};

		template<typename PROTOCOL>
		struct Callback : public CallbackBase
		{
			virtual void Deserialize(session_id_t sid, Octets &data) override;
			typedef std::function<void (const PROTOCOL &, session_id_t)> CallbackFunc;
			explicit Callback(CallbackFunc cb) : callback(cb) {}
			explicit Callback(const Callback &c) : callback(c.callback) {}

		private:
			CallbackFunc callback;
		};

		typedef std::shared_ptr<CallbackBase> CallbackPtr;
		CallbackPtr callback_ptr;

		template<typename CALLBACKPTR>
		void InitCallback(CALLBACKPTR cb);

		template<typename PROTOCOL>
		void InitCallback(std::function<void (const PROTOCOL &, session_id_t)> callback);

		//CALLBACK PART END

		//HANDLE READ/WRITE PART BEGIN
		
		enum EVENT_FLAG
		{
			READ_ACCESS	= 0x1,
			READ_PENDING	= 0x2,
			WRITE_ACCESS	= 0x4,
			WRITE_READY	= 0x8,
			WRITE_PENDING	= 0x10,
			CLOSE_READY	= 0x20,

			EMPTY_EVENT_FLAG = 0x0,
			EVENT_FLAG_MASK = READ_ACCESS | READ_PENDING | WRITE_ACCESS | WRITE_READY | WRITE_PENDING | CLOSE_READY,
		};

		mutable SpinLock event_flag_lock;
		typedef unsigned char event_flag_t;
		event_flag_t event_flag;

		bool TestAndModifyEventFlag(event_flag_t test, event_flag_t except, event_flag_t set, event_flag_t clear);
		bool TestEventFlag(event_flag_t e) const;
		void SetEventFlag(event_flag_t e);
		void ClrEventFlag(event_flag_t e);

		static void Read(SessionPtr ptr);
		Octets read_data;

		static void Write(SessionPtr ptr);
		typedef std::list<Octets> SendDataList;
		SendDataList send_data_list;
		mutable SpinLock send_data_list_lock;
		size_t cursor_of_first_send_data;
		static bool AppendSendData(SessionPtr ptr, const Octets &data);

		typedef void (Session::*InnerReadFuncPtr)();
		InnerReadFuncPtr read_func_ptr;
		void DefaultReadFunc() { Log::Error("Session::DefaultReadFunc, you should never call me."); }
		void AcceptorReadFunc();
		void ExchangerReadFunc();
		void SecureExchangerReadFunc();

		typedef void (Session::*InnerWriteFuncPtr)();
		InnerWriteFuncPtr write_func_ptr;
		void DefaultWriteFunc() { Log::Error("Session::DefaultWriteFunc, you should never call me."); }
		void ExchangerWriteFunc();
		void ConnectorWriteFunc();

		//HANDLE READ/WRITE PART END
	};
	// session end

private:
	struct SessionWriteTask : public Task
	{
		SessionWriteTask(SessionPtr p) : Task(RDWR_TASK),  ps(p) {} 
		void Exec() { Session::Write(ps); }
	private:
		SessionPtr ps;
	};

	struct SessionReadTask : public Task
	{
		SessionReadTask(SessionPtr p) : Task(RDWR_TASK), ps(p) {}
		void Exec() { Session::Read(ps); }
	private:
		SessionPtr ps;
	};

	struct NeterPollTask : public Task
	{
		NeterPollTask() : Task(POLL_TASK) {}
		void Exec() { while(true) { Neter::GetInstance().Wait(); } }
	};

	static void TryAddReadTask(SessionPtr ptr);
	static void TryAddWriteTask(SessionPtr ptr);

	mutable SpinLock session_container_lock;
	typedef std::map<session_id_t, SessionPtr> SessionContainer;
	SessionContainer session_container;

	SessionPtr GetSession(session_id_t sid) const;

	typedef std::list<session_id_t> SessionKeyList;
	SessionKeyList ready_close_session_list;
	SpinLock ready_close_session_list_lock;

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

	Neter();
	~Neter();

	bool Ctrl(int op, int fd, struct epoll_event *event);

	void Wait(time_t timeout = 1000);

	SpinLock session_id_spawner_lock;
	session_id_t session_id_spawner;

	session_id_t GenerateSessionID() { SpinLockGuard guard(session_id_spawner_lock); return ++session_id_spawner; }

public:

	void AddReadyCloseSession(session_id_t sid)
	{
		SpinLockGuard guard(ready_close_session_list_lock);
		ready_close_session_list.push_back(sid);
	}

	template<typename PROTOCOL>
	static bool Listen(const char *ip, int port, std::function<void (const PROTOCOL &, session_id_t)> callback);

	typedef std::function<void (session_id_t sid)> ConnectCallback;
	template<typename PROTOCOL>
	static bool Connect(const char *ip, int port, ConnectCallback dcb, std::function<void (const PROTOCOL &, session_id_t)> callback);

	template<typename PROTOCOL>
	static bool SendProtocol(session_id_t sid, const PROTOCOL &protocol);

	static Neter& GetInstance() { static Neter n; return n; }
	bool IsInitSuccess() { return epoll_instance_fd != -1; }
};

template<typename PROTOCOL>
bool Neter::SendProtocol(session_id_t sid, const PROTOCOL &protocol)
{
	try
	{
		SessionPtr ptr = Neter::GetInstance().GetSession(sid);
		if(!ptr)
		{
			Log::Error("Neter::SendProtocol, sid=", sid, " ,session not found !!!");
			return false;
		}
		OctetsStream os;
		os << protocol;
		bool res = Session::AppendSendData(ptr, os.GetData());
		TryAddWriteTask(ptr);
		return res;
	}
	catch(...)
	{
		Log::Error("Neter::SendProtocol, sid=", sid, " ,serialize failed !!!");
	}

	return false;
}

template<typename PROTOCOL>
bool Neter::Connect(const char *ip, int port, ConnectCallback connect_callback, std::function<void (const PROTOCOL &, session_id_t)> callback)
{
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock < 0)
	{
		Log::Error("Neter::Connect, ip=", ip, " ,port=", port, " ,info=", strerror(errno));
		return false;
	}

	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = inet_addr(ip);

	if(connect(sock, (sockaddr*)&addr, sizeof(addr)) < 0)
	{
		Log::Error("Neter::Connect, ip=", ip, " ,port=", port, ", connect failed, info=", strerror(errno));
		close(sock);
		return false;
	}

	fcntl(sock, F_SETFL, fcntl(sock, F_GETFL) | O_NONBLOCK);

	SessionPtr ptr(new Session(Neter::GetInstance().GenerateSessionID(), Session::EXCHANGE_SESSION, sock));
	ptr->InitCallback(callback);
	epoll_event ev;
	ev.events = EPOLLIN|EPOLLOUT|EPOLLET;
	ev.data.u64 = ptr->GetSID();

	Neter::GetInstance().session_container.insert(std::make_pair(ptr->GetSID(), ptr));
	Neter::GetInstance().Ctrl(EPOLL_CTL_ADD, ptr->GetFD(), &ev);
	connect_callback(ptr->GetSID());
	return true;
}

template<typename PROTOCOL>
bool Neter::Listen(const char *ip, int port, std::function<void (const PROTOCOL &, session_id_t sid)> callback)
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
	SessionPtr ptr(new Session(Neter::GetInstance().GenerateSessionID(), Session::ACCEPTOR_SESSION, sockfd));
	ptr->InitCallback<PROTOCOL>(callback);
	Neter::GetInstance().session_container.insert(std::make_pair(ptr->GetSID(), ptr));
	ev.data.u64 = ptr->GetSID();
	Neter::GetInstance().Ctrl(EPOLL_CTL_ADD, ptr->GetFD(), &ev);

	return true;
}

template<typename CALLBACKPTR>
void Neter::Session::InitCallback(CALLBACKPTR cb)
{
	if(callback_ptr.get())
	{
		Log::Error("Neter::Session::InitCallback, forbid reset callback!");
		return ;
	}

	callback_ptr = cb;
}

template<typename PROTOCOL>
void Neter::Session::InitCallback(std::function<void (const PROTOCOL &, session_id_t)> callback)
{
	if(callback_ptr.get())
	{
		Log::Error("Neter::Session::InitCallback, forbid reset callback!");
		return ;
	}
	callback_ptr.reset(new Callback<PROTOCOL>(callback));
}

template<typename PROTOCOL>
void Neter::Session::Callback<PROTOCOL>::Deserialize(session_id_t sid, Octets &data)
{
	OctetsStream os(data);
	PROTOCOL p;
	try
	{
		for(;;)
		{
			os >> OctetsStream::START >> p >> OctetsStream::COMMIT;
			callback(p, sid);
			Log::Trace("Neter::Session::Callback, deserialize success !!!");
		}
	}
	catch(...)
	{
		Log::Trace("Neter::Session::Callback, deserialize throw exception !!!");
		os >> OctetsStream::REVERT;
	}

	data = os.GetData();
}

}

namespace
{
struct NeterInit
{
	NeterInit()
	{
		if(TCORE::Neter::GetInstance().IsInitSuccess() == false)
		{
			throw "init neter failed !!!";
		}
	}
};
NeterInit _neter_init_;
}

#endif
