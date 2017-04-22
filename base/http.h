#ifndef __HTTP_H_
#define __HTTP_H_

#include "octets.h"
#include "channel.h"

class Http
{
public:
	Http() {};
	~Http() {};

	virtual int DoRequest();
	virtual int DoResponse();

	typedef std::map<Octets, Octest> Header;

	struct  Request
	{
		Octets method;
		Octets path;
		Octets version;
		Header header;
		Octets body;

		void Print();
	};

	struct Response
	{
		Octets version;
		int code;
		Octets status;
		Header header;
		Octets body;
	};
public:
	static void *Handler(void *args);
	static Octets ErrorResponse(int status_code);

private:
	bool Parse(const Octets &data);
};

#endif
