#include "thread.h"
#include "websitetask.h"
#include "websiteparser.h"
#include "neter.h"
#include "channel.h"
#include "config.h"
#include <iostream>

int main()
{

	ConfigManager::GetInstance().Reset({"website"});

	assert(Acceptor::Listen("0.0.0.0", 9090, WebsiteParser::Hatcher));
	assert(Acceptor::Listen("0.0.0.0", 9080, Website_80_Port_Parser::Hatcher));
	ThreadPool::GetInstance().AddTask(new WebsiteTask());
	
	ThreadPool::GetInstance().Start();

	return 0;
}
