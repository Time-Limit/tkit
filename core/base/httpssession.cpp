#include "httpssession.h"

HttpsSession::HttpsSession(int fd)
: HttpSession(fd)
{}

void HttpsSession::Parse()
{
	OctetsStream os(recv_data);
	HandShake<ClientHello> hello;
	os >> hello;

	hello.msg.Debug();

	return ;
	for(size_t i = 0; i < recv_data.size(); ++i)
	{
		unsigned char t = *(((const char *)recv_data.begin()) + i);
		printf("0x%x ", t);
	}

	printf("\n");

	printf("%*.s\n", recv_data.size(), (const char *)recv_data.begin());

	const unsigned char *tmp = (const unsigned char *)recv_data.begin();

	if(tmp + 1 >= recv_data.end()) return ;
	printf("HandShake : %d\n", *tmp); tmp++;

	if(tmp + 1 >= recv_data.end()) return ;
	printf("Major : %d\n", *tmp); tmp++;

	if(tmp + 1 >= recv_data.end()) return;
	printf("Second : %d\n", *tmp); tmp++;

	if(tmp + 2 >= recv_data.end()) return ;
	printf("Length : %d\n", ((*tmp)*64) + *(tmp + 1)); tmp += 2;
	
	if(tmp + 1 >= recv_data.end()) return;
	printf("Type : %d\n", *tmp); tmp++;
	
	if(tmp + 3 >= recv_data.end()) return ;
	printf("Second-Length : %d\n", ((*tmp)*64*64) + (*(tmp + 1))*64 + *(tmp + 2)); tmp += 3;

	if(tmp + 1 >= recv_data.end()) return ;
	printf("Major : %d\n", *tmp); tmp++;

	if(tmp + 1 >= recv_data.end()) return;
	printf("Second : %d\n", *tmp); tmp++;

	if(tmp + 32 >= recv_data.end()) return ;

	printf("Random : ");
	for(int i = 0; i < 32; ++i, ++tmp)
	{
		printf("0x%x ", *tmp);
	}
	puts("");

}

void HttpsSessionManager::OnConnect(int fd)
{
	HttpsSession *session = new HttpsSession(fd);
	AddSession(session);
}
