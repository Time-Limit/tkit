#ifndef _PARSER_H_
#define _PARSER_H_

#include "octets.h"
#include "exptype.h"
#include <string>
#include <sstream>
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

		std::string Stream() const
		{
			std::stringstream streamer;
			std::string origin_url = url;
			if(args.size())
			{
				origin_url += '?';
				for(const auto &p : headers)
				{
					origin_url += p.first + "=" + p.second;
				}
			}
			streamer << method << " " << origin_url << " " << version << '\r' << '\n';
			for(const auto &p : headers)
			{
				streamer << p.first <<  ": " << p.second << '\r' << '\n';
			}

			return streamer.str();
		}
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
		std::string Stream() const
		{
			std::stringstream streamer;
			streamer << version <<  " " << status << " " << statement << '\r' << '\n';
			for(const auto &p : headers)
			{
				streamer << p.first <<  ": " << p.second << '\r' << '\n';
			}
			streamer << '\r' << '\n';
			streamer << body;
			return streamer.str();
		}
	};

	HttpParser()
	: Parser()
	{}

	void Parse(int64_t param);

	virtual Task* GenRequestTask(channel_id_t, Request &&req) = 0;
};

#endif
