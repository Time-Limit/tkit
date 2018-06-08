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

using TCORE::Protocol;
using TCORE::Config;
using TCORE::ConfigManager;
using TCORE::HttpsSessionManager;
using TCORE::HttpSessionManager;
using TCORE::Acceptor;
using TCORE::ThreadPool;

int main(int argc, char **argv)
{
	ConfigManager::GetInstance().Reset({"website"});

	const Config &website_config = *ConfigManager::GetInstance().GetConfig("website");

	default_https_port = website_config["https-port"].Num();
	default_http_port = website_config["http-port"].Num();
	default_base_folder = website_config["base-folder"].Str();
	default_file_name = website_config["default-file-name"].Str();

	SessionManager::ProtocolHandler https_protocol_handler =
		[](SessionManager *manager, session_id_t sid, Protocol &p)->void
		{
			HttpRequest *req = dynamic_cast<HttpRequest *>(&p);
			if(req)
			{
				SourceReq *s = new SourceReq(manager, sid, *req);
				s->Exec();
				delete s;
			}
		};

	SessionManager::ProtocolHandler http_protocol_handler =
		[](SessionManager *manager, session_id_t sid, Protocol &p)->void
		{
			HttpRequest *req = dynamic_cast<HttpRequest *>(&p);
			if(req)
			{
				RedirectReq *s = new RedirectReq(manager, sid, *req);
				s->Exec();
				delete s;
			}
		};

	std::shared_ptr<HttpsSessionManager> https_session_manager(new HttpsSessionManager(https_protocol_handler));
	//assert(https_session_manager->InitSSLData(website_config));
	//assert(Acceptor::Listen("0.0.0.0", default_https_port, *https_session_manager));

	std::shared_ptr<HttpSessionManager> http_session_manager(new HttpSessionManager(http_protocol_handler));
	assert(Acceptor::Listen("0.0.0.0", default_http_port, [http_session_manager](int fd) ->void { http_session_manager->OnConnect(fd); }));

	ThreadPool::GetInstance().AddTask(new WebsiteTask());

	ThreadPool::GetInstance().Start();

	return 0;
}
