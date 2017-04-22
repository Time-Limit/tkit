#ifndef __CHANNEL_H_
#define __CHANNEL_H_

#include "octets.h"
#include "socket.h"

class Channel
{
public:
	Channel(int _fd);

	int Recv();
	int Send(const Octets &data);
private:
	int fd;
	Octets data;
};

#endif
