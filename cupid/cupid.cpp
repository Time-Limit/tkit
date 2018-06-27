#include <iostream>
#include <unistd.h>

#include "cupidtask.h"
#include "cupidbase.h"
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
	ConfigManager::GetInstance().Reset({"cupid"});

	const Config &cupid_config = *ConfigManager::GetInstance().GetConfig("cupid");

	Log::GetInstance().SetDebugLevel();

	default_https_port = cupid_config["https-port"].Num();
	default_http_port = cupid_config["http-port"].Num();
	default_base_folder = cupid_config["base-folder"].Str();
	default_file_name = cupid_config["default-file-name"].Str();
	default_logic_thread_count = cupid_config["default-logic-thread-count"].Num();

	ThreadPool logic_thread_pool(default_logic_thread_count, ThreadPool::PT_XT_TO_XQ,
								[](task_id_t task_id, size_t thread_size)->size_t
								{
								return task_id%thread_size;
								});

	bool res = Neter::Listen<HttpRequest>("0.0.0.0", default_http_port, nullptr, nullptr, [&logic_thread_pool](const HttpRequest &p, session_id_t sid)->void
								{
									logic_thread_pool.AddTask(TaskPtr(new HandleHttpRequestTask(sid, sid, p)));
								});
	std::cout << "main-function, result of Neter::Listen is " << res << std::endl;

	res = Neter::Listen<HttpRequest>("0.0.0.0", default_https_port, nullptr, nullptr, [&logic_thread_pool](const HttpRequest &p, session_id_t sid)->void
								{
									logic_thread_pool.AddTask(TaskPtr(new HandleHttpRequestTask(sid, sid, p)));
								},
								Neter::SecureConfig(Neter::SecureConfig::ENABLE, cupid_config["cert_path"].Str(), cupid_config["pkey_path"].Str()));

	std::cout << "main-function, result of Neter::Listen is " << res << std::endl;

	//std::function<void(session_id_t sid)> connect_callback = [](session_id_t sid)->void
	//							{
	//								HttpRequest req;
	//								HttpPacketVisitor<HttpRequest> visitor(req);
	//								visitor.SetURL("/");
	//								visitor.SetVersion("HTTP/1.1");
	//								visitor.SetMethod("GET");
	//								visitor.SetHeader("User-Agent", "curl/7.29.0");
	//								visitor.SetHeader("Accept", "*/*");
	//								visitor.SetHeader("Host", "119.75.213.61");
	//								visitor.SetHeader("Connection", "keep-alive");
	//
	//								Neter::SendProtocol(sid, req);
	//							}; 
	//
	//std::function<void(const HttpResponse &p, session_id_t sid)> deserialize = [](const HttpResponse &p, session_id_t sid)->void
	//							{
	//								std::cout << "receive response !!!" << std::endl;
	//							};
	//
	//std::function<void(session_id_t, const std::string&, int port)> disconnect_callback = [&connect_callback, &disconnect_callback, &deserialize](session_id_t old_sid, const std::string &ip, int port)->void
	//							{
	//								bool res = Neter::Connect<HttpResponse>(ip, port, connect_callback, disconnect_callback, deserialize);
	//								std::cout << "::main::disconnect_callback, res=" << res << std::endl;
	//							};
	//
	//
	//bool connect_res = Neter::Connect<HttpResponse>("119.75.213.61", 80, connect_callback, disconnect_callback, deserialize);
	//
	//std::cout << "main-function, result of Neter::Connect is " << connect_res << std::endl;

	WaitSignal::GetInstance().Wait();

	return 0;
}
