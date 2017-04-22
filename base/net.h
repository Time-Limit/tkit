#ifndef __NET_H_
#define __NET_H_

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string>
#include <map>
#include <set>
#include <string.h>
#include "exptype.h"
#include <channel.h>

class Net
{
public:
	Net();
	~Net();

private:
	int epoll_fd;

	int Add(int fd);
	int Mod(int fd);
	int Del(int fd);
};

#endif
