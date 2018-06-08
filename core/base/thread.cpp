#include "thread.h"

using namespace TCORE;

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
	pthread_t ptid = pthread_self();
	Log::Trace("thread ", ptid, " will work.");
	for(;;)
	{
		pool->InnerLock(type);

		if(pool->IsQuit())
		{
			pool->InnerUnlock(type);
			Log::Trace("thread ", ptid, " will quit, work_count=", work_count);
			pthread_exit(NULL);
			return ;
		}

		if(pool->IsQuit() == false && pool->InnerIsEmpty(type, ptid))
		{
			pool->InnerWait(type);
		}
		Task *task = pool->GetTaskWithoutLock(type, ptid);
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

Thread::Thread(Task *task, flag_t f)
: flag(f)
{
	int res = pthread_create(&tid, NULL, Run, (void *)task);
	if(res)
	{
		Log::Error("Thread::Thread, error=", strerror(errno));
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

LogicTask *ThreadPool::InnerPool::GetTaskWithoutLock(pthread_t ptid)
{
	flag_t flag = TransThreadID(ptid);
	if(tasks[flag].empty()) {return NULL;}

	LogicTask *tmp = tasks[flag].front();
	tasks[flag].pop();
	return tmp;
}

bool ThreadPool::InnerPool::AddTask(LogicTask *task)
{
	if(!task) { return false; }
	
	pthread_mutex_lock(&lock);

	if(ThreadPool::GetInstance().IsQuit())
	{
		pthread_mutex_unlock(&lock);
		return false;
	}

	tasks[HashTaskFlag(task->GetFlag())].push(task);
	
	pthread_mutex_unlock(&lock);

	if(!ThreadPool::GetInstance().IsStart()) { return true; }
	pthread_cond_broadcast(&cond);

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
		pool[i] = new Thread(new ThreadTask(type), i);
		flag_map[pool[i]->GetThreadID()] = pool[i]->GetFlag();
	}
}
