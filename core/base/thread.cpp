#include "thread.h"

void* HandleTask(void *p)
{
	ThreadPool *pool = (ThreadPool *)p;
	while(!pool->init)
	{
		sleep(1);
	}
	size_t work_count = 0;
	LOG_TRACE("thread 0x%lx will work.", pthread_self());
	for(;;)
	{
		pthread_mutex_lock(&pool->lock);

		if(pool->quit)
		{
			pthread_mutex_unlock(&pool->lock);
			LOG_TRACE("thread 0x%lx will quit, work_count=%zu.", pthread_self(), work_count);
			pthread_exit(NULL) ;
			return nullptr;
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
	return nullptr;
}

Thread::Thread(FUNC f, void * p)
{
	int res = pthread_create(&tid, NULL, f, p);
	if(res)
	{
		LOG_TRACE("Thread::Thread, error=%s", strerror(errno));
	}
}

Thread::~Thread()
{
	pthread_join(tid, NULL);
}

ThreadPool::ThreadPool()
	: quit(false)
	, start(false)
	, init(false)
{
	lock = PTHREAD_MUTEX_INITIALIZER;
	cond = PTHREAD_COND_INITIALIZER;

	for(unsigned int i = 0; i < TP_MAX_SIZE; ++i)
	{
		pool[i] = new Thread(HandleTask, this);
	}

	init = true;
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
