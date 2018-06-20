#ifndef _FILE_H_
#define _FILE_H_

#include "octets.h"
#include <fcntl.h>
#include <set>
#include <string>
#include "lock.h"
#include <memory>

namespace TCORE
{
class File
{
	enum
	{
		MAX_SIZE = 1024*1024*1024*10LL,
	};

	std::string name;
	std::string data;
	bool is_load;
public:
	const std::string Name() const { return name; }
	const std::string Data() const { return data; }
	const size_t Size() const { return data.size(); }

	explicit File(const std::string &name, bool load_flag = true);

	bool IsLoad() const { return is_load; }
};

class FileManager
{
public:
	typedef std::shared_ptr<File> FilePtr;

private:
	struct FilePtrCompare
	{
		bool operator()(const FilePtr &lhs, const FilePtr &rhs) const
		{
			return lhs->Name() < rhs->Name();
		}
	};

	typedef std::set<FilePtr, FilePtrCompare> FileSet;
	Mutex file_set_lock;
	FileSet file_set;

public:
	static FileManager& GetInstance() { static FileManager m; return m; }

	FilePtr GetFilePtr(const std::string &);
};

namespace
{

struct FileManagerIniter
{
	FileManagerIniter() { FileManager::GetInstance(); }
};

FileManagerIniter _file_manager_initer_;

}

}

#endif
