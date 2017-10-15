#ifndef __BASETOOL_H_
#define __BASETOOL_H_

#include <sstream>
template<typename T>
inline std::string tostring(const T &args)
{
	std::stringstream ss;	
	ss << args;
	return ss.str();
}

#endif
