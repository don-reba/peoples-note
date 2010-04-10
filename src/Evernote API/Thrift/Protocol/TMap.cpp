#include "stdafx.h"
#include "TMap.h"

using namespace Thrift;
using namespace Thrift::Protocol;

TMap::TMap()
	: keyType   (TypeVoid)
	, valueType (TypeVoid)
	, count     (0)
{
}

TType TMap::GetKeyType() const
{
	return keyType;
}

TType TMap::GetValueType() const
{
	return valueType;
}

int TMap::GetCount() const
{
	return count;
}

void TMap::SetKeyType(TType value)
{
	keyType = value;
}

void TMap::SetValueType(TType value)
{
	valueType = value;
}

void TMap::SetCount(int value)
{
	count = value;
}
