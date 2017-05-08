#ifndef _SESSION_H_
#define _SESSION_H_

#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>

class Session
{
public:
	Session(int fd);
	~Session();

	int Recv(Octets &data);
	int Send(const Octets &data);

private:

	int sock;
	pthread_mutex_lock ilock, olock;

	Octest idata, odata;
}

#endif
