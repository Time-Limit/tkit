#ifndef _THREADPOOL_H_
#define _THREADPOOL_H_

#include <sys/types.h>
#include <functional>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <memory>
#include <vector>
#include <array>
#include <list>

#include "exptype.h"
#include "lock.h"
#include "log.h"

namespace TCORE
{

struct Task
{
	Task(task_id_t i = 0) : id(i) {}
	virtual ~Task() {}
	virtual void Exec() {}
	task_id_t ID() const { return id; }

private:
	task_id_t id;
};
typedef std::shared_ptr<Task> TaskPtr;

class TaskQueue
{
	typedef std::list<TaskPtr> TaskList;
	mutable SpinLock task_list_lock;
	TaskList task_list;
	
	pthread_cond_t cond;
	pthread_mutex_t lock;

	void NotifyAll() { pthread_cond_broadcast(&cond); }

public:
	bool Push(TaskPtr task);
	TaskPtr Pop();
	size_t Size() const { SpinLockGuard guard(task_list_lock); return task_list.size(); }
	bool Empty() const { SpinLockGuard guard(task_list_lock); return task_list.empty(); }
	void Wait() { pthread_cond_wait(&cond, &lock); }
};
typedef std::shared_ptr<TaskQueue> TaskQueuePtr;

class ClearTaskQueueTask : public Task
{
	bool stop_flag;
	TaskQueuePtr task_queue_ptr;
public:
	ClearTaskQueueTask(TaskQueuePtr ptr) : Task(), stop_flag(false), task_queue_ptr(ptr) {}

	virtual void Exec()
	{
		if(task_queue_ptr.get() == nullptr)
		{
			return ;
		}

		while(stop_flag == false)
		{
			if(task_queue_ptr->Size())
			{
				TaskPtr task = task_queue_ptr->Pop();
				if(task) task->Exec();
			}
			else
			{
				task_queue_ptr->Wait();
			}
		}
	}

	void Stop() { stop_flag = true; }
};

class Thread
{
private:
	pthread_t tid;
	TaskPtr task;
	bool start_flag;
	static void* RunTask(void *task);
	static void  exit_cur_thread(int);
	static void  notify_task_quit(int);

public:

	pthread_t GetThreadID() const { return tid; }

	explicit Thread(TaskPtr t) : task(t), start_flag(false) {}

	void Run()
	{
		int res = pthread_create(&tid, NULL, RunTask, (void *)task.get());
		if(res)
		{
			Log::Error("Thread::Run, failed, info=", strerror(errno));
		}
		else
		{
			start_flag = true;
		}
	}

	~Thread()
	{
		if(start_flag)
		{
			pthread_cancel(tid);
			pthread_join(tid, nullptr);
		}
	}
};

class ThreadPool
{
public:
	typedef std::function<size_t (task_id_t, size_t thread_size)> DistributeFunc;
	
	enum POOL_TYPE
	{
		PT_XT_TO_XQ = 0,
		PT_XT_TO_1Q = 1,
	};

private:
	typedef std::vector<Thread> ThreadVector;
	ThreadVector threads;

	typedef std::vector<TaskQueuePtr> TaskQueueVector;
	TaskQueueVector taskqueues;

	DistributeFunc distribute;

	static size_t DefaultDistributeFunc(task_id_t task_id, size_t thread_size)
	{
		return task_id % thread_size;
	}

public:
	explicit ThreadPool(size_t _thread_size, POOL_TYPE type, DistributeFunc _distribute = DistributeFunc(DefaultDistributeFunc))
	: distribute(_distribute)
	{ 
		try
		{
			if(type == PT_XT_TO_XQ)
			{
				for(size_t i = 0; i < _thread_size; ++i)
				{
					taskqueues.push_back(TaskQueuePtr(new TaskQueue()));
				}

				for(size_t i = 0; i < _thread_size; ++i)
				{
					threads.push_back(Thread(TaskPtr(new ClearTaskQueueTask(taskqueues[i]))));
				}
			}
			else if(type == PT_XT_TO_1Q)
			{
				taskqueues.push_back(TaskQueuePtr(new TaskQueue()));

				for(size_t i = 0; i < _thread_size; ++i)
				{
					threads.push_back(Thread(TaskPtr(new ClearTaskQueueTask(taskqueues[0]))));
				}
			}
			else
			{
				throw;
			}

			for(auto &t : threads)
			{
				t.Run();
			}
		}
		catch(...)
		{
			threads.clear();
			taskqueues.clear();
			distribute = DistributeFunc(nullptr);
			throw;
		}
	}

	void Stop();

	bool AddTask(TaskPtr ptr);
};

}

#endif
