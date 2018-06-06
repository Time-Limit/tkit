#ifndef _TASK_H_
#define _TASK_H_

#include <sstream>
#include "session.h"
#include "protocol.h"

class Task
{
public:
	virtual flag_t GetFlag() { return flag_t(0); }
	virtual void Exec() = 0;
	virtual ~Task() = default;
};

class LogicTask : public Task
{
public:
	enum TASK_TYPE
	{
		TASK_TYPE_NORMAL,
		TASK_TYPE_COUNT,
	};
	
	LogicTask(TASK_TYPE t) : type(t) {}
	void Exec() = 0;

	TASK_TYPE GetType() const { return type; }
private:
	TASK_TYPE type;
};

class NormalTask : public LogicTask
{
public:
	NormalTask() : LogicTask(LogicTask::TASK_TYPE_NORMAL) {}
};

class HandleNetProtocolTask : public NormalTask
{
private:
	SessionManager *manager;
	session_id_t sid;
	Protocol *protocol;
public:

	virtual flag_t GetFlag() { return flag_t(sid); }

	void Exec();

	HandleNetProtocolTask(SessionManager *_manager, session_id_t _sid, Protocol *_protocol)
	: manager(_manager)
	, sid(_sid)
	, protocol(_protocol)
	{}
};

class ConnectTask : public NormalTask
{
private:
	SessionManager *manager;
	std::string ip;
	int port;
public:
	void Exec();

	ConnectTask(SessionManager *_manager, const char *_ip, int _port)
	: manager(_manager)
	, ip(_ip)
	, port(_port)
	{}
};

class HttpRequestTask : public NormalTask
{
	SessionManager *manager;
	session_id_t	sid;

protected:
	HttpRequest request;
	HttpResponse response;

public:
	HttpRequestTask(SessionManager * _manager, session_id_t _sid, const HttpRequest &req)
	: manager(_manager)
	, sid(_sid)
	, request(req)
	{}

	HttpRequestTask(const HttpRequestTask &) = default;
	HttpRequestTask& operator= (const HttpRequestTask &) = default;

	static bool IsValidEscapeChar(unsigned int);

protected:
	void BaseCheckRequest();
	virtual void ExtendBaseCheckRequest() {}
	virtual void LogicCheckRequest() {}
	virtual void ConstructResponse() {}
	virtual void ExtendCompleteResponse() {}
	void CompleteResponse();
public:
	virtual void Exec() final
	{
		BaseCheckRequest();
		ExtendBaseCheckRequest();

		LogicCheckRequest();
		ConstructResponse();

		ExtendCompleteResponse();
		CompleteResponse();
	}
};

#endif
