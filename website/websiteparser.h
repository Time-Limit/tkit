#ifndef _WEBSITEPARSER_H_
#define _WEBSITEPARSER_H_

#include "parser.h"

class WebsiteParser : public HttpParser
{
public:
	WebsiteParser() : HttpParser() {}
	virtual Task* GenRequestTask(channel_id_t c, Request &&req) override;
};

class Website_HTTP_PORT_Parser:  public HttpParser
{
public:
	Website_HTTP_PORT_Parser() : HttpParser() {};
	virtual Task* GenRequestTask(channel_id_t c, Request &&req) override;
};

class Proxy_Port_Parser : public HttpParser
{
public:
	Proxy_Port_Parser() : HttpParser() {}
	virtual Task* GenRequestTask(channel_id_t c, Request &&req) override;
};

class Proxy_Connect_Parser : public HttpParser
{
public:
	Proxy_Connect_Parser(channel_id_t ) {}
	void Parse(int64_t param) {}
	Task * GenRequestTask(channel_id_t c, Request &&req) override { return nullptr; }
};

#endif
