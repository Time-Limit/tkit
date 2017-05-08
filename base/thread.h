#ifndef _THREAD_H_
#define _THREAD_H_

#include <queue>
#include <map>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "task.h"

class ThreadPool;

class Thread
{
public:
	Thread(Task *task);
	~Thread();
	
	static void * Run(void *args);
private:
	pthread_t tid;
};

class TPTask : public Task
{
public:
	virtual void Exec();

	TPTask(ThreadPool *p);
	~TPTask();
private:
	ThreadPool *pool;
};

class ThreadPool
{
public:
	friend class TPTask;

	enum THREAD_POOL_CONFIG
	{
		TP_MAX_SIZE = 4,
	};

	ThreadPool();
	~ThreadPool();

	typedef unsigned int thread_id_t;
	typedef std::map<thread_id_t, Thread *> Pool;
	typedef std::queue<Task *> Queue;

	bool AddTask(Task *task);
private:

	Pool pool;
	Queue  tasks;

	bool quit;
	pthread_mutex_t lock;
	pthread_cond_t cond;
	
	TPTask *thread_pool_task;

	Task * GetTaskWithoutLock();
};

#endif
