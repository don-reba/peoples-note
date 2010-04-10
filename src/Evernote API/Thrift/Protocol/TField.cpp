#include "stdafx.h"
#include "TField.h"

using namespace Thrift;
using namespace Thrift::Protocol;

TField::TField()
	: name (L"")
	, type (TypeVoid)
	, id   (0)
{
}

const wchar_t * TField::GetName() const
{
	return name.c_str();
}

TType TField::GetType() const
{
	return type;
}

short TField::GetID() const
{
	return id;
}

void TField::SetName(const wchar_t * value)
{
	name = value;
}

void TField::SetType(TType value)
{
	type = value;
}

void TField::SetID(short value)
{
	id = value;
}
