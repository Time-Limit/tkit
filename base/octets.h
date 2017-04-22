#ifndef __OCTETS_H_
#define __OCTETS_H_

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "lock.h"
#include "exptype.h"

class Octets
{

public:
	Octets();
	Octets(void const *_data, size_t _size);
	Octets(char const *_data);
	Octets(const Octets &o);
	Octets &operator= (const Octets &o);
	~Octets();
public:

public:
	void const * Data() const;
	size_t	Size() const;
	size_t	Capacity() const;

	void Append(const void *data, size_t size);
	void Clean();
private:
	enum
	{
		INIT_SIZE = 64,
	};

	void* Copy() const;
	
	static size_t GetSize(size_t len, size_t cap);

	struct Retain
	{
		Mutex mutex_lock;
		size_t len;
		size_t cap; 
		size_t ref;

		void retain()
		{
			MutexGuard keeper(mutex_lock);
			ref++;
		}

		void release()
		{
			mutex_lock.Lock();
			if(--ref == 0)
			{
				mutex_lock.Unlock();
				delete this;
			}
			else
			{
				mutex_lock.Unlock();
			}
		}

		bool Unique()
		{
			mutex_lock.Lock();
			if(ref == 1)
			{
				return true;
			}
			mutex_lock.Unlock();
			return false;
		}

		static Retain* Create(size_t cap)
		{
			Retain *ori = (Retain *)new(cap) Retain;
			ori->len = 0;
			ori->cap = cap;
			ori->ref = 1;
			return ori;
		}

		static void * operator new (size_t self, size_t cap)
		{
			return malloc(sizeof(char)*(self+cap));
		}

		static void operator delete(void *p)
		{
			free(p);
		}
	};

	mutable Retain *retain;

public:
	bool operator < (const Octets &rhs)
	{
		size_t l_size = this->Size();		
		size_t r_size = rhs.Size();

		int result = memcmp(this->Data(), rhs.Data(), _min(l_size, r_size));

		if(result)
		{
			return result < 0;
		}

		return l_size < r_size;
	}

	bool operator == (const Octets &rhs)
	{
		size_t size = this->Size();
		if(size != rhs.Size())
		{
			return false;
		}

		return memcmp(this->Data(), rhs.Data(), size) == 0;
	}
};

class NextFlag
{
public:
	NextFlag(const Octets &_data)
	: data(_data)
	, pos(0)
	{}
	bool Find(const Octets &flag, size_t &goal);
private:
	const Octets data;
	size_t pos;
};


#endif
