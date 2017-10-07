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

class Task;

class HttpParser : public Parser
{
public:
	struct Request
	{
		std::string method;
		std::string url;
		std::string version;
		std::map<std::string, std::string> headers;
		std::string body;
		std::map<std::string, std::string> args;

		Request() = default;
		Request(Request&&) = default;
		Request& operator=(Request&&) = default;
		Request(const Request &) = default;
		Request& operator=(const Request&) = default;
	};

	struct Response
	{
		std::string version = "HTTP/1.1";
		unsigned short status;
		std::string statement;
		std::map<std::string, std::string> headers;
		std::string body;

		Response() = default;
		Response(Response&&) = default;
		Response& operator=(Response&&) = default;
		Response(const Response &) = default;
		Response& operator=(const Response&) = default;
	};

	HttpParser(channel_id_t c)
	: Parser(c)
	{}

	void Parse();

	virtual Task* GenRequestTask(channel_id_t, Request &&req) = 0;
};

typedef Parser *(*ParserHatcher) (channel_id_t cid);

#endif
