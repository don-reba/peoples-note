#pragma once

#include "TType.h"

namespace Thrift
{
namespace Protocol
{

class TSet
{
private:

	TType elementType;
	int   count;

public:

	TSet();

	TType GetElementType() const;
	int   GetCount() const;

	void SetElementType (TType value);
	void SetCount       (int   count);
};

} // Protocol
} // Thrift
