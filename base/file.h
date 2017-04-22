#ifndef __FILE_H_
#define __FILE_H_

#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include "exptype.h"
#include "lock.h"
#include "octets.h"

class File
{
public:
	File(const char *_path, int _flag, int _mode);
	virtual ~File();
public:
	int Read(size_t size, Octets &data);
	int Read(Octets &data);
	int Write(const Octets &data);
	bool Reset();

	static bool IsExist(const char *_path);
private:
	Octets path;
	int mode;
	int flag;
	int fd;
	Mutex lock;

	enum
	{
		MAXLEN = 1024,
	};

	char *buf[MAXLEN];
};

#endif
