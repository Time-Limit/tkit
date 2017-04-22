#include "octets.h"

Octets::Octets()
{		
	retain = Retain::Create(INIT_SIZE);
}

Octets::Octets(char const *_data)
{
	retain = Retain::Create(INIT_SIZE);
	Append(_data, strlen(_data));
}

Octets::Octets(void const *_data, size_t _size)
{
	retain = Retain::Create(INIT_SIZE);
	Append(_data, _size);
}

Octets::Octets(const Octets &o)
{
	retain = (Retain *)o.Copy();
}

Octets& Octets::operator= (const Octets &o)
{
	retain->release();
	retain = (Retain *)o.Copy();
}

Octets::~Octets()
{
	retain->release();
	retain = NULL;
}

void* Octets::Copy() const
{
	retain->retain();	
	return retain;
}

void Octets::Append(const void *data, size_t size)
{
	if(size == 0 || data == NULL)
	{
		return ;
	}

	if(retain->len + size > retain->cap || retain->Unique() == false)
	{
		Retain *tmp = NULL;
		{
			MutexGuard keeper(retain->mutex_lock);
			tmp = Retain::Create(GetSize(retain->len + size, retain->cap));	
			memcpy(tmp+1, retain+1, sizeof(char)*retain->len);
		}
		retain->release();
		retain = tmp;

		retain->mutex_lock.Lock();
	}

	memcpy(((char *)(retain+1)) + retain->len, (char *)data, size);

	retain->len += size;

	retain->mutex_lock.Unlock();
}

void const * Octets::Data() const
{
	return retain+1;
}

size_t Octets::Size() const
{
	return retain->len;
}

size_t Octets::Capacity() const
{
	return retain->cap;
}

size_t Octets::GetSize(size_t len, size_t cap)
{
	while(len > cap)
	{
		(cap <<= 1) += 1;
	}
	return cap;
}

bool NextFlag::Find(const Octets &flag, size_t &goal)
{
	for(size_t p = pos; p+flag.Size()-1 <= data.Size(); ++p)
	{
		if(!memcmp((char *)flag.Data(), ((char *)data.Data())+p, flag.Size()))
		{
			pos = p + flag.Size();
			goal = p;
			return true;
		}
	}
	return false;
}
