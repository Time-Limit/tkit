#ifndef _LOG_H_
#define _LOG_H_

#include <stdio.h>
#include <stdarg.h>
#include <string>
#include <array>

namespace TCORE
{

class Log
{
public:
	enum LOG_LEVEL
	{
		LL_DEBUG = 0,
		LL_TRACE = 1,
		LL_ERROR = 2,

		LL_COUNT,
	};

private:
	int level; 
	std::array<int, LL_COUNT> log_file;

	Log()
	: level(LL_TRACE)
	, log_file({2, 2, 2})
	{}

	void Output(LOG_LEVEL level, const std::string &info);

public:
	static Log& GetInstance() { static Log instance;  return instance; }

	static void LOG_DEBUG(const char *fmt, ...)
	{
	}

	static void LOG_TRACE(const char *fmt, ...)
	{
	}

	static void LOG_ERROR(const char *fmt, ...)
	{
	}
};

//static Log _log_instance_;

struct LogIniter
{
	LogIniter()
	{
		Log::GetInstance();
	}
};

namespace
{
	static LogIniter _log_inter;
}

}

#endif
