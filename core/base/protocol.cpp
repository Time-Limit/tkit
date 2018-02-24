#include "protocol.h"
#include "enums.h"
#include "basetool.h"
#include "session.h"
#include "log.h"

void HttpRequest::Handle(SessionManager *manager, session_id_t sid)
{
	HttpResponse response;
	response.version = "HTTP/1.1";
	ResetHttpResponseStatus(response, HTTP_SC_OK);

	response.body =
	"<html>"
	"<h1 align=\"center\">" + tostring(response.status) + " " + response.statement + "</h1>"
	"<hr></hr>"
	"<p align=\"center\">tcore</p>"
	"</html>";
	ForceSetHeader(response, HTTP_CONTENT_LENGTH, tostring(response.body.size()));
	ForceSetHeader(response, HTTP_CONTENT_TYPE, GetMimeType("html"));

	OctetsStream os;
	os << response;

	const Octets &data = os.GetData();

	manager->Send(sid, (const char *)data.begin(), data.size());
}

OctetsStream& HttpResponse::Deserialize(OctetsStream &os)
{
	enum PARSE_STATE
	{
		PARSE_LINE = 0,
		PARSE_HEADER,
		PARSE_BODY,
		PARSE_DONE,
	};

	typedef unsigned char parse_state_t;

	os >> OctetsStream::START;

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
				os >> status;
				unsigned char next = 0;
				for(os >> next >> c >> next; c != '\r' || next != '\n'; c = next, os >> next)
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
				LOG_ERROR("HttpRequest::Deserialize, wrong parse state");
				assert(false);
			}
			break;
		}
	}
	return os;
}

OctetsStream& HttpResponse::Serialize(OctetsStream &os) const
{
	os.Append(version.c_str(), version.c_str()+version.size());
	os << ' ';
	os << status;
	os << ' ';
	os.Append(statement.c_str(), statement.c_str() + statement.size());
	os << '\r' << '\n';

	for(const auto &p : headers)
	{
		os.Append(p.first.c_str(), p.first.c_str() + p.first.size());
		os << ':' << ' ';
		os.Append(p.second.c_str(), p.second.c_str() + p.second.size());
		os << '\r' << '\n';
	}

	os << '\r' << '\n';
	os.Append(body.c_str(), body.c_str() + body.size());

	return os;
}

OctetsStream& HttpRequest::Serialize(OctetsStream &os) const
{
	os.Append(method.c_str(), method.c_str() + method.size());
	os << ' ';
	os.Append(url.c_str(), url.c_str() + url.size());
	os << ' ';
	os.Append(version.c_str(), version.c_str() + version.size());
	os << '\r' << '\n';
	for(const auto &p : headers)
	{
		os.Append(p.first.c_str(), p.first.c_str() + p.first.size());
		os << ':' << ' ';
		os.Append(p.second.c_str(), p.second.c_str() + p.second.size());
		os << '\r' << '\n';
	}

	os << '\r' << '\n';
	os.Append(body.c_str(), body.c_str() + body.size());

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

	os >> OctetsStream::START;

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
				LOG_ERROR("HttpRequest::Deserialize, wrong parse state");
				assert(false);
			}
			break;
		}
	}

	return os;
}
