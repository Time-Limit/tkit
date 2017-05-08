#include "thread.h"
#include "task.h"
#include "neter.h"

class SleepTask : public Task
{
public:
	virtual void Exec()
	{
		sleep(1);
		printf("sleeptask : 0x%x\n", pthread_self());
	}
};

int main()
{
	ThreadPool *pool = new ThreadPool();
	for(int i = 0; i < 10; ++i)
	{
		pool->AddTask(new SleepTask());
		printf("%d\n", i);
	}
	delete pool;

	return 0;
}
