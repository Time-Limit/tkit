#include "threadpool.h"

using namespace TCORE;

bool ThreadPool::AddTask(TaskPtr ptr)
{
	size_t index = distribute(ptr->ID(), taskqueues.size());

	if(index < taskqueues.size())
	{
		return taskqueues.Push(ptr);
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

void* Thread::RunTask(void *task)
{
	if(task == nullptr) return nullptr;
	Task *t = static_cast<Task *>(task);
	t->Exec();
	return nullptr;
}

