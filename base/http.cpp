#include "http.h"
#include <string>

static Http::Octets response_version("HTTP/1.0", 8);

Octets Http::ErrorResponse(int status_code)
{
	char buff[40];
	sprintf(buff, "<html><center>%d</center></html>", status_code);
	return Octets(buff, strlen(buff));
}

bool Http::Parse(const Octets &data, Http::Request &_req)
{
	Request req;
	NextFlag finder;
	size_t now = 0, pre = 0;
	if(!finder.Find(Octets(" "), now))
	{
		return false;
	}

	req.method = Octets(data.Data()+pre, now-pre);
	pre = now+1;

	if(!finder.Find(Octets(" "), now))
	{
		return false;
	}

	req.path = Octets(data.Data()+pre, now-pre);
	pre = now+1;

	if(!finder.Find("\r\n", now))
	{
		return false;
	}

	req.version = Octets(data.Data()+pre, now-pre);
	now = pre+2;

	while(1)
	{
		if(finder.Find(Octets(":"), now))
		{
			size_t colon = now;
			if(!finder.Find(Octets("\r\n"), now))
			{
				return false;
			}
			header[Octets(data.Data()+pre, colon-pre)] = Octets(data.Data()+colon+1, now-colon-1);
			pre = now+2;
		}
		else
		{
			if(finder.Find(Octets("\r\n"), now)
				&& now-pre == 2)
			{
				// The next part is body
				break;
			}
			else
			{
				return false;
			}
		}
	}

	now += 2;
	req.body = Octets(data.Data()+now+2, data.Size() - now);
	_req = req;
	return true;
}

void Http::Request::Print()
{
	printf("\r\n%*s %*s %*s"
		, method.Size(), (char *)method.Data()
		, path.Size(), (char *)path.Data()
		, version.Size(), (char *)version.Data());

	Header::const_iterator it = header.begin();
	Header::const_iterator ie = header.end();

	for(; it != ie; ++it)
	{
		printf("\r\n%*s:%*s"
			, it->first.Size(), (char *)it->first.Data()
			, it->second.Size(), (char *)it->second.Data());
	}

	printf("\r\n%*s\r\n", body.Size(), (char *)body.Data());
}

void * Http::Handler(void *args)
{
	if(!args)
	{
		return ;
	}
}
