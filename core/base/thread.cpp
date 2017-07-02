#include "thread.h"
#include "log.h"

void * Thread::Run(void * args)
{
	if(args)
	{
		((Task *)args)->Exec();
	}
}

Thread::Thread(Task *task)
{
	int res = pthread_create(&tid, NULL, Thread::Run, (void *)task);
	if(res)
	{
		Log::Trace("Thread::Thread, error=%s", strerror(errno));
	}
}

Thread::~Thread()
{
	pthread_join(tid, NULL);
}

TPTask::TPTask(ThreadPool *p)
{
	pool = p;
}

TPTask::~TPTask()
{
	pool = NULL;
}

void TPTask::Exec()
{
	for(;;)
	{
		pthread_mutex_lock(&pool->lock);

		if(pool->quit && pool->tasks.empty())
		{
			pthread_mutex_unlock(&pool->lock);
			Log::Trace("thread 0x%x will quit.\n", pthread_self());
			pthread_exit(NULL) ;
		}

		if(pool->tasks.empty())
		{
			pthread_cond_wait(&pool->cond, &pool->lock);
		}
		Task *task = pool->GetTaskWithoutLock();
		pthread_mutex_unlock(&pool->lock);

		if(task)
		{
			task->Exec();
		}
	}
}

ThreadPool::ThreadPool()
	: quit(false)
{
	lock = PTHREAD_MUTEX_INITIALIZER;
	cond = PTHREAD_COND_INITIALIZER;

	thread_pool_task = new TPTask(this);

	for(unsigned int i = 0; i < TP_MAX_SIZE; ++i)
	{
		pool[i] = new Thread(thread_pool_task);
	}
}

ThreadPool::~ThreadPool()
{
	pthread_mutex_lock(&lock);	
	quit = true;
	pthread_mutex_unlock(&lock);
	pthread_cond_broadcast(&cond);

	for(unsigned int i = 0; i < TP_MAX_SIZE; ++i)
	{
		delete pool[i];
	}
	pool.clear();

	delete thread_pool_task;
	thread_pool_task = NULL;

	pthread_mutex_destroy(&lock);
	pthread_cond_destroy(&cond);
}

Task *ThreadPool::GetTaskWithoutLock()
{
	if(tasks.empty()) {return NULL;}

	Task *tmp = tasks.front();
	tasks.pop();
	return tmp;
}

bool ThreadPool::AddTask(Task *task)
{
	if(!task) {return false;}

	pthread_mutex_lock(&lock);

	if(quit) {return false;}
	tasks.push(task);

	pthread_mutex_unlock(&lock);

	pthread_cond_signal(&cond);

	return true;
}
