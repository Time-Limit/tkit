#include "thread.h"
#include "websitetask.h"
#include "websiteparser.h"
#include "neter.h"
#include "channel.h"

int main()
{
	assert(Acceptor::Listen("0.0.0.0", 9090, WebsiteParser::Hatcher));
	ThreadPool::GetInstance().AddTask(new WebsiteTask());
	
	ThreadPool::GetInstance().Start();

	return 0;
}
