#include "neter.h"
#include "thread.h"
#include "websitetask.h"
#include "enums.h"
#include <stdlib.h>

void WebsiteTask::Exec()
{
	signal(SIGPIPE, SIG_IGN);
	const ThreadPool *tp = &ThreadPool::GetInstance();
	while(tp->IsStart())
	{
		Neter::GetInstance().Wait(1000);
	}
}

void SourceReq::LogicCheckRequest()
{
}

void SourceReq::ConstructResponse()
{
}

void OperateReq::LogicCheckRequest()
{
}

void OperateReq::ConstructResponse()
{
}

void RedirectReq::LogicCheckRequest()
{
	const auto &it = request.headers.find("Host");
	if(it == request.headers.cend())
	{
		ResetHttpResponseStatus(response, HTTP_SC_BAD_REQUEST);
		return ;
	}

	ResetHttpResponseStatus(response, HTTP_SC_MOVED_PERMANENTLY);

	const std::string &host = it->second;
	size_t pos = host.find(':');
	if(pos == std::string::npos)
	{
		response.headers["Location"] = host + request.url;
	}
	else
	{
		response.headers["Location"] = host.substr(0, pos) + request.url;
	}
}

void RedirectReq::ConstructResponse()
{
}
