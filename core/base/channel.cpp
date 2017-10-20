#include "channel.h"
#include "exptype.h"
#include "neter.h"
#include "thread.h"
#include <arpa/inet.h>

Channel::Channel(int f)
	: fd(f)
	, cid(ChannelManager::AllocID())
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
	LOG_TRACE("Channel::Handle, fd=%d, events=0x%x", fd, event->events);
}

void Channel::Close()
{
	if(IsClose()) return;
	ready_close = true;
	ChannelManager::GetInstance().ReadyClose(this);
}

void Exchanger::Send(const void * buf, size_t size)
{
	MutexGuard guarder(olock);
	obuff.insert(obuff.end(), buf, size);
	if(ready_send == false)
	{
		ready_send = true;
		epoll_event event;
		event.events = EPOLLIN | EPOLLOUT | EPOLLET;
		event.data.ptr = this;
		Neter::GetInstance().Ctl(EPOLL_CTL_MOD, fd, &event);
	}
}

void Exchanger::OnSend()
{
	if(IsClose()) return;
	{
		MutexGuard guarder(olock);	
		ready_send = false;	
		int per_cnt = 0;
		size_t cnt = 0;
		do
		{
			if((per_cnt = write(fd, ((char *)obuff.begin()) + cnt, obuff.size() - cnt)) >= 0)
			{
				if((cnt += per_cnt) == obuff.size())
				{
					obuff.clear();
					return;
				}
			}
		}while(per_cnt >= 0 || errno == EINTR || errno == EAGAIN);

		obuff.clear();
	}
	
	Close();

	LOG_ERROR("Exchanger::Send, errno=%d, info=%s", errno, strerror(errno));
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

	LOG_ERROR("Exchanger::Recv, cnt=%d, errno=%d, info=%s", cnt, errno, strerror(errno));
}

void Exchanger::OnRecv()
{
	if(IsClose()) return;
	parser->Append(ibuff);
	ibuff.clear();
	parser->Parse(ID());
}

void Exchanger::InitPeerName()
{
	struct sockaddr_in peer;
	int len = sizeof(sockaddr_in);
	if(getpeername(fd, (sockaddr *)&peer, (socklen_t *)&len) == 0)
	{
		inet_ntop(AF_INET, &peer.sin_addr, ip, sizeof(ip));
		LOG_TRACE("Exchanger::InitPeerName, ip=%s", ip);
	}
	else
	{
		memset(ip, 0, sizeof(ip));
		LOG_ERROR("Exchanger::InitPeerName, getpeername failed, errno=%s", strerror(errno));
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
		LOG_TRACE("Acceptor::Recv, connect_fd=%d", connect_fd);
		ChannelManager::GetInstance().Add(hatcher(connect_fd));
	}
}

bool Acceptor::Listen(const char * ip, int port, ExchangerHatcher hatcher)
{
	if(!hatcher || !ip)
	{
		LOG_ERROR("Acceptor::Listen, invalid argument.");
		return false;
	}
	int sockfd = 0;
	int optval = -1;
	struct sockaddr_in server;
	socklen_t socklen = sizeof(struct sockaddr_in);

	sockfd = socket(PF_INET, SOCK_STREAM, 0);

	if(sockfd < 0)
	{
		LOG_ERROR("Acceptor::Init, socket, error=%s", strerror(errno));
		return false;
	}

	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
	memset(&server, 0, socklen);
	struct in_addr address;
	if(inet_pton(AF_INET, ip, &address) == -1)
	{
		LOG_ERROR("Acceptor::inet_pton, ip=%s, error=%s", ip, strerror(errno));
		return false;
	}
	server.sin_addr.s_addr = address.s_addr;
	server.sin_port = htons(port);
	server.sin_family = AF_INET;

	if(bind(sockfd, (struct sockaddr *)&server, socklen) < 0)
	{
		LOG_ERROR("Acceptor::Listen, bind, error=%s", strerror(errno));
		return false ;
	}

	listen(sockfd, LISTEN_QUEUE_SIZE);
	LOG_TRACE("Acceptor::Init, fd=%d", sockfd);	
	ChannelManager::GetInstance().Add(new Acceptor(sockfd, hatcher));

	return true;
}

bool ChannelManager::Send(channel_id_t cid, const char * buf, size_t size)
{
	MutexGuard guarder(lock);

	ChannelMap::iterator it = channel_map.find(cid);

	if(it == channel_map.end() || it->second == NULL)
	{
		return false;
	}

	it->second->Send(buf, size);

	return true;
}

void ChannelManager::ReadyClose(Channel * c)
{
	MutexGuard guarder(lock);
	channel_map.erase(c->ID());
	ready_close_vector.push_back(c);
}

void ChannelManager::Add(Channel * c)
{
	MutexGuard guarder(lock);
	if(channel_map.find(c->ID()) != channel_map.end())
	{
		LOG_ERROR("ChannelManager::Add, fatal error, addr=0x%p", c);
		c->Close();
		return ;
	}
	channel_map[c->ID()] = c;
	epoll_event ev;
	ev.events = EPOLLIN|EPOLLET|EPOLLET;
	ev.data.ptr = c;
	LOG_TRACE("ChannelManager::Add, fd=%d, addr=0x%p", c->fd, c);
	Neter::GetInstance().Ctl(EPOLL_CTL_ADD, c->fd, &ev);
}

void ChannelManager::Close()
{
	MutexGuard guarder(lock);
	ChannelVector::iterator it = ready_close_vector.begin();
	ChannelVector::iterator ie = ready_close_vector.end();
	for(; it != ie; ++it)
	{
		static epoll_event event;
		Neter::GetInstance().Ctl(EPOLL_CTL_DEL, (*it)->fd, &event);
		delete *it;
	}

	ready_close_vector.clear();
}

