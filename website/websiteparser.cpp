#include "websiteparser.h"
#include "websitetask.h"

Task* WebsiteParser::GenRequestTask(channel_id_t c, Request &&req)
{
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
