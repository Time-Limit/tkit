#include "neter.h"
#include "thread.h"
#include "websitetask.h"
#include "enums.h"
#include "websitebase.h"
#include "basetool.h"
#include "file.h"
#include <stdlib.h>

using TCORE::ThreadPool;
using TCORE::Neter;

void WebsiteTask::Exec()
{
	signal(SIGPIPE, SIG_IGN);
	const ThreadPool *tp = &ThreadPool::GetInstance();
	if(tp->IsStart()) Neter::GetInstance().Wait(1000);
	else sleep(1);
	ThreadPool::GetInstance().AddTask(new WebsiteTask());
}

void SourceReq::LogicCheckRequest()
{
}

void SourceReq::ConstructResponse()
{
	except_status_code(response, HTTP_SC_OK);
	response.body = File(default_base_folder + request.url).GetContent();
	if(response.body.size() == 0)
	{
		response.body = File(default_base_folder + request.url + default_file_name).GetContent();
		if(response.body.size() > 0)
		{
			request.url += default_file_name;
		}
		else
		{
			ResetHttpResponseStatus(response, HTTP_SC_NOT_FOUND);
		}
	}
}

void OperateReq::LogicCheckRequest()
{
}

void OperateReq::ConstructResponse()
{
}

void RedirectReq::LogicCheckRequest()
{
	except_status_code(response, HTTP_SC_OK);

	const auto &it = request.headers.find(HTTP_HOST);
	if(it == request.headers.cend())
	{
		ResetHttpResponseStatus(response, HTTP_SC_BAD_REQUEST);
		return ;
	}
}

void RedirectReq::ConstructResponse()
{
	except_status_code(response, HTTP_SC_OK);

	ResetHttpResponseStatus(response, HTTP_SC_MOVED_PERMANENTLY);

	const std::string &host = request.headers[HTTP_HOST];
	size_t pos = host.find(':');
	if(pos == std::string::npos)
	{
		ForceSetHeader(response, HTTP_LOCATION, "https://" + host + ':' + tostring(default_https_port));
	}
	else
	{
		ForceSetHeader(response, HTTP_LOCATION, "https://" + host.substr(0, pos) + ':' + tostring(default_https_port));
	}
}
