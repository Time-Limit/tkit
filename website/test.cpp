#include "session.h"
#include "thread.h"
#include "neter.h"
#include "octets.h"

using namespace TCORE;

int main()
{
	struct NeterTask : public NormalTask
	{
		void Exec()
		{
			signal(SIGPIPE, SIG_IGN);
			const ThreadPool *tp = &ThreadPool::GetInstance();
			if(tp->IsStart()) Neter::GetInstance().Wait(1000);
			else sleep(1);
			ThreadPool::GetInstance().AddTask(new NeterTask());
		}
	};

	assert(Neter::Listen<char>("0.0.0.0", 8080, [](const char &req, Neter::SendHandle sh)
							{
								std::cout << "client:" << req << std::endl;
								OctetsStream os;
								os << req;
								sh(os.GetData());
							}));

	ThreadPool::GetInstance().AddTask(new NeterTask());
	ThreadPool::GetInstance().Start();
	return 0;
}
