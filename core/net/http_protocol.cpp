#include "protocol.h"
#include "basetool.h"
#include "octets.h"
#include "enums.h"
#include "log.h"

using namespace TCORE;

OctetsStream& HttpResponse::Deserialize(OctetsStream &os)
{
	enum PARSE_STATE
	{
		PARSE_LINE = 0,
		PARSE_HEADER,
		PARSE_BODY,
		PARSE_CHUNK,
		PARSE_DONE,
	};

	typedef unsigned char parse_state_t;

	for(parse_state_t state = PARSE_LINE; state < PARSE_DONE; )
	{
		switch(state)
		{
			case PARSE_LINE:
			{
				unsigned char c = 0;
				for(os >> c; c != ' '; os >> c)
				{
					version += c;
				}

				for(status = 0, os >> c; c != ' '; os >> c)
				{
					if(c < '0' || '9' < c)
					{
						throw "except number";
					}
					(status *= 10) += c-'0';
				}

				unsigned char next = 0;
				for(os >> c >> next; c != '\r' || next != '\n'; c = next, os >> next)
				{
					statement += c;
				}
				state = PARSE_HEADER;
			}
			break;
			case PARSE_HEADER:
			{
				std::string key, value;
				unsigned char a = 0, b = 0;
				enum
				{
					PARSE_HEADER_KEY = 0,
					PARSE_HEADER_INTERVAL = 1,
					PARSE_HEADER_VALUE = 2,
					PARSE_HEADER_END = 3,
					PARSE_HEADER_DONE = 4,
				};
				unsigned char where = PARSE_HEADER_KEY;
				os >> b;
				while(where != PARSE_HEADER_DONE)
				{
					a = b;
					os >> b;
					switch(where)
					{
					case PARSE_HEADER_KEY:
					{
						if(a == ':')
						{
							if(b == ' ')
							{
								where = PARSE_HEADER_INTERVAL;
							}
							else
							{
								where = PARSE_HEADER_VALUE;
							}
						}
						else
						{
							key += a;
						}
					}
					break;
					case PARSE_HEADER_INTERVAL:
					{
						if(a == ' ' && b == ' ')
						{
						}
						else
						{
							where = PARSE_HEADER_VALUE;
						}
					}
					break;
					case PARSE_HEADER_VALUE:
					{
						if(a == '\r' && b == '\n')
						{
							where = PARSE_HEADER_END;
						}
						else
						{
							value += a;
						}
					}
					break;
					case PARSE_HEADER_END:
					{
						if(a == '\n')
						{
							headers.insert(make_pair(key, value));
							key = std::string();
							value = std::string();
							if(b != '\r')
							{
								where = PARSE_HEADER_KEY;
							}
						}
						else if(a == '\r' && b == '\n')
						{
							where = PARSE_HEADER_DONE;
							state = PARSE_BODY;
						}
					}
					break;
					case PARSE_HEADER_DONE:
					{
					}
					break;
					}
				}
			}
			break;
			case PARSE_BODY:
			{
				std::map<std::string, std::string>::const_iterator cit = headers.find("Content-Length");

				int length = 0;

				if(cit != headers.end())
				{
					length = atoi(cit->second.c_str());
				}

				unsigned char c;
				while(--length >= 0)
				{
					os >> c;
					body += c;
				}

				state = PARSE_DONE;
			};
			break;
			default:
			{
				Log::Error("HttpRequest::Deserialize, wrong parse state");
				throw;
			}
			break;
		}
	}
	return os;
}

OctetsStream& HttpResponse::Serialize(OctetsStream &os) const
{
	os.push_back(version.c_str(), version.c_str()+version.size());
	os << ' ';
	std::stringstream ss_status;
	ss_status << status;
	std::string str = ss_status.str();
	os.push_back(str.c_str(), str.c_str() + str.size());
	os << ' ';
	os.push_back(statement.c_str(), statement.c_str() + statement.size());
	os << '\r' << '\n';

	for(const auto &p : headers)
	{
		os.push_back(p.first.c_str(), p.first.c_str() + p.first.size());
		os << ':' << ' ';
		os.push_back(p.second.c_str(), p.second.c_str() + p.second.size());
		os << '\r' << '\n';
	}

	os << '\r' << '\n';

	os.push_back(body.c_str(), body.c_str() + body.size());

	return os;
}

OctetsStream& HttpRequest::Serialize(OctetsStream &os) const
{
	os.push_back(method.c_str(), method.c_str() + method.size());
	os << ' ';
	os.push_back(url.c_str(), url.c_str() + url.size());
	os << ' ';
	os.push_back(version.c_str(), version.c_str() + version.size());
	os << '\r' << '\n';
	for(const auto &p : headers)
	{
		os.push_back(p.first.c_str(), p.first.c_str() + p.first.size());
		os << ':' << ' ';
		os.push_back(p.second.c_str(), p.second.c_str() + p.second.size());
		os << '\r' << '\n';
	}

	os << '\r' << '\n';
	os.push_back(body.c_str(), body.c_str() + body.size());

	return os;
}

OctetsStream& HttpRequest::Deserialize(OctetsStream &os)
{
	enum PARSE_STATE
	{
		PARSE_LINE = 0,
		PARSE_HEADER,
		PARSE_BODY,
		PARSE_DONE,
	};

	typedef unsigned char parse_state_t;

	for(parse_state_t state = PARSE_LINE; state < PARSE_DONE; )
	{
		switch(state)
		{
			case PARSE_LINE:
			{
				unsigned char c = 0;
				for(os >> c; c != ' '; os >> c)
				{
					method += c;
				}
				for(os >> c; c != ' '; os >> c)
				{
					url += c;
				}
				unsigned char next = 0;
				for(os >> c >> next; c != '\r' || next != '\n'; c = next, os >> next)
				{
					version += c;
				}
				state = PARSE_HEADER;
			}
			break;
			case PARSE_HEADER:
			{
				std::string key, value;
				unsigned char a = 0, b = 0;
				enum
				{
					PARSE_HEADER_KEY = 0,
					PARSE_HEADER_INTERVAL = 1,
					PARSE_HEADER_VALUE = 2,
					PARSE_HEADER_END = 3,
					PARSE_HEADER_DONE = 4,
				};
				unsigned char where = PARSE_HEADER_KEY;
				os >> b;
				while(where != PARSE_HEADER_DONE)
				{
					a = b;
					os >> b;
					switch(where)
					{
					case PARSE_HEADER_KEY:
					{
						if(a == ':')
						{
							if(b == ' ')
							{
								where = PARSE_HEADER_INTERVAL;
							}
							else
							{
								where = PARSE_HEADER_VALUE;
							}
						}
						else
						{
							key += a;
						}
					}
					break;
					case PARSE_HEADER_INTERVAL:
					{
						if(a == ' ' && b == ' ')
						{
						}
						else
						{
							where = PARSE_HEADER_VALUE;
						}
					}
					break;
					case PARSE_HEADER_VALUE:
					{
						if(a == '\r' && b == '\n')
						{
							where = PARSE_HEADER_END;
						}
						else
						{
							value += a;
						}
					}
					break;
					case PARSE_HEADER_END:
					{
						if(a == '\n')
						{
							headers.insert(make_pair(key, value));
							key = std::string();
							value = std::string();
							if(b != '\r')
							{
								where = PARSE_HEADER_KEY;
							}
						}
						else if(a == '\r' && b == '\n')
						{
							where = PARSE_HEADER_DONE;
							state = PARSE_BODY;
						}
					}
					break;
					case PARSE_HEADER_DONE:
					{
					}
					break;
					}
				}
			}
			break;
			case PARSE_BODY:
			{
				std::map<std::string, std::string>::const_iterator cit = headers.find("Content-Length");

				int length = 0;

				if(cit != headers.end())
				{
					length = atoi(cit->second.c_str());
				}

				unsigned char c;
				while(--length >= 0)
				{
					os >> c;
					body += c;
				}

				state = PARSE_DONE;
			};
			break;
			default:
			{
				Log::Error("HttpRequest::Deserialize, wrong parse state");
				throw;
			}
			break;
		}
	}

	std::vector<std::string> result = split_string(url, "?");

	if(result.size() == 2)
	{
		url = result[0];

		result = split_string(result[1], "&");
		for(const auto &arg : result)
		{
			std::vector<std::string> kv = split_string(arg, "=");
			if(kv.size() == 1)
			{
				args[kv[0]] = "";
			}
			else if(kv.size() == 2)
			{
				args[kv[0]] = kv[1];
			}
		}
	}

	return os;
}
