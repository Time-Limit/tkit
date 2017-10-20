#include <stdio.h>
#include <stdarg.h>

#define LOG_TRACE(fmt, ...) { fprintf(stdout, fmt"\n", ##__VA_ARGS__); }
#define LOG_ERROR(fmt, ...) { fprintf(stderr, fmt"\n", ##__VA_ARGS__); }

/*
namespace Log
{
	void Error(const char * fmt, ...);
	void Trace(const char * fmt, ...);
};
*/
