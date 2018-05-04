#ifndef _BASETOOL_H_
#define _BASETOOL_H_

#include <sstream>

template<typename T>
inline std::string tostring(const T &args)
{
	std::stringstream ss;	
	ss << args;
	return ss.str();
}

inline std::string GetFileSuffixName(const std::string &str)
{
	for(const char *p = str.c_str() + str.size() - 1, *q = str.c_str(); p >= q && *p != '/'; --p)
	{
		if(*p == '.')
		{
			return str.substr(p-q+1);	
		}
	}
	return "";
}

#define ResetHttpResponseStatus( r, s) { (r).status = (s); (r).statement = GetStatusCodeInfo(s); }
#define ExceptHeaderExist(r, h) if((r).headers.find((h)) != (r).headers.cend())
#define ExceptHeaderNotExist(r, h) if((r).headers.find((h)) == (r).headers.cend())
#define TrySetHeader(r, h, v) { ExceptHeaderNotExist((r), (h)) { (r).headers[(h)] = (v); } }
#define ForceSetHeader(r, h, v) { (r).headers[(h)] = (v); }

#endif
