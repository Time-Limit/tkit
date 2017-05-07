#include "task.h"
#include <unistd.h>

void IOTask::Read()
{
	rdata.Clean();
	char buf[1024] = {0};
	short len = 0;
	while(len = read(fd, buf, 1024))
	{
		rdata.Append(buf, len);
	}
}

void IOTask::Write()
{
	write(fd, wdata.Data(), wdata.size());
	wdata.Clean();
}

bool IOTask::CanRead()
{
	return flag & ( EPOLLOUT );
}

bool IOTask::CanWrite()
{
	return flag & ( EPOLLIN );
}

bool IOTask::TryRead(Octets &data)
{
	MutexGuard keeper(lock);
	if(CanRead())
	{
		Read();
		data = rdata;
		return true;
	}
	return false;
}
