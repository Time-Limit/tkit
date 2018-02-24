#include "httpssession.h"
#include "thread.h"

HttpsSession::HttpsSession(int fd)
: HttpSession(fd)
, connect_state(CS_WAIT_CLIENT_HELLO)
{}

OctetsStream& HttpsSession::ServerHello::Serialize(OctetsStream& os) const
{
	return os;
}

OctetsStream& HttpsSession::ServerHello::Deserialize(OctetsStream &os)
{
	return os;
}

OctetsStream& HttpsSession::ClientHello::Serialize(OctetsStream& os) const
{
	OctetsStream tmp;
	tmp << length;
	tmp << version.major << version.minor;
	tmp << random.gmt_unix_time;
	for(size_t i = 0; i < sizeof(random.random_bytes)/sizeof(random.random_bytes[0]); ++i)
	{
		tmp << random.random_bytes[i];
	}

	tmp << session_id;
	tmp << cipher_suites;
	tmp << compression_methods;
	tmp << extension;

	const Octets& data = os.GetData();

	os.Append(data.begin(), ((const unsigned char *)data.begin())+data.size());


	//InitBasePart();
	HttpsSession::HandShake::Serialize(os);

	return os;
}

OctetsStream& HttpsSession::ClientHello::Deserialize(OctetsStream& os)
{
	HttpsSession::HandShake::Deserialize(os);
	os >> length;
	os >> version.major >> version.minor;
	os >> random.gmt_unix_time;
	for(size_t i = 0; i < sizeof(random.random_bytes)/sizeof(random.random_bytes[0]); ++i)
	{
		os >> random.random_bytes[i];
	}

	os >> session_id;
	os >> cipher_suites;
	os >> compression_methods;
	os >> extension;

	return os;
}

void HttpsSession::OnDataIn()
{
	OctetsStream os(recv_data);
	TLS *tls = nullptr;

	try
	{
		switch(connect_state)
		{
			case CS_WAIT_CLIENT_HELLO:
			{
				tls = new ClientHello(*this);
			}
			break;
			default:
			{
				throw;
			}
			break;
		}

		os >> *tls >> OctetsStream::COMMIT;
		ThreadPool::GetInstance().AddTask(new HandleNetProtocolTask(GetManager(), ID(), tls));
	}
	catch(OctetsStream::OSException e)
	{
		os >> OctetsStream::REVERT;
		delete tls;
	}
	catch(...)
	{
		delete tls;
	}

	return ;
}

void HttpsSessionManager::OnConnect(int fd)
{
	HttpsSession *session = new HttpsSession(fd);
	AddSession(session);
}
