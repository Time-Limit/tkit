#include "thread.h"
#include <assert.h>
#include <stdlib.h>
#include <exception>
#include <stdexcept>
#include <unistd.h>

ThreadPool::ThreadPool(size_t _size)
	: size(_size)
	, flag(0)
	, task_queue()
	, thread_pool(NULL)
{
	if(pthread_cond_init(&task_ready, NULL))
	{
		throw std::runtime_error("init task_ready failed.");
	}
	if(pthread_mutex_init(&task_lock, NULL))
	{
		throw std::runtime_error("init task_lock failed.");
	}

	thread_pool = (pthread_t *)malloc(sizeof(pthread_t)*size);

	if(thread_pool == NULL)
	{
		throw std::runtime_error("alloc thread_pool failed.");
	}

	for(int i = 0; i < size; ++i)
	{	
		if(pthread_create(thread_pool+i, NULL, Work, (void*)this))
		{
			throw std::runtime_error("alloc thread failed.");
		}
	}
}

ThreadPool::~ThreadPool()
{
	pthread_mutex_lock(&task_lock);
	
	SetFlag(_T_REFUSE);	 

	while(task_queue.empty() == false)
	{
		pthread_mutex_unlock(&task_lock);
		sleep(1);
		pthread_mutex_lock(&task_lock);
	}

	SetFlag(_T_EXIT);	 
	
	pthread_mutex_unlock(&task_lock);

	pthread_cond_broadcast(&task_ready);

	for(int i = 0; i < size; ++i)
	{
		pthread_join(thread_pool[i], NULL);
	}

	free(thread_pool);

	pthread_mutex_lock(&task_lock);
	pthread_mutex_unlock(&task_lock);
	pthread_mutex_destroy(&task_lock);
	pthread_cond_destroy(&task_ready);
}

void *ThreadPool::Work(void *args)
{
	assert(args && "args is valid!");
	ThreadPool *pool = (ThreadPool *)args;
	for(;;)
	{
		pthread_mutex_lock(&pool->task_lock);
		while(pool->task_queue.empty() && !pool->TestFlag(_T_EXIT))
		{
			pthread_cond_wait(&pool->task_ready, &pool->task_lock);
		}

		if(pool->TestFlag(_T_EXIT))
		{
			pthread_mutex_unlock(&pool->task_lock);
			pthread_exit(NULL);
		}

		Task task = pool->task_queue.front();
		pool->task_queue.pop();

		pthread_mutex_unlock(&pool->task_lock);

		if(task.first)
		{
			task.first(task.second);
		}
	}
	return NULL;
}

bool ThreadPool::AddTask(Handler handler, Args args)
{
	if(handler)
	{
		pthread_mutex_lock(&task_lock);
		task_queue.push(Task(handler, args));
		pthread_mutex_unlock(&task_lock);
		pthread_cond_signal(&task_ready);
		return true;
	}
	return false;
}


