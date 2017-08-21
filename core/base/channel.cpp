#include "channel.h"
#include "exptype.h"
#include "neter.h"

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
	{
		MutexGuard guarder(olock);	
		ready_send = false;	
		size_t per_cnt = 0, cnt = 0;
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
	printf("len=%ld\n", ibuff.size());
	printf("%.*s\n", ibuff.size(), ibuff.begin());
	ibuff.clear();
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
	ChannelManager::GetInstance().ReadyClose(this);
}

bool Acceptor::Listen(int port)
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
	
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(port);
	server.sin_family = AF_INET;

	if(bind(sockfd, (struct sockaddr *)&server, socklen) < 0)
	{
		Log::Error("Acceptor::Listen, bind, error=%s\n", strerror(errno));
		return false ;
	}

	listen(sockfd, LISTEN_QUEUE_SIZE);
	Log::Trace("Acceptor::Init, fd=%d\n", sockfd);	
	ChannelManager::GetInstance().Add(new Acceptor(sockfd));

	return true;
}

void Acceptor::Recv()
{
	struct sockaddr_in accept_addr;
	int server_addr_len;
	int connect_fd;
	
	while((connect_fd = accept(fd, (struct sockaddr *)&accept_addr, (socklen_t *)&server_addr_len)) > 0)
	{
		Log::Trace("Acceptor::Recv, connect_fd=%d\n", connect_fd);
		ChannelManager::GetInstance().Add(new Channel(connect_fd));
	}
}

bool ChannelManager::PutData(channel_id_t cid, const char * buf, size_t size)
{
	MutexGuard guarder(lock);

	ChannelSet::iterator it = channel_set.find((Channel *)cid);

	if(it == channel_set.end())
	{
		return false;
	}

	(*it)->PutData(buf, size);

	return true;
}

void ChannelManager::ReadyClose(Channel * c)
{
	MutexGuard guarder(lock);
	channel_set.erase(c);
	ready_close_vector.push_back(c);
}

void ChannelManager::Add(Channel * c)
{
	MutexGuard guarder(lock);
	if(channel_set.find(c) != channel_set.end())
	{
		Log::Error("ChannelManager::Add, fatal error, addr=0x%x\n", c);
		assert(false);
	}
	channel_set.insert(c);
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
		Neter::GetInstance().Ctl(EPOLL_CTL_MOD, (*it)->fd, &event);
		delete *it;
	}

	ready_close_vector.clear();
}
