#ifndef _OCTETS_H_
#define _OCTETS_H_

#include <stdlib.h>

class Octets
{
public:
	enum OCTETS_CONFIG
	{
		OCTETS_INIT_CAP = 32;
	};
public:
	Octets();
	Octets(const Octets &rhs);

	const Octets& operator= (const Octets &rhs);

	const char * data();
	size_t size();

	void Append(const char * d, size_t s);

	~Octets();

	struct Rep
	{
		size_t cap;
		size_t len;
		size_t ref;

		static void* operator new (size_t size, size_t extra)
		{
			return malloc(size + extra);
		}

		static void operator delete(void *p)
		{
			free(p);
		}

		static Req* create(size_t cap)
		{
			Rep *rep = new (cap) Req;
			req->cap = cap;
			req->len = 0;
			req->ref = 1;
		}

		void * data()
		{
			return reinterpret_cast<void *>(this+1);
		}
	};
private:
};

#endif
