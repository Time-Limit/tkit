#include "basetool.h"

std::vector<std::string> split_string(const std::string &_src, const std::string &split)
{
	std::vector<std::string> result;
	std::string src = _src + split;
	size_t lsrc = src.size(), lsplit = split.size();
	if(lsplit <= 0 || lsrc <= 0 || lsrc <= lsplit) return result;

	size_t pre = 0, now = 0;

	for(; now + lsplit <= lsrc; )
	{
		bool flag = true;
		for(size_t i = 0; i < lsplit; ++i)
		{
			if(split[i] != src[now+i])
			{
				flag = false;
				break;
			}
		}

		if(flag)
		{
			if(pre != now)
			{
				result.push_back(src.substr(pre, now-pre));
			}
			now += lsplit;
			pre = now;
		}
		else
		{
			++now;
		}
	}

	return result;
}
