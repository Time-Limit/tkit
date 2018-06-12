#include "task.h"
#include "neter.h"
#include "channel.h"
#include "enums.h"
#include "thread.h"
#include "basetool.h"

using namespace TCORE;

bool HttpRequestTask::IsValidEscapeChar(unsigned int c)
{
	if('0' <= c && c <= '9') return true;
	if('a' <= c && c <= 'z') return true;
	if('A' <= c && c <= 'Z') return true;
	return (   c == ';' || c == '/' || c == '?' || c == ':' || c == '@'
		|| c == '=' || c == '&' || c == '<' || c == '>' || c == '"'
		|| c == '#' || c == '%' || c == '{' || c == '}' || c == '|'
		|| c == '\\' || c == '^' || c == '~' || c == '[' || c == ']' ); 
}

void HttpRequestTask::BaseCheckRequest()
{
	// construct request-line
	response.version = "HTTP/1.1";
	ResetHttpResponseStatus(response, HTTP_SC_OK);

	// interpret escape character of url
	std::string tmp_url(request.url.size(), '\0');
	for(size_t i = 0, len = request.url.size(), ti = 0; i < len; ++i)
	{
		if(request.url[i] == '%')
		{
			if(i+2 < len)
			{
				unsigned int c = ((request.url[i+1]-'0') << 4) + (request.url[i+2]-'0');
				if(IsValidEscapeChar(c))
				{
					tmp_url[ti++] = c;
					i += 2;
				}
				else
				{
					ResetHttpResponseStatus(response, HTTP_SC_BAD_REQUEST);
					break;
				}
			}
			else
			{
				ResetHttpResponseStatus(response, HTTP_SC_BAD_REQUEST);
				break;
			}
		}
		else
		{
			tmp_url[ti++] = request.url[i];
		}
	}
	if(response.status == HTTP_SC_OK)
	{
		request.url = std::move(tmp_url);
	}

	// forbid some headers
	if(request.headers.find(HTTP_TRANSFER_ENCODING) != request.headers.end())
	{
		ResetHttpResponseStatus(response, HTTP_SC_LENGTH_REQUIRED);
	}

	// forbid some methods
	if(request.method != "GET" && request.method != "HEAD")
	{
		ResetHttpResponseStatus(response, HTTP_SC_LENGTH_REQUIRED);
		ForceSetHeader(response, HTTP_ALLOW, "GET, HEAD");
	}

	// forbid ".." in url.
	for(size_t i = 1, len = request.url.size(); i < len; ++i)
	{
		if(request.url[i-1] == '.' && request.url[i] == '.')
		{
			ResetHttpResponseStatus(response, HTTP_SC_FORBIDDEN);
			response.statement = "forbid \"..\" in url.";
		}
	}

	// parser query
	const char *begin = request.url.c_str() , *end = request.url.c_str() + request.url.size(), *tmp = NULL;
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

				request.args[key] = value;

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
			request.args[key] = value;	
		}
		else if(tmp < begin)
		{
			key = std::string(tmp, begin);
			request.args[key] = value;
		}
	}

	//truncate url
	size_t pos = request.url.find(';');	
	if(pos != std::string::npos) request.url.erase(pos, request.url.size() - pos);
	pos = request.url.find('#');	
	if(pos != std::string::npos) request.url.erase(pos, request.url.size() - pos);
	pos = request.url.find('?');	
	if(pos != std::string::npos) request.url.erase(pos, request.url.size() - pos);

	// complete url
	if(request.url == "/")
	{
		request.url = "/index.html";
	}
}

void HttpRequestTask::CompleteResponse()
{
	//complete response
	if(response.body.size() == 0)
	{
		response.body =
		"<html>"
		"<h1 align=\"center\">" + tostring(response.status) + " " + response.statement + "</h1>"
		"<hr></hr>"
		"<p align=\"center\">tcore</p>"
		"</html>";
		ForceSetHeader(response, HTTP_CONTENT_LENGTH, tostring(response.body.size()));
		ForceSetHeader(response, HTTP_CONTENT_TYPE, GetMimeType("html"));
	}
	TrySetHeader(response, HTTP_CONTENT_TYPE, GetMimeType(GetFileSuffixName(request.url)));
	TrySetHeader(response, HTTP_CONTENT_LENGTH, tostring(response.body.size()));

	// 2018-06-06 开始支持 Keep-Alive
	// 暂不支持Keep-Alive Orz
	// 但是服务器不会主动断连，所以先注释掉吧
	//ForceSetHeader(response, HTTP_CONNECTION, "close");

	// if request's method is HEAD, then body should be cleared.
	if(request.method == "HEAD")
	{
		response.body = "";
	}

	manager->Send(sid, response);
}

void HandleNetProtocolTask::Exec()
{
	SessionManager::ProtocolHandler ph = manager->GetProtocolHandler();
	if(ph)
	{
		ph(manager, sid, *protocol);
	}
	delete protocol;
	protocol = nullptr;
}

void ConnectTask::Exec()
{
	///Connector connect(ip.c_str(), port, *manager);
	///if(connect.Connect())
	///{
	///	return ;
	///}
}
