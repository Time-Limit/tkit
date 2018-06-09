#include "channel.h"
#include "exptype.h"
#include "neter.h"
#include "thread.h"
#include <arpa/inet.h>

using namespace TCORE;

Channel::Channel(int f)
	: fd(f)
	, ready_close(false)
{
	fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}

void Channel::Handle(const epoll_event *event)
{
	if(event->events & (EPOLLIN | EPOLLERR | EPOLLHUP))
	{
		Recv();
		OnRecv();
	}
	if(event->events & EPOLLOUT)
	{
		OnSend();
	}
	Log::Trace("Channel::Handle, fd=", fd, ", events=0x", std::hex, event->events, std::dec);
}

void Channel::Close()
{
	if(IsClose()) return;
	ready_close = true;
	Neter::GetInstance().ReadyClose(this);
}

void Exchanger::Send(const void * buf, size_t size)
{
	if(IsClose()) return ;

	Log::Debug("Exchanger::Send, size=", size);

	static const size_t OBUFF_SIZE_LIMIT = 256*1024*1024; 
	{
		MutexGuard guarder(olock);
		if(obuff.size() + size > OBUFF_SIZE_LIMIT)
		{
			Log::Error("Exchanger::Send, size limit exceed, ip=", ip, ", size=", obuff.size() + size);
			Close();
			return ;
		}
		obuff.insert(obuff.end(), buf, size);
		RegisterSendEvent();
	}

	Log::Debug("Exchanger::Send, all=", obuff.size());
}

void Exchanger::OnSend()
{
	if(IsClose()) return;

	MutexGuard guarder(olock);
	ready_send = false;
	int per_cnt = write(fd, ((char *)obuff.begin()) + cur_cursor, obuff.size() - cur_cursor);
	if(per_cnt < 0)
	{
		if(errno == EAGAIN || errno == EINTR)
		{
			RegisterSendEvent();
			return ;
		}
		Log::Error("Exchanger::Send, errno=", errno, ", info=", strerror(errno));
		obuff.clear();
		cur_cursor = 0;
		Close();
		return ;
	}
	if((size_t)per_cnt == obuff.size() - cur_cursor)
	{
		Log::Debug("Exchanger::Send, all send, cur_cursor=", cur_cursor, ", per_cnt=", per_cnt, ", size=", obuff.size());
		cur_cursor = 0;
		obuff.clear();
		return ;
	}
	cur_cursor += per_cnt;
	RegisterSendEvent();
}

void Exchanger::Recv()
{
	if(IsClose()) return;
	int cnt = 0, per_cnt = 0;
	do
	{
		while((per_cnt = read(fd, tmp_buff, TMP_BUFF_SIZE)) > 0)
		{
			ibuff.insert(ibuff.end(), tmp_buff, per_cnt);
			cnt += per_cnt;
			if(per_cnt < TMP_BUFF_SIZE)
			{
				return ;
			}
		}
	}while(per_cnt < 0 && errno == EINTR);

	if(cnt && per_cnt < 0 && errno == EAGAIN)
	{
		return ;
	}
	
	Close();

	Log::Error("Exchanger::Recv, cnt=", cnt, ", errno=", errno, ", info=", strerror(errno));
}

void Exchanger::OnRecv()
{
	if(IsClose()) return;
	session->DataIn(ibuff);
	ibuff.clear();
}

void Exchanger::InitPeerName()
{
	struct sockaddr_in peer;
	memset(ip, 0, sizeof(ip));
	int len = sizeof(sockaddr_in);
	if(getpeername(fd, (sockaddr *)&peer, (socklen_t *)&len) == 0)
	{
		inet_ntop(AF_INET, &peer.sin_addr, ip, sizeof(ip));
		Log::Trace("Exchanger::InitPeerName, ip=", ip);
	}
	else
	{
		Close();
		Log::Error("Exchanger::InitPeerName, getpeername failed, errno=", strerror(errno));
	}
}

void Exchanger::RegisterSendEvent()
{
	if(ready_send == false)
	{
		ready_send = true;
		epoll_event event;
		event.events = EPOLLIN | EPOLLOUT | EPOLLET;
		event.data.ptr = this;
		Neter::GetInstance().Ctl(EPOLL_CTL_MOD, fd, &event);
	}
}

void Exchanger::RegisterRecvEvent()
{
	if(ready_send == false)
	{
		ready_send = true;
		epoll_event event;
		event.events = EPOLLIN | EPOLLOUT | EPOLLET;
		event.data.ptr = this;
		Neter::GetInstance().Ctl(EPOLL_CTL_MOD, fd, &event);
	}
}

void Acceptor::OnRecv()
{
	if(IsClose())
	{
		return ;
	}
	struct sockaddr_in accept_addr;
	int server_addr_len;
	int connect_fd;
	
	while((connect_fd = accept(fd, (struct sockaddr *)&accept_addr, (socklen_t *)&server_addr_len)) > 0 || errno == EINTR)
	{
		Log::Debug("Acceptor::OnRecv, connect_fd=", connect_fd);

		accept_handle(connect_fd);
	}
}

bool Acceptor::Listen(const char * ip, int port, AcceptHandle ah)
{
	if(!ip)
	{
		Log::Error("Acceptor::Listen, invalid ip address.");
		return false;
	}
	int sockfd = 0;
	int optval = -1;
	struct sockaddr_in server;
	socklen_t socklen = sizeof(struct sockaddr_in);

	sockfd = socket(PF_INET, SOCK_STREAM, 0);

	if(sockfd < 0)
	{
		Log::Error("Acceptor::Init, socket, error=", strerror(errno));
		return false;
	}

	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
	memset(&server, 0, socklen);
	struct in_addr address;
	if(inet_pton(AF_INET, ip, &address) == -1)
	{
		Log::Error("Acceptor::inet_pton, ip=", ip, ", error=", strerror(errno));
		return false;
	}
	server.sin_addr.s_addr = address.s_addr;
	server.sin_port = htons(port);
	server.sin_family = AF_INET;

	if(bind(sockfd, (struct sockaddr *)&server, socklen) < 0)
	{
		Log::Error("Acceptor::Listen, bind, error=", strerror(errno));
		return false ;
	}

	listen(sockfd, LISTEN_QUEUE_SIZE);
	Log::Debug("Acceptor::Init, fd=", sockfd);	

	epoll_event ev;
	ev.events = EPOLLIN|EPOLLET;
	Acceptor *acceptor = new Acceptor(sockfd, ah);
	ev.data.ptr = acceptor;
	Neter::GetInstance().Ctl(EPOLL_CTL_ADD, acceptor->ID(), &ev);

	return true;
}

bool Connector::Connect()
{
	int socketfd = socket(AF_INET,SOCK_STREAM,0);
	struct sockaddr_in sockaddr;
	memset(&sockaddr,0,sizeof(sockaddr));
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = htons(port);
	inet_pton(AF_INET,ip.c_str(), &sockaddr.sin_addr);
	if( (connect(socketfd,(struct sockaddr*)&sockaddr,sizeof(sockaddr))) < 0 )
	{
		Log::Error("Connector::connect, ip=", ip, " prot=", port, " connect error ", strerror(errno));
		return false;
	}

	connect_handle(socketfd);

	return true;
}

void SecureExchanger::OnSend()
{

	if(IsClose()) return;

	if(!is_handshake_finish)
	{
		Handshake();
		return ;
	}

	MutexGuard guarder(olock);
	ready_send = false;
	int per_cnt = SSL_write(ssl, ((char *)obuff.begin()) + cur_cursor, obuff.size() - cur_cursor);
	if(per_cnt < 0)
	{
		int error = SSL_get_error(ssl, per_cnt);
		if(error == SSL_ERROR_WANT_WRITE)
		{
			RegisterRecvEvent();
			return ;
		}
		Log::Error("SecureExchanger::OnSend, info=", strerror(errno));
		obuff.clear();
		cur_cursor = 0;
		Close();
		return ;
	}
	if((size_t)per_cnt == obuff.size() - cur_cursor)
	{
		cur_cursor = 0;
		obuff.clear();
		return ;
	}
	cur_cursor += per_cnt;
	RegisterSendEvent();
}

bool SecureExchanger::Handshake()
{
	if(is_handshake_finish)
	{
		return true;
	}

	if(!ssl)
	{
		return false;
	}

	int r = 0;
	if(!is_init_ssl)
	{
		r = SSL_set_fd(ssl, fd);
		if(r != 1)
		{
			int error = SSL_get_error(ssl, r);
			Log::Error("SecureExchanger::Handshake, info=", error);
			return false;
		}

		SSL_set_accept_state(ssl);

		is_init_ssl = true;
	}

	r = SSL_do_handshake(ssl);

	if(r != 1)
	{
		int error = SSL_get_error(ssl, r);
		if(error == SSL_ERROR_WANT_READ)
		{
			RegisterSendEvent();
			return true;
		}
		else if(error == SSL_ERROR_WANT_WRITE)
		{
			RegisterRecvEvent();
			return true;
		}

		Log::Error("SecureExchanger::Handshake, info=", error);
		return false;
	}

	is_handshake_finish = true;

	return true;
}

void SecureExchanger::Recv()
{
	if(IsClose()) return;

	if(is_handshake_finish == false)
	{
		Handshake();
		return ;
	}

	int cnt = 0, per_cnt = 0, error = 0;
	for(;;)
	{
		per_cnt = SSL_read(ssl, tmp_buff, TMP_BUFF_SIZE);
		if(per_cnt > 0)
		{
			ibuff.insert(ibuff.end(), tmp_buff, per_cnt);
			cnt += per_cnt;
			if(per_cnt < TMP_BUFF_SIZE)
			{
				return ;
			}
		}
		else if(per_cnt < 0)
		{
			error = SSL_get_erorr(ssl, per_cnt);
			if(error == SSL_ERROR_WANT_READ)
			{
				RegisterRecvEvent()
			}
		}
		else
		{
			Log::Error("SecureExchanger::Recv, cnt=", cnt, ", error=", error);
			Close();
			return ;
		}
	}
}

