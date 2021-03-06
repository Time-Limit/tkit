#include "file.h"
#include "log.h"
#include <errno.h>
#include <string.h>
#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <algorithm>

using namespace TCORE;

File::File(const std::string &n, bool load_flag)
: name(n)
, is_load(false)
{
	if(load_flag == false)
	{
		return ;
	}
	std::ifstream t(name);
	t.seekg(0, std::ios::end);
	size_t length = t.tellg();
	t.seekg(0, std::ios::beg);
	if(length >= MAX_SIZE)
	{
		return ;
	}
	try
	{
		data = std::string(std::istreambuf_iterator<char>(t), std::istreambuf_iterator<char>());
		is_load = true;
	}
	catch(...)
	{
		data = std::string();
		is_load = false;

		Log::Error("File::File, load failed, name=", name, " length=", length);
	}
}

FileManager::FilePtr FileManager::GetFilePtr(const std::string &name)
{
	{
		SpinLockGuard guard(file_cache_lock);
		FilePtr fptr;
		if(file_cache.find(name, fptr))
		{
			Log::Debug("FileManager::GetFilePtr, cache, name=", name);
			return fptr;
		}
	}

	FilePtr fptr(new File(name));
	
	if(fptr->IsLoad())
	{
		{
			SpinLockGuard guard(file_cache_lock);
			if(file_cache.insert(name, fptr))
			{
				Log::Debug("FileManager::GetFilePtr, insert, name=", name);
			}
		}
		return fptr;
	}

	return FilePtr(nullptr);
}
