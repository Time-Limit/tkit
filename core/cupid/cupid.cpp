#include <iostream>
#include <unistd.h>

#include "threadpool.h"
#include "base_param.h"
#include "protocol.h"
#include "callback.h"
#include "config.h"
#include "cupid.h"
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

	default_http_port = cupid_config["http-port"].Num();
	default_base_folder = cupid_config["base-folder"].Str();
	default_file_name = cupid_config["default-file-name"].Str();
	default_logic_thread_count = cupid_config["default-logic-thread-count"].Num();

	ImportModule();

	ThreadPool logic_thread_pool(default_logic_thread_count, ThreadPool::PT_XT_TO_XQ,
								[](task_id_t task_id, size_t thread_size)->size_t
								{
								return task_id%thread_size;
								});

	bool res = Neter::Listen<HttpRequest>("0.0.0.0", default_http_port, nullptr, nullptr, [&logic_thread_pool](HttpRequest &&p, session_id_t sid)->void
								{
									//logic_thread_pool.AddTask(TaskPtr(new HttpCheatTask(sid, sid, p)));
									logic_thread_pool.AddTask(TaskPtr(new HttpCallbackTask(sid, p)));
								});
	std::cout << "main-function, result of Neter::Listen is " << res << std::endl;
	
	WaitSignal::GetInstance().Wait();

	return 0;
}
