#ifndef _LOG_H_
#define _LOG_H_

#include <stdio.h>
#include <stdarg.h>
#include <string>
#include <tuple>
#include <sstream>
#include <iostream>

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

	Log()
	: level(LL_TRACE)
	{}

	template<size_t I, typename TUPLE>
	struct Output
	{
		static void Print(std::stringstream &ss, const TUPLE &t)
		{
			Output<I-1, decltype(t)>::Print(ss, t);
			ss<< std::get<I-1>(t);
		}
	};

	template<typename TUPLE>
	struct Output<1, TUPLE>
	{
		static void Print(std::stringstream &ss, const TUPLE &t)
		{
			ss << std::get<0>(t);
		}
	};

public:
	static Log& GetInstance() { static Log instance;  return instance; }

	template<typename ...Args>
	static void Debug(const Args &...args)
	{
		auto t = std::make_tuple(args...);
		std::stringstream ss;
		Output<sizeof...(Args), decltype(t)>::Print(ss, t);
		std::cout << ss.str() << std::endl;
	}

	template<typename ...Args>
	static void Trace(const Args &...args)
	{
		auto t = std::make_tuple(args...);
		std::stringstream ss;
		Output<sizeof...(Args), decltype(t)>::Print(ss, t);
		std::cout << ss.str() << std::endl;
	}

	template<typename ...Args>
	static void Error(const Args &...args)
	{
		auto t = std::make_tuple(args...);
		std::stringstream ss;
		Output<sizeof...(Args), decltype(t)>::Print(ss, t);
		std::cout << ss.str() << std::endl;
	}
};

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
