#include "thread.h"
#include "websitetask.h"
#include "websiteparser.h"
#include "websitebase.h"
#include "neter.h"
#include "channel.h"
#include "config.h"
#include "file.h"
#include "md5.h"
#include <iostream>

Exchanger* HatchExchangerWithWebsiteParser(int fd)
{
	Parser *p = nullptr;
	Exchanger *e = nullptr;
	try
	{
		p = new WebsiteParser();
		e = new Exchanger(fd, p);
	}
	catch(...)
	{
		delete p;
		delete e;
		LOG_TRACE("HatchExchangerWithWebsiteParser, failed, fd=%d", fd);
	}
	return e;
}

Exchanger* HatchExchangerWithWebsite_HTTP_PORT_Parser(int fd)
{
	Parser *p = nullptr;
	Exchanger *e = nullptr;
	try
	{
	        p = new Website_HTTP_PORT_Parser();
	        e = new Exchanger(fd, p);
	}
	catch(...)
	{
	        delete p;
	        delete e;
	        LOG_TRACE("HatchExchangerWithWebsiteParser, failed, fd=%d", fd);
	}
	return e;
}

int main(int argc, char **argv)
{
	ConfigManager::GetInstance().Reset({"website"});

	const Config &website_config = *ConfigManager::GetInstance().GetConfig("website");

	default_https_port = website_config["https-port"].Num();
	default_http_port = website_config["http-port"].Num();
	default_base_folder = website_config["base-folder"].Str();

	assert(Acceptor::Listen("0.0.0.0", default_https_port, HatchExchangerWithWebsiteParser));
	assert(Acceptor::Listen("0.0.0.0", default_http_port, HatchExchangerWithWebsite_HTTP_PORT_Parser));
	ThreadPool::GetInstance().AddTask(new WebsiteTask());
	
	ThreadPool::GetInstance().Start();

	return 0;
}
