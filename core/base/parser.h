#ifndef _PARSER_H_
#define _PARSER_H_

#include "octets.h"
#include "exptype.h"
#include <string>
#include <map>

class Parser
{
protected:
	Octets data;
	channel_id_t cid;
public:
	Parser(channel_id_t c)
	: cid(c)
	{}
	void Append(const Octets &fresh_data);
	virtual void Parse() = 0;
};

class HttpParser : Parser
{
public:
	struct Request
	{
		std::string method;
		std::string url;
		std::string version;
		std::map<std::string, std::string> headers;
		std::string body;
	};

	HttpParser(channel_id_t c)
	: Parser(c)
	{}

	void Parse();

	static Parser * Hatcher(channel_id_t cid) { return new HttpParser(cid); }
};

typedef Parser *(*ParserHatcher) (channel_id_t cid);

#endif
