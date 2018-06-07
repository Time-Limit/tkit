#include "log.h"

using namespace TCORE;

void Log::Output(LOG_LEVEL level, const std::string &info)
{
	write(log_file, info.c_str(), info.size());
}

//static Log _log_instance_;

/*
namespace Log
{
	void Error(const char * fmt, ...)
	{
		va_list argptr;
		va_start(argptr, fmt);
		vfprintf(stderr, fmt, argptr);
		va_end(argptr);
	}

	void Trace(const char * fmt, ...)
	{
		va_list argptr;
		va_start(argptr, fmt);
		vfprintf(stdout, fmt, argptr);
		va_end(argptr);
	}
};
*/
