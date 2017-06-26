#ifndef _PARSER_H_
#define _PARSER_H_

class Parser
{
public:
	virtual bool Parse(char * const begin, char * const end) const = 0;
};

class Rule
{
public:
	virtual Rule const * Trans(char ** const cur, char * const end, Parser *parser) const = 0;
};

#endif
