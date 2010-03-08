#include "stdafx.h"
#include "Tag.h"

using namespace std;

Tag::Tag(wstring name)
	: name (name)
{
}

wstring Tag::GetName() const
{
	return name;
}
