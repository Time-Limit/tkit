#include "pthread.h"
#include <queue>

class ThreadPool
{
public:
	typedef void *(*Handler) (void *);
	typedef void * Args;
	typedef std::pair<Handler, Args> Task;

	ThreadPool(size_t _size = 64);

	ThreadPool(const ThreadPool &rhs) = delete;
	ThreadPool &operator = (const ThreadPool &rhs) = delete;

	~ThreadPool();

	bool AddTask(Handler handler, Args args);

	static void *Work(void *args);

	enum THREAD_FLAG
	{
		_T_EXIT    = 0x00000001,
		_T_REFUSE  = 0x00000002,
	};

	inline bool TestFlag(THREAD_FLAG f)
	{
		return flag&f;
	}

private:
	size_t size;

	unsigned int flag;	

	inline void SetFlag(THREAD_FLAG f)
	{
		flag |= f;
	}

	inline void ClrFlag(THREAD_FLAG f)
	{
		if(flag&f)
		{
			flag ^= f;
		}
	}

	std::queue< Task > task_queue;
	pthread_t *thread_pool;
	
	pthread_cond_t task_ready;
	pthread_mutex_t task_lock;
};
