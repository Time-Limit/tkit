#ifndef _FILE_H_
#define _FILE_H_

#include "octets.h"
#include <fcntl.h>
#include <set>
#include <string>

class File
{
	friend class FileManager;
private:
	std::string name;
	int flag;
	mode_t mode;
	std::string content;
	int fd;
	int error_number;
public:
	File(const std::string &_name, int _flag = O_RDONLY, mode_t _mode = 0, bool readall = true);
	~File();

	void Read();

	const std::string& GetContent() const { return content; }
};

class FileManager
{
private:
	//typedef std::set<File, [](const File &lhs, const File &rhs) ->bool { return lhs.name < rhs.name; }> FileSet;
	//FileSet file_set;

public:
	FileManager& GetInstance() { static FileManager m; return m; }

	std::string GetFileData(const std::string &);
};

#endif
