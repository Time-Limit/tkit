#ifndef __PROTOCOL_H_
#define __PROTOCOL_H_

#include "exptype.h"

class Protocol
{
public:
	virtual void DataIn(const Octets &data, session_id_t sid);
	virtual void DataOut()
}
#endif
