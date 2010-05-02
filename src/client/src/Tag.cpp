#include "stdafx.h"
#include "Tag.h"

using namespace std;

Tag::Tag(wstring name)
	: name    (name)
	, usn     (-1)
	, isDirty (true)
{
}

wstring Tag::GetName() const
{
	return name;
}

Guid Tag::GetGuid() const
{
	return guid;
}

int Tag::GetUsn() const
{
	return usn;
}

bool Tag::IsDirty() const
{
	return isDirty;
}
