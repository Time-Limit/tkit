#include "httpssession.h"

HttpsSession::HttpsSession(int fd)
: HttpSession(fd)
{}

void HttpsSession::OnDataIn()
{
	OctetsStream os(recv_data);

	HandShake<ClientHello> hello;

	os >> hello;

	hello.msg.Debug();

	return ;
}

void HttpsSessionManager::OnConnect(int fd)
{
	HttpsSession *session = new HttpsSession(fd);
	AddSession(session);
}
