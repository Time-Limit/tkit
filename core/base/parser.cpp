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

void HttpParser::Parse()
{
	//printf("%.*s\n", data.size(), data.begin());
	//ThreadPool::GetInstance().AddTask(new ResponseTask(cid));
	
	enum PARSE_STATE
	{
		PARSE_LINE = 0,
		PARSE_HEADER,
		PARSE_BODY,
		PARSE_DONE,
	};
	
#define parse_state_t unsigned char 
	
	const char *begin = (const char *)data.begin() , *end = (const char *)data.end();
	
	Request req;

	for(parse_state_t state = PARSE_LINE; state < PARSE_DONE;)
	{
		switch(state)
		{
			case PARSE_LINE:
			{
				if(begin >= end) return ;
				const char * tmp = begin;

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
				const char * tmp = NULL;
				
				std::string key, value;
				while(begin < end)
				{
					tmp = begin;
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

	printf("size of data is %d\n", data.size());

	ThreadPool::GetInstance().AddTask(new ResponseTask(cid));
#undef parse_state_t
}









