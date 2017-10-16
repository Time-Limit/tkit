#include "thread.h"
#include "websitetask.h"
#include "websiteparser.h"
#include "websitebase.h"
#include "neter.h"
#include "channel.h"
#include "config.h"
#include <iostream>

int main()
{
	ConfigManager::GetInstance().Reset({"website"});

	const Config &website_config = *ConfigManager::GetInstance().GetConfig("website");

	default_https_port = website_config["https-port"].Num();
	default_http_port = website_config["http-port"].Num();

	assert(Acceptor::Listen("0.0.0.0", default_https_port, WebsiteParser::Hatcher));
	assert(Acceptor::Listen("0.0.0.0", default_http_port, Website_80_Port_Parser::Hatcher));
	ThreadPool::GetInstance().AddTask(new WebsiteTask());
	
	ThreadPool::GetInstance().Start();

	return 0;
}
