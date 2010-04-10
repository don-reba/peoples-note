#pragma once

#include "TType.h"

namespace Thrift
{
namespace Protocol
{

class TList
{
private:

	TType elementType;
	int   count;

public:

	TList();

	TType GetElementType() const;
	int   GetCount()       const;

	void SetElementType (TType value);
	void SetCount       (int value);
};

} // Protocol
} // Thrift
