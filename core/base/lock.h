#ifndef _LOCK_H_
#define _LOCK_H_

#include <sys/time.h>
#include <pthread.h>
#include <errno.h>
#include <assert.h>

class Mutex
{
	pthread_mutex_t m_lock;
public:
	explicit Mutex()
	{ 
		pthread_mutexattr_t attr;
		pthread_mutexattr_init(&attr);
		pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_NORMAL);
		pthread_mutex_init(&m_lock, &attr);
		pthread_mutexattr_destroy(&attr);

	}
	~Mutex() 
	{ 
		while(pthread_mutex_destroy(&m_lock) == EBUSY) 
		{ 
			Lock(); 
			UnLock();
		}
	}
	int Lock() 
	{
		return pthread_mutex_lock(&m_lock);
	}
	int UnLock() 
	{
		return pthread_mutex_unlock(&m_lock);
	}
	bool TryLock()
	{
		return (pthread_mutex_trylock(&m_lock) == 0);
	}

};
template <typename T>
class LockGuard
{
private:
	T* m_lock;
public:
	explicit LockGuard(T& t) : m_lock(&t)
	{
		m_lock->Lock();
	}

	~LockGuard()
	{
		if(m_lock) m_lock->UnLock();
	}

	void Detach() { m_lock = NULL; }
	void UnLock() { m_lock->UnLock();}
};
typedef LockGuard<Mutex> MutexGuard;

class SpinLock
{
	pthread_spinlock_t m_lock;
	SpinLock(const SpinLock& rhs) { }
public:
	explicit SpinLock() 
	{ 
		pthread_spin_init(&m_lock, PTHREAD_PROCESS_PRIVATE); 
	}
	explicit SpinLock(const char * id) 
	{ 
		pthread_spin_init(&m_lock, PTHREAD_PROCESS_PRIVATE); 
	}
	~SpinLock() 
	{
	       	pthread_spin_destroy(&m_lock); 
	}
	void Lock() 
	{
	       	pthread_spin_lock(&m_lock); 
	}
	void UnLock() 
	{ 
		pthread_spin_unlock(&m_lock); 
	}
};
typedef LockGuard<SpinLock> SpinLockGuard;

#endif
