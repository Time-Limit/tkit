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
#include <signal.h>
#include "task.h"
#include "log.h"

class ThreadPool;

class Thread
{
public:
	Thread(Task *t);
	~Thread();
	
	static void * Run(void *args);
private:
	Task * task;
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
	size_t work_count;
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

	void Start();
	
	static ThreadPool& GetInstance()
	{
		static ThreadPool instance;
		return instance;
	}
	
	static void StopFunc(int)
	{
		ThreadPool::GetInstance().Stop();
	}

private:

	Pool pool;
	Queue  tasks;

	bool quit;
	pthread_mutex_t lock;
	pthread_cond_t cond;
	
	TPTask *thread_pool_task;

	Task * GetTaskWithoutLock();

	bool start;

	void Stop()
	{
		start = false;
		Log::Trace("ThreadPool::Stop, i will quit.\n");
	}
};

#endif
