#include "task.h"
#include "neter.h"
#include "channel.h"
#include "enums.h"
#include "thread.h"
#include "basetool.h"

bool HttpRequestTask::IsValidEscapeChar(unsigned int c)
{
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
	if(request.headers.find("Transfer-Encoding") != request.headers.end())
	{
		ResetHttpResponseStatus(response, HTTP_SC_LENGTH_REQUIRED);
	}

	// forbid some methods
	if(request.method != "GET" && request.method != "HEAD")
	{
		ResetHttpResponseStatus(response, HTTP_SC_LENGTH_REQUIRED);
		response.headers["Allow"] = "GET, HEAD";
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
}

void HttpRequestTask::ExtendBaseCheckRequest()
{
}

void HttpRequestTask::LogicCheckRequest()
{
}

void HttpRequestTask::ConstructResponse()
{
}

void HttpRequestTask::CompleteResponse()
{
	// if request's method is HEAD, then body should be cleared.
	if(request.method == "HEAD")
	{
		response.body = "";
	}

	if( (response.status != HTTP_SC_OK && response.body.size() == 0)
		|| (response.headers.size() == 0 && response.body.size() == 0))
	{
		response.body =
		"<html>"
		"<h1 align=\"center\">" + tostring(response.status) + " " + response.statement + "</h1>"
		"<hr></hr>"
		"<p align=\"center\">tcore</p>"
		"</html>";
		response.headers["Content-Length"] = tostring(response.body.size());
		response.headers["Content-Type"] = GetMimeType("html");
	}
	ThreadPool::GetInstance().AddTask(new HttpResponseTask(cid, response));
}

void HttpRequestTask::ExtendCompleteResponse()
{
}

void HttpResponseTask::Exec()
{
	res_stream_t streamer;
	streamer << response.version <<  " " << response.status << " " << response.statement << "\r\n";
	for(const auto &p : response.headers)
	{
		streamer << p.first <<  ": " << p.second << "\r\n";
	}
	streamer << "\r\n";
	streamer << response.body;
	ChannelManager::GetInstance().PutData(cid, streamer.str().c_str(), streamer.str().size());
}
