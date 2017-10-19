#ifndef _FILE_H_
#define _FILE_H_

#include "octets.h"
#include <fstream>
#include <set>

class File
{
	friend class FileManager;
private:
	std::string name;
	std::string content;
public:
	File(const std::string &);

	std::string GetContent() { return content; }
};

class FileManager
{
private:
	//typedef std::set<File, [](const File &lhs, const File &rhs) ->bool { return lhs.name < rhs.name; }> FileSet;
	//FileSet file_set;

public:
	FileManager& GetInstance() { static FileManager m; return m; }

	File GetFile(const std::string &);
};

#endif
