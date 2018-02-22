#ifndef _TASK_H_
#define _TASK_H_

#include <sstream>
#include "session.h"
#include "protocol.h"

class Task
{
public:
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
	void Exec();

	HandleNetProtocolTask(SessionManager *_manager, session_id_t _sid, Protocol *_protocol)
	: manager(_manager)
	, sid(_sid)
	, protocol(_protocol)
	{}
};

class HttpRequestTask : public NormalTask
{
	channel_id_t cid;
protected:
	HttpRequest request;
	HttpResponse response;
public:
	HttpRequestTask(channel_id_t c, const HttpRequest &req)
	: cid(c)
	, request(req)
	{}

	HttpRequestTask(channel_id_t c, HttpRequest &&req)
	: cid(c)
	, request(std::move(req))
	{}

	HttpRequestTask(HttpRequestTask &&) = default;
	HttpRequestTask& operator= (HttpRequestTask &&) = default;
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

class HttpResponseTask : public NormalTask
{
public:
	using res_data_t = std::string;
	using res_stream_t = std::stringstream;
private:
	channel_id_t cid;
	HttpResponse response;
public:
	HttpResponseTask(channel_id_t c, const HttpResponse &res)
	: cid(c)
	, response(res)
	{}

	HttpResponseTask(channel_id_t c, HttpResponse &&res)
	: cid(c)
	, response(std::move(res))
	{}

	HttpResponseTask(HttpResponseTask &&) = default;
	HttpResponseTask& operator=(HttpResponseTask &&) = default;
	HttpResponseTask(const HttpResponseTask &) = default;
	HttpResponseTask& operator=(const HttpResponseTask &) = default;

	virtual void Exec() final;
};

#endif
