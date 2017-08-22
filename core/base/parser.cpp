#include "parser.h"
#include "task.h"
#include "channel.h"
#include "thread.h"

class ResponseTask : public Task
{
	channel_id_t cid;
public:
	ResponseTask(channel_id_t c)
	: cid(c)
	{}
	void Exec()
	{
		static const char * content = "HTTP/1.1 200 SUCCESS\r\nContent-Length:3\r\n\r\nzmx";
		ChannelManager::GetInstance().PutData(cid, content, strlen(content));
	}
};

void Parser::Append(const Octets &fresh_data)
{
	data.insert(data.end(), fresh_data.begin(), fresh_data.size());
}

void Parser::Parse()
{
	printf("%.*s\n", data.size(), data.begin());
	ThreadPool::GetInstance().AddTask(new ResponseTask(cid));
}
