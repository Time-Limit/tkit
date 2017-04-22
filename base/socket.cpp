#include "socket.h"
#include <errno.h>

Socket::Socket(int port)
{
	listen_fd = socket(AF_INET, SOCK_STREAM, 0);

	printf("fd = %d\n", listen_fd);

	memset(&addr, 0, sizeof(addr));

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(port);

	int optval = 1;

	setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
	
	int bind_ret = bind(listen_fd, (sockaddr *)&addr, sizeof(addr));

	printf("bind = %d %s\n", bind_ret, strerror(errno));

	int listen_ret = listen(listen_fd, 10);

	printf("listen = %d %s\n", listen_ret, strerror(errno));
}

int Socket::Accept()
{
	return accept(listen_fd, NULL, NULL);
}
