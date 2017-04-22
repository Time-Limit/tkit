#include "serialize.h"
#include <stdio.h>

OctetsStream::OctetsStream() : pos(0)
{
}

OctetsStream::OctetsStream(const Octets &o) : stream(o), pos(0)
{
}

template<class T>
OctetsStream& OctetsStream::operator<<(const T &data)
{
	push_byte(data);
	return *this;
}

template<class T>
OctetsStream& OctetsStream::operator>>(T &data)
{
	pop_byte(data);
	return *this;
}

void OctetsStream::push_byte(int data)
{
	stream.Append(&data, sizeof(int));
}

void OctetsStream::pop_byte(int &data)
{
	data = *(int *)(((char *)stream.Data()) + pos);
	pos += sizeof(int);
}

void OctetsStream::push_byte(unsigned int data)
{
	stream.Append(&data, sizeof(unsigned int));
}

void OctetsStream::pop_byte(unsigned int &data)
{
	data = *(unsigned int *)(((char *)stream.Data()) + pos);
	pos += sizeof(unsigned int);
}

void OctetsStream::push_byte(const Octets &data)
{
	stream.Append(data.Data(), data.Size());
}

void OctetsStream::pop_byte(Octets &data)
{
	size_t size = *(size_t *)(((char *)stream.Data()) + pos);
	if(pos + sizeof(size_t) + size <= stream.Capacity())
	{
		void * begin = (void *)(((char *)stream.Data()) + pos + sizeof(size_t)); 
		data = Octets(begin, size);
	}
}

OctetsStream::~OctetsStream()
{}
