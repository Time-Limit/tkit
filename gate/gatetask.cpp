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
	Octets &data = connector->GetDataIn();
	printf("%.*s\n", data.size(), data.begin());
	data.clear();
	//Parse();
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
HttpRequestParser::InvalidRule invalid_rule;

HttpRequestParser::HttpRequestParser()
{
}

bool HttpRequestParser::Parse(char * const begin, char * const end) const
{
}

Rule const * HttpRequestParser::MethodRule::Trans(char ** const cur, char * const end, Parser * parser) const
{
	return NULL;
}

Rule const * HttpRequestParser::UrlRule::Trans(char ** const cur, char * const end, Parser * parser) const
{
	return NULL;
}

Rule const * HttpRequestParser::VersionRule::Trans(char ** const cur, char * const end, Parser * parser) const
{
	return NULL;
}

Rule const * HttpRequestParser::HeaderKeyRule::Trans(char ** const cur, char * const end, Parser * parser) const
{
	return NULL;
}

Rule const * HttpRequestParser::HeaderValueRule::Trans(char ** const cur, char * const end, Parser * parser) const
{
	return NULL;
}

Rule const * HttpRequestParser::BodyRule::Trans(char ** const cur, char * const end, Parser * parser) const
{
	return NULL;
}

Rule const * HttpRequestParser::EndRule::Trans(char ** const cur, char * const end, Parser * parser) const
{
	return NULL;
}

Rule const * HttpRequestParser::InvalidRule::Trans(char ** const cur, char * const end, Parser * parser) const
{
	return NULL;
}
