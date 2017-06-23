#include "thread.h"
#include "task.h"
#include "neter.h"
#include "netertask.h"

int main()
{
	Neter::GetInstance().Listen(9090);
	ThreadPool::GetInstance().AddTask(new NeterTask());
	while(1)
	{
		sleep(100);
	}
	return 0;
}
