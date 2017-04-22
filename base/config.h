#ifndef __CONFIG_H_
#define __CONFIG_H_

#include "octets.h"
#include <map>

class Config : public File
{

public:
	Config(const char *_path);
	~Config();
public:
	Octets GetLine(const Octets &block_name, const Octets &line_name);

private:
	bool Format();

private:
	typedef std::map<Octets, Octets> BlockMap;
	typedef std::map<Octets, BlockMap> ConfigMap
};

#endif
