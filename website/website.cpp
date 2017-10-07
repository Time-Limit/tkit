#include "thread.h"
#include "websitetask.h"
#include "websiteparser.h"
#include "neter.h"
#include "channel.h"
#include "config.h"
#include <iostream>

int main()
{
	std::cout << &ConfigManager::GetInstance() << std::endl;
	assert(Acceptor::Listen("0.0.0.0", 9090, WebsiteParser::Hatcher));
	ThreadPool::GetInstance().AddTask(new WebsiteTask());
	
	ThreadPool::GetInstance().Start();

	return 0;
}
