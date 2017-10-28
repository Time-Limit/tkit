#include "websiteparser.h"
#include "websitetask.h"
#include "log.h"

Task* WebsiteParser::GenRequestTask(channel_id_t c, Request &&req)
{
	LOG_TRACE("WebsiteParser::GenRequestTask, url=%s", req.url.c_str());
	if(strncmp(req.url.c_str(), "/operate", 7) == 0)
	{
		return new OperateReq(c, std::move(req));
	}
	else
	{
		return new SourceReq(c, std::move(req));
	}
}

Task* Website_HTTP_PORT_Parser::GenRequestTask(channel_id_t c, Request &&req)
{
	return new RedirectReq(c, std::move(req));
}

Task* Proxy_Port_Parser::GenRequestTask(channel_id_t c, Request &&req)
{
	return new ProxyRequestTask(c, std::move(req));
}
