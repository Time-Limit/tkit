#ifndef _NETERTASK_H_
#define _NETERTASK_H_

#include "task.h"
#include <set>
#include <string>

class WebsiteTask : public Task
{
public:
	virtual void Exec();
};

class StatusCodeRes : public HttpResponseTask
{
public:
	StatusCodeRes(channel_id_t c, unsigned short s, HttpParser::Request && req)
	: HttpResponseTask(c, res_data_t()), status(s), request(std::move(req)) {}

	StatusCodeRes(channel_id_t c, unsigned short s, const HttpParser::Request &req)
	: HttpResponseTask(c, res_data_t()), status(s), request(req) {}

	StatusCodeRes(const StatusCodeRes &) = default;
	StatusCodeRes& operator= (const StatusCodeRes &) = default;
	StatusCodeRes(StatusCodeRes &&) = default;
	StatusCodeRes& operator= (StatusCodeRes&&) = default;

	virtual void Exec() override;
private:
	unsigned short status;
	HttpParser::Request request;
};

class SourceReq : public HttpRequestTask
{
public:
	SourceReq(channel_id_t c, const HttpParser::Request &req)
	: HttpRequestTask(c, req) {}
	SourceReq(channel_id_t c, HttpParser::Request &&req)
	: HttpRequestTask(c, std::move(req)) {}

	SourceReq(const SourceReq &) = default;
	SourceReq& operator= (const SourceReq &) = default;
	SourceReq(SourceReq &&) = default;
	SourceReq& operator= (SourceReq&&) = default;

	virtual void Exec() override;
};

class OperateReq : public HttpRequestTask
{
public:
	OperateReq(channel_id_t c, const HttpParser::Request &req)
	: HttpRequestTask(c, req) {}
	OperateReq(channel_id_t c, HttpParser::Request &&req)
	: HttpRequestTask(c, std::move(req)) {}

	OperateReq(const OperateReq &) = default;
	OperateReq& operator= (const OperateReq &) = default;
	OperateReq(OperateReq &&) = default;
	OperateReq& operator= (OperateReq&&) = default;

	virtual void Exec() override;
};

#endif
