#ifndef _TASK_H_
#define _TASK_H_

#include "parser.h"
#include <sstream>

class Connector;

class Task
{
public:
	virtual void Exec() = 0;
};

class HttpRequestTask : public Task
{
	channel_id_t cid;
	HttpParser::Request request;
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
public:
	void Exec() = 0;
};

class HttpResponseTask : public Task
{
public:
	using res_data_t = std::string;
	using res_stream_t = std::stringstream;
private:
	channel_id_t cid;
	res_data_t response;
public:
	HttpResponseTask(channel_id_t c, const res_data_t &res)
	: cid(c)
	, response(res)
	{}

	HttpResponseTask(channel_id_t c, res_data_t &&res)
	: cid(c)
	, response(std::move(res))
	{}

	HttpResponseTask(HttpResponseTask &&) = default;
	HttpResponseTask& operator=(HttpResponseTask &&) = default;
	HttpResponseTask(const HttpResponseTask &) = default;
	HttpResponseTask& operator=(const HttpResponseTask &) = default;

	void Exec() {};

	~HttpResponseTask();

	static res_data_t GenResData(const HttpParser::Response &);
};

#endif
