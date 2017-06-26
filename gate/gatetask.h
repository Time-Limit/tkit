#ifndef _NETERTASK_H_
#define _NETERTASK_H_

#include "task.h"
#include <set>
#include <string>
#include "parser.h"

class GateTask : public Task
{
public:
	virtual void Exec();
};

class HttpTask : public Task
{
public:
	HttpTask();
	~HttpTask();
	virtual void Exec();
	
	struct Header
	{
		std::string key, value;

		bool operator < (const Header &rhs) const
		{
			return key < rhs.key;
		}
	};

	typedef std::set<Header> Headers;

	std::string method, url, version, body;

private:
	char * data;
	size_t size;

	void Parse();
};

class HttpRequestParser : public Parser
{
public:
	typedef std::map<std::string, std::string> Headers;

	HttpRequestParser();

 	virtual bool Parse(char * const begin, char * const end) const;
public:
	class MethodRule : public Rule
	{
	public:
		virtual Rule const * Trans(char ** const cur, char * const end, Parser * parser) const;
	};

	class UrlRule : public Rule
	{
	public:
		virtual Rule const * Trans(char ** const cur, char * const end, Parser * parser) const;
	};

	class VersionRule : public Rule
	{
	public:
		virtual Rule const * Trans(char ** const cur, char * const end, Parser * parser) const;
	};

	class HeaderKeyRule : public Rule
	{
	public:
		virtual Rule const * Trans(char ** const cur, char * const end, Parser * parser) const;
	};

	class HeaderValueRule : public Rule
	{
	public:
		virtual Rule const * Trans(char ** const cur, char * const end, Parser * parser) const;
	};

	class BodyRule : public Rule
	{
	public:
		virtual Rule const * Trans(char ** const cur, char * const end, Parser * parser) const;
	};

	class EndRule : public Rule
	{
	public:
		virtual Rule const * Trans(char ** const cur, char * const end, Parser * parser) const;
	};

	class RollbackRule : public Rule
	{
	public:
		virtual Rule const * Trans(char ** const cur, char * const end, Parser * parser) const;
	};

	class InvalidRule : public Rule
	{
	public:
		virtual Rule const * Trans(char ** const cur, char * const end, Parser * parser) const;
	};

	static const MethodRule method_rule;
	static const UrlRule url_rule;
	static const VersionRule version_rule;
	static const HeaderKeyRule headerkey_rule;
	static const HeaderValueRule headervalue_rule;
	static const BodyRule body_rule;
	static const EndRule end_rule;
	static const RollbackRule rollback_rule;
	static const InvalidRule invalid_rule;
private:
	std::string method;
	std::string url;
	std::string version;
	Headers headers;
	std::string body;
};

#endif
