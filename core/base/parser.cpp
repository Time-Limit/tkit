#include "parser.h"
#include "task.h"
#include "channel.h"
#include "thread.h"
#include <sstream>

class ResponseTask : public Task
{
	channel_id_t cid;
	HttpParser::Request request;
public:
	ResponseTask(channel_id_t c, const HttpParser::Request &req)
	: cid(c)
	, request(req)
	{}
	void Exec()
	{
		sleep(3);
		std::stringstream stream;
		stream << "url = " << request.url << "\n";
		stream << "method = " << request.method << "\n";
		stream << "version = " << request.version << "\n";
		stream << "headers  = " << request.headers.size() << "\n";

		std::map<std::string, std::string>::iterator it = request.headers.begin();
		std::map<std::string, std::string>::iterator ie = request.headers.end();

		for(; it != ie; ++it)
		{
			stream << "header->"<<it->first<<" = "<< it->second << "\n";
		}

		stream << "body = " << request.body << "\n";

		it = request.args.begin();
		ie = request.args.end();
		
		stream << "args = " << request.args.size() << "\n";

		for(; it != ie; ++it)
		{
			stream << "arg->" << it->first << " = " << it->second << "\n";
		}
		
		std::stringstream response;

		response << "HTTP/1.1 200 SUCCESS\r\nContent-Length:" << stream.str().size()<<"\r\n\r\n" << stream.str();

		ChannelManager::GetInstance().PutData(cid, response.str().c_str(), response.str().size());
	}
};

void Parser::Append(const Octets &fresh_data)
{
	data.insert(data.end(), fresh_data.begin(), fresh_data.size());
}

void HttpParser::Parse()
{
	enum PARSE_STATE
	{
		PARSE_LINE = 0,
		PARSE_HEADER,
		PARSE_BODY,
		PARSE_DONE,
	};
	
#define parse_state_t unsigned char 
	
	const char *begin = (const char *)data.begin() , *end = (const char *)data.end(), *tmp = NULL;
	
	Request req;

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
				Log::Error("Parser::Parse, wrong parse state");
				assert(false);
			}
		};
	}

	data.erase((char *)data.begin(), (char *)begin);

	begin = req.url.c_str();
	end = req.url.c_str() + req.url.size();

	for(; begin < end && *begin != '?'; ++begin) ;

	if(begin != end)
	{
		std::string key, value;
		for(tmp = ++begin; begin < end;)
		{
			if(*begin == '=')
			{
				key = std::string(tmp, begin);
				tmp = ++begin;
			}
			else if(*begin == '&')
			{
				value = std::string(tmp, begin);
				tmp = ++begin;

				req.args[key] = value;

				key.clear();
				value.clear();
			}
			else
			{
				++begin;
			}
		}
		if(key.size())
		{
			value = std::string(tmp, begin);
			req.args[key] = value;	
		}
		else if(tmp < begin)
		{
			key = std::string(tmp, begin);
			req.args[key] = value;
		}
	}

	ThreadPool::GetInstance().AddTask(new ResponseTask(cid, req));
#undef parse_state_t
}









