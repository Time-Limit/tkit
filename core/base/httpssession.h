#ifndef _HTTPSSESSION_H_
#define _HTTPSSESSION_H_

#include "session.h"
#include <vector>
#include <iostream>

typedef unsigned char opaque;
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
struct uint24
{
	friend OctetsStream& operator>>(OctetsStream&, uint24&);

private:
	uint8 a, b, c;
public:
	unsigned int get() const { return a*256*256 + b*256 + c; }
	operator unsigned int() { return get(); }
};

inline OctetsStream& operator>>(OctetsStream& os, uint24 &data)
{
	return os >> data.a >> data.b >> data.c;
}

template<typename LENGTH, typename CONTENT>
struct DynamicArray
{
	LENGTH length;
	std::vector<CONTENT> content;

	void Debug() const
	{
		std::cout << (size_t)length << std::endl;
		for(const auto &p : content)
		{
			std::cout << " " << p << std::endl;
		}
	}
};

template<typename LENGTH, typename CONTENT>
inline OctetsStream& operator>>(OctetsStream &os, DynamicArray<LENGTH, CONTENT> &d)
{
	os >> d.length;
	d.length /= sizeof(CONTENT);
	d.content.resize(d.length);
	for(LENGTH i = 0;i < d.length; ++i)
	{
		os >> d.content[i];
	}
	return os;
}

struct ClientHello
{
#pragma pack(1)
	struct ProtocolVersion
	{
		uint8 major;
		uint8 minor;
	};

	struct Random
	{
		uint32 gmt_unix_time;
		opaque random_bytes[28];
	};

#pragma pack()

	typedef DynamicArray<uint8, opaque> SessionID;
	typedef DynamicArray<uint16, uint16> CipherSuites;
	typedef DynamicArray<uint8, uint8> CompressionMethods;

	uint24 length;
	ProtocolVersion version;
	Random random;
	SessionID session_id;
	CipherSuites cipher_suites;
	CompressionMethods compression_methods;

	void Debug() const
	{
		using std::cout;
		using std::endl;
		cout << "major : " << version.major << endl;
		cout << "minor : " << version.minor << endl;
		cout << "length :" << length.get() << endl;
		cout << "time  : " << random.gmt_unix_time << endl;
		cout << "random_bytes : ";
		for(size_t i = 0; i < sizeof(random.random_bytes)/sizeof(random.random_bytes[0]); ++i)
		{
			printf("0x%x ", random.random_bytes[i]);
		}
		cout << endl;

		session_id.Debug();
		cipher_suites.Debug();
		compression_methods.Debug();
	}
};

inline OctetsStream& operator>> (OctetsStream &os, ClientHello &c)
{
	os >> c.length;
	os >> c.version.major >> c.version.minor;
	os >> c.random.gmt_unix_time;
	for(size_t i = 0; i < sizeof(c.random.random_bytes)/sizeof(c.random.random_bytes[0]); ++i)
	{
		os >> c.random.random_bytes[i];
	}
	os >> c.session_id;
	os >> c.cipher_suites;
	os >> c.compression_methods;

	return os;
}

template<typename MSG>
struct HandShake
{
#pragma pack(1)
	uint8 msg_type;
	uint8 major;
	uint8 minor;
	uint16 length;
	uint8 type;
#pragma pack()
	MSG msg;
};

template<typename MSG>
inline OctetsStream& operator>>(OctetsStream &os, HandShake<MSG> &data)
{
	os >> data.msg_type;
	os >> data.major;
	os >> data.minor;
	os >> data.length;
	os >> data.type;
	os >> data.msg;
	return os;
}

class HttpsSession : public HttpSession
{
public:
	explicit HttpsSession(int fd);

private:
	virtual void Parse() override;
};

class HttpsSessionManager : public SessionManager
{
public:
	virtual void OnConnect(int fd) override;
};

#endif
