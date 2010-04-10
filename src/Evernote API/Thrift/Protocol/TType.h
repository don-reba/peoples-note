#pragma once

namespace Thrift
{
namespace Protocol
{

enum TType
{
	TypeStop   = 0,
	TypeVoid   = 1,
	TypeBool   = 2,
	TypeByte   = 3,
	TypeDouble = 4,
	TypeI16    = 6,
	TypeI32    = 8,
	TypeI64    = 10,
	TypeString = 11,
	TypeStruct = 12,
	TypeMap    = 13,
	TypeSet    = 14,
	TypeList   = 15,
};

} // Protocol
} // Thrift