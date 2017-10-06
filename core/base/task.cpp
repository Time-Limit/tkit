#include "task.h"
#include "neter.h"
#include "channel.h"

HttpResponseTask::~HttpResponseTask()
{
	ChannelManager::GetInstance().PutData(cid, response.c_str(), response.size());
}

HttpResponseTask::res_data_t HttpResponseTask::GenResData(const HttpParser::Response &res)
{
	res_stream_t streamer;
	streamer << res.version <<  " " << res.status << " " << res.statement << "\r\n";
	for(const auto &p : res.headers)
	{
		streamer << p.first <<  ": " << p.second << "\r\n";
	}
	streamer << "\r\n";
	streamer << res.body;
	return streamer.str();
}
