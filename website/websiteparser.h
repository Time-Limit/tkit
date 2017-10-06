#ifndef _WEBSITEPARSER_H_
#define _WEBSITEPARSER_H_

#include "parser.h"

class WebsiteParser : public HttpParser
{
public:
	WebsiteParser(channel_id_t c) : HttpParser(c) {}
	virtual Task* GenRequestTask(channel_id_t c, Request &&req) override;

	static Parser* Hatcher(channel_id_t c);
};

#endif
