#include "httpssession.h"
#include "thread.h"
#include "config.h"
#include "openssl/ssl.h"
#include "channel.h"

using namespace TCORE;

std::vector<Mutex> HttpsSessionManager::mutex_vec;

HttpsSession::HttpsSession(int fd)
: HttpSession(fd)
{}

void HttpsSessionManager::OnConnect(int fd)
{
	HttpsSession *session = new HttpsSession(fd);
	AddSession(session);
}

void HttpsSession::SetExchanger()
{
	exchanger = new SecureExchanger(ID(), this, SSL_new(dynamic_cast<HttpsSessionManager*>(GetManager())->GetSSLCTX()));
}

bool HttpsSessionManager::InitSSLData(const Config &config)
{
	if(is_finish_init)
	{
		return false;
	}

	std::string cert_path = config["cert_path"].Str();
	std::string pkey_path = config["pkey_path"].Str();

	SSL_load_error_strings ();
	SSL_library_init ();
	ssl_ctx = SSL_CTX_new (SSLv23_method ());

	if(1 != SSL_CTX_use_certificate_file(ssl_ctx, cert_path.c_str(), X509_FILETYPE_PEM)
		|| 1 != SSL_CTX_use_PrivateKey_file(ssl_ctx, pkey_path.c_str(), X509_FILETYPE_PEM)
		|| 1 != SSL_CTX_check_private_key(ssl_ctx))
	{
		char info[1024] = {0};
		unsigned long error = 0;
		error = ERR_get_error();
		ERR_error_string(error,info);
		LOG_TRACE("HttpsSessionManager::InitSSLData, error=%ld, info=%s", error, info);
		delete ssl_ctx;
		ssl_ctx = nullptr;
		return false;
	}

	SetupThreadData();

	is_finish_init = true;

	return true;
}

unsigned long HttpsSessionManager::GetThreadID()
{
	return pthread_self();
}

void HttpsSessionManager::SetupThreadData()
{
	size_t n = CRYPTO_num_locks();
	mutex_vec.resize(n);
	CRYPTO_set_id_callback(GetThreadID);
	CRYPTO_set_locking_callback(OperateMutexVector);

	LOG_TRACE("HttpsSessionManager::SetupThreadData, number of lock is %ld\n", n);
}

void HttpsSessionManager::OperateMutexVector(int mode, int n, const char *file, int line)
{
	if(mode & CRYPTO_LOCK)
	{
		mutex_vec[n].Lock();
	}
	else
	{
		mutex_vec[n].UnLock();
	}
}
