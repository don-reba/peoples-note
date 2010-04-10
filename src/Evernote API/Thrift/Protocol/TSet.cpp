#include "stdafx.h"
#include "TSet.h"

using namespace Thrift;
using namespace Thrift::Protocol;

TSet::TSet()
	: elementType (TypeVoid)
	, count       (0)
{
}

TType TSet::GetElementType() const
{
	return elementType;
}

int TSet::GetCount() const
{
	return count;
}

void TSet::SetElementType(TType value)
{
	elementType = value;
}

void TSet::SetCount(int value)
{
	count = value;
}
