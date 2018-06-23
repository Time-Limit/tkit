#ifndef _CACHE_REPLACEMENT_
#define _CACHE_REPLATEMENT_

#include "trie.h"

namespace TCORE
{

template<typename VALUE>
struct DefaultGetSize
{
	size_t operator()(const VALUE &value)
	{
		return 1;
	}
};

template<typename QUERY, size_t MAXSIZE = 1024, typename GETSIZE = DefaultGetSize<VALUE> >
class LRU
{
	QUERY query;
};

}

#endif
