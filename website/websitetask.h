#ifndef _NETERTASK_H_
#define _NETERTASK_H_

#include "task.h"
#include <set>
#include <string>

#define except_status_code(res, sc) \
	if(((res).status) != (sc)) { return; }

using TCORE::NormalTask;
using TCORE::HttpRequestTask;
using TCORE::SessionManager;
using TCORE::HttpRequest;

class WebsiteTask : public NormalTask
{
public:
	virtual void Exec();
};

class SourceReq : public HttpRequestTask
{
public:
	SourceReq(SessionManager *manager, session_id_t sid, const HttpRequest &req)
	: HttpRequestTask(manager, sid, req) {}

	SourceReq(const SourceReq &) = default;
	SourceReq& operator= (const SourceReq &) = default;
protected:
	//virtual void ExtendBaseCheckRequest() override;
	virtual void LogicCheckRequest() override;
	virtual void ConstructResponse() override;
	//virtual void ExtendCompleteResponse() override;
};

class OperateReq : public HttpRequestTask
{
public:
	OperateReq(SessionManager *manager, session_id_t sid, const HttpRequest &req)
	: HttpRequestTask(manager, sid, req) {}

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

class RedirectReq : public HttpRequestTask
{
public:
	RedirectReq(SessionManager *manager, session_id_t sid, const HttpRequest &req)
	: HttpRequestTask(manager, sid, req) {}

	RedirectReq(const RedirectReq &) = default;
	RedirectReq& operator= (const RedirectReq &) = default;
	RedirectReq(RedirectReq &&) = default;
	RedirectReq& operator= (RedirectReq&&) = default;
protected:
	//virtual void ExtendBaseCheckRequest() override;
	virtual void LogicCheckRequest() override;
	virtual void ConstructResponse() override;
	//virtual void ExtendCompleteResponse() override;
};

#endif
