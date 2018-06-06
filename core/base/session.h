#ifndef _SESSION_H_
#define _SESSION_H_

#include "octets.h"
#include "lock.h"
#include "exptype.h"
#include <map>
#include <functional>

class Exchanger;
class SessionManager;
class HttpProtocol;
class HttpRequest;

class Session
{
private:
	session_id_t sid;
	SessionManager *manager;

protected:
	Octets recv_data;
	Exchanger *exchanger;


public:
	explicit Session(int fd);

	virtual ~Session();

	session_id_t ID() const { return sid; }

	void DataOut(const char *data, size_t size);

	void DataIn(const Octets &data)
	{
		recv_data.insert(recv_data.end(), data.begin(), data.size());
		OnDataIn();
	}

	void Close();

	virtual void SetExchanger();

	Exchanger * GetExchanger() const { return exchanger; };

	void SetManager(SessionManager *m) { manager = m; }

protected:
	SessionManager * GetManager() { return manager; }

private:
	virtual void OnDataIn() = 0;
};

class SessionManager
{
public:
	typedef std::function<void (SessionManager*, session_id_t, Protocol&)> ProtocolHandler;
public:
	SessionManager();
	SessionManager(ProtocolHandler ph);
	virtual ~SessionManager();

	typedef std::map<session_id_t, Session*> SessionMap;

	virtual void OnConnect(int fd) = 0;

	void Send(session_id_t sid, const char *data, size_t size);
	void Send(session_id_t sid, const Protocol &p);
	void DelSession(Session *session);
	ProtocolHandler GetProtocolHandler() const { return protocol_handler; }
	void SetProtocolHandler(ProtocolHandler p) { protocol_handler = p; }

protected:
	void AddSession(Session *session);

private:
	Mutex session_map_lock;
	SessionMap session_map;
	ProtocolHandler  protocol_handler;
};

class HttpSession : public Session
{
public:
	explicit HttpSession(int fd);

private:
	virtual void OnDataIn() override;
};

class HttpSessionManager : public SessionManager
{
public:
	HttpSessionManager() : SessionManager() {}
	HttpSessionManager(ProtocolHandler ph) : SessionManager(ph) {}
	virtual void OnConnect(int fd) override;
};

#endif
