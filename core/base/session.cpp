#include "session.h"
#include "channel.h"
#include "neter.h"
#include "thread.h"
#include "protocol.h"

Session::Session(int fd)
: sid(fd)
, exchanger(nullptr)
{
}

Session::~Session()
{
	delete exchanger;
	exchanger = nullptr;
}

void Session::Close()
{
	manager->DelSession(this);
}

void Session::DataOut(const char *data, size_t size)
{
	exchanger->Send(data, size);
}

HttpSession::HttpSession(int fd)
: Session(fd)
{}

void SessionManager::DelSession(Session *session)
{
	if(session)
	{
		MutexGuard guarder(session_map_lock);
		SessionMap::iterator it = session_map.find(session->ID());

		if(it != session_map.end())
		{
			session_map.erase(it);
		}

		delete session;
	}
}

void SessionManager::Send(session_id_t sid, const char *data, size_t size)
{
	SessionMap::iterator it = session_map.find(sid);

	if(it == session_map.end())
	{
		return ;
	}

	it->second->DataOut(data, size);
}

void SessionManager::AddSession(Session *session)
{
	MutexGuard guard(session_map_lock);
	Exchanger *exchanger= new Exchanger(session->ID(), session);
	session->SetExchanger(exchanger);
	session->SetManager(this);
	session_map.insert(std::make_pair(session->ID(), session));

	epoll_event ev;
	ev.events = EPOLLIN|EPOLLET;
	ev.data.ptr = exchanger;
	LOG_TRACE("SessionManager::Add, fd=%d, ip=%s", session->ID(), exchanger->IP());
	Neter::GetInstance().Ctl(EPOLL_CTL_ADD, session->ID(), &ev);
}

SessionManager::~SessionManager()
{
	for(auto &it : session_map)
	{
		delete it.second;
		it.second = nullptr;
	}

	session_map.clear();
}

void HttpSession::Parse()
{
	enum PARSE_STATE
	{
		PARSE_LINE = 0,
		PARSE_HEADER,
		PARSE_BODY,
		PARSE_DONE,
	};
	
#define parse_state_t unsigned char 
	
	const char *begin = (const char *)recv_data.begin() , *end = (const char *)recv_data.end(), *tmp = NULL;
	
	HttpRequest req;

	for(parse_state_t state = PARSE_LINE; state < PARSE_DONE;)
	{
		switch(state)
		{
			case PARSE_LINE:
			{
				if(begin >= end) return ;
				tmp = begin;

				for(; begin < end && (*begin != ' '); ++begin);
				if(begin == end) return ;
				req.method = std::string(tmp, begin-tmp);

				for(tmp = ++begin; begin < end && (*begin != ' '); ++begin);
				if(begin == end) return ;
				req.url	   = std::string(tmp, begin-tmp);

				for(tmp = ++begin; begin < end && (*begin != '\r'); ++begin);
				if(begin == end) return ;
				req.version = std::string(tmp, begin-tmp);

				if(begin+1 == end)
				{
					return ;
				}
				begin += 2;
				state = PARSE_HEADER;
			}
			break;
			case PARSE_HEADER:
			{
				if(begin >= end) return ;
				
				std::string key, value;
				while(begin < end)
				{
					for(tmp = begin; begin < end && (*begin != ':'); ++begin);
					if(begin == end) return ;
					key = std::string(tmp, begin-tmp);

					for(tmp = ++begin; begin < end && (*begin == ' '); ++begin);
					if(begin == end) return ;
					
					for(tmp = begin; begin < end && (*begin != '\r'); ++begin);
					if(begin == end) return ;
					value = std::string(tmp, begin-tmp);
					req.headers.insert(make_pair(key, value));

					if(begin+3 < end)
					{
						if(*++begin == '\n' && *++begin == '\r' && *++begin== '\n')
						{
							++begin;
							state = PARSE_BODY;
							break;
						}
					}
					else if(begin+1 >= end)
					{
						return ;
					}
					else
					{
						begin += 2;
					}
				}
			}
			break;
			case PARSE_BODY:
			{
				std::map<std::string, std::string>::const_iterator cit = req.headers.find("Content-Length");

				int length = 0;

				if(cit != req.headers.end())
				{
					length = atoi(cit->second.c_str());
				}

				if(begin + length > end)
				{
					return ;
				}

				req.body = std::string(begin, length);
				begin += length;
				state = PARSE_DONE;
			}
			break;
			default:
			{
				LOG_ERROR("Parser::Parse, wrong parse state");
				assert(false);
			}
		};
	}

	recv_data.erase((char *)recv_data.begin(), (char *)begin);

	ThreadPool::GetInstance().AddTask(new HandleNetProtocolTask(GetManager(), ID(), GenHttpProtocol(req)));
#undef parse_state_t
}

HttpProtocol* HttpSession::GenHttpProtocol(const HttpRequest &req)
{
	return new HttpProtocol(req);
}

void HttpSessionManager::OnConnect(int fd)
{
	HttpSession *session = new HttpSession(fd);
	AddSession(session);
}
