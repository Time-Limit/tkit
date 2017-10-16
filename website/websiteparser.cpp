#include "websiteparser.h"
#include "websitetask.h"

Task* WebsiteParser::GenRequestTask(channel_id_t c, Request &&req)
{
	if(strncmp(req.url.c_str(), "/source", 6) == 0)
	{
		return new SourceReq(c, std::move(req));
	}
	else if(strncmp(req.url.c_str(), "/operate", 7) == 0)
	{
		return new OperateReq(c, std::move(req));
	}
	else if(strncmp(req.url.c_str(), "/favicon.ico", 12) == 0)
	{
		req.url = "/source/favicon.ico";
		return new OperateReq(c, std::move(req));
	}
	req.url = "/source/index.html";
	return new SourceReq(c, std::move(req));
}

Task* Website_80_Port_Parser::GenRequestTask(channel_id_t c, Request &&req)
{
	return new RedirectReq(c, std::move(req));
}
