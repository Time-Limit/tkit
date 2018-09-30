#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

#include <sstream>
#include <string>
#include <memory>
#include <vector>
#include <map>

#include "exptype.h"
#include "octets.h"

namespace TCORE
{

class OctetsStream;

class Protocol
{
public:
	virtual OctetsStream& Deserialize(OctetsStream &os) = 0;
	virtual OctetsStream& Serialize(OctetsStream &os) const = 0;
	virtual ~Protocol() {}
};

template<typename PACKET>
class HttpPacketVisitor
{
public:
	HttpPacketVisitor(PACKET &p) : packet(p) {}

	void SetVersion(const std::string &str) { packet.version = str; }
	void SetStatus(unsigned short s) { packet.status = s; }
	void SetStatement(const std::string &str) { packet.statement = str; }
	void SetHeader(const std::string &k, const std::string &v) { packet.headers[k] = v; }
	void SetBody(const std::string &body) { packet.body = body; }
	void SetMethod(const std::string &m) { packet.method = m; }
	void SetURL(const std::string &u) { packet.url = u; }
	void SetArgument(const std::string &k, const std::string &v) { packet.args[k] = v; }

	std::string GetURL() const { return packet.url; }

private:
	PACKET &packet;
};

class HttpRequest : public Protocol
{
	friend class HttpPacketVisitor<HttpRequest>;
	std::string method;
	std::string url;
	std::string version;
	std::map<std::string, std::string> headers;
	std::string body;
	std::map<std::string, std::string> args;

public:
	HttpRequest() = default;
	HttpRequest(HttpRequest&&) = default;
	HttpRequest& operator=(HttpRequest&&) = default;
	HttpRequest(const HttpRequest &) = default;
	HttpRequest& operator=(const HttpRequest&) = default;

	virtual OctetsStream& Deserialize(OctetsStream &os) override;
	virtual OctetsStream& Serialize(OctetsStream &os) const override;
};

class HttpResponse : public Protocol
{
	friend class HttpPacketVisitor<HttpResponse>;
	std::string version;
	unsigned short status;
	std::string statement;
	std::map<std::string, std::string> headers;
	std::string body;

public:
	HttpResponse() = default;
	HttpResponse(HttpResponse&&) = default;
	HttpResponse& operator=(HttpResponse&&) = default;
	HttpResponse(const HttpResponse &) = default;
	HttpResponse& operator=(const HttpResponse&) = default;

	virtual OctetsStream& Deserialize(OctetsStream &os) override;
	virtual OctetsStream& Serialize(OctetsStream &os) const override;
};

}

#endif
