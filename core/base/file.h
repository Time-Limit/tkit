#ifndef _FILE_H_
#define _FILE_H_

#include <fcntl.h>
#include <string>
#include <memory>
#include <list>

#include "octets.h"
#include "cache.h"
#include "lock.h"
#include "trie.h"

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

public:
	static FileManager& GetInstance() { static FileManager m; return m; }

	FilePtr GetFilePtr(const std::string &);

private:
	SpinLock file_cache_lock;
	typedef Cache<std::string, FilePtr, LRU<std::string>, 2, Trie<std::string, FilePtr, 256> >  FileCache;
	FileCache file_cache;
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
