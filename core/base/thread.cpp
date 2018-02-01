#include "thread.h"

void* Thread::Run(void *task)
{
	if(task) ((Task*)task)->Exec();
	delete (Task *)task;
	return NULL;
};

void ThreadTask::Exec()
{
	ThreadPool *pool = &ThreadPool::GetInstance();
	while(!pool->IsInit())
	{
		sleep(1);
	}
	size_t work_count = 0;
	LOG_TRACE("thread 0x%lx will work.", pthread_self());
	for(;;)
	{
		pool->InnerLock(type);

		if(pool->IsQuit())
		{
			pool->InnerUnlock(type);
			LOG_TRACE("thread 0x%lx will quit, work_count=%zu.", pthread_self(), work_count);
			pthread_exit(NULL);
			return ;
		}

		if(pool->IsQuit() == false && pool->InnerIsEmpty(type))
		{
			pool->InnerWait(type);
		}
		Task *task = pool->GetTaskWithoutLock();
		pool->InnerUnlock(type);
		if(task)
		{
			task->Exec();
			++work_count;
			delete task;
		}
	}
	return ;
}

Thread::Thread(Task *task)
{
	int res = pthread_create(&tid, NULL, Run, (void *)task);
	if(res)
	{
		LOG_TRACE("Thread::Thread, error=%s", strerror(errno));
	}
}

Thread::~Thread()
{
	pthread_join(tid, NULL);
}

ThreadPool::~ThreadPool()
{
	/*
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

	pthread_mutex_destroy(&lock);
	pthread_cond_destroy(&cond);
	*/
}

ThreadPool::InnerPool::~InnerPool()
{
}

Task *ThreadPool::GetTaskWithoutLock()
{
	return NULL;
	/*
	if(tasks.empty()) {return NULL;}

	Task *tmp = tasks.front();
	tasks.pop();
	return tmp;
	*/
}

bool ThreadPool::AddTask(Task *task)
{
	/*
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

	*/
	return true;
}

void ThreadPool::Start()
{
	assert(start == false);
	start = true;

	/*
	pthread_cond_signal(&cond);	
	signal(SIGINT, ThreadPool::StopFunc);	
	while(start)
	{
		sleep(1);
	}
	*/
}

ThreadPool::ThreadPool()
: quit(false)
, start(false)
, init(false)
, inner_pools({NORMAL_THREAD_COUNT})
{}

ThreadPool::InnerPool::InnerPool(thread_count_t count)
: thread_count(count)
, lock(PTHREAD_MUTEX_INITIALIZER)
, cond(PTHREAD_COND_INITIALIZER)
{
	/*
	for(thread_count_t i = 0; i < thread_count; ++i)
	{
		pool[i] = new Thread(ThreadTask());
	}
	*/
}
