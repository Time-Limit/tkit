#ifndef _HTTPSSESSION_H_
#define _HTTPSSESSION_H_

#include "session.h"
#include <vector>
#include <iostream>
#include "protocol.h"
#include "openssl/ssl.h"
#include "openssl/err.h"

namespace TCORE
{

class HttpsSession : public HttpSession
{
public:
	explicit HttpsSession(int fd);

	void SetExchanger() override;
};

class Config;

class HttpsSessionManager : public SessionManager
{
public:
	virtual void OnConnect(int fd) override;

	HttpsSessionManager()
	: SessionManager()
	, is_finish_init(false)
	, ssl_ctx(nullptr)
	{}

	HttpsSessionManager(ProtocolHandler ph)
	: HttpsSessionManager()
	{
		SetProtocolHandler(ph);
	}

	bool InitSSLData(const Config &config);

	SSL_CTX* GetSSLCTX() { return ssl_ctx; }

private:
	bool is_finish_init;
	SSL_CTX *ssl_ctx;
	
	std::string cert;
	std::string pkey;

	static std::vector<Mutex> mutex_vec;

private:
	static unsigned long GetThreadID();
	static void SetupThreadData();
	static void OperateMutexVector(int mode, int n, const char * file, int line);
};

}

#endif
