#ifndef _OCTETS_H_
#define _OCTETS_H_

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>

namespace TCORE
{

class Protocol;

inline void *fmemmove(void *dest, const void *src, size_t n)
{
	char *d = (char *)dest;
	char *s = (char *)src;
	if ( (s < d && s + n >= d ) || (s >d && d + n >=s)) // overlap
		return memmove(dest, src, n);
	else
		return memcpy(dest, src, n);
}

class Octets
{
	struct Rep
	{
		size_t cap;
		size_t len;
		size_t ref;

		void retain()
		{
			__asm__ __volatile__ (
				"lock; add $1, %0	\n"
				: "=m"(ref)
			);
		}
		void release()
		{
			size_t old;
			__asm__ __volatile__ (
				"lock; xadd  %2, %0	\n"
				: "=m"(ref), "=r"(old)
				: "1"(-1) : "memory"
			);

			if ( old == 1 ) delete this;
		}

		void* data() { return reinterpret_cast<void *>(this + 1); }

		void* clone()
		{
			Rep *rep = create(cap);
			memcpy(rep->data(), data(), rep->len = len);
			return rep->data();
		}

		void* unique()
		{
			if ( ref > 1 )
			{
				void* r = clone();
				release();
				return r;
			}
			return data();
		}

		void* reserve(size_t size)
		{
			size = frob_size(size);
			if ( size > cap )
			{
				Rep* rep = create(size);
				memcpy(rep->data(), data(), rep->len = len);
				release();
				return rep->data();
			}
			return unique();
		}

		static size_t frob_size(size_t size)
		{
			size_t tmp = 16;
			while ( size > tmp ) tmp <<= 1;
			return tmp;
		}

		static Rep* create(size_t cap)
		{
			Rep *rep = new (cap) Rep;
			rep->cap = cap;
			rep->len = 0;
			rep->ref = 1;
			return rep;
		}

		static void* operator new (size_t size, size_t extra) { return malloc(size + extra); }
		static void  operator delete (void *p) { free(p); }
		static Rep null;
	};
	void *base;
	Rep *rep () const { return reinterpret_cast<Rep *>(base) - 1; }
	void unique() { base = rep()->unique(); }
public:
	Octets& reserve(size_t size)
	{
		base = rep()->reserve(size);
		return *this;
	}
	Octets& replace(const void *data, size_t size)
	{
		reserve(size);
		memcpy(base, data, size);
		rep()->len = size;
		return *this;
	}
	virtual ~Octets() { rep()->release(); }
	Octets () : base(Rep::null.data()) { rep()->retain(); }
	explicit Octets (size_t size) : base(Rep::create(size)->data()) { }
	Octets (const void *x, size_t size) : base(Rep::create(size)->data())
	{
		memcpy(base, x, size);
		rep()->len = size;
	}
	Octets (const void *x, const void *y) : base(Rep::create((char*)y-(char*)x)->data())
	{
		size_t size = (char*)y-(char*)x;
		memcpy(base, x, size);
		rep()->len = size;
	} 
	Octets (const Octets &x) : base(x.base) { rep()->retain(); }
	Octets& operator = (const Octets&x)
	{
		if ( &x != this )
		{
			rep()->release();
			base = x.base;
			rep()->retain();
		}
		return *this;
	}
	bool operator == (const Octets &x) const { return size() == x.size() && !memcmp( base, x.base, size() ); }
	bool operator != (const Octets &x) const { return ! operator == (x); }
	Octets& swap(Octets &x) { void *tmp = base; base = x.base; x.base = tmp; return *this; }
	void *begin() { unique(); return base; }
	void *end()   { unique(); return (char*)base + rep()->len; }
	const void *begin() const { return base; }
	const void *end()   const { return (char*)base + rep()->len; }
	size_t size()     const { return rep()->len; }
	size_t capacity() const { return rep()->cap; }
	Octets& clear() { unique(); rep()->len = 0; return *this;  }
	Octets& erase(size_t pos, size_t len) { char * x = (char*)begin(); return erase(x + pos, x + pos + len); }
	Octets& erase(void *x, void *y)
	{
		if (x != y)
		{
			void *tmp = base;
			base = rep()->unique();
			ptrdiff_t o = (char *)base - (char *)tmp;
			if (o)
			{
				x = (char *)x + o;
				y = (char *)y + o;
			}
			fmemmove((char*)x, (char*)y, ((char*)base + rep()->len) - (char*)y);
			rep()->len -= (char *)y - (char *)x;
		}
		return *this;
	}
	Octets& insert(void *pos, const void *x, size_t len)
	{
		ptrdiff_t off = (char *)pos - (char *)base;
		reserve(size() + len);
		pos = (char *)base + off;
		size_t adjust = size() - off;
		if ( adjust ) fmemmove((char*)pos + len, pos, adjust);
		fmemmove(pos, x, len);
		rep()->len += len;
		return *this;
	}
	Octets& insert(void *pos, const void *x, const void *y) { insert(pos, x, (char*)y - (char*)x); return *this; }
	Octets& resize(size_t size) { reserve(size); rep()->len = size; return *this; }
	void dump()
	{
		const unsigned char *p = (const unsigned char *)begin();
		for(size_t i = 0; i < size(); i++)
			fprintf(stderr, "%02x ", *p++);
		fprintf(stderr, "\n");
	}
};

class OctetsStream
{
	Octets data;
	unsigned int start_cursor;
	unsigned int cursor;

	template<typename T>
	bool CheckLength()
	{
		if(cursor < data.size() && data.size()-cursor >= sizeof(T))
		{
			return true;
		}
		return false;
	}

	template<typename T>
	OctetsStream& pop_bytes(T &d)
	{
		if(CheckLength<T>() == false)
		{
			throw OSException(OSException::OS_EXCEPTION_LENGTH);
		}

		const unsigned char *tmp = (const unsigned char *)data.begin() + cursor;

#ifdef _BIG_ENDIAN_
		d = *(const T *)tmp;
#else
		union U
		{
			T t;
			unsigned char c[sizeof(T)];
		} u;
		for(unsigned char i = 0; i < sizeof(T); ++i)
		{
			u.c[sizeof(T) - i - 1] = *(tmp+i);
		}
		d = u.t;
#endif
		cursor += sizeof(T);
		return *this;
	}

	OctetsStream& pop_bytes(Octets &d)
	{
		size_t size = 0;
		operator>>(size);
		if(cursor < data.size() && data.size() - cursor >= size)
		{
			d = Octets((char *)data.begin()+cursor, size);
			cursor += size;
		}
		else
		{
			throw OSException(OSException::OS_EXCEPTION_LENGTH);
		}
		return *this;
	}

	template<typename T>
	OctetsStream& push_bytes(const T &d)
	{
#ifdef _BIG_ENDIAN_
		data.insert(data.end(), &d, sizeof(T));
#else
		union U
		{
			T t;
			unsigned char c[sizeof(T)];
		} u;
		const unsigned char *tmp = (const unsigned char *)(&d);
		for(unsigned char i = 0; i < sizeof(T); ++i)
		{
			u.c[sizeof(T) - i - 1] = *(tmp+i);
		}
		data.insert(data.end(), &u.t, sizeof(T));
#endif
		return *this;
	}

	OctetsStream& push_bytes(const Octets &d)
	{
		size_t size = d.size();
		operator<<(size);
		data.insert(data.end(), d.begin(), d.size());
		return *this;
	}
	
public:
	class Start { };
	class Commit {};
	class Revert {};

	static const Start  START;
	static const Commit COMMIT;
	static const Revert REVERT;

public:
	struct OSException
	{
		enum
		{
			OS_EXCEPTION_LENGTH = 0,
		};

		unsigned char reason;
		OSException(unsigned char r) : reason(r) {}
	};
	explicit OctetsStream() : start_cursor(0), cursor(0) {}
	explicit OctetsStream(const Octets &d) : data(d), start_cursor(0), cursor(0) {}

	const Octets& GetData() const { return data; }

	void push_back(const void *x, const void *y) { data.insert(data.end(), x, y); }
	void push_back(const void *x, size_t s) { data.insert(data.end(), x, ((const char *)x)+s); }
	OctetsStream& operator<<(const Protocol &d);
	OctetsStream& operator<<(const unsigned char &d) { return push_bytes(d); }
	OctetsStream& operator<<(const char &d) { return push_bytes(d); }
	OctetsStream& operator<<(const unsigned short &d) { return push_bytes(d); }
	OctetsStream& operator<<(const short &d) { return push_bytes(d); }
	OctetsStream& operator<<(const unsigned int &d) { return push_bytes(d); }
	OctetsStream& operator<<(const int &d) { return push_bytes(d); }
	OctetsStream& operator<<(const unsigned long &d) { return push_bytes(d); }
	OctetsStream& operator<<(const int64_t &d) { return push_bytes(d); }
	OctetsStream& operator<<(const float &d) { return push_bytes(d); }
	OctetsStream& operator<<(const double &d) { return push_bytes(d); }
	OctetsStream& operator<<(const Octets &d) { return push_bytes(d); }

	Octets pop_front(size_t size)
	{
		if(cursor < data.size() && data.size() - cursor >= size)
		{
			Octets d = Octets((char *)data.begin()+cursor, size);
			cursor += size;
			return d;
		}
		throw OSException(OSException::OS_EXCEPTION_LENGTH);
		return Octets();
	}
	OctetsStream& operator>>(Protocol &d);
	OctetsStream& operator>>(unsigned char &d) { return pop_bytes(d); }
	OctetsStream& operator>>(char &d) { return pop_bytes(d); }
	OctetsStream& operator>>(unsigned short &d) { return pop_bytes(d); }
	OctetsStream& operator>>(short &d) { return pop_bytes(d); }
	OctetsStream& operator>>(unsigned int &d) { return pop_bytes(d); }
	OctetsStream& operator>>(int &d) { return pop_bytes(d); }
	OctetsStream& operator>>(unsigned long &d) { return pop_bytes(d); }
	OctetsStream& operator>>(int64_t &d) { return pop_bytes(d); }
	OctetsStream& operator>>(float &d) { return pop_bytes(d); }
	OctetsStream& operator>>(double &d) { return pop_bytes(d); }
	OctetsStream& operator>>(Octets &d) { return pop_bytes(d);}

	OctetsStream& operator>>(const Start &start)
	{
		start_cursor = cursor;
		return *this;
	}
	
	OctetsStream& operator>>(const Commit &commit)
	{
		char * b = (char *)data.begin();
		data.erase(b+start_cursor, b+cursor);
		cursor = start_cursor;
		start_cursor = 0;
		return *this;
	}

	OctetsStream& operator>>(const Revert &revert)
	{
		cursor = start_cursor;
		start_cursor = 0;
		return *this;
	}
};

}

#endif

