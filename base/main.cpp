#include "serialize.h"
#include <bits/stdc++.h>
#include "file.h"
#include "thread.h"
#include "socket.h"

using namespace std;

int main()
{
	string str("123456789");
	Octets data(str.c_str(), str.size());

	NextFlag finder(data);

	for(int i = 1; i <= 9; ++i)
	{
		size_t goal;
		char buf[10];
		sprintf(buf, "%d", i);

		if(finder.Find(Octets((void *)buf, 1), goal))
		{
			cout<<"goal = "<<goal<<endl;
		}
		else
		{
			cout<<"error = i = "<<i<<endl;
		}
	}

	File f("./log", O_RDWR|O_CREAT, 0664);
	char buf[1024] = "123456\n!@#$%^\nabcdef\n";
	f.Write(Octets((void *)buf, strlen(buf)));
	Octets content;
	f.Reset();
	int n = f.Read(content);
	printf("n = %d size = %d, c = %*s\n", n, content.Size(), content.Size(), (char *)content.Data());
	printf("%*s\n", 5, "123456789");
	cout<<File::IsExist("./abcdasdfasdf")<<endl;
	ThreadPool pool;

	Socket server(8080);
	int accept_fd;
	while(accept_fd = server.Accept())
	{
		int *fd = new int(accept_fd);	
		pool.AddTask(HttpHandler, fd);
	}
	
	return 0;
}
