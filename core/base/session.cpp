#include "neter.h"

using namespace TCORE;

Neter::Session::Session(session_id_t s, SESSION_TYPE t, int f)
: sid(s)
, fd(f)
, type(t)
, is_need_deserialize(false)
, event_flag(0)
, cursor_of_first_send_data(0)
, read_func_ptr(&Session::DefaultReadFunc)
, write_func_ptr(&Session::DefaultWriteFunc)
, port(0)
, acceptor_use_secure_flag(false)
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
			SetNeedDeserialize(true);
		}break;
		case SECURE_EXCHANGE_SESSION:
		{
			read_func_ptr = &Session::SecureExchangerReadFunc;
			write_func_ptr = &Session::SecureExchangerWriteFunc;
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
		if(disconnect_callback)
		{
			disconnect_callback(GetSID(), GetIP(), GetPort());
		}
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
	int val = 0;
	socklen_t len = sizeof(int);
	int ret = getsockopt(GetFD(), SOL_SOCKET, SO_ERROR, &val, &len);

	if(ret != 0 || val != 0 || len != sizeof(int))
	{
		Log::Error("Neter::Session::ConenctorWriteFunc, ip=", GetIP()
				, ", port=", GetPort()
				, ", ret=", ret
				, ", val=", val
				, ", len=", len
				, ", info=", strerror(errno)
				);
		Close();
		return ;
	}

	write_func_ptr = &Session::ExchangerWriteFunc;
	read_func_ptr = &Session::ExchangerReadFunc;

	Log::Trace("Neter::Session::ConnectorWriteFunc, connect success, ip=", GetIP()
				, ", port=", GetPort());

	ClrEventFlag(Session::WRITE_READY);
	SetNeedDeserialize(true);

	connect_callback(GetSID());
}

void Neter::Session::AcceptorReadFunc()
{
	int new_fd = -1;
	struct sockaddr_in accept_addr;
	socklen_t server_addr_len = sizeof(accept_addr);

	for(;;)
	{
		new_fd = accept(GetFD(), (struct sockaddr *)&accept_addr, (socklen_t *)&server_addr_len);

		if(new_fd > 0)
		{
			fcntl(new_fd, F_SETFL, fcntl(new_fd, F_GETFL) | O_NONBLOCK);

			SessionPtr ptr(new Session(Neter::GetInstance().GenerateSessionID(), GetSecureFlag() ? Session::SECURE_EXCHANGE_SESSION : Session::EXCHANGE_SESSION, new_fd));
			server_addr_len = sizeof(accept_addr);
			getpeername(new_fd, (struct sockaddr *)&accept_addr, &server_addr_len);
			char ip_buff[16] = "0.0.0.0";
			inet_ntop(AF_INET, &accept_addr.sin_addr, ip_buff, sizeof(ip_buff));
			ip_buff[15] = 0;
			ptr->SetIP(ip_buff);
			ptr->SetPort(ntohs(accept_addr.sin_port));
			if(connect_callback)
			{
				connect_callback(ptr->GetSID());
				ptr->SetConnectCallback(connect_callback);
			}
			ptr->SetDisconnectCallback(disconnect_callback);
			ptr->InitCallback(callback_ptr);
			if(GetSecureFlag())
			{
				SSLPtr ssl_ptr(SSL_new(ssl_ctx_ptr.get()), SSL_free);
				SSL_set_accept_state(ssl_ptr.get());
				SSL_set_fd(ssl_ptr.get(), fd);
				ptr->SetEventFlag(WRITE_READY);
				ptr->SetSSLPtr(ssl_ptr);
			}

			Log::Trace("Neter::Session::AcceptorReadFunc, client's ip: ", ptr->GetIP(), ", port: ", ptr->GetPort());
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
	const static size_t READ_BUFF_SIZE = 1024;
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
	const static size_t READ_BUFF_SIZE = 1024;
	unsigned char read_buff[READ_BUFF_SIZE];
	size_t allcnt = 0;
	size_t percnt = 0;
	int res = 0;

	for(;;)
	{
		res = SSL_read_ex(ssl_ptr.get(), read_buff, READ_BUFF_SIZE, &percnt);
		Log::Debug("Neter::Session::SecureExchangeReadFunc",
				", res=", res,
				", percnt=", percnt);
		if(res == 1)
		{
			allcnt += percnt;
			read_data.insert(read_data.end(), read_buff, percnt);
		}
		else
		{
			int error = SSL_get_error(ssl_ptr.get(), res);
			if(error == SSL_ERROR_WANT_READ)
			{
				break;
			}
			else if(error == SSL_ERROR_WANT_WRITE)
			{
				SetEventFlag(WRITE_READY);
				break;
			}
			else if(error == SSL_ERROR_NONE)
			{
			}
			else if(error == SSL_ERROR_ZERO_RETURN)
			{
					Close();
					return ;
			}
			else
			{
				Close();
				return ;
			}
		}
	}

	return ;
}

void Neter::Session::SecureExchangerWriteFunc()
{
	SendDataList tmp_send_data_list;
	{
		SpinLockGuard guard(send_data_list_lock);
		swap(tmp_send_data_list, send_data_list);
	}

	while(tmp_send_data_list.size() > 0)
	{
		const Octets &data = tmp_send_data_list.front();
		int res = 0;
		size_t percnt = 0;
		res = SSL_write_ex(ssl_ptr.get(), static_cast<const char *>(data.begin())+cursor_of_first_send_data, data.size() - cursor_of_first_send_data, &percnt);
		if(res == 1)
		{
			cursor_of_first_send_data += percnt;
			if(cursor_of_first_send_data == data.size())
			{
				tmp_send_data_list.pop_front();
				cursor_of_first_send_data = 0;
			}
		}
		else
		{
			int error = SSL_get_error(ssl_ptr.get(), res);
			if(error == SSL_ERROR_WANT_READ)
			{
				break;
			}
			else if(error == SSL_ERROR_WANT_WRITE)
			{
				break;
			}
			else if(error == SSL_ERROR_NONE)
			{
			}
			else if(error == SSL_ERROR_ZERO_RETURN)
			{
					Close();
					return ;
			}
			else
			{
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

void Neter::Session::Read(SessionPtr ptr)
{
	if(false == ptr->TestAndModifyEventFlag(CLOSE_READY|READ_ACCESS|READ_PENDING, READ_ACCESS|READ_PENDING, Session::EMPTY_EVENT_FLAG, READ_PENDING|READ_ACCESS))
	{
		return ;
	}

	(ptr.get()->*(ptr->read_func_ptr))();
	
	if(ptr->IsNeedDeserialize())
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
