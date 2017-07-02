#include "thread.h"
#include "task.h"
#include "neter.h"
#include "gatetask.h"

class HttpTaskCreator : public TaskCreator
{
public:
	Task * Create() const
	{
		return (Task *) new HttpTask();
	}
};

int main()
{
	Neter::GetInstance().Listen(9090, new HttpTaskCreator());
	ThreadPool::GetInstance().AddTask(new GateTask());
	while(1)
	{
		sleep(100);
	}
	return 0;
}
