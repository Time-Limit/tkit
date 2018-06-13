#include <iostream>
#include <unistd.h>

#include "websitebase.h"
#include "threadpool.h"
#include "config.h"
#include "neter.h"
#include "file.h"
#include "md5.h"

using namespace TCORE;

struct WaitSignal
{
	void Wait()
	{
		while(flag)
		{
			sleep(1);
		}
	}

	static void OnRecvSignal(int)
	{
		WaitSignal::GetInstance().flag = false;
		std::cout << "WaitSignal::OnRecvSignal, I will quit." << std::endl;
	}

	static WaitSignal & GetInstance() { static WaitSignal instance; return instance; }

private:
	WaitSignal() : flag(true)
	{
		sighandler_t old = signal(SIGTERM, OnRecvSignal);
		if(old == SIG_ERR)
		{
			std::cout << "WaitSignal::WaitSignal, register handler failed !!!" << std::endl;
		}
		else
		{
			std::cout << "WaitSignal::WaitSignal, register handler success !!!" << std::endl;
		}
	}
	bool flag;
};

int main(int argc, char **argv)
{
	ConfigManager::GetInstance().Reset({"website"});

	const Config &website_config = *ConfigManager::GetInstance().GetConfig("website");

	default_https_port = website_config["https-port"].Num();
	default_http_port = website_config["http-port"].Num();
	default_base_folder = website_config["base-folder"].Str();
	default_file_name = website_config["default-file-name"].Str();

	bool res = Neter::Listen<char>("0.0.0.0", default_http_port, [](const char &p, Neter::SessionPtr ptr)->void
								{
									std::cout << p << std::endl;
								});


	std::cout << "main-function, result of Neter::Listen is " << res << std::endl;

	WaitSignal::GetInstance().Wait();

	//std::shared_ptr<HttpsSessionManager> https_session_manager(new HttpsSessionManager(https_protocol_handler));
	//assert(https_session_manager->InitSSLData(website_config));
	//assert(Acceptor::Listen("0.0.0.0", default_https_port, [https_session_manager](int fd) ->void { https_session_manager->OnConnect(fd); }));
	//
	//std::shared_ptr<HttpSessionManager> http_session_manager(new HttpSessionManager(http_protocol_handler));
	//assert(Acceptor::Listen("0.0.0.0", default_http_port, [http_session_manager](int fd) ->void { http_session_manager->OnConnect(fd); }));
	//
	//ThreadPool::GetInstance().AddTask(new WebsiteTask());
	//
	//ThreadPool::GetInstance().Start();

	return 0;
}
