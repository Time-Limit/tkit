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
protected:
	//virtual void ExtendBaseCheckRequest() override;
	virtual void LogicCheckRequest() override;
	virtual void ConstructResponse() override;
	//virtual void ExtendCompleteResponse() override;
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
protected:
	//virtual void ExtendBaseCheckRequest() override;
	virtual void LogicCheckRequest() override;
	virtual void ConstructResponse() override;
	//virtual void ExtendCompleteResponse() override;
};

#endif