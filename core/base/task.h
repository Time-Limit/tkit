#ifndef _TASK_H_
#define _TASK_H_

#include "parser.h"
#include <sstream>

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

class HttpRequestTask : public NormalTask
{
	channel_id_t cid;
protected:
	HttpParser::Request request;
	HttpParser::Response response;
public:
	HttpRequestTask(channel_id_t c, const HttpParser::Request &req)
	: cid(c)
	, request(req)
	{}

	HttpRequestTask(channel_id_t c, HttpParser::Request &&req)
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
	HttpParser::Response response;
public:
	HttpResponseTask(channel_id_t c, const HttpParser::Response &res)
	: cid(c)
	, response(res)
	{}

	HttpResponseTask(channel_id_t c, HttpParser::Response &&res)
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
