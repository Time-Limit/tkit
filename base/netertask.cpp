#include "neter.h"
#include "thread.h"
#include "netertask.h"
#include <stdlib.h>

void NeterTask::Exec()
{
	Neter::GetInstance().Wait(1000);
	ThreadPool::GetInstance().AddTask(this);
}

HttpTask::HttpTask(const Octets &o)
{
	size = o.size();
	data = (char *)malloc(size);
	memcpy(data, o.begin(), size);
}

HttpTask::~HttpTask()
{
	if(data)
	{
		free(data);
		data = NULL;
	}
}

void HttpTask::Parse()
{
	size_t cur = 0, end = size;

	while(cur < end)
	{
	}
}

void HttpTask::Exec()
{
	Parse();
}

HttpParse::HttpParse()
{
	trans[BEGIN].rules["GET"] = METHOD;
	trans[BEGIN].rules["POST"] = METHOD;

	trans[METHOD].rules[" "] = URL;

	trans[URL].rules[" "] = VERSION;

	trans[VERSION].rules["\r\n"] = HEADER_KEY;

	trans[HEADER_KEY].rules[":"] = HEADER_VALUE;

	trans[HEADER_VALUE].rules["\r\n"] = HEADER_KEY;
	trans[HEADER_VALUE].rules["\r\n\r\n"] = BODY;
}
