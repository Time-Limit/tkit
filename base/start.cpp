#include "thread.h"
#include "task.h"
#include "neter.h"

int main()
{
	Neter::GetInstance().Listen(9090);
	ThreadPool::GetInstance().AddTask(new NeterTask());
	return 0;
}
