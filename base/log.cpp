#include "log.h"

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
