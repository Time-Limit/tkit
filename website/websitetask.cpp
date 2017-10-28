#include "neter.h"
#include "thread.h"
#include "websitetask.h"
#include "enums.h"
#include "websitebase.h"
#include "basetool.h"
#include "file.h"
#include "websiteparser.h"
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
	except_status_code(response, HTTP_SC_OK);
	response.body = File(default_base_folder + request.url).GetContent();
	if(response.body.size() == 0)
	{
		ResetHttpResponseStatus(response, HTTP_SC_NOT_FOUND);
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
		ForceSetHeader(response, HTTP_LOCATION, "http://" + host + ':' + tostring(default_https_port));
	}
	else
	{
		ForceSetHeader(response, HTTP_LOCATION, "http://" + host.substr(0, pos) + ':' + tostring(default_https_port));
	}
}

void ProxyRequestTask::Exec()
{
	char ip[IP_SIZE] = {0};
	HttpParser::Response response;
	do
	{
		if(!Connector::GetIPByDomain(req.headers[HTTP_HOST].c_str(), ip))
		{
			LOG_ERROR("ProxyRequestTask::Exec, get ip failed, host=%s", req.headers[HTTP_HOST].c_str());
			ResetHttpResponseStatus(response, HTTP_SC_BAD_REQUEST);
			break;
		}
		
		std::stringstream streamer;
		streamer << origin_cid;
		streamer << req.Stream();
		channel_id_t goal = Connector::Connect(ip, 80,ProxyConnectHatcher, streamer.str());
		if(goal == INVALID_CHANNEL_ID)
		{
			LOG_ERROR("ProxyRequestTask::Exec, connect failed, host=%s, ip=%s", req.headers[HTTP_HOST].c_str(), ip);
			ResetHttpResponseStatus(response, HTTP_SC_BAD_REQUEST);
			break;
		}
	}while(0);

	if(response.status == HTTP_SC_OK)
	{
		return ;
	}

	std::string str = response.Stream();
	ChannelManager::GetInstance().Send(origin_cid, str.c_str(), str.size());
}

Exchanger * ProxyConnectHatcher(int fd, const std::string &param)
{
	Parser *p = nullptr;
	Exchanger *e = nullptr;
	try
	{
		std::stringstream streamer(param);
		channel_id_t origin_id = INVALID_CHANNEL_ID;
		streamer >> origin_id;
		std::string request;
		streamer >> request;
	        p = new Proxy_Connect_Parser(origin_id);
	        e = new Exchanger(fd, p, request.c_str(), request.size());
	}       
	catch(...)
	{
	        delete p;
	        delete e;               
	        p = nullptr;             
	        e = nullptr;      
	        LOG_TRACE("HatchExchangerWithWebsiteParser, failed, fd=%d", fd);
	}       
	return e;   
}
