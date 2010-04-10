#pragma once

#include "TProtocol.h"
#include "TType.h"

namespace Thrift
{
namespace Protocol
{

class TProtocolUtil
{
public:

	static void Skip(TProtocol & prot, TType type);
};

} // Protocol
} // Thrift
