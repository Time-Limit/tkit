#include "neter.h"

using namespace TCORE;

Neter::Session::Session(session_id_t s, SESSION_TYPE t, int f)
: sid(s)
, fd(f)
, type(t)
, event_flag(0)
, cursor_of_first_send_data(0)
, read_func_ptr(&Session::DefaultReadFunc)
, write_func_ptr(&Session::DefaultWriteFunc)
, port(0)
{
	switch(type)
	{
		case ACCEPTOR_SESSION:
		{
			read_func_ptr = &Session::AcceptorReadFunc;
		}break;
		case CONNECTOR_SESSION:
		{
			write_func_ptr = &Session::ConnectorWriteFunc;
		}break;
		case EXCHANGE_SESSION:
		{
			read_func_ptr = &Session::ExchangerReadFunc;
			write_func_ptr = &Session::ExchangerWriteFunc;
		}break;
		case SECURE_EXCHANGE_SESSION:
		{
			read_func_ptr = &Session::SecureExchangerReadFunc;
		}break;
		default:
		{
			event_flag = 0;
			fd = -1;
			type = INVALID_SESSION;
			read_func_ptr = nullptr;
			write_func_ptr = nullptr;
		}
	}
}

Neter::Session::~Session()
{
	int res = close(fd);
	if(res == -1)
	{
		Log::Error("Session::~Session, close fd failed, info=", strerror(errno));
	}
	else
	{
		Log::Debug("Sesssion::~Session, close success!");
	}
}

void Neter::Session::Close()
{
	if(TestAndModifyEventFlag(CLOSE_READY, EMPTY_EVENT_FLAG, CLOSE_READY, EMPTY_EVENT_FLAG))
	{
		Neter::GetInstance().AddReadyCloseSession(GetSID());
	}
}

bool Neter::Session::TestEventFlag(event_flag_t e) const
{
	SpinLockGuard guard(event_flag_lock);
	return ((event_flag&e) == e) && e;
}

void Neter::Session::SetEventFlag(event_flag_t e)
{
	SpinLockGuard guard(event_flag_lock);
	(event_flag |= e) &= EVENT_FLAG_MASK;
}

void Neter::Session::ClrEventFlag(event_flag_t e)
{
	SpinLockGuard guard(event_flag_lock);
	event_flag &= ((~e)&EVENT_FLAG_MASK);
}

bool Neter::Session::TestAndModifyEventFlag(event_flag_t test, event_flag_t except, event_flag_t set, event_flag_t clear)
{
	SpinLockGuard guard(event_flag_lock);
	if((event_flag&test) == except)
	{
		(event_flag |= set) &= EVENT_FLAG_MASK;
		(event_flag &= ~clear);
		return true;
	}
	return false;
}

void Neter::Session::ConnectorWriteFunc()
{
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
			Log::Trace("client's ip: ", inet_ntoa(accept_addr.sin_addr), ", port: ", ntohs(accept_addr.sin_port));
			fcntl(new_fd, F_SETFL, fcntl(new_fd, F_GETFL) | O_NONBLOCK);
			// accept success
			SessionPtr ptr(new Session(Neter::GetInstance().GenerateSessionID(), Session::EXCHANGE_SESSION, new_fd));
			ptr->SetIP(GetIP());
			ptr->SetPort(GetPort());

			ptr->InitCallback(callback_ptr);
			epoll_event ev;
			ev.events = EPOLLIN|EPOLLOUT|EPOLLET;
			ev.data.u64 = ptr->GetSID();

			Neter::GetInstance().session_container.insert(std::make_pair(ptr->GetSID(), ptr));
			Neter::GetInstance().Ctrl(EPOLL_CTL_ADD, ptr->GetFD(), &ev);
		}
		else
		{
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
				Log::Error("Neter::Session::AcceptorReadFunc, accept failed, info=", strerror(errno));

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
		Log::Debug("Neter::Session::ExchangerReadFunc, cnt=", percnt);
		if(percnt > 0)
		{
			allcnt += percnt;
			read_data.insert(read_data.end(), read_buff, percnt);
		}
		else
		{
			if(errno == EINTR)
			{
			}
			else if(errno == EAGAIN)
			{
				break;
			}
			else
			{
				Log::Error("Neter::Session::ExchangerReadFunc, read failed, info=", strerror(errno));
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
	return ;
}

void Neter::Session::Read(SessionPtr ptr)
{
	if(false == ptr->TestAndModifyEventFlag(CLOSE_READY|READ_ACCESS|READ_PENDING, READ_ACCESS|READ_PENDING, Session::EMPTY_EVENT_FLAG, READ_PENDING|READ_ACCESS))
	{
		return ;
	}

	(ptr.get()->*(ptr->read_func_ptr))();
	
	if(ptr->IsExchangeSession())
	{
		ptr->callback_ptr->Deserialize(ptr->GetSID(), ptr->read_data);
	}
}

void Neter::Session::ExchangerWriteFunc()
{
	SendDataList tmp_send_data_list;
	{
		SpinLockGuard guard(send_data_list_lock);
		swap(tmp_send_data_list, send_data_list);
	}

	while(tmp_send_data_list.size() > 0)
	{
		const Octets &data = tmp_send_data_list.front();
		int per_cnt = write(fd, static_cast<const char *>(data.begin())+cursor_of_first_send_data, data.size() - cursor_of_first_send_data);
		if(per_cnt > 0)
		{
			cursor_of_first_send_data += per_cnt;
			if(cursor_of_first_send_data == data.size())
			{
				tmp_send_data_list.pop_front();
				cursor_of_first_send_data = 0;
			}
		}
		else
		{
			if(errno == EINTR)
			{
			}
			else if(errno == EAGAIN)
			{
				break;
			}
			else
			{
				Log::Error("Neter::Session::ExchangerWriteFunc, info=", strerror(errno));
				Close();
				return ;
			}
		}
	}

	if(tmp_send_data_list.size() > 0)
	{
		{
			SpinLockGuard guard(send_data_list_lock);
			auto rb = tmp_send_data_list.rbegin();
			auto re = tmp_send_data_list.rend();
			for(; rb != re; ++rb)
			{
				send_data_list.push_front(*rb);
			}
			//for(const auto &p : tmp_send_data_list)
			//{
			//	send_data_list.push_front(p);
			//}
		}
		SetEventFlag(WRITE_READY);
	}
	else
	{
		SetEventFlag(WRITE_ACCESS);
	}
}


void Neter::Session::Write(SessionPtr ptr)
{
	if(false == ptr->TestAndModifyEventFlag(CLOSE_READY|WRITE_ACCESS|WRITE_READY|WRITE_PENDING, WRITE_ACCESS|WRITE_READY|WRITE_PENDING, EMPTY_EVENT_FLAG, WRITE_READY|WRITE_PENDING))
	{
		return ;
	}

	(ptr.get()->*(ptr->write_func_ptr))();
}

bool Neter::Session::AppendSendData(SessionPtr ptr, const Octets &data)
{
	try
	{
		ptr->SetEventFlag(WRITE_READY);
		{
			SpinLockGuard guard(ptr->send_data_list_lock);
			ptr->send_data_list.push_back(data);
		}
		Neter::TryAddWriteTask(ptr);
	}
	catch(...)
	{
		Log::Error("Neter::Session::AppendSendData, sid=", ptr->GetSID());
		return false;
	}
	return true;
}
