#ifndef _ALLOCER_H_
#define _ALLOCER_H_

#include <stdlib.h>
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include "lock.h"

template<size_t SIZE>
class Allocer
{
public:
	Allocer()
	{
		header = NULL;
	}

	~Allocer()
	{
		void *walker = header;

		while(walker)
		{
			header = walker;
			walker = (void *)*(ptrdiff_t*)walker;
			free(header);
		}

		header = NULL;
	}

	void * Alloc()
	{
		SpinLockGuard guarder(lock);
		if(header)
		{
			void * res = header;
			header = (void *)*(ptrdiff_t *)header;
			return res;
		}
		return malloc(SIZE);
	}
	void Free(void * ptr)
	{
		SpinLockGuard guarder(lock);
		if(ptr)
		{
			*(ptrdiff_t *)ptr = (ptrdiff_t)header;
			header = ptr;
		}
	}

	size_t Size() { return SIZE; }
private:
	void *header;
	SpinLock lock;
};

#endif
