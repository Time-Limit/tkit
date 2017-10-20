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
	using FUNC = void* (*) (void *);
public:
	Thread(FUNC, void *);
	~Thread();
	
private:
	pthread_t tid;
};

class ThreadPool
{
public:
	friend void* HandleTask(void*);

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

	bool IsStart() const { return start; }
	bool IsInit() const { return init; }

private:

	Pool pool;
	Queue  tasks;

	bool quit;
	pthread_mutex_t lock;
	pthread_cond_t cond;
	
	Task * GetTaskWithoutLock();

	bool start;
	bool init;

	void Stop()
	{
		start = false;
		LOG_TRACE("ThreadPool::Stop, i will quit.");
	}
};

#endif
