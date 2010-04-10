#include "stdafx.h"
#include "TList.h"

using namespace Thrift;
using namespace Thrift::Protocol;

TList::TList()
	: elementType (TypeVoid)
	, count       (0)
{
}

TType TList::GetElementType() const
{
	return elementType;
}

int TList::GetCount() const
{
	return count;
}

void TList::SetElementType(TType value)
{
	elementType = value;
}

void TList::SetCount(int value)
{
	count = value;
}
