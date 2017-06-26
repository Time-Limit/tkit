#include <stdio.h>
#include <stdarg.h>

namespace Log
{
	void Error(const char * fmt, ...);
	void Trace(const char * fmt, ...);
};
