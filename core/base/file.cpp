#include "file.h"
#include "log.h"
#include <errno.h>
#include <string.h>
#include <iostream>
#include <sys/types.h>
#include <unistd.h>

using namespace TCORE;

/*
File::File(const std::string& n) : name(n)
{
	std::fstream fs(name, std::fstream::in | std::fstream::out | std::fstream::binary);

	if(!fs)
	{
		Log::Error("File::File, open failed, file=%s", name.c_str());
		return ;
	}

	fs.seekg(0, std::ios::end);
	size_t cnt = fs.tellg();
	fs.seekg(0, std::ios::beg);

	LOG_TRACE("File::File, file=%s, size=%zu", name.c_str(), cnt);

	content = std::string(cnt, '\0');
	
	fs.read((char *)content.c_str(), cnt);

	fs.close();
}
*/

File::File(const std::string &_name, int _flag, mode_t _mode, bool readall)
: name(_name), flag(_flag), mode(_mode), error_number(0)
{
	fd = open(name.c_str(), flag, mode);
	if(fd == -1)
	{
		error_number = errno;
		Log::Error("File::File, open failed, name=", name, ", info=", strerror(error_number));

		return ;
	}

	int len = lseek(fd, 0L, SEEK_END);

	if(len < 0)
	{
		return ;
	}


	if(readall)
	{
		Read();
	}
}

void File::Read()
{
	int len = lseek(fd, 0L, SEEK_END);
	lseek(fd, 0L, SEEK_SET);
	char *buf = new char[len];
	if(!buf)
	{
		error_number = errno;
		Log::Error("File::Read, open failed, name=", name, ", info=", strerror(error_number));
		return ;
	}
	size_t sum = 0;
	while(len > 0)
	{
		int part = read(fd, buf + sum, len);
		if(part == -1)
		{
			error_number = errno;
			Log::Error("File::Read, open failed, name=", name, ", info=", strerror(error_number));
			return ;
		}
		len -= part;
		sum += part;
	}

	content = std::string(buf, sum);

	delete [] buf;
}

File::~File()
{
	if(fd != -1) close(fd);
}

std::string FileManager::GetFileData(const std::string &name)
{
	//auto it = file_set.find(name);
	//if(it != file_set.end())
	//{
	//	return *it;
	//}

	return File(name).content;
}
