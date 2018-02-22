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

class LogicTask;

class HttpParser : public Parser
{
public:
	HttpParser()
	: Parser()
	{}

	void Parse(int64_t param);

	virtual LogicTask* GenRequestTask(channel_id_t, Request &&req) = 0;
};

#endif
