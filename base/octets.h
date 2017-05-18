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
	
	Octets();
	Octets(const Octets &rhs);

	const Octets& operator= (const Octets &rhs);

	const char * data();
	size_t size();

	~Octets();
private:
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

		void retain()
		{
			__asm__ __volatile__ (
				"lock; add $1, %0 \n"	
			);
		}

		void release()
		{
			size_t old;
			__asm__ __volatile__ (
				"lock; xadd %2, %0 \n"
				: "=m"(ref), "=r"(old)
				: "1"(-1) : "memory"
			);

			if( old == 1 ) delete this;
		}

		void * clone()
		{
			Rep *rep = create(cap);
			memecpy(req->data(), data(), req->len = len);
			return req->data();
		}

		void * unique()
		{
			if(ref > 1)
			{
				void * r = clone();
				release();
				return r;
			}
			return data();
		}
	};

	void *base;
	Rep *rep() const { return reinterpret_cast<Rep *>(base) - 1; }
};

#endif
