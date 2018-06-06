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

class ThreadTask : public Task
{
public:
	void Exec() override;
	ThreadTask(LogicTask::TASK_TYPE t) : type(t) {}
private:
	LogicTask::TASK_TYPE type;
};

class Thread
{
public:
	Thread(Task *task);
	~Thread();
	
private:
	pthread_t tid;

	static void * Run(void *p);
};

class ThreadPool
{
public:

	class InnerPool
	{
		friend class ThreadPool;
	public:
		
		enum INNER_POOL_THREAD_COUNT
		{
			NORMAL_THREAD_COUNT = 2,
		};

		typedef unsigned char thread_count_t;
		typedef unsigned int  thread_id_t;
		typedef std::map<thread_id_t, Thread *> Pool;
		typedef std::queue<LogicTask *> Queue;

		InnerPool(LogicTask::TASK_TYPE t);
		~InnerPool();

		void Lock() { pthread_mutex_lock(&lock); }
		void Unlock() { pthread_mutex_unlock(&lock); }

		void Wait() { pthread_cond_wait(&cond, &lock); }
		void Notify() { pthread_cond_signal(&cond); } 
		void NotifyAll() { pthread_cond_broadcast(&cond); }

		bool IsEmpty() { return tasks.empty(); }

		bool AddTask(LogicTask *task);
		LogicTask* GetTaskWithoutLock();

		static thread_count_t GetThreadCount(LogicTask::TASK_TYPE t)
		{
			switch(t)
			{
				case LogicTask::TASK_TYPE_NORMAL: return NORMAL_THREAD_COUNT; break;
				default: break;
			}

			return 0;
		}

	private:

		LogicTask::TASK_TYPE type;

		thread_count_t thread_count;

		pthread_mutex_t lock;
		pthread_cond_t cond;

		Queue tasks;

		Pool pool;
	};

	typedef std::array<InnerPool, LogicTask::TASK_TYPE_COUNT> InnerPoolArray;

	static ThreadPool& GetInstance()
	{
		static ThreadPool instance;
		return instance;
	}

	void Start();
	bool IsStart() const { return start; }
	bool IsInit() const { return init; }
	bool IsQuit() const { return quit; }

	void InnerLock(LogicTask::TASK_TYPE type) { inner_pools[type].Lock(); } 
	void InnerUnlock(LogicTask::TASK_TYPE type) { inner_pools[type].Unlock(); }
	void InnerWait(LogicTask::TASK_TYPE type) { inner_pools[type].Wait(); }

	void InnerNotify(LogicTask::TASK_TYPE type) { inner_pools[type].Notify(); }
	void InnerNotifyAll(LogicTask::TASK_TYPE type) { inner_pools[type].NotifyAll(); }
	bool InnerIsEmpty(LogicTask::TASK_TYPE type) { return inner_pools[type].IsEmpty(); }

	bool AddTask(LogicTask *task) { return inner_pools[task->GetType()].AddTask(task); }
	LogicTask* GetTaskWithoutLock(LogicTask::TASK_TYPE type) { return inner_pools[type].GetTaskWithoutLock(); }

	static void StopFunc(int)
	{
		ThreadPool::GetInstance().Stop();
	}

	void Stop()
	{
		start = false;
		LOG_TRACE("ThreadPool::Stop, i will quit.");
	}
private:
	bool quit;
	bool start;
	bool init;

	InnerPoolArray inner_pools;

	ThreadPool();
	~ThreadPool();
};

/*
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

};
*/

#endif
