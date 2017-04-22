#include "file.h"
#include <string.h>
#include <stdexcept>

File::File(const char *_path, int _flag, int _mode)
	: path(_path, strlen(_path))
	, flag(_flag)
	, mode(_mode)
	, fd(open(_path, flag, mode))
	, lock()
{
	if(fd == -1)
	{
		char buf[1025];
		sprintf(buf, "File::File, path=%*s , mode=%d, flag=%d, msg=%s\n", path.Size(), (char *)path.Data(), mode, flag, strerror(errno));

		throw(std::runtime_error(buf));
	}
}

File::~File()
{
	if(fd != -1)
	{
		close(fd);
	}
}

int File::Read(size_t size, Octets &data)
{
	MutexGuard keeper(lock);

	int count = 0, part = 0;

	while(count < size)
	{
		part = read(fd, buf, _min(MAXLEN, size-count));
		if(part == 0)
		{
			return count;
		}
		data.Append(buf, part);
		count += part;
	}
	return count;
}

int File::Read(Octets &data)
{
	MutexGuard keeper(lock);

	int count = 0, part = 0;

	do
	{
		part = read(fd, buf, MAXLEN);
		if(part == 0)
		{
			return count;
		}
		data.Append(buf, part);
		count += part;
	}while(part);
}

int File::Write(const Octets &data)
{
	return write(fd, data.Data(), data.Size());
}

bool File::Reset()
{
	return lseek(fd, 0, SEEK_SET) != -1;
}

bool File::IsExist(const char *_path)
{
	int fd = open(_path, O_EXCL, 0);
	if(fd == -1)
	{
		if(errno== ENOENT)
		{
			return true;
		}
		char buf[1025];
		sprintf(buf, "File::IsExist, path=%s, msg=%s\n", _path, strerror(errno));
		throw(std::runtime_error(buf));
	}
	else
	{
		close(fd);
	}
	return false;
}
