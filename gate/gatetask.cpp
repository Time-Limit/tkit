#include "neter.h"
#include "thread.h"
#include "gatetask.h"
#include <stdlib.h>

void GateTask::Exec()
{
	Neter::GetInstance().Wait(1000);
	ThreadPool::GetInstance().AddTask(this);
}

HttpTask::HttpTask()
{
}

HttpTask::~HttpTask()
{
	if(data)
	{
		free(data);
		data = NULL;
	}
}

void HttpTask::Exec()
{
	Parse();
}

void HttpTask::Parse()
{
}

HttpRequestParser::MethodRule method_rule;
HttpRequestParser::UrlRule url_rule;
HttpRequestParser::VersionRule version_rule;
HttpRequestParser::HeaderKeyRule headerkey_rule;
HttpRequestParser::HeaderValueRule headervalue_rule;
HttpRequestParser::BodyRule body_rule;
HttpRequestParser::EndRule end_rule;
HttpRequestParser::RollbackRule rollback_rule;
HttpRequestParser::InvalidRule invalid_rule;

HttpRequestParser::HttpRequestParser()
{
}

bool HttpRequestParser::Parse(char * const begin, char * const end) const
{
}

Rule const * HttpRequestParser::MethodRule::Trans(char ** const cur, char * const end, Parser * parser) const
{
	char *ptr = *cur;
	HttpRequestParser * p = (HttpRequestParser *)parser;

	for(; ptr <= end && *ptr != ' '; ++ptr)
		;

	if(ptr > end)	
	{
		return &rollback_rule;
	}
	
	if(parser->SaveMethod(*cur, ptr))
	{
		*cur = ptr + 1;
		return &url_rule;
	}
	cur = ptr+1;
	return &invalid_rule;
}

Rule const * HttpRequestParser::UrlRule::Trans(char ** const cur, char * const end, Parser * parser) const
{
	char *ptr = *cur;
	HttpRequestParser * p = (HttpRequestParser *)parser;

	for(; ptr <= end && *ptr != ' '; ++ptr)
		;

	if(ptr > end)
	{
		return &rollback_rule;
	}

	if(parser->SaveUrl(*cur, ptr))
	{
		*cur = ptr+1;
		return &version_rule;
	}
	cur = ptr+1;
	return &invalid_rule;
}

Rule const * HttpRequestParser::UrlRule::Trans(char ** const cur, char * const end, Parser * parser) const
{
	char *ptr = *cur;
	HttpRequestParser * p = (HttpRequestParser *)parser;

	for(; ptr+1 <= end && (*ptr != '\r' || *(ptr+1) == '\n'); ++ptr)
		;

	if(ptr+1 > end)
	{
		return &rollback_rule;
	}

	if(parser->SaveVersion(*cur, ptr))
	{
		*cur = ptr+2;
		return &headerkey_rule;
	}
	cur = ptr+2;
	return &invalid_rule;
}


Rule const * HttpRequestParser::HeaderKeyRule::Trans(char ** const cur, char * const end, Parser * parser) const
{
	char *ptr = *cur;
	HttpRequestParser * p = (HttpRequestParser *)parser;

	for(; ptr <= end && *ptr != ':'; ++ptr)
		;

	if(ptr > end)
	{
		return &rollback_rule;
	}

	if(parser->SaveHeaderKey(*cur, ptr))
	{
		*cur = ptr+1;
		return &headervalue_rule;
	}
	cur = ptr+1;
	return &invalid_rule;
}

Rule const * HttpRequestParser::HeaderValueRule::Trans(char ** const cur, char * const end, Parser * parser) const
{
	char *ptr = *cur;
	HttpRequestParser * p = (HttpRequestParser *)parser;

	for(; ptr+1 <= end && (*ptr == '\r' || *(ptr+1) == '\n'); ++ptr)
		;

	if(ptr+1 > end)
	{
		return &rollback_rule;
	}
	
	return &invalid_rule;
}


