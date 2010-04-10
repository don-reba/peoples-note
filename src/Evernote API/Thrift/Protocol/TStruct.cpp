#include "stdafx.h"
#include "TStruct.h"

using namespace Thrift;
using namespace Thrift::Protocol;

TStruct::TStruct()
	: name (L"")
{
}

const wchar_t * TStruct::GetName() const
{
	return name.c_str();
}

void TStruct::SetName(const wchar_t * value)
{
	name = value;
}
