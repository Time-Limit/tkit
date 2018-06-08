#ifndef _LOG_H_
#define _LOG_H_

#include <stdio.h>
#include <stdarg.h>
#include <string>
#include <array>
#include <tuple>
#include <sstream>

namespace TCORE
{

class Log
{
	enum LOG_LEVEL
	{
		LL_DEBUG = 0,
		LL_TRACE = 1,
		LL_ERROR = 2,

		LL_COUNT,
	};

	int level; 
	std::array<int, LL_COUNT> log_file;

	Log()
	: level(LL_TRACE)
	, log_file({2, 2, 2})
	{}

	template<size_t I, typename TUPLE>
	struct Output
	{
		static void Print(std::stringstream &ss, const TUPLE &t)
		{
			ss << std::get<I-1>(t);
			if(I > 1)
			{
				Output<I-1>::Print(ss, t);
			}
		}

		static void Print(const TUPLE &t, LOG_LEVEL level)
		{
			std::stringstream ss;
			Output<I>::Print(ss, t);
		}
	}

public:
	static Log& GetInstance() { static Log instance;  return instance; }

	template<typename ...Args>
	static void Debug(const Args &...args)
	{
		Output<sizeof...(Args)>::Print(std::make_tuple(args...), LL_DEBUG);
	}

	template<typename ...Args>
	static void Trace(const Args &...args)
	{
		Output<sizeof...(Args)>::Print(std::make_tuple(args...), LL_TRACE);
	}

	template<typename ...Args>
	static void Error(const Args &...args)
	{
		Output<sizeof...(Args)>::Print(std::make_tuple(args...), LL_ERROR);
	}
};

using LOG_

//static Log _log_instance_;

namespace
{
struct LogIniter
{
	LogIniter()
	{
		Log::GetInstance();
	}
};

static LogIniter _log_inter;
}

}

#endif
