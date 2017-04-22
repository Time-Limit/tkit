#include "channel.h"

Channel::Channel(int _fd) : fd(_fd) {}

int Channel::Recv()
{
	size_t size = 0, len = 0;
	char buff[1024];
	data = Octets();
	while(recv(fd, buff, size, 0))
	{
		data.Append(buff, size);
		len += size;
	}

	return len;
}

int Channel::Send(const Octets &data)
{
	return send(fd, data.Data(), data.Size(), 0);
}
