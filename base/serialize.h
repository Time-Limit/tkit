#ifndef __SERIALIZE_H_
#define __SERIALIZE_H_

#include "octets.h"

class OctetsStream
{
public:
	OctetsStream();
	OctetsStream(const Octets &o);
	//OctetsStream(const OctetsStream &os) = delete;
	//OctetsStream& operator=(const OctetsStream &os) = delete;
	~OctetsStream();

public:
	template<class T>
	OctetsStream& operator<<(const T &data);

	template<class T>
	OctetsStream& operator>>(T &data);

private:
	
	void push_byte(int data);
	
	void pop_byte(int &data);

	void push_byte(unsigned int data);

	void pop_byte(unsigned int &data);

	void push_byte(const Octets &data);

	void pop_byte(Octets &data);

private:
	Octets stream;
	size_t pos;
};

#endif
