#include "stdafx.h"
#include "MockTag.h"

using namespace std;

MockTag::MockTag(wstring name)
	: name (name)
{
}

wstring MockTag::GetName() const
{
	return name;
}
