#include "neter.h"
#include "thread.h"
#include "gatetask.h"
#include <stdlib.h>

void GateTask::Exec()
{
	signal(SIGPIPE, SIG_IGN);
	while(true) Neter::GetInstance().Wait(1000);
	//ThreadPool::GetInstance().AddTask(this);
}
