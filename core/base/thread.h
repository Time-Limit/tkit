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

namespace TCORE
{

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
	Thread(Task *task, flag_t f);
	~Thread();

	flag_t GetFlag() const { return flag; }
	pthread_t GetThreadID() const { return tid; }
	
private:
	pthread_t tid;
	flag_t flag;

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
#ifndef _NORMAL_THREAD_COUNT_
#define _NORMAL_THREAD_COUNT_ 4
#endif
			NORMAL_THREAD_COUNT = (_NORMAL_THREAD_COUNT_),
		};

		typedef unsigned char thread_count_t;
		typedef unsigned int  thread_id_t;
		typedef std::map<thread_id_t, Thread *> Pool;
		typedef std::queue<LogicTask *> Queue;
		typedef std::map<thread_id_t, flag_t> FlagMap;

		InnerPool(LogicTask::TASK_TYPE t);
		~InnerPool();

		void Lock() { pthread_mutex_lock(&lock); }
		void Unlock() { pthread_mutex_unlock(&lock); }

		void Wait() { pthread_cond_wait(&cond, &lock); }
		void Notify() { pthread_cond_signal(&cond); } 
		void NotifyAll() { pthread_cond_broadcast(&cond); }

		bool IsEmpty(pthread_t ptid) { return tasks[TransThreadID(ptid)].empty(); }

		bool AddTask(LogicTask *task);
		LogicTask* GetTaskWithoutLock(pthread_t ptid);

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
		size_t HashTaskFlag(flag_t flag) const
		{
			return flag % NORMAL_THREAD_COUNT;
		}

		flag_t TransThreadID(pthread_t ptid) const
		{
			FlagMap::const_iterator cit = flag_map.find(ptid);
			if(cit == flag_map.end())
			{
				assert(false);
				return flag_t(0);
			}
			return cit->second;
		}

	private:

		LogicTask::TASK_TYPE type;

		thread_count_t thread_count;

		pthread_mutex_t lock;
		pthread_cond_t cond;

		Queue tasks[NORMAL_THREAD_COUNT];

		FlagMap flag_map;

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
	bool InnerIsEmpty(LogicTask::TASK_TYPE type, pthread_t ptid) { return inner_pools[type].IsEmpty(ptid); }

	bool AddTask(LogicTask *task) { return inner_pools[task->GetType()].AddTask(task); }
	LogicTask* GetTaskWithoutLock(LogicTask::TASK_TYPE type, pthread_t ptid) { return inner_pools[type].GetTaskWithoutLock(ptid); }

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

}

#endif
