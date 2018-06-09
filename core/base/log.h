#ifndef _LOG_H_
#define _LOG_H_

#include <stdio.h>
#include <stdarg.h>
#include <string>
#include <tuple>
#include <sstream>
#include <iostream>
#include <array>
#include <functional>

namespace TCORE
{

class Log
{
public:
	typedef std::function<void (const std::string &info)> OutputHandle;
	typedef unsigned char level_t;

private:
	enum LOG_LEVEL
	{
		LL_DEBUG = 0,
		LL_TRACE = 1,
		LL_ERROR = 2,

		LL_COUNT,
	};

	static void default_output_handle(const std::string &info)
	{
		std::cout << info << std::endl;
	}

	typedef std::array<OutputHandle, LL_COUNT> OutputHandleArray;

	level_t level; 
	OutputHandleArray output_handle_array;

	Log()
	: level(LL_TRACE)
	, output_handle_array({default_output_handle, default_output_handle, default_output_handle})
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

	void ResetDebugOutputHandle(OutputHandle handle = default_output_handle) { output_handle_array[LL_DEBUG] = handle; }
	void ResetTraceOutputHandle(OutputHandle handle = default_output_handle) { output_handle_array[LL_TRACE] = handle; }
	void ResetErrorOutputHandle(OutputHandle handle = default_output_handle) { output_handle_array[LL_ERROR] = handle; }

	bool CheckLevel(LOG_LEVEL l) const
	{
		return l >= level;
	}

public:
	static Log& GetInstance() { static Log instance;  return instance; }

	template<typename ...Args>
	static void Debug(const Args &...args)
	{
		if(GetInstance().CheckLevel(LL_DEBUG) && GetInstance().output_handle_array[LL_DEBUG])
		{
			auto t = std::make_tuple(args...);
			std::stringstream ss;
			Output<sizeof...(Args), decltype(t)>::Print(ss, t);
			GetInstance().output_handle_array[LL_DEBUG](ss.str());
		}
	}

	template<typename ...Args>
	static void Trace(const Args &...args)
	{
		if(GetInstance().CheckLevel(LL_TRACE) && GetInstance().output_handle_array[LL_DEBUG])
		{
			auto t = std::make_tuple(args...);
			std::stringstream ss;
			Output<sizeof...(Args), decltype(t)>::Print(ss, t);
			GetInstance().output_handle_array[LL_TRACE](ss.str());
		}
	}

	template<typename ...Args>
	static void Error(const Args &...args)
	{
		if(GetInstance().CheckLevel(LL_ERROR) && GetInstance().output_handle_array[LL_DEBUG])
		{
			auto t = std::make_tuple(args...);
			std::stringstream ss;
			Output<sizeof...(Args), decltype(t)>::Print(ss, t);
			GetInstance().output_handle_array[LL_ERROR](ss.str());
		}
	}
};

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
