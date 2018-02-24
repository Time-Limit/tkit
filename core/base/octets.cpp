#include "octets.h"
#include "protocol.h"

Octets::Rep Octets::Rep::null = { 0, 0, 1 };

const OctetsStream::Start  OctetsStream::START;
const OctetsStream::Commit OctetsStream::COMMIT;
const OctetsStream::Revert OctetsStream::REVERT;

OctetsStream& OctetsStream::operator>>(Protocol &d)
{
	return d.Deserialize(*this);
}
