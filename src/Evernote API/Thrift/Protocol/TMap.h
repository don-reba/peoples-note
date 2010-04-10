#pragma once
#include "TType.h"

namespace Thrift
{
namespace Protocol
{

class TMap
{
private:

	TType keyType;
	TType valueType;
	int   count;

public:

	TMap();

	TType GetKeyType()   const;
	TType GetValueType() const;
	int   GetCount()     const;

	void SetKeyType   (TType value);
	void SetValueType (TType value);
	void SetCount     (int   value);
};

} // Protocol
} // Thrift
