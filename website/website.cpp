#include <iostream>
#include <unistd.h>

#include "websitebase.h"
#include "threadpool.h"
#include "protocol.h"
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

	bool res = Neter::Listen<HttpRequest>("0.0.0.0", default_http_port, [](const HttpRequest &p, session_id_t sid)->void
								{
									std::cout << p.url << std::endl;
									HttpResponse res;
									res.version = "HTTP/1.1";
									res.status = 200;
									res.statement = "OK";
									Neter::SendProtocol(sid, res);
								});


	std::cout << "main-function, result of Neter::Listen is " << res << std::endl;

	WaitSignal::GetInstance().Wait();

	return 0;
}
