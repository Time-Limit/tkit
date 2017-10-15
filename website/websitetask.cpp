#include "neter.h"
#include "thread.h"
#include "websitetask.h"
#include <stdlib.h>

void WebsiteTask::Exec()
{
	signal(SIGPIPE, SIG_IGN);
	const ThreadPool *tp = &ThreadPool::GetInstance();
	while(tp->IsStart())
	{
		Neter::GetInstance().Wait(1000);
	}
}

void SourceReq::LogicCheckRequest()
{
}

void SourceReq::ConstructResponse()
{
}

void OperateReq::LogicCheckRequest()
{
}

void OperateReq::ConstructResponse()
{
}
