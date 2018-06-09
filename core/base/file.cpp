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
	MutexGuard guarder(file_set_lock);

	//FileSet::iterator it = std::find_if(file_set.begin(), file_set.end(), [&name](const FilePtr& rhs)->bool { return rhs->Name() == name; });
	FileSet::iterator it = file_set.find(FilePtr(new File(name, true)));

	return *it;
	
	FilePtr fptr(new File(name));
	
	if(fptr->IsLoad())
	{
		file_set.insert(fptr);
		return fptr;
	}

	return FilePtr(nullptr);
}
