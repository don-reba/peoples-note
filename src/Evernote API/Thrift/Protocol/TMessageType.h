#pragma once

namespace Thrift
{
namespace Protocol
{

enum TMessageType
{
	MessageUnknown   = 0,
	MessageCall      = 1,
	MessageReply     = 2,
	MessageException = 3,
	MessageOneway    = 4,
};

} // Protocol
} // Thrift