#include "neter.h"

using namespace TCORE;

bool Neter::Ctrl(int op, int fd, struct epoll_event *event)
{
	int res = epoll_ctl(epoll_instance_fd, op, fd, event);
	if(res == -1)
	{
		Log::Error("Neter::Ctrl, error=", strerror(errno));
		return false;
	}
	return true;
}

void Neter::TryAddReadTask(SessionPtr ptr)
{
	if(false == ptr->TestAndModifyEventFlag(Session::READ_ACCESS|Session::READ_PENDING, Session::READ_ACCESS, Session::READ_PENDING, Session::EMPTY_EVENT_FLAG))
	{
		Log::Debug("Neter::Session::TryAddReadTask, unexcept event flag");
		return ;
	}

	if(false == Neter::GetInstance().threadpool.AddTask(TaskPtr(new SessionReadTask(ptr))))
	{
		Log::Error("Neter::Session::TryAddReadTask, add read task failed");
		ptr->ClrEventFlag(Session::READ_PENDING);
		return ;
	}
	return;
}

void Neter::TryAddWriteTask(SessionPtr ptr)
{
	if(false == ptr->TestAndModifyEventFlag(Session::WRITE_ACCESS|Session::WRITE_PENDING|Session::WRITE_READY, Session::WRITE_ACCESS|Session::WRITE_READY, Session::WRITE_PENDING, Session::EMPTY_EVENT_FLAG))
	{
		Log::Debug("Neter::Session::TryAddWriteTask, unexcept event flag");
		return ;
	}

	if(false == Neter::GetInstance().threadpool.AddTask(TaskPtr(new SessionWriteTask(ptr))))
	{
		Log::Error("Neter::Session::TryAddWriteTask, add write task failed");
		ptr->ClrEventFlag(Session::WRITE_PENDING);
		return ;
	}
	return ;
}

void Neter::Wait(time_t timeout)
{
	int res = epoll_wait(epoll_instance_fd, events, EPOLL_EVENT_SIZE, timeout);

	struct epoll_event *begin = events, *end = events + res;

	for( ; begin < end; ++begin)
	{
		const epoll_event &event = *begin;
		session_id_t ses = static_cast<session_id_t>(event.data.u64);
		SessionContainer::iterator it = session_container.find(ses);
		if(event.events & EPOLLIN)
		{
			it->second->SetEventFlag(Session::READ_ACCESS);
			TryAddReadTask(it->second);
		}
		if(event.events & EPOLLOUT)
		{
			it->second->SetEventFlag(Session::WRITE_ACCESS);
			TryAddWriteTask(it->second);
		}
		if(event.events & (~(EPOLLIN|EPOLLOUT)))
		{
			Log::Error("Neter::Wait, unhandle event, event=", std::hex, event.events, std::dec);
			it->second->Close();
		}
	}

	SessionKeyList tmp_ready_close_session_list;
	do
	{
		SpinLockGuard guard(ready_close_session_list_lock);
		swap(tmp_ready_close_session_list, ready_close_session_list);
	}while(0);

	Log::Trace("Neter::Wait, ", tmp_ready_close_session_list.size(), " sessions will close!");

	for(auto addr : tmp_ready_close_session_list)
	{
		SessionContainer::iterator it = session_container.find(addr);
		if(it == session_container.end())
		{
			Log::Error("Neter::Wait, session cache not found!");
			continue;
		}
		struct epoll_event event;
		Ctrl(EPOLL_CTL_DEL, it->second->GetFD(), &event);
		session_container.erase(it);
	}
}

Neter::Neter()
: threadpool(THREAD_COUNT, ThreadPool::PT_XT_TO_XQ, [](task_id_t task_id, size_t thread_size)->size_t { return task_id; })
, epoll_instance_fd(epoll_create(1))
, session_id_spawner(0)
{
	if(epoll_instance_fd == -1)
	{
		Log::Error("Neter::Neter, create epoll instance failed, info=", strerror(errno));
		return ;
	}
	if(!threadpool.AddTask(TaskPtr(new NeterPollTask())))
	{
		Log::Error("Neter::Neter, add poll task failed!!!");
	}
}

Neter::~Neter()
{
	if(epoll_instance_fd != -1)
	{
		int res = close(epoll_instance_fd);
		if(res == -1)
		{
			Log::Error("Neter::~Neter, close epoll instance failed, info=", strerror(errno));
		}
	}
}

Neter::SessionPtr Neter::GetSession(session_id_t sid) const
{
	SpinLockGuard guard(session_container_lock);
	SessionContainer::const_iterator it = session_container.find(sid);
	if(it == session_container.end())
	{
		return SessionPtr(nullptr);
	}
	return it->second;
}
