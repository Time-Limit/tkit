#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

#include <sstream>
#include <string>
#include <map>

#include "exptype.h"

namespace TCORE
{

class SessionManager;
class OctetsStream;

class Protocol
{
public:
	virtual void Handle(SessionManager *manager, session_id_t sid) = 0;
	virtual OctetsStream& Deserialize(OctetsStream &os) = 0;
	virtual OctetsStream& Serialize(OctetsStream &os) const = 0;
	virtual ~Protocol() {}
};

struct HttpRequest : public Protocol
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

	virtual void Handle(SessionManager *manager, session_id_t sid) override {}
	virtual OctetsStream& Deserialize(OctetsStream &os) override;
	virtual OctetsStream& Serialize(OctetsStream &os) const override;
};

struct HttpResponse : public Protocol
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

	virtual void Handle(SessionManager *manager, session_id_t sid) override {}
	virtual OctetsStream& Deserialize(OctetsStream &os) override;
	virtual OctetsStream& Serialize(OctetsStream &os) const override;
};

}

#endif
