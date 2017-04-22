#ifndef __SOCKET_H_
#define __SOCKET_H_

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string>
#include <string.h>

class Socket
{
public:
	Socket(int port);

	int Accept();
private:
	int port;
	int listen_fd;
	sockaddr_in addr;
};

#endif
