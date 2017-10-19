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
public:
	virtual ~Parser() = default;
	void Append(const Octets &fresh_data);
	virtual void Parse(int64_t param) = 0;
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
		std::string version;
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

	HttpParser()
	: Parser()
	{}

	void Parse(int64_t param);

	virtual Task* GenRequestTask(channel_id_t, Request &&req) = 0;
};

#endif
