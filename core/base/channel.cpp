#include "channel.h"
#include "exptype.h"
#include "neter.h"
#include "thread.h"
#include <arpa/inet.h>

Channel::Channel(int f)
	: fd(f)
	, cid(ChannelManager::AllocID())
	, parser(nullptr)
	, ready_send(false)
	, ready_close(false)
	{
		fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);

		memset(ip, 0, sizeof(ip));
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
		Send();
	}
	Log::Trace("Channel::Handle, fd=%d, events=0x%x\n", fd, event->events);
}

void Channel::Send()
{
	if(IsClose()) return;
	{
		MutexGuard guarder(olock);	
		ready_send = false;	
		int per_cnt = 0, cnt = 0;
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
		}while(per_cnt >= 0 || errno == EINTR);

		obuff.clear();
	}
	
	Close();

	Log::Error("Channel::Send, errno=%d, info=%s\n", errno, strerror(errno));
}

void Channel::Recv()
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

	Log::Error("Channel::Recv, cnt=%d, errno=%d, info=%s\n", cnt, errno, strerror(errno));
}

void Channel::OnRecv()
{
	if(parser) parser->Append(ibuff);
	ibuff.clear();
	if(parser) parser->Parse();
}

void Channel::PutData(const char * buf, size_t size)
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

void Channel::Close()
{
	if(IsClose()) return;
	ready_close = true;
	ChannelManager::GetInstance().ReadyClose(this);
}

bool Acceptor::Listen(const char * ip, int port, ParserHatcher hatcher)
{
	int sockfd = 0;
	int optval = -1;
	struct sockaddr_in server;
	socklen_t socklen = sizeof(struct sockaddr_in);

	sockfd = socket(PF_INET, SOCK_STREAM, 0);

	if(sockfd < 0)
	{
		Log::Error("Acceptor::Init, socket, error=%s\n", strerror(errno));
		return false;
	}

	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
	memset(&server, 0, socklen);
	struct in_addr address;
	if(inet_pton(AF_INET, ip, &address) == -1)
	{
		Log::Error("Acceptor::inet_pton, ip=%s, error=%s\n", ip, strerror(errno));
		return false;
	}
	server.sin_addr.s_addr = address.s_addr;
	server.sin_port = htons(port);
	server.sin_family = AF_INET;

	if(bind(sockfd, (struct sockaddr *)&server, socklen) < 0)
	{
		Log::Error("Acceptor::Listen, bind, error=%s\n", strerror(errno));
		return false ;
	}

	listen(sockfd, LISTEN_QUEUE_SIZE);
	Log::Trace("Acceptor::Init, fd=%d\n", sockfd);	
	ChannelManager::GetInstance().Add(new Acceptor(sockfd, hatcher));

	return true;
}

void Channel::InitPeerName()
{
	struct sockaddr_in peer;
	int len = sizeof(sockaddr_in);
	if(getpeername(fd, (sockaddr *)&peer, (socklen_t *)&len) == 0)
	{
		inet_ntop(AF_INET, &peer.sin_addr, ip, sizeof(ip));
		Log::Trace("Channel::InitPeerName, ip=%s\n", ip);
	}
	else
	{
		memset(ip, 0, sizeof(ip));
		Log::Error("Channel::InitPeerName, getpeername failed, errno=%s\n", strerror(errno));
	}
}

void Acceptor::OnRecv()
{
	struct sockaddr_in accept_addr;
	int server_addr_len;
	int connect_fd;
	
	while((connect_fd = accept(fd, (struct sockaddr *)&accept_addr, (socklen_t *)&server_addr_len)) > 0 || errno == EINTR)
	{
		Log::Trace("Acceptor::Recv, connect_fd=%d\n", connect_fd);
		Channel *c = new Channel(connect_fd);
		c->SetParser(hatcher(c->GetCid()));
		c->InitPeerName();
		ChannelManager::GetInstance().Add(c);
	}
}

bool ChannelManager::PutData(channel_id_t cid, const char * buf, size_t size)
{
	MutexGuard guarder(lock);

	ChannelMap::iterator it = channel_map.find(cid);

	if(it == channel_map.end() || it->second == NULL)
	{
		return false;
	}

	it->second->PutData(buf, size);

	return true;
}

void ChannelManager::ReadyClose(Channel * c)
{
	MutexGuard guarder(lock);
	channel_map.erase(c->GetCid());
	ready_close_vector.push_back(c);
}

void ChannelManager::Add(Channel * c)
{
	MutexGuard guarder(lock);
	if(channel_map.find(c->GetCid()) != channel_map.end())
	{
		Log::Error("ChannelManager::Add, fatal error, addr=0x%x\n", c);
		c->Close();
		return ;
	}
	channel_map[c->GetCid()] = c;
	epoll_event ev;
	ev.events = EPOLLIN|EPOLLET|EPOLLET;
	ev.data.ptr = c;
	Log::Trace("ChannelManager::Add, fd=%d, addr=0x%x\n", c->fd, c);
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

