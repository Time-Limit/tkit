#ifndef _REDISCLIENT_H_
#define _REDISCLIENT_H_

#include <functional>
#include <list>
#include <tuple>

#include "protocol.h"
#include "neter.h"
#include "log.h"

namespace TCORE
{

class RedisClient
{
public:
	typedef std::function<void(RESP &&resp)> ResponseCallback;

private:
	bool is_connect;
	size_t command_id_hatcher;
	session_id_t redis_server_sid;

	typedef std::list<ResponseCallback> ResponseCallbackList;
	ResponseCallbackList rcb_list;

public:
	RedisClient()
	: is_connect(false)
	, command_id_hatcher(0)
	, redis_server_sid(0)
	{}

	bool Connect(const std::string &ip, int port);

	bool IsConnect() const { return is_connect; }

	bool Command(ResponseCallback rcb, const RESP &resp);

	bool Command(ResponseCallback rcb, const std::string &cmd);

	bool Command(ResponseCallback rcb, const std::string &cmd, const std::string &param);
};

}

#endif
