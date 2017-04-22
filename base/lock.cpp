#include "lock.h"

Mutex::Mutex()
{
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_NORMAL);
	pthread_mutex_init(&m_lock, &attr);
	pthread_mutexattr_destroy(&attr);
}

Mutex::~Mutex()
{
	while(pthread_mutex_destroy(&m_lock) == EBUSY)
	{
		Lock();
		Unlock();
	}
}

int Mutex::Lock()
{
	return pthread_mutex_lock(&m_lock);
}

int Mutex::Unlock()
{
	return pthread_mutex_unlock(&m_lock);
}

bool Mutex::TryLock()
{
	return (pthread_mutex_trylock(&m_lock) == 0);
}
