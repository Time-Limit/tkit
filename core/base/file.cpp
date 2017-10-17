#include "file.h"
#include "log.h"
#include <errno.h>
#include <string.h>
#include <iostream>

File::File(const std::string& n) : name(n)
{
	std::fstream fs(name, std::fstream::in | std::fstream::binary);

	if(!fs)
	{
		Log::Error("File::File, open failed, file=%s\n", name.c_str());
		return ;
	}

	fs.seekg(0, std::ios::end);
	size_t cnt = fs.tellg();
	fs.seekg(0, std::ios::beg);

	Log::Trace("File::File, file=%s, size=%d\n", name.c_str(), cnt);

	content = std::string(cnt, '\0');
	
	fs.read((char *)content.c_str(), cnt);

	fs.close();
}

File FileManager::GetFile(const std::string &name)
{
	//auto it = file_set.find(name);
	//if(it != file_set.end())
	//{
	//	return *it;
	//}

	return File(name);
}
