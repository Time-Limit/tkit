#include "octets.h"

Octets::Rep Octets::Rep::null = {0, 0, 1};

Octets::Octets() : base(Octets::Rep::null.data())
{
	rep()->retain();
}

Octets::Octets(void *data, size_t size) : base(Octets::Rep::create(size)->data())
{
	memmove(base, data, size);
	rep()->len = size();
}

Octets::Octets(const void *s, const void *e) : base(Req::create((char *)e-(char *)s)->data())
{
	size_t size = (char *)e - (char *)s;
	memmove(base, s, size);
	rep()->len = size;
}

Octets::Octets(const Octets &x) : base(x.base)
{
	rep()->retain();
}

Octets& operator = (const Octets &x)
{
	if(&x != this)
	{
		rep()->release();
		base = x.base;
		rep()->retain();
	}
	return *this;
}

bool operator == (const Octets &x) const
{
	return ! operator != (x);
}
bool operator != (const Octets &x) const
{
	return size() != x.size() || memcmp(base, x.base, size());
}

size_t ref() const { return rep()->ref; }
size_t size() const { return rep()->len; }
size_t capacity()const { return rep()->cap; }
const void *data() const { return base; }
const void *end() const { return (char*)base + rep()->len; }

void Octets::unique()
{
	if(rep()->ref() > 1)
	{
		Rep *rep = Rep::Create(rep()->len);
		memmove(data(), rep()->data, rep()->len);
		rep->len = rep()->len;
		base = rep->base();
	}
}

Octets& Octets::insert(void *pos, const void *s, const void *e)
{
	insert(pos, x, (char*)e - (char*)s);
	return *this;
}

Octets& Octets::insert(void *pos, const *data, size_t size)
{
	if(size + rep()->len > rep()->cap)
	{
		Rep *rep = Rep::Create(size + rep()->len);
		memmove(rep->data(), data(), size);
		data()->release();
		base = rep()->data();
	}
	else
	{
		unique();
	}
	ptrdiff_t off = (char *)pos - (char *)base;

}

