#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

#include "exptype.h"
#include <string>
#include <sstream>
#include <map>

class SessionManager;

class Protocol
{
public:
	virtual void Handle(SessionManager *manager, session_id_t sid) = 0;
	virtual ~Protocol() {}
};

struct HttpRequest
{
	std::string method;
	std::string url;
	std::string version;
	std::map<std::string, std::string> headers;
	std::string body;
	std::map<std::string, std::string> args;

	HttpRequest() = default;
	HttpRequest(HttpRequest&&) = default;
	HttpRequest& operator=(HttpRequest&&) = default;
	HttpRequest(const HttpRequest &) = default;
	HttpRequest& operator=(const HttpRequest&) = default;
};

inline std::stringstream& operator<< (std::stringstream& ss, const HttpRequest &req)
{
	ss << req.method << " " << req.url << " " << req.version << "\r\n";

	for(const auto & header : req.headers)
	{
		ss << header.first << ": " << header.second << "\r\n";
	}

	ss << "\r\n";

	ss << req.body;

	return ss;
}

struct HttpResponse
{
	std::string version;
	unsigned short status;
	std::string statement;
	std::map<std::string, std::string> headers;
	std::string body;

	HttpResponse() = default;
	HttpResponse(HttpResponse&&) = default;
	HttpResponse& operator=(HttpResponse&&) = default;
	HttpResponse(const HttpResponse &) = default;
	HttpResponse& operator=(const HttpResponse&) = default;
};

inline std::stringstream& operator<< (std::stringstream &ss, const HttpResponse &res)
{
	ss << res.version << " " << res.status << " " << res.statement << "\r\n";
	for(const auto & header : res.headers)
	{
		ss << header.first << ": " << header.second << "\r\n";
	}

	ss << "\r\n";

	ss << res.body;

	return ss;
}

class HttpProtocol : public Protocol
{
private:
	HttpRequest req;
public:
	HttpProtocol(const HttpRequest &req)
	: req(req)
	{}
	virtual void Handle(SessionManager *manager, session_id_t sid);
};

#endif
