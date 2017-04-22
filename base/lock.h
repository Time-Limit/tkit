#ifndef __LOCK_H_
#define __LOCK_H_

#include <pthread.h>
#include <errno.h>

template<typename T>
class LockGuard
{
public:
	LockGuard(T &lock)
	{
		_lock = &lock;
		_lock->Lock();
	}

	~LockGuard()
	{
		if(_lock)
		{
			_lock->Unlock();
		}
	}

private:
	T *_lock;
};

class Mutex
{
        pthread_mutex_t m_lock;
public:
        Mutex();
        ~Mutex();

        int Lock();
        int Unlock();
        bool TryLock();
};
typedef LockGuard<Mutex> MutexGuard;


#endif
