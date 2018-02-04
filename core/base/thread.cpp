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
		Task *task = pool->GetTaskWithoutLock(type);
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
	quit = true;
}

ThreadPool::InnerPool::~InnerPool()
{
	pthread_cond_broadcast(&cond);

	for(auto &it : pool)
	{
		delete it.second;
		it.second = NULL;
	}

	pool.clear();

	pthread_mutex_destroy(&lock);
	pthread_cond_destroy(&cond);
}

LogicTask *ThreadPool::InnerPool::GetTaskWithoutLock()
{
	if(tasks.empty()) {return NULL;}

	LogicTask *tmp = tasks.front();
	tasks.pop();
	return tmp;
}

bool ThreadPool::InnerPool::AddTask(LogicTask *task)
{
	if(!task) { return false; }
	
	bool need_notify = false;

	pthread_mutex_lock(&lock);

	if(ThreadPool::GetInstance().IsQuit())
	{
		pthread_mutex_unlock(&lock);
		return false;
	}
	if(!tasks.size()) need_notify = true;
	tasks.push(task);
	
	pthread_mutex_unlock(&lock);

	if(!ThreadPool::GetInstance().IsStart()) { return true; }
	if(need_notify) pthread_cond_signal(&cond);

	return true;
}

void ThreadPool::Start()
{
	assert(start == false);
	start = true;

	for(auto &it : inner_pools)
	{
		it.Notify();
	}

	signal(SIGINT, ThreadPool::StopFunc);	

	init = true;

	while(start)
	{
		sleep(1);
	}
}

ThreadPool::ThreadPool()
: quit(false)
, start(false)
, init(false)
, inner_pools({LogicTask::TASK_TYPE_NORMAL})
{}

ThreadPool::InnerPool::InnerPool(LogicTask::TASK_TYPE t)
: type(t)
, thread_count(GetThreadCount(t))
, lock(PTHREAD_MUTEX_INITIALIZER)
, cond(PTHREAD_COND_INITIALIZER)
{
	for(thread_count_t i = 0; i < thread_count; ++i)
	{
		pool[i] = new Thread(new ThreadTask(type));
	}
}
