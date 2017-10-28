#include "thread.h"
#include "websitetask.h"
#include "websiteparser.h"
#include "websitebase.h"
#include "neter.h"
#include "channel.h"
#include "config.h"
#include "file.h"
#include <iostream>

Exchanger* HatchExchangerWithWebsiteParser(int fd, const std::string& )
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
		p = nullptr;
		e = nullptr;
		LOG_TRACE("HatchExchangerWithWebsiteParser, failed, fd=%d", fd);
	}
	return e;
}

Exchanger* HatchExchangerWithWebsite_HTTP_PORT_Parser(int fd, const std::string&)
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
		p = nullptr;
		e = nullptr;
	        LOG_TRACE("HatchExchangerWithWebsiteParser, failed, fd=%d", fd);
	}
	return e;
}

Exchanger* HatchExchangerWithProxy_PROT_Parser(int fd, const std::string&)
{
	Parser *p = nullptr;
	Exchanger *e = nullptr;
	try
	{
	        p = new Proxy_Port_Parser();
	        e = new Exchanger(fd, p);
	}
	catch(...)
	{
	        delete p;
	        delete e;
		p = nullptr;
		e = nullptr;
	        LOG_TRACE("HatchExchangeWithWebsite_Proxy_PROT_Parser, failed, fd=%d", fd);
	}
	return e;
}

int main()
{
	ConfigManager::GetInstance().Reset({"website"});

	const Config &website_config = *ConfigManager::GetInstance().GetConfig("website");

	default_https_port = website_config["https-port"].Num();
	default_http_port = website_config["http-port"].Num();
	default_proxy_port = website_config["proxy-port"].Num();
	default_base_folder = website_config["base-folder"].Str();

	assert(Acceptor::Listen("0.0.0.0", default_https_port, HatchExchangerWithWebsiteParser));
	assert(Acceptor::Listen("0.0.0.0", default_http_port, HatchExchangerWithWebsite_HTTP_PORT_Parser));
	assert(Acceptor::Listen("0.0.0.0", default_proxy_port, HatchExchangerWithProxy_PROT_Parser));

	ThreadPool::GetInstance().AddTask(new WebsiteTask());
	
	ThreadPool::GetInstance().Start();

	return 0;
}
