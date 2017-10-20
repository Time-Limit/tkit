#ifndef _TASK_H_
#define _TASK_H_

#include "parser.h"
#include <sstream>

class Connector;

class Task
{
public:
	virtual void Exec() = 0;
	virtual ~Task() = default;
public:

};

/*
 * HttpRequestTask Ê¹ÓÃËµÃ÷
 * 1. Ê×ÏÈ£¬ÔÚÀàÖ´ÐÐ³õÊ¼»¯Ê±£¬ÒÑ¾­ÓÐÁË¿Í»§¶ËµÄÇëÇó±¨ÎÄ¡£µ«ÊÇºÜ´Ö²Ú£¬²¢Ã»ÓÐ¶Ô±¨ÎÄ½øÐÐ¼ì²é¡£
 * 2. void Exec() { ... } ÖÐµ÷ÓÃÁË¸ÃÀàµÄÒ»Ð©º¯Êý¡£
 * 3. ÕâÐ©º¯Êý·Ö±ðÊÇ, BaseCheckRequest, ExtendBaseCheckRequest, LogicCheckRequest, ConsturctResponse, CompleteResponse, ExtendComplexteResponse¡£
 * 4. BaseCheckRequest : ¹ýÂË²»Ö§³ÖµÄÌØÐÔ£¬¼ì²éÊÇ·ñÓÐ»ù±¾µÄÇëÇóÍ·£¬¼ì²éurlÊÇ·ñºÏ·¨£¬¹¹ÔìÏìÓ¦ÐÐ¡£
 * 5. ExtendBaseCheckRequest : ¿ÉÓÉ×ÓÀà×Ô¼º²¹³ä¡£
 * 6. LogicCheckRequest: ¼ì²é¸ÃÇëÇóµÄÂß¼­ÊÇ·ñÕýÈ·¡£
 * 7. ConstructResponse: ¹¹ÔìÏìÓ¦±¨ÎÄ¡£
 * 8. CompleteResponse : ²¹³ä±ØÐèµÄÏìÓ¦×Ö¶Î¡£
 * 9. ExtendCompleteResponse : ¿ÉÓÉ×ÓÀà×Ô¼º²¹³ä¡£
 */

class HttpRequestTask : public Task
{
	channel_id_t cid;
protected:
	HttpParser::Request request;
	HttpParser::Response response;
public:
	HttpRequestTask(channel_id_t c, const HttpParser::Request &req)
	: cid(c)
	, request(req)
	{}

	HttpRequestTask(channel_id_t c, HttpParser::Request &&req)
	: cid(c)
	, request(std::move(req))
	{}

	HttpRequestTask(HttpRequestTask &&) = default;
	HttpRequestTask& operator= (HttpRequestTask &&) = default;
	HttpRequestTask(const HttpRequestTask &) = default;
	HttpRequestTask& operator= (const HttpRequestTask &) = default;

	static bool IsValidEscapeChar(unsigned int);
protected:
	void BaseCheckRequest();
	virtual void ExtendBaseCheckRequest() {}
	virtual void LogicCheckRequest() {}
	virtual void ConstructResponse() {}
	virtual void ExtendCompleteResponse() {}
	void CompleteResponse();
public:
	virtual void Exec() final
	{
		BaseCheckRequest();
		ExtendBaseCheckRequest();

		LogicCheckRequest();
		ConstructResponse();

		ExtendCompleteResponse();
		CompleteResponse();
	}
};

/*
 * HttpResponseTask Ê¹ÓÃËµÃ÷
 * ¸ÃÀà²¢Ã»ÓÐ×ö¹ý¶àµÄÊÂÇé, Ö»ÊÇ½«ÏìÓ¦±¨ÎÄÁ÷»¯²¢·ÅÈë·¢ËÍ¶ÓÁÐ¡£
 */

class HttpResponseTask : public Task
{
public:
	using res_data_t = std::string;
	using res_stream_t = std::stringstream;
private:
	channel_id_t cid;
	HttpParser::Response response;
public:
	HttpResponseTask(channel_id_t c, const HttpParser::Response &res)
	: cid(c)
	, response(res)
	{}

	HttpResponseTask(channel_id_t c, HttpParser::Response &&res)
	: cid(c)
	, response(std::move(res))
	{}

	HttpResponseTask(HttpResponseTask &&) = default;
	HttpResponseTask& operator=(HttpResponseTask &&) = default;
	HttpResponseTask(const HttpResponseTask &) = default;
	HttpResponseTask& operator=(const HttpResponseTask &) = default;

	virtual void Exec() final;
};

#endif
