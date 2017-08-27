#include "thread.h"
#include "gatetask.h"
#include "neter.h"
#include "channel.h"

int main()
{
	signal(SIGPIPE, SIG_IGN);
	assert(Acceptor::Listen(80, HttpParser::Hatcher));
	ThreadPool::GetInstance().AddTask(new GateTask());

	ThreadPool::GetInstance().Start();

	return 0;
}
