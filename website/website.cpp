#include "thread.h"
#include "websitetask.h"
#include "websitebase.h"
#include "neter.h"
#include "channel.h"
#include "config.h"
#include "file.h"
#include "md5.h"
#include "httpssession.h"
#include <iostream>

int main(int argc, char **argv)
{
	ConfigManager::GetInstance().Reset({"website"});

	const Config &website_config = *ConfigManager::GetInstance().GetConfig("website");

	default_https_port = website_config["https-port"].Num();
	default_http_port = website_config["http-port"].Num();
	default_base_folder = website_config["base-folder"].Str();
	default_file_name = website_config["default-file-name"].Str();
	

	//HttpsSessionManager *https_session_manager = new HttpsSessionManager();
	//assert(https_session_manager->InitSSLData(website_config));
	//assert(Acceptor::Listen("0.0.0.0", default_https_port, *https_session_manager));
	HttpSessionManager *http_session_manager = new HttpSessionManager();
	assert(Acceptor::Listen("0.0.0.0", default_http_port, *http_session_manager));
	ThreadPool::GetInstance().AddTask(new WebsiteTask());

	ThreadPool::GetInstance().AddTask(new ConnectTask(http_session_manager, "127.0.0.1", 80));
	
	ThreadPool::GetInstance().Start();

	//delete https_session_manager;
	delete http_session_manager;

	return 0;
}
