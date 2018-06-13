#include "neter.h"

using namespace TCORE;

Neter::Session::Session(SESSION_TYPE t, int f)
: fd(f)
, type(t)
{
	//TODO zmx
	write_func_ptr = &Session::DefaultWriteFunc;

	switch(type)
	{
		case ACCEPTOR_SESSION:
		{
			read_func_ptr = &Session::AcceptorReadFunc;
		}break;
		case EXCHANGE_SESSION:
		{
			read_func_ptr = &Session::ExchangerReadFunc;
		}break;
		case SECURE_EXCHANGE_SESSION:
		{
			read_func_ptr = &Session::SecureExchangerReadFunc;
		}break;
		default:
		{
			fd = -1;
			type = INVALID_SESSION;
			read_func_ptr = nullptr;
			write_func_ptr = nullptr;
		}
	}
}

void Neter::Session::Read(SessionPtr ptr)
{
	if(TestEventFlag(CLOSE_READY))
	{
		Log::Trace("Neter::Session::Read, I am closing.");
		return ;
	}

	if(false == TestEventFlag(READ_ACCESS))
	{
		return ;
	}

	ClrEventFlag(READ_ACCESS);


	(this->*read_func_ptr)();

	if(IsExchangeSession())
	{
		callback_ptr->Deserialize(ptr, read_data);
	}
}

void Neter::Session::Close()
{
	if(TestAndSetEventFlag(CLOSE_READY))
	{
		Neter::GetInstance().AddReadyCloseSession((ptrdiff_t)this);
	}
}

void Neter::Session::AcceptorReadFunc()
{
	int new_fd = -1;
	struct sockaddr_in accept_addr;
	int server_addr_len;

	for(;;)
	{
		new_fd = accept(GetFD(), (struct sockaddr *)&accept_addr, (socklen_t *)&server_addr_len);

		if(new_fd > 0)
		{
			fcntl(new_fd, F_SETFL, fcntl(new_fd, F_GETFL) | O_NONBLOCK);
			// accept success
			SessionPtr ptr(new Session(Session::EXCHANGE_SESSION, new_fd));
			ptr->InitCallback(callback_ptr);
			epoll_event ev;
			ev.events = EPOLLIN|EPOLLOUT|EPOLLET;
			ev.data.ptr = ptr.get();

			Neter::GetInstance().session_container.insert(std::make_pair((ptrdiff_t)(ptr.get()), ptr));
			Neter::GetInstance().Ctrl(EPOLL_CTL_ADD, ptr->GetFD(), &ev);
		}
		else
		{
			Log::Error("Neter::Session::AcceptorReadFunc, accept failed, info=", strerror(errno));

			if(errno == EAGAIN)
			{
				return ;
			}
			else if(errno == EINTR)
			{
				// try accept again
			}
			else
			{
				Close();
				return ;
			}
		}
	}
}

void Neter::Session::ExchangerReadFunc()
{
	const size_t READ_BUFF_SIZE = 1024;
	unsigned char read_buff[READ_BUFF_SIZE];
	size_t allcnt = 0;
	int percnt = 0;

	for(;;)
	{
		percnt = read(fd, read_buff, READ_BUFF_SIZE);
		if(percnt > 0)
		{
			allcnt += percnt;
			read_data.insert(read_data.end(), read_buff, percnt);
		}
		else
		{
			Log::Error("Neter::Session::ExchangerReadFunc, read failed, info=", strerror(errno));

			if(errno == EINTR)
			{
			}
			else if(errno == EAGAIN)
			{
				break;
			}
			else
			{
				Close();
				return ;
			}
		}
	}

	if(allcnt == 0)
	{
		Close();
		return ;
	}
}

void Neter::Session::SecureExchangerReadFunc()
{
}

void Neter::Session::Send(const Octets &data)
{
}

void Neter::Session::Write(SessionPtr ptr)
{
	if(TestEventFlag(CLOSE_READY))
	{
		Log::Trace("Neter::Session::Write, I am closing.");
		return ;
	}

	(this->*write_func_ptr)();
}

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

void Neter::Session::NotifyReadAccess(SessionPtr ptr)

{
	if(false == ptr->TestAndSetEventFlag(READ_ACCESS))
	{
		return ;
	}

	if(false == ptr->TestAndSetEventFlag(READ_READY))
	{
		return ;
	}

	bool res = Neter::GetInstance().threadpool.AddTask(TaskPtr(new SessionReadTask(ptr)));
	if(res == false)
	{
		Log::Error("Neter::Session::NotifyReadAccess, add task failed !!!");
	}
}

void Neter::Session::NotifyWriteAccess(SessionPtr ptr)
{
	if(false == ptr->TestAndSetEventFlag(WRITE_ACCESS))
	{
		return ;
	}

	if(false == ptr->TestAndSetEventFlag(WRITE_READY))
	{
		return ;
	}

	bool res = Neter::GetInstance().threadpool.AddTask(TaskPtr(new SessionReadTask(ptr)));
	if(res == false)
	{
		Log::Error("Neter::Session::NotifyWriteAccess, add task failed!!!");
	}
}

void Neter::Wait(time_t timeout)
{
	int res = epoll_wait(epoll_instance_fd, events, EPOLL_EVENT_SIZE, timeout);

	struct epoll_event *begin = events, *end = events + res;

	for( ; begin < end; ++begin)
	{
		const epoll_event &event = *begin;
		ptrdiff_t ses = static_cast<ptrdiff_t>(event.data.u64);
		SessionContainer::iterator it = session_container.find(ses);
		if(event.events & EPOLLIN)
		{
			Session::NotifyReadAccess(it->second);
		}
		if(event.events & EPOLLOUT)
		{
			Session::NotifyWriteAccess(it->second);
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
};

