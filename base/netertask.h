#ifndef _NETERTASK_H_
#define _NETERTASK_H_

#include "task.h"
#include <set>
#include <string>

class NeterTask : public Task
{
public:
	virtual void Exec();
};

class HttpTask : public Task
{
public:
	HttpTask(const Octets &o);
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

class HttpParse
{
public:
	enum STATUS
	{
		INVALID = 0,

		ROLLBACK,

		BEGIN,
		METHOD,
		URL,
		VERSION,
		HEADER_KEY,
		HEADER_VALUE,
		BODY,
		END,

		COUNT,
	};
	struct Trans
	{
		typedef std::map<std::string, STATUS> RULES;

		RULES rules;
	};
	HttpParse();
private:
	Trans trans[STATUS::COUNT];
};

#endif
