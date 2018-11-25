#ifndef _CALLBACK_H_
#define _CALLBACK_H_

#include <functional>
#include <unordered_map>
#include <string>
#include "lock.h"
#include "threadpool.h"
#include "protocol.h"

namespace TCORE
{

typedef std::function<const HttpResponse (const HttpRequest &)> HttpCallback;

template<typename Callback>
class CallbackManager{
	typedef std::unordered_map<std::string, Callback> CallbackMap;
	CallbackMap callback_map;

	mutable SpinLock callback_map_lock;

	CallbackManager() {}

public:
	static CallbackManager<Callback> &GetInstance(){
		static CallbackManager<Callback> m;
		return m;
	}

	void Set(const std::string &key, const Callback &cb){
		SpinLockGuard guard(callback_map_lock);
		auto it = callback_map.find(key);
		if(it == callback_map.end()){
			callback_map.insert(std::make_pair(key, cb));
		}
		else{
			it->second = cb;
		}
	}

	bool Get(const std::string &key, Callback &cb) const {
		SpinLockGuard guard(callback_map_lock);
		auto it = callback_map.find(key);
		if(it == callback_map.end()){
			return false;
		}
		cb = it->second;
		return true;
	}
};

class HttpCallbackTask : public Task{
public:
	HttpCallbackTask(session_id_t sid, const HttpRequest &req)
		: Task(sid), sid(sid), req(req){}

	void Exec() override;
private:
	session_id_t sid;
	HttpRequest req;
};

}


#endif
