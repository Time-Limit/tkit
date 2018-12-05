#include "threadpool.h"

using namespace TCORE;

void ThreadPool::Stop()
{
	Log::Trace("ThreadPool::Stop, the number of TaskQueue is ", taskqueues.size());
	Log::Trace("ThreadPool::Stop, the number of ThreadPool is ", threads.size());
	for(const auto &q : taskqueues)
	{
		Log::Trace("ThreadPool::Stop, the number of remaining task is ", q->Size());
	}
}

bool ThreadPool::AddTask(TaskPtr ptr)
{
	size_t index = distribute(ptr->ID(), taskqueues.size());

	if(index < taskqueues.size())
	{
		return taskqueues[index]->Push(ptr);
	}

	return false;
}

bool TaskQueue::Push(TaskPtr task)
{
	if(task.get() == nullptr)
	{
		return false;
	}
	try
	{
		SpinLockGuard guard(task_list_lock);
		task_list.push_back(task);
	}
	catch(...)
	{
		return false;
	}
	NotifyAll();
	return true;
}

TaskPtr TaskQueue::Pop()
{
	SpinLockGuard guard(task_list_lock);
	if(task_list.empty()) return TaskPtr(nullptr);
	TaskPtr ptr = task_list.front();
	task_list.pop_front();
	return ptr;
}

void Thread::exit_cur_thread(int)
{
	Log::Error("Thread::exit_cur_thread, tid=", std::hex, pthread_self(), std::dec);
	pthread_exit(nullptr);
}

void Thread::notify_task_quit(int)
{
	Log::Error("Thread::notify_task_quit, tid=", std::hex, pthread_self(), std::dec);
	pthread_exit(nullptr);
}

void* Thread::RunTask(void *task)
{
	if(task == nullptr) return nullptr;
	Task *t = static_cast<Task *>(task);
	t->Exec();
	return nullptr;
}

