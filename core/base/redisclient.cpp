#include "redisclient.h"

using namespace TCORE;

bool RedisClient::Connect(const std::string &ip, int port)
{
	Neter::Callback::Connect::Func connect_callback = [&](session_id_t sid)->void { redis_server_sid = sid; is_connect = true; };
	Neter::Callback::Disconnect::Func disconnect_callback = [&](session_id_t sid, const std::string &ip, int port)->void { is_connect = false; redis_server_sid = 0; rcb_list.clear(); };
	Neter::Callback::Response<RESP>::Func response_callback = [&](RESP &&response, session_id_t sid)->void
								{
									ResponseCallback rcb = rcb_list.front();
									rcb_list.pop_front();
									rcb(std::move(response));
								};
	return Neter::Connect<RESP>(ip, port, connect_callback, disconnect_callback, response_callback);
}

bool RedisClient::Command(ResponseCallback rcb, const std::string &cmd)
{
	RESP::ary_t cmdarr;
	cmdarr.push_back(RESP::Value(RESP::blk_t(cmd.c_str(), cmd.size())));
	return Command(rcb, RESP(RESP::Value(cmdarr)));
}

bool RedisClient::Command(ResponseCallback rcb, const std::string &cmd, const std::string &param)
{
	RESP::ary_t cmdarr;
	cmdarr.push_back(RESP::Value(RESP::blk_t(cmd.c_str(), cmd.size())));
	cmdarr.push_back(RESP::Value(RESP::blk_t(param.c_str(), param.size())));
	return Command(rcb, RESP(RESP::Value(cmdarr)));
}

bool RedisClient::Command(ResponseCallback rcb, const RESP &resp)
{
	if(IsConnect())
	{
		if(Neter::SendProtocol(redis_server_sid, resp))
		{
			rcb_list.push_back(rcb);
			return true;
		}
	}
	return false;
}
