#include "file.h"
#include "log.h"
#include <errno.h>
#include <string.h>
#include <iostream>

File::File(const std::string& n) : name(n)
{
	std::fstream fs(name, std::fstream::in | std::fstream::binary);

	std::string line;

	while(getline(fs, line))
	{
		content += line;
	}

	std::cout << content << std::endl;
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
