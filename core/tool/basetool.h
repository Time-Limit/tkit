#ifndef _BASETOOL_H_
#define _BASETOOL_H_

#include <sstream>
#include <vector>

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

std::vector<std::string> split_string(const std::string &src, const std::string &split);

#endif
