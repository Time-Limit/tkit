#include "thread.h"
#include "gatetask.h"
#include "neter.h"
#include "channel.h"

int main()
{
	assert(Acceptor::Listen(9090, HttpParser::Hatcher));
	Thread * gate_task_thread = new Thread(new GateTask());
	while(true)
	{
		;
	}
	return 0;
}
