#ifndef _WEBSITEPARSER_H_
#define _WEBSITEPARSER_H_

#include "parser.h"

class WebsiteParser : public HttpParser
{
public:
	WebsiteParser() : HttpParser() {}
	virtual LogicTask* GenRequestTask(channel_id_t c, Request &&req) override;
};

class Website_HTTP_PORT_Parser:  public HttpParser
{
public:
	Website_HTTP_PORT_Parser() : HttpParser() {};
	virtual LogicTask* GenRequestTask(channel_id_t c, Request &&req) override;
};

#endif
