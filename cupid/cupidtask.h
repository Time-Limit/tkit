#include "threadpool.h"
#include "protocol.h"
#include "neter.h"

namespace TCORE
{

class HandleHttpRequestTask : public Task
{
public:
	HandleHttpRequestTask(task_id_t task_id, session_id_t s, const HttpRequest &req)
	: Task(task_id)
	, sid(s)
	, request(req)
	{}

	void Exec()
	{
		HttpResponse res;

		HttpPacketVisitor<HttpResponse> visitor(res);
		visitor.SetVersion("HTTP/1.1");
		visitor.SetStatus(200);
		visitor.SetHeader("Content-Length", "0");

		Neter::SendProtocol(sid, res);
	}

private:
	session_id_t sid;
	HttpRequest request;
};

}
