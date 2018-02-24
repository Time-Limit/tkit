#ifndef _HTTPSSESSION_H_
#define _HTTPSSESSION_H_

#include "session.h"
#include <vector>
#include <iostream>
#include "protocol.h"

typedef unsigned char opaque;
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
struct uint24
{
	friend OctetsStream& operator>>(OctetsStream&, uint24&);
	friend OctetsStream& operator<<(OctetsStream&, const uint24&);

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

inline OctetsStream& operator<<(OctetsStream& os, const uint24 &data)
{
	return os << data.a << data.b << data.c;
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

template<typename LENGTH, typename CONTENT>
inline OctetsStream& operator<<(OctetsStream &os, const DynamicArray<LENGTH, CONTENT> &d)
{
	os << d.length;
	for(const auto &k : d.content)
	{
		os << k;
	}

	return os;
}

class HttpsSession : public HttpSession
{
	friend class ClientHello;
	typedef unsigned char connect_state_t;

public:
	explicit HttpsSession(int fd);

	void SetConnectState(connect_state_t state)
	{
		connect_state = state;
	}

private:

	virtual void OnDataIn() override;

	enum CONNECT_STATE
	{
		CS_WAIT_CLIENT_HELLO = 0,
		CS_WAIT_FINISH = 1,
		CS_SUCCESS = 2,
	};

	connect_state_t connect_state;

	class TLS : public Protocol
	{
	protected:
		HttpsSession &session;
	public:
		TLS(HttpsSession &s) : session(s) {}
	};

	class HandShake : public TLS
	{
	public:
		virtual OctetsStream& Serialize(OctetsStream& os) const
		{
			os << msg_type << version_major << version_minor << length << type;
			return os;
		}

		virtual OctetsStream& Deserialize(OctetsStream& os)
		{
			os >> msg_type >> version_major >> version_minor >> length >> type;
			return os;
		}

		virtual ~HandShake() {}

		HandShake(HttpsSession &_session)
		: TLS(_session) , msg_type(0) ,version_major(0) , version_minor(0) , length(0) , type(0)
		{}

	private:
		uint8 msg_type;
		uint8 version_major;
		uint8 version_minor;
		uint16 length;
		uint8 type;
	};

	class ClientHello : public HandShake
	{
	public:
		ClientHello(HttpsSession &_session) : HandShake(_session) {}
		virtual ~ClientHello() {}
		virtual void Handle(SessionManager *manager, session_id_t sid)
		{
			session.SetConnectState(CS_WAIT_FINISH);
			//Send Server Hello
			//sesssion.Send();
		}

		virtual OctetsStream& Serialize(OctetsStream& os) const override;
		virtual OctetsStream& Deserialize(OctetsStream& os) override;
	private:

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

		typedef DynamicArray<uint8, opaque> SessionID;
		typedef DynamicArray<uint16, uint16> CipherSuites;
		typedef DynamicArray<uint8, uint8> CompressionMethods;
		typedef DynamicArray<uint16, uint8> Extension;

		uint24 length;
		ProtocolVersion version;
		Random random;
		SessionID session_id;
		CipherSuites cipher_suites;
		CompressionMethods compression_methods;
		Extension extension;

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
};

class HttpsSessionManager : public SessionManager
{
public:
	virtual void OnConnect(int fd) override;
};

#endif
