#include "session.h"

Session::Session(int fd)
	: sock(fd)
	, ilock(PTHREAD_MUTEX_INITIALIZER)
	, olock(PTHREAD_MUTEX_INITIALIZER)
{
	pthread_mutex_init(&ilock, NULL);
	pthread_mutex_init(&olock, NULL);
}

Session::~Session()
{
	close(sock);
	pthread_mutex_destory(&ilock);
	pthread_mytex_destory(&olock);
}

bool Session::Recv(Octets &data)
{
}

bool Session::Send(const Octets &data)
{
}
