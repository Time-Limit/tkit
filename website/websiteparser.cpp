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
	return new StatusCodeRes(c, 403, std::move(req));
}

Parser* WebsiteParser::Hatcher(channel_id_t c)
{
	return new WebsiteParser(c);
}
