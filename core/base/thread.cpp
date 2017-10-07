#include "thread.h"

void * Thread::Run(void * args)
{
	if(args)
	{
		((Task *)args)->Exec();
	}
}

Thread::Thread(Task *t)
	: task(t)
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
	delete task;
	task = NULL;
}

TPTask::TPTask(ThreadPool *p) : pool(p), work_count(0) {}

TPTask::~TPTask()
{
	pool = NULL;
}

void TPTask::Exec()
{
	Log::Trace("thread 0x%x will work.\n", pthread_self());
	for(;;)
	{
		pthread_mutex_lock(&pool->lock);

		if(pool->quit)
		{
			pthread_mutex_unlock(&pool->lock);
			Log::Trace("thread 0x%x will quit, work_count=%d.\n", pthread_self(), work_count);
			pthread_exit(NULL) ;
			return ;
		}

		if(pool->quit == false && pool->tasks.empty())
		{
			pthread_cond_wait(&pool->cond, &pool->lock);
		}
		Task *task = pool->GetTaskWithoutLock();
		pthread_mutex_unlock(&pool->lock);
		if(task)
		{
			task->Exec();
			++work_count;
			delete task;
		}
	}
}

ThreadPool::ThreadPool()
	: quit(false)
	, start(false)
{
	lock = PTHREAD_MUTEX_INITIALIZER;
	cond = PTHREAD_COND_INITIALIZER;

	for(unsigned int i = 0; i < TP_MAX_SIZE; ++i)
	{
		pool[i] = new Thread(new TPTask(this));
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
		pool[i] = NULL;
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
	if(!task) { return false; }
	
	bool need_notify = false;

	pthread_mutex_lock(&lock);

	if(quit)
	{
		pthread_mutex_unlock(&lock);
		return false;
	}
	if(!tasks.size()) need_notify = true;
	tasks.push(task);
	
	pthread_mutex_unlock(&lock);

	if(!start) { return true; }
	if(need_notify) pthread_cond_signal(&cond);

	return true;
}

void ThreadPool::Start()
{
	assert(start == false);
	start = true;

	pthread_cond_signal(&cond);	
	signal(SIGINT, ThreadPool::StopFunc);	
	while(start)
	{
		sleep(1);
	}
}
